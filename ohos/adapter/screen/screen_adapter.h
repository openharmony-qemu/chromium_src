// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_SCREEN_SCREEN_ADAPTER_H_
#define OHOS_ADAPTER_SCREEN_SCREEN_ADAPTER_H_

#include <cstdint>
#include <string>
#include <vector>
#include "aki/jsbind.h"
#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/export.h"
#include "ohos/adapter/window/window_common.h"

namespace ohos {
namespace adapter {

enum OhosDisplayOrientation {
  PORTRAIT = 0,
  LANDSCAPE = 1,
  PORTRAIT_INVERTED = 2,
  LANDSCAPE_INVERTED = 3,
};

enum OhosDisplayRotation {
  ROTATE_0 = 0,
  ROTATE_90,
  ROTATE_180,
  ROTATE_270,
  ROTATE_INVALID,
};

enum OhosDisplayState {
  STATE_UNKNOWN = 0,
  STATE_OFF = 1,
  STATE_ON = 2,
  STATE_DOZE = 3,
  STATE_DOZE_SUSPEND = 4,
  STATE_VR = 5,
  STATE_ON_SUSPEND = 6,
};

struct Rect {
 public:
  int32_t left;
  int32_t top;
  int32_t width;
  int32_t height;
};

struct OhosDisplay {
  int32_t id = -1;
  std::string name;
  bool alive = false;
  OhosDisplayState state = STATE_UNKNOWN;
  int32_t refresh_rate = 0;
  OhosDisplayRotation rotation = ROTATE_0;
  int32_t width = 0;
  int32_t height = 0;
  Rect avail_area;
  float density_dpi = 0.0;
  OhosDisplayOrientation orientation = PORTRAIT;
  float density_pixels = 0.0;
  float scaled_density = 0.0;
  float x_dpi = 0.0;
  float y_dpi = 0.0;
};

class ADAPTER_EXPORT_API ScreenAdapter {
 public:
  using DisplayChangeCallback =
      std::function<void(const std::string&, int32_t)>;

  using AvailableAreaChangeCallback =
      std::function<void(window::WindowRect, int32_t)>;

  using AvoidAreaChangeCallback =
      std::function<void(int32_t)>;

  void GetPrimaryDisplay(OhosDisplay& ohos_display);
  void GetAllDisplays(std::vector<OhosDisplay>& ohos_displays);
  int32_t GetStatusBarHeight();
  void RegisterDisplayMonitor();
  void RegisterAvailableAreaMonitor();
  void initFontSizeScale();
  void RegisterCallback(DisplayChangeCallback callback);
  void RegisterAvailableAreaCallback(AvailableAreaChangeCallback callback);
  void RegisterAvoidAreaCallback(AvoidAreaChangeCallback callback);
  void SetFontSizeZoom(float zoom);
  void OnAvoidAreaChangeCallback(int32_t status_bar_height);
  DisplayChangeCallback GetCallback();
  AvailableAreaChangeCallback GetAvailableAreaCallback();
  AvoidAreaChangeCallback GetAvoidAreaCallback();
  static ScreenAdapter& GetInstance();

 private:
  ScreenAdapter();
  void ConvertDisplay(aki::Value complete_display, OhosDisplay& ohos_display);

  DisplayChangeCallback callback_;
  AvailableAreaChangeCallback available_area_callback_;
  AvoidAreaChangeCallback avoid_area_callback_;
  float font_size_zoom_ = 1.0f;
  int32_t status_bar_height_ = 0;
};

}  // namespace adapter
}  // namespace ohos
#endif  // OHOS_ADAPTER_SCREEN_SCREEN_ADAPTER_H_
