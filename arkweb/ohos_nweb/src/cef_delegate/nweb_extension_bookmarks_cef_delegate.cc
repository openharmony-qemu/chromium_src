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

#include "nweb_extension_bookmarks_cef_delegate.h"

#include <atomic>
#include <map>
#include <mutex>

#include "base/logging.h"
#include "cef/libcef/browser/request_context_impl.h"
#include "cef/ohos_cef_ext/libcef/browser/extensions/api/bookmarks/bookmarks_event_router.h"
#include "content/public/browser/browser_context.h"

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "ohos_nweb_ex/core/extension/nweb_extension_bookmarks_dispatcher.h"
#endif

namespace OHOS::NWeb {

namespace {

static std::map<int, BookmarksCreateCallback> g_bookmarks_create_callback_map_;
std::mutex g_bookmarks_create_callback_map_mutex;

static std::map<int, BookmarksGetCallback> g_bookmarks_get_callback_map_;
std::mutex g_bookmarks_get_callback_map_mutex;

static std::map<int, BookmarksGetChildrenCallback>
    g_bookmarks_getchildren_callback_map_;
std::mutex g_bookmarks_getchildren_callback_map_mutex;

static std::map<int, BookmarksGetRecentCallback>
    g_bookmarks_getrecent_callback_map_;
std::mutex g_bookmarks_getrecent_callback_map_mutex;

static std::map<int, BookmarksGetSubTreeCallback>
    g_bookmarks_getsubtree_callback_map_;
std::mutex g_bookmarks_getsubtree_callback_map_mutex;

static std::map<int, BookmarksGetTreeCallback>
    g_bookmarks_gettree_callback_map_;
std::mutex g_bookmarks_gettree_callback_map_mutex;

static std::map<int, BookmarksMoveCallback> g_bookmarks_move_callback_map_;
std::mutex g_bookmarks_move_callback_map_mutex;

static std::map<int, BookmarksRemoveCallback> g_bookmarks_remove_callback_map_;
std::mutex g_bookmarks_remove_callback_map_mutex;

static std::map<int, BookmarksRemoveTreeCallback>
    g_bookmarks_removetree_callback_map_;
std::mutex g_bookmarks_removetree_callback_map_mutex;

static std::map<int, BookmarksSearchCallback> g_bookmarks_search_callback_map_;
std::mutex g_bookmarks_search_callback_map_mutex;

static std::map<int, BookmarksUpdateCallback> g_bookmarks_update_callback_map_;
std::mutex g_bookmarks_update_callback_map_mutex;

content::BrowserContext* GetBrowserContext() {
  CefRequestContextImpl* request_context = static_cast<CefRequestContextImpl*>(
      CefRequestContext::GetGlobalContext().get());
  if (!request_context) {
    LOG(ERROR) << "request_context is null";
    return nullptr;
  }
  CefBrowserContext* cef_browser_context = request_context->GetBrowserContext();
  if (!cef_browser_context) {
    LOG(ERROR) << "cef_browser_context is null";
    return nullptr;
  }
  return cef_browser_context->AsBrowserContext();
}

}  // namespace

// static
NWebExtensionBookmarksCefDelegate&
NWebExtensionBookmarksCefDelegate::GetInstance() {
  static NWebExtensionBookmarksCefDelegate instance;
  return instance;
}

void NWebExtensionBookmarksCefDelegate::OnCreated(
    const char* id,
    const NWebExtensionBookmarkTreeNode* bookmark) {
  extensions::CefBookmarksEventRouter::GetInstance()
      .DispatchBookmarksCreatedEvent(GetBrowserContext(), id, bookmark);
}

void NWebExtensionBookmarksCefDelegate::OnChanged(
    const char* id,
    const NWebExtensionBookmarksChangeInfo* changeInfo) {
  extensions::CefBookmarksEventRouter::GetInstance()
      .DispatchBookmarksChangedEvent(GetBrowserContext(), id, changeInfo);
}

void NWebExtensionBookmarksCefDelegate::OnChildrenReordered(
    const char* id,
    const NWebExtensionBookmarksReorderInfo* reorderInfo) {
  extensions::CefBookmarksEventRouter::GetInstance()
      .DispatchBookmarksChildrenReorderedEvent(GetBrowserContext(), id,
                                               reorderInfo);
}

void NWebExtensionBookmarksCefDelegate::OnImportBegan() {
  extensions::CefBookmarksEventRouter::GetInstance()
      .DispatchBookmarksImportBeganEvent(GetBrowserContext());
}

void NWebExtensionBookmarksCefDelegate::OnImportEnded() {
  extensions::CefBookmarksEventRouter::GetInstance()
      .DispatchBookmarksImportEndedEvent(GetBrowserContext());
}

void NWebExtensionBookmarksCefDelegate::OnMoved(
    const char* id,
    const NWebExtensionBookmarksMoveInfo* moveInfo) {
  extensions::CefBookmarksEventRouter::GetInstance()
      .DispatchBookmarksMovedEvent(GetBrowserContext(), id, moveInfo);
}

void NWebExtensionBookmarksCefDelegate::OnRemoved(
    const char* id,
    const NWebExtensionBookmarksRemoveInfo* removeInfo) {
  extensions::CefBookmarksEventRouter::GetInstance()
      .DispatchBookmarksRemovedEvent(GetBrowserContext(), id, removeInfo);
}

bool NWebExtensionBookmarksCefDelegate::OnCreate(
    const NWebExtensionBookmarksCreateDetails* details,
    BookmarksCreateCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static std::atomic<int> requestId(0);
  int currentRequestId;
  {
    std::lock_guard<std::mutex> lock(g_bookmarks_create_callback_map_mutex);
    currentRequestId = ++requestId;
    g_bookmarks_create_callback_map_[currentRequestId] = std::move(callback);
  }
  bool result = NWebExtensionBookmarksDispatcher::GetInstance().OnCreate(
      currentRequestId, details);
  if (!result) {
    std::lock_guard<std::mutex> lock(g_bookmarks_create_callback_map_mutex);
    g_bookmarks_create_callback_map_.erase(currentRequestId);
  }
  return result;
#endif
}

void NWebExtensionBookmarksCefDelegate::OnCreateCallback(
    const NWebExtensionBookmarksCallbackResult* result) {
  if (!result) {
    LOG(ERROR) << "OnCreateCallback result is null";
    return;
  }
  BookmarksCreateCallback callback;
  {
    std::lock_guard<std::mutex> lock(g_bookmarks_create_callback_map_mutex);
    auto it = g_bookmarks_create_callback_map_.find(result->requestId);
    if (it == g_bookmarks_create_callback_map_.end()) {
      LOG(ERROR) << "OnCreateCallback requestId not found: "
                 << result->requestId;
      return;
    }

    callback = std::move(it->second);
    g_bookmarks_create_callback_map_.erase(it);
  }
  std::move(callback).Run(result->treeNodes, result->error);
}

bool NWebExtensionBookmarksCefDelegate::OnGet(
    const NWebExtensionBookmarksGetParam* getParam,
    BookmarksGetCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static std::atomic<int> requestId(0);
  int currentRequestId;
  {
    std::lock_guard<std::mutex> lock(g_bookmarks_get_callback_map_mutex);
    currentRequestId = ++requestId;
    g_bookmarks_get_callback_map_[currentRequestId] = std::move(callback);
  }
  bool result = NWebExtensionBookmarksDispatcher::GetInstance().OnGet(
      currentRequestId, getParam);
  if (!result) {
    std::lock_guard<std::mutex> lock(g_bookmarks_get_callback_map_mutex);
    g_bookmarks_get_callback_map_.erase(currentRequestId);
  }
  return result;
#endif
}

void NWebExtensionBookmarksCefDelegate::OnGetCallback(
    const NWebExtensionBookmarksCallbackResult* result) {
  if (!result) {
    LOG(ERROR) << "OnGetCallback result is null";
    return;
  }
  BookmarksGetCallback callback;
  {
    std::lock_guard<std::mutex> lock(g_bookmarks_get_callback_map_mutex);
    auto it = g_bookmarks_get_callback_map_.find(result->requestId);
    if (it == g_bookmarks_get_callback_map_.end()) {
      LOG(ERROR) << "OnGetCallback requestId not found: " << result->requestId;
      return;
    }

    callback = std::move(it->second);
    g_bookmarks_get_callback_map_.erase(it);
  }
  std::move(callback).Run(result->treeSize, result->treeNodes, result->error);
}

bool NWebExtensionBookmarksCefDelegate::OnGetChildren(
    const char* id,
    BookmarksGetChildrenCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static std::atomic<int> requestId(0);
  int currentRequestId;
  {
    std::lock_guard<std::mutex> lock(
        g_bookmarks_getchildren_callback_map_mutex);
    currentRequestId = ++requestId;
    g_bookmarks_getchildren_callback_map_[currentRequestId] =
        std::move(callback);
  }
  bool result = NWebExtensionBookmarksDispatcher::GetInstance().OnGetChildren(
      currentRequestId, id);
  if (!result) {
    std::lock_guard<std::mutex> lock(
        g_bookmarks_getchildren_callback_map_mutex);
    g_bookmarks_getchildren_callback_map_.erase(currentRequestId);
  }
  return result;
#endif
}

void NWebExtensionBookmarksCefDelegate::OnGetChildrenCallback(
    const NWebExtensionBookmarksCallbackResult* result) {
  if (!result) {
    LOG(ERROR) << "OnGetChildrenCallback result is null";
    return;
  }
  BookmarksGetChildrenCallback callback;
  {
    std::lock_guard<std::mutex> lock(
        g_bookmarks_getchildren_callback_map_mutex);
    auto it = g_bookmarks_getchildren_callback_map_.find(result->requestId);
    if (it == g_bookmarks_getchildren_callback_map_.end()) {
      LOG(ERROR) << "OnGetChildrenCallback requestId not found: "
                 << result->requestId;
      return;
    }

    callback = std::move(it->second);
    g_bookmarks_getchildren_callback_map_.erase(it);
  }
  std::move(callback).Run(result->treeSize, result->treeNodes, result->error);
}

bool NWebExtensionBookmarksCefDelegate::OnGetRecent(
    int numberOfItems,
    BookmarksGetRecentCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static std::atomic<int> requestId(0);
  int currentRequestId;
  {
    std::lock_guard<std::mutex> lock(g_bookmarks_getrecent_callback_map_mutex);
    currentRequestId = ++requestId;
    g_bookmarks_getrecent_callback_map_[currentRequestId] = std::move(callback);
  }
  bool result = NWebExtensionBookmarksDispatcher::GetInstance().OnGetRecent(
      currentRequestId, numberOfItems);
  if (!result) {
    std::lock_guard<std::mutex> lock(g_bookmarks_getrecent_callback_map_mutex);
    g_bookmarks_getrecent_callback_map_.erase(currentRequestId);
  }
  return result;
#endif
}

void NWebExtensionBookmarksCefDelegate::OnGetRecentCallback(
    const NWebExtensionBookmarksCallbackResult* result) {
  if (!result) {
    LOG(ERROR) << "OnGetRecentCallback result is null";
    return;
  }
  BookmarksGetRecentCallback callback;
  {
    std::lock_guard<std::mutex> lock(g_bookmarks_getrecent_callback_map_mutex);
    auto it = g_bookmarks_getrecent_callback_map_.find(result->requestId);
    if (it == g_bookmarks_getrecent_callback_map_.end()) {
      LOG(ERROR) << "OnGetRecentCallback requestId not found: " << result->requestId;
      return;
    }

    callback = std::move(it->second);
    g_bookmarks_getrecent_callback_map_.erase(it);
  }
  std::move(callback).Run(result->treeSize, result->treeNodes, result->error);
}

bool NWebExtensionBookmarksCefDelegate::OnGetSubTree(
    const char* id,
    BookmarksGetSubTreeCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static std::atomic<int> requestId(0);
  int currentRequestId;
  {
    std::lock_guard<std::mutex> lock(g_bookmarks_getsubtree_callback_map_mutex);
    currentRequestId = ++requestId;
    g_bookmarks_getsubtree_callback_map_[currentRequestId] =
        std::move(callback);
  }
  bool result = NWebExtensionBookmarksDispatcher::GetInstance().OnGetSubTree(
      currentRequestId, id);
  if (!result) {
    std::lock_guard<std::mutex> lock(g_bookmarks_getsubtree_callback_map_mutex);
    g_bookmarks_getsubtree_callback_map_.erase(currentRequestId);
  }
  return result;
#endif
}

void NWebExtensionBookmarksCefDelegate::OnGetSubTreeCallback(
    const NWebExtensionBookmarksCallbackResult* result) {
  if (!result) {
    LOG(ERROR) << "OnGetSubTreeCallback result is null";
    return;
  }
  BookmarksGetSubTreeCallback callback;
  {
    std::lock_guard<std::mutex> lock(g_bookmarks_getsubtree_callback_map_mutex);
    auto it = g_bookmarks_getsubtree_callback_map_.find(result->requestId);
    if (it == g_bookmarks_getsubtree_callback_map_.end()) {
      LOG(ERROR) << "OnGetSubTreeCallback requestId not found: " << result->requestId;
      return;
    }

    callback = std::move(it->second);
    g_bookmarks_getsubtree_callback_map_.erase(it);
  }
  std::move(callback).Run(result->treeSize, result->treeNodes, result->error);
}

bool NWebExtensionBookmarksCefDelegate::OnGetTree(
    BookmarksGetTreeCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static std::atomic<int> requestId(0);
  int currentRequestId;
  {
    std::lock_guard<std::mutex> lock(g_bookmarks_gettree_callback_map_mutex);
    currentRequestId = ++requestId;
    g_bookmarks_gettree_callback_map_[currentRequestId] = std::move(callback);
  }
  bool result = NWebExtensionBookmarksDispatcher::GetInstance().OnGetTree(
      currentRequestId);
  if (!result) {
    std::lock_guard<std::mutex> lock(g_bookmarks_gettree_callback_map_mutex);
    g_bookmarks_gettree_callback_map_.erase(currentRequestId);
  }
  return result;
#endif
}

void NWebExtensionBookmarksCefDelegate::OnGetTreeCallback(
    const NWebExtensionBookmarksCallbackResult* result) {
  if (!result) {
    LOG(ERROR) << "OnGetTreeCallback result is null";
    return;
  }
  BookmarksGetTreeCallback callback;
  {
    std::lock_guard<std::mutex> lock(g_bookmarks_gettree_callback_map_mutex);
    auto it = g_bookmarks_gettree_callback_map_.find(result->requestId);
    if (it == g_bookmarks_gettree_callback_map_.end()) {
      LOG(ERROR) << "OnGetTreeCallback requestId not found: " << result->requestId;
      return;
    }

    callback = std::move(it->second);
    g_bookmarks_gettree_callback_map_.erase(it);
  }
  std::move(callback).Run(result->treeSize, result->treeNodes, result->error);
}

bool NWebExtensionBookmarksCefDelegate::OnMove(
    const NWebExtensionBookmarksMoveParam* moveParam,
    BookmarksMoveCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static std::atomic<int> requestId(0);
  int currentRequestId;
  {
    std::lock_guard<std::mutex> lock(g_bookmarks_move_callback_map_mutex);
    currentRequestId = ++requestId;
    g_bookmarks_move_callback_map_[currentRequestId] = std::move(callback);
  }
  bool result = NWebExtensionBookmarksDispatcher::GetInstance().OnMove(
      currentRequestId, moveParam);
  if (!result) {
    std::lock_guard<std::mutex> lock(g_bookmarks_move_callback_map_mutex);
    g_bookmarks_move_callback_map_.erase(currentRequestId);
  }
  return result;
#endif
}

void NWebExtensionBookmarksCefDelegate::OnMoveCallback(
    const NWebExtensionBookmarksCallbackResult* result) {
  if (!result) {
    LOG(ERROR) << "OnMoveCallback result is null";
    return;
  }
  BookmarksMoveCallback callback;
  {
    std::lock_guard<std::mutex> lock(g_bookmarks_move_callback_map_mutex);
    auto it = g_bookmarks_move_callback_map_.find(result->requestId);
    if (it == g_bookmarks_move_callback_map_.end()) {
      LOG(ERROR) << "OnMoveCallback requestId not found: " << result->requestId;
      return;
    }

    callback = std::move(it->second);
    g_bookmarks_move_callback_map_.erase(it);
  }
  std::move(callback).Run(result->treeNodes, result->error);
}

bool NWebExtensionBookmarksCefDelegate::OnRemove(
    const char* id,
    BookmarksRemoveCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static std::atomic<int> requestId(0);
  int currentRequestId;
  {
    std::lock_guard<std::mutex> lock(g_bookmarks_remove_callback_map_mutex);
    currentRequestId = ++requestId;
    g_bookmarks_remove_callback_map_[currentRequestId] = std::move(callback);
  }
  bool result = NWebExtensionBookmarksDispatcher::GetInstance().OnRemove(
      currentRequestId, id);
  if (!result) {
    std::lock_guard<std::mutex> lock(g_bookmarks_remove_callback_map_mutex);
    g_bookmarks_remove_callback_map_.erase(currentRequestId);
  }
  return result;
#endif
}

void NWebExtensionBookmarksCefDelegate::OnRemoveCallback(int requestId,
                                                         const char* error) {
  BookmarksRemoveCallback callback;
  {
    std::lock_guard<std::mutex> lock(g_bookmarks_remove_callback_map_mutex);
    auto it = g_bookmarks_remove_callback_map_.find(requestId);
    if (it == g_bookmarks_remove_callback_map_.end()) {
      LOG(ERROR) << "OnRemoveCallback requestId not found: " << requestId;
      return;
    }

    callback = std::move(it->second);
    g_bookmarks_remove_callback_map_.erase(it);
  }
  std::move(callback).Run(error);
}

bool NWebExtensionBookmarksCefDelegate::OnRemoveTree(
    const char* id,
    BookmarksRemoveTreeCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static std::atomic<int> requestId(0);
  int currentRequestId;
  {
    std::lock_guard<std::mutex> lock(g_bookmarks_removetree_callback_map_mutex);
    currentRequestId = ++requestId;
    g_bookmarks_removetree_callback_map_[currentRequestId] =
        std::move(callback);
  }
  bool result = NWebExtensionBookmarksDispatcher::GetInstance().OnRemoveTree(
      currentRequestId, id);
  if (!result) {
    std::lock_guard<std::mutex> lock(g_bookmarks_removetree_callback_map_mutex);
    g_bookmarks_removetree_callback_map_.erase(currentRequestId);
  }
  return result;
#endif
}

void NWebExtensionBookmarksCefDelegate::OnRemoveTreeCallback(
    int requestId,
    const char* error) {
  BookmarksRemoveTreeCallback callback;
  {
    std::lock_guard<std::mutex> lock(g_bookmarks_removetree_callback_map_mutex);
    auto it = g_bookmarks_removetree_callback_map_.find(requestId);
    if (it == g_bookmarks_removetree_callback_map_.end()) {
      LOG(ERROR) << "OnRemoveTreeCallback requestId not found: " << requestId;
      return;
    }

    callback = std::move(it->second);
    g_bookmarks_removetree_callback_map_.erase(it);
  }
  std::move(callback).Run(error);
}

bool NWebExtensionBookmarksCefDelegate::OnSearch(
    const NWebExtensionBookmarksSearchParam* searchParam,
    BookmarksSearchCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static std::atomic<int> requestId(0);
  int currentRequestId;
  {
    std::lock_guard<std::mutex> lock(g_bookmarks_search_callback_map_mutex);
    currentRequestId = ++requestId;
    g_bookmarks_search_callback_map_[currentRequestId] = std::move(callback);
  }
  bool result = NWebExtensionBookmarksDispatcher::GetInstance().OnSearch(
      currentRequestId, searchParam);
  if (!result) {
    std::lock_guard<std::mutex> lock(g_bookmarks_search_callback_map_mutex);
    g_bookmarks_search_callback_map_.erase(currentRequestId);
  }
  return result;
#endif
}

void NWebExtensionBookmarksCefDelegate::OnSearchCallback(
    const NWebExtensionBookmarksCallbackResult* result) {
  if (!result) {
    LOG(ERROR) << "OnSearchCallback result is null";
    return;
  }
  BookmarksSearchCallback callback;
  {
    std::lock_guard<std::mutex> lock(g_bookmarks_search_callback_map_mutex);
    auto it = g_bookmarks_search_callback_map_.find(result->requestId);
    if (it == g_bookmarks_search_callback_map_.end()) {
      LOG(ERROR) << "OnSearchCallback requestId not found: "
                 << result->requestId;
      return;
    }

    callback = std::move(it->second);
    g_bookmarks_search_callback_map_.erase(it);
  }
  std::move(callback).Run(result->treeSize, result->treeNodes, result->error);
}

bool NWebExtensionBookmarksCefDelegate::OnUpdate(
    const NWebExtensionBookmarksUpdateParam* updateParam,
    BookmarksUpdateCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static std::atomic<int> requestId(0);
  int currentRequestId;
  {
    std::lock_guard<std::mutex> lock(g_bookmarks_update_callback_map_mutex);
    currentRequestId = ++requestId;
    g_bookmarks_update_callback_map_[currentRequestId] = std::move(callback);
  }
  bool result = NWebExtensionBookmarksDispatcher::GetInstance().OnUpdate(
      currentRequestId, updateParam);
  if (!result) {
    std::lock_guard<std::mutex> lock(g_bookmarks_update_callback_map_mutex);
    g_bookmarks_update_callback_map_.erase(currentRequestId);
  }
  return result;
#endif
}

void NWebExtensionBookmarksCefDelegate::OnUpdateCallback(
    const NWebExtensionBookmarksCallbackResult* result) {
  if (!result) {
    LOG(ERROR) << "OnUpdateCallback result is null";
    return;
  }
  BookmarksUpdateCallback callback;
  {
    std::lock_guard<std::mutex> lock(g_bookmarks_update_callback_map_mutex);
    auto it = g_bookmarks_update_callback_map_.find(result->requestId);
    if (it == g_bookmarks_update_callback_map_.end()) {
      LOG(ERROR) << "OnUpdateCallback requestId not found: "
                 << result->requestId;
      return;
    }

    callback = std::move(it->second);
    g_bookmarks_update_callback_map_.erase(it);
  }
  std::move(callback).Run(result->treeNodes, result->error);
}

}  // namespace OHOS::NWeb
