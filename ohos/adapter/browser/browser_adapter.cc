// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "browser_adapter.h"

#include <js_native_api.h>
#include <js_native_api_types.h>

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/constants.h"

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

uint32_t GetLastActive() {
  return BrowserAdapter::GetInstance().GetLastActiveAcceleratedWidget();
}

void BrowserAdapter::StartNewWindow(const std::string& url) {
  if (new_window_callback_ != nullptr) {
    new_window_callback_(url);
  }
}

JSBIND_GLOBAL() {
  JSBIND_FUNCTION(GetLastActive);
}

}  // namespace ohos::adapter
