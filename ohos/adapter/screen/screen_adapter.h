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

  void GetDefaultDisplay(OhosDisplay& ohos_display);
  void GetAllDisplays(std::vector<OhosDisplay>& ohos_displays);
  void RegisterDisplayMonitor();
  void initFontSizeScale();
  void RegisterCallback(DisplayChangeCallback callback);
  void SetFontSizeZoom(float zoom);
  DisplayChangeCallback GetCallback();
  static ScreenAdapter& GetInstance();

 private:
  ScreenAdapter();
  void ConvertDisplay(aki::Value complete_display, OhosDisplay& ohos_display);

  DisplayChangeCallback callback_;
  float font_size_zoom_ = 1.0f;
};

}  // namespace adapter
}  // namespace ohos
#endif  // OHOS_ADAPTER_SCREEN_SCREEN_ADAPTER_H_
