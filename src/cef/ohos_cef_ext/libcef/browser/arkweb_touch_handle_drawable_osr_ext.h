// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_OSR_TOUCH_HANDLE_DRAWABLE_OSR_EXT_H_
#define CEF_LIBCEF_BROWSER_OSR_TOUCH_HANDLE_DRAWABLE_OSR_EXT_H_

#include "cef/libcef/browser/osr/touch_handle_drawable_osr.h"

class CefTouchHandleDrawableOSR;
class ArkWebCefTouchHandleDrawableOSRExt : public CefTouchHandleDrawableOSR {
 public:
  explicit ArkWebCefTouchHandleDrawableOSRExt(CefRenderWidgetHostViewOSR* rwhv);
  ArkWebCefTouchHandleDrawableOSRExt* AsArkWebCefTouchHandleDrawableOSRExt()
      override {
    return this;
  }
  void SetOrigin(const gfx::PointF& position) override;
  void SetAlpha(float alpha) override;
  gfx::RectF GetVisibleBounds() const override;

#if BUILDFLAG(ARKWEB_MENU)
  void SetEdge(const gfx::PointF& top, const gfx::PointF& bottom) override;
  void UpdateVisiableBounds();
#endif  // BUILDFLAG(ARKWEB_MENU)

#if BUILDFLAG(ARKWEB_MENU)
  // Handle line height
  float edge_height_ = 0.f;
#endif  // BUILDFLAG(ARKWEB_MENU)
};
#endif  // CEF_LIBCEF_BROWSER_OSR_TOUCH_HANDLE_DRAWABLE_OSR_EXT_H_
