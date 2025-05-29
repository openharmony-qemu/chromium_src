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

struct OhosDisplay {
  int32_t id = -1;
  std::string name;
  bool alive = false;
  OhosDisplayState state = STATE_UNKNOWN;
  int32_t refresh_rate = 0;
  OhosDisplayRotation rotation = ROTATE_0;
  int32_t width = 0;
  int32_t height = 0;
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

  void GetDefaultDisplay(OhosDisplay& ohos_display);
  void GetAllDisplays(std::vector<OhosDisplay>& ohos_displays);
  void RegisterDisplayMonitor();
  void RegisterCallback(DisplayChangeCallback callback);
  void SetFontSizeZoom(float zoom);
  DisplayChangeCallback GetCallback();
  static ScreenAdapter& GetInstance();

 private:
  void ConvertDisplay(aki::Value display, OhosDisplay& ohos_display);

  DisplayChangeCallback callback_;
  float font_size_zoom_ = 1.0f;
};

}  // namespace adapter
}  // namespace ohos
#endif  // OHOS_ADAPTER_SCREEN_SCREEN_ADAPTER_H_
