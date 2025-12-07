/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "gpu/config/gpu_finch_features.h"
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "arkweb/chromium_ext/gpu/config/gpu_finch_features_ext.h"

namespace features {
#if BUILDFLAG(ARKWEB_VULKAN)
bool IsEnableVulkan()
{
  auto& system_properties_adapter = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                    .GetSystemPropertiesInstance();
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  bool cmd_value = false;
  if (command_line) {
    cmd_value = command_line->HasSwitch(switches::kOhosEnableVulkan);
  }
  std::string vulkan_enable = system_properties_adapter.GetVulkanStatus();
  LOG(DEBUG) << "vulkan switch config is: " << cmd_value
             << ", cmd is: " << vulkan_enable;
  if (vulkan_enable == "false") {
    return false;
  } else if (vulkan_enable == "None") {
    return cmd_value;
  } else {
    return true;
  }
}
#endif

#if BUILDFLAG(ARKWEB_VULKAN)
BASE_FEATURE(kInsertVKEndSemaphore,
             "InsertVKEndSemaphore",
             base::FEATURE_ENABLED_BY_DEFAULT);
#endif

#if BUILDFLAG(ARKWEB_DRDC)
bool IsDrDcForVulkan()
{
  if (IsEnableVulkan()) {
    LOG(DEBUG) << "vulkan drdc enabled "
               << base::FeatureList::IsEnabled(features::kEnableDrDc);
    return base::FeatureList::IsEnabled(features::kEnableDrDc);
  }
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  if (command_line) {
    LOG(DEBUG) << "vulkan drdc enabled "
               << command_line->HasSwitch(::switches::kOhosEnableDrDc);
    return command_line->HasSwitch(::switches::kOhosEnableDrDc);
  }
  return false;
}
#endif

#if BUILDFLAG(ARKWEB_WEBGL)
BASE_FEATURE(kPreferDrawToCopy,
             "PreferDrawToCopy",
             base::FEATURE_ENABLED_BY_DEFAULT);
#endif
}
