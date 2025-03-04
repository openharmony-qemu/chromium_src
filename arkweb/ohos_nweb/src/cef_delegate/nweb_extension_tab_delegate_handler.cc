/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "nweb_extension_tab_delegate_hander.h"
#include "base/logging.h"

namespace OHOS::NWeb {

namespace {
std::shared_ptr<NWebExtensionApiCallback> g_extension_api_listener = nullptr;
}

std::unique_ptr<NwebExtensionTabDelegateHandler> NwebExtensionTabDelegateHandler::instance = nullptr;
std::mutex NwebExtensionTabDelegateHandler::mtx;

void NwebExtensionTabDelegateHandler::RegisterWebExtensionTabApiListener(
    std::shared_ptr<NWebExtensionApiCallback> callback) {
  LOG(INFO)
      << "NwebExtensionTabDelegateHandler::RegisterWebExtensionTabApiListener";
  g_extension_api_listener = callback;
}

void NwebExtensionTabDelegateHandler::UnRegisterWebExtensionTabApiListener() {
  LOG(INFO)
      << "NwebExtensionTabDelegateHandler::UnRegisterWebExtensionTabApiListener";
  g_extension_api_listener = nullptr;
}

NwebExtensionTabDelegateHandler* NwebExtensionTabDelegateHandler::GetInstance() {
  std::lock_guard<std::mutex> lock(mtx);
  if (!instance) {
    instance = std::make_unique<NwebExtensionTabDelegateHandler>();
  }
  return instance.get();
}

NO_SANITIZE("cfi-icall")
std::unique_ptr<NWebExtensionTab> NwebExtensionTabDelegateHandler::GetTab(
    int tab_id) {
  LOG(DEBUG) << "NwebExtensionTabDelegateHandler::GetTab";
  if (!g_extension_api_listener || !g_extension_api_listener->NotifyGet) {
    LOG(ERROR) << "extension api listener is null";
    return nullptr;
  }
  return g_extension_api_listener->NotifyGet(tab_id);
}

NO_SANITIZE("cfi-icall")
std::vector<NWebExtensionTab> NwebExtensionTabDelegateHandler::QueryTab(
    const NWebExtensionTabQueryInfo& queryInfo) {
  LOG(DEBUG) << "NwebExtensionTabDelegateHandler::QueryTab";
  std::vector<NWebExtensionTab> tabs;
  if (!g_extension_api_listener || !g_extension_api_listener->NotifyQuery) {
    LOG(ERROR) << "extension api listener is null";
    return tabs;
  }
  g_extension_api_listener->NotifyQuery(queryInfo, tabs);
  return tabs;
}

}  // namespace OHOS::NWeb