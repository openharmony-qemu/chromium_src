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

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#include "base/logging.h"
#include "chrome/browser/extensions/extension_tab_util.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_side_panel_cef_delegate.h"
#endif  // ARKWEB_ARKWEB_EXTENSIONS

namespace extensions {

void RunFunctionForInclude(
    raw_ptr<SidePanelSetOptionsFunction> obj,
    std::optional<api::side_panel::SetOptions::Params>& params) {
  LOG(INFO) << "SidePanelSetOptionsFunction::RunFunction";
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  std::optional<std::string> absolute_path;
  std::optional<bool> enabled;
  std::optional<int> tab_id;
  if (params->options.path.has_value()) {
    absolute_path =
        obj->extension()->GetResourceURL(*params->options.path).spec();
  }
  if (params->options.enabled.has_value()) {
    enabled = *params->options.enabled;
  }
  if (params->options.tab_id.has_value()) {
    tab_id = *params->options.tab_id;
  }
#endif

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  OHOS::NWeb::NWebExtensionSidePanelCefDelegate::OnSetOptions(
      obj->extension()->id(), enabled, tab_id, absolute_path);
#endif
}

}  // namespace extensions
