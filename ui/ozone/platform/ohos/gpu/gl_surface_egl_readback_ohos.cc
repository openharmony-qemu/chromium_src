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

#include "ui/ozone/platform/ohos/gpu/gl_surface_egl_readback_ohos.h"

#include <cstring>
#include <sys/mman.h>
#include "base/logging.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"
#include "third_party/skia/include/core/SkImageInfo.h"
#include "third_party/skia/include/core/SkPixmap.h"
#include "ui/ozone/platform/ohos/common/ohos_util.h"

namespace ui {

GLSurfaceEglReadbackOhos::GLSurfaceEglReadbackOhos(gl::GLDisplayEGL* display,
                                                   gfx::AcceleratedWidget window)
    : GLSurfaceEglReadback(display), window_(window) {}

bool GLSurfaceEglReadbackOhos::Initialize(gl::GLSurfaceFormat format) {
  if (!GLSurfaceEglReadback::Initialize(format)) {
    return false;
  }
  return true;
}

void GLSurfaceEglReadbackOhos::Destroy() {
  GLSurfaceEglReadback::Destroy();
}

GLSurfaceEglReadbackOhos::~GLSurfaceEglReadbackOhos() {
  Destroy();
}

bool GLSurfaceEglReadbackOhos::CopyPixelsAndFlush(OHNativeWindow* native_window,
                                                  uint32_t width_src,
                                                  uint32_t height_src,
                                                  uint8_t* pixels) {
  // Obtain addr_dst through the native_window.
  OHNativeWindowBuffer* buffer = nullptr;
  int fence_fd = -1;
  OH_NativeWindow_NativeWindowRequestBuffer(native_window, &buffer, &fence_fd);
  if (buffer == nullptr || fence_fd == -1) {
      LOG(ERROR) << "get buffer failed";
      return false;
  }
  OH_NativeWindow_NativeObjectReference(buffer);
  BufferHandle* buffer_handle = OH_NativeWindow_GetBufferHandleFromNative(buffer);
  if (buffer_handle == nullptr) {
      LOG(ERROR) << "get buffer_handle failed";
      OH_NativeWindow_NativeObjectUnreference(buffer);
      return false;
  }
  void* mapped_addr = mmap(buffer_handle->virAddr, buffer_handle->size,
                           PROT_READ | PROT_WRITE, MAP_SHARED, buffer_handle->fd, 0);
  if (mapped_addr == MAP_FAILED) {
      LOG(ERROR) << "mmap failed";
      OH_NativeWindow_NativeObjectUnreference(buffer);
      return false;
  }
  uint8_t *addr_dst = static_cast<uint8_t *>(mapped_addr);

  // Obtain width_dst & height_dst
  uint32_t width_dst = UINT32_MAX;
  uint32_t height_dst = UINT32_MAX;
  int32_t code = GET_BUFFER_GEOMETRY;
  OH_NativeWindow_NativeWindowHandleOpt(native_window, code, &height_dst, &width_dst);
  width_dst = std::min(width_dst, width_src);
  height_dst = std::min(height_dst, height_src);

 // Copy pixels to addr_dst
  const size_t bytes_per_pixel = 4;
  memset(addr_dst, 0, width_dst * height_dst * bytes_per_pixel);
  uint8_t* addr_src = pixels;
  for (int i = 0; i < height_dst; i++) {
      memcpy(addr_dst, addr_src, width_dst * bytes_per_pixel);
      addr_dst += width_src * bytes_per_pixel;
      addr_src += GetSize().width() * bytes_per_pixel;
  }

  // flush
  Region region{nullptr, 0};
  OH_NativeWindow_NativeWindowFlushBuffer(native_window, buffer, fence_fd, region);

  // Release buffer
  int result = munmap(mapped_addr, buffer_handle->size);
  if (result == -1) {
      LOG(ERROR) << "munmap failed";
  }
  OH_NativeWindow_NativeObjectUnreference(buffer);
  return true;
}

bool GLSurfaceEglReadbackOhos::HandlePixels(uint8_t* pixels) {
  LOG(INFO) << "use angle swiftshader";
  if (pixels == nullptr) {
    LOG(ERROR) << "pixels == nullptr";
    return false;
  }

  auto temp = util::GetWindowFromWidget(window_);
  OHNativeWindow* native_window = static_cast<OHNativeWindow*>(temp);
  if (native_window == nullptr) {
    LOG(ERROR) << "native_window == nullptr";
    return false;
  }

  int32_t code = SET_BUFFER_GEOMETRY;
  const size_t bits_per_pixel = 64;
  uint64_t width_src = bits_per_pixel *
                       (GetSize().width() / bits_per_pixel + ((GetSize().width() % bits_per_pixel) ? 1 : 0));
  uint64_t height_src = GetSize().height();
  OH_NativeWindow_NativeWindowHandleOpt(native_window, code, width_src, height_src);

  return CopyPixelsAndFlush(native_window, width_src, height_src, pixels);
}

}  // namespace ui
