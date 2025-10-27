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

#include "ohos_nweb/bridge/ark_web_access_request_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebAccessRequestImpl::ArkWebAccessRequestImpl(std::shared_ptr<OHOS::NWeb::NWebAccessRequest> nweb_access_request)
    : nweb_access_request_(nweb_access_request)
{}

void ArkWebAccessRequestImpl::Agree(int resource_id)
{
    nweb_access_request_->Agree(resource_id);
}

ArkWebString ArkWebAccessRequestImpl::Origin()
{
    return ArkWebStringClassToStruct(nweb_access_request_->Origin());
}

void ArkWebAccessRequestImpl::Refuse()
{
    nweb_access_request_->Refuse();
}

int ArkWebAccessRequestImpl::ResourceAccessId()
{
    return nweb_access_request_->ResourceAcessId();
}

} // namespace OHOS::ArkWeb
