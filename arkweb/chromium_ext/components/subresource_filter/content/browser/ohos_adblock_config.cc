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

#include "components/subresource_filter/content/browser/ohos_adblock_config.h"

#include "base/logging.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "base/values.h"
#include "net/base/registry_controlled_domains/registry_controlled_domain.h"

#include "base/functional/bind.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/memory/singleton.h"
#include "base/lazy_instance.h"
#include "base/task/thread_pool.h"
#include "base/values.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "net/base/registry_controlled_domains/registry_controlled_domain.h"
#include "services/network/public/cpp/resource_request.h"
#include "components/subresource_filter/core/browser/user_ruleset_version.h"
#include "components/subresource_filter/core/common/constants.h"

namespace OHOS {
namespace adblock {

namespace {

std::vector<uint8_t> ToByteArray(const std::string& text) {
  std::vector<uint8_t> result(text.length() + 1, 0);
  result.assign(text.begin(), text.end());
  return result;
}

bool CheckIsInResult(const std::set<std::string>& result,
                     const std::string& schemeAndHostName) {
  if (result.find(schemeAndHostName) != result.end()) {
    LOG(DEBUG) << "[adblock] CheckIsInResult:true";
    return true;
  }

  std::size_t found_dot = schemeAndHostName.find_first_of('.');
  std::size_t found_slash = schemeAndHostName.find_first_of('/');
  std::size_t found = std::string::npos;

  if (found_dot != std::string::npos && found_slash != std::string::npos) {
    found = std::min(found_dot, found_slash);
    return CheckIsInResult(result, schemeAndHostName.substr(found + 1));
  } else if (found_dot != std::string::npos) {
    found = found_dot;
    return CheckIsInResult(result, schemeAndHostName.substr(found + 1));
  } else if (found_slash != std::string::npos) {
    found = found_slash;
    return CheckIsInResult(result, schemeAndHostName.substr(found + 1));
  }

  LOG(DEBUG) << "[adblock] CheckIsInResult:false";
  return false;
}

}  // namespace

// static
AdBlockConfig* AdBlockConfig::GetInstance() {
  return base::Singleton<AdBlockConfig>::get();
}

void AdBlockConfig::SetAdsBlockRules(const std::string& rulesFiles,
                                     const bool replace) {
  {
    base::AutoLock lock(lock_);
    rules_files_ = rulesFiles;
    replace_ = replace;
    subresource_filter::UserIndexedRulesetVersion info(subresource_filter::kSafeBrowsingUserRulesetConfig.filter_tag);
    if (!local_state_) {
      LOG(ERROR) << "[Adblock] web core has not been initialized";
      return;
    }

    info.SaveUserEasylistInfoToPrefs(local_state_, rules_files_, replace_);
    is_valid_ = false;
  }
}

AdBlockConfig::AdBlockConfig() {}

AdBlockConfig::~AdBlockConfig() {
  disallow_data_ = nullptr;
  allow_data_ = nullptr;
  local_state_ = nullptr;
}

bool AdBlockConfig::GetUserEasylistReplaceSwitch() {
  base::AutoLock lock(lock_);
  return replace_;
}

void AdBlockConfig::ReadFromPrefService() {
  {
    base::AutoLock lock(lock_);
    if (is_valid_) {
      return;
    }
    subresource_filter::UserIndexedRulesetVersion info(subresource_filter::kSafeBrowsingUserRulesetConfig.filter_tag);
    if (!local_state_) {
      LOG(ERROR) << "[Adblock] web core has not been initialized";
      return;
    }

    info.ReadUserEasylistInfoFromPrefs(local_state_);
    replace_ = info.user_easylist_replace;
    rules_files_ = info.user_easylist_path;
    is_valid_ = true;
  }
}

void AdBlockConfig::SetPrefService(PrefService* local_state) {
  local_state_ = local_state;
}

bool AdBlockConfig::CheckIsInDisallowData(GURL url) {
  if (url.spec().empty()) {
    return false;
  }

  std::set<std::string> result;

  {
    base::AutoLock lock(lock_);
    if (!disallow_data_ || disallow_data_->empty()) {
      return false;
    }
  }

  std::string registrable_domain =
      net::registry_controlled_domains::GetDomainAndRegistry(
          url.host(),
          net::registry_controlled_domains::INCLUDE_PRIVATE_REGISTRIES);

  std::vector<uint8_t> key = ToByteArray(registrable_domain);
  LOG(DEBUG) << "[Adblock] registrable key:" << registrable_domain;
  std::reverse(std::begin(key), std::end(key));

  {
    base::AutoLock lock(lock_);
    disallow_data_->FindDataForKeyPrefix(key, &result);
  }

  for (auto item : result) {
    LOG(DEBUG) << "[Adblock] result item:" << item;
  }

  return CheckIsInResult(result, url.scheme() + "://" + url.host());
}

std::unique_ptr<autofill::Trie<std::string>>
AdBlockConfig::AddAdsBlockDisallowListInternal(
    const std::vector<std::string>& domain_suffixes) {
  std::unique_ptr<autofill::Trie<std::string>> disallow_data =
      std::make_unique<autofill::Trie<std::string>>();

  base::AutoLock lock(lock_);
  for (auto& item : domain_suffixes) {
    disallow_list_.insert(item);
  }

  for (auto& item : disallow_list_) {
    std::string trimed_str;
    base::TrimWhitespaceASCII(item, base::TRIM_ALL, &trimed_str);
    LOG(DEBUG) << "[Adblock] trimed key:" << trimed_str;
    std::vector<uint8_t> key = ToByteArray(trimed_str);
    std::reverse(std::begin(key), std::end(key));
    disallow_data->AddDataForKey(key, trimed_str);
  }

  return disallow_data;
}

void AdBlockConfig::AddAdsBlockDisallowListDone(
    std::unique_ptr<autofill::Trie<std::string>> disallow_data) {
  content::GetIOThreadTaskRunner({})->PostTask(
      FROM_HERE,
      base::BindOnce(&AdBlockConfig::UpdateDisallowDataOnIOThread,
                     base::Unretained(this), std::move(disallow_data)));
}

void AdBlockConfig::UpdateDisallowDataOnIOThread(
    std::unique_ptr<autofill::Trie<std::string>> disallow_data) {
  if (!disallow_data) {
    return;
  }

  {
    base::AutoLock lock(lock_);
    disallow_data_ = std::move(disallow_data);
  }

  LOG(INFO) << "update adblock disallow data on io thread done";
}

void AdBlockConfig::AddAdsBlockDisallowList(
    const std::vector<std::string>& domain_suffixes) {
  base::ThreadPool::PostTaskAndReplyWithResult(
      FROM_HERE, {base::MayBlock(), base::TaskPriority::BEST_EFFORT},
      base::BindOnce(&AdBlockConfig::AddAdsBlockDisallowListInternal,
                     base::Unretained(this), domain_suffixes),
      base::BindOnce(&AdBlockConfig::AddAdsBlockDisallowListDone,
                     base::Unretained(this)));
}

std::unique_ptr<autofill::Trie<std::string>>
AdBlockConfig::RemoveAdsBlockDisallowedListInternal(
    const std::vector<std::string>& domain_suffixes) {
  std::unique_ptr<autofill::Trie<std::string>> disallow_data =
      std::make_unique<autofill::Trie<std::string>>();

  base::AutoLock lock(lock_);
  for (auto& item : domain_suffixes) {
    disallow_list_.erase(item);
  }

  for (auto& item : disallow_list_) {
    std::string trimed_str;
    base::TrimWhitespaceASCII(item, base::TRIM_ALL, &trimed_str);
    LOG(DEBUG) << "[Adblock] trimed key:" << trimed_str;
    std::vector<uint8_t> key = ToByteArray(trimed_str);
    std::reverse(std::begin(key), std::end(key));
    disallow_data->AddDataForKey(key, trimed_str);
  }

  return disallow_data;
}

void AdBlockConfig::RemoveAdsBlockDisallowedListDone(
    std::unique_ptr<autofill::Trie<std::string>> disallow_data) {
  content::GetIOThreadTaskRunner({})->PostTask(
      FROM_HERE,
      base::BindOnce(&AdBlockConfig::UpdateDisallowDataOnIOThread,
                     base::Unretained(this), std::move(disallow_data)));
}

void AdBlockConfig::RemoveAdsBlockDisallowedList(
    const std::vector<std::string>& domain_suffixes) {
  base::ThreadPool::PostTaskAndReplyWithResult(
      FROM_HERE, {base::MayBlock(), base::TaskPriority::BEST_EFFORT},
      base::BindOnce(&AdBlockConfig::RemoveAdsBlockDisallowedListInternal,
                     base::Unretained(this), domain_suffixes),
      base::BindOnce(&AdBlockConfig::RemoveAdsBlockDisallowedListDone,
                     base::Unretained(this)));
}

std::unique_ptr<autofill::Trie<std::string>>
AdBlockConfig::ClearAdsBlockDisallowedListInternal() {
  std::unique_ptr<autofill::Trie<std::string>> disallow_data =
      std::make_unique<autofill::Trie<std::string>>();

  base::AutoLock lock(lock_);
  disallow_list_.clear();

  return disallow_data;
}

void AdBlockConfig::ClearAdsBlockDisallowedListDone(
    std::unique_ptr<autofill::Trie<std::string>> disallow_data) {
  content::GetIOThreadTaskRunner({})->PostTask(
      FROM_HERE,
      base::BindOnce(&AdBlockConfig::UpdateDisallowDataOnIOThread,
                     base::Unretained(this), std::move(disallow_data)));
}

void AdBlockConfig::ClearAdsBlockDisallowedList() {
  base::ThreadPool::PostTaskAndReplyWithResult(
      FROM_HERE, {base::MayBlock(), base::TaskPriority::BEST_EFFORT},
      base::BindOnce(&AdBlockConfig::ClearAdsBlockDisallowedListInternal,
                     base::Unretained(this)),
      base::BindOnce(&AdBlockConfig::ClearAdsBlockDisallowedListDone,
                     base::Unretained(this)));
}

bool AdBlockConfig::CheckIsInAllowData(GURL url) {
  if (url.spec().empty()) {
    return false;
  }

  std::set<std::string> result;

  {
    base::AutoLock lock(lock_);
    if (!allow_data_ || allow_data_->empty()) {
      return false;
    }
  }

  std::string registrable_domain =
      net::registry_controlled_domains::GetDomainAndRegistry(
          url.host(),
          net::registry_controlled_domains::INCLUDE_PRIVATE_REGISTRIES);

  std::vector<uint8_t> key = ToByteArray(registrable_domain);
  std::reverse(std::begin(key), std::end(key));

  {
    base::AutoLock lock(lock_);
    allow_data_->FindDataForKeyPrefix(key, &result);
  }

  for (auto item : result) {
    LOG(DEBUG) << "[Adblock] result item:" << item;
  }

  return CheckIsInResult(result, url.scheme() + "://" + url.host());
}

std::unique_ptr<autofill::Trie<std::string>>
AdBlockConfig::AddAdsBlockAllowListInternal(
    const std::vector<std::string>& domain_suffixes) {
  std::unique_ptr<autofill::Trie<std::string>> allow_data =
      std::make_unique<autofill::Trie<std::string>>();

  base::AutoLock lock(lock_);
  for (auto& item : domain_suffixes) {
    allow_list_.insert(item);
  }

  for (auto& item : allow_list_) {
    std::string trimed_str;
    base::TrimWhitespaceASCII(item, base::TRIM_ALL, &trimed_str);
    std::vector<uint8_t> key = ToByteArray(trimed_str);
    std::reverse(std::begin(key), std::end(key));
    allow_data->AddDataForKey(key, trimed_str);
  }

  return allow_data;
}

void AdBlockConfig::AddAdsBlockAllowListDone(
    std::unique_ptr<autofill::Trie<std::string>> allow_data) {
  content::GetIOThreadTaskRunner({})->PostTask(
      FROM_HERE, base::BindOnce(&AdBlockConfig::UpdateAllowDataOnIOThread,
                                base::Unretained(this), std::move(allow_data)));
}

void AdBlockConfig::UpdateAllowDataOnIOThread(
    std::unique_ptr<autofill::Trie<std::string>> allow_data) {
  if (!allow_data) {
    return;
  }

  {
    base::AutoLock lock(lock_);
    allow_data_ = std::move(allow_data);
  }
  LOG(INFO) << "update adblock allow data on io thread done";
}

void AdBlockConfig::AddAdsBlockAllowList(
    const std::vector<std::string>& domain_suffixes) {
  base::ThreadPool::PostTaskAndReplyWithResult(
      FROM_HERE, {base::MayBlock(), base::TaskPriority::BEST_EFFORT},
      base::BindOnce(&AdBlockConfig::AddAdsBlockAllowListInternal,
                     base::Unretained(this), domain_suffixes),
      base::BindOnce(&AdBlockConfig::AddAdsBlockAllowListDone,
                     base::Unretained(this)));
}

std::unique_ptr<autofill::Trie<std::string>>
AdBlockConfig::RemoveAdsBlockAllowedListInternal(
    const std::vector<std::string>& domain_suffixes) {
  std::unique_ptr<autofill::Trie<std::string>> allow_data =
      std::make_unique<autofill::Trie<std::string>>();

  base::AutoLock lock(lock_);
  for (auto& item : domain_suffixes) {
    allow_list_.erase(item);
  }

  for (auto& item : allow_list_) {
    std::string trimed_str;
    base::TrimWhitespaceASCII(item, base::TRIM_ALL, &trimed_str);
    LOG(DEBUG) << "[Adblock] trimed key:" << trimed_str;
    std::vector<uint8_t> key = ToByteArray(trimed_str);
    std::reverse(std::begin(key), std::end(key));
    allow_data->AddDataForKey(key, trimed_str);
  }

  return allow_data;
}

void AdBlockConfig::RemoveAdsBlockAllowedListDone(
    std::unique_ptr<autofill::Trie<std::string>> allow_data) {
  content::GetIOThreadTaskRunner({})->PostTask(
      FROM_HERE, base::BindOnce(&AdBlockConfig::UpdateAllowDataOnIOThread,
                                base::Unretained(this), std::move(allow_data)));
}

void AdBlockConfig::RemoveAdsBlockAllowedList(
    const std::vector<std::string>& domain_suffixes) {
  base::ThreadPool::PostTaskAndReplyWithResult(
      FROM_HERE, {base::MayBlock(), base::TaskPriority::BEST_EFFORT},
      base::BindOnce(&AdBlockConfig::RemoveAdsBlockAllowedListInternal,
                     base::Unretained(this), domain_suffixes),
      base::BindOnce(&AdBlockConfig::RemoveAdsBlockAllowedListDone,
                     base::Unretained(this)));
}

std::unique_ptr<autofill::Trie<std::string>>
AdBlockConfig::ClearAdsBlockAllowedListInternal() {
  std::unique_ptr<autofill::Trie<std::string>> allow_data =
      std::make_unique<autofill::Trie<std::string>>();

  base::AutoLock lock(lock_);
  allow_list_.clear();

  return allow_data;
}

void AdBlockConfig::ClearAdsBlockAllowedListDone(
    std::unique_ptr<autofill::Trie<std::string>> allow_data) {
  content::GetIOThreadTaskRunner({})->PostTask(
      FROM_HERE, base::BindOnce(&AdBlockConfig::UpdateAllowDataOnIOThread,
                                base::Unretained(this), std::move(allow_data)));
}

void AdBlockConfig::ClearAdsBlockAllowedList() {
  base::ThreadPool::PostTaskAndReplyWithResult(
      FROM_HERE, {base::MayBlock(), base::TaskPriority::BEST_EFFORT},
      base::BindOnce(&AdBlockConfig::ClearAdsBlockAllowedListInternal,
                     base::Unretained(this)),
      base::BindOnce(&AdBlockConfig::ClearAdsBlockAllowedListDone,
                     base::Unretained(this)));
}

bool AdBlockConfig::IsAdblockEnabledForUrl(GURL url) {
  if (!url.is_valid() || url.spec().empty()) {
    return false;
  }

  if (!CheckIsInAllowData(url) && CheckIsInDisallowData(url)) {
    return false;
  }

  return true;
}

}  // namespace adblock
}  // namespace OHOS
