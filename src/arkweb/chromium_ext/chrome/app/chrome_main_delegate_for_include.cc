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

static bool SetUserDataDirForArkweb(const base::FilePath& user_data_dir) {
  static const std::string web_data_tag("cache/web");

  base::FilePath cache_web_data_dir;
  if (!user_data_dir.empty() &&
      user_data_dir.MaybeAsASCII().find(web_data_tag) == std::string::npos) {
    cache_web_data_dir = user_data_dir.Append(web_data_tag);
  }

  const bool cache_web_directory_valid =
      !cache_web_data_dir.empty() &&
      base::PathService::OverrideAndCreateIfNeeded(
          chrome::DIR_USER_DATA, cache_web_data_dir, false, true);

  bool specified_directory_was_invalid = false;
  if (!cache_web_directory_valid) {
    specified_directory_was_invalid =
        !user_data_dir.empty() &&
        !base::PathService::OverrideAndCreateIfNeeded(
            chrome::DIR_USER_DATA, user_data_dir, false, true);
  }

  return specified_directory_was_invalid;
}