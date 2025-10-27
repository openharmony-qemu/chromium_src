// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_SERVICE_SHARED_IMAGE_OHOS_NATIVE_BUFFER_IMAGE_BACKING_H_
#define GPU_COMMAND_BUFFER_SERVICE_SHARED_IMAGE_OHOS_NATIVE_BUFFER_IMAGE_BACKING_H_

#include "base/containers/flat_set.h"
#include "base/files/scoped_file.h"
#include "base/ohos/scoped_native_buffer_handle.h"
#include "build/buildflag.h"
#include "gpu/command_buffer/service/ohos/ohos_image_backing.h"
#include "gpu/command_buffer/service/ref_counted_lock.h"
#include "gpu/command_buffer/service/shared_image/shared_image_backing.h"
#include "gpu/command_buffer/service/shared_image/shared_image_format_service_utils.h"

namespace gpu {

class OhosNativeBufferImageBacking : public OhosImageBacking {
 public:
  OhosNativeBufferImageBacking(const Mailbox& mailbox,
                               viz::SharedImageFormat format,
                               const gfx::Size& size,
                               const gfx::ColorSpace& color_space,
                               GrSurfaceOrigin surface_origin,
                               SkAlphaType alpha_type,
                               SharedImageUsageSet usage,
                               std::string debug_label,
                               size_t estimated_size,
                               bool is_thread_safe,
                               base::ScopedFD initial_upload_fd,
                               const GLFormatCaps& gl_format_caps_);

  OhosNativeBufferImageBacking(const Mailbox& mailbox,
                               viz::SharedImageFormat format,
                               const gfx::Size& size,
                               const gfx::ColorSpace& color_space,
                               GrSurfaceOrigin surface_origin,
                               SkAlphaType alpha_type,
                               SharedImageUsageSet usage,
                               std::string debug_label,
                               gpu::ScopedNativeBufferHandle handle,
                               size_t estimated_size,
                               bool is_thread_safe,
                               base::ScopedFD initial_upload_fd,
                               bool use_passthrough,
                               const GLFormatCaps& gl_format_caps_);

  ~OhosNativeBufferImageBacking() override;
  OhosNativeBufferImageBacking(const OhosNativeBufferImageBacking&) = delete;
  OhosNativeBufferImageBacking& operator=(const OhosNativeBufferImageBacking&) =
      delete;

  SharedImageBackingType GetType() const override;
  void Update(std::unique_ptr<gfx::GpuFence> in_fence) override;
  gfx::Rect ClearedRect() const override;
  void SetClearedRect(const gfx::Rect& cleared_rect) override;
  gpu::ScopedNativeBufferHandle GetNativeBufferHandle() const;

 protected:
  std::unique_ptr<GLTextureImageRepresentation> ProduceGLTexture(
      SharedImageManager* manager,
      MemoryTypeTracker* tracker) override;

  std::unique_ptr<GLTexturePassthroughImageRepresentation>
  ProduceGLTexturePassthrough(SharedImageManager* manager,
                              MemoryTypeTracker* tracker) override;

  std::unique_ptr<SkiaGaneshImageRepresentation> ProduceSkiaGanesh(
      SharedImageManager* manager,
      MemoryTypeTracker* tracker,
      scoped_refptr<SharedContextState> context_state) override;

  const gpu::ScopedNativeBufferHandle native_buffer_handle_;

  const bool use_passthrough_;

  mutable RefCountedLockHelperDrDc drdc_lock_helper_;
  const GLFormatCaps gl_format_caps_;
};

}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_SERVICE_SHARED_IMAGE_OHOS_NATIVE_BUFFER_IMAGE_BACKING_H_
