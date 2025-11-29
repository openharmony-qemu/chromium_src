// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/file_manager/file_manager_adapter.h"

#include <filemanagement/file_uri/error_code.h>
#include <filemanagement/file_uri/oh_file_uri.h>

#include "aki/value/array_buffer.h"
#include "ohos/adapter/aki_hook/aki_hook.h"

namespace ohos::adapter {

FileManagerAdapter& FileManagerAdapter::GetInstance() {
  static FileManagerAdapter instance;
  return instance;
}

void FileManagerAdapter::OpenItemInFolder(const std::string& full_path) {
  auto func =
    ohos::adapter::GetJSFunction("FileManagerAdapter.OpenItemInFolder");
  if (func) {
    func->Invoke<void>(full_path);
  }
}

void FileManagerAdapter::OpenVerifiedItem(const std::string& full_path) {
  auto func =
    ohos::adapter::GetJSFunction("FileManagerAdapter.OpenVerifiedItem");
  if (func) {
    func->Invoke<void>(full_path);
  }
}

void FileManagerAdapter::GetPathForUri(const char* uri, std::string& path)
{
  if (uri == nullptr) {
    LOGE("[FileManagerAdapter] GetPathForUri fail, uri is null");
    return;
  }
  unsigned int uri_length = strlen(uri);
  char* temp = nullptr;
  FileManagement_ErrCode ret =
      OH_FileUri_GetPathFromUri(uri, uri_length, &temp);
  if (ret != ERR_OK) {
    LOGE("[FileManagerAdapter] GetPathForUri fail, ret=%{public}d, uri=%{public}s", ret, uri);
  } else {
    path = temp;
    std::free(temp);
    temp = nullptr;
  }
}

std::string FileManagerAdapter::GetFileTypeIdByFileExtension(
    const std::string& file_extension_) {
  auto func = ohos::adapter::GetJSFunction(
      "FileManagerAdapter.GetFileTypeIdByFileExtension");
  if (func) {
    return func->Invoke<std::string>(file_extension_);
  }
  return "";
}

void FileManagerAdapter::GetFileIconByFileTypeId(
    const std::string& file_type_id_,
    FileIconCallback callback) {
  auto func = ohos::adapter::GetJSFunction(
      "FileManagerAdapter.GetFileIconByFileTypeId");
  if (func) {
    std::function<void(aki::ArrayBuffer)> jsCallback =
        [callback](aki::ArrayBuffer buffer) {
          const uint8_t* data = static_cast<const uint8_t*>(buffer.GetData());
          const int size = buffer.GetLength();
          if (size == 0) {
            callback(std::nullopt);
          } else {
            callback(std::vector<uint8_t>(data, data + size));
          }
        };
    func->Invoke<void>(file_type_id_, jsCallback);
  } else {
    LOGE(
        "[FileManagerAdapter] get FileManagerAdapter.GetFileIconByFileTypeId "
        "js function failed.");
    callback(std::nullopt);
  }
}

void FileManagerAdapter::GetUriForPath(const char* path, std::string& uri) {
  if (path == nullptr) {
    LOGE("[FileManagerAdapter] %{public}s fail, path is null", __FUNCTION__);
    return;
  }
  unsigned int path_length = strlen(path);
  char* temp = nullptr;
  FileManagement_ErrCode ret =
      OH_FileUri_GetUriFromPath(path, path_length, &temp);
  if (ret != ERR_OK) {
    LOGE(
        "[FileManagerAdapter] %{public}s fail, ret=%{public}d, path=%{public}s",
        __FUNCTION__, ret, path);
  } else {
    uri = temp;
    std::free(temp);
    temp = nullptr;
  }
}
}  // namespace ohos::adapter
