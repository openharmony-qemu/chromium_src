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

#ifndef NWEB_JS_SSL_SELECT_CERT_RESULT_IMPL_H
#define NWEB_JS_SSL_SELECT_CERT_RESULT_IMPL_H

#include "cef/include/cef_request_handler.h"
#include "nweb_js_ssl_select_cert_result.h"

namespace OHOS::NWeb {
class NWebJSSslSelectCertResultImpl : public NWebJSSslSelectCertResult {
 public:
  NWebJSSslSelectCertResultImpl() = default;

  explicit NWebJSSslSelectCertResultImpl(
      CefRefPtr<CefSelectClientCertificateCallback> callback);

  ~NWebJSSslSelectCertResultImpl() = default;

  void Confirm(const std::string& private_key_file,
               const std::string& cert_chain_file) override;

  void Ignore() override;

  void Cancel() override;

  void Confirm(const std::string& identity, int32_t type) override;

 private:
  CefRefPtr<CefSelectClientCertificateCallback> callback_;
};
}  // namespace OHOS::NWeb

#endif
