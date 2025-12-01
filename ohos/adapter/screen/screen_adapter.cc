// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/screen/screen_adapter.h"

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/logging.h"

namespace ohos {
namespace adapter {

ScreenAdapter::ScreenAdapter() {
  initFontSizeScale();
}

void ScreenAdapter::ConvertDisplay(aki::Value complete_display,
                                   OhosDisplay& ohos_display) {
  aki::Value display = complete_display["baseDisplay"];
  aki::Value avail_area = complete_display["availArea"];

  ohos_display.id = display["id"].As<int32_t>();
  ohos_display.name = display["name"].As<std::string>();
  ohos_display.alive = display["alive"].As<bool>();
  ohos_display.state = display["state"].As<OhosDisplayState>();
  ohos_display.refresh_rate = display["refreshRate"].As<int32_t>();
  ohos_display.rotation = display["rotation"].As<OhosDisplayRotation>();
  ohos_display.width = display["width"].As<int32_t>();
  ohos_display.height = display["height"].As<int32_t>();
  ohos_display.density_dpi = display["densityDPI"].As<float>();
  ohos_display.orientation =
      display["orientation"].As<OhosDisplayOrientation>();
  ohos_display.density_pixels = display["densityPixels"].As<float>();
  ohos_display.scaled_density =
      display["scaledDensity"].As<float>() * font_size_zoom_;
  ohos_display.x_dpi = display["xDPI"].As<float>();
  ohos_display.y_dpi = display["yDPI"].As<float>();

  ohos_display.avail_area.left = avail_area["left"].As<int32_t>();
  ohos_display.avail_area.top = avail_area["top"].As<int32_t>();
  ohos_display.avail_area.width = avail_area["width"].As<int32_t>();
  ohos_display.avail_area.height = avail_area["height"].As<int32_t>();
}

void ScreenAdapter::GetPrimaryDisplay(OhosDisplay& ohos_display) {
  std::promise<bool> promise;
  std::function<void(aki::Value)> callback =
    [&](aki::Value complete_display) {
    if (complete_display["id"].As<int32_t>() == -1) {
      promise.set_value(false);
      return;
    }
    ConvertDisplay(complete_display, ohos_display);
    promise.set_value(true);
  };

  if (auto func =
          ohos::adapter::GetJSFunction("OhosDisplayAdapter.GetPrimaryDisplay")) {
    func->Invoke<void>(callback);
    promise.get_future().get();
  }

  return;
}

void ScreenAdapter::GetAllDisplays(std::vector<OhosDisplay>& ohos_displays) {
  std::promise<bool> promise;
  std::function<void(aki::Value, int32_t)> callback =
    [&](aki::Value complete_displays, int32_t len) {
    if (!complete_displays.IsArray()) {
      LOGE("[Display]GetAllDisplays input param err, displays is not array.");
      promise.set_value(false);
      return;
    }

    for (int32_t i = 0; i < len; i++) {
      OhosDisplay ohos_display;
      ConvertDisplay(complete_displays[i], ohos_display);
      ohos_displays.push_back(ohos_display);
    }

    promise.set_value(true);
  };

  if (auto func =
          ohos::adapter::GetJSFunction("OhosDisplayAdapter.GetALLDisplays")) {
    func->Invoke<void>(callback);
    promise.get_future().get();
  }
}

ScreenAdapter& ScreenAdapter::GetInstance() {
  static ScreenAdapter helper;
  return helper;
}

void ScreenAdapter::RegisterDisplayMonitor() {
  if (auto func =
        ohos::adapter::GetJSFunction("OhosDisplayAdapter.registerDisplayMonitor")) {
    func->Invoke<void>();
  }
}

void ScreenAdapter::initFontSizeScale() {
  if (auto func =
        ohos::adapter::GetJSFunction("OhosDisplayAdapter.getFontSizeScale")) {
    font_size_zoom_ = func->Invoke<float>();
  }
}

void ScreenAdapter::RegisterCallback(
    ScreenAdapter::DisplayChangeCallback callback) {
  callback_ = callback;
}

ScreenAdapter::DisplayChangeCallback ScreenAdapter::GetCallback() {
  return callback_;
}

void ScreenAdapter::RegisterAvailableAreaMonitor() {
  if (auto func =
        ohos::adapter::GetJSFunction("OhosDisplayAdapter.registerAvailableAreaMonitor")) {
    func->Invoke<void>();
  }
}

void ScreenAdapter::RegisterAvailableAreaCallback(
    ScreenAdapter::AvailableAreaChangeCallback callback) {
  available_area_callback_ = callback;
}
 
ScreenAdapter::AvailableAreaChangeCallback ScreenAdapter::GetAvailableAreaCallback() {
  return available_area_callback_;
}

void ScreenAdapter::RegisterAvoidAreaCallback(
    ScreenAdapter::AvoidAreaChangeCallback callback) {
  avoid_area_callback_ = callback;
}
 
ScreenAdapter::AvoidAreaChangeCallback ScreenAdapter::GetAvoidAreaCallback() {
  return avoid_area_callback_;
}

void OnDisplayChangeCallback(const std::string& event, int32_t id) {
  if (ScreenAdapter::GetInstance().GetCallback() != nullptr) {
    ScreenAdapter::GetInstance().GetCallback()(event, id);
  }
}

void OnAvailableAreaChangeCallback(const aki::Value available_area, int32_t display_id) {
  window::WindowRect work_area;
  work_area.left = available_area["left"].As<int32_t>();
  work_area.top = available_area["top"].As<int32_t>();
  work_area.width = available_area["width"].As<int32_t>();
  work_area.height = available_area["height"].As<int32_t>();
  if (ScreenAdapter::GetInstance().GetAvailableAreaCallback() != nullptr) {
    ScreenAdapter::GetInstance().GetAvailableAreaCallback()(work_area, display_id);
  }
}

void OnFontSizeChangeCallback(float zoom) {
  ScreenAdapter::GetInstance().SetFontSizeZoom(zoom);
  if (ScreenAdapter::GetInstance().GetCallback() != nullptr) {
    ScreenAdapter::GetInstance().GetCallback()("change", 0);
  }
}

void ScreenAdapter::SetFontSizeZoom(float zoom) {
  font_size_zoom_ = zoom;
}

void ScreenAdapter::OnAvoidAreaChangeCallback(int32_t status_bar_height) {
  if (status_bar_height_ == status_bar_height) {
    return;
  }
  status_bar_height_ = status_bar_height;
  if (GetAvoidAreaCallback() != nullptr) {
    GetAvoidAreaCallback()(status_bar_height_);
  }
}
 
int32_t ScreenAdapter::GetStatusBarHeight() {
  return status_bar_height_;
}
 
void OnAvoidAreaChangeCallback(int32_t status_bar_height) {
  ScreenAdapter::GetInstance().OnAvoidAreaChangeCallback(status_bar_height);
}

JSBIND_GLOBAL() {
  JSBIND_FUNCTION(OnAvailableAreaChangeCallback);
  JSBIND_FUNCTION(OnDisplayChangeCallback);
  JSBIND_FUNCTION(OnFontSizeChangeCallback);
  JSBIND_FUNCTION(OnAvoidAreaChangeCallback);
}

}  // namespace adapter
}  // namespace ohos
