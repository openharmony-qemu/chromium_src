/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "nweb_permission_status_query_manager.h"
#include "nweb_hilog.h"

namespace OHOS::NWeb {

static NWebPermissionStatusQueryDelegateCallback* g_permission_status_query_delegate_callback;

void NWebPermissionStatusQueryManager::SetPermissionStatusQueryDelegate(
    NWebPermissionStatusQueryDelegateCallback* callback) {
  g_permission_status_query_delegate_callback = callback;
}

void NWebPermissionStatusQueryManager::QueryPermissionStatus(
    NWebPermissionStatusQuery* query) {
  if (!g_permission_status_query_delegate_callback) {
    WVLOG_E("QueryPermissionStatus callback is null.");
    if (query && query->access_query) {
      query->access_query->ReportQueryResult(1);
    }
    return;
  }

  g_permission_status_query_delegate_callback->onPermissionStatusQuery(query);
}

}
