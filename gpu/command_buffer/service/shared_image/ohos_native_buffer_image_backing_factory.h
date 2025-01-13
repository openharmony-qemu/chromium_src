// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_SERVICE_OHOS_NATIVE_BUFFER_IMAGE_BACKING_FACTORY_H_
#define GPU_COMMAND_BUFFER_SERVICE_OHOS_NATIVE_BUFFER_IMAGE_BACKING_FACTORY_H_

#include <dawn/dawn_proc_table.h>

#include "base/containers/flat_map.h"
#include "gpu/command_buffer/service/gles2_cmd_validation.h"
#include "gpu/command_buffer/service/shared_image/shared_image_backing_factory.h"
#include "gpu/gpu_gles2_export.h"
#include "ohos_adapter_helper.h"
#include "ui/gl/gl_bindings.h"

namespace gfx {
class Size;
class ColorSpace;
}  // namespace gfx

namespace gpu {

namespace gles2 {
class FeatureInfo;
}  // namespace gles2

class SharedImageBacking;
struct Mailbox;

// Implementation of SharedImageBackingFactory that produces OHOS Native Buffer
// backed SharedImages. This is meant to be used on OHOS only.
class GPU_GLES2_EXPORT OHOSNativeBufferImageBackingFactory
    : public SharedImageBackingFactory {
 public:
  explicit OHOSNativeBufferImageBackingFactory(
      const gles2::FeatureInfo* feature_info,
      const GpuPreferences& gpu_preferences);

  OHOSNativeBufferImageBackingFactory(
      const OHOSNativeBufferImageBackingFactory&) = delete;
  OHOSNativeBufferImageBackingFactory& operator=(
      const OHOSNativeBufferImageBackingFactory&) = delete;

  ~OHOSNativeBufferImageBackingFactory() override;

  // SharedImageBackingFactory implementation.
  std::unique_ptr<SharedImageBacking> CreateSharedImage(
      const Mailbox& mailbox,
      viz::SharedImageFormat format,
      SurfaceHandle surface_handle,
      const gfx::Size& size,
      const gfx::ColorSpace& color_space,
      GrSurfaceOrigin surface_origin,
      SkAlphaType alpha_type,
      uint32_t usage,
      std::string debug_label,
      bool is_thread_safe) override;
  std::unique_ptr<SharedImageBacking> CreateSharedImage(
      const Mailbox& mailbox,
      viz::SharedImageFormat format,
      const gfx::Size& size,
      const gfx::ColorSpace& color_space,
      GrSurfaceOrigin surface_origin,
      SkAlphaType alpha_type,
      uint32_t usage,
      std::string debug_label,
      base::span<const uint8_t> pixel_data) override;
  std::unique_ptr<SharedImageBacking> CreateSharedImage(
      const Mailbox& mailbox,
      gfx::GpuMemoryBufferHandle handle,
      gfx::BufferFormat buffer_format,
      gfx::BufferPlane plane,
      const gfx::Size& size,
      const gfx::ColorSpace& color_space,
      GrSurfaceOrigin surface_origin,
      SkAlphaType alpha_type,
      uint32_t usage,
      std::string debug_label) override;
  bool IsSupported(uint32_t usage,
                   viz::SharedImageFormat format,
                   const gfx::Size& size,
                   bool thread_safe,
                   gfx::GpuMemoryBufferType gmb_type,
                   GrContextType gr_context_type,
                   base::span<const uint8_t> pixel_data) override;
  bool IsFormatSupported(viz::SharedImageFormat format);

 private:
  struct FormatInfo {
    FormatInfo();
    ~FormatInfo();

    unsigned int nb_format = 0;
    bool gl_supported = false;
    GLuint internal_format = 0;
    GLenum gl_format = 0;
    GLenum gl_type = 0;
  };

  static FormatInfo FormatInfoForSupportedFormat(
      viz::SharedImageFormat format,
      const gles2::Validators* validators);

  bool ValidateUsage(uint32_t usage,
                     const gfx::Size& size,
                     viz::SharedImageFormat format) const;

  bool CanImportGpuMemoryBuffer(gfx::GpuMemoryBufferType memory_buffer_type);

  std::unique_ptr<SharedImageBacking> MakeBacking(
    const Mailbox& mailbox,
    viz::SharedImageFormat format,
    const gfx::Size& size,
    const gfx::ColorSpace& color_space,
    GrSurfaceOrigin surface_origin,
    SkAlphaType alpha_type,
    uint32_t usage,
    bool is_thread_safe,
    base::span<const uint8_t> pixel_data);

  std::unique_ptr<SharedImageBacking> MakeBackingWithValidateConfig(
    const Mailbox& mailbox,
    viz::SharedImageFormat format,
    const gfx::Size& size,
    const gfx::ColorSpace& color_space,
    GrSurfaceOrigin surface_origin,
    SkAlphaType alpha_type,
    uint32_t usage,
    bool is_thread_safe,
    base::span<const uint8_t> pixel_data);

  const FormatInfo& GetFormatInfo(viz::SharedImageFormat format) const {
    auto iter = format_infos_.find(format);
    CHECK(iter != format_infos_.end());
    return iter->second;
  }

  base::flat_map<viz::SharedImageFormat, FormatInfo> format_infos_;
  int32_t max_gl_texture_size_ = 0;

  scoped_refptr<base::RefCountedData<DawnProcTable>> dawn_procs_;
  const bool use_passthrough_;
};

}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_SERVICE_OHOS_NATIVE_BUFFER_IMAGE_BACKING_FACTORY_H_
