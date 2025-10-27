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

#include "ohos_nweb/bridge/ark_web_native_media_player_surface_info_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebNativeMediaPlayerSurfaceInfoWrapper::ArkWebNativeMediaPlayerSurfaceInfoWrapper(
    ArkWebRefPtr<ArkWebNativeMediaPlayerSurfaceInfo> ark_web_native_media_player_surface_info)
    : ark_web_native_media_player_surface_info_(ark_web_native_media_player_surface_info)
{}

std::string ArkWebNativeMediaPlayerSurfaceInfoWrapper::GetId()
{
    ArkWebString stId = ark_web_native_media_player_surface_info_->GetId();

    std::string objId = ArkWebStringStructToClass(stId);
    ArkWebStringStructRelease(stId);
    return objId;
}

double ArkWebNativeMediaPlayerSurfaceInfoWrapper::GetX()
{
    return ark_web_native_media_player_surface_info_->GetX();
}

double ArkWebNativeMediaPlayerSurfaceInfoWrapper::GetY()
{
    return ark_web_native_media_player_surface_info_->GetY();
}

double ArkWebNativeMediaPlayerSurfaceInfoWrapper::GetWidth()
{
    return ark_web_native_media_player_surface_info_->GetWidth();
}

double ArkWebNativeMediaPlayerSurfaceInfoWrapper::GetHeight()
{
    return ark_web_native_media_player_surface_info_->GetHeight();
}

} // namespace OHOS::ArkWeb
