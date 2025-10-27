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
#ifndef CEF_LIBCEF_BROWSER_EXTENSIONS_API_HISTORY_H_
#define CEF_LIBCEF_BROWSER_EXTENSIONS_API_HISTORY_H_

#include "content/public/browser/browser_context.h"
#include "extensions/browser/event_router.h"
#include "ohos_nweb/src/capi/browser_service/nweb_extension_history_types.h"

namespace extensions {

class CefHistoryEventRouter {
 public:
  static CefHistoryEventRouter* GetInstance();

  void DispatchHistoryVisitedEvent(content::BrowserContext* browser_context,
                                   const NWebExtensionHistoryItem* item);
  void DispatchHistoryVisitRemovedEvent(
      content::BrowserContext* browser_context,
      const NWebExtensionHistoryVisiteRemovedItem* item);

 private:
  CefHistoryEventRouter() = default;
  ~CefHistoryEventRouter() = default;
  CefHistoryEventRouter(const CefHistoryEventRouter&) = delete;
  CefHistoryEventRouter& operator=(const CefHistoryEventRouter&) = delete;

  void DispatchEvent(events::HistogramValue histogram_value,
                     const std::string& event_name,
                     base::Value::List&& event_args,
                     content::BrowserContext* browser_context);
};
}  // namespace extensions
#endif  // CEF_LIBCEF_BROWSER_EXTENSIONS_API_HISTORY_H_