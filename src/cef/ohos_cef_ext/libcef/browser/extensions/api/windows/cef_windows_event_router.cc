/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "cef_windows_event_router.h"

#include "base/containers/contains.h"
#include "chrome/browser/extensions/api/tabs/tabs_constants.h"
#include "chrome/common/extensions/api/windows.h"
#include "extensions/browser/event_router.h"
#include "extensions/common/features/feature.h"
#include "extensions/common/mojom/context_type.mojom.h"
#include "libcef/browser/extensions/window_extensions_util.h"
#include "cef/ohos_cef_ext/libcef/browser/chrome/extensions/arkweb_chrome_extension_util_ext.h"

namespace extensions {

namespace {

constexpr char kWindowTypesKey[] = "windowTypes";

bool WillDispatchWindowEvent(
    const std::string& window_type,
    content::BrowserContext* browser_context,
    mojom::ContextType target_context,
    const Extension* extension,
    const base::Value::Dict* listener_filter,
    std::optional<base::Value::List>& event_args_out,
    mojom::EventFilteringInfoPtr& event_filtering_info_out) {
  event_filtering_info_out = mojom::EventFilteringInfo::New();
  if (listener_filter && listener_filter->contains(kWindowTypesKey)) {
    event_filtering_info_out->window_type = window_type;
  } else {
    event_filtering_info_out->window_exposed_by_default = true;
    event_filtering_info_out->has_window_exposed_by_default = true;
  }
  return true;
}
}

//static
CefWindowsEventRouter* CefWindowsEventRouter::GetInstance() {
  static CefWindowsEventRouter instance;
  return &instance;
}

void CefWindowsEventRouter::DispatchWindowCreatedEvent(content::BrowserContext* browser_context,
                                                      const WebExtensionWindow& window) {
  LOG(DEBUG) << "CefWindowsEventRouter::DispatchWindowCreatedEvent";
  if (!browser_context) {
    LOG(ERROR) << "browser_context is null";
    return;
  }
  base::Value::List args;
  std::vector<ExtensionTabUtil::ScrubTabBehavior> scrub_tab_behaviors;
  constexpr mojom::ContextType context_type = mojom::ContextType::kUnspecified;
  for (NWebExtensionTab tab : window.tabs) {
    GURL gurl(tab.url.value_or(""));
    ExtensionTabUtil::ScrubTabBehavior scrub_tab_behavior =
        ExtensionTabUtil::GetScrubTabBehaviorExt(nullptr, context_type, gurl, tab.id.value_or(-1));
    scrub_tab_behaviors.emplace_back(scrub_tab_behavior);
  }
  args.Append(GetWindowValue(window, scrub_tab_behaviors));
  auto event = std::make_unique<Event>(events::WINDOWS_ON_CREATED,
                                       api::windows::OnCreated::kEventName,
                                       std::move(args),
                                       browser_context);
  event->will_dispatch_callback = base::BindRepeating(&WillDispatchWindowEvent, *window.type);
  EventRouter::Get(browser_context)->BroadcastEvent(std::move(event));
}

void CefWindowsEventRouter::DispatchWindowRemovedEvent(content::BrowserContext* browser_context,
                                                      const WebExtensionWindow& window) {
  LOG(DEBUG) << "CefWindowsEventRouter::DispatchWindowRemovedEvent";
  if (!browser_context) {
    LOG(ERROR) << "browser_context is null";
    return;
  }
  base::Value::List args;
  args.Append(*window.id);
  auto event = std::make_unique<Event>(events::WINDOWS_ON_REMOVED,
                                       api::windows::OnRemoved::kEventName,
                                       std::move(args),
                                       browser_context);
  event->will_dispatch_callback = base::BindRepeating(&WillDispatchWindowEvent, *window.type);
  EventRouter::Get(browser_context)->BroadcastEvent(std::move(event));
}

void CefWindowsEventRouter::DispatchWindowBoundsChangedEvent(content::BrowserContext* browser_context,
                                                             const WebExtensionWindow& window) {
  LOG(DEBUG) << "CefWindowsEventRouter::DispatchWindowBoundsChangedEvent";
  if (!browser_context) {
    LOG(ERROR) << "browser_context is null";
    return;
  }
  base::Value::List args;
  std::vector<ExtensionTabUtil::ScrubTabBehavior> scrub_tab_behaviors;
  constexpr mojom::ContextType context_type = mojom::ContextType::kUnspecified;
  for (NWebExtensionTab tab : window.tabs) {
    GURL gurl(tab.url.value_or(""));
    ExtensionTabUtil::ScrubTabBehavior scrub_tab_behavior =
        ExtensionTabUtil::GetScrubTabBehaviorExt(nullptr, context_type, gurl, tab.id.value_or(-1));
    scrub_tab_behaviors.emplace_back(scrub_tab_behavior);
  }
  args.Append(GetWindowValue(window, scrub_tab_behaviors));
  auto event = std::make_unique<Event>(events::WINDOWS_ON_BOUNDS_CHANGED,
                                       api::windows::OnBoundsChanged::kEventName,
                                       std::move(args),
                                       browser_context);
  EventRouter::Get(browser_context)->BroadcastEvent(std::move(event));
}

void CefWindowsEventRouter::DispatchWindowFocusChangedEvent(content::BrowserContext* browser_context,
                                                            const WebExtensionWindow& window) {
  LOG(DEBUG) << "CefWindowsEventRouter::DispatchWindowFocusChangedEvent";
  if (!browser_context) {
    LOG(ERROR) << "browser_context is null";
    return;
  }
  base::Value::List args;
  args.Append(*window.id);
  auto event = std::make_unique<Event>(events::WINDOWS_ON_FOCUS_CHANGED,
                                       api::windows::OnFocusChanged::kEventName,
                                       std::move(args),
                                       browser_context);
  event->will_dispatch_callback = base::BindRepeating(&WillDispatchWindowEvent, *window.type);
  EventRouter::Get(browser_context)->BroadcastEvent(std::move(event));
}

}  //  namespace extensions
