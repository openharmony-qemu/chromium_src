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

#include "arkweb/chromium_ext/components/download/public/common/arkweb_download_utils_ext.h"

namespace download {

namespace {
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
static FileRenameOptions gFileRenameOptions =
    FileRenameOptions::RENAME_AND_UNIQUIFY_MODE;
#endif  // BUILDFLAG(ARKWEB_EX_DOWNLOAD)

}  // namespace

#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
void SetFileRenameOptions(FileRenameOptions options) {
  LOG(INFO) << "SetFileRenameOptions to options: " << (int)options;
  gFileRenameOptions = options;
}

FileRenameOptions GetFileRenameOptions() {
  return gFileRenameOptions;
}
#endif  // BUILDFLAG(ARKWEB_EX_DOWNLOAD)
}  // namespace download
