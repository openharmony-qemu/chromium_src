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

#include "nweb_extension_side_panel_cef_delegate.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "content/public/common/content_switches.h"

namespace OHOS::NWeb {

namespace {
std::shared_ptr<NWebExtensionSidePanelApiCallback> g_extension_api_listener =
    nullptr;
}

// static
void NWebExtensionSidePanelCefDelegate::RegisterWebExtensionApiListener(
    std::shared_ptr<NWebExtensionSidePanelApiCallback>
        web_extension_api_listener) {
  LOG(DEBUG)
      << "NWebExtensionSidePanelCefDelegate::RegisterWebExtensionApiListener";
  g_extension_api_listener = web_extension_api_listener;
}

// static
void NWebExtensionSidePanelCefDelegate::UnRegisterWebExtensionApiListener() {
  LOG(DEBUG)
      << "NWebExtensionSidePanelCefDelegate::UnRegisterWebExtensionApiListener";
  g_extension_api_listener = nullptr;
}

// static
NO_SANITIZE("cfi-icall")
void NWebExtensionSidePanelCefDelegate::OnSetOptions(
    std::string extension_id,
    std::optional<bool> enabled,
    std::optional<int> tab_id,
    std::optional<std::string> path) {
  if (!g_extension_api_listener) {
    LOG(ERROR) << "No web extension side panel api listener";
    return;
  }

  if (!(g_extension_api_listener->OnSidePanelSetOptions)) {
    LOG(ERROR) << "No OnSidePanelSetOptions callback";
    return;
  }

  LOG(DEBUG) << "OnSetOptions extension ID:" << extension_id;
  g_extension_api_listener->OnSidePanelSetOptions(extension_id, enabled,
                                                  tab_id, path);
}

// static
NO_SANITIZE("cfi-icall")
void NWebExtensionSidePanelCefDelegate::OnSetPanelBehavior(
    std::string extension_id,
    bool open_panel_on_action_click) {
  if (!g_extension_api_listener) {
    LOG(ERROR) << "No web extension side panel api listener";
    return;
  }

  if (!(g_extension_api_listener->OnSidePanelSetPanelBehavior)) {
    LOG(ERROR) << "No OnSidePanelSetPanelBehavior callback";
    return;
  }

  LOG(DEBUG) << "OnSetPanelBehavior extension ID:" << extension_id;
  g_extension_api_listener->OnSidePanelSetPanelBehavior(
      extension_id.c_str(), open_panel_on_action_click);
}

NO_SANITIZE("cfi-icall")
void NWebExtensionSidePanelCefDelegate::OnOpen(std::string extension_id,
                                               int tab_id, int window_id) {
  if (!g_extension_api_listener) {
    LOG(ERROR) << "No web extension side panel api listener";
    return;
  }

  if (!(g_extension_api_listener->OnSidePanelOpen)) {
    LOG(ERROR) << "No OnSidePanelOpen callback";
    return;
  }

  LOG(DEBUG) << "OnOpen extension ID:" << extension_id;
  g_extension_api_listener->OnSidePanelOpen(
      extension_id.c_str(), tab_id, window_id);
}

}  // namespace OHOS::NWeb
