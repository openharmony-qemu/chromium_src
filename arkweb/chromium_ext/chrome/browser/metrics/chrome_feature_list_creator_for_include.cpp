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

#if BUILDFLAG(ARKWEB_SAFEBROWSING)
#include "arkweb/ohos_nweb_ex/overrides/ohos_nweb/src/cef_delegate/cloud_control_config/global_config/nweb_cloud_control_global_config.h"
#include "base/base_switches.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "base/task/task_traits.h"
#include "base/task/thread_pool.h"
#include "base/threading/platform_thread.h"
#include "base/values.h"
#include "cef/ohos_cef_ext/libcef/browser/global_config/global_config_prefs.h"
#include "components/prefs/pref_service.h"
#endif

namespace {

#if BUILDFLAG(ARKWEB_SAFEBROWSING)
void ApplyCommandLineFromJson(const std::string& commandline) {
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();

  // Split the command line into individual arguments.
  std::vector<std::string> args = base::SplitString(
      commandline, " ", base::TRIM_WHITESPACE, base::SPLIT_WANT_NONEMPTY);

  for (const auto& arg : args) {
    if (arg.empty()) {
      continue;
    }

    size_t equals_pos = arg.find('=');
    if (equals_pos != std::string::npos && arg.length() >= 2 && (arg[0] == '-' && arg[1] == '-')) {
      // Argument with value: --switch=value
      std::string switch_name = arg.substr(0, equals_pos);
      std::string value = arg.substr(equals_pos + 1);
      std::string existing = command_line->GetSwitchValueASCII(switch_name.substr(2));
      std::set<std::string> all_features;  // Automatic deduplication using set

      // Join existing
      if (!existing.empty()) {
          std::vector<std::string> existing_list =
          base::SplitString(existing, std::string_view(","), base::TRIM_WHITESPACE, base::SPLIT_WANT_NONEMPTY);
          all_features.insert(existing_list.begin(), existing_list.end());
      }

      // Join the new data
      std::vector<std::string> new_list =
      base::SplitString(value, std::string_view(","), base::TRIM_WHITESPACE, base::SPLIT_WANT_NONEMPTY);
      all_features.insert(new_list.begin(), new_list.end());

      // Recombined
      std::vector<std::string> features_vector(all_features.begin(), all_features.end());
      std::string final_value = base::JoinString(features_vector, ",");

      // Update switch (remove and then add)
      command_line->RemoveSwitch(switch_name.substr(2));
      command_line->AppendSwitchASCII(switch_name.substr(2), final_value);
    } else if (arg.length() >= 2 && arg[0] == '-' && arg[1] == '-') {
      // Switch without value: --switch
      command_line->AppendSwitch(arg.substr(2));
    } else {
      // Regular argument
      command_line->AppendArgNative(arg);
    }
  }
}

void AddGlobalConfigFeaturesSwitchesToCommandLine(PrefService* local_state) {
  if (!local_state) {
    return;
  }
  const base::Value::List& featuresSwitches =
    local_state->GetList(global_config::kGlobalConfigFeaturesSwitches);
  if (featuresSwitches.empty()) {
    return;
  }
  for (const auto& item : featuresSwitches) {
    if (!item.is_dict()) {
      continue;
    }

    const std::string* name = item.GetDict().FindString("name");
    const std::string* cmdLine = item.GetDict().FindString("commandline");
    if (name && cmdLine) {
      ApplyCommandLineFromJson(*cmdLine);
    }
  }
}

void DealGlobalConfig(PrefService* local_state) {
  AddGlobalConfigFeaturesSwitchesToCommandLine(local_state);

  OHOS::NWeb::NwebCloudControlGlobalConfig::GetInstance().HandleGlobalConfig(local_state);
}
#endif
}
