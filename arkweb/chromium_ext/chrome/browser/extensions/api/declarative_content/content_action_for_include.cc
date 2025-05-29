/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef CHROME_BROWSER_EXTENSIONS_API_DECLARATIVE_CONTENT_CONTENT_ACTION_H_
#error "must be include form CHROME_BROWSER_EXTENSIONS_API_DECLARATIVE_CONTENT_CONTENT_ACTION_H_"
#endif

#include "base/logging.h"
#include "chrome/browser/extensions/extension_tab_util.h"
#include "extensions/browser/extension_action.h"
#include "extensions/common/extension.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_action_cef_delegate.h"
#include "ui/gfx/image/image.h"

namespace extensions {

namespace {

gfx::Image ContentActionGetActionIcon(const ExtensionAction* action,
                                      int tab_id) {
  if (!action) {
    return gfx::Image();
  }
  gfx::Image icon = action->GetExplicitlySetIcon(tab_id);
  if (!icon.IsEmpty()) {
    return icon;
  }
  icon = action->GetDeclarativeIcon(tab_id);
  if (!icon.IsEmpty()) {
    return icon;
  }
  return action->GetDefaultIconImageV2();
}

void ContentActionShowExtension(ExtensionAction* action,
                                const ContentAction::ApplyInfo& apply_info,
                                bool forced_flag) {
  if (!action || !apply_info.extension) {
    return;
  }
  auto extension = apply_info.extension;
  if (!extension) {
    return;
  }
  auto extension_id = extension->id();
  std::optional<int> tab_id(ExtensionTabUtil::GetTabId(apply_info.tab));
  if (forced_flag) {
    OHOS::NWeb::NWebExtensionActionCefDelegate::GetInstance()->OnEnable(
        extension_id, tab_id);
    return;
  }
  if (action->GetIsVisible(tab_id.value())) {
    OHOS::NWeb::NWebExtensionActionCefDelegate::GetInstance()->OnEnable(
        extension_id, tab_id);
  } else {
    OHOS::NWeb::NWebExtensionActionCefDelegate::GetInstance()->OnDisable(
        extension_id, tab_id);
  }
}

void ContentActionSetIcon(ExtensionAction* action,
                          const ContentAction::ApplyInfo& apply_info) {
  if (!action || !apply_info.extension) {
    return;
  }
  auto extension = apply_info.extension;
  if (!extension) {
    return;
  }
  auto extension_id = extension->id();
  int tab_id = ExtensionTabUtil::GetTabId(apply_info.tab);
  gfx::Image icon = ContentActionGetActionIcon(action, tab_id);
  if (!icon.IsEmpty()) {
    OHOS::NWeb::NWebExtensionActionCefDelegate::OnSetIcon(extension_id, icon,
                                                          tab_id);
  } else {
    LOG(ERROR) << "ContentActionSetIcon: invalid icon for extension "
               << extension_id << " of tab " << tab_id;
  }
}

}  // namespace

}  // namespace extensions
