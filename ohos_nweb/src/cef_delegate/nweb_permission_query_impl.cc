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
#include "base/logging.h"
#include "cef/include/cef_permission_status_query.h"
#include "ohos_nweb/src/nweb_permission_status_query_manager.h"
#include "capi/nweb_permission_status_query.h"
#include "nweb_access_query_delegate.h"

#include <memory>

void CefPermissionQuery::GetPermissionStatusAsync(CefRefPtr<CefAccessQuery> query) {
  std::shared_ptr<OHOS::NWeb::NWebAccessQuery> access_query =
      std::make_shared<OHOS::NWeb::NWebAccessQueryDelegate>(query);

  NWebPermissionStatusQuery* nweb_query = new NWebPermissionStatusQuery(access_query);

  OHOS::NWeb::NWebPermissionStatusQueryManager::QueryPermissionStatus(nweb_query);
}
