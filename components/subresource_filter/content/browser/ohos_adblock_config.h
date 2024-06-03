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

#ifndef COMPONENTS_SUBRESOURCE_FILTER_CONTENT_BROWSER_OHOS_ADBLOCK_CONFIG_H_
#define COMPONENTS_SUBRESOURCE_FILTER_CONTENT_BROWSER_OHOS_ADBLOCK_CONFIG_H_

#include "base/base_export.h"
#include "base/containers/flat_map.h"
#include "base/memory/raw_ptr.h"
#include "base/no_destructor.h"
#include "base/synchronization/lock.h"
#include "components/prefs/pref_service.h"
#include "third_party/libaddressinput/chromium/trie.h"
#include "url/gurl.h"

namespace OHOS {
namespace adblock {

class AdBlockConfig {
 public:
  static AdBlockConfig* GetInstance();

  void SetAdsBlockRules(const std::string& rulesFiles, const bool replace);

  void AddAdsBlockDisallowList(const std::vector<std::string>& domain_suffixes);

  void AddAdsBlockAllowList(const std::vector<std::string>& domain_suffixes);

  void RemoveAdsBlockDisallowedList(
      const std::vector<std::string>& domainSuffixes);

  void RemoveAdsBlockAllowedList(
      const std::vector<std::string>& domainSuffixes);

  void ClearAdsBlockDisallowedList();

  void ClearAdsBlockAllowedList();

  bool IsAdblockEnabledForUrl(GURL url);

  bool GetUserEasylistReplaceSwitch();

  void ReadFromPrefService();

  void SetPrefService(PrefService* local_state);

  virtual ~AdBlockConfig();

  AdBlockConfig();

 private:
  bool CheckIsInDisallowData(GURL url);

  std::unique_ptr<autofill::Trie<std::string>> AddAdsBlockDisallowListInternal(
      const std::vector<std::string>& domain_suffixes);

  void AddAdsBlockDisallowListDone(
      std::unique_ptr<autofill::Trie<std::string>> disallow_data);

  std::unique_ptr<autofill::Trie<std::string>>
  RemoveAdsBlockDisallowedListInternal(
      const std::vector<std::string>& domain_suffixes);

  void RemoveAdsBlockDisallowedListDone(
      std::unique_ptr<autofill::Trie<std::string>> disallow_data);

  std::unique_ptr<autofill::Trie<std::string>>
  ClearAdsBlockDisallowedListInternal();

  void ClearAdsBlockDisallowedListDone(
      std::unique_ptr<autofill::Trie<std::string>> disallow_data);

  void UpdateDisallowDataOnIOThread(
      std::unique_ptr<autofill::Trie<std::string>> disallow_data);

  bool CheckIsInAllowData(GURL url);

  std::unique_ptr<autofill::Trie<std::string>> AddAdsBlockAllowListInternal(
      const std::vector<std::string>& domain_suffixes);

  void AddAdsBlockAllowListDone(
      std::unique_ptr<autofill::Trie<std::string>> allow_data);

  std::unique_ptr<autofill::Trie<std::string>>
  RemoveAdsBlockAllowedListInternal(
      const std::vector<std::string>& domain_suffixes);

  void RemoveAdsBlockAllowedListDone(
      std::unique_ptr<autofill::Trie<std::string>> disallow_data);

  std::unique_ptr<autofill::Trie<std::string>>
  ClearAdsBlockAllowedListInternal();

  void ClearAdsBlockAllowedListDone(
      std::unique_ptr<autofill::Trie<std::string>> disallow_data);

  void UpdateAllowDataOnIOThread(
      std::unique_ptr<autofill::Trie<std::string>> allow_data);

  std::string rules_files_;
  bool replace_ = false;
  bool is_valid_ = false;

  std::set<std::string> disallow_list_;
  std::set<std::string> allow_list_;

  std::unique_ptr<autofill::Trie<std::string>> disallow_data_;
  std::unique_ptr<autofill::Trie<std::string>> allow_data_;

  PrefService* local_state_ = nullptr;

  base::Lock lock_;
};

}  // namespace adblock
}  // namespace OHOS

#endif  // COMPONENTS_SUBRESOURCE_FILTER_CONTENT_BROWSER_OHOS_ADBLOCK_CONFIG_H_