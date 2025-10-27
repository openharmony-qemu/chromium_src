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
#if BUILDFLAG(ARKWEB_CRASHPAD)
#include "base/base_switches.h"
#include "base/logging.h"
#endif  // BUILDFLAG(ARKWEB_CRASHPAD)

namespace resource_util {

#if BUILDFLAG(IS_OHOS)
bool GetDefaultUserDataDirectory(base::FilePath* result) {
  if (!base::PathService::Get(base::DIR_OHOS_APP_DATA, result)) {
    return false;
  }
  *result = result->Append(FILE_PATH_LITERAL("cef"));
  *result = result->Append(FILE_PATH_LITERAL("cef_user_data"));
  return true;
}
#endif

void OverrideUserDataDirExt(const base::FilePath& user_data_path,
                            const base::CommandLine* command_line) {
  base::PathService::Override(base::DIR_CACHE,
                              user_data_path.Append("cache/web"));
  base::PathService::Override(base::DIR_OHOS_APP_DATA, user_data_path);
#if BUILDFLAG(ARKWEB_CRASHPAD)
  // log path need to get by interface
  base::PathService::Override(base::DIR_OHOS_CRASHPAD,
                              user_data_path.Append("../log/crashpad"));
  base::FilePath bundle_dir;
  if (command_line->HasSwitch(switches::kBundleInstallationDir)) {
    bundle_dir =
        command_line->GetSwitchValuePath(switches::kBundleInstallationDir);
  }
  if (!bundle_dir.empty()) {
    base::PathService::Override(base::DIR_OHOS_APP_INSTALLATION, bundle_dir);
  } else {
    LOG(ERROR)
        << "crashpad OverrideUserDataDir, get Bundle installation dir failed";
  }
#endif  // BUILDFLAG(ARKWEB_CRASHPAD)
}

}  // namespace resource_util
