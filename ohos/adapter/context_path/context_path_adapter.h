// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_CONTEXT_PATH_CONTEXT_PATH_ADAPTER_H_
#define OHOS_ADAPTER_CONTEXT_PATH_CONTEXT_PATH_ADAPTER_H_

#include "ohos/adapter/export.h"

#include <string>

namespace ohos::adapter::ContextPathAdapter {

struct ContextPaths {
  std::string cache_dir;
  std::string files_dir;
  std::string temp_dir;
  std::string user_document_dir;
  std::string user_desktop_dir;
  std::string user_download_dir;
};

void ADAPTER_EXPORT_API
SetContextPaths(const ContextPaths& paths);
std::string ADAPTER_EXPORT_API GetCacheDir();
std::string ADAPTER_EXPORT_API GetTempDir();
std::string ADAPTER_EXPORT_API GetFilesDir();
std::string ADAPTER_EXPORT_API GetUserDocumentDir();
std::string ADAPTER_EXPORT_API GetUserDesktopDir();
std::string ADAPTER_EXPORT_API GetUserDownloadDir();
std::string ADAPTER_EXPORT_API GetAppDownloadDir();

inline constexpr char kDefaultTempDir[] = "/data/storage/el2/base/temp";
}  // namespace ohos::adapter::ContextPathAdapter

#endif  // OHOS_ADAPTER_CONTEXT_PATH_CONTEXT_PATH_ADAPTER_H_
