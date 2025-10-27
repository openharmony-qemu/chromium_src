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

#include "chrome/browser/extensions/api/extension_action/extension_action_api.h"
#include "ohos_nweb/src/capi/nweb_extension_action_icon.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_action_cef_delegate.h"

namespace extensions {

ExtensionFunction::ResponseAction ActionOpenPopupFunction::Run() {
  // Unfortunately, the action API types aren't compiled. However, the bindings
  // should still valid the form of the arguments.
  EXTENSION_FUNCTION_VALIDATE(args().size() == 1u);
  EXTENSION_FUNCTION_VALIDATE(extension());
  const base::Value& options = args()[0];

  // Support specifying the tab ID? This is
  // kind of racy (because really what the extension probably cares about is
  // the document ID; tab ID persists across pages, whereas document ID would
  // detect things like navigations).
  std::optional<int32_t> window_id;
  if (options.is_dict()) {
    const base::Value* window_value = options.GetDict().Find("windowId");
    if (window_value) {
      EXTENSION_FUNCTION_VALIDATE(window_value->is_int());
      window_id = window_value->GetInt();
    }
  }

  NWebExtensionActionOpenPopupOptions open_popup_option;
  open_popup_option.windowId = window_id;
  OHOS::NWeb::NWebExtensionActionCefDelegate::GetInstance()->OnOpenPopup(
      extension()->id(), open_popup_option,
      OHOS::NWeb::GetExtensionContextType(browser_context()),
      include_incognito_information());
  return RespondNow(NoArguments());
}

}  // namespace extensions
