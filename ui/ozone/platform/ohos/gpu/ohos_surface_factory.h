// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_OHOS_OHOS_SURFACE_FACTORY_H_
#define UI_OZONE_PLATFORM_OHOS_OHOS_SURFACE_FACTORY_H_

#include <memory>
#include <vector>

#include "base/files/file_path.h"
#include "third_party/abseil-cpp/absl/types/optional.h"
#include "ui/ozone/public/gl_ozone.h"
#include "ui/ozone/public/surface_factory_ozone.h"

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
