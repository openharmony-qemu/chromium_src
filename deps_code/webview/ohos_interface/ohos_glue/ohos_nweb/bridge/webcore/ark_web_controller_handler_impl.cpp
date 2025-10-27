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

#include "ohos_nweb/bridge/ark_web_controller_handler_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebControllerHandlerImpl::ArkWebControllerHandlerImpl(
    std::shared_ptr<OHOS::NWeb::NWebControllerHandler> nweb_comtroller_handler)
    : nweb_comtroller_handler_(nweb_comtroller_handler)
{}

int32_t ArkWebControllerHandlerImpl::GetId()
{
    return nweb_comtroller_handler_->GetId();
}

bool ArkWebControllerHandlerImpl::IsFrist()
{
    return nweb_comtroller_handler_->IsFrist();
}

int32_t ArkWebControllerHandlerImpl::GetNWebHandlerId()
{
    return nweb_comtroller_handler_->GetNWebHandlerId();
}

void ArkWebControllerHandlerImpl::SetNWebHandlerById(int32_t nweb_id)
{
    nweb_comtroller_handler_->SetNWebHandlerById(nweb_id);
}
} // namespace OHOS::ArkWeb
