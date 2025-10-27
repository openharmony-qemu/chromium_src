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

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
#include "base/base_switches.h"
#include "base/command_line.h"
#endif  // BUILDFLAG(ARKWEB_NETWORK_BASE)

namespace blink {

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
String Navigator::appPackageName() const {
  std::string package_name = "";
  const base::CommandLine* command_line =
      base::CommandLine::ForCurrentProcess();
  if (command_line && command_line->HasSwitch(switches::kBundleName)) {
    package_name = command_line->GetSwitchValueASCII(switches::kBundleName);
  }
  return String::FromUTF8(package_name);
}
#endif  // BUILDFLAG(ARKWEB_NETWORK_BASE)

}  // namespace blink
