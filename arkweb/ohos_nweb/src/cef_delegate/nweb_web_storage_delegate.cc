/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "nweb_web_storage_delegate.h"

#include <memory>
#include <thread>
#include <vector>

#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "base/logging.h"
#include "base/synchronization/waitable_event.h"
#include "base/time/time.h"
#include "cef/include/cef_waitable_event.h"
#include "cef/libcef/common/time_util.h"
#include "nweb_web_storage_origin_impl.h"
#include "url/gurl.h"

using namespace OHOS::NWeb;
using base::WaitableEvent;

namespace {
class WebStorageCompletionCallback : public CefCompletionCallback {
 public:
  WebStorageCompletionCallback(std::shared_ptr<WaitableEvent> event,
                               std::shared_ptr<NWebBoolValueCallback> callback)
      : event_(event), callback_(callback) {}
  void OnComplete() override {
    if (event_ != nullptr) {
      event_->Signal();
    }
    if (callback_ != nullptr) {
      callback_->OnReceiveValue(true);
    }
  }

 private:
  std::shared_ptr<WaitableEvent> event_;
  std::shared_ptr<NWebBoolValueCallback> callback_;
  IMPLEMENT_REFCOUNTING(WebStorageCompletionCallback);
};

class GetOriginsCallback : public CefGetOriginsCallback {
 public:
  GetOriginsCallback(
      std::shared_ptr<WaitableEvent> event,
      std::shared_ptr<NWebWebStorageOriginVectorValueCallback> callback)
      : event_(event), callback_(callback) {}
  void OnComplete() override {
    for (size_t i = 0; i < origins_.size(); i++) {
      std::shared_ptr<NWebWebStorageOriginImpl> items =
          std::make_shared<NWebWebStorageOriginImpl>();
      items->SetOrigin(origins_[i]);
      items->SetQuota(quotas_[i]);
      items->SetUsage(usages_[i]);
      results_.push_back(items);
    }
    if (event_ != nullptr) {
      event_->Signal();
    }
    if (callback_ != nullptr) {
      callback_->OnReceiveValue(results_);
    }
  }

  void OnOrigins(std::vector<CefString>& origins) override {
    for (size_t i = 0; i < origins.size(); i++) {
      std::string origin = origins[i].ToString();
      origins_.push_back(origin);
    }
  }

  void OnUsages(std::vector<CefString>& usages) override {
    for (size_t i = 0; i < usages.size(); i++) {
      long usage = atol(usages[i].ToString().c_str());
      usages_.push_back(usage);
    }
  }

  void OnQuotas(std::vector<CefString>& quotas) override {
    for (size_t i = 0; i < quotas.size(); i++) {
      long quota = atol(quotas[i].ToString().c_str());
      quotas_.push_back(quota);
    }
  }

  std::vector<std::shared_ptr<NWebWebStorageOrigin>> GetWebStorageOrigin()
      const {
    return results_;
  }

 private:
  std::shared_ptr<WaitableEvent> event_;
  std::shared_ptr<NWebWebStorageOriginVectorValueCallback> callback_;
  std::vector<std::string> origins_;
  std::vector<long> usages_;
  std::vector<long> quotas_;
  std::vector<std::shared_ptr<NWebWebStorageOrigin>> results_;

  IMPLEMENT_REFCOUNTING(GetOriginsCallback);
};

class GetOriginUsageOrQuotaCallback : public CefGetOriginUsageOrQuotaCallback {
 public:
  GetOriginUsageOrQuotaCallback(std::shared_ptr<WaitableEvent> event,
                                std::shared_ptr<NWebLongValueCallback> callback)
      : event_(event), callback_(callback), nums_(-1) {}
  void OnComplete(int64_t nums) override {
    nums_ = nums;
    if (event_ != nullptr) {
      event_->Signal();
    }
    if (callback_ != nullptr) {
      callback_->OnReceiveValue(nums_);
    }
  }

