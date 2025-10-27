// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/browser/global_config/global_config_prefs.h"

#include <AbilityKit/ability_runtime/application_context.h>
 
#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/hash/hash.h"
#include "base/json/json_reader.h"
#include "base/json/json_writer.h"
#include "base/logging.h"
#include "base/strings/string_number_conversions.h"
#include "base/uuid.h"
#include "base/values.h"
#include "chrome/browser/browser_process.h"
#include "content/public/browser/browser_thread.h"
#include "components/version_info/version_info.h"

namespace global_config {

#if BUILDFLAG(IS_ARKWEB_EXT)
const uint32_t kMaxProbability = 10001;
const char kGlobalConfigFeaturesSwitches[] = "global_config.FeaturesSwitches";
std::string kGlobalConfigDataPath = "";

void RegisterGlobalConfigPrefs(PrefRegistrySimple* registry) {
  registry->RegisterListPref(kGlobalConfigFeaturesSwitches);
}

bool CheckFeaturesSwitches(const base::Value& item) {
  const std::string* name = item.GetDict().FindString("name");
  const std::string* cmd_line = item.GetDict().FindString("commandline");
  std::optional<int> probability = item.GetDict().FindInt("probability");
  if ((!name || name->empty()) || (!cmd_line || cmd_line->empty())
      || (!probability || !probability.has_value())) {
    LOG(INFO) << "FeaturesSwitches is invalid.";
    return false;
  }

  std::string hash_str = base::Uuid::GenerateRandomV4().AsLowercaseString() + *name;
  size_t hash_value = base::FastHash(hash_str);
  if ((int)(hash_value % kMaxProbability) > probability.value()) {
    LOG(INFO) << "probability is Not Satisfied.";
    return false;
  }
  return true;
}

void SetFeaturesSwitchesToPrefsFile(base::Value::List prefsList, PrefService* localState) {
  if (!content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    content::GetUIThreadTaskRunner({})->PostTask(
        FROM_HERE,
        base::BindOnce(&SetFeaturesSwitchesToPrefsFile, std::move(prefsList), localState));
    return;
  }

  if (g_browser_process && g_browser_process->local_state()) {
    g_browser_process->local_state()->ClearPref(kGlobalConfigFeaturesSwitches);
    g_browser_process->local_state()->SetList(kGlobalConfigFeaturesSwitches, std::move(prefsList));
    g_browser_process->local_state()->CommitPendingWrite();
  } else {
    if (localState != nullptr) {
      localState->ClearPref(kGlobalConfigFeaturesSwitches);
      localState->SetList(kGlobalConfigFeaturesSwitches, std::move(prefsList));
      localState->CommitPendingWrite();
    }
  }
}

bool ProcessFeaturesSwitches(const base::Value& value, PrefService* localState) {
  const base::Value::List* switches_list = value.GetDict().FindList("FeaturesSwitches");
  if (!switches_list) {
    LOG(WARNING) << "Missing 'FeaturesSwitches' list.";
    return false;
  }
 
  AbilityRuntime_ErrorCode code = ABILITY_RUNTIME_ERROR_CODE_PARAM_INVALID;
  constexpr int32_t NATIVE_BUFFER_SIZE = 1024;
  char bundle_name[NATIVE_BUFFER_SIZE] = {0};
  int32_t bundle_name_length = 0;
  code = OH_AbilityRuntime_ApplicationContextGetBundleName(bundle_name, NATIVE_BUFFER_SIZE, &bundle_name_length);
  if (code != ABILITY_RUNTIME_ERROR_CODE_NO_ERROR) {
    LOG(ERROR) << "OH_AbilityRuntime_ApplicationContextGetBundleName failed:err=" << code;
    return false;
  }
  
  std::string bundle_name_str(bundle_name);
  if(bundle_name_str.empty()) {
    LOG(ERROR) << "bundle_name is empty.";
    return false;
  }
 
  base::Value::List prefs_list;
  for (const auto& item : *switches_list) {
    if (!item.is_dict()) {
      continue;
    }
    const base::Value::List* app_list = item.GetDict().FindList("applist");
    for (const auto& val : *app_list) {
      const std::string *app_name = val.GetIfString();
      if (!app_name || (*app_name != bundle_name_str && *app_name != "*")) {
        continue;
      }
      if (CheckFeaturesSwitches(item)) {
        const std::string* name = item.GetDict().FindString("name");
        const std::string* cmd_line = item.GetDict().FindString("commandline");
        base::Value::Dict switch_info;
        switch_info.Set("name", *name);
        switch_info.Set("commandline", *cmd_line);
        prefs_list.Append(std::move(switch_info));
      }
    }
  }
 
  SetFeaturesSwitchesToPrefsFile(std::move(prefs_list), localState);
  return true;
}

bool ParseFeaturesSwitchesToPrefs(PrefService* localState) {
  base::FilePath global_config_path(kGlobalConfigDataPath);
  if (!base::PathExists(global_config_path)) {
    LOG(ERROR) << "global config path is not exist:" << global_config_path.value();
    return false;
  }

  if (!base::ReadFileToString(global_config_path, &global_config_json)) {
    LOG(WARNING) << "read global config file failed.";
    return false;
  }

  std::optional<base::Value> global_config_value = base::JSONReader::Read(global_config_json);
  if (!global_config_value || !global_config_value->is_dict()) {
    LOG(WARNING) << "Invalid JSON.";
    return false;
  }

  const base::Value::List* versioned_config_list = global_config_value->GetDict().FindList("VersionedConfig");
  if (!versioned_config_list) {
    LOG(WARNING) << "Missing 'VersionedConfig' field in JSON.";
    return false;
  }
 
  const std::string version = version_info::GetMajorVersionNumber();
  if (version.empty()) {
    LOG(WARNING) << "Failed to get current version.";
    return false;
  }
  base::Value versioned_config;
  for (const auto& item : *versioned_config_list) {
    if (!item.is_dict()) {
      LOG(WARNING) << "Invalid item in VersionedConfig list.";
      continue;
    }
    const std::string *arkweb_version = item.GetDict().FindString("ArkWebCoreVersion");
    if (arkweb_version && *arkweb_version == version) {
      versioned_config = item.Clone();
      break;
    }
  }
  if (!versioned_config.is_dict() || versioned_config.GetDict().empty()) {
    LOG(WARNING) << "No data matching the version is found in 'VersionedConfig' .";
    return false;
  }
 
  return ProcessFeaturesSwitches(versioned_config, localState);
}

bool OnGlobalConfigResult(const std::string& path, PrefService* localState) {
  if (!path.empty()) {
    kGlobalConfigDataPath = path;
  }
  return ParseFeaturesSwitchesToPrefs(localState);
}
#endif

} // global_config
