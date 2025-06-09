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

#ifndef UI_OZONE_PLATFORM_OHOS_OHOS_SURFACE_FACTORY_H_
#define UI_OZONE_PLATFORM_OHOS_OHOS_SURFACE_FACTORY_H_

#include <memory>
#include <vector>

#include "base/files/file_path.h"
#include "ui/ozone/public/gl_ozone.h"
#include "ui/ozone/public/surface_factory_ozone.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace ui {

class OhosSurfaceFactory : public SurfaceFactoryOzone {
 public:
  OhosSurfaceFactory();

  OhosSurfaceFactory(const OhosSurfaceFactory&) = delete;
  OhosSurfaceFactory& operator=(const OhosSurfaceFactory&) = delete;

  ~OhosSurfaceFactory() override;

  std::vector<gl::GLImplementationParts> GetAllowedGLImplementations() override;
  GLOzone* GetGLOzone(const gl::GLImplementationParts& implementation) override;
#if BUILDFLAG(ENABLE_VULKAN)
  std::unique_ptr<gpu::VulkanImplementation> CreateVulkanImplementation(
      bool use_swiftshader,
      bool allow_protected_memory) override;
#endif  // BUILDFLAG(ENABLE_VULKAN)
  std::unique_ptr<SurfaceOzoneCanvas> CreateCanvasForWidget(
      gfx::AcceleratedWidget widget) override;
  scoped_refptr<gfx::NativePixmap> CreateNativePixmap(
      gfx::AcceleratedWidget widget,
      gpu::VulkanDeviceQueue* device_queue,
      gfx::Size size,
      gfx::BufferFormat format,
      gfx::BufferUsage usage,
      absl::optional<gfx::Size> framebuffer_size = absl::nullopt) override;

 private:
  std::unique_ptr<GLOzone> egl_implementation_;
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OHOS_OHOS_SURFACE_FACTORY_H_
