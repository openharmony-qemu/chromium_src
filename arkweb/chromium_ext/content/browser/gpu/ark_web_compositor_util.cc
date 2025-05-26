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

#include "arkweb/chromium_ext/content/browser/gpu/ark_web_compositor_util.h"

#include <optional>

#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "base/command_line.h"
#include "cc/base/switches.h"

namespace content {

std::optional<int> ArkWebNumberOfRendererRasterThreads() {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  const base::CommandLine& command_line =
      *base::CommandLine::ForCurrentProcess();
  if (command_line.HasSwitch(switches::kEnableNwebEx)) {
    // TODO(ccs): Check the logic works fine.
    bool excludable_devices = base::CommandLine::ForCurrentProcess()->HasSwitch(
        switches::kNumRasterThreads);
    // Same with leagcy mobile phone.
    if (!excludable_devices) {
      return 1;
    }
  }
#endif
  return std::nullopt;
}

}  // namespace content
