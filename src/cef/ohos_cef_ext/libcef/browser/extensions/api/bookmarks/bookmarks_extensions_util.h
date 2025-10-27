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

#ifndef CEF_LIBCEF_BROWSER_EXTENSIONS_BOOKMARKS_EXTENSIONS_UTIL_H_
#define CEF_LIBCEF_BROWSER_EXTENSIONS_BOOKMARKS_EXTENSIONS_UTIL_H_

#include "base/values.h"
#include "ohos_nweb/src/capi/browser_service/nweb_extension_bookmarks_types.h"

namespace extensions {

base::Value::List CreateNWebExtensionBookmarksGetList(
    uint32_t count,
    const NWebExtensionBookmarkTreeNode* nodes);

base::Value::List CreateNWebExtensionBookmarksGetChildrenList(
    uint32_t count,
    const NWebExtensionBookmarkTreeNode* nodes);

base::Value::List CreateNWebExtensionBookmarksGetRecentList(
    uint32_t count,
    const NWebExtensionBookmarkTreeNode* nodes);

base::Value::List CreateNWebExtensionBookmarksGetTreeList(
    uint32_t count,
    const NWebExtensionBookmarkTreeNode* nodes);

base::Value::List CreateNWebExtensionBookmarksSearchList(
    uint32_t count,
    const NWebExtensionBookmarkTreeNode* nodes);

base::Value::List CreateNWebExtensionBookmarksCreateList(
    const NWebExtensionBookmarkTreeNode* node);

base::Value::List CreateNWebExtensionBookmarksMoveList(
    const NWebExtensionBookmarkTreeNode* node);

base::Value::List CreateNWebExtensionBookmarksUpdateList(
    const NWebExtensionBookmarkTreeNode* node);

base::Value::List CreateNWebExtensionBookmarksOnCreatedList(
    const char* id,
    const NWebExtensionBookmarkTreeNode* node);

base::Value::List CreateNWebExtensionBookmarksOnRemovedList(
    const char* id,
    const NWebExtensionBookmarksRemoveInfo* info);

base::Value::List CreateNWebExtensionBookmarksOnChangedList(
    const char* id,
    const NWebExtensionBookmarksChangeInfo* info);

base::Value::List CreateNWebExtensionBookmarksOnMovedList(
    const char* id,
    const NWebExtensionBookmarksMoveInfo* info);

base::Value::List CreateNWebExtensionBookmarksOnChildrenReorderedList(
    const char* id,
    const NWebExtensionBookmarksReorderInfo* info);

base::Value::List CreateNWebExtensionBookmarksOnImportBeganList();

base::Value::List CreateNWebExtensionBookmarksOnImportEndedList();

}  // namespace extensions

#endif  // CEF_LIBCEF_BROWSER_EXTENSIONS_BOOKMARKS_EXTENSIONS_UTIL_H_
