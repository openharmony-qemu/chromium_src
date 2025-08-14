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

#include "arkweb/chromium_ext/chrome/browser/extensions/api/declarative_content/declarative_content_is_bookmarked_condition_delegate.h"

#include "arkweb/ohos_nweb/src/cef_delegate/nweb_extension_bookmarks_cef_delegate.h"

namespace extensions {
namespace {
GURL GetUrlFromBookmarkTreeNode(const NWebExtensionBookmarkTreeNode* node) {
  if (node && node->url && node->url[0] != '\0') {
    return GURL(node->url);
  }
  return GURL();
}

void GetUrlsFromBookmarkTreeNode(const NWebExtensionBookmarkTreeNode* node,
                                 std::set<GURL>& urls) {
  GURL url = GetUrlFromBookmarkTreeNode(node);
  if (!url.is_empty()) {
    urls.insert(url);
  }
  if (node && node->hasChildren && node->children) {
    for (uint32_t i = 0; i < node->childCount; i++) {
      GetUrlsFromBookmarkTreeNode(&node->children[i], urls);
    }
  }
}
}  // namespace

DeclarativeContentIsBookmarkedConditionDelegate&
DeclarativeContentIsBookmarkedConditionDelegate::GetInstance() {
  static DeclarativeContentIsBookmarkedConditionDelegate instance;
  return instance;
}

void DeclarativeContentIsBookmarkedConditionDelegate::AddObserver(
    content::BrowserContext* context,
    NWebExtensionBookmarksObserver* observer) {
  std::lock_guard<std::mutex> lock(mutex_);
  auto& observers = observers_[context];
  observers.AddObserver(observer);
}

void DeclarativeContentIsBookmarkedConditionDelegate::RemoveObserver(
    NWebExtensionBookmarksObserver* observer) {
  std::lock_guard<std::mutex> lock(mutex_);
  for (auto iter = observers_.begin(); iter != observers_.end();) {
    iter->second.RemoveObserver(observer);
    if (iter->second.empty()) {
      iter = observers_.erase(iter);
    } else {
      ++iter;
    }
  }
}

void DeclarativeContentIsBookmarkedConditionDelegate::OnBookmarksCreated(
    content::BrowserContext* context,
    const char* id,
    const NWebExtensionBookmarkTreeNode* bookmark) {
  if (!bookmark) {
    return;
  }
  GURL url = GetUrlFromBookmarkTreeNode(bookmark);
  if (url.is_empty()) {
    return;
  }
  std::lock_guard<std::mutex> lock(mutex_);
  auto observers_iter = observers_.find(context);
  if (observers_iter == observers_.end()) {
    return;
  }
  for (auto& observer : observers_iter->second) {
    observer.OnBookmarksCreated(url);
  }
}

void DeclarativeContentIsBookmarkedConditionDelegate::OnBookmarksRemoved(
    content::BrowserContext* context,
    const char* id,
    const NWebExtensionBookmarksRemoveInfo* removedInfo) {
  if (!removedInfo) {
    return;
  }
  std::set<GURL> urls;
  GetUrlsFromBookmarkTreeNode(removedInfo->treeNode, urls);
  if (urls.empty()) {
    return;
  }

  std::lock_guard<std::mutex> lock(mutex_);
  auto observers_iter = observers_.find(context);
  if (observers_iter == observers_.end()) {
    return;
  }
  for (auto& observer : observers_iter->second) {
    observer.OnBookmarksRemoved(urls);
  }
}

void DeclarativeContentIsBookmarkedConditionDelegate::OnBookmarksImportBegin(
    content::BrowserContext* context) {
  std::lock_guard<std::mutex> lock(mutex_);
  auto observers_iter = observers_.find(context);
  if (observers_iter == observers_.end()) {
    return;
  }
  for (auto& observer : observers_iter->second) {
    observer.OnBookmarksImportBegin();
  }
}

void DeclarativeContentIsBookmarkedConditionDelegate::OnBookmarksImportEnd(
    content::BrowserContext* context) {
  std::lock_guard<std::mutex> lock(mutex_);
  auto observers_iter = observers_.find(context);
  if (observers_iter == observers_.end()) {
    return;
  }
  for (auto& observer : observers_iter->second) {
    observer.OnBookmarksImportEnd();
  }
}

void DeclarativeContentIsBookmarkedConditionDelegate::RequestIsBookmarked(
    const GURL& url,
    RequestIsBookmarkedCallback callback) {
  if (url.is_empty()) {
    return;
  }
  NWebExtensionBookmarksSearchParam param = {0};
  char* param_url = strdup(url.spec().c_str());
  if (!param_url) {
    LOG(ERROR) << __FUNCTION__ << " failed to allocate param_url";
    return;
  }
  param.url = param_url;
  bool success =
      OHOS::NWeb::NWebExtensionBookmarksCefDelegate::GetInstance().OnSearch(
          &param, std::move(callback));
  if (param.url) {
    free(param.url);
    param.url = nullptr;
  }
  if (!success) {
    LOG(ERROR) << __FUNCTION__ << " failed to search url in bookmarks";
  }
}
}  // namespace extensions
