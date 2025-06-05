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

#include "ohos/adapter/file_manager/file_manager_adapter.h"

#include <filemanagement/file_uri/error_code.h>
#include <filemanagement/file_uri/oh_file_uri.h>

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
}  // namespace ohos