  long GetUsageOrQuota() const { return nums_; }

 private:
  std::shared_ptr<WaitableEvent> event_;
  std::shared_ptr<NWebLongValueCallback> callback_;
  long nums_;

  IMPLEMENT_REFCOUNTING(GetOriginUsageOrQuotaCallback);
};

// #if BUILDFLAG(ARKWEB_EXT_PASSWORD)
class GetPasswordCallback : public CefGetPasswordCallback {
 public:
  GetPasswordCallback(std::shared_ptr<WaitableEvent> event,
                      std::shared_ptr<NWebStringValueCallback> callback)
      : event_(event), callback_(callback), result_("") {}
  void OnComplete(const CefString& result) override {
    result_ = result;
    if (event_ != nullptr) {
      event_->Signal();
    }
    if (callback_ != nullptr) {
      callback_->OnReceiveValue(result.ToString());
    }
  }

  std::string GetPassword() const { return result_; }

 private:
  std::shared_ptr<WaitableEvent> event_;
  std::shared_ptr<NWebStringValueCallback> callback_;
  std::string result_;

  IMPLEMENT_REFCOUNTING(GetPasswordCallback);
};

class GetSavedPasswordsCallback : public CefGetSavedPasswordsCallback {
 public:
  GetSavedPasswordsCallback(std::shared_ptr<WaitableEvent> event,
                            std::shared_ptr<NWebStringValueCallback> callback)
      : event_(event), callback_(callback), result_("") {}
  void OnComplete(const std::vector<CefString>& url,
                  const std::vector<CefString>& username) override {
    int size = url.size();
    for (int i = 0; i < size; i++) {
      url_.push_back(url[i].ToString());
      username_.push_back(username[i].ToString());
    }

    if (event_ != nullptr) {
      event_->Signal();
    }
    if (callback_ != nullptr) {
      callback_->OnReceiveValue(result_);
    }
  }

  std::string GetPassword() const { return result_; }

  std::vector<std::string> GetPasswordUsername() { return username_; }

  std::vector<std::string> GetPasswordUrl() { return url_; }

 private:
  std::shared_ptr<WaitableEvent> event_;
  std::shared_ptr<NWebStringValueCallback> callback_;
  std::string result_;
  std::vector<std::string> url_;

  std::vector<std::string> username_;
  IMPLEMENT_REFCOUNTING(GetSavedPasswordsCallback);
};
// #endif
}  // namespace

