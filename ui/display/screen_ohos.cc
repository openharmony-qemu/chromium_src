// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
