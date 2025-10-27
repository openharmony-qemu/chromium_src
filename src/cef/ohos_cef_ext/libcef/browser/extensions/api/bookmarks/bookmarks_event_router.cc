/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ohos_cef_ext/libcef/browser/extensions/api/bookmarks/bookmarks_event_router.h"

#include "chrome/common/extensions/api/bookmarks.h"
#include "ohos_cef_ext/libcef/browser/extensions/api/bookmarks/bookmarks_extensions_util.h"
#include "ohos_nweb/src/capi/browser_service/nweb_extension_bookmarks_types.h"

namespace extensions {

// static
CefBookmarksEventRouter& CefBookmarksEventRouter::GetInstance() {
  static CefBookmarksEventRouter instance;
  return instance;
}

void CefBookmarksEventRouter::DispatchBookmarksCreatedEvent(
    content::BrowserContext* browser_context,
    const char* id,
    const NWebExtensionBookmarkTreeNode* bookmark) {
  LOG(DEBUG) << "CefBookmarksEventRouter::DispatchBookmarksCreatedEvent";
  if (!browser_context) {
    LOG(ERROR) << "browser_context is null";
    return;
  }
  DispatchEvent(
      events::BOOKMARKS_ON_CREATED, api::bookmarks::OnCreated::kEventName,
      CreateNWebExtensionBookmarksOnCreatedList(id, bookmark), browser_context);
}

void CefBookmarksEventRouter::DispatchBookmarksRemovedEvent(
    content::BrowserContext* browser_context,
    const char* id,
    const NWebExtensionBookmarksRemoveInfo* info) {
  LOG(DEBUG) << "CefBookmarksEventRouter::DispatchBookmarksRemovedEvent";
  if (!browser_context) {
    LOG(ERROR) << "browser_context is null";
    return;
  }
  DispatchEvent(
      events::BOOKMARKS_ON_REMOVED, api::bookmarks::OnRemoved::kEventName,
      CreateNWebExtensionBookmarksOnRemovedList(id, info), browser_context);
}

void CefBookmarksEventRouter::DispatchBookmarksChangedEvent(
    content::BrowserContext* browser_context,
    const char* id,
    const NWebExtensionBookmarksChangeInfo* info) {
  LOG(DEBUG) << "CefBookmarksEventRouter::DispatchBookmarksChangedEvent";
  if (!browser_context) {
    LOG(ERROR) << "browser_context is null";
    return;
  }
  DispatchEvent(
      events::BOOKMARKS_ON_CHANGED, api::bookmarks::OnChanged::kEventName,
      CreateNWebExtensionBookmarksOnChangedList(id, info), browser_context);
}

void CefBookmarksEventRouter::DispatchBookmarksMovedEvent(
    content::BrowserContext* browser_context,
    const char* id,
    const NWebExtensionBookmarksMoveInfo* info) {
  LOG(DEBUG) << "CefBookmarksEventRouter::DispatchBookmarksMovedEvent";
  if (!browser_context) {
    LOG(ERROR) << "browser_context is null";
    return;
  }
  DispatchEvent(events::BOOKMARKS_ON_MOVED, api::bookmarks::OnMoved::kEventName,
                CreateNWebExtensionBookmarksOnMovedList(id, info),
                browser_context);
}

void CefBookmarksEventRouter::DispatchBookmarksChildrenReorderedEvent(
    content::BrowserContext* browser_context,
    const char* id,
    const NWebExtensionBookmarksReorderInfo* info) {
  LOG(DEBUG)
      << "CefBookmarksEventRouter::DispatchBookmarksChildrenReorderedEvent";
  if (!browser_context) {
    LOG(ERROR) << "browser_context is null";
    return;
  }
  DispatchEvent(events::BOOKMARKS_ON_CHILDREN_REORDERED,
                api::bookmarks::OnChildrenReordered::kEventName,
                CreateNWebExtensionBookmarksOnChildrenReorderedList(id, info),
                browser_context);
}

void CefBookmarksEventRouter::DispatchBookmarksImportBeganEvent(
    content::BrowserContext* browser_context) {
  LOG(DEBUG) << "CefBookmarksEventRouter::DispatchBookmarksImportBeganEvent";
  if (!browser_context) {
    LOG(ERROR) << "browser_context is null";
    return;
  }
  DispatchEvent(events::BOOKMARKS_ON_IMPORT_BEGAN,
                api::bookmarks::OnImportBegan::kEventName,
                CreateNWebExtensionBookmarksOnImportBeganList(),
                browser_context);
}

void CefBookmarksEventRouter::DispatchBookmarksImportEndedEvent(
    content::BrowserContext* browser_context) {
  LOG(DEBUG) << "CefBookmarksEventRouter::DispatchBookmarksImportEndedEvent";
  if (!browser_context) {
    LOG(ERROR) << "browser_context is null";
    return;
  }
  DispatchEvent(events::BOOKMARKS_ON_IMPORT_ENDED,
                api::bookmarks::OnImportEnded::kEventName,
                CreateNWebExtensionBookmarksOnImportEndedList(),
                browser_context);
}

void CefBookmarksEventRouter::DispatchEvent(
    events::HistogramValue histogram_value,
    const std::string& event_name,
    base::Value::List&& event_args,
    content::BrowserContext* browser_context) {
  EventRouter* event_router = EventRouter::Get(browser_context);
  if (event_router) {
    event_router->BroadcastEvent(std::make_unique<extensions::Event>(
        histogram_value, event_name, std::move(event_args)));
  }
}

}  // namespace extensions
