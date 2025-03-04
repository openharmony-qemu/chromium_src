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

#ifndef OHOS_NWEB_SRC_NWEB_PERMISSION_STATUS_QUERY_H_
#define OHOS_NWEB_SRC_NWEB_PERMISSION_STATUS_QUERY_H_

#include "ohos_nweb/src/nweb_hilog.h"
#include "ohos_nweb/src/nweb_access_query.h"
#include <string.h>
#include <memory>
#include <string>

struct NWebPermissionStatusQuery {
  int resource_id;
  std::string origin_url;
  std::shared_ptr<OHOS::NWeb::NWebAccessQuery> access_query;

  NWebPermissionStatusQuery()
      : access_query(nullptr) {
    WVLOG_I("NWebPermissionStatusQuery() is called");
  }

  ~NWebPermissionStatusQuery() {
    WVLOG_I("~NWebPermissionStatusQuery() is called");
  }

  NWebPermissionStatusQuery(
      std::shared_ptr<OHOS::NWeb::NWebAccessQuery> query)
      : access_query(query) {
    WVLOG_I("NWebPermissionStatusQuery() is called");
    if (access_query.get()) {
      resource_id = access_query->ResourceAcessId();
      origin_url = access_query->Origin();
    }
  }
};

#endif  //  OHOS_NWEB_SRC_NWEB_PERMISSION_STATUS_QUERY_H_
