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

class BookmarksGetFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("bookmarks.get", BOOKMARKS_GET)
  ~BookmarksGetFunction() override {}
  ResponseAction Run() override;

 private:
  static void GetCallback(const base::WeakPtr<BookmarksGetFunction>& function,
                          uint32_t bookmarkCount,
                          const NWebExtensionBookmarkTreeNode* bookmarks,
                          const char* error);
  bool call_get_bookmarks_ = false;
  base::WeakPtrFactory<BookmarksGetFunction> weak_ptr_factory_{this};
};

class BookmarksGetChildrenFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("bookmarks.getChildren", BOOKMARKS_GETCHILDREN)
  ~BookmarksGetChildrenFunction() override {}
  ResponseAction Run() override;

 private:
  static void GetChildrenCallback(
      const base::WeakPtr<BookmarksGetChildrenFunction>& function,
      uint32_t bookmarkCount,
      const NWebExtensionBookmarkTreeNode* bookmarks,
      const char* error);
  bool call_getchildren_bookmarks_ = false;
  base::WeakPtrFactory<BookmarksGetChildrenFunction> weak_ptr_factory_{this};
};

class BookmarksGetRecentFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("bookmarks.getRecent", BOOKMARKS_GETRECENT)
  ~BookmarksGetRecentFunction() override {}
  ResponseAction Run() override;

 private:
  static void GetRecentCallback(
      const base::WeakPtr<BookmarksGetRecentFunction>& function,
      uint32_t bookmarkCount,
      const NWebExtensionBookmarkTreeNode* bookmarks,
      const char* error);
  bool call_getrecent_bookmarks_ = false;
  base::WeakPtrFactory<BookmarksGetRecentFunction> weak_ptr_factory_{this};
};

class BookmarksGetTreeFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("bookmarks.getTree", BOOKMARKS_GETTREE)
  ~BookmarksGetTreeFunction() override {}
  ResponseAction Run() override;

 private:
  static void GetTreeCallback(
      const base::WeakPtr<BookmarksGetTreeFunction>& function,
      uint32_t bookmarkCount,
      const NWebExtensionBookmarkTreeNode* bookmarks,
      const char* error);
  bool call_gettree_bookmarks_ = false;
  base::WeakPtrFactory<BookmarksGetTreeFunction> weak_ptr_factory_{this};
};

class BookmarksGetSubTreeFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("bookmarks.getSubTree", BOOKMARKS_GETSUBTREE)
  ~BookmarksGetSubTreeFunction() override {}
  ResponseAction Run() override;

 private:
  static void GetSubTreeCallback(
      const base::WeakPtr<BookmarksGetSubTreeFunction>& function,
      uint32_t bookmarkCount,
      const NWebExtensionBookmarkTreeNode* bookmarks,
      const char* error);
  bool call_getsubtree_bookmarks_ = false;
  base::WeakPtrFactory<BookmarksGetSubTreeFunction> weak_ptr_factory_{this};
};

class BookmarksSearchFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("bookmarks.search", BOOKMARKS_SEARCH)
  ~BookmarksSearchFunction() override {}
  ResponseAction Run() override;

 private:
  static void SearchCallback(
      const base::WeakPtr<BookmarksSearchFunction>& function,
      uint32_t bookmarkCount,
      const NWebExtensionBookmarkTreeNode* bookmarks,
      const char* error);
  bool call_search_bookmarks_ = false;
  base::WeakPtrFactory<BookmarksSearchFunction> weak_ptr_factory_{this};
};

class BookmarksRemoveFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("bookmarks.remove", BOOKMARKS_REMOVE)
  ~BookmarksRemoveFunction() override {}
  ResponseAction Run() override;

 private:
  static void RemoveCallback(
      const base::WeakPtr<BookmarksRemoveFunction>& function,
      const char* error);
  bool call_remove_bookmarks_ = false;
  base::WeakPtrFactory<BookmarksRemoveFunction> weak_ptr_factory_{this};
};

class BookmarksRemoveTreeFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("bookmarks.removeTree", BOOKMARKS_REMOVETREE)
  ~BookmarksRemoveTreeFunction() override {}
  ResponseAction Run() override;

 private:
  static void RemoveTreeCallback(
      const base::WeakPtr<BookmarksRemoveTreeFunction>& function,
      const char* error);
  bool call_removetree_bookmarks_ = false;
  base::WeakPtrFactory<BookmarksRemoveTreeFunction> weak_ptr_factory_{this};
};

class BookmarksCreateFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("bookmarks.create", BOOKMARKS_CREATE)
  ~BookmarksCreateFunction() override {}
  ResponseAction Run() override;

 private:
  static void CreateCallback(
      const base::WeakPtr<BookmarksCreateFunction>& function,
      const NWebExtensionBookmarkTreeNode* bookmark,
      const char* error);
  bool call_create_bookmarks_ = false;
  base::WeakPtrFactory<BookmarksCreateFunction> weak_ptr_factory_{this};
};

class BookmarksMoveFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("bookmarks.move", BOOKMARKS_MOVE)
  ~BookmarksMoveFunction() override {}
  ResponseAction Run() override;

 private:
  static void MoveCallback(const base::WeakPtr<BookmarksMoveFunction>& function,
                           const NWebExtensionBookmarkTreeNode* bookmark,
                           const char* error);
  bool call_move_bookmarks_ = false;
  base::WeakPtrFactory<BookmarksMoveFunction> weak_ptr_factory_{this};
};

class BookmarksUpdateFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("bookmarks.update", BOOKMARKS_UPDATE)
  ~BookmarksUpdateFunction() override {}
  ResponseAction Run() override;

 private:
  static void UpdateCallback(
      const base::WeakPtr<BookmarksUpdateFunction>& function,
      const NWebExtensionBookmarkTreeNode* bookmark,
      const char* error);
  bool call_update_bookmarks_ = false;
  base::WeakPtrFactory<BookmarksUpdateFunction> weak_ptr_factory_{this};
};
