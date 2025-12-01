// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_DISPLAY_SCREEN_OHOS_H_
#define UI_DISPLAY_SCREEN_OHOS_H_

#include "ui/display/display_export.h"
#include "ui/display/screen.h"
#include "ui/gfx/geometry/point_f.h"
#include "ui/gfx/geometry/transform.h"

namespace gfx {
class Display;
class Rect;
}  // namespace gfx

namespace display {
namespace ohos {

class DISPLAY_EXPORT ScreenOhos : public Screen {
 public:
  ScreenOhos() : Screen() {}

  ScreenOhos(const ScreenOhos&) = delete;
  ScreenOhos& operator=(const ScreenOhos&) = delete;

  static gfx::Rect ConvertPixelToDIP(const gfx::Rect& rect_in_pixels);
  static gfx::Rect ConvertPixelToDIP(int64_t display_id,
                                     const gfx::Rect& rect_in_pixels);
  static gfx::Rect ConvertPixelToDIP(display::Display display,
                                     const gfx::Rect& rect_in_pixels);
  static gfx::Rect ConvertDipToPixel(display::Display display,
                                     const gfx::Rect& rect_in_dip);

  static gfx::PointF ConvertDipToPixel(int64_t display_id,
                                       const gfx::PointF& point_f_dip);
  static gfx::PointF ConvertDipToPixel(display::Display display,
                                       const gfx::PointF& point_f_dip);
  static gfx::PointF ConvertPixelToDip(display::Display display,
                                       const gfx::PointF& point_f_pixel);

  static gfx::Transform GetTransform();
  static gfx::Transform GetTransform(display::Display display);
};

}  // namespace ohos
}  // namespace display

#endif  // UI_DISPLAY_SCREEN_OHOS_H_
