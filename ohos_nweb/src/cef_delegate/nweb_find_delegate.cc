/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
 *
 * Based on find_helper.cc originally written by
 * Copyright 2012 The Chromium Authors
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "nweb_find_delegate.h"
#include "cef/include/cef_browser.h"
#include "nweb_find_callback.h"

namespace OHOS::NWeb {

NWebFindDelegate::NWebFindDelegate()
    : listener_(nullptr),
      async_find_started_(false),
      last_match_count_(-1),
      last_active_ordinal_(-1) {}

NWebFindDelegate::~NWebFindDelegate() {}

void NWebFindDelegate::SetListener(std::shared_ptr<NWebFindCallback> listener) {
  listener_ = listener;
}

void NWebFindDelegate::FindAllAsync(CefBrowser* browser,
                                    const std::u16string& search_string) {
  if (browser == nullptr) {
    return;
  }
  // Stop any ongoing asynchronous request.
  browser->GetHost()->StopFinding(false);

  async_find_started_ = true;

  StartNewSession(search_string);

  if (MaybeHandleEmptySearch(browser, search_string)) {
    return;
  }

  browser->GetHost()->Find(search_string, true /* forward*/,
                           false /* matchCase*/, false /* findNext*/,
                           /* newSession*/ true);
}

void NWebFindDelegate::HandleFindReply(int request_id,
                                       int match_count,
                                       int active_ordinal,
                                       bool finished) {
  if (!async_find_started_) {
    return;
  }

  NotifyResults(active_ordinal, match_count, finished);
}

void NWebFindDelegate::FindNext(CefBrowser* browser, bool forward) {
  if (browser == nullptr) {
    return;
  }
  if (!async_find_started_) {
    return;
  }

  if (MaybeHandleEmptySearch(browser, last_search_string_)) {
    return;
  }

  browser->GetHost()->Find(last_search_string_, forward /* forward*/,
                           false /* matchCase*/, true /* findNext*/,
                           /* newSession*/ false);
}

void NWebFindDelegate::ClearMatches(CefBrowser* browser) {
  if (browser == nullptr) {
    return;
  }
  browser->GetHost()->StopFinding(true);

  async_find_started_ = false;
  last_search_string_.clear();
  last_match_count_ = -1;
  last_active_ordinal_ = -1;
}

bool NWebFindDelegate::MaybeHandleEmptySearch(
    CefBrowser* browser,
    const std::u16string& search_string) {
  if (!search_string.empty()) {
    return false;
  }

  browser->GetHost()->StopFinding(true);
  NotifyResults(0, 0, true);
  return true;
}

void NWebFindDelegate::StartNewSession(const std::u16string& search_string) {
  last_search_string_ = search_string;
  last_match_count_ = -1;
  last_active_ordinal_ = -1;
}

void NWebFindDelegate::NotifyResults(int active_ordinal,
                                     int match_count,
                                     bool finished) {
  // Match count or ordinal values set to -1 refer to received replies.
  if (match_count == -1) {
    match_count = last_match_count_;
  } else {
    last_match_count_ = match_count;
  }

  if (active_ordinal == -1) {
    active_ordinal = last_active_ordinal_;
  } else {
    last_active_ordinal_ = active_ordinal;
  }

  // Skip the update if we don't still have a valid ordinal.
  // The next update, final or not, should have this information.
  if (!finished && active_ordinal == -1) {
    return;
  }

  // Safeguard in case of errors to prevent reporting -1 to the API listeners.
  if (match_count == -1) {
    NOTREACHED();
    match_count = 0;
  }

  if (active_ordinal == -1) {
    NOTREACHED();
    active_ordinal = 0;
  }

  // WebView.FindListener active match ordinals are 0-based while WebKit sends
  // 1-based ordinals. Still we can receive 0 ordinal in case of no results.
  active_ordinal = std::max(active_ordinal - 1, 0);

  if (listener_) {
    listener_->OnFindResultReceived(active_ordinal, match_count, finished);
  }
}

}  // namespace OHOS::NWeb
