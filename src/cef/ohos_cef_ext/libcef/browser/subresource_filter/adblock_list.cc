/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "libcef/browser/subresource_filter/adblock_list.h"

#include "base/logging.h"
#include "base/memory/singleton.h"
#include "base/observer_list.h"
#include "base/path_service.h"
#include "base/strings/string_number_conversions.h"
#include "components/subresource_filter/core/browser/subresource_filter_constants.h"
#include "components/subresource_filter/core/browser/user_subresource_filter_constants.h"
#include "components/subresource_filter/core/common/indexed_ruleset.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "libcef/browser/subresource_filter/adblock_ruleset_manager.h"
#include "libcef/browser/subresource_filter/user_adblock_ruleset_manager.h"

using subresource_filter::AdblockRulesetManager;
using subresource_filter::UserAdblockRulesetManager;

namespace adblock {
long long mAdBlockEasyListVersion = 0;

base::FilePath mOfficialEasyListPath;
base::FilePath mUserEasyListPath;

namespace {

class AdBlockListImpl : public AdBlockList {
 public:
  void AddObserver(Observer* observer) override {
    observers_.AddObserver(observer);
  }
  void RemoveObserver(Observer* observer) override {
    observers_.RemoveObserver(observer);
  }

  void NotifyEasyListToObservers() {
    for (Observer& observer : observers_) {
      observer.OnAdBlockEasyListUpdated();
    }
  }

  static AdBlockListImpl* GetInstance() {
    return base::Singleton<AdBlockListImpl>::get();
  }

 private:
  base::ObserverList<Observer>::Unchecked observers_;
};

}  // namespace

// static
AdBlockList* AdBlockList::GetInstance() {
  return AdBlockListImpl::GetInstance();
}

void UpdateEasyListRulesInIOThread() {
  static_cast<AdBlockListImpl*>(AdBlockList::GetInstance())
      ->NotifyEasyListToObservers();

  std::vector<base::FilePath> easylists;
  easylists.push_back(mOfficialEasyListPath);

  LOG(INFO) << "[AdBlock] Received a request to update easylist rules:"
            << mOfficialEasyListPath.value();

  AdblockRulesetManager::GetInstance()->EasyListFileUpdated(
      easylists, mAdBlockEasyListVersion);
}

void UpdateUserEasyListRulesInIOThread() {
  std::vector<base::FilePath> user_easylists;
  user_easylists.push_back(mUserEasyListPath);

  LOG(INFO) << "[AdBlock] Received a request to update user easylist rules:"
            << mUserEasyListPath.value();
  UserAdblockRulesetManager::GetInstance()->UserEasyListFileUpdated(
      user_easylists);
}

void AdBlockList::UpdateUserEasyListRules(const std::string& path) {
  mUserEasyListPath = base::FilePath(path);

  content::GetIOThreadTaskRunner({})->PostTask(
      FROM_HERE, base::BindOnce(&UpdateUserEasyListRulesInIOThread));
}

void UpdateAdblockEasyListRules(long adBlockEasyListVersion) {
  mAdBlockEasyListVersion = adBlockEasyListVersion;

  content::GetIOThreadTaskRunner({})->PostTask(
      FROM_HERE, base::BindOnce(&UpdateEasyListRulesInIOThread));
}

void UpdateAdblockEasyListRules(base::FilePath path) {
  mOfficialEasyListPath = path;

  content::GetIOThreadTaskRunner({})->PostTask(
      FROM_HERE, base::BindOnce(&UpdateEasyListRulesInIOThread));
}

void TryRemoveAdblockSentinelFile(long adBlockEasyListVersion) {
  base::FilePath app_data_dir;
  base::PathService::Get(base::DIR_CACHE, &app_data_dir);
  base::FilePath sentinel_file_path =
      app_data_dir
          .Append(
              FILE_PATH_LITERAL(::subresource_filter::kTopLevelDirectoryName))
          .Append(FILE_PATH_LITERAL(
              ::subresource_filter::kIndexedRulesetBaseDirectoryName))
          .AppendASCII(base::NumberToString(
              ::subresource_filter::RulesetIndexer::kIndexedFormatVersion))
          .AppendASCII(base::NumberToString(adBlockEasyListVersion))
          .Append(::subresource_filter::kSentinelFileName);

  if (base::PathExists(sentinel_file_path)) {
    if (base::DeleteFile(sentinel_file_path)) {
      LOG(INFO) << "delete sentinel file success";
    } else {
      LOG(INFO) << "delete sentinel file fail";
    }
  }
}

}  // namespace adblock
