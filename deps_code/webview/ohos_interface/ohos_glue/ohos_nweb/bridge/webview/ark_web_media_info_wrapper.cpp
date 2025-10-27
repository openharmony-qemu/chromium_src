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

#include "ohos_nweb/bridge/ark_web_media_info_wrapper.h"

#include "ohos_nweb/bridge/ark_web_native_media_player_surface_info_wrapper.h"
#include "ohos_nweb/ctocpp/ark_web_media_source_info_vector_ctocpp.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebMediaInfoWrapper::ArkWebMediaInfoWrapper(ArkWebRefPtr<ArkWebMediaInfo> ark_web_media_info)
    : ark_web_media_info_(ark_web_media_info)
{}

ArkWebPreload ArkWebMediaInfoWrapper::GetPreload()
{
    return static_cast<ArkWebPreload>(ark_web_media_info_->GetPreload());
}

bool ArkWebMediaInfoWrapper::GetIsMuted()
{
    return ark_web_media_info_->GetIsMuted();
}

std::string ArkWebMediaInfoWrapper::GetEmbedId()
{
    ArkWebString stEmbedId = ark_web_media_info_->GetEmbedId();

    std::string objEmbedId = ArkWebStringStructToClass(stEmbedId);
    ArkWebStringStructRelease(stEmbedId);
    return objEmbedId;
}

std::string ArkWebMediaInfoWrapper::GetPosterUrl()
{
    ArkWebString stUrl = ark_web_media_info_->GetPosterUrl();

    std::string objUrl = ArkWebStringStructToClass(stUrl);
    ArkWebStringStructRelease(stUrl);
    return objUrl;
}

ArkWebMediaType ArkWebMediaInfoWrapper::GetMediaType()
{
    return static_cast<ArkWebMediaType>(ark_web_media_info_->GetMediaType());
}

bool ArkWebMediaInfoWrapper::GetIsControlsShown()
{
    return ark_web_media_info_->GetIsControlsShown();
}

std::vector<std::string> ArkWebMediaInfoWrapper::GetControls()
{
    ArkWebStringVector stControls = ark_web_media_info_->GetControls();

    std::vector<std::string> objControls = ArkWebStringVectorStructToClass(stControls);
    ArkWebStringVectorStructRelease(stControls);
    return objControls;
}

std::map<std::string, std::string> ArkWebMediaInfoWrapper::GetHeaders()
{
    ArkWebStringMap stHeaders = ark_web_media_info_->GetHeaders();

    std::map<std::string, std::string> objHeaders = ArkWebStringMapStructToClass(stHeaders);
    ArkWebStringMapStructRelease(stHeaders);
    return objHeaders;
}

std::map<std::string, std::string> ArkWebMediaInfoWrapper::GetAttributes()
{
    ArkWebStringMap stAttributes = ark_web_media_info_->GetAttributes();

    std::map<std::string, std::string> objAttributes = ArkWebStringMapStructToClass(stAttributes);
    ArkWebStringMapStructRelease(stAttributes);
    return objAttributes;
}

std::vector<std::shared_ptr<OHOS::NWeb::NWebMediaSourceInfo>> ArkWebMediaInfoWrapper::GetSourceInfos()
{
    ArkWebMediaSourceInfoVector stMediaSourceInfoVector = ark_web_media_info_->GetSourceInfos();

    std::vector<std::shared_ptr<OHOS::NWeb::NWebMediaSourceInfo>> objMediaSourceInfoVector =
        ArkWebMediaSourceInfoVectorStructToClass(stMediaSourceInfoVector);
    ArkWebMediaSourceInfoVectorStructRelease(stMediaSourceInfoVector);
    return objMediaSourceInfoVector;
}

std::shared_ptr<OHOS::NWeb::NWebNativeMediaPlayerSurfaceInfo> ArkWebMediaInfoWrapper::GetSurfaceInfo()
{
    ArkWebRefPtr<ArkWebNativeMediaPlayerSurfaceInfo> ark_web_native_media_player_surface_info =
        ark_web_media_info_->GetSurfaceInfo();
    if (CHECK_REF_PTR_IS_NULL(ark_web_native_media_player_surface_info)) {
        return nullptr;
    }

    return std::make_shared<ArkWebNativeMediaPlayerSurfaceInfoWrapper>(ark_web_native_media_player_surface_info);
}

} // namespace OHOS::ArkWeb
