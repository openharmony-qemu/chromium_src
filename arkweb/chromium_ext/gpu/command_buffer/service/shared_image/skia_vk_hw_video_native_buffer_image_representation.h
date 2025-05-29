// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_SERVICE_SHARED_IMAGE_SKIA_VK_OHOS_HW_VIDEO_NATIVE_BUFFER_IMAGE_REPRESENTATION_H_
#define GPU_COMMAND_BUFFER_SERVICE_SHARED_IMAGE_SKIA_VK_OHOS_HW_VIDEO_NATIVE_BUFFER_IMAGE_REPRESENTATION_H_

#include <vulkan/vulkan.h>
#include <memory>

#include "base/memory/scoped_refptr.h"
#include "gpu/command_buffer/service/shared_image/shared_image_representation.h"
#include "gpu/command_buffer/service/ohos/hw_video_native_buffer_image_backing.h"
#include "third_party/skia/include/private/chromium/GrPromiseImageTexture.h"
#include "gpu/command_buffer/service/skia_utils.h"


namespace gpu {
class SharedContextState;
class HwVideoNativeBufferImageBacking;
class VulkanImage;
class VulkanImplementation;

// A generic Skia vulkan representation which can be used by any backing on
// OHOS.
class SkiaVkHWVideoNBImageRepresentation : public SkiaGaneshImageRepresentation {
 public:
  SkiaVkHWVideoNBImageRepresentation(
      SharedImageManager* manager,
      OhosImageBacking* backing,
      scoped_refptr<SharedContextState> context_state,
      MemoryTypeTracker* tracker);

  ~SkiaVkHWVideoNBImageRepresentation() override;

  std::vector<sk_sp<SkSurface>> BeginWriteAccess(
      int final_msaa_count,
      const SkSurfaceProps& surface_props,
      const gfx::Rect& update_rect,
      std::vector<GrBackendSemaphore>* begin_semaphores,
      std::vector<GrBackendSemaphore>* end_semaphores,
      std::unique_ptr<skgpu::MutableTextureState>* end_state) override;
  std::vector<sk_sp<GrPromiseImageTexture>> BeginWriteAccess(
      std::vector<GrBackendSemaphore>* begin_semaphores,
      std::vector<GrBackendSemaphore>* end_semaphores,
      std::unique_ptr<skgpu::MutableTextureState>* end_state) override;
  void EndWriteAccess() override;
  std::vector<sk_sp<GrPromiseImageTexture>> BeginReadAccess(
      std::vector<GrBackendSemaphore>* begin_semaphores,
      std::vector<GrBackendSemaphore>* end_semaphores,
      std::unique_ptr<skgpu::MutableTextureState>* end_state) override;
  void EndReadAccess() override;

 protected:
  OhosImageBacking* hw_ohos_backing() const
  {
    return static_cast<OhosImageBacking*>(backing());
  }

  SharedContextState* context_state() const { return context_state_.get(); }

  std::unique_ptr<VulkanImage> vulkan_image_;

  // Initial read fence to wait on before reading |vulkan_image_|.
  base::ScopedFD init_read_fence_;
  sk_sp<GrPromiseImageTexture> promise_texture_;

 private:
  bool BeginAccess(bool readonly,
                   std::vector<GrBackendSemaphore>* begin_semaphores,
                   std::vector<GrBackendSemaphore>* end_semaphores,
                   base::ScopedFD init_read_fence);
  void EndAccess(bool readonly);
  std::unique_ptr<skgpu::MutableTextureState> GetEndAccessState();

  VkDevice vk_device();
  VulkanImplementation* vk_implementation();
  VkPhysicalDevice vk_phy_device();
  VkQueue vk_queue();

  RepresentationAccessMode mode_ = RepresentationAccessMode::kNone;
  int surface_msaa_count_ = 0;
  sk_sp<SkSurface> surface_;
  scoped_refptr<SharedContextState> context_state_;
  VkSemaphore begin_access_semaphore_ = VK_NULL_HANDLE;
  VkSemaphore end_access_semaphore_ = VK_NULL_HANDLE;
};

}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_SERVICE_SHARED_IMAGE_SKIA_VK_OHOS_HW_VIDEO_NATIVE_BUFFER_IMAGE_REPRESENTATION_H_
