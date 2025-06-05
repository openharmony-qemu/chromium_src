/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// Defines base::PathProviderOHOS which replaces base::PathProviderPosix for
// OHOS in base/path_service.cc.
#include "base/base_paths_ohos.h"

#include "base/base_paths.h"
#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/logging.h"
#include "base/notreached.h"
#include "base/process/process_metrics.h"
#include "ohos/adapter/context_path/context_path_adapter.h"

namespace base {
// TODO: temporary solution to load resources for multiple processes
bool ParseAssetsOHOS(FilePath* result) {
  auto bundle_path =
      base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
          switches::kBundleInstallationDir);
  if (bundle_path.empty()) {
    FilePath bin_dir;
    if (!ReadSymbolicLink(FilePath(kProcSelfExe), &bin_dir)) {
      LOG(ERROR) << "Unable to resolve " << kProcSelfExe << ".";
      return false;
    }
    *result = bin_dir.DirName();
    return true;
  }
  *result = FilePath(bundle_path.c_str());
  return true;
}

bool PathProviderOHOS(int key, FilePath* result) {
  FilePath bin_dir;
  switch (key) {
    case base::FILE_EXE: {
      if (!ReadSymbolicLink(FilePath(kProcSelfExe), &bin_dir)) {
        NOTREACHED() << "Unable to resolve " << kProcSelfExe << ".";
        return false;
      }
      *result = bin_dir;
      return true;
    }
    case base::DIR_MODULE: {
      *result = base::FilePath();
      return true;
    }
    case base::DIR_USER_DESKTOP:
      *result =
          FilePath(::ohos::adapter::ContextPathAdapter::GetUserDesktopDir());
      return !result->empty();
    case base::DIR_CACHE:
      *result = FilePath(::ohos::adapter::ContextPathAdapter::GetCacheDir());
      return !result->empty();
    case base::DIR_TEMP:
      *result = FilePath(::ohos::adapter::ContextPathAdapter::GetTempDir());
      return !result->empty();
    case base::DIR_ASSETS:
      // TODO: temporary solution to load resources for multiple processes
      // resource file packed to system images
      return ParseAssetsOHOS(result);
    case base::DIR_OHOS_APP_DATA:
      *result = FilePath(::ohos::adapter::ContextPathAdapter::GetFilesDir());
      return !result->empty();
    default:
      return false;
  }
}

}  // namespace base
