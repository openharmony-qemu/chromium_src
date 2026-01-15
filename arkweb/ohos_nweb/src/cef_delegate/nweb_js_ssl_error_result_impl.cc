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

#include "nweb_js_ssl_error_result_impl.h"

#include "base/logging.h"

using namespace OHOS::NWeb;

NWebJSSslErrorResultImpl::NWebJSSslErrorResultImpl(
    CefRefPtr<ArkWebCefSslCallback> callback)
    : callback_(callback) {}

void NWebJSSslErrorResultImpl::HandleConfirm() {
  if (callback_ != nullptr) {
    LOG_FEEDBACK(INFO, kNavigation) << "OnSslErrorEventReceiveHandleConfirm";
    return callback_->Continue();
  }
}

void NWebJSSslErrorResultImpl::HandleCancel() {
  if (callback_ != nullptr) {
    LOG_FEEDBACK(INFO, kNavigation) << "OnSslErrorEventReceiveHandleCancel";
    return callback_->Cancel();
  }
}

void NWebJSSslErrorResultImpl::HandleCancelV2(bool abortLoading) {
  if (callback_ != nullptr) {
    LOG_FEEDBACK(INFO, kNavigation)
        << "OnSslErrorEventReceiveHandleCancelV2 abortLoading:" << abortLoading;
    return callback_->Cancel(abortLoading);
  }
}

NWebJSAllSslErrorResultImpl::NWebJSAllSslErrorResultImpl(
    CefRefPtr<ArkWebCefSslCallback> callback)
    : callback_(callback) {}

void NWebJSAllSslErrorResultImpl::HandleConfirm() {
  if (callback_ != nullptr) {
    LOG_FEEDBACK(INFO, kNavigation) << "OnSslErrorEventHandleConfirm";
    return callback_->Continue();
  }
}

void NWebJSAllSslErrorResultImpl::HandleCancel() {
  if (callback_ != nullptr) {
    LOG_FEEDBACK(INFO, kNavigation) << "OnSslErrorEventHandleCancel";
    return callback_->Cancel();
  }
}

void NWebJSAllSslErrorResultImpl::HandleCancelV2(bool abortLoading) {
  if (callback_ != nullptr) {
    LOG_FEEDBACK(INFO, kNavigation)
        << "OnSslErrorEventHandleCancelV2 abortLoading:" << abortLoading;
    return callback_->Cancel(abortLoading);
  }
}
