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

#ifndef NWEB_EXTENSION_BOOKMARKS_CEF_DELEGATE_H_
#define NWEB_EXTENSION_BOOKMARKS_CEF_DELEGATE_H_

#include <memory>

#include "base/functional/callback.h"
#include "ohos_nweb/src/capi/browser_service/nweb_extension_bookmarks_types.h"

using BookmarksCreateCallback =
    base::RepeatingCallback<void(const NWebExtensionBookmarkTreeNode* bookmark,
                                 const char* error)>;
using BookmarksMoveCallback =
    base::RepeatingCallback<void(const NWebExtensionBookmarkTreeNode* bookmark,
                                 const char* error)>;
using BookmarksUpdateCallback =
    base::RepeatingCallback<void(const NWebExtensionBookmarkTreeNode* bookmark,
                                 const char* error)>;
using BookmarksGetCallback =
    base::RepeatingCallback<void(uint32_t bookmarkCount,
                                 const NWebExtensionBookmarkTreeNode* bookmarks,
                                 const char* error)>;
using BookmarksGetChildrenCallback =
    base::RepeatingCallback<void(uint32_t bookmarkCount,
                                 const NWebExtensionBookmarkTreeNode* bookmarks,
                                 const char* error)>;
using BookmarksGetRecentCallback =
    base::RepeatingCallback<void(uint32_t bookmarkCount,
                                 const NWebExtensionBookmarkTreeNode* bookmarks,
                                 const char* error)>;
using BookmarksGetSubTreeCallback =
    base::RepeatingCallback<void(uint32_t bookmarkCount,
                                 const NWebExtensionBookmarkTreeNode* bookmarks,
                                 const char* error)>;
using BookmarksGetTreeCallback =
    base::RepeatingCallback<void(uint32_t bookmarkCount,
                                 const NWebExtensionBookmarkTreeNode* bookmarks,
                                 const char* error)>;
using BookmarksSearchCallback =
    base::RepeatingCallback<void(uint32_t bookmarkCount,
                                 const NWebExtensionBookmarkTreeNode* bookmarks,
                                 const char* error)>;
using BookmarksRemoveCallback =
    base::RepeatingCallback<void(const char* error)>;
using BookmarksRemoveTreeCallback =
    base::RepeatingCallback<void(const char* error)>;

namespace OHOS::NWeb {

class NWebExtensionBookmarksCefDelegate {
 public:
  static NWebExtensionBookmarksCefDelegate& GetInstance();

  // bookmarks.onCreated
  void OnCreated(const char* id, const NWebExtensionBookmarkTreeNode* bookmark);

  // bookmarks.onChanged
  void OnChanged(const char* id,
                 const NWebExtensionBookmarksChangeInfo* changeInfo);

  // bookmarks.onChildrenRecorded
  void OnChildrenReordered(
      const char* id,
      const NWebExtensionBookmarksReorderInfo* reorderInfo);

  // bookmarks.onImportBegan
  void OnImportBegan();

  // bookmarks.onImportEnded
  void OnImportEnded();

  // bookmarks.onMoved
  void OnMoved(const char* id, const NWebExtensionBookmarksMoveInfo* movedInfo);

  // bookmarks.onRemoved
  void OnRemoved(const char* id,
                 const NWebExtensionBookmarksRemoveInfo* removedInfo);

  // bookmarks.create
  bool OnCreate(const NWebExtensionBookmarksCreateDetails* details,
                BookmarksCreateCallback callback);

  void OnCreateCallback(const NWebExtensionBookmarksCallbackResult* result);

  // bookmarks.get
  bool OnGet(const NWebExtensionBookmarksGetParam* getParam,
             BookmarksGetCallback callback);

  void OnGetCallback(const NWebExtensionBookmarksCallbackResult* result);

  // bookmarks.getChildren
  bool OnGetChildren(const char* id, BookmarksGetChildrenCallback callback);

  void OnGetChildrenCallback(
      const NWebExtensionBookmarksCallbackResult* result);

  // bookmarks.getRecent
  bool OnGetRecent(int numberOfItems, BookmarksGetRecentCallback callback);

  void OnGetRecentCallback(const NWebExtensionBookmarksCallbackResult* result);

  // bookmarks.getSubTree
  bool OnGetSubTree(const char* id, BookmarksGetSubTreeCallback callback);

  void OnGetSubTreeCallback(const NWebExtensionBookmarksCallbackResult* result);

  // bookmarks.getTree
  bool OnGetTree(BookmarksGetTreeCallback callback);

  void OnGetTreeCallback(const NWebExtensionBookmarksCallbackResult* result);

  // bookmarks.move
  bool OnMove(const NWebExtensionBookmarksMoveParam* moveParam,
              BookmarksMoveCallback callback);

  void OnMoveCallback(const NWebExtensionBookmarksCallbackResult* result);

  // bookmarks.remove
  bool OnRemove(const char* id, BookmarksRemoveCallback callback);

  void OnRemoveCallback(int requestId, const char* error);

  // bookmarks.removeTree
  bool OnRemoveTree(const char* id, BookmarksRemoveTreeCallback callback);

  void OnRemoveTreeCallback(int requestId, const char* error);

  // bookmarks.search
  bool OnSearch(const NWebExtensionBookmarksSearchParam* searchParam,
                BookmarksSearchCallback callback);

  void OnSearchCallback(const NWebExtensionBookmarksCallbackResult* result);

  // bookmarks.update
  bool OnUpdate(const NWebExtensionBookmarksUpdateParam* updateParam,
                BookmarksUpdateCallback callback);

  void OnUpdateCallback(const NWebExtensionBookmarksCallbackResult* result);

 private:
  NWebExtensionBookmarksCefDelegate() = default;
  ~NWebExtensionBookmarksCefDelegate() = default;
  NWebExtensionBookmarksCefDelegate(const NWebExtensionBookmarksCefDelegate&) =
      delete;
  NWebExtensionBookmarksCefDelegate& operator=(
      const NWebExtensionBookmarksCefDelegate&) = delete;
};

}  // namespace OHOS::NWeb

#endif  // NWEB_EXTENSION_BOOKMARKS_CEF_DELEGATE_H_
