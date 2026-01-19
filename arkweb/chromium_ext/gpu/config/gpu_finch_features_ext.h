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


#ifndef GPU_CONFIG_GPU_FINCH_FEATURES_EXT_H_
#define GPU_CONFIG_GPU_FINCH_FEATURES_EXT_H_

#include "arkweb/build/features/features.h"
#include "base/command_line.h"

#if BUILDFLAG(ARKWEB_VULKAN)
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#endif
#include "gpu/config/gpu_finch_features.h"

namespace features {
#if BUILDFLAG(ARKWEB_VULKAN)
bool IsEnableVulkan();
#endif

#if BUILDFLAG(ARKWEB_VULKAN)
GPU_EXPORT BASE_DECLARE_FEATURE(kInsertVKEndSemaphore);

GPU_EXPORT BASE_DECLARE_FEATURE(kVulkanVideoZeroCopy);
#endif

#if BUILDFLAG(ARKWEB_DRDC)
bool IsDrDcForVulkan();
#endif

#if BUILDFLAG(ARKWEB_WEBGL)
GPU_EXPORT BASE_DECLARE_FEATURE(kPreferDrawToCopy);
#endif
}

#endif
