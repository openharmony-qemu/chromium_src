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

#include "nweb_extension_side_panel_cef_controller.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "content/public/common/content_switches.h"
#include "chrome/browser/extensions/api/side_panel/side_panel_service.h"
#include "libcef/browser/browser_context.h"
#include "libcef/browser/request_context_impl.h"
#include "libcef/common/app_manager.h"

namespace OHOS::NWeb {

namespace {
content::BrowserContext* GetActiveBrowserContext() {
  auto request_context = static_cast<CefRequestContextImpl*>(
      CefAppManager::Get()->GetGlobalRequestContext().get());
  return request_context->GetBrowserContext()->AsBrowserContext();
}
}
// static
NWebExtensionSidePanelOptions
NWebExtensionSidePanelCefController::GetOptions(std::string extension_id,
                                                std::optional<int> tab_id) {
  NWebExtensionSidePanelOptions result;
  auto browser_context = GetActiveBrowserContext();
  auto registry = extensions::ExtensionRegistry::Get(browser_context);
  scoped_refptr<const extensions::Extension> extension(
      registry->GetExtensionById(extension_id,
                                 extensions::ExtensionRegistry::EVERYTHING));
  DCHECK(extension.get());

  auto service = extensions::SidePanelService::Get(browser_context);
  LOG(INFO) << "GetSidePanelService service pointer:" << service;

  absl::optional<int> in_tab_id;
  if (tab_id.has_value())
    in_tab_id = *tab_id;
  auto options = service->GetOptions(*extension, in_tab_id);
  if (options.enabled.has_value())
    result.enabled = *options.enabled;
  if (options.tab_id.has_value())
    result.tab_id = *options.tab_id;
  if (options.path.has_value()) {
    result.path = extension->GetResourceURL(*options.path).spec();
  }

  return result;
}

// static
NWebExtensionSidePanelBehavior
NWebExtensionSidePanelCefController::GetPanelBehavior(
    std::string extension_id) {
  NWebExtensionSidePanelBehavior result;
  auto browser_context = GetActiveBrowserContext();
  auto service = extensions::SidePanelService::Get(browser_context);
  LOG(INFO) << "GetSidePanelService service pointer:" << service;

  bool openSidePanelOnIconClick =
      service->OpenSidePanelOnIconClick(extension_id);
  result.open_panel_on_action_click = openSidePanelOnIconClick;
  return result;
}
}  // namespace OHOS::NWeb
