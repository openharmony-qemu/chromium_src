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

#include "ui/display/screen_ohos.h"

#include "ui/display/display.h"
#include "ui/display/screen.h"

namespace display {
namespace ohos {

gfx::Rect ScreenOhos::ConvertPixelToDIP(const gfx::Rect& rect_in_pixels) {
  return GetTransform().InverseMapRect(rect_in_pixels).value_or(rect_in_pixels);
}

gfx::Rect ScreenOhos::ConvertPixelToDIP(int64_t display_id,
                                        const gfx::Rect& rect_in_pixels) {
  display::Screen* screen = display::Screen::GetScreen();
  display::Display display;
  bool result = screen->GetDisplayWithDisplayId(display_id, &display);
  if (!result) {
    display = screen->GetPrimaryDisplay();
  }
  return ConvertPixelToDIP(display, rect_in_pixels);
}

gfx::Rect ScreenOhos::ConvertPixelToDIP(display::Display display,
                                        const gfx::Rect& rect_in_pixels) {
  return GetTransform(display)
      .InverseMapRect(rect_in_pixels)
      .value_or(rect_in_pixels);
}

gfx::Rect ScreenOhos::ConvertDipToPixel(display::Display display,
                                        const gfx::Rect& rect_in_dip) {
  return GetTransform(display).MapRect(rect_in_dip);
}

gfx::PointF ScreenOhos::ConvertDipToPixel(display::Display display,
                                          const gfx::PointF& point_f_dip) {
  return GetTransform(display).MapPoint(point_f_dip);
}

gfx::PointF ScreenOhos::ConvertDipToPixel(int64_t display_id,
                                          const gfx::PointF& point_f_dip) {
  display::Screen* screen = display::Screen::GetScreen();
  display::Display display;
  bool result = screen->GetDisplayWithDisplayId(display_id, &display);
  if (!result) {
    display = screen->GetPrimaryDisplay();
  }
  return ConvertDipToPixel(display, point_f_dip);
}

gfx::PointF ScreenOhos::ConvertPixelToDip(display::Display display,
                                          const gfx::PointF& point_f_pixel) {
  return GetTransform(display)
      .InverseMapPoint(point_f_pixel)
      .value_or(point_f_pixel);
}


gfx::Transform ScreenOhos::GetTransform() {
  display::Display display = display::Screen::GetScreen()->GetPrimaryDisplay();
  gfx::Transform transform;
  float scale = display.device_scale_factor();
  transform.Scale(scale, scale);
  return transform;
}

gfx::Transform ScreenOhos::GetTransform(display::Display display) {
  if (!display.is_valid()) {
    LOG(WARNING) << "GetTransform fail, display is invalid";
    return GetTransform();
  }
  gfx::Transform transform;
  float scale = display.device_scale_factor();
  transform.Scale(scale, scale);
  return transform;
}

}  // namespace ohos
}  // namespace display
