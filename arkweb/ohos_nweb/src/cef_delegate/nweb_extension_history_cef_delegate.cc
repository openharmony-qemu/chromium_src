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

#include "nweb_extension_history_cef_delegate.h"

#include <atomic>
#include <mutex>

#include "base/logging.h"
#include "cef/libcef/browser/request_context_impl.h"
#include "cef/ohos_cef_ext/libcef/browser/extensions/api/history/cef_history_event_router.h"
#include "content/public/browser/browser_context.h"
#include "nweb_extension_utils.h"

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "ohos_nweb_ex/core/extension/nweb_extension_history_dispatcher.h"
#endif

namespace OHOS::NWeb {
namespace {
  static std::map<int, HistoryGetVisitsCallback> g_history_get_visits_map_;
  std::mutex g_history_get_visits_map_mutex_;

  static std::map<int, HistorySearchCallback> g_history_search_map_;
  std::mutex g_history_search_map_mutex_;

  static std::map<int, HistoryAddUrlCallback> g_history_add_url_map_;
  std::mutex g_history_add_url_map_mutex_;

  static std::map<int, HistoryDeleteUrlCallback> g_history_delete_url_map_;
  std::mutex g_history_delete_url_map_mutex_;

  static std::map<int, HistoryDeleteAllCallback> g_history_delete_all_map_;
  std::mutex g_history_delete_all_map_mutex_;

  static std::map<int, HistoryDeleteRangeCallback> g_history_delete_range_map_;
  std::mutex g_history_delete_range_map_mutex_;
}

// static
NWebExtensionHistoryCefDelegate* NWebExtensionHistoryCefDelegate::GetInstance() {
  static NWebExtensionHistoryCefDelegate instance;
  return &instance;
}

bool NWebExtensionHistoryCefDelegate::GetVisits(
    const std::string& url,
    HistoryGetVisitsCallback callback) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  static std::atomic<int> requestId(0);
  int currentRequestId;
  {
    std::lock_guard<std::mutex> lock(g_history_get_visits_map_mutex_);
    currentRequestId = ++requestId;
    g_history_get_visits_map_[currentRequestId] = std::move(callback);
  }
  bool result = NWebExtensionHistoryDispatcher::GetInstance().GetVisits(
      currentRequestId, url);
  if (!result) {
    std::lock_guard<std::mutex> lock(g_history_get_visits_map_mutex_);
    g_history_get_visits_map_.erase(currentRequestId);
  }
  return result;
#else
  return false;
#endif
}

NO_SANITIZE("cfi-icall")
bool NWebExtensionHistoryCefDelegate::Search(const NWebExtensionHistoryQueryInfo* data,
                                             HistorySearchCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static std::atomic<int> requestId(0);
  int currentRequestId;
  {
    std::lock_guard<std::mutex> lock(g_history_search_map_mutex_);
    currentRequestId = ++requestId;
    g_history_search_map_[currentRequestId] = std::move(callback);
  }
  bool result = NWebExtensionHistoryDispatcher::GetInstance().Search(
      currentRequestId, data);
  if (!result) {
    std::lock_guard<std::mutex> lock(g_history_search_map_mutex_);
    g_history_search_map_.erase(currentRequestId);
  }
  return result;
#endif
}

bool NWebExtensionHistoryCefDelegate::AddUrl(const char* url,
                                             HistoryAddUrlCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static std::atomic<int> requestId(0);
  int currentRequestId;
  {
    std::lock_guard<std::mutex> lock(g_history_add_url_map_mutex_);
    currentRequestId = ++requestId;
    g_history_add_url_map_[currentRequestId] = std::move(callback);
  }
  bool result = NWebExtensionHistoryDispatcher::GetInstance().AddUrl(
      currentRequestId, url);
  if (!result) {
    std::lock_guard<std::mutex> lock(g_history_add_url_map_mutex_);
    g_history_add_url_map_.erase(currentRequestId);
  }
  return result;
#endif
}

bool NWebExtensionHistoryCefDelegate::DeleteAll(HistoryDeleteAllCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static std::atomic<int> requestId(0);
  int currentRequestId;
  {
    std::lock_guard<std::mutex> lock(g_history_delete_all_map_mutex_);
    currentRequestId = ++requestId;
    g_history_delete_all_map_[currentRequestId] = std::move(callback);
  }
  bool result = NWebExtensionHistoryDispatcher::GetInstance().DeleteAll(
      currentRequestId);
  if (!result) {
    std::lock_guard<std::mutex> lock(g_history_delete_all_map_mutex_);
    g_history_delete_all_map_.erase(currentRequestId);
  }
  return result;
#endif
}

