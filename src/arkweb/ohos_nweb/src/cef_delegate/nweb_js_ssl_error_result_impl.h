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

#ifndef NWEB_JS_SSL_ERROR_RESULT_IMPL_H
#define NWEB_JS_SSL_ERROR_RESULT_IMPL_H

#include "cef/include/cef_callback.h"
#include "cef/ohos_cef_ext/include/arkweb_cef_ssl_callback.h"
#include "nweb_js_ssl_error_result.h"

namespace OHOS::NWeb {
class NWebJSSslErrorResultImpl : public NWebJSSslErrorResult {
 public:
  NWebJSSslErrorResultImpl() = default;
  explicit NWebJSSslErrorResultImpl(CefRefPtr<ArkWebCefSslCallback> callback);
  ~NWebJSSslErrorResultImpl() = default;
  void HandleConfirm() override;
  void HandleCancel() override;
  void HandleCancelV2(bool abortLoading) override;

 private:
  CefRefPtr<ArkWebCefSslCallback> callback_;
};

class NWebJSAllSslErrorResultImpl : public NWebJSAllSslErrorResult {
 public:
  NWebJSAllSslErrorResultImpl() = default;
  explicit NWebJSAllSslErrorResultImpl(CefRefPtr<ArkWebCefSslCallback> callback);
  ~NWebJSAllSslErrorResultImpl() = default;
  void HandleConfirm() override;
  void HandleCancel() override;
  void HandleCancelV2(bool abortLoading) override;

 private:
  CefRefPtr<ArkWebCefSslCallback> callback_;
};
}  // namespace OHOS::NWeb

#endif
