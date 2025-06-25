// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on vulkan_image_android.cc originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 

#include <vulkan/vulkan.h>

#include "base/debug/crash_logging.h"
#include "base/logging.h"
#include "gpu/command_buffer/service/ohos/native_buffer_config_impl.h"
#include "gpu/vulkan/vulkan_device_queue.h"
#include "gpu/vulkan/vulkan_function_pointers.h"
#include "gpu/vulkan/vulkan_image.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "vulkan/vulkan.h"

namespace gpu {

namespace {
bool IsSinglePlaneRGBVulkanNBFormat(VkFormat format) {
  switch (format) {
    case VK_FORMAT_R8G8B8A8_UNORM:
    case VK_FORMAT_R8G8B8_UNORM:
    case VK_FORMAT_R5G6B5_UNORM_PACK16:
    case VK_FORMAT_R16G16B16A16_SFLOAT:
    case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
      return true;
    default:
      return false;
  }
}
}  // namespace

bool VulkanImage::InitializeFromGpuMemoryBufferHandle(
    scoped_refptr<gfx::NativePixmap> pixmap,
    VulkanDeviceQueue* device_queue,
    gfx::GpuMemoryBufferHandle gmb_handle,
    const gfx::Size& size,
    VkFormat format,
    VkImageUsageFlags usage,
    VkImageCreateFlags flags,
    VkImageTiling image_tiling,
    uint32_t queue_family_index) {
  if (gmb_handle.type == gfx::GpuMemoryBufferType::OHOS_NATIVE_BUFFER) {
    DCHECK(gmb_handle.ohos_hardware_buffer.is_valid());
    SCOPED_CRASH_KEY_BOOL("vulkan", "gmb_buffer.is_valid",
                          gmb_handle.ohos_hardware_buffer.is_valid());
    auto& nb_handle = gmb_handle.ohos_hardware_buffer;

    VkNativeBufferFormatPropertiesOHOS nb_format_props = {
        VK_STRUCTURE_TYPE_NATIVE_BUFFER_FORMAT_PROPERTIES_OHOS,
    };
    VkNativeBufferPropertiesOHOS nb_props = {
        .sType = VK_STRUCTURE_TYPE_NATIVE_BUFFER_PROPERTIES_OHOS,
        .pNext = &nb_format_props,
    };

    if (!nb_handle.get()) {
      LOG(ERROR) << "InitializeFromGpuMemoryBufferHandle null!";
      return false;
    }

    VkDevice vk_device = device_queue->GetVulkanDevice();
    VkResult result = vkGetNativeBufferPropertiesOHOS(
        vk_device, static_cast<OH_NativeBuffer*>(nb_handle.get()), &nb_props);
    if (result != VK_SUCCESS) {
      LOG(ERROR) << "vkGetNativeBufferPropertiesOHOS failed : "
                 << result;
      return false;
    }

    // To create an image with an external format, include an instance of
    // VkExternalFormatOHOS in the pNext chain of VkImageCreateInfo.
    VkExternalFormatOHOS external_format = {
        .sType = VK_STRUCTURE_TYPE_EXTERNAL_FORMAT_OHOS,
        // If externalFormat is zero, the effect is as if the
        // VkExternalFormatOHOS structure was not present. Otherwise, the image
        // will have the specified external format.
        .externalFormat = 0,
    };

    const bool should_use_external_format = !IsSinglePlaneRGBVulkanNBFormat(
        nb_format_props.format);
    if (should_use_external_format) {
      external_format.externalFormat = nb_format_props.externalFormat;
    }

    // To define a set of external memory handle types that may be used as
    // backing store for an image, add a VkExternalMemoryImageCreateInfo
    // structure to the pNext chain of the VkImageCreateInfo structure.
    VkExternalMemoryImageCreateInfo external_memory_image_info = {
        .sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO,
        .pNext = &external_format,
        .handleTypes =
            VK_EXTERNAL_MEMORY_HANDLE_TYPE_OHOS_NATIVE_BUFFER_BIT_OHOS,
    };

    OHOS::NWeb::OhosNativeBufferAdapter& adapter =
        OHOS::NWeb::OhosAdapterHelper::GetInstance()
            .GetOhosNativeBufferAdapter();
    std::shared_ptr<OHOS::NWeb::NativeBufferConfigAdapterImpl> nb_desc =
        std::make_shared<OHOS::NWeb::NativeBufferConfigAdapterImpl>();
    adapter.Describe(nb_desc, nb_handle.get());

    // Intended usage of the image.
    VkImageUsageFlags usage_flags = 0;
    auto res = nb_desc->GetBufferUsage();
    // Get Vulkan Image usage flag equivalence of NB usage.
    if ((res & gpu::NATIVEBUFFER_USAGE_HW_TEXTURE) ||
        (res & gpu::NATIVEBUFFER_USAGE_MEM_DMA)) {
      usage_flags = usage_flags | VK_IMAGE_USAGE_SAMPLED_BIT |
                    VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
    }
    if ((res & gpu::NATIVEBUFFER_USAGE_HW_RENDER) ||
        (res & gpu::NATIVEBUFFER_USAGE_MEM_DMA)) {
      usage_flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }

    if (!usage_flags) {
      LOG(ERROR) << "No valid usage flags found";
      return false;
    }

    // Skia currently requires all wrapped VkImages to have transfer src and dst
    // usage. Additionally all nativebuffer support these usages when imported into
    // vulkan.
    usage_flags |=
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    VkImageCreateFlags create_flags = 0;

    // To import memory created outside of the current Vulkan instance from an
    // OHOS native buffer, add a VkImportNativeBufferInfoOHOS
    // structure to the pNext chain of the VkMemoryAllocateInfo structure.
    VkImportNativeBufferInfoOHOS nb_import_info = {
        .sType = VK_STRUCTURE_TYPE_IMPORT_NATIVE_BUFFER_INFO_OHOS,
        .buffer = static_cast<OH_NativeBuffer*>(nb_handle.get()),
    };

    VkMemoryRequirements requirements = {
        .size = nb_props.allocationSize,
        .memoryTypeBits = nb_props.memoryTypeBits,
    };

    if (!InitializeSingleOrJointPlanes(
            device_queue, size,
            should_use_external_format ? VK_FORMAT_UNDEFINED
                                       : nb_format_props.format,
            usage_flags, create_flags, VK_IMAGE_TILING_OPTIMAL,
            &external_memory_image_info, &nb_import_info, &requirements)) {
      LOG(ERROR) << "InitializeSingleOrJointPlanes fails!";
      return false;
    }

    queue_family_index_ = queue_family_index;

    if (should_use_external_format) {
      ycbcr_info_.emplace(VK_FORMAT_UNDEFINED, nb_format_props.externalFormat,
                          nb_format_props.suggestedYcbcrModel,
                          nb_format_props.suggestedYcbcrRange,
                          nb_format_props.suggestedXChromaOffset,
                          nb_format_props.suggestedYChromaOffset,
                          nb_format_props.formatFeatures);
    }
    return true;
  } else if (gmb_handle.type == gfx::GpuMemoryBufferType::NATIVE_PIXMAP) {
    queue_family_index_ = queue_family_index;
    auto& native_pixmap_handle = gmb_handle.native_pixmap_handle;
    auto& scoped_fd = native_pixmap_handle.planes[0].fd;
    if (!scoped_fd.is_valid()) {
      return false;
    }

    VkExternalFormatOHOS external_format = {
        .sType = VK_STRUCTURE_TYPE_EXTERNAL_FORMAT_OHOS,
        // If externalFormat is zero, the effect is as if the
        // VkExternalFormatOHOS structure was not present. Otherwise, the image
        // will have the specified external format.
        .externalFormat = 0,
    };

    VkExternalMemoryImageCreateInfo external_memory_image_info = {
        .sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO,
        .pNext = &external_format,
        .handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OHOS_NATIVE_BUFFER_BIT_OHOS,
    };

    // TODO Get VkImageUsageFlags via NativeBuffer Describe.
    VkImageUsageFlags usage_flags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                              VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                              VK_IMAGE_USAGE_SAMPLED_BIT |
                              VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;

    void* native_buffer = nullptr;
    OHOS::NWeb::OhosNativeBufferAdapter& adapter =
      OHOS::NWeb::OhosAdapterHelper::GetInstance().GetOhosNativeBufferAdapter();
    adapter.NativeBufferFromNativeWindowBuffer(pixmap->GetWindowBuffer(), &native_buffer);

    VkImportNativeBufferInfoOHOS nb_import_info = {
        .sType = VK_STRUCTURE_TYPE_IMPORT_NATIVE_BUFFER_INFO_OHOS,
        .buffer = static_cast<OH_NativeBuffer*>(native_buffer),
    };

    // TODO Get VkMemoryRequirements via NativeBuffer Prop using vkGetNativeBufferPropertiesOHOS,
    // but that fails now.
    VkMemoryRequirements* req = nullptr;
    if (!InitializeSingleOrJointPlanes(
            device_queue, size,
            VK_FORMAT_R8G8B8A8_UNORM,
            usage_flags, 0, VK_IMAGE_TILING_OPTIMAL,
            &external_memory_image_info, &nb_import_info, req)) {
      return false;
    }
    return true;
  }

  return false;
}

}  // namespace gpu