bool NWebExtensionHistoryCefDelegate::DeleteUrl(const char* url,
                                             HistoryDeleteUrlCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static std::atomic<int> requestId(0);
  int currentRequestId;
  {
    std::lock_guard<std::mutex> lock(g_history_delete_url_map_mutex_);
    currentRequestId = ++requestId;
    g_history_delete_url_map_[currentRequestId] = std::move(callback);
  }
  bool result = NWebExtensionHistoryDispatcher::GetInstance().DeleteUrl(
      currentRequestId, url);
  if (!result) {
    std::lock_guard<std::mutex> lock(g_history_delete_url_map_mutex_);
    g_history_delete_url_map_.erase(currentRequestId);
  }
  return result;
#endif
}

bool NWebExtensionHistoryCefDelegate::DeleteRange(
    int64_t start_time,
    int64_t end_time,
    HistoryDeleteRangeCallback callback) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  static std::atomic<int> requestId(0);
  int currentRequestId;
  {
    std::lock_guard<std::mutex> lock(g_history_delete_range_map_mutex_);
    currentRequestId = ++requestId;
    g_history_delete_range_map_[currentRequestId] = std::move(callback);
  }
  bool result = NWebExtensionHistoryDispatcher::GetInstance().DeleteRange(
      currentRequestId, start_time, end_time);
  if (!result) {
    std::lock_guard<std::mutex> lock(g_history_delete_range_map_mutex_);
    g_history_delete_range_map_.erase(currentRequestId);
  }
  return result;
#else
  return false;
#endif
}

void NWebExtensionHistoryCefDelegate::GetVisitsCallback(
    int requestId,
    const std::string& error,
    const std::vector<NWebExtensionVisitItem>& results) {
  std::lock_guard<std::mutex> lock(g_history_get_visits_map_mutex_);
  if (g_history_get_visits_map_.count(requestId)) {
    std::move(g_history_get_visits_map_[requestId]).Run(error, results);
    g_history_get_visits_map_.erase(requestId);
  }
}

void NWebExtensionHistoryCefDelegate::SearchCallback(
  const NWebExtensionHistoryItems* items) {
  if (!items) {
    return;
  }

  std::lock_guard<std::mutex> lock(g_history_search_map_mutex_);
  if (g_history_search_map_.count(items->requestId)) {
    std::move(g_history_search_map_[items->requestId]).Run(items);
    g_history_search_map_.erase(items->requestId);
  }
}

void NWebExtensionHistoryCefDelegate::AddUrlCallback(int requestId, const char* error) {
  std::lock_guard<std::mutex> lock(g_history_add_url_map_mutex_);
  if (g_history_add_url_map_.count(requestId)) {
    std::move(g_history_add_url_map_[requestId]).Run(error);
    g_history_add_url_map_.erase(requestId);
  }
}

void NWebExtensionHistoryCefDelegate::DeleteUrlCallback(int requestId, const char* error) {
  std::lock_guard<std::mutex> lock(g_history_delete_url_map_mutex_);
  if (g_history_delete_url_map_.count(requestId)) {
    std::move(g_history_delete_url_map_[requestId]).Run(error);
    g_history_delete_url_map_.erase(requestId);
  }
}

void NWebExtensionHistoryCefDelegate::DeleteAllCallback(int requestId, const char* error) {
  std::lock_guard<std::mutex> lock(g_history_delete_all_map_mutex_);
  if (g_history_delete_all_map_.count(requestId)) {
    std::move(g_history_delete_all_map_[requestId]).Run(error);
    g_history_delete_all_map_.erase(requestId);
  }
}

void NWebExtensionHistoryCefDelegate::DeleteRangeCallback(
    int requestId,
    const std::string& error) {
  std::lock_guard<std::mutex> lock(g_history_delete_range_map_mutex_);
  if (g_history_delete_range_map_.count(requestId)) {
    std::move(g_history_delete_range_map_[requestId]).Run(error);
    g_history_delete_range_map_.erase(requestId);
  }
}

void NWebExtensionHistoryCefDelegate::OnVisited(const NWebExtensionHistoryItem* item) {
  extensions::CefHistoryEventRouter::GetInstance()
      ->DispatchHistoryVisitedEvent(GetBrowserContext(), item);
}

void NWebExtensionHistoryCefDelegate::OnVisitRemoved(const NWebExtensionHistoryVisiteRemovedItem* item) {
  extensions::CefHistoryEventRouter::GetInstance()
      ->DispatchHistoryVisitRemovedEvent(GetBrowserContext(), item);
}
} // namespace OHOS::NWeb
