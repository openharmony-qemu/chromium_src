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

#include "nweb_js_ssl_select_cert_result_impl.h"

#include "base/logging.h"

using namespace OHOS::NWeb;

NWebJSSslSelectCertResultImpl::NWebJSSslSelectCertResultImpl(
    CefRefPtr<CefSelectClientCertificateCallback> callback)
    : callback_(callback) {}

void NWebJSSslSelectCertResultImpl::Confirm(
    const std::string& private_key_file,
    const std::string& cert_chain_file) {
  if (callback_ != nullptr) {
    return callback_->AsCefSelectClientCertificateCallbackExt()->Select(
        CefString(private_key_file), CefString(cert_chain_file));
  }
}

void NWebJSSslSelectCertResultImpl::Cancel() {
  if (callback_ != nullptr) {
    return callback_->AsCefSelectClientCertificateCallbackExt()->Cancel();
  }
}

void NWebJSSslSelectCertResultImpl::Ignore() {
  if (callback_ != nullptr) {
    return callback_->AsCefSelectClientCertificateCallbackExt()->Ignore();
  }
}

void NWebJSSslSelectCertResultImpl::Confirm(const std::string& identity, int32_t type) {
  if (callback_ != nullptr) {
    return callback_->AsCefSelectClientCertificateCallbackExt()->Select(
        CefString(identity), type);
  }
}
