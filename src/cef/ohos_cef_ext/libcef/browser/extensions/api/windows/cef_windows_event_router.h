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
#ifndef CEF_LIBCEF_BROWSER_EXTENSIONS_API_WINDOWS_H_
#define CEF_LIBCEF_BROWSER_EXTENSIONS_API_WINDOWS_H_

#include "content/public/browser/browser_context.h"
#include "ohos_nweb/src/capi/web_extension_window_items.h"

namespace extensions {

class CefWindowsEventRouter {
  public:
    static CefWindowsEventRouter* GetInstance();

    void DispatchWindowCreatedEvent(content::BrowserContext* browser_context, const WebExtensionWindow& window);
    void DispatchWindowRemovedEvent(content::BrowserContext* browser_context, const WebExtensionWindow& window);
    void DispatchWindowBoundsChangedEvent(content::BrowserContext* browser_context, const WebExtensionWindow& window);
    void DispatchWindowFocusChangedEvent(content::BrowserContext* browser_context, const WebExtensionWindow& window);
};
}
#endif // CEF_LIBCEF_BROWSER_EXTENSIONS_API_WINDOWS_H_