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

#include "ohos_nweb/src/ndk/scheme_handler/resource_handler.h"

#include "base/logging.h"
#include "ohos_nweb/src/capi/arkweb_scheme_handler.h"
#include "ohos_nweb/src/cef_delegate/nweb_pipe_resource_handler.h"
#include "ohos_nweb/src/ndk/scheme_handler/response.h"

ArkWeb_ResourceHandler_::ArkWeb_ResourceHandler_(
    const ArkWeb_ResourceRequest* resource_request,
    CefRefPtr<OHOS::NWeb::NWebSchemeHandlerFactory> factory,
    const std::string& web_tag,
    bool from_service_worker) {
  pipe_resource_handler = new OHOS::NWeb::NWebPipeResourceHandler(
      resource_request, this, factory, web_tag, from_service_worker);
}

ArkWeb_ResourceHandler_::~ArkWeb_ResourceHandler_() {}

void ArkWeb_ResourceHandler_::DidReceiveResponse(
    const ArkWeb_Response* response) const {
  if (!pipe_resource_handler) {
    LOG(ERROR) << "scheme_handler pipe_resource_handler is nullptr.";
    return;
  }

  if (!response) {
    LOG(ERROR) << "scheme_handler response is nullptr";
    return;
  }

  if (!response->cef_response) {
    LOG(ERROR) << "scheme_handler cef response is nullptr";
    return;
  }

  pipe_resource_handler->DidReceiveResponse(response->cef_response);
}

void ArkWeb_ResourceHandler_::DidReceiveData(const uint8_t* buffer,
                                             int64_t buf_len) const {
  if (!pipe_resource_handler) {
    LOG(ERROR) << "scheme_handler pipe_resource_handler is nullptr.";
    return;
  }

  if (!buffer) {
    LOG(ERROR) << "scheme_handler buffer is nullptr.";
    return;
  }

  pipe_resource_handler->DidReceiveData(buffer, buf_len);
}

void ArkWeb_ResourceHandler_::DidFinish() const {
  if (!pipe_resource_handler) {
    LOG(ERROR) << "scheme_handler pipe_resource_handler is nullptr.";
    return;
  }

  pipe_resource_handler->DidFinish();
}

void ArkWeb_ResourceHandler_::DidFailWithError(
    ArkWeb_NetError error_code) const {
  if (!pipe_resource_handler) {
    LOG(ERROR) << "scheme_handler pipe_resource_handler is nullptr.";
    return;
  }

  pipe_resource_handler->DidFailWithError(error_code);
}
