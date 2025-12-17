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

#include "ohos_nweb/src/cef_delegate/nweb_extension_utils.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "ohos_nweb_ex/core/extension/nweb_extension_manager_dispatcher.h"
#endif

namespace extensions {
void LoadErrorReporter::ReportError(const std::u16string& message,
                                    bool be_noisy,
                                    content::BrowserContext* browser_context) {
#if BUILDFLAG(IS_ARKWEB_EXT)
  // NOTE: There won't be a |ui_task_runner_| in the unit test environment.
  CHECK(!ui_task_runner_ || ui_task_runner_->BelongsToCurrentThread())
      << "ReportError can only be called from the UI thread.";

  errors_.push_back(message);

  LOG(WARNING) << "LoadErrorReporter: Extension load error.";

  if (enable_noisy_errors_ && be_noisy) {
    NWebExtensionLoadError error_data;
    error_data.errorMsg = base::UTF16ToUTF8(message);
    error_data.beNoisy = be_noisy;

    std::optional<std::string> type_opt =
        OHOS::NWeb::GetExtensionContextType(browser_context);

    if (type_opt.has_value()) {
      error_data.contextType = type_opt.value();
    } else {
      error_data.contextType = "REGULAR";
    }

    NWebExtensionManagerDispatcher::OnExtensionLoadErrorByPb(error_data);
  }
#endif
}

}  // namespace extensions