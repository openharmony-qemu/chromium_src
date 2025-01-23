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
 
#include "nweb_extension_context_menus_delegate_handler.h"
 
#include "base/logging.h"
 
namespace OHOS::NWeb {
namespace {
  std::shared_ptr<NWebExtensionContextMenusCallback> g_extension_context_menus_listener = nullptr;
}
 
std::unique_ptr<NweExtensionContextMenusDelegateHandler> NweExtensionContextMenusDelegateHandler::instance = nullptr;
std::mutex NweExtensionContextMenusDelegateHandler::mtx;
 
// static
NweExtensionContextMenusDelegateHandler* NweExtensionContextMenusDelegateHandler::GetInstance() {
  std::lock_guard<std::mutex> lock(mtx);
  if (!instance) {
    instance = std::make_unique<NweExtensionContextMenusDelegateHandler>();
  }
  return instance.get();
}
 
// static
void NweExtensionContextMenusDelegateHandler::RegisterExtensionContextMenusListener(
      std::shared_ptr<NWebExtensionContextMenusCallback> extension_context_menus_listener) {
  LOG(INFO) << "NweExtensionContextMenusDelegateHandler::RegisterExtensionContextMenusListener";
  g_extension_context_menus_listener = extension_context_menus_listener;
}
 
// static
void NweExtensionContextMenusDelegateHandler::UnRegisterExtensionContextMenusListener() {
  LOG(INFO) << "NweExtensionContextMenusDelegateHandler::UnRegisterExtensionContextMenusListener";
  g_extension_context_menus_listener = nullptr;
}

NO_SANITIZE("cfi-icall")
void NweExtensionContextMenusDelegateHandler::OnContextMenusCreate(
    const std::string& extension_id,
    const NWebContextMenusItem& menu_item) {
  LOG(DEBUG) << "NweExtensionContextMenusDelegateHandler::OnContextMenusCreate,"
                " extension_id: "
             << extension_id << " menu_item_id: " << menu_item.id;
  if (!g_extension_context_menus_listener) {
    LOG(ERROR) << "extension context menus listener is null";
    return;
  }
  g_extension_context_menus_listener->OnCreated(extension_id.c_str(),
                                                menu_item);
}

NO_SANITIZE("cfi-icall")
void NweExtensionContextMenusDelegateHandler::OnContextMenusUpdate(
    const std::string& extension_id,
    const NWebContextMenusItem& menu_item) {
  LOG(DEBUG) << "NweExtensionContextMenusDelegateHandler::OnContextMenusUpdate,"
                " extension_id: "
             << extension_id << " menu_item_id: " << menu_item.id;
  if (!g_extension_context_menus_listener) {
    LOG(ERROR) << "extension context menus listener is null";
    return;
  }
  g_extension_context_menus_listener->OnUpdated(
      extension_id.c_str(), menu_item.id.c_str(), menu_item);
}

NO_SANITIZE("cfi-icall")
void NweExtensionContextMenusDelegateHandler::OnContextMenusRemove(
    const std::string& extension_id,
    const std::string& menu_item_id) {
  LOG(DEBUG) << "NweExtensionContextMenusDelegateHandler::OnContextMenusRemove,"
                " extension_id: "
             << extension_id << " menu_item_id: " << menu_item_id;
  if (!g_extension_context_menus_listener) {
    LOG(ERROR) << "extension context menus listener is null";
    return;
  }
  g_extension_context_menus_listener->OnRemoved(extension_id.c_str(),
                                                menu_item_id.c_str());
}

NO_SANITIZE("cfi-icall")
void NweExtensionContextMenusDelegateHandler::OnContextMenusRemoveAll(
    const std::string& extension_id) {
  LOG(DEBUG) << "NweExtensionContextMenusDelegateHandler::"
                "OnContextMenusRemoveAll, extension_id: "
             << extension_id;
  if (!g_extension_context_menus_listener) {
    LOG(ERROR) << "extension context menus listener is null";
    return;
  }
  g_extension_context_menus_listener->OnRemovedAll(extension_id.c_str());
}
} // namespace OHOS::NWeb