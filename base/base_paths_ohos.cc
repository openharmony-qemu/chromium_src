// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
