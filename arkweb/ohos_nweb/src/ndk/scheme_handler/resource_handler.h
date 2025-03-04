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

#ifndef OHOS_NWEB_SRC_CAPI_SCHEME_HANDLER_RESOURCE_HANDLER_H
#define OHOS_NWEB_SRC_CAPI_SCHEME_HANDLER_RESOURCE_HANDLER_H

#include "ohos_nweb/src/capi/arkweb_scheme_handler.h"
#include "ohos_nweb/src/cef_delegate/nweb_pipe_resource_handler.h"
#include "ohos_nweb/src/ndk/scheme_handler/response.h"

struct ArkWeb_ResourceHandler_ {
  ArkWeb_ResourceHandler_(
      const ArkWeb_ResourceRequest* resource_request,
      CefRefPtr<OHOS::NWeb::NWebSchemeHandlerFactory> factory,
      const std::string& web_tag,
      bool from_service_worker);
  ~ArkWeb_ResourceHandler_();

  void DidReceiveResponse(const ArkWeb_Response* response) const;
  void DidReceiveData(const uint8_t* buffer, int64_t buf_len) const;
  void DidFinish() const;
  void DidFailWithError(ArkWeb_NetError error_code) const;

  CefRefPtr<OHOS::NWeb::NWebPipeResourceHandler> pipe_resource_handler;
};

#endif  // OHOS_NWEB_SRC_CAPI_SCHEME_HANDLER_RESOURCE_HANDLER_H
