/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
 
#include "nweb_extension_window_delegate_handler.h"
 
#include "base/logging.h"
 
namespace OHOS::NWeb {
namespace {
  std::shared_ptr<NWebExtensionWindowsApiCallback> g_extension_windows_api_listener = nullptr;
}
 
std::unique_ptr<NweExtensionWindowDelegateHandler> NweExtensionWindowDelegateHandler::instance = nullptr;
std::mutex NweExtensionWindowDelegateHandler::mtx;
 
// static
NweExtensionWindowDelegateHandler* NweExtensionWindowDelegateHandler::GetInstance() {
  std::lock_guard<std::mutex> lock(mtx);
  if (!instance) {
    instance = std::make_unique<NweExtensionWindowDelegateHandler>();
  }
  return instance.get();
}
 
// static
void NweExtensionWindowDelegateHandler::RegisterWebExtensionWindowsApiListener(
      std::shared_ptr<NWebExtensionWindowsApiCallback> web_extension_windows_api_callback) {
  LOG(INFO) << "NweExtensionWindowDelegateHandler::RegisterWebExtensionWindowsApiListener";
  g_extension_windows_api_listener = web_extension_windows_api_callback;
}
 
// static
void NweExtensionWindowDelegateHandler::UnRegisterWebExtensionWindowsApiListener() {
  LOG(INFO) << "NweExtensionWindowDelegateHandler::UnRegisterWebExtensionWindowsApiListener";
  g_extension_windows_api_listener = nullptr;
}
 
std::vector<WebExtensionWindow> NweExtensionWindowDelegateHandler::OnGetAllWindows(
      const WebExtensionWindowQueryOptions& queryOptions) {
  LOG(DEBUG) << "NweExtensionWindowDelegateHandler::OnGetAllWindows";
  std::vector<WebExtensionWindow> windows;
  if (!g_extension_windows_api_listener) {
    LOG(ERROR) << "extension windows api listener is null";
    return windows;
  }
  g_extension_windows_api_listener->OnGetAllWindows(queryOptions, windows);
  return windows;
}

} // namespace OHOS::NWeb