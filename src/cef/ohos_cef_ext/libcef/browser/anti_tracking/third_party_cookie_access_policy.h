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

#ifndef CEF_LIBCEF_BROWSER_ANTI_TRACKING_THIRD_PARTY_COOKIE_ACCESS_POLICY_H
#define CEF_LIBCEF_BROWSER_ANTI_TRACKING_THIRD_PARTY_COOKIE_ACCESS_POLICY_H

#include "base/files/file_path.h"
#include "base/lazy_instance.h"
#include "base/values.h"
#include "content/public/browser/browser_context.h"
#include "services/network/public/cpp/resource_request.h"
#include "third_party/libaddressinput/chromium/trie.h"

namespace ohos_anti_tracking {
class ThirdPartyCookieAccessPolicy {
 public:
  static ThirdPartyCookieAccessPolicy* GetInstance();

  bool AllowGetCookies(const network::ResourceRequest& request,
                       const GURL& main_frame_host);
  void SetTBWFilePath(const base::FilePath& tbw_path);

  void AddITPBypassingList(const std::vector<std::string>& host_list);
  void RemoveITPBypassingList(const std::vector<std::string>& host_list);
  void ClearITPBypassingList();
  void EnableIntelligentTrackingPrevention(
      content::BrowserContext* browser_context,
      bool enable);

 private:
  friend struct base::LazyInstanceTraitsBase<ThirdPartyCookieAccessPolicy>;

  ThirdPartyCookieAccessPolicy();
  ~ThirdPartyCookieAccessPolicy();

  bool IsHostInITPBypassingList(const std::string& host);
  void OnTBWDataReadDone(std::unique_ptr<autofill::Trie<std::string>> tbw_data);
  void UpdateTBWOnIOThread(
      std::unique_ptr<autofill::Trie<std::string>> tbw_data);
  bool CheckIsInTBW(const std::string& host);
  void AddITPBypassingListOnIOThread(const std::vector<std::string>& host_list);
  void RemoveITPBypassingListOnIOThread(
      const std::vector<std::string>& host_list);
  void ClearITPBypassingListOnIOThread();

  base::FilePath tbw_path_;

  // tbw_data_ only can be visited on the io thread
  std::unique_ptr<autofill::Trie<std::string>> tbw_data_;

  // bypassing_host_list_ can be visited on the io thread
  std::set<std::string> bypassing_host_list_;
};

}  // namespace ohos_anti_tracking

#endif  // CEF_LIBCEF_BROWSER_ANTI_TRACKING_THIRD_PARTY_COOKIE_ACCESS_POLICY_H