namespace OHOS::NWeb {
CefRefPtr<CefWebStorage> NWebWebStorageDelegate::GetGlobalWebStorage() {
  if (!web_storage_) {
    web_storage_ = CefWebStorage::GetGlobalManager(nullptr);
  }
  return web_storage_;
}

#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
CefRefPtr<CefWebStorage>
NWebWebStorageDelegate::GetGlobalIncognitoWebStorage() {
  if (!incognito_web_storage_) {
    incognito_web_storage_ = CefWebStorage::GetGlobalIncognitoManager(nullptr);
  }
  return incognito_web_storage_;
}
#endif

void NWebWebStorageDelegate::DeleteAllData(bool incognito_mode) {
  CefRefPtr<CefWebStorage> web_storage =
#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
      incognito_mode ? GetGlobalIncognitoWebStorage() : GetGlobalWebStorage();
#else
      GetGlobalWebStorage();
#endif

  if (web_storage == nullptr) {
    return;
  }
  web_storage->DeleteAllData();
}

int NWebWebStorageDelegate::DeleteOrigin(const std::string& origin) {
  GURL gurl = GURL(origin);
  if (gurl.is_empty() || !gurl.is_valid()) {
    return OHOS::NWeb::NWEB_INVALID_ORIGIN;
  }
  CefRefPtr<CefWebStorage> web_storage = GetGlobalWebStorage();
  if (web_storage == nullptr) {
    return NWEB_ERR;
  }
  web_storage->DeleteOrigin(origin);
  return NWEB_OK;
}

void NWebWebStorageDelegate::GetOrigins(
    std::shared_ptr<NWebWebStorageOriginVectorValueCallback> callback) {
  CefRefPtr<CefWebStorage> web_storage = GetGlobalWebStorage();
  if (web_storage == nullptr) {
    return;
  }
  web_storage->GetOrigins(new GetOriginsCallback(nullptr, callback));
}

void NWebWebStorageDelegate::GetOrigins(
    std::vector<std::shared_ptr<NWebWebStorageOrigin>>& origins) {
  CefRefPtr<CefWebStorage> web_storage = GetGlobalWebStorage();
  if (web_storage == nullptr) {
    return;
  }
  std::shared_ptr<base::WaitableEvent> completion =
      std::make_shared<base::WaitableEvent>(
          base::WaitableEvent::ResetPolicy::AUTOMATIC,
          base::WaitableEvent::InitialState::NOT_SIGNALED);
  CefRefPtr<GetOriginsCallback> callback =
      new GetOriginsCallback(completion, nullptr);
  web_storage->GetOrigins(callback);
  completion->Wait();
  origins = callback->GetWebStorageOrigin();
}

void NWebWebStorageDelegate::GetOriginQuota(
    const std::string& origin,
    std::shared_ptr<NWebLongValueCallback> callback) {
  CefRefPtr<CefWebStorage> web_storage = GetGlobalWebStorage();
  if (web_storage == nullptr) {
    return;
  }
  web_storage->GetOriginQuota(
      CefString(origin), new GetOriginUsageOrQuotaCallback(nullptr, callback));
}

long NWebWebStorageDelegate::GetOriginQuota(const std::string& origin) {
  CefRefPtr<CefWebStorage> web_storage = GetGlobalWebStorage();
  if (web_storage == nullptr) {
    return NWEB_ERR;
  }
  GURL gurl = GURL(origin);
  if (gurl.is_empty() || !gurl.is_valid()) {
    return NWEB_INVALID_ORIGIN;
  }
  std::shared_ptr<base::WaitableEvent> completion =
      std::make_shared<base::WaitableEvent>(
          base::WaitableEvent::ResetPolicy::AUTOMATIC,
          base::WaitableEvent::InitialState::NOT_SIGNALED);
  CefRefPtr<GetOriginUsageOrQuotaCallback> callback =
      new GetOriginUsageOrQuotaCallback(completion, nullptr);
  web_storage->GetOriginQuota(CefString(origin), callback);
  completion->Wait();
  return callback->GetUsageOrQuota();
}

void NWebWebStorageDelegate::GetOriginUsage(
    const std::string& origin,
    std::shared_ptr<NWebLongValueCallback> callback) {
  CefRefPtr<CefWebStorage> web_storage = GetGlobalWebStorage();
  if (web_storage == nullptr) {
    return;
  }
  web_storage->GetOriginUsage(
      CefString(origin), new GetOriginUsageOrQuotaCallback(nullptr, callback));
}

long NWebWebStorageDelegate::GetOriginUsage(const std::string& origin) {
  CefRefPtr<CefWebStorage> web_storage = GetGlobalWebStorage();
  if (web_storage == nullptr) {
    return NWEB_ERR;
  }
  GURL gurl = GURL(origin);
  if (gurl.is_empty() || !gurl.is_valid()) {
    return NWEB_INVALID_ORIGIN;
  }
  std::shared_ptr<base::WaitableEvent> completion =
      std::make_shared<base::WaitableEvent>(
          base::WaitableEvent::ResetPolicy::AUTOMATIC,
          base::WaitableEvent::InitialState::NOT_SIGNALED);
  CefRefPtr<GetOriginUsageOrQuotaCallback> callback =
      new GetOriginUsageOrQuotaCallback(completion, nullptr);
  web_storage->GetOriginUsage(CefString(origin), callback);
  completion->Wait();
  return callback->GetUsageOrQuota();
}

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
std::string NWebWebStorageDelegate::GetPassword(const std::string& url,
                                                const std::string& username,
                                                int callback_id) {
  CefRefPtr<CefWebStorage> web_storage = GetGlobalWebStorage();
  if (web_storage == nullptr) {
    return "";
  }

  std::shared_ptr<base::WaitableEvent> completion =
      std::make_shared<base::WaitableEvent>(
          base::WaitableEvent::ResetPolicy::AUTOMATIC,
          base::WaitableEvent::InitialState::NOT_SIGNALED);
  CefRefPtr<GetPasswordCallback> callback =
      new GetPasswordCallback(completion, nullptr);
  web_storage->GetPassword(CefString(url), CefString(username), callback);
  completion->Wait();
  web_storage_extension_callback_->OnGetPassword(
      callback->GetPassword().c_str(), callback_id);
  return callback->GetPassword();
}

void NWebWebStorageDelegate::GetSavedPasswordsInfo(int callback_id) {
  CefRefPtr<CefWebStorage> web_storage = GetGlobalWebStorage();
  if (web_storage == nullptr) {
    return;
  }
  std::shared_ptr<base::WaitableEvent> completion =
      std::make_shared<base::WaitableEvent>(
          base::WaitableEvent::ResetPolicy::AUTOMATIC,
          base::WaitableEvent::InitialState::NOT_SIGNALED);
  CefRefPtr<GetSavedPasswordsCallback> callback =
      new GetSavedPasswordsCallback(completion, nullptr);
  web_storage->GetSavedPasswordsInfo(callback);
  completion->Wait();
  LOG(INFO) << "get saved password callback id: " << callback_id;
  web_storage_extension_callback_->OnGetSavedPasswords(
      callback->GetPasswordUrl(), callback->GetPasswordUsername(), callback_id);
}

void NWebWebStorageDelegate::MigratePasswordsInfo() {
  CefRefPtr<CefWebStorage> web_storage = GetGlobalWebStorage();
  if (web_storage == nullptr) {
    return;
  }
  web_storage->MigratePasswordsInfo();
  LOG(INFO) << "[Autofill] migrate password info.";
}

void NWebWebStorageDelegate::RegisterWebStorageExtensionCallback(
    std::shared_ptr<NWebStorageExtensionCallback>
        web_storage_extension_callback) {
  web_storage_extension_callback_ = web_storage_extension_callback;
}

void NWebWebStorageDelegate::ClearPassword() {
  CefRefPtr<CefWebStorage> web_storage = GetGlobalWebStorage();
  if (web_storage == nullptr) {
    return;
  }
  web_storage->ClearPassword();
}

void NWebWebStorageDelegate::RemovePassword(const std::string& url,
                                            const std::string& username) {
  CefRefPtr<CefWebStorage> web_storage = GetGlobalWebStorage();
  if (web_storage == nullptr) {
    return;
  }
  web_storage->RemovePassword(url, username);
}

void NWebWebStorageDelegate::ModifyPassword(const std::string& url,
                                            const std::string& old_username,
                                            const std::string& new_username,
                                            const std::string& new_password) {
  CefRefPtr<CefWebStorage> web_storage = GetGlobalWebStorage();
  if (web_storage == nullptr) {
    return;
  }
  web_storage->ModifyPassword(url, old_username, new_username, new_password);
}

void NWebWebStorageDelegate::RemovePasswordByUrl(const std::string& url) {
  CefRefPtr<CefWebStorage> web_storage = GetGlobalWebStorage();
  if (web_storage == nullptr) {
    return;
  }
  web_storage->RemovePasswordByUrl(url);
}
#endif  // ARKWEB_EXT_PASSWORD
}  // namespace OHOS::NWeb
