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

#include "ui/ozone/platform/ohos/gpu/ohos_surface_factory.h"

#include <memory>

#include "base/files/file_util.h"
#include "base/functional/bind.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/strings/string_number_conversions.h"
#include "base/task/thread_pool.h"
#include "build/build_config.h"
#include "skia/ext/legacy_display_globals.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkImageInfo.h"
#include "third_party/skia/include/core/SkPixmap.h"
#include "third_party/skia/include/core/SkRefCnt.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "third_party/skia/include/core/SkSurfaceProps.h"
#include "ui/gfx/buffer_format_util.h"
#include "ui/gfx/codec/png_codec.h"
#include "ui/gfx/geometry/skia_conversions.h"
#include "ui/gfx/native_pixmap.h"
#include "ui/gfx/vsync_provider.h"
#include "ui/gl/gl_surface_egl.h"
#include "ui/ozone/common/egl_util.h"
#include "ui/ozone/common/gl_ozone_egl.h"
#include "ui/ozone/common/gl_surface_egl_readback.h"
#include "ui/ozone/platform/ohos/common/ohos_util.h"
#include "ui/ozone/platform/ohos/host/ohos_canvas_surface.h"
#include "ui/ozone/platform/ohos/host/ohos_window.h"
#include "ui/ozone/platform/ohos/host/ohos_window_manager.h"
#include "ui/ozone/platform/ohos/gpu/gl_surface_egl_ohos.h"
#include "ui/ozone/platform/ohos/gpu/gl_surface_egl_readback_ohos.h"
#include "ui/ozone/public/surface_ozone_canvas.h"

namespace ui {

namespace {

class GLOzoneEGLOhos : public GLOzoneEGL {
 public:
  GLOzoneEGLOhos() = default;

  GLOzoneEGLOhos(const GLOzoneEGLOhos&) = delete;
  GLOzoneEGLOhos& operator=(const GLOzoneEGLOhos&) = delete;

  bool InitializeStaticGLBindings(
    const gl::GLImplementationParts& implementation) override {
    is_swiftshader_ = gl::IsSoftwareGLImplementation(implementation);
    return GLOzoneEGL::InitializeStaticGLBindings(implementation);
  }

  ~GLOzoneEGLOhos() override = default;

  scoped_refptr<gl::GLSurface> CreateViewGLSurface(
      gl::GLDisplay* display,
      gfx::AcceleratedWidget window) override {
    if (is_swiftshader_) {
      return gl::InitializeGLSurface(
          base::MakeRefCounted<GLSurfaceEglReadbackOhos>(
              display->GetAs<gl::GLDisplayEGL>(), window));
    } else {
      auto temp = util::GetWindowFromWidget(window);
      // TODO Check why the temp is null.
      if (!temp) {
        LOG(ERROR) << "CreateViewGLSurface window is not exists, window:"
                   << window;
        return nullptr;
      }
      return gl::InitializeGLSurface(
          base::MakeRefCounted<gl::NativeViewGLSurfaceEGLOhos>(
              display->GetAs<gl::GLDisplayEGL>(),
              reinterpret_cast<EGLNativeWindowType>(temp), nullptr));
    }
  }

  scoped_refptr<gl::GLSurface> CreateOffscreenGLSurface(
      gl::GLDisplay* display,
      const gfx::Size& size) override {
    return gl::InitializeGLSurface(
        base::MakeRefCounted<gl::PbufferGLSurfaceEGL>(
            display->GetAs<gl::GLDisplayEGL>(), size));
  }

 protected:
  gl::EGLDisplayPlatform GetNativeDisplay() override {
    return gl::EGLDisplayPlatform(EGL_DEFAULT_DISPLAY);
  }

  bool LoadGLES2Bindings(
      const gl::GLImplementationParts& implementation) override {
    return LoadDefaultEGLGLES2Bindings(implementation);
  }

private:
  bool is_swiftshader_ = false;
};

}  // namespace

OhosSurfaceFactory::~OhosSurfaceFactory() = default;

OhosSurfaceFactory::OhosSurfaceFactory()
    : egl_implementation_(std::make_unique<GLOzoneEGLOhos>()) {}

std::vector<gl::GLImplementationParts>
OhosSurfaceFactory::GetAllowedGLImplementations() {
  return std::vector<gl::GLImplementationParts>{
      gl::GLImplementationParts(gl::kGLImplementationEGLGLES2),
      gl::GLImplementationParts(gl::kGLImplementationEGLANGLE),
  };
}

GLOzone* OhosSurfaceFactory::GetGLOzone(
    const gl::GLImplementationParts& implementation) {
  switch (implementation.gl) {
    case gl::kGLImplementationEGLGLES2:
    case gl::kGLImplementationEGLANGLE:
      return egl_implementation_.get();

    default:
      return nullptr;
  }
}

std::unique_ptr<SurfaceOzoneCanvas> OhosSurfaceFactory::CreateCanvasForWidget(
    gfx::AcceleratedWidget widget) {
  return std::make_unique<OhosCanvasSurface>(widget);
}

scoped_refptr<gfx::NativePixmap> OhosSurfaceFactory::CreateNativePixmap(
    gfx::AcceleratedWidget widget,
    gpu::VulkanDeviceQueue* device_queue,
    gfx::Size size,
    gfx::BufferFormat format,
    gfx::BufferUsage usage,
    absl::optional<gfx::Size> framebuffer_size) {
  return nullptr;
}

#if BUILDFLAG(ENABLE_VULKAN)
std::unique_ptr<gpu::VulkanImplementation>
OhosSurfaceFactory::CreateVulkanImplementation(bool use_swiftshader,
                                               bool allow_protected_memory) {
  return nullptr;
}
#endif  // BUILDFLAG(ENABLE_VULKAN)

}  // namespace ui
