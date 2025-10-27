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

#include "chrome/browser/extensions/api/bookmarks/bookmarks_api.h"
#include "chrome/browser/extensions/bookmarks/bookmarks_error_constants.h"
#include "chrome/common/extensions/api/bookmarks.h"
#include "extensions/browser/extension_function.h"
#include "ohos_cef_ext/libcef/browser/extensions/api/bookmarks/bookmarks_extensions_util.h"
#include "ohos_nweb/src/capi/browser_service/nweb_extension_bookmarks_types.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_bookmarks_cef_delegate.h"

namespace extensions {

namespace {

#define NWEB_BOOKMARKS_SAFE_FREE(ptr) \
  do {                                \
    if (ptr) {                        \
      free(ptr);                      \
      ptr = nullptr;                  \
    }                                 \
  } while (0)

void NWebExtensionBookmarksGetParamRelease(
    NWebExtensionBookmarksGetParam* param) {
  if (!param) {
    return;
  }
  if (param->id) {
    for (uint32_t i = 0; i < param->count; ++i) {
      NWEB_BOOKMARKS_SAFE_FREE(param->id[i]);
    }
    NWEB_BOOKMARKS_SAFE_FREE(param->id);
  }
  param->count = 0;
}

void NWebExtensionBookmarksSearchParamRelease(
    NWebExtensionBookmarksSearchParam* param) {
  if (!param) {
    return;
  }
  NWEB_BOOKMARKS_SAFE_FREE(param->query);
  NWEB_BOOKMARKS_SAFE_FREE(param->url);
  NWEB_BOOKMARKS_SAFE_FREE(param->title);
}

void NWebExtensionBookmarksCreateDetailsRelease(
    NWebExtensionBookmarksCreateDetails* details) {
  if (!details) {
    return;
  }
  NWEB_BOOKMARKS_SAFE_FREE(details->parentId);
  NWEB_BOOKMARKS_SAFE_FREE(details->index);
  NWEB_BOOKMARKS_SAFE_FREE(details->title);
  NWEB_BOOKMARKS_SAFE_FREE(details->url);
}

void NWebExtensionBookmarksMoveParamRelease(
    NWebExtensionBookmarksMoveParam* param) {
  if (!param) {
    return;
  }
  NWEB_BOOKMARKS_SAFE_FREE(param->index);
  NWEB_BOOKMARKS_SAFE_FREE(param->id);
  NWEB_BOOKMARKS_SAFE_FREE(param->parentId);
}

void NWebExtensionBookmarksUpdateParamRelease(
    NWebExtensionBookmarksUpdateParam* param) {
  if (!param) {
    return;
  }
  NWEB_BOOKMARKS_SAFE_FREE(param->id);
  NWEB_BOOKMARKS_SAFE_FREE(param->url);
  NWEB_BOOKMARKS_SAFE_FREE(param->title);
}

// Note: If returns false, the caller MUST call
// NWebExtensionBookmarksGetParamRelease to free any partially allocated memory
// in getParam
bool BookmarksBuildGetParams(
    const std::optional<api::bookmarks::Get::Params>& params,
    NWebExtensionBookmarksGetParam* getParam) {
  if (!params) {
    return false;
  }

  std::vector<std::string> idList;
  if (params->id_or_id_list.as_strings) {
    idList = *params->id_or_id_list.as_strings;
  } else {
    idList.push_back(*params->id_or_id_list.as_string);
  }
  if (idList.empty()) {
    return false;
  }

  getParam->count = static_cast<uint32_t>(idList.size());
  getParam->id = static_cast<char**>(calloc(getParam->count, sizeof(char*)));
  if (!getParam->id) {
    return false;
  }

  for (uint32_t i = 0; i < getParam->count; ++i) {
    getParam->id[i] = strdup(idList[i].c_str());
    if (!getParam->id[i]) {
      return false;
    }
  }
  return true;
}

// Note: If returns false, the caller MUST call
// NWebExtensionBookmarksSearchParamRelease to free any partially allocated
// memory in searchParam
bool BookmarksBuildSearchParams(
    const std::optional<api::bookmarks::Search::Params>& params,
    NWebExtensionBookmarksSearchParam* searchParam) {
  if (!params) {
    return false;
  }

  if (params->query.as_string) {
    searchParam->query = strdup(params->query.as_string.value().c_str());
    if (!searchParam->query) {
      return false;
    }
  } else if (params->query.as_object) {
    const auto& queryObj = *params->query.as_object;
    if (queryObj.query) {
      searchParam->query = strdup(queryObj.query.value().c_str());
      if (!searchParam->query) {
        return false;
      }
    }
    if (queryObj.url) {
      searchParam->url = strdup(queryObj.url.value().c_str());
      if (!searchParam->url) {
        return false;
      }
    }
    if (queryObj.title) {
      searchParam->title = strdup(queryObj.title.value().c_str());
      if (!searchParam->title) {
        return false;
      }
    }
  } else {
    return false;
  }
  return true;
}

// Note: If returns false, the caller MUST call
// NWebExtensionBookmarksCreateDetailsRelease to free any partially allocated
// memory in details
bool BookmarksBuildCreateParams(
    std::optional<api::bookmarks::Create::Params>& params,
    NWebExtensionBookmarksCreateDetails* details) {
  if (!params) {
    return false;
  }

  if (params->bookmark.parent_id) {
    details->parentId = strdup(params->bookmark.parent_id.value().c_str());
    if (!details->parentId) {
      return false;
    }
  }
  if (params->bookmark.index) {
    details->index = static_cast<int*>(calloc(1, sizeof(int)));
    if (!details->index) {
      return false;
    }
    *details->index = params->bookmark.index.value();
  }
  if (params->bookmark.title) {
    details->title = strdup(params->bookmark.title.value().c_str());
    if (!details->title) {
      return false;
    }
  }
  if (params->bookmark.url) {
    details->url = strdup(params->bookmark.url.value().c_str());
    if (!details->url) {
      return false;
    }
  }

  return true;
}

// Note: If returns false, the caller MUST call
// NWebExtensionBookmarksMoveParamRelease to free any partially allocated memory
// in moveParam
bool BookmarksBuildMoveParams(
    const std::optional<api::bookmarks::Move::Params>& params,
    NWebExtensionBookmarksMoveParam* moveParam) {
  if (!params) {
    return false;
  }

  moveParam->id = strdup(params->id.c_str());
  if (!moveParam->id) {
    return false;
  }
  if (params->destination.parent_id) {
    moveParam->parentId = strdup(params->destination.parent_id->c_str());
    if (!moveParam->parentId) {
      return false;
    }
  }
  if (params->destination.index) {
    moveParam->index = static_cast<int*>(calloc(1, sizeof(int)));
    if (!moveParam->index) {
      return false;
    }
    *moveParam->index = *(params->destination.index);
  }

  return true;
}

// Note: If returns false, the caller MUST call
// NWebExtensionBookmarksUpdateParamRelease to free any partially allocated
// memory in updateParam
bool BookmarksBuildUpdateParams(
    const std::optional<api::bookmarks::Update::Params>& params,
    NWebExtensionBookmarksUpdateParam* updateParam) {
  if (!params) {
    return false;
  }

  updateParam->id = strdup(params->id.c_str());
  if (!updateParam->id) {
    return false;
  }
  if (params->changes.title) {
    updateParam->title = strdup(params->changes.title->c_str());
    if (!updateParam->title) {
      return false;
    }
  }
  if (params->changes.url) {
    updateParam->url = strdup(params->changes.url->c_str());
    if (!updateParam->url) {
      return false;
    }
  }

  return true;
}

}  // namespace

ExtensionFunction::ResponseAction BookmarksGetFunction::Run() {
  std::optional<api::bookmarks::Get::Params> params =
      api::bookmarks::Get::Params::Create(args());
  if (!params) {
    return RespondNow(BadMessage());
  }

  NWebExtensionBookmarksGetParam getParam = {0};
  if (!BookmarksBuildGetParams(params, &getParam)) {
    NWebExtensionBookmarksGetParamRelease(&getParam);
    LOG(ERROR) << "BookmarksGetFunction failed to build get parameters";
    return RespondNow(BadMessage());
  }

  call_get_bookmarks_ = true;
  bool success =
      OHOS::NWeb::NWebExtensionBookmarksCefDelegate::GetInstance().OnGet(
          &getParam, base::BindRepeating(&BookmarksGetFunction::GetCallback,
                                         weak_ptr_factory_.GetWeakPtr()));
  call_get_bookmarks_ = false;
  NWebExtensionBookmarksGetParamRelease(&getParam);

  if (did_respond()) {
    LOG(INFO) << "BookmarksGetFunction did_respond";
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    LOG(INFO) << "BookmarksGetFunction AddRef";
    return RespondLater();
  } else {
    return RespondNow(Error("not support bookmarks.get"));
  }
}

void BookmarksGetFunction::GetCallback(
    const base::WeakPtr<BookmarksGetFunction>& function,
    uint32_t bookmarkCount,
    const NWebExtensionBookmarkTreeNode* bookmarks,
    const char* error) {
  DCHECK(function);
  if (!function) {
    LOG(ERROR) << "BookmarksGetFunction is empty!!!!";
    return;
  }

  if (error) {
    function->Respond(function->Error(std::string(error)));
  } else {
    function->Respond(function->ArgumentList(
        CreateNWebExtensionBookmarksGetList(bookmarkCount, bookmarks)));
  }

  if (!function->call_get_bookmarks_) {
    LOG(INFO) << "BookmarksGetFunction Release";
    function->Release();
  }
}

ExtensionFunction::ResponseAction BookmarksGetChildrenFunction::Run() {
  std::optional<api::bookmarks::GetChildren::Params> params =
      api::bookmarks::GetChildren::Params::Create(args());
  if (!params) {
    return RespondNow(BadMessage());
  }

  char* id = strdup(params->id.c_str());
  if (!id) {
    LOG(ERROR) << "BookmarksGetChildrenFunction failed to build getChildren "
                  "parameters";
    return RespondNow(BadMessage());
  }

  call_getchildren_bookmarks_ = true;
  bool success =
      OHOS::NWeb::NWebExtensionBookmarksCefDelegate::GetInstance()
          .OnGetChildren(id,
                         base::BindRepeating(
                             &BookmarksGetChildrenFunction::GetChildrenCallback,
                             weak_ptr_factory_.GetWeakPtr()));
  call_getchildren_bookmarks_ = false;
  NWEB_BOOKMARKS_SAFE_FREE(id);

  if (did_respond()) {
    LOG(INFO) << "BookmarksGetChildrenFunction did_respond";
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    LOG(INFO) << "BookmarksGetChildrenFunction AddRef";
    return RespondLater();
  } else {
    return RespondNow(Error("not support bookmarks.getChildren"));
  }
}

void BookmarksGetChildrenFunction::GetChildrenCallback(
    const base::WeakPtr<BookmarksGetChildrenFunction>& function,
    uint32_t bookmarkCount,
    const NWebExtensionBookmarkTreeNode* bookmarks,
    const char* error) {
  DCHECK(function);
  if (!function) {
    LOG(ERROR) << "BookmarksGetChildrenFunction is empty!!!!";
    return;
  }

  if (error) {
    function->Respond(function->Error(std::string(error)));
  } else {
    function->Respond(function->ArgumentList(
        CreateNWebExtensionBookmarksGetChildrenList(bookmarkCount, bookmarks)));
  }

  if (!function->call_getchildren_bookmarks_) {
    LOG(INFO) << "BookmarksGetChildrenFunction Release";
    function->Release();
  }
}

ExtensionFunction::ResponseAction BookmarksGetRecentFunction::Run() {
  std::optional<api::bookmarks::GetRecent::Params> params =
      api::bookmarks::GetRecent::Params::Create(args());
  if (!params) {
    return RespondNow(BadMessage());
  }
  if (params->number_of_items < 1) {
    return RespondNow(Error("numberOfItems cannot be less than 1."));
  }

  call_getrecent_bookmarks_ = true;
  bool success =
      OHOS::NWeb::NWebExtensionBookmarksCefDelegate::GetInstance().OnGetRecent(
          params->number_of_items,
          base::BindRepeating(&BookmarksGetRecentFunction::GetRecentCallback,
                              weak_ptr_factory_.GetWeakPtr()));
  call_getrecent_bookmarks_ = false;

  if (did_respond()) {
    LOG(INFO) << "BookmarksGetRecentFunction did_respond";
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    LOG(INFO) << "BookmarksGetRecentFunction AddRef";
    return RespondLater();
  } else {
    return RespondNow(Error("not support bookmarks.getRecent"));
  }
}

void BookmarksGetRecentFunction::GetRecentCallback(
    const base::WeakPtr<BookmarksGetRecentFunction>& function,
    uint32_t bookmarkCount,
    const NWebExtensionBookmarkTreeNode* bookmarks,
    const char* error) {
  DCHECK(function);
  if (!function) {
    LOG(ERROR) << "BookmarksGetRecentFunction is empty!!!!";
    return;
  }

  if (error) {
    function->Respond(function->Error(std::string(error)));
  } else {
    function->Respond(function->ArgumentList(
        CreateNWebExtensionBookmarksGetRecentList(bookmarkCount, bookmarks)));
  }

  if (!function->call_getrecent_bookmarks_) {
    LOG(INFO) << "BookmarksGetRecentFunction Release";
    function->Release();
  }
}

ExtensionFunction::ResponseAction BookmarksGetTreeFunction::Run() {
  call_gettree_bookmarks_ = true;
  bool success =
      OHOS::NWeb::NWebExtensionBookmarksCefDelegate::GetInstance().OnGetTree(
          base::BindRepeating(&BookmarksGetTreeFunction::GetTreeCallback,
                              weak_ptr_factory_.GetWeakPtr()));
  call_gettree_bookmarks_ = false;

  if (did_respond()) {
    LOG(INFO) << "BookmarksGetTreeFunction did_respond";
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    LOG(INFO) << "BookmarksGetTreeFunction AddRef";
    return RespondLater();
  } else {
    return RespondNow(Error("not support bookmarks.getTree"));
  }
}

void BookmarksGetTreeFunction::GetTreeCallback(
    const base::WeakPtr<BookmarksGetTreeFunction>& function,
    uint32_t bookmarkCount,
    const NWebExtensionBookmarkTreeNode* bookmarks,
    const char* error) {
  DCHECK(function);
  if (!function) {
    LOG(ERROR) << "BookmarksGetTreeFunction is empty!!!!";
    return;
  }

  if (error) {
    function->Respond(function->Error(std::string(error)));
  } else {
    function->Respond(function->ArgumentList(
        CreateNWebExtensionBookmarksGetTreeList(bookmarkCount, bookmarks)));
  }

  if (!function->call_gettree_bookmarks_) {
    LOG(INFO) << "BookmarksGetTreeFunction Release";
    function->Release();
  }
}

ExtensionFunction::ResponseAction BookmarksGetSubTreeFunction::Run() {
  std::optional<api::bookmarks::GetSubTree::Params> params =
      api::bookmarks::GetSubTree::Params::Create(args());
  if (!params) {
    return RespondNow(BadMessage());
  }

  char* id = strdup(params->id.c_str());
  if (!id) {
    LOG(ERROR)
        << "BookmarksGetSubTreeFunction failed to build getSubTree parameters";
    return RespondNow(BadMessage());
  }

  call_getsubtree_bookmarks_ = true;
  bool success =
      OHOS::NWeb::NWebExtensionBookmarksCefDelegate::GetInstance().OnGetSubTree(
          id,
          base::BindRepeating(&BookmarksGetSubTreeFunction::GetSubTreeCallback,
                              weak_ptr_factory_.GetWeakPtr()));
  call_getsubtree_bookmarks_ = false;
  NWEB_BOOKMARKS_SAFE_FREE(id);

  if (did_respond()) {
    LOG(INFO) << "BookmarksGetSubTreeFunction did_respond";
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    LOG(INFO) << "BookmarksGetSubTreeFunction AddRef";
    return RespondLater();
  } else {
    return RespondNow(Error("not support bookmarks.getSubTree"));
  }
}

void BookmarksGetSubTreeFunction::GetSubTreeCallback(
    const base::WeakPtr<BookmarksGetSubTreeFunction>& function,
    uint32_t bookmarkCount,
    const NWebExtensionBookmarkTreeNode* bookmarks,
    const char* error) {
  DCHECK(function);
  if (!function) {
    LOG(ERROR) << "BookmarksGetSubTreeFunction is empty!!!!";
    return;
  }

  if (error) {
    function->Respond(function->Error(std::string(error)));
  } else {
    function->Respond(function->ArgumentList(
        CreateNWebExtensionBookmarksGetChildrenList(bookmarkCount, bookmarks)));
  }

  if (!function->call_getsubtree_bookmarks_) {
    LOG(INFO) << "BookmarksGetSubTreeFunction Release";
    function->Release();
  }
}

ExtensionFunction::ResponseAction BookmarksSearchFunction::Run() {
  std::optional<api::bookmarks::Search::Params> params =
      api::bookmarks::Search::Params::Create(args());
  if (!params) {
    return RespondNow(BadMessage());
  }

  NWebExtensionBookmarksSearchParam searchParam = {0};
  if (!BookmarksBuildSearchParams(params, &searchParam)) {
    NWebExtensionBookmarksSearchParamRelease(&searchParam);
    LOG(ERROR) << "BookmarksSearchFunction failed to build search parameters";
    return RespondNow(BadMessage());
  }

  if (searchParam.url && searchParam.url[0] != '\0') {
    GURL url(searchParam.url);
    if (url.is_valid()) {
      char* newUrl = strdup(url.spec().c_str());
      if (!newUrl) {
        NWebExtensionBookmarksSearchParamRelease(&searchParam);
        LOG(ERROR) << "BookmarksSearchFunction failed to allocate url";
        return RespondNow(Error("Internal memory error"));
      }
      NWEB_BOOKMARKS_SAFE_FREE(searchParam.url);
      searchParam.url = newUrl;
    }
  }

  call_search_bookmarks_ = true;
  bool success =
      OHOS::NWeb::NWebExtensionBookmarksCefDelegate::GetInstance().OnSearch(
          &searchParam,
          base::BindRepeating(&BookmarksSearchFunction::SearchCallback,
                              weak_ptr_factory_.GetWeakPtr()));
  call_search_bookmarks_ = false;
  NWebExtensionBookmarksSearchParamRelease(&searchParam);

  if (did_respond()) {
    LOG(INFO) << "BookmarksSearchFunction did_respond";
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    LOG(INFO) << "BookmarksSearchFunction AddRef";
    return RespondLater();
  } else {
    return RespondNow(Error("not support bookmarks.search"));
  }
}

void BookmarksSearchFunction::SearchCallback(
    const base::WeakPtr<BookmarksSearchFunction>& function,
    uint32_t bookmarkCount,
    const NWebExtensionBookmarkTreeNode* bookmarks,
    const char* error) {
  DCHECK(function);
  if (!function) {
    LOG(ERROR) << "BookmarksSearchFunction is empty!!!!";
    return;
  }

  if (error) {
    function->Respond(function->Error(std::string(error)));
  } else {
    function->Respond(function->ArgumentList(
        CreateNWebExtensionBookmarksSearchList(bookmarkCount, bookmarks)));
  }

  if (!function->call_search_bookmarks_) {
    LOG(INFO) << "BookmarksSearchFunction Release";
    function->Release();
  }
}

ExtensionFunction::ResponseAction BookmarksRemoveFunction::Run() {
  std::optional<api::bookmarks::Remove::Params> params =
      api::bookmarks::Remove::Params::Create(args());
  if (!params) {
    return RespondNow(BadMessage());
  }

  char* id = strdup(params->id.c_str());
  if (!id) {
    LOG(ERROR) << "BookmarksRemoveFunction failed to build remove parameters";
    return RespondNow(BadMessage());
  }

  call_remove_bookmarks_ = true;
  bool success =
      OHOS::NWeb::NWebExtensionBookmarksCefDelegate::GetInstance().OnRemove(
          id, base::BindRepeating(&BookmarksRemoveFunction::RemoveCallback,
                                  weak_ptr_factory_.GetWeakPtr()));
  call_remove_bookmarks_ = false;
  NWEB_BOOKMARKS_SAFE_FREE(id);

  if (did_respond()) {
    LOG(INFO) << "BookmarksRemoveFunction did_respond";
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    LOG(INFO) << "BookmarksRemoveFunction AddRef";
    return RespondLater();
  } else {
    return RespondNow(Error("not support bookmarks.remove"));
  }
}

void BookmarksRemoveFunction::RemoveCallback(
    const base::WeakPtr<BookmarksRemoveFunction>& function,
    const char* error) {
  DCHECK(function);
  if (!function) {
    LOG(ERROR) << "BookmarksRemoveFunction is empty!!!!";
    return;
  }

  if (error) {
    function->Respond(function->Error(std::string(error)));
  } else {
    function->Respond(function->NoArguments());
  }

  if (!function->call_remove_bookmarks_) {
    LOG(INFO) << "BookmarksRemoveFunction Release";
    function->Release();
  }
}

ExtensionFunction::ResponseAction BookmarksRemoveTreeFunction::Run() {
  std::optional<api::bookmarks::Remove::Params> params =
      api::bookmarks::Remove::Params::Create(args());
  if (!params) {
    return RespondNow(BadMessage());
  }

  char* id = strdup(params->id.c_str());
  if (!id) {
    LOG(ERROR)
        << "BookmarksRemoveTreeFunction failed to build removeTree parameters";
    return RespondNow(BadMessage());
  }

  call_removetree_bookmarks_ = true;
  bool success =
      OHOS::NWeb::NWebExtensionBookmarksCefDelegate::GetInstance().OnRemoveTree(
          id,
          base::BindRepeating(&BookmarksRemoveTreeFunction::RemoveTreeCallback,
                              weak_ptr_factory_.GetWeakPtr()));
  call_removetree_bookmarks_ = false;
  NWEB_BOOKMARKS_SAFE_FREE(id);

  if (did_respond()) {
    LOG(INFO) << "BookmarksRemoveTreeFunction did_respond";
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    LOG(INFO) << "BookmarksRemoveTreeFunction AddRef";
    return RespondLater();
  } else {
    return RespondNow(Error("not support bookmarks.removeTree"));
  }
}

void BookmarksRemoveTreeFunction::RemoveTreeCallback(
    const base::WeakPtr<BookmarksRemoveTreeFunction>& function,
    const char* error) {
  DCHECK(function);
  if (!function) {
    LOG(ERROR) << "BookmarksRemoveTreeFunction is empty!!!!";
    return;
  }

  if (error) {
    function->Respond(function->Error(std::string(error)));
  } else {
    function->Respond(function->NoArguments());
  }

  if (!function->call_removetree_bookmarks_) {
    LOG(INFO) << "BookmarksRemoveTreeFunction Release";
    function->Release();
  }
}

ExtensionFunction::ResponseAction BookmarksCreateFunction::Run() {
  std::optional<api::bookmarks::Create::Params> params =
      api::bookmarks::Create::Params::Create(args());
  if (!params) {
    return RespondNow(BadMessage());
  }

  NWebExtensionBookmarksCreateDetails details = {0};
  if (!BookmarksBuildCreateParams(params, &details)) {
    NWebExtensionBookmarksCreateDetailsRelease(&details);
    LOG(ERROR) << "BookmarksCreateFunction failed to build create parameters";
    return RespondNow(BadMessage());
  }

  if (details.url) {
    std::string urlStr(details.url);
    if (!urlStr.empty()) {
      GURL url(urlStr);
      if (!url.is_valid()) {
        NWebExtensionBookmarksCreateDetailsRelease(&details);
        return RespondNow(Error(bookmarks_errors::kInvalidUrlError));
      }
      NWEB_BOOKMARKS_SAFE_FREE(details.url);
      details.url = strdup(url.spec().c_str());
      if (!details.url) {
        NWebExtensionBookmarksCreateDetailsRelease(&details);
        LOG(ERROR) << "BookmarksCreateFunction failed to allocate url";
        return RespondNow(Error("Internal memory error"));
      }
    }
  }

  call_create_bookmarks_ = true;
  bool success =
      OHOS::NWeb::NWebExtensionBookmarksCefDelegate::GetInstance().OnCreate(
          &details,
          base::BindRepeating(&BookmarksCreateFunction::CreateCallback,
                              weak_ptr_factory_.GetWeakPtr()));
  call_create_bookmarks_ = false;
  NWebExtensionBookmarksCreateDetailsRelease(&details);

  if (did_respond()) {
    LOG(INFO) << "BookmarksCreateFunction did_respond";
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    LOG(INFO) << "BookmarksCreateFunction AddRef";
    return RespondLater();
  } else {
    return RespondNow(Error("not support bookmarks.create"));
  }
}

void BookmarksCreateFunction::CreateCallback(
    const base::WeakPtr<BookmarksCreateFunction>& function,
    const NWebExtensionBookmarkTreeNode* bookmark,
    const char* error) {
  DCHECK(function);
  if (!function) {
    LOG(ERROR) << "BookmarksCreateCallback is empty!!!!";
    return;
  }

  if (error) {
    function->Respond(function->Error(std::string(error)));
  } else {
    function->Respond(function->ArgumentList(
        CreateNWebExtensionBookmarksCreateList(bookmark)));
  }

  if (!function->call_create_bookmarks_) {
    LOG(INFO) << "BookmarksCreateFunction Release";
    function->Release();
  }
}

ExtensionFunction::ResponseAction BookmarksMoveFunction::Run() {
  std::optional<api::bookmarks::Move::Params> params =
      api::bookmarks::Move::Params::Create(args());
  if (!params) {
    return RespondNow(BadMessage());
  }

  NWebExtensionBookmarksMoveParam moveParam = {0};
  if (!BookmarksBuildMoveParams(params, &moveParam)) {
    NWebExtensionBookmarksMoveParamRelease(&moveParam);
    LOG(ERROR) << "BookmarksMoveFunction failed to build move parameters";
    return RespondNow(BadMessage());
  }

  call_move_bookmarks_ = true;
  bool success =
      OHOS::NWeb::NWebExtensionBookmarksCefDelegate::GetInstance().OnMove(
          &moveParam, base::BindRepeating(&BookmarksMoveFunction::MoveCallback,
                                          weak_ptr_factory_.GetWeakPtr()));
  call_move_bookmarks_ = false;
  NWebExtensionBookmarksMoveParamRelease(&moveParam);

  if (did_respond()) {
    LOG(INFO) << "BookmarksMoveFunction did_respond";
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    LOG(INFO) << "BookmarksMoveFunction AddRef";
    return RespondLater();
  } else {
    return RespondNow(Error("not support bookmarks.move"));
  }
}

void BookmarksMoveFunction::MoveCallback(
    const base::WeakPtr<BookmarksMoveFunction>& function,
    const NWebExtensionBookmarkTreeNode* bookmark,
    const char* error) {
  DCHECK(function);
  if (!function) {
    LOG(ERROR) << "BookmarksMoveFunction is empty!!!!";
    return;
  }

  if (error) {
    function->Respond(function->Error(std::string(error)));
  } else {
    function->Respond(
        function->ArgumentList(CreateNWebExtensionBookmarksMoveList(bookmark)));
  }

  if (!function->call_move_bookmarks_) {
    LOG(INFO) << "BookmarksMoveFunction Release";
    function->Release();
  }
}

ExtensionFunction::ResponseAction BookmarksUpdateFunction::Run() {
  std::optional<api::bookmarks::Update::Params> params =
      api::bookmarks::Update::Params::Create(args());
  if (!params) {
    return RespondNow(BadMessage());
  }

  NWebExtensionBookmarksUpdateParam updateParam = {0};
  if (!BookmarksBuildUpdateParams(params, &updateParam)) {
    NWebExtensionBookmarksUpdateParamRelease(&updateParam);
    LOG(ERROR) << "BookmarksUpdateFunction failed to build update parameters";
    return RespondNow(BadMessage());
  }

  if (updateParam.url) {
    std::string urlStr(updateParam.url);
    if (!urlStr.empty()) {
      GURL url(urlStr);
      if (!url.is_valid()) {
        NWebExtensionBookmarksUpdateParamRelease(&updateParam);
        return RespondNow(Error(bookmarks_errors::kInvalidUrlError));
      }
      NWEB_BOOKMARKS_SAFE_FREE(updateParam.url);
      updateParam.url = strdup(url.spec().c_str());
      if (!updateParam.url) {
        NWebExtensionBookmarksUpdateParamRelease(&updateParam);
        LOG(ERROR) << "BookmarksUpdateFunction failed to allocate url";
        return RespondNow(Error("Internal memory error"));
      }
    }
  }

  call_update_bookmarks_ = true;
  bool success =
      OHOS::NWeb::NWebExtensionBookmarksCefDelegate::GetInstance().OnUpdate(
          &updateParam,
          base::BindRepeating(&BookmarksUpdateFunction::UpdateCallback,
                              weak_ptr_factory_.GetWeakPtr()));
  call_update_bookmarks_ = false;
  NWebExtensionBookmarksUpdateParamRelease(&updateParam);

  if (did_respond()) {
    LOG(INFO) << "BookmarksUpdateFunction did_respond";
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    LOG(INFO) << "BookmarksUpdateFunction AddRef";
    return RespondLater();
  } else {
    return RespondNow(Error("not support bookmarks.update"));
  }
}

void BookmarksUpdateFunction::UpdateCallback(
    const base::WeakPtr<BookmarksUpdateFunction>& function,
    const NWebExtensionBookmarkTreeNode* bookmark,
    const char* error) {
  DCHECK(function);
  if (!function) {
    LOG(ERROR) << "BookmarksUpdateFunction is empty!";
    return;
  }

  if (error) {
    function->Respond(function->Error(std::string(error)));
  } else {
    function->Respond(function->ArgumentList(
        CreateNWebExtensionBookmarksUpdateList(bookmark)));
  }

  if (!function->call_update_bookmarks_) {
    LOG(INFO) << "BookmarksUpdateFunction Release";
    function->Release();
  }
}

}  // namespace extensions
