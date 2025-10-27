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

#include "ohos_cef_ext/libcef/browser/extensions/api/bookmarks/bookmarks_extensions_util.h"

#include "base/logging.h"

namespace extensions {

namespace {

base::Value::Dict NWebExtensionBookmarkTreeNodeToValue(
    const NWebExtensionBookmarkTreeNode* node);

base::Value::List NWebExtensionBookmarkTreeNodesToValue(
    uint32_t count,
    const NWebExtensionBookmarkTreeNode* nodes) {
  base::Value::List list;
  if (count == 0 || !nodes) {
    return list;
  }
  list.reserve(count);
  for (uint32_t index = 0; index < count; ++index) {
    const NWebExtensionBookmarkTreeNode* currentNode = &nodes[index];
    list.Append(NWebExtensionBookmarkTreeNodeToValue(currentNode));
  }

  return list;
}

base::Value::Dict NWebExtensionBookmarkTreeNodeToValue(
    const NWebExtensionBookmarkTreeNode* node) {
  base::Value::Dict dict;
  if (!node) {
    return dict;
  }

  dict.Set("id", node->id ? node->id : "");
  if (node->parentId) {
    dict.Set("parentId", node->parentId);
  }
  if (node->index) {
    dict.Set("index", *node->index);
  }
  if (node->url) {
    dict.Set("url", node->url);
  }
  dict.Set("title", node->title ? node->title : "");
  if (node->dateAdded) {
    dict.Set("dateAdded", *node->dateAdded);
  }
  if (node->dateLastUsed) {
    dict.Set("dateLastUsed", *node->dateLastUsed);
  }
  if (node->dateGroupModified) {
    dict.Set("dateGroupModified", *node->dateGroupModified);
  }
  if (node->unmodifiable) {
    dict.Set("unmodifiable", node->unmodifiable);
  }
  if (node->hasChildren) {
    dict.Set("children", NWebExtensionBookmarkTreeNodesToValue(node->childCount,
                                                               node->children));
  }
  if (node->folderType) {
    dict.Set("type", node->folderType);
  }
  if (node->syncing) {
    dict.Set("syncing", *node->syncing);
  }

  return dict;
}

base::Value::Dict NWebExtensionBookmarksRemoveInfoToValue(
    const NWebExtensionBookmarksRemoveInfo* info) {
  base::Value::Dict dict;
  if (!info) {
    return dict;
  }
  dict.Set("parentId", info->parentId ? info->parentId : "");
  dict.Set("index", info->index);
  dict.Set("node", NWebExtensionBookmarkTreeNodeToValue(info->treeNode));
  return dict;
}

base::Value::Dict NWebExtensionBookmarksChangeInfoToValue(
    const NWebExtensionBookmarksChangeInfo* info) {
  base::Value::Dict dict;
  if (!info) {
    return dict;
  }
  dict.Set("title", info->title ? info->title : "");
  if (info->url) {
    dict.Set("url", info->url);
  }
  return dict;
}

base::Value::Dict NWebExtensionBookmarksMoveInfoToValue(
    const NWebExtensionBookmarksMoveInfo* info) {
  base::Value::Dict dict;
  if (!info) {
    return dict;
  }
  dict.Set("parentId", info->parentId ? info->parentId : "");
  dict.Set("index", info->index);
  dict.Set("oldParentId", info->oldParentId ? info->oldParentId : "");
  dict.Set("oldIndex", info->oldIndex);
  return dict;
}

base::Value::Dict NWebExtensionBookmarksReorderInfoToValue(
    const NWebExtensionBookmarksReorderInfo* info) {
  base::Value::Dict dict;
  if (!info) {
    return dict;
  }
  base::Value::List childIdsList;
  if (info->count > 0 && info->childIds) {
    childIdsList.reserve(info->count);
    for (uint32_t i = 0; i < info->count; ++i) {
      childIdsList.Append(info->childIds[i] ? info->childIds[i] : "");
    }
  }
  dict.Set("childIds", std::move(childIdsList));
  return dict;
}

base::Value::List CreateBookmarksListImpl(
    const NWebExtensionBookmarkTreeNode* node) {
  base::Value::List list;
  list.reserve(1);
  list.Append(NWebExtensionBookmarkTreeNodeToValue(node));
  return list;
}

base::Value::List CreateBookmarksListImpl(
    uint32_t count,
    const NWebExtensionBookmarkTreeNode* nodes) {
  base::Value::List list;
  list.reserve(1);
  list.Append(NWebExtensionBookmarkTreeNodesToValue(count, nodes));
  return list;
}

template <typename InfoType, typename Converter>
base::Value::List CreateBookmarksListImpl(const char* id,
                                          const InfoType* info,
                                          Converter converter) {
  base::Value::List list;
  list.reserve(2);
  list.Append(id ? id : "");
  list.Append(converter(info));
  return list;
}

base::Value::List CreateBookmarksListImpl() {
  return base::Value::List();
}

}  // namespace

base::Value::List CreateNWebExtensionBookmarksGetList(
    uint32_t count,
    const NWebExtensionBookmarkTreeNode* nodes) {
  return CreateBookmarksListImpl(count, nodes);
}

base::Value::List CreateNWebExtensionBookmarksGetChildrenList(
    uint32_t count,
    const NWebExtensionBookmarkTreeNode* nodes) {
  return CreateBookmarksListImpl(count, nodes);
}

base::Value::List CreateNWebExtensionBookmarksGetRecentList(
    uint32_t count,
    const NWebExtensionBookmarkTreeNode* nodes) {
  return CreateBookmarksListImpl(count, nodes);
}

base::Value::List CreateNWebExtensionBookmarksGetTreeList(
    uint32_t count,
    const NWebExtensionBookmarkTreeNode* nodes) {
  return CreateBookmarksListImpl(count, nodes);
}

base::Value::List CreateNWebExtensionBookmarksSearchList(
    uint32_t count,
    const NWebExtensionBookmarkTreeNode* nodes) {
  return CreateBookmarksListImpl(count, nodes);
}

base::Value::List CreateNWebExtensionBookmarksCreateList(
    const NWebExtensionBookmarkTreeNode* node) {
  return CreateBookmarksListImpl(node);
}

base::Value::List CreateNWebExtensionBookmarksMoveList(
    const NWebExtensionBookmarkTreeNode* node) {
  return CreateBookmarksListImpl(node);
}

base::Value::List CreateNWebExtensionBookmarksUpdateList(
    const NWebExtensionBookmarkTreeNode* node) {
  return CreateBookmarksListImpl(node);
}

base::Value::List CreateNWebExtensionBookmarksOnCreatedList(
    const char* id,
    const NWebExtensionBookmarkTreeNode* node) {
  return CreateBookmarksListImpl(id, node,
                                 NWebExtensionBookmarkTreeNodeToValue);
}

base::Value::List CreateNWebExtensionBookmarksOnRemovedList(
    const char* id,
    const NWebExtensionBookmarksRemoveInfo* info) {
  return CreateBookmarksListImpl(id, info,
                                 NWebExtensionBookmarksRemoveInfoToValue);
}

base::Value::List CreateNWebExtensionBookmarksOnChangedList(
    const char* id,
    const NWebExtensionBookmarksChangeInfo* info) {
  return CreateBookmarksListImpl(id, info,
                                 NWebExtensionBookmarksChangeInfoToValue);
}

base::Value::List CreateNWebExtensionBookmarksOnMovedList(
    const char* id,
    const NWebExtensionBookmarksMoveInfo* info) {
  return CreateBookmarksListImpl(id, info,
                                 NWebExtensionBookmarksMoveInfoToValue);
}

base::Value::List CreateNWebExtensionBookmarksOnChildrenReorderedList(
    const char* id,
    const NWebExtensionBookmarksReorderInfo* info) {
  return CreateBookmarksListImpl(id, info,
                                 NWebExtensionBookmarksReorderInfoToValue);
}

base::Value::List CreateNWebExtensionBookmarksOnImportBeganList() {
  return CreateBookmarksListImpl();
}

base::Value::List CreateNWebExtensionBookmarksOnImportEndedList() {
  return CreateBookmarksListImpl();
}

}  // namespace extensions
