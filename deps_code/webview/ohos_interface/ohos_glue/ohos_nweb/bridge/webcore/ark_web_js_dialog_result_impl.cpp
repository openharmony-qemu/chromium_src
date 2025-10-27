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

#include "ohos_nweb/bridge/ark_web_js_dialog_result_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebJsDialogResultImpl::ArkWebJsDialogResultImpl(
    std::shared_ptr<OHOS::NWeb::NWebJSDialogResult> nweb_js_dialog_result)
    : nweb_js_dialog_result_(nweb_js_dialog_result)
{}

void ArkWebJsDialogResultImpl::Cancel()
{
    nweb_js_dialog_result_->Cancel();
}

void ArkWebJsDialogResultImpl::Confirm()
{
    nweb_js_dialog_result_->Confirm();
}

void ArkWebJsDialogResultImpl::Confirm(const ArkWebString& message)
{
    nweb_js_dialog_result_->Confirm(ArkWebStringStructToClass(message));
}

} // namespace OHOS::ArkWeb
