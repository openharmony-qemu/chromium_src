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

#include "nweb_js_http_auth_result_impl.h"

using namespace OHOS::NWeb;

NWebJSHttpAuthResultImpl::NWebJSHttpAuthResultImpl(
    CefRefPtr<CefAuthCallback> callback)
    : callback_(callback) {}

bool NWebJSHttpAuthResultImpl::Confirm(const std::string& userName,
                                       const std::string& pwd) {
  if (callback_ != nullptr) {
    callback_->Continue(CefString(userName), CefString(pwd));
    return true;
  }
  return false;
}

bool NWebJSHttpAuthResultImpl::IsHttpAuthInfoSaved() {
  if (callback_ != nullptr) {
    return callback_->IsHttpAuthInfoSaved();
  }
  return false;
}

void NWebJSHttpAuthResultImpl::Cancel() {
  if (callback_ != nullptr) {
    callback_->Cancel();
  }
}
 