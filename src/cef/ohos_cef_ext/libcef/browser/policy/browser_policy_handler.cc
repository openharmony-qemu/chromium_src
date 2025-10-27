/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "browser_policy_handler.h"

#include "base/files/file_util.h"
#include "base/no_destructor.h"
#include "base/ohos/sys_info_utils_ext.h"
#include "cef/libcef/browser/chrome/chrome_browser_context.h"
#include "cef/libcef/browser/request_context_impl.h"
#include "cef/libcef/common/app_manager.h"
#include "components/policy/core/common/policy_pref_names.h"
#include "components/policy/core/common/policy_loader_ohos.h"
#include "components/prefs/pref_service.h"
#include "content/public/common/content_switches.h"

namespace policy {

namespace {
constexpr int kApiMinApiVersion = 16;
const char kBrowserPolicyFileName[] = "BrowserEnterprisePolicy.json";

Profile* GetActiveProfile() {
  auto request_context = static_cast<CefRequestContextImpl*>(
      CefAppManager::Get()->GetGlobalRequestContext().get());
  if (!request_context || !request_context->GetBrowserContext()) {
    LOG(ERROR) << "GetActiveProfile failed";
    return nullptr;
  }
  return request_context->GetBrowserContext()->AsProfile();
}
}  // namespace

// static
BrowserPolicyHandler* BrowserPolicyHandler::GetInstance() {
  static base::NoDestructor<BrowserPolicyHandler> instance;
  return instance.get();
}

void BrowserPolicyHandler::MaybeInitFromPersistentPrefs() {
  if (current_version_ != kInvalidPolicyVersion) {
    return;
  }

  if (!GetActiveProfile()) {
    LOG(ERROR) << "BrowserPolicyHandler GetActiveProfile failed";
    return;
  }
  auto prefs = GetActiveProfile()->GetPrefs();
  if (!prefs) {
    LOG(ERROR) << "BrowserPolicyHandler MaybeInitFromPersistentPrefs "
                  "GetPrefs failed";
    return;
  }

  if (!prefs->FindPreference(policy_prefs::kBrowserPolicyVersion)) {
    LOG(ERROR) << "BrowserPolicyHandler initial failed: pref not found";
  }

  current_version_ = prefs->GetInteger(policy_prefs::kBrowserPolicyVersion);
  LOG(INFO) << "BrowserPolicyHandler current_version_: " << current_version_;
}

void BrowserPolicyHandler::InitPolicyFromFile(
    const base::FilePath& cache_path) {
  static bool initialized = false;
  if (initialized) {
    return;
  }
  policy_file_path_ =
      base::FilePath(cache_path.AppendASCII(kBrowserPolicyFileName).value());
  LOG(INFO) << "InitPolicyFromFile policy_file_path_: " << policy_file_path_;

  std::string policy;
  bool succeeded = base::ReadFileToString(policy_file_path_, &policy);
  if (!succeeded) {
    LOG(WARNING) << "BrowserPolicyHandler read file failed";
    if (!base::PathExists(cache_path)) {
      CreateDirectory(cache_path);
    }
    base::WriteFile(policy_file_path_, "{}");
    return;
  }
  LOG(INFO) << "InitPolicyFromFile initial policy: " << policy;
  PolicyLoaderOhos::ParsePolicy(policy, &bundle_);
  initialized = true;
}

void BrowserPolicyHandler::SetPolicyAndNotify(const std::string& policy,
                                              int version) {
  if (base::ohos::ApplicationApiVersion() < kApiMinApiVersion) {
    LOG(ERROR) << "current api version does not support enterprise ";
    return;
  }

  if (!SetPolicy(policy, version)) {
    return;
  }
  for (Observer& observer : observers_) {
    observer.OnPolicyChanged();
  }
}

PolicyBundle BrowserPolicyHandler::GetPolicyBundle() {
  LOG(INFO) << "BrowserPolicyHandler policy bundle gotten";

  return bundle_.Clone();
}

bool BrowserPolicyHandler::SetPolicy(const std::string& policy, int version) {
  MaybeInitFromPersistentPrefs();
  if (current_version_ == kInvalidPolicyVersion) {
    LOG(ERROR) << "BrowserPolicyHandler set policy failed: init failed";
    return false;
  }

  if (version <= current_version_) {
    LOG(WARNING) << "BrowserPolicyHandler set policy failed: version not newer";
    return false;
  }

  if (!GetActiveProfile()) {
    LOG(ERROR) << "BrowserPolicyHandler SetPolicy GetActiveProfile failed";
    return false;
  }

  auto prefs = GetActiveProfile()->GetPrefs();
  if (!prefs) {
    LOG(ERROR) << "BrowserPolicyHandler SetPolicy GetPrefs failed";
    return false;
  }

  PolicyBundle bundle;
  if (!PolicyLoaderOhos::ParsePolicy(policy, &bundle)) {
    LOG(ERROR) << "BrowserPolicyHandler policy json format invalid";
    return false;
  }

  current_version_ = version;
  bundle_ = bundle.Clone();
  LOG(INFO) << "WriteFile policy_file_path_: " << policy_file_path_;
  if (!base::WriteFile(policy_file_path_, policy)) {
    LOG(ERROR) << "BrowserPolicyHandler WriteFile failed";
  }
  prefs->SetInteger(policy_prefs::kBrowserPolicyVersion, version);

  LOG(INFO) << "BrowserPolicyHandler new policy set: " << policy;
  return true;
}

void BrowserPolicyHandler::AddObserver(Observer* observer) {
  if (observers_.HasObserver(observer)) {
    return;
  }

  LOG(INFO) << "BrowserPolicyHandler add new observer";
  observers_.AddObserver(observer);
}

void BrowserPolicyHandler::RemoveObserver(Observer* observer) {
  observers_.RemoveObserver(observer);
}

}  // namespace policy
