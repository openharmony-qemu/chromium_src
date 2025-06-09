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
  void CopyAndFlushBuffer (SkPixmap skia_pixmap,
                           BufferHandle* bufferHandle,
                           OHNativeWindowBuffer* nativeWindowBuffer,
                           int fenceFd);
};
}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OHOS_HOST_OHOS_CANVAS_SURFACE_H_
