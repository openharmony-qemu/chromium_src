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

#ifndef NWEB_EXTENSION_UTILS_H_
#define NWEB_EXTENSION_UTILS_H_

#include <optional>

#include "content/public/browser/browser_context.h"
#include "extensions/common/extension.h"

struct NWebExtensionFunctionContext;
namespace OHOS::NWeb {

content::BrowserContext* GetBrowserContext();

const extensions::Extension* FindExtensionById(
    content::BrowserContext* browser_context,
    const std::string& extension_id);

std::optional<std::string> GetExtensionContextType(
    content::BrowserContext* browser_context);

std::optional<bool> GetIncludeIncognitoInformation(
    const std::string& extension_id,
    content::BrowserContext* browser_context);

content::BrowserContext* GetIncognitoContext(
    content::BrowserContext* browser_context);

std::optional<NWebExtensionFunctionContext> GetExtensionFunctionContext(
    const std::string& extension_id,
    content::BrowserContext* browser_context,
    std::optional<bool> include_incognito_info = std::nullopt);

}  // namespace OHOS::NWeb

#endif  // NWEB_EXTENSION_UTILS_H_
