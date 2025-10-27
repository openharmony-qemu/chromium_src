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

#include "ohos_nweb/bridge/ark_web_url_resource_request_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebUrlResourceRequestImpl::ArkWebUrlResourceRequestImpl(
    std::shared_ptr<OHOS::NWeb::NWebUrlResourceRequest> nweb_url_resource_request)
    : nweb_url_resource_request_(nweb_url_resource_request)
{}

ArkWebString ArkWebUrlResourceRequestImpl::Url()
{
    return ArkWebStringClassToStruct(nweb_url_resource_request_->Url());
}

ArkWebString ArkWebUrlResourceRequestImpl::Method()
{
    return ArkWebStringClassToStruct(nweb_url_resource_request_->Method());
}

bool ArkWebUrlResourceRequestImpl::FromGesture()
{
    return nweb_url_resource_request_->FromGesture();
}

ArkWebStringMap ArkWebUrlResourceRequestImpl::RequestHeaders()
{
    return ArkWebStringMapClassToStruct(nweb_url_resource_request_->RequestHeaders());
}

bool ArkWebUrlResourceRequestImpl::IsAboutMainFrame()
{
    return nweb_url_resource_request_->IsAboutMainFrame();
}

bool ArkWebUrlResourceRequestImpl::IsRequestRedirect()
{
    return nweb_url_resource_request_->IsRequestRedirect();
}

} // namespace OHOS::ArkWeb
