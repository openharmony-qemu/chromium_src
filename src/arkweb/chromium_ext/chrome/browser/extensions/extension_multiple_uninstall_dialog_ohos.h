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

#ifndef CHROME_BROWSER_UI_VIEWS_EXTENSIONS_DIALOGS_EXTENSION_MULTIPLE_UNINSTALL_DIALOG_OHOS_H_
#define CHROME_BROWSER_UI_VIEWS_EXTENSIONS_DIALOGS_EXTENSION_MULTIPLE_UNINSTALL_DIALOG_OHOS_H_

#include <vector>

#include "base/functional/callback_helpers.h"
#include "extensions/common/extension_id.h"

namespace ohos {

void ShowExtensionMultipleUninstallDialog(
    const std::vector<extensions::ExtensionId>& extension_ids,
    base::OnceClosure accept_callback,
    base::OnceClosure cancel_callback);

} // namespace ohos

#endif  // CHROME_BROWSER_UI_VIEWS_EXTENSIONS_DIALOGS_EXTENSION_MULTIPLE_UNINSTALL_DIALOG_OHOS_H_
