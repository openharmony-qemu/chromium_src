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

#include "ohos_nweb/include/nweb_access_request.h"
#include "ohos_nweb/src/capi/nweb_c_api.h"
#include "ohos_nweb/src/nweb_hilog.h"

#ifndef OHOS_NWEB_SRC_NWEB_PERMISSION_REQUEST_H_
#define OHOS_NWEB_SRC_NWEB_PERMISSION_REQUEST_H_

struct NWebPermissionRequest {
  int nweb_id;
  int resource_id;
  std::string origin_url;
  std::shared_ptr<OHOS::NWeb::NWebAccessRequest> access_request;

  NWebPermissionRequest() : nweb_id(-1), access_request(nullptr) {
    WVLOG_I("NWebPermissionRequest() is called");
  }

  ~NWebPermissionRequest() { WVLOG_I("~NWebPermissionRequest() is called"); }

  NWebPermissionRequest(int nweb_id,
                        std::shared_ptr<OHOS::NWeb::NWebAccessRequest> request)
      : nweb_id(nweb_id), access_request(request) {
    WVLOG_I("NWebPermissionRequest() is called");
    if (access_request.get()) {
      resource_id = access_request->ResourceAcessId();
      origin_url = access_request->Origin();
    }
  }
};

#endif  //  OHOS_NWEB_SRC_NWEB_PERMISSION_REQUEST_H_
