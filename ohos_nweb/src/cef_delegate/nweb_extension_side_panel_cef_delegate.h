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

#ifndef NWEB_EXTENSION_SIDE_PANEL_CEF_DELEGATE_H
#define NWEB_EXTENSION_SIDE_PANEL_CEF_DELEGATE_H

#include "base/values.h"
#include "ohos_nweb/src/capi/nweb_extension_api_callback.h"

namespace OHOS::NWeb {

class NWebExtensionSidePanelCefDelegate {
 public:
  static void RegisterWebExtensionApiListener(
      std::shared_ptr<NWebExtensionSidePanelApiCallback>
          web_extension_api_listener);
  static void UnRegisterWebExtensionApiListener();
  static void OnSetOptions(
      std::string extension_id,
      std::optional<bool> enabled,
      std::optional<int> tab_id,
      std::optional<std::string> path);
  static void OnSetPanelBehavior(std::string extension_id,
                          bool open_panel_on_action_click);
  static void OnOpen(std::string extension_id, int tab_id, int window_id);
};

}  // namespace OHOS::NWeb

#endif // NWEB_EXTENSION_SIDE_PANEL_CEF_DELEGATE_H
