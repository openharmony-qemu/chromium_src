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

#include "cef_history_event_router.h"

#include "base/containers/contains.h"
#include "chrome/common/extensions/api/history.h"
#include "extensions/browser/event_router.h"
#include "extensions/common/features/feature.h"
#include "libcef/browser/extensions/window_extensions_util.h"

namespace extensions {

namespace {
api::history::HistoryItem GetHistoryItem(const NWebExtensionHistoryItem* item) {
  api::history::HistoryItem history_item;
  if (!item) {
    return history_item;
  }

  if (item->id) {
    history_item.id = std::string(item->id);
  }

  if (item->url) {
    history_item.url = std::string(item->url);
  }

  if (item->title) {
    history_item.title = std::string(item->title);
  }

  if (item->lastVisitTime) {
    history_item.last_visit_time = *(item->lastVisitTime);
  }

  if (item->typedCount) {
    history_item.typed_count = *(item->typedCount);
  }

  if (item->visitCount) {
    history_item.visit_count = *(item->visitCount);
  }

  return history_item;
}

base::Value::List CreateNWebExtensionHistoryVisitedList(
    const NWebExtensionHistoryItem* item) {
  if (!item) {
    return base::Value::List();
  }

  if (item->id) {
    LOG(DEBUG) << "CreateNWebExtensionHistoryVisitedList, id:" << item->id;
  }

  if (item->title) {
    LOG(DEBUG) << "CreateNWebExtensionHistoryVisitedList, title:"
               << item->title;
  }

  if (item->url) {
    LOG(DEBUG) << "CreateNWebExtensionHistoryVisitedList, url:" << item->url;
  }

  if (item->lastVisitTime) {
    LOG(DEBUG) << "CreateNWebExtensionHistoryVisitedList, "
                  "lastVisitTime:"
               << *(item->lastVisitTime);
  }

  if (item->typedCount) {
    LOG(DEBUG) << "CreateNWebExtensionHistoryVisitedList, "
                  "typedCount:"
               << *(item->typedCount);
  }

  if (item->visitCount) {
    LOG(DEBUG) << "CreateNWebExtensionHistoryVisitedList, "
                  "visitCount:"
               << *(item->visitCount);
  }

  auto args = api::history::OnVisited::Create(GetHistoryItem(item));
  return args;
}

base::Value::List CreateNWebExtensionHistoryVisitRemovedList(
    const NWebExtensionHistoryVisiteRemovedItem* item) {
  if (!item) {
    return base::Value::List();
  }

  api::history::OnVisitRemoved::Removed removed;
  removed.all_history = *(item->allHistory);

  removed.urls.emplace();
  for (uint32_t i = 0; i < item->count; i++) {
    LOG(DEBUG) << "CreateNWebExtensionHistoryVisitRemovedList, "
                  "item->urls["
               << i << "]:" << item->urls[i]
               << ", allHistory:" << *(item->allHistory)
               << ", count:" << item->count;

    removed.urls->push_back(item->urls[i]);
  }

  auto args = api::history::OnVisitRemoved::Create(removed);
  return args;
}
}  // namespace

// static
CefHistoryEventRouter* CefHistoryEventRouter::GetInstance() {
  static CefHistoryEventRouter instance;
  return &instance;
}

void CefHistoryEventRouter::DispatchHistoryVisitedEvent(
    content::BrowserContext* browser_context,
    const NWebExtensionHistoryItem* item) {
  LOG(DEBUG) << "CefHistoryEventRouter::DispatchHistoryVisitedEvent";
  if (!browser_context) {
    LOG(ERROR) << "browser_context is null";
    return;
  }
  DispatchEvent(events::HISTORY_ON_VISITED, api::history::OnVisited::kEventName,
                CreateNWebExtensionHistoryVisitedList(item), browser_context);
}

void CefHistoryEventRouter::DispatchHistoryVisitRemovedEvent(
    content::BrowserContext* browser_context,
    const NWebExtensionHistoryVisiteRemovedItem* item) {
  LOG(DEBUG) << "CefHistoryEventRouter::DispatchHistoryVisitRemovedEvent";
  if (!browser_context) {
    LOG(ERROR) << "browser_context is null";
    return;
  }
  DispatchEvent(events::HISTORY_ON_VISIT_REMOVED,
                api::history::OnVisitRemoved::kEventName,
                CreateNWebExtensionHistoryVisitRemovedList(item),
                browser_context);
}

void CefHistoryEventRouter::DispatchEvent(
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