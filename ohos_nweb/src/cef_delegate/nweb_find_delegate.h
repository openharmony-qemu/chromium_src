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

#ifndef NWEB_FIND_DELEGATE_H
#define NWEB_FIND_DELEGATE_H
#include <functional>
#include <string>
#include "cef/include/base/cef_macros.h"

class CefBrowser;

namespace OHOS::NWeb {
class NWebFindCallback;

// Handles the ohos find-in-page API requests.
class NWebFindDelegate {
 public:
  class Listener {
   public:
    // Called when receiving a new find-in-page update.
    // This will be triggered when the results of FindAllSync, FindAllAsync and
    // FindNext are available. The value provided in active_ordinal is 0-based.
    virtual void OnFindResultReceived(int active_ordinal,
                                      int match_count,
                                      bool finished) = 0;
    virtual ~Listener() {}
  };

  explicit NWebFindDelegate();
  ~NWebFindDelegate();

  // Asynchronous API.
  void FindAllAsync(CefBrowser* browser, const std::u16string& search_string);
  void HandleFindReply(int request_id,
                       int match_count,
                       int active_ordinal,
                       bool finished);

  // Methods valid in both synchronous and asynchronous modes.
  void FindNext(CefBrowser* browser, bool forward);
  void ClearMatches(CefBrowser* browser);
  void SetListener(std::shared_ptr<NWebFindCallback> listener);

 private:
  void StartNewSession(const std::u16string& search_string);
  bool MaybeHandleEmptySearch(CefBrowser* browser,
                              const std::u16string& search_string);
  void NotifyResults(int active_ordinal, int match_count, bool finished);

  // Listener results are reported to.
  std::shared_ptr<NWebFindCallback> listener_;

  // Used to check the validity of FindNext operations.
  bool async_find_started_;

  // Required by FindNext and the incremental find replies.
  std::u16string last_search_string_;
  int last_match_count_;
  int last_active_ordinal_;
};

}  // namespace OHOS::NWeb

#endif  // NWEB_FIND_DELEGATE_H
