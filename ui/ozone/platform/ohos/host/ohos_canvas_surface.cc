// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/ozone/platform/ohos/host/ohos_canvas_surface.h"

#include <ace/xcomponent/native_interface_xcomponent.h>
#include <napi/native_api.h>
#include <native_buffer/native_buffer.h>
#include <native_image/native_image.h>
#include <native_window/external_window.h>
#include <sys/mman.h>

#include <cstddef>
#include <cstdint>

#include "base/task/thread_pool.h"
#include "include/core/SkSurface.h"
#include "skia/ext/legacy_display_globals.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkImageInfo.h"
#include "third_party/skia/include/core/SkPixmap.h"
#include "third_party/skia/include/core/SkRefCnt.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "third_party/skia/include/core/SkSurfaceProps.h"
#include "ui/gfx/codec/png_codec.h"
#include "ui/gl/gl_surface.h"
#include "ui/ozone/platform/ohos/common/ohos_util.h"
#include "ui/ozone/public/gl_ozone.h"
#include "ui/ozone/public/surface_ozone_canvas.h"

namespace ui {
void OhosCanvasSurface::ResizeCanvas(const gfx::Size& viewport_size,
                                     float scale) {
  width_ = viewport_size.width();
  height_ = viewport_size.height();

  SkImageInfo info = SkImageInfo::Make(width_, height_, kRGBA_8888_SkColorType,
                                       kOpaque_SkAlphaType);
  SkSurfaceProps props = skia::LegacyDisplayGlobals::GetSkSurfaceProps();
  surface_ = SkSurfaces::Raster(info, &props);
}

SkCanvas* OhosCanvasSurface::GetCanvas() {
  return surface_->getCanvas();
}

void OhosCanvasSurface::PresentCanvas(const gfx::Rect& damage) {
  if ((width_ == 1 && height_ == 1) || width_ <= 0 || height_ <= 0) {
    return;
  }

  window_ = (OHNativeWindow*)util::GetWindowFromWidget(widget_);
  if (window_ == nullptr) {
    LOG(ERROR) << "OhosCanvasSurface::PresentCanvas window null";
    return;
  }

  SkPixmap skia_pixmap;
  if (surface_) {
    surface_->peekPixels(&skia_pixmap);
  }
  if (!skia_pixmap.addr()) {
    LOG(ERROR) << "skia_pixmap.addr() is null";
    return;
  }

  int32_t code = SET_BUFFER_GEOMETRY;
  int ret = OH_NativeWindow_NativeWindowHandleOpt((OHNativeWindow*)window_,
                                                  code, width_, height_);

  OHNativeWindowBuffer* nativeWindowBuffer = nullptr;
  int fenceFd = -1;
  ret = OH_NativeWindow_NativeWindowRequestBuffer(
      (OHNativeWindow*)window_, &nativeWindowBuffer, &fenceFd);
  if (ret != 0) {
    LOG(ERROR) << "OH_NativeWindow_NativeWindowRequestBuffer fail ret=" << ret;
    return;
  }
  if (!nativeWindowBuffer) {
    LOG(ERROR) << "OhosCanvasSurface::PresentCanvas nativeWindowBuffer is null";
    return;
  }
  OH_NativeWindow_NativeObjectReference(nativeWindowBuffer);
  uint32_t width = UINT32_MAX;
  uint32_t height = UINT32_MAX;
  code = GET_BUFFER_GEOMETRY;
  ret = OH_NativeWindow_NativeWindowHandleOpt((OHNativeWindow*)window_, code,
                                              &height, &width);
  if (ret != 0) {
    LOG(ERROR) << "OH_NativeWindow_NativeWindowHandleOpt fail ret=" << ret;
    OH_NativeWindow_NativeObjectUnreference(nativeWindowBuffer);
    return;
  }
  width = std::min(width, width_);
  height = std::min(height, height_);

  BufferHandle* bufferHandle =
      OH_NativeWindow_GetBufferHandleFromNative(nativeWindowBuffer);
  auto* mappedAddr =
      mmap(bufferHandle->virAddr, bufferHandle->size, PROT_READ | PROT_WRITE,
           MAP_SHARED, bufferHandle->fd, 0);
  if (mappedAddr == MAP_FAILED) {
    LOG(ERROR) << "mmap failed";
    OH_NativeWindow_NativeObjectUnreference(nativeWindowBuffer);
    return;
  }
  void* addr = (void*)skia_pixmap.addr();
  uint8_t* pixel = static_cast<uint8_t*>(mappedAddr);
  uint8_t* bitmapAddr = static_cast<uint8_t*>(addr);
  const size_t bytes_per_pixel = 4;
  for (uint64_t y = 0; y < height; y++) {
    auto dst = pixel + y * bufferHandle->stride;
    auto src = bitmapAddr + y * width_ * bytes_per_pixel;
#if BUILDFLAG(ARKWEB_SAFE_FUNCTION)
    if (memcpy_s(dst, width * bytes_per_pixel, src, width * bytes_per_pixel) !=
        EOK) {
      LOG(ERROR) << "OhosCanvasSurface::PresentCanvas memcpy_s failed.";
      OH_NativeWindow_NativeObjectUnreference(nativeWindowBuffer);
      return;
    }
#else
    std::memcpy(dst, src, width * bytes_per_pixel);
#endif
  }

  Region region{nullptr, 0};
  OH_NativeWindow_NativeWindowFlushBuffer((OHNativeWindow*)window_,
                                          nativeWindowBuffer, fenceFd, region);
  if (munmap(mappedAddr, bufferHandle->size) < 0) {
    LOG(ERROR) << ("munmap failed");
  }
  OH_NativeWindow_NativeObjectUnreference(nativeWindowBuffer);
}

std::unique_ptr<gfx::VSyncProvider> OhosCanvasSurface::CreateVSyncProvider() {
  return nullptr;
}
}  // namespace ui
