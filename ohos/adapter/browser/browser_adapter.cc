/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ohos/adapter/browser/browser_adapter.h"

#include <js_native_api.h>
#include <js_native_api_types.h>

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/constants.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"

namespace ohos::adapter {

BrowserAdapter& BrowserAdapter::GetInstance() {
  static BrowserAdapter helper;
  return helper;
}

void BrowserAdapter::RegisterNewWindowCallback(NewWindowCallback callback) {
  if (callback != nullptr) {
    new_window_callback_ = callback;
  }
}

void BrowserAdapter::RegisterGetLastActiveCallback(
    GetLastActiveCallback callback) {
  if (callback != nullptr) {
    get_last_active_callback_ = callback;
  }
}

void BrowserAdapter::UnRegisterGetLastActiveCallback() {
  get_last_active_callback_ = nullptr;
}

uint32_t BrowserAdapter::GetLastActiveAcceleratedWidget() {
  if (get_last_active_callback_ != nullptr) {
    return get_last_active_callback_();
  }
  return 0;
}

uint32_t GetLastActiveWidgetId() {
  return BrowserAdapter::GetInstance().GetLastActiveAcceleratedWidget();
}

uint32_t AllocateWidgetId() {
  return xcomponent::WindowAdapter::GetInstance().NextWindowWidgetId();
}

void BrowserAdapter::StartNewWindow(const std::string& url, bool force_open) {
  if (new_window_callback_ != nullptr) {
    new_window_callback_(url, force_open);
  }
}

BrowserCloseResponse BrowserAdapter::GetBrowserCloseResponse(int32_t id) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (close_responses_.contains(id)) {
    return close_responses_[id];
  } else {
    return BrowserCloseResponse::kUndetermined;
  }
}

BrowserCloseResponse GetBrowserCloseResponse(int32_t id) {
  return BrowserAdapter::GetInstance().GetBrowserCloseResponse(id);
}

void BrowserAdapter::SetBrowserCloseResponse(int32_t id,
                                             BrowserCloseResponse response) {
  std::lock_guard<std::mutex> lock(mutex_);
  close_responses_[id] = response;
}

JSBIND_GLOBAL() {
  JSBIND_FUNCTION(GetLastActiveWidgetId);
  JSBIND_FUNCTION(AllocateWidgetId);
  JSBIND_FUNCTION(GetBrowserCloseResponse);
}

}  // namespace ohos::adapter
