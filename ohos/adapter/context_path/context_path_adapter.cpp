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

#include "ohos/adapter/context_path/context_path_adapter.h"

#include <functional>
#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/logging.h"

namespace ohos::adapter::ContextPathAdapter {
std::string GetDir(std::string& getDirFuncName) {
  if (auto getDirFunc = ohos::adapter::GetJSFunction(getDirFuncName)) {
    auto path = getDirFunc->Invoke<std::string>();
    return path;
  }
  LOGE("ContextPathAdapter get %{public}s js function failed.",
       getDirFuncName.c_str());
  return "";
}

std::string GetCacheDir() {
  std::string funcName("ContextPathAdapter.GetCacheDir");
  return GetDir(funcName);
}

std::string GetFilesDir() {
  std::string funcName("ContextPathAdapter.GetFilesDir");
  return GetDir(funcName);
}

std::string GetTempDir() {
  std::string funcName("ContextPathAdapter.GetTempDir");
  return GetDir(funcName);
}

std::string GetUserDocumentDir() {
  std::string funcName("ContextPathAdapter.GetUserDocumentDir");
  return GetDir(funcName);
}

std::string GetUserDesktopDir() {
  std::string funcName("ContextPathAdapter.GetUserDesktopDir");
  return GetDir(funcName);
}

std::string GetUserDownloadDir() {
  std::string funcName("ContextPathAdapter.GetUserDownloadDir");
  return GetDir(funcName);
}
 
std::string GetAppDownloadDir() {
  std::promise<std::string> promise;
  std::function<void(std::string)> download_dir = [&promise](std::string ret) {
    promise.set_value(ret);
  };
  if (auto getDirFunc = ohos::adapter::GetJSFunction(
      "ContextPathAdapter.GetAppDownloadDir")) {
    getDirFunc->Invoke<void>(download_dir);
    return promise.get_future().get();
  }
  LOGE(
      "ContextPathAdapter get ContextPathAdapter.GetAppDownloadDir js "
      "function failed.");
  return "";
}
}  // namespace ohos::adapter::ContextPathAdapter
