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

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "ohos_nweb_ex/core/extension/nweb_extension_history_dispatcher.h"
#endif

namespace OHOS::NWeb {
namespace {
  static std::map<int, HistorySearchCallback> g_history_search_map_;
  std::mutex g_history_search_map_mutex_;

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
}

// static
NWebExtensionHistoryCefDelegate* NWebExtensionHistoryCefDelegate::GetInstance() {
  static NWebExtensionHistoryCefDelegate instance;
  return &instance;
}

NO_SANITIZE("cfi-icall")
bool NWebExtensionHistoryCefDelegate::Search(const NWebExtensionHistoryQueryInfo* data,
                                             HistorySearchCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static int request_id = 0;
  request_id++;

  if (!NWebExtensionHistoryDispatcher::GetInstance().Search(request_id, data)) {
    return false;
  }
  std::lock_guard<std::mutex> lock(g_history_search_map_mutex_);
  g_history_search_map_[request_id] = std::move(callback);
  return true;
#endif
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

bool NWebExtensionHistoryCefDelegate::AddUrl(const char* url) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  if (!NWebExtensionHistoryDispatcher::GetInstance().AddUrl(url)) {
    LOG(ERROR) << "NWebExtensionHistoryCefDelegate::AddUrl fail";
    return false;
  }

  return true;
#endif
}

bool NWebExtensionHistoryCefDelegate::DeleteAll() {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  if (!NWebExtensionHistoryDispatcher::GetInstance().DeleteAll()) {
    LOG(ERROR) << "NWebExtensionHistoryCefDelegate::DeleteAll fail";
    return false;
  }
  return true;
#endif
}

bool NWebExtensionHistoryCefDelegate::DeleteUrl(const char* url) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  if (!NWebExtensionHistoryDispatcher::GetInstance().DeleteUrl(url)) {
    LOG(ERROR) << "NWebExtensionHistoryCefDelegate::DeleteUrl fail";
    return false;
  }

  return true;
#endif
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