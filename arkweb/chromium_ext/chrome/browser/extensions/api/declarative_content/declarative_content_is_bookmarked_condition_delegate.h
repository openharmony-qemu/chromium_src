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

#ifndef CHROME_BROWSER_EXTENSIONS_API_DECLARATIVE_CONTENT_DECLARATIVE_CONTENT_IS_BOOKMARKED_CONDITION_DELEGATE_H_
#define CHROME_BROWSER_EXTENSIONS_API_DECLARATIVE_CONTENT_DECLARATIVE_CONTENT_IS_BOOKMARKED_CONDITION_DELEGATE_H_

#include <mutex>

#include "base/functional/callback.h"
#include "base/observer_list.h"
#include "base/observer_list_types.h"
#include "base/no_destructor.h"
#include "content/public/browser/browser_context.h"
#include "ohos_nweb/src/capi/browser_service/nweb_extension_bookmarks_types.h"

using RequestIsBookmarkedCallback =
    base::RepeatingCallback<void(uint32_t bookmarkCount,
                                 const NWebExtensionBookmarkTreeNode* bookmarks,
                                 const char* error)>;

namespace extensions {

class NWebExtensionBookmarksObserver : public base::CheckedObserver {
 public:
  NWebExtensionBookmarksObserver() = default;
  virtual ~NWebExtensionBookmarksObserver() = default;

  virtual void OnBookmarksCreated(const GURL& url) {}
  virtual void OnBookmarksRemoved(const std::set<GURL>& urls) {}
  virtual void OnBookmarksImportBegin() {}
  virtual void OnBookmarksImportEnd() {}
};

class DeclarativeContentIsBookmarkedConditionDelegate {
 public:
  static DeclarativeContentIsBookmarkedConditionDelegate& GetInstance();

  void AddObserver(content::BrowserContext* context,
                   NWebExtensionBookmarksObserver* observer);
  void RemoveObserver(NWebExtensionBookmarksObserver* observer);

  void OnBookmarksCreated(content::BrowserContext* context,
                          const char* id,
                          const NWebExtensionBookmarkTreeNode* bookmark);
  void OnBookmarksRemoved(content::BrowserContext* context,
                          const char* id,
                          const NWebExtensionBookmarksRemoveInfo* removedInfo);
  void OnBookmarksImportBegin(content::BrowserContext* context);
  void OnBookmarksImportEnd(content::BrowserContext* context);

  void RequestIsBookmarked(const GURL& url,
                           RequestIsBookmarkedCallback callback);

 private:
  friend class base::NoDestructor<DeclarativeContentIsBookmarkedConditionDelegate>;
  DeclarativeContentIsBookmarkedConditionDelegate() = default;
  ~DeclarativeContentIsBookmarkedConditionDelegate() = default;
  DeclarativeContentIsBookmarkedConditionDelegate(
      const DeclarativeContentIsBookmarkedConditionDelegate&) = delete;
  DeclarativeContentIsBookmarkedConditionDelegate& operator=(
      const DeclarativeContentIsBookmarkedConditionDelegate&) = delete;

  std::map<content::BrowserContext*,
           base::ObserverList<NWebExtensionBookmarksObserver>>
      observers_;
  std::mutex mutex_;
};
}  // namespace extensions
#endif  // CHROME_BROWSER_EXTENSIONS_API_DECLARATIVE_CONTENT_DECLARATIVE_CONTENT_IS_BOOKMARKED_CONDITION_DELEGATE_H_
