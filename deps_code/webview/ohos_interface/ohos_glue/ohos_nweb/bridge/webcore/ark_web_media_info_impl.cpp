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

#include "ohos_nweb/bridge/ark_web_media_info_impl.h"

#include "ohos_nweb/bridge/ark_web_native_media_player_surface_info_impl.h"
#include "ohos_nweb/cpptoc/ark_web_media_source_info_vector_cpptoc.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebMediaInfoImpl::ArkWebMediaInfoImpl(std::shared_ptr<OHOS::NWeb::NWebMediaInfo> nweb_media_info)
    : nweb_media_info_(nweb_media_info)
{}

int ArkWebMediaInfoImpl::GetPreload()
{
    return static_cast<int>(nweb_media_info_->GetPreload());
}

bool ArkWebMediaInfoImpl::GetIsMuted()
{
    return nweb_media_info_->GetIsMuted();
}

ArkWebString ArkWebMediaInfoImpl::GetEmbedId()
{
    return ArkWebStringClassToStruct(nweb_media_info_->GetEmbedId());
}

ArkWebString ArkWebMediaInfoImpl::GetPosterUrl()
{
    return ArkWebStringClassToStruct(nweb_media_info_->GetPosterUrl());
}

int ArkWebMediaInfoImpl::GetMediaType()
{
    return static_cast<int>(nweb_media_info_->GetMediaType());
}

bool ArkWebMediaInfoImpl::GetIsControlsShown()
{
    return nweb_media_info_->GetIsControlsShown();
}

ArkWebStringVector ArkWebMediaInfoImpl::GetControls()
{
    return ArkWebStringVectorClassToStruct(nweb_media_info_->GetControls());
}

ArkWebStringMap ArkWebMediaInfoImpl::GetHeaders()
{
    return ArkWebStringMapClassToStruct(nweb_media_info_->GetHeaders());
}

ArkWebStringMap ArkWebMediaInfoImpl::GetAttributes()
{
    return ArkWebStringMapClassToStruct(nweb_media_info_->GetAttributes());
}

ArkWebMediaSourceInfoVector ArkWebMediaInfoImpl::GetSourceInfos()
{
    return ArkWebMediaSourceInfoVectorClassToStruct(nweb_media_info_->GetSourceInfos());
}

ArkWebRefPtr<ArkWebNativeMediaPlayerSurfaceInfo> ArkWebMediaInfoImpl::GetSurfaceInfo()
{
    std::shared_ptr<OHOS::NWeb::NWebNativeMediaPlayerSurfaceInfo> nweb_native_media_player_surface_info =
        nweb_media_info_->GetSurfaceInfo();
    if (CHECK_SHARED_PTR_IS_NULL(nweb_native_media_player_surface_info)) {
        return nullptr;
    }

    return new ArkWebNativeMediaPlayerSurfaceInfoImpl(nweb_native_media_player_surface_info);
}

} // namespace OHOS::ArkWeb
