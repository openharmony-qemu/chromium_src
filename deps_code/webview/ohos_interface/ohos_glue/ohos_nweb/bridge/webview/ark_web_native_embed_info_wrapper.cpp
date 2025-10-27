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

#include "ohos_nweb/bridge/ark_web_native_embed_info_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebNativeEmbedInfoWrapper::ArkWebNativeEmbedInfoWrapper(
    ArkWebRefPtr<ArkWebNativeEmbedInfo> ark_web_native_embed_info)
    : ark_web_native_embed_info_(ark_web_native_embed_info)
{}

int32_t ArkWebNativeEmbedInfoWrapper::GetWidth()
{
    return ark_web_native_embed_info_->GetWidth();
}

int32_t ArkWebNativeEmbedInfoWrapper::GetHeight()
{
    return ark_web_native_embed_info_->GetHeight();
}

std::string ArkWebNativeEmbedInfoWrapper::GetId()
{
    ArkWebString stId = ark_web_native_embed_info_->GetId();

    std::string objId = ArkWebStringStructToClass(stId);
    ArkWebStringStructRelease(stId);
    return objId;
}

std::string ArkWebNativeEmbedInfoWrapper::GetSrc()
{
    ArkWebString stSrc = ark_web_native_embed_info_->GetSrc();

    std::string objSrc = ArkWebStringStructToClass(stSrc);
    ArkWebStringStructRelease(stSrc);
    return objSrc;
}

std::string ArkWebNativeEmbedInfoWrapper::GetUrl()
{
    ArkWebString stUrl = ark_web_native_embed_info_->GetUrl();

    std::string objUrl = ArkWebStringStructToClass(stUrl);
    ArkWebStringStructRelease(stUrl);
    return objUrl;
}

std::string ArkWebNativeEmbedInfoWrapper::GetType()
{
    ArkWebString stType = ark_web_native_embed_info_->GetType();

    std::string objType = ArkWebStringStructToClass(stType);
    ArkWebStringStructRelease(stType);
    return objType;
}

std::string ArkWebNativeEmbedInfoWrapper::GetTag()
{
    ArkWebString stType = ark_web_native_embed_info_->GetTag();

    std::string objType = ArkWebStringStructToClass(stType);
    ArkWebStringStructRelease(stType);
    return objType;
}

std::map<std::string, std::string> ArkWebNativeEmbedInfoWrapper::GetParams()
{
    ArkWebStringMap stType = ark_web_native_embed_info_->GetParams();

    std::map<std::string, std::string> objType = ArkWebStringMapStructToClass(stType);
    ArkWebStringMapStructRelease(stType);
    return objType;
}

int32_t ArkWebNativeEmbedInfoWrapper::GetX()
{
    return ark_web_native_embed_info_->GetX();
}

int32_t ArkWebNativeEmbedInfoWrapper::GetY()
{
    return ark_web_native_embed_info_->GetY();
}

} // namespace OHOS::ArkWeb
