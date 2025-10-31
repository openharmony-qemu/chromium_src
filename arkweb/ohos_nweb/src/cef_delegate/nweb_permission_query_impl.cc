/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <map>
#include <memory>
#include <shared_mutex>

#include "arkweb/build/features/features.h"
#include "base/logging.h"
#include "capi/nweb_permission_status_query.h"
#include "nweb_access_query_delegate.h"
#include "nweb_common.h"
#include "ohos_cef_ext/include/cef_permission_status_query.h"
#include "ohos_nweb/src/nweb_permission_status_query_manager.h"
#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "ohos_nweb_ex/public/nweb_basic_types_utils.h"
#include "ohos_nweb_ex/public/nweb_common_pub.h"
#endif

#if BUILDFLAG(ARKWEB_CLIPBOARD)
#include "nweb_impl.h"
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)

static std::atomic<int> new_key = 0;
static std::unordered_map<int, std::shared_ptr<OHOS::NWeb::NWebAccessQuery>>
    g_ArkWebMap;
static std::shared_mutex g_ArkWebWebMapSharedLock;

static std::shared_ptr<OHOS::NWeb::NWebAccessQuery> GetQueryByKey(int key) {
  std::shared_lock<std::shared_mutex> lock(g_ArkWebWebMapSharedLock);
  if (auto it = g_ArkWebMap.find(key); it != g_ArkWebMap.end()) {
    return it->second;
  }
  return nullptr;
}

static int InsertQuery(
    std::shared_ptr<OHOS::NWeb::NWebAccessQuery> access_query) {
  std::unique_lock<std::shared_mutex> lock(g_ArkWebWebMapSharedLock);
  g_ArkWebMap[++new_key] = access_query;
  return new_key;
}

static bool DeleteQuery(int query_key) {
  LOG(INFO) << "delete query by key is " << query_key;
  std::unique_lock<std::shared_mutex> lock(g_ArkWebWebMapSharedLock);
  if (g_ArkWebMap.find(query_key) != g_ArkWebMap.end()) {
    g_ArkWebMap.erase(query_key);
    return true;
  }
  return false;
}

#if BUILDFLAG(ARKWEB_NWEB_EX)
ArkWebOptionalString ArkWebAccessQuery_Origin(int key) {
  std::shared_ptr<OHOS::NWeb::NWebAccessQuery> access_query =
      GetQueryByKey(key);
  std::optional<std::string> result = "";

  if (access_query == nullptr) {
    result = std::nullopt;
  } else {
    result = access_query->Origin();
  }
  return NWebBasicTypesUtils::OptionalStringClassToStruct(result,
                                                          FreeAllocMemory);
}
#endif

int ArkWebAccessQuery_ResourceAccessId(int key) {
  std::shared_ptr<OHOS::NWeb::NWebAccessQuery> access_query =
      GetQueryByKey(key);
  if (access_query == nullptr) {
    return -1;
  }
  return access_query->ResourceAcessId();
}

void ArkWebAccessQuery_ReportQueryResult(int32_t status, int key) {
  std::shared_ptr<OHOS::NWeb::NWebAccessQuery> access_query =
      GetQueryByKey(key);
  if (access_query == nullptr) {
    return;
  }
  return access_query->ReportQueryResult(status);
}

void CefPermissionQuery::GetPermissionStatusAsync(
    CefRefPtr<CefAccessQuery> query) {
  std::shared_ptr<OHOS::NWeb::NWebAccessQuery> access_query =
      std::make_shared<OHOS::NWeb::NWebAccessQueryDelegate>(query);
#if BUILDFLAG(ARKWEB_NWEB_EX)
  if (IsNativeApiEnable()) {
    int cef_access_query_key = InsertQuery(access_query);
    ArkWebAccessQuery arkWebAccessQuery = {
        .cef_access_query_key = cef_access_query_key,
        .origin = ArkWebAccessQuery_Origin,
        .resource_access_id = ArkWebAccessQuery_ResourceAccessId,
        .report_query_result = ArkWebAccessQuery_ReportQueryResult,
        .report_delete_by_key = DeleteQuery};
    ArkWebPermissionStatusQuery arkWebPermissionStatusQuery = {
        .arkweb_access_query = &arkWebAccessQuery};
    if (access_query != nullptr) {
      arkWebPermissionStatusQuery.resource_id = access_query->ResourceAcessId();
      arkWebPermissionStatusQuery.origin_url =
          NWebBasicTypesUtils::StringClassToStruct(access_query->Origin(),
                                                   FreeAllocMemory);
    }
    OHOS::NWeb::NWebPermissionStatusQueryManager::QueryPermissionStatus(
        &arkWebPermissionStatusQuery);
    return;
  }
#endif  // ARKWEB_NWEB_EX

  NWebPermissionStatusQuery* nweb_query =
      new NWebPermissionStatusQuery(access_query);

  OHOS::NWeb::NWebPermissionStatusQueryManager::QueryPermissionStatus(
      nweb_query);
}

#if BUILDFLAG(ARKWEB_CLIPBOARD)
bool CefPermissionQuery::IsClipboardSitePermissionEnabled() {
  return OHOS::NWeb::NWebImpl::IsClipboardSitePermissionEnabled();
}
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)