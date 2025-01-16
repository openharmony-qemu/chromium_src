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
 
#ifndef NWEB_EXTENSION_CONTEXT_MENUS_HANDLER_DELEGATE_H
#define NWEB_EXTENSION_CONTEXT_MENUS_HANDLER_DELEGATE_H
 
#include <string>
#include <mutex>
#include <memory>
 
#include "cef/include/cef_extension_context_menus_handler.h"
#include "ohos_nweb/src/capi/nweb_context_menus_item.h"
#include "ohos_nweb/src/capi/nweb_extension_context_menus_callback.h"
 
namespace OHOS::NWeb {
class NweExtensionContextMenusDelegateHandler : public CefExtensionContextMenusHandler {
  public:
    static NweExtensionContextMenusDelegateHandler* GetInstance();
    static void RegisterExtensionContextMenusListener(
      std::shared_ptr<NWebExtensionContextMenusCallback> extension_context_menus_listener);
    static void UnRegisterExtensionContextMenusListener();
 
    void OnContextMenusCreate(const std::string& extension_id, const NWebContextMenusItem& menu_item) override;
    void OnContextMenusUpdate(const std::string& extension_id, const NWebContextMenusItem& menu_item) override;
    void OnContextMenusRemove(const std::string& extension_id, const std::string& menu_item_id) override;
    void OnContextMenusRemoveAll(const std::string& extension_id) override;
 
  private:
    static std::unique_ptr<NweExtensionContextMenusDelegateHandler> instance;
    static std::mutex mtx;
};
} // namespace OHOS::NWeb
#endif  // NWEB_EXTENSION_CONTEXT_MENUS_HANDLER_DELEGATE_H