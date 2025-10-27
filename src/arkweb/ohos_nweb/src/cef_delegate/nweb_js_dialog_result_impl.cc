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

#include "nweb_js_dialog_result_impl.h"

using namespace OHOS::NWeb;

NWebJSDialogResultImpl::NWebJSDialogResultImpl(
    CefRefPtr<CefJSDialogCallback> callback)
    : callback_(callback), has_put_(false) {}

void NWebJSDialogResultImpl::Confirm() {
  if (callback_ != nullptr && !has_put_) {
    callback_->Continue(true, CefString(std::string()));
    has_put_ = true;
  }
}

void NWebJSDialogResultImpl::Confirm(const std::string& message) {
  if (callback_ != nullptr && !has_put_) {
    callback_->Continue(true, CefString(message));
    has_put_ = true;
  }
}

void NWebJSDialogResultImpl::Cancel() {
  if (callback_ != nullptr && !has_put_) {
    callback_->Continue(false, CefString(std::string()));
    has_put_ = true;
  }
}
