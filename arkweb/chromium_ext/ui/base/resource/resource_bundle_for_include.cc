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

#include "arkweb/build/features/features.h"

#if BUILDFLAG(ARKWEB_HAP_DECOMPRESSED)
#include "base/command_line.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#endif

namespace ui {

#if BUILDFLAG(ARKWEB_HAP_DECOMPRESSED)
bool LocaleDataPakExistsExt(const std::string& locale) {
  const auto path = ResourceBundle::GetLocaleFilePath(locale);
  // If the hap package is not decompressed, the directory does not exist.
  if (path.empty() || !base::PathExists(path)) {
    if (locale == "zh-CN" || locale == "en-US" || locale == "resources" ||
        locale == "bo-CN" || locale == "ug" || locale == "zh-TW" ||
        locale == "zh-HK" || locale == "chrome_100_percent" ||
        locale == "chrome_200_percent") {
      return true;
    } else {
      return false;
    }
  } else {
    return true;
  }
}
#endif

}  // namespace ui
