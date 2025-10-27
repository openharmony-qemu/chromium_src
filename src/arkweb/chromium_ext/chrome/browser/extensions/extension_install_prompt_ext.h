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

#ifndef ARKWEB_CHROMIUM_EXT_CHROME_BROWSER_EXTENSIONS_EXTENSION_INSTALL_PROMPT_EXT_H_
#define ARKWEB_CHROMIUM_EXT_CHROME_BROWSER_EXTENSIONS_EXTENSION_INSTALL_PROMPT_EXT_H_

#include "chrome/browser/extensions/extension_install_prompt.h"

class ExtensionInstallPromptShowParams;

namespace ohos {

void ShowExtensionInstallDialogImpl(
    std::unique_ptr<ExtensionInstallPromptShowParams> show_params,
    ExtensionInstallPrompt::DoneCallback done_callback,
    std::unique_ptr<ExtensionInstallPrompt::Prompt> prompt);

} // namespace ohos

#endif  // ARKWEB_CHROMIUM_EXT_CHROME_BROWSER_EXTENSIONS_EXTENSION_INSTALL_PROMPT_EXT_H_
