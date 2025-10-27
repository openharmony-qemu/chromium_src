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

#include "ohos_nweb/bridge/ark_web_url_resource_request_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebUrlResourceRequestWrapper::ArkWebUrlResourceRequestWrapper(
    ArkWebRefPtr<ArkWebUrlResourceRequest> ark_web_url_resource_request)
    : ark_web_url_resource_request_(ark_web_url_resource_request)
{}

std::string ArkWebUrlResourceRequestWrapper::Url()
{
    ArkWebString stUrl = ark_web_url_resource_request_->Url();

    std::string objUrl = ArkWebStringStructToClass(stUrl);
    ArkWebStringStructRelease(stUrl);
    return objUrl;
}

std::string ArkWebUrlResourceRequestWrapper::Method()
{
    ArkWebString stMethod = ark_web_url_resource_request_->Method();

    std::string objMethod = ArkWebStringStructToClass(stMethod);
    ArkWebStringStructRelease(stMethod);
    return objMethod;
}

bool ArkWebUrlResourceRequestWrapper::FromGesture()
{
    return ark_web_url_resource_request_->FromGesture();
}

std::map<std::string, std::string> ArkWebUrlResourceRequestWrapper::RequestHeaders()
{
    ArkWebStringMap stHeaders = ark_web_url_resource_request_->RequestHeaders();

    std::map<std::string, std::string> objHeaders = ArkWebStringMapStructToClass(stHeaders);
    ArkWebStringMapStructRelease(stHeaders);
    return objHeaders;
}

bool ArkWebUrlResourceRequestWrapper::IsAboutMainFrame()
{
    return ark_web_url_resource_request_->IsAboutMainFrame();
}

bool ArkWebUrlResourceRequestWrapper::IsRequestRedirect()
{
    return ark_web_url_resource_request_->IsRequestRedirect();
}

} // namespace OHOS::ArkWeb
