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

#ifndef NWEB_EXTENSION_SIDE_PANEL_CEF_CONTROLLER_H
#define NWEB_EXTENSION_SIDE_PANEL_CEF_CONTROLLER_H

#include <string>
#include <optional>

namespace OHOS::NWeb {

struct NWebExtensionSidePanelOptions {
  std::optional<bool> enabled;
  std::optional<int32_t> tab_id;
  std::optional<std::string> path;
};

struct NWebExtensionSidePanelBehavior {
  int open_panel_on_action_click;
};

class NWebExtensionSidePanelCefController {
 public:
  static NWebExtensionSidePanelOptions GetOptions(std::string extension_id,
                                                  std::optional<int> tab_id);
  static NWebExtensionSidePanelBehavior GetPanelBehavior(
      std::string extension_id);
};

}  // namespace OHOS::NWeb

#endif // NWEB_EXTENSION_SIDE_PANEL_CEF_CONTROLLER_H
