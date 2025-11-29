// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_FILE_MANAGER_FILE_MANAGER_ADAPTER_H_
#define OHOS_ADAPTER_FILE_MANAGER_FILE_MANAGER_ADAPTER_H_

#include <functional>
#include <optional>
#include <string>
#include <vector>

#include "ohos/adapter/export.h"

namespace ohos::adapter {

class ADAPTER_EXPORT_API FileManagerAdapter {
 public:
  static FileManagerAdapter& GetInstance();

  FileManagerAdapter(const FileManagerAdapter&) = delete;
  FileManagerAdapter& operator=(const FileManagerAdapter&) = delete;

  ~FileManagerAdapter() = default;

  void OpenItemInFolder(const std::string& full_path);
  void OpenVerifiedItem(const std::string& full_path);
  void GetPathForUri(const char* uri, std::string& path);
  using FileIconCallback =
    std::function<void(std::optional<std::vector<uint8_t>>)>;
  std::string GetFileTypeIdByFileExtension(const std::string& file_extension_);
  void GetFileIconByFileTypeId(const std::string& file_type_id_,
                               FileIconCallback callback);
  void GetUriForPath(const char* path, std::string& uri);

 private:
  FileManagerAdapter() = default;
};

}  // namespace ohos::adapter

#endif  // OHOS_ADAPTER_FILE_MANAGER_FILE_MANAGER_ADAPTER_H_
