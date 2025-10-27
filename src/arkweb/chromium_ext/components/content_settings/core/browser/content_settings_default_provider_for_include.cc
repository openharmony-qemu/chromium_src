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

#if BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
#include "base/command_line.h"
#include "content/public/common/content_switches.h"
#endif

namespace content_settings {

static bool ShouldSkipSettingForContentTypeExt(
    ContentSettingsType content_type) {
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableNwebExExceptionList)) {
    if (content_type != ContentSettingsType::COOKIES &&
        content_type != ContentSettingsType::JAVASCRIPT) {
      return true;
    }
  } else {
    return true;
  }
}

} // namespace content_settings