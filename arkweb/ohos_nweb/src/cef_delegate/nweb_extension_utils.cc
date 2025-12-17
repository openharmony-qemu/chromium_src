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

#include "nweb_extension_utils.h"

#include <optional>

#include "base/logging.h"
#include "cef/libcef/browser/request_context_impl.h"
#include "chrome/browser/profiles/profile.h"
#include "extensions/browser/extension_registry.h"
#include "extensions/browser/extensions_browser_client.h"
#include "extensions/common/extension.h"
#include "ohos_nweb/src/capi/browser_service/nweb_extension_common_types.h"

namespace OHOS::NWeb {

content::BrowserContext* GetBrowserContext() {
  CefRefPtr<CefRequestContext> request_context =
      CefRequestContext::GetGlobalContext();
  if (!request_context) {
    LOG(ERROR) << "request context is null";
    return nullptr;
  }

  CefRequestContextImpl* request_context_impl =
      static_cast<CefRequestContextImpl*>(request_context.get());
  CefBrowserContext* cef_browser_context =
      request_context_impl->GetBrowserContext();
  if (!cef_browser_context) {
    LOG(ERROR) << "cef browser context is null";
    return nullptr;
  }
  content::BrowserContext* browser_context =
      cef_browser_context->AsBrowserContext();
  return browser_context;
}

const extensions::Extension* FindExtensionById(
    content::BrowserContext* browser_context,
    const std::string& extension_id) {
  if (!browser_context) {
    LOG(ERROR) << "null browser_context passed in";
    return nullptr;
  }

  extensions::ExtensionRegistry* registry =
      extensions::ExtensionRegistry::Get(browser_context);
  if (!registry) {
    LOG(ERROR) << "failed to get extension registry";
    return nullptr;
  }

  const extensions::Extension* extension = registry->GetExtensionById(
      extension_id, extensions::ExtensionRegistry::EVERYTHING);
  if (!extension) {
    LOG(ERROR) << "failed to find extension " << extension_id;
  }
  return extension;
}

std::optional<std::string> GetExtensionContextType(
    content::BrowserContext* browser_context) {
  if (!browser_context) {
    return std::nullopt;
  }
  if (browser_context->IsOffTheRecord()) {
    return "INCOGNITO";
  }
  Profile* profile = Profile::FromBrowserContext(browser_context);
  if (!profile) {
    return std::nullopt;
  }
  if (profile->IsRegularProfile()) {
    return "REGULAR";
  }
  return std::nullopt;
}

std::optional<bool> GetIncludeIncognitoInformation(
    const std::string& extension_id,
    content::BrowserContext* browser_context) {
  if (!browser_context) {
    return std::nullopt;
  }
  extensions::ExtensionRegistry* registry =
      extensions::ExtensionRegistry::Get(browser_context);
  if (!registry) {
    LOG(ERROR) << "Failed to get ExtensionRegistry";
    return std::nullopt;
  }
  const extensions::Extension* extension =
      registry->enabled_extensions().GetByID(extension_id);
  if (!extension) {
    LOG(ERROR) << "Extension not found: " << extension_id;
    return std::nullopt;
  }
  return extensions::ExtensionsBrowserClient::Get()->CanExtensionCrossIncognito(
      extension, browser_context);
}

content::BrowserContext* GetIncognitoContext(
    content::BrowserContext* browser_context) {
  if (!browser_context) {
    LOG(ERROR) << "browser context is null";
    return nullptr;
  }

  extensions::ExtensionsBrowserClient* browser_client =
      extensions::ExtensionsBrowserClient::Get();
  if (!browser_client->HasOffTheRecordContext(browser_context)) {
    LOG(ERROR) << "Off-the-record context is not available";
    return nullptr;
  }

  return browser_client->GetOffTheRecordContext(browser_context);
}

std::optional<NWebExtensionFunctionContext> GetExtensionFunctionContext(
    const std::string& extension_id,
    content::BrowserContext* browser_context,
    std::optional<bool> include_incognito_info) {
  NWebExtensionFunctionContext context;
  context.extension_id = extension_id;
  context.context_type = GetExtensionContextType(browser_context);
  context.include_incognito_info =
      include_incognito_info
          ? include_incognito_info
          : GetIncludeIncognitoInformation(extension_id, browser_context);

  return context;
}

}  // namespace OHOS::NWeb
