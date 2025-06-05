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

void ScreenAdapter::GetDefaultDisplay(OhosDisplay& ohos_display) {
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
          ohos::adapter::GetJSFunction("OhosDisplayAdapter.GetDefaultDisplay")) {
    func->Invoke<void>(callback);
    promise.get_future().get();
  }

  return;
}

void ScreenAdapter::GetAllDisplays(std::vector<OhosDisplay>& ohos_displays) {
  std::promise<bool> promise;
  std::function<void(aki::Value, int32_t)> callback =
    [&](aki::Value complete_display, int32_t len) {
    if (!complete_display.IsArray()) {
      LOGE("[Display]GetAllDisplays input param err, displays is not array.");
      promise.set_value(false);
      return;
    }

    for (int32_t i = 0; i < len; i++) {
      OhosDisplay ohos_display;
      ConvertDisplay(complete_display[i], ohos_display);
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
