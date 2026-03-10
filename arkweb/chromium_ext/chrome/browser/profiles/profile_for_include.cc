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

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "base/command_line.h"
#include "content/public/common/content_switches.h"

bool Profile::HasOTRProfile() {
  if ((*base::CommandLine::ForCurrentProcess())
          .HasSwitch(switches::kEnableNwebEx)) {
    return off_the_record_ ? true : false;
  }

  return false;
}
  
Profile* Profile::GetOTRProfile() {
  if ((*base::CommandLine::ForCurrentProcess())
          .HasSwitch(switches::kEnableNwebEx)) {
    return off_the_record_;
  }

  return nullptr;
}
#endif
