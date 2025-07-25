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

#include <regex>

#include "base/command_line.h"
#include "base/ohos/locale_utils.h"
#include "content/public/common/content_switches.h"
#include "extensions/common/extension_l10n_util.h"
#include "ui/base/ui_base_switches.h"

namespace extension_l10n_util {

std::string CurrentLocaleOrDefault() {
  std::string region;
  std::string current_locale;

  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();

  if (command_line && command_line->HasSwitch(::switches::kLang)) {
    std::string lang = command_line->GetSwitchValueASCII(::switches::kLang);
    std::regex pattern("-");
    std::smatch match;
    if (std::regex_search(lang, match, pattern)) {
      region = match.suffix();
    }
  }

  current_locale = base::ohos::ComputeLanguageByRegion(region);

  if (current_locale.empty()) {
    current_locale = "en";
  }

  return current_locale;
}

}  // namespace extension_l10n_util
