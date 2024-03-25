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

#include "ohos_nweb/src/cef_delegate/nweb_access_request_delegate.h"
#include "base/logging.h"
#include "cef/include/cef_permission_request.h"

namespace OHOS::NWeb {
NWebAccessRequestDelegate::NWebAccessRequestDelegate(
    CefRefPtr<CefAccessRequest> request)
    : request_(request) {}
#if defined(OHOS_WEBRTC)
NWebAccessRequestDelegate::~NWebAccessRequestDelegate() {
  if (request_ != nullptr) {
    request_->ReportRequestResult(false);
  }
}
#endif // defined(OHOS_WEBRTC)

std::string NWebAccessRequestDelegate::Origin() {
  if (request_ != nullptr) {
    return request_->Origin();
  }
  return "";
}

int NWebAccessRequestDelegate::ResourceAcessId() {
  if (request_ != nullptr) {
    return request_->ResourceAcessId();
  }
  return -1;
}

void NWebAccessRequestDelegate::Agree(int resourceId) {
  if (request_ != nullptr) {
    if (resourceId == ResourceAcessId()) {
      request_->ReportRequestResult(true);
    } else {
      request_->ReportRequestResult(false);
    }
  }
}

void NWebAccessRequestDelegate::Refuse() {
  if (request_ != nullptr) {
    request_->ReportRequestResult(false);
  }
}

#if defined(OHOS_WEBRTC)
NWebScreenCaptureAccessRequestDelegate::NWebScreenCaptureAccessRequestDelegate(CefRefPtr<CefScreenCaptureAccessRequest> request)
    : request_(request) {}
NWebScreenCaptureAccessRequestDelegate::~NWebScreenCaptureAccessRequestDelegate() {
  if (request_ != nullptr) {
    request_->ReportRequestResult(false);
  }
}

std::string NWebScreenCaptureAccessRequestDelegate::Origin() {
  if (request_ != nullptr) {
    return request_->Origin();
  }
  return "";
}

void NWebScreenCaptureAccessRequestDelegate::Agree(std::shared_ptr<NWebScreenCaptureConfig> config) {
  if (request_ != nullptr) {
    request_->SetCaptureMode(config ? config->GetMode() : 0);
    request_->SetCaptureSourceId(config ? config->GetSourceId() : -1);
    request_->ReportRequestResult(true);
  }
}

void NWebScreenCaptureAccessRequestDelegate::Refuse() {
  if (request_ != nullptr) {
    request_->ReportRequestResult(false);
  }
}
#endif // defined(OHOS_WEBRTC)
}  // namespace OHOS::NWeb
