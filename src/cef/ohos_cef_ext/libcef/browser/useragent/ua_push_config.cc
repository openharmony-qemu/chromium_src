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

#include "libcef/browser/useragent/ua_push_config.h"

#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "base/memory/singleton.h"
#include "content/public/browser/browser_thread.h"
#include "ohos_adapter_helper.h"
#include "system_properties_adapter.h"
namespace ohos_user_agent {

UAPushConfig::UAPushConfig()
    : sequenced_task_runner_(base::ThreadPool::CreateSequencedTaskRunner(
          {base::MayBlock(), base::TaskPriority::USER_VISIBLE,
           base::TaskShutdownBehavior::BLOCK_SHUTDOWN})) {}

UAPushConfig* UAPushConfig::GetInstance() {
  return base::Singleton<UAPushConfig>::get();
}
void UAPushConfig::RegisterProfilePrefs(PrefRegistrySimple* registry) {
  registry->RegisterDictionaryPref(kUAPushConfigInfo);
}

void UAPushConfig::Init(PrefService* pref_service) {
  pref_service_ = pref_service;
  ReadConfigInfoFromPrefs();
}
std::optional<OSPositionScheme> UAPushConfig::LoadConfigFromFile(
    const std::string& file_path) {
  LOG(INFO) << kUAPushLogTag << "LoadConfigFromFile begin";
  base::AutoLock locker(lock_);
  // Read entire file content into string buffer
  std::string file_contents;
  const base::FilePath config_file = base::FilePath(file_path);
  if (!base::ReadFileToString(config_file, &file_contents)) {
    return std::nullopt;
  }
  if (file_contents.empty()) {
    return std::nullopt;
  }
  // Parse JSON content into base::Value
  std::optional<base::Value> root = base::JSONReader::Read(file_contents);
  if (!root || !root->is_dict()) {
    return std::nullopt;
  }
  base::Value* os_compatible = root->FindPath(kUAPushConfigOSCompatible);
  if (!os_compatible || !os_compatible->is_list()) {
    return std::nullopt;
  }
  std::string bundle_name = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                .GetSystemPropertiesInstance()
                                .GetBundleName();
  if (bundle_name.empty()) {
    LOG(WARNING) << kUAPushLogTag << "bundle_name is empty";
    return std::nullopt;
  }
  return ParseOSCompatibleData(os_compatible, bundle_name);
}

std::optional<OSPositionScheme> UAPushConfig::ParseOSCompatibleData(
    base::Value* os_compatible,
    const std::string& bundle_name) {
  LOG(INFO) << kUAPushLogTag << "ParseOSCompatibleData begin ";
  OSPositionScheme os_position_str;
  for (const auto& it : os_compatible->GetList()) {
    const base::Value::Dict* dict_val = it.GetIfDict();
    if (!dict_val) {
      return std::nullopt;
    }
    const std::string* os_supplement =
        dict_val->FindString(kUAPushConfigOSSupplement);
    int position = dict_val->FindInt(kUAPushConfigPosition)
                       .value_or(UAPushConfigPositionType::INVALID);
    if (!os_supplement || position == UAPushConfigPositionType::INVALID) {
      return std::nullopt;
    }
    const base::Value::List* trust_list = dict_val->FindList(kUATrustList);
    const base::Value::List* device_type_list =
        dict_val->FindList(kUAPushConfigDeviceType);
    if (!trust_list || trust_list->empty()) {
      return std::nullopt;
    }
    if (!device_type_list || device_type_list->empty()) {
      return std::nullopt;
    }
    std::set<std::string> url_and_device_flag;
    for (const auto& val : *trust_list) {
      const std::string* url = val.GetIfString();
      if (!url || bundle_name != *url || url_and_device_flag.count(*url)) {
        continue;
      }
      url_and_device_flag.insert(*url);
      for (const auto& device_type : *device_type_list) {
        const std::string* device_type_val = device_type.GetIfString();
        if (!device_type_val || url_and_device_flag.count(*device_type_val)) {
          continue;
        }
        url_and_device_flag.insert(*device_type_val);
        auto supplement_dict_val =
            GetSupplement(*device_type_val, *os_supplement);
        UpdateOsPositionStr(os_position_str, std::move(supplement_dict_val),
                            position);
      }
    }
  }
  return os_position_str;
}

void UAPushConfig::UpdateOsPositionStr(OSPositionScheme& os_position_str,
                                       base::Value::Dict dict_val,
                                       int position) {
  if (position == UAPushConfigPositionType::FRONT) {
    os_position_str.front_str.Append(base::Value(std::move(dict_val)));
    return;
  }
  if (position == UAPushConfigPositionType::BACK) {
    os_position_str.back_str.Append(base::Value(std::move(dict_val)));
  }
}
base::Value::Dict UAPushConfig::GetSupplement(
    const std::string& device_type_val,
    const std::string& os_supplement) {
  base::Value::Dict os_supplement_info;
  os_supplement_info.Set(kUAPushConfigDeviceType, device_type_val);
  os_supplement_info.Set(kUAPushConfigOSSupplement, os_supplement);
  return os_supplement_info;
}

void UAPushConfig::DidLoadConfigFromFile(
    std::optional<OSPositionScheme> os_position_str) {
  if (!pref_service_) {
    LOG(ERROR) << kUAPushLogTag
               << "ua push config info save failed for pref_service is null";
    return;
  }
  if (!os_position_str.has_value()) {
    return;
  }
  LOG(INFO) << kUAPushLogTag << "save config info to prefs begin";
  ScopedDictPrefUpdate update(pref_service_, kUAPushConfigInfo);
  base::Value::Dict& storage_dict = update.Get();
  storage_dict.Set(kFrontStr,
                   base::Value(std::move(os_position_str.value().front_str)));
  storage_dict.Set(kBackStr,
                   base::Value(std::move(os_position_str.value().back_str)));
  pref_service_->CommitPendingWrite();
  LOG(INFO) << kUAPushLogTag << "success to save config info to prefs";
}

std::optional<OSPositionPrefsInfo> UAPushConfig::GetLastOsPositionStr(
    const std::string& device_type) {
  return last_os_position_str_map_[device_type];
}

void UAPushConfig::ReadConfigInfoFromPrefs() {
  LOG(INFO) << kUAPushLogTag << "start ReadConfigInfoFromPrefs";
  if (!pref_service_) {
    LOG(ERROR) << kUAPushLogTag
               << "ua push config info read failed for pref_service is null";
    return;
  }
  const base::Value* user_pref_value =
      pref_service_->GetUserPrefValue(kUAPushConfigInfo);
  if (!user_pref_value || !user_pref_value->is_dict()) {
    LOG(ERROR) << kUAPushLogTag
               << "user_pref_value for PrefService: " << kUAPushConfigInfo
               << " is not a dictionary or is not exit";
    return;
  }
  const base::Value::Dict& storage_dict = user_pref_value->GetDict();
  if (storage_dict.size() <= 0UL) {
    LOG(ERROR) << kUAPushLogTag
               << "No storage dict for user_agent.push_config PrefService.";
    return;
  }
  UpdateLastOsPositionStr(kFrontStr, storage_dict);
  UpdateLastOsPositionStr(kBackStr, storage_dict);
}

void UAPushConfig::UpdateLastOsPositionStr(
    const std::string& position,
    const base::Value::Dict& storage_dict) {
  const base::Value::List* supplement_list = storage_dict.FindList(position);
  if (!supplement_list || supplement_list->empty()) {
    LOG(ERROR) << kUAPushLogTag << "No storage os_supplement_list for "
               << position << " PrefService.";
    return;
  }
  for (const auto& supplement : *supplement_list) {
    const auto& supplement_dict = supplement.GetIfDict();
    if (!supplement_dict) {
      continue;
    }
    const std::string* dict_device_type =
        supplement_dict->FindString(kUAPushConfigDeviceType);
    const std::string* dict_supplement =
        supplement_dict->FindString(kUAPushConfigOSSupplement);
    if (!dict_device_type || !dict_supplement) {
      continue;
    }
    auto it = last_os_position_str_map_.find(*dict_device_type);
    if (position == kFrontStr) {
      if (it != last_os_position_str_map_.end()) {
        it->second.front_str = it->second.front_str + *dict_supplement;
      } else {
        last_os_position_str_map_[*dict_device_type] =
            OSPositionPrefsInfo(*dict_supplement, "");
      }
      continue;
    }
    if (position == kBackStr) {
      if (it != last_os_position_str_map_.end()) {
        it->second.back_str = it->second.back_str + *dict_supplement;
      } else {
        last_os_position_str_map_[*dict_device_type] =
            OSPositionPrefsInfo("", *dict_supplement);
      }
      continue;
    }
  }
}

void UAPushConfig::LoadAndParseFileOnThreadPool(const std::string& file_path) {
  if (!content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    content::GetUIThreadTaskRunner({})->PostTask(
        FROM_HERE,
        base::BindOnce(&UAPushConfig::LoadAndParseFileOnThreadPool,
                       base::Unretained(this), std::move(file_path)));
    return;
  }
  sequenced_task_runner_->PostTaskAndReplyWithResult(
      FROM_HERE,
      base::BindOnce(&UAPushConfig::LoadConfigFromFile, base::Unretained(this),
                     file_path),
      base::BindOnce(&UAPushConfig::DidLoadConfigFromFile,
                     base::Unretained(this)));
}

void UpdateUAPushConfigInThread(const std::string& official_config_path) {
  LOG(INFO) << kUAPushLogTag
            << " Received a request to update";
  UAPushConfig::GetInstance()->LoadAndParseFileOnThreadPool(
      official_config_path);
}
void UpdateUAPushConfigRule(const std::string& path) {
  const base::FilePath file_path = base::FilePath(path);
  if (!base::PathExists(file_path) || file_path.empty()) {
    LOG(WARNING) << kUAPushLogTag << " arkweb_push_ua_config file is not valid";
    return;
  }
  UpdateUAPushConfigInThread(path);
}
}  // namespace ohos_user_agent
