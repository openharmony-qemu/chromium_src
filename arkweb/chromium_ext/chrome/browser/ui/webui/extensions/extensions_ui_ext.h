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

#ifndef CHROME_BROWSER_UI_WEBUI_EXTENSIONS_EXTENSIONS_UI_EXT_H_
#define CHROME_BROWSER_UI_WEBUI_EXTENSIONS_EXTENSIONS_UI_EXT_H_

#include "content/public/browser/web_ui_data_source.h"

namespace extensions {

void ArkWebExtensionsWebUIOverrideContentSecurityPolicies(
    content::WebUIDataSource* source);

}  // namespace extensions

#endif  // CHROME_BROWSER_UI_WEBUI_EXTENSIONS_EXTENSIONS_UI_EXT_H_
