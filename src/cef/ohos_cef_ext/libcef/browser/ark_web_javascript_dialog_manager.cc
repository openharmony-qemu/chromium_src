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

#include "cef/ohos_cef_ext/libcef/browser/ark_web_javascript_dialog_manager.h"

#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "components/strings/grit/components_strings.h"
#include "ui/base/l10n/l10n_util.h"

constexpr int32_t API_TARGET_VERSION_12 = 12;
// Same as API_VERSION_MOD in js_ui_ability.cpp of ability_runtime
constexpr int32_t API_VERSION_MOD = 100;

int32_t GetApiTargetVersion() {
  if (!base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kOhosAppApiVersion)) {
    LOG(ERROR) << "kOhosAppApiVersion not exist";
    return -1;
  }
  std::string apiVersion =
      base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
          switches::kOhosAppApiVersion);
  if (apiVersion.empty()) {
    return -1;
  }
  return std::stoi(apiVersion) % API_VERSION_MOD;
}

const std::u16string ArkWebGetBeforeUnloadDialogMessage() {
  static int32_t api_target_version = GetApiTargetVersion();
  std::u16string message = u"Is it OK to leave/reload this page?";
  if (api_target_version >= API_TARGET_VERSION_12) {
    message = l10n_util::GetStringUTF16(IDS_BEFOREUNLOAD_MESSAGEBOX_MESSAGE);
  }
  return message;
}
