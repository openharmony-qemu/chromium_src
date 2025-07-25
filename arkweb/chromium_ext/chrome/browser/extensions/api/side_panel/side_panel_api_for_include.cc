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

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "ohos_nweb_ex/core/extension/nweb_extension_side_panel_dispatcher.h"
#endif
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#include "arkweb/ohos_nweb/src/nweb_common.h"
#include "base/logging.h"
#include "chrome/browser/extensions/extension_tab_util.h"
#endif  // ARKWEB_ARKWEB_EXTENSIONS

namespace extensions {

void RunFunctionForInclude(
    raw_ptr<SidePanelSetOptionsFunction> obj,
    std::optional<api::side_panel::SetOptions::Params>& params) {
  LOG(INFO) << "SidePanelSetOptionsFunction::RunFunction";
#if BUILDFLAG(ARKWEB_NWEB_EX)
  std::optional<std::string> absolute_path;
  if (params->options.path.has_value()) {
    absolute_path =
        obj->extension()->GetResourceURL(*params->options.path).spec();
  }
  if (IsNativeApiEnable()) {
    NWebExtensionSidePanelDispatcher::OnSetOptionsNative(
        obj->extension()->id(), params->options.enabled, params->options.tab_id,
        absolute_path);
  } else {
    NWebExtensionSidePanelDispatcher::OnSetOptions(
        obj->extension()->id(), params->options.enabled, params->options.tab_id,
        absolute_path);
  }
#endif
}

}  // namespace extensions
