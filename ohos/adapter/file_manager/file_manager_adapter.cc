// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/file_manager/file_manager_adapter.h"

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
  } else {
    LOGE("[FileManagerAdapter] OpenItemInFolder ets function not found");
  }
}

void FileManagerAdapter::OpenVerifiedItem(const std::string& full_path) {
  auto func =
      ohos::adapter::GetJSFunction("FileManagerAdapter.OpenVerifiedItem");
  if (func) {
    func->Invoke<void>(full_path);
  } else {
    LOGE("[FileManagerAdapter] OpenVerifiedItem ets function not found");
  }
}

}  // namespace ohos::adapter
