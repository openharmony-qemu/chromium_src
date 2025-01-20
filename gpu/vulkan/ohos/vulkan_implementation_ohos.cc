// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/vulkan/ohos/vulkan_implementation_ohos.h"

#include <vulkan/vulkan_android.h>
#include <vulkan/vulkan.h>

#include "base/files/file_path.h"
#include "base/functional/callback_helpers.h"
#include "base/logging.h"
#include "gpu/ipc/common/nweb_native_window_tracker.h"
#include "gpu/ipc/common/vulkan_ycbcr_info.h"
#include "gpu/vulkan/vulkan_device_queue.h"
#include "gpu/vulkan/vulkan_function_pointers.h"
#include "gpu/vulkan/vulkan_image.h"
#include "gpu/vulkan/vulkan_instance.h"
#include "gpu/vulkan/vulkan_surface.h"
#include "gpu/vulkan/vulkan_util.h"
#include "ohos_adapter_helper.h"
#include "ui/gfx/gpu_fence.h"
#include "ui/gfx/gpu_memory_buffer.h"

namespace gpu {

VulkanImplementationOhos::VulkanImplementationOhos() = default;

VulkanImplementationOhos::~VulkanImplementationOhos() = default;

bool VulkanImplementationOhos::InitializeVulkanInstance(bool using_surface) {
  LOG(DEBUG) << "InitializeVulkan Instance";
  DCHECK(using_surface);
  std::vector<const char*> required_extensions = {
      VK_KHR_SURFACE_EXTENSION_NAME,
      "VK_KHR_surface", // /system/etc/vulkan/implicit_layer.d/VkLayer_swapchain.json
      VK_OHOS_SURFACE_EXTENSION_NAME,
      VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME,
      VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME};

  return vulkan_instance_.Initialize(base::FilePath("libvulkan.so"),
                                     required_extensions, {});
}

VulkanInstance* VulkanImplementationOhos::GetVulkanInstance() {
  return &vulkan_instance_;
}

std::unique_ptr<VulkanSurface> VulkanImplementationOhos::CreateViewSurface(
    gfx::AcceleratedWidget window) {
  VkSurfaceKHR surface;

  void* nativeWindow = NWebNativeWindowTracker::Instance()->GetNativeWindow(window);
  LOG(ERROR) << __FUNCTION__ <<
    "CreateViewSurface vulkan native_window_id == " << window << " nativeWindow = "<< nativeWindow;

  VkSurfaceCreateInfoOHOS surfaceCreateInfo = {};
  surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_SURFACE_CREATE_INFO_OHOS;
  surfaceCreateInfo.window = static_cast<OHNativeWindow* >(nativeWindow);

  VkResult result = vkCreateSurfaceOHOS(vulkan_instance_.vk_instance(), &surfaceCreateInfo, NULL, &surface);
  if (VK_SUCCESS != result) {
    LOG(ERROR)<< __FUNCTION__ << "vulkan vkCreateSurfaceOHOS failed";
    return nullptr;
  }
  return std::make_unique<VulkanSurface>(vulkan_instance_.vk_instance(), window, surface);
}

bool VulkanImplementationOhos::GetPhysicalDevicePresentationSupport(
    VkPhysicalDevice device,
    const std::vector<VkQueueFamilyProperties>& queue_family_properties,
    uint32_t queue_family_index) {
  // On Android, all physical devices and queue families must be capable of
  // presentation with any native window.
  // As a result there is no Android-specific query for these capabilities.
  return true;
}

std::vector<const char*>
VulkanImplementationOhos::GetRequiredDeviceExtensions() {
  return {};
}

std::vector<const char*>
VulkanImplementationOhos::GetOptionalDeviceExtensions() {
  // VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME also requires
  // VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME as per spec.
  return {
      VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,
      VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME,
      VK_KHR_EXTERNAL_MEMORY_FD_EXTENSION_NAME,
      VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME,
      VK_KHR_EXTERNAL_SEMAPHORE_FD_EXTENSION_NAME,
      VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
      VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME,
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
      VK_OHOS_EXTERNAL_MEMORY_EXTENSION_NAME,
      VK_EXT_QUEUE_FAMILY_FOREIGN_EXTENSION_NAME,
      VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,
  };
}

VkFence VulkanImplementationOhos::CreateVkFenceForGpuFence(
    VkDevice vk_device) {
  NOTREACHED();
  return VK_NULL_HANDLE;
}

std::unique_ptr<gfx::GpuFence>
VulkanImplementationOhos::ExportVkFenceToGpuFence(VkDevice vk_device,
                                                     VkFence vk_fence) {
  NOTREACHED();
  return nullptr;
}

VkSemaphore VulkanImplementationOhos::CreateExternalSemaphore(
    VkDevice vk_device) {
  return CreateExternalVkSemaphore(
      vk_device, VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_SYNC_FD_BIT);
}

VkSemaphore VulkanImplementationOhos::ImportSemaphoreHandle(
    VkDevice vk_device,
    SemaphoreHandle sync_handle) {
  return ImportVkSemaphoreHandle(vk_device, std::move(sync_handle));
}

SemaphoreHandle VulkanImplementationOhos::GetSemaphoreHandle(
    VkDevice vk_device,
    VkSemaphore vk_semaphore) {
  // VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_SYNC_FD_BIT specifies a POSIX file
  // descriptor handle to a Linux Sync File or Android Fence object.
  return GetVkSemaphoreHandle(vk_device, vk_semaphore,
                              VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_SYNC_FD_BIT);
}

VkExternalMemoryHandleTypeFlagBits
VulkanImplementationOhos::GetExternalImageHandleType() {
  return VK_EXTERNAL_MEMORY_HANDLE_TYPE_OHOS_NATIVE_BUFFER_BIT_OHOS;
}

bool VulkanImplementationOhos::CanImportGpuMemoryBuffer(
    VulkanDeviceQueue* device_queue,
    gfx::GpuMemoryBufferType memory_buffer_type) {
  return true;
}

std::unique_ptr<VulkanImage>
VulkanImplementationOhos::CreateImageFromGpuMemoryHandle(
    VulkanDeviceQueue* device_queue,
    gfx::GpuMemoryBufferHandle gmb_handle,
    gfx::Size size,
    VkFormat vk_format,
    const gfx::ColorSpace& color_space) {
  // TODO(sergeyu): Move code from CreateVkImageAndImportAHB() here and remove
  // CreateVkImageAndImportAHB().
  NOTIMPLEMENTED();
  return nullptr;
}

}  // namespace gpu
