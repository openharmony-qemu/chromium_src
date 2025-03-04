// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/screen/screen_adapter.h"

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/logging.h"

namespace ohos {
namespace adapter {
void ScreenAdapter::ConvertDisplay(aki::Value display,
                                   OhosDisplay& ohos_display) {
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
      display["scaledDensity"].As<float>() * ScreenAdapter::font_size_zoom_;

  ohos_display.x_dpi = display["xDPI"].As<float>();
  ohos_display.y_dpi = display["yDPI"].As<float>();
}

void ScreenAdapter::GetDefaultDisplay(OhosDisplay& ohos_display) {
  std::function<void(aki::Value)> callback = [&](aki::Value display) {
    ConvertDisplay(display, ohos_display);
  };

  if (auto func = ohos::adapter::GetJSFunction(
          "OhosDisplayAdapter.GetDefaultDisplay")) {
    func->Invoke<void>(callback);
  }

  return;
}

void ScreenAdapter::GetAllDisplays(std::vector<OhosDisplay>& ohos_displays) {
  std::promise<bool> promise;
  std::function<void(aki::Value, int32_t)> callback = [&](aki::Value displays,
                                                          int32_t len) {
    if (!displays.IsArray()) {
      LOGE("[Display]GetAllDisplays input param err, displays is not array.");
      promise.set_value(false);
      return;
    }

    for (int32_t i = 0; i < len; i++) {
      OhosDisplay ohos_display;
      ConvertDisplay(displays[i], ohos_display);
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
  if (auto func = ohos::adapter::GetJSFunction(
          "OhosDisplayAdapter.registerDisplayMonitor")) {
    func->Invoke<void>();
  }
}

void ScreenAdapter::RegisterCallback(
    ScreenAdapter::DisplayChangeCallback callback) {
  callback_ = callback;
}

ScreenAdapter::DisplayChangeCallback ScreenAdapter::GetCallback() {
  return callback_;
}

void OnDisplayChangeCallback(const std::string& event, int32_t id) {
  if (ScreenAdapter::GetInstance().GetCallback() != nullptr) {
    ScreenAdapter::GetInstance().GetCallback()(event, id);
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

JSBIND_GLOBAL() {
  JSBIND_FUNCTION(OnDisplayChangeCallback);
  JSBIND_FUNCTION(OnFontSizeChangeCallback);
}

}  // namespace adapter
}  // namespace ohos
