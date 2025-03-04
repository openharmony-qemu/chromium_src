// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_OHOS_HOST_OHOS_CANVAS_SURFACE_H_
#define UI_OZONE_PLATFORM_OHOS_HOST_OHOS_CANVAS_SURFACE_H_

#include "base/files/file_util.h"
#include "base/task/thread_pool.h"
#include "include/core/SkSurface.h"
#include "native_window/external_window.h"
#include "ui/gfx/codec/png_codec.h"
#include "ui/gl/gl_surface.h"
#include "ui/ozone/public/gl_ozone.h"
#include "ui/ozone/public/surface_factory_ozone.h"
#include "ui/ozone/public/surface_ozone_canvas.h"

namespace ui {
class OhosCanvasSurface : public ui::SurfaceOzoneCanvas {
 public:
  explicit OhosCanvasSurface(gfx::AcceleratedWidget widget) : widget_(widget) {}
  ~OhosCanvasSurface() override {}

  void ResizeCanvas(const gfx::Size& viewport_size, float scale) override;
  SkCanvas* GetCanvas() override;
  void PresentCanvas(const gfx::Rect& damage) override;
  std::unique_ptr<gfx::VSyncProvider> CreateVSyncProvider() override;

 private:
  sk_sp<SkSurface> surface_;
  gfx::AcceleratedWidget widget_;
  OHNativeWindow* window_;
  uint32_t width_;
  uint32_t height_;
  void CopyAndFlushBuffer(SkPixmap skia_pixmap,
                          BufferHandle* bufferHandle,
                          OHNativeWindowBuffer* nativeWindowBuffer,
                          int fenceFd);
};
}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OHOS_HOST_OHOS_CANVAS_SURFACE_H_
