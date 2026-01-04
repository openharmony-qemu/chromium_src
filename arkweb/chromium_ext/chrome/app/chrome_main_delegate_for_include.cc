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

static bool SetUserDataDirForArkweb(base::CommandLine* command_line) {
  bool cache_web_exists = base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kCacheDirExists);
  bool arkweb_exists = base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kUserDataDirExists);
  base::FilePath arkweb_app_base_dir = command_line->GetSwitchValuePath(switches::kArkwebAppDataDir);
  base::FilePath user_data_dir = command_line->GetSwitchValuePath(switches::kUserDataDir);
  if (arkweb_app_base_dir.empty()) {
    LOG(ERROR) << " arkweb_app_data_dir is empty. ";
    return true;
  }

  base::FilePath arkweb_user_data_dir = user_data_dir;
  if (!arkweb_app_base_dir.IsParent(user_data_dir) &&
      arkweb_app_base_dir != user_data_dir) {
    arkweb_user_data_dir = user_data_dir.empty() ?
                    arkweb_app_base_dir.Append("files/__arkweb") :
                    arkweb_app_base_dir.Append(user_data_dir);
  }

  base::FilePath target_user_data_dir;
  if (cache_web_exists && !arkweb_exists) {
    // mix
    target_user_data_dir = base::FilePath("/data/storage/el2/base/cache/web");
  } else {
    // separation
    target_user_data_dir = arkweb_user_data_dir;
  }

  command_line->AppendSwitchPath(switches::kUserDataDir, target_user_data_dir);

  base::PathService::OverrideAndCreateIfNeeded(
    base::DIR_USER_DATA, target_user_data_dir, false, true);

  return !base::PathService::OverrideAndCreateIfNeeded(
            chrome::DIR_USER_DATA, target_user_data_dir, false, true);
}