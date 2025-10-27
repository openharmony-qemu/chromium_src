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

#include "ohos_nweb/bridge/ark_web_controller_handler_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebControllerHandlerWrapper::ArkWebControllerHandlerWrapper(
    ArkWebRefPtr<ArkWebControllerHandler> ark_web_comtroller_handler)
    : ark_web_comtroller_handler_(ark_web_comtroller_handler)
{}

int32_t ArkWebControllerHandlerWrapper::GetId()
{
    return ark_web_comtroller_handler_->GetId();
}

bool ArkWebControllerHandlerWrapper::IsFrist()
{
    return ark_web_comtroller_handler_->IsFrist();
}

int32_t ArkWebControllerHandlerWrapper::GetNWebHandlerId()
{
    return ark_web_comtroller_handler_->GetNWebHandlerId();
}

void ArkWebControllerHandlerWrapper::SetNWebHandlerById(int32_t nweb_id)
{
    ark_web_comtroller_handler_->SetNWebHandlerById(nweb_id);
}
} // namespace OHOS::ArkWeb
