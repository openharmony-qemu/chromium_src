// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
 
#include "ohos/adapter/context/context_adapter.h"
 
#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/device_info/device_info.h"
 
namespace ohos::adapter {
 
ContextAdapter::ContextAdapter() {}
ContextAdapter& ContextAdapter::GetInstance() {
  static ContextAdapter adapter;
  return adapter;
}
 
void UpdateWindowDeviceModeSwitchCB(DeviceMode device_mode) {
  LOGI(
      "UpdateWindowDeviceModeSwitchCB device_mode:%{public}d",
      device_mode);
  ContextAdapter::GetInstance().SetDeviceMode(
      device_mode);
}

uint32_t GetLastActiveWidgetId() {
  return ContextAdapter::GetInstance().GetLastActiveWidgetId();
}

void ContextAdapter::UpdateDisplayBrightnessCB(
    const std::string& display_brightness) {
  if (delegate_ != nullptr) {
    delegate_->UpdateAllDisplays();
  } else {
    LOGW(
        "[HDRVivid] ContextAdapter::UpdateDisplayBrightnessCB delegate_ is "
        "nullptr. This will make SDR part of HDR not change with display "
        "brightness change.");
  }
}

void UpdateDisplayBrightnessCB(const std::string& display_brightness) {
  ohos::adapter::ContextAdapter::GetInstance().UpdateDisplayBrightnessCB(
      display_brightness);
}

void ContextAdapter::Register(Delegate* delegate) {
  delegate_ = delegate;
}

void ContextAdapter::UnRegister() {
  delegate_ = nullptr;
}

JSBIND_GLOBAL() {
  JSBIND_FUNCTION(UpdateWindowDeviceModeSwitchCB);
  JSBIND_FUNCTION(GetLastActiveWidgetId);
  JSBIND_FUNCTION(UpdateDisplayBrightnessCB);
}
}  // namespace ohos::adapter::ContextPathAdapter