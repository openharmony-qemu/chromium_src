/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gpu/command_buffer/service/shared_image/shared_image_util.h"

#include "gpu/command_buffer/service/shared_image/shared_image_backing.h"
#include "base/logging.h"


namespace gpu {
namespace ohos {
const char* SharedImageUsageToString(SharedImageUsage usage)
{
    switch (usage) {
        case SHARED_IMAGE_USAGE_GLES2_READ:
            return "SHARED_IMAGE_USAGE_GLES2_READ";
        case SHARED_IMAGE_USAGE_RASTER_READ:
            return "SHARED_IMAGE_USAGE_RASTER_READ";
        case SHARED_IMAGE_USAGE_DISPLAY_READ:
            return "SHARED_IMAGE_USAGE_DISPLAY_READ";
        case SHARED_IMAGE_USAGE_DISPLAY_WRITE:
            return "SHARED_IMAGE_USAGE_DISPLAY_WRITE";
        case SHARED_IMAGE_USAGE_SCANOUT:
            return "SHARED_IMAGE_USAGE_SCANOUT";
        case SHARED_IMAGE_USAGE_OOP_RASTERIZATION:
            return "SHARED_IMAGE_USAGE_OOP_RASTERIZATION";
        case SHARED_IMAGE_USAGE_WEBGPU_READ:
            return "SHARED_IMAGE_USAGE_WEBGPU_READ";
        case SHARED_IMAGE_USAGE_CONCURRENT_READ_WRITE:
            return "SHARED_IMAGE_USAGE_CONCURRENT_READ_WRITE";
        case SHARED_IMAGE_USAGE_VIDEO_DECODE:
            return "SHARED_IMAGE_USAGE_VIDEO_DECODE";
        case SHARED_IMAGE_USAGE_WEBGPU_SWAP_CHAIN_TEXTURE:
            return "SHARED_IMAGE_USAGE_WEBGPU_SWAP_CHAIN_TEXTURE";
        case SHARED_IMAGE_USAGE_MACOS_VIDEO_TOOLBOX:
            return "SHARED_IMAGE_USAGE_MACOS_VIDEO_TOOLBOX";
        case SHARED_IMAGE_USAGE_MIPMAP:
            return "SHARED_IMAGE_USAGE_MIPMAP";
        case SHARED_IMAGE_USAGE_CPU_WRITE:
            return "SHARED_IMAGE_USAGE_CPU_WRITE";
        case SHARED_IMAGE_USAGE_RAW_DRAW:
            return "SHARED_IMAGE_USAGE_RAW_DRAW";
        case SHARED_IMAGE_USAGE_RASTER_DELEGATED_COMPOSITING:
            return "SHARED_IMAGE_USAGE_RASTER_DELEGATED_COMPOSITING";
        case SHARED_IMAGE_USAGE_HIGH_PERFORMANCE_GPU:
            return "SHARED_IMAGE_USAGE_HIGH_PERFORMANCE_GPU";
        case SHARED_IMAGE_USAGE_SCANOUT_DCOMP_SURFACE:
            return "SHARED_IMAGE_USAGE_SCANOUT_DCOMP_SURFACE";
        case SHARED_IMAGE_USAGE_WEBGPU_STORAGE_TEXTURE:
            return "SHARED_IMAGE_USAGE_WEBGPU_STORAGE_TEXTURE";
        case SHARED_IMAGE_USAGE_CPU_UPLOAD:
            return "SHARED_IMAGE_USAGE_CPU_UPLOAD";
        default:
            return "Unknown Usage";
    }
}

void PrintSharedImageUsageInfo(SharedImageUsageSet valid_usages)
{
    uint32_t usage = 1;
    while (usage <= LAST_SHARED_IMAGE_USAGE) {
        if (valid_usages & usage) {
            LOG(DEBUG)
                << __FUNCTION__
                << " HWIMGDEC - Supported SharedImageBackingFactory with usage: "
                << SharedImageUsageToString(static_cast<SharedImageUsage>(usage))
                << " (" << usage << ")";
        }
        usage <<= 1;  // Shift to the next enum value
    }
}
} // namespace ohos
} // namespace gpu
