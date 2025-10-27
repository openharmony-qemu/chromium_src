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

#include "ohos_nweb/bridge/ark_web_native_media_player_surface_info_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebNativeMediaPlayerSurfaceInfoImpl::ArkWebNativeMediaPlayerSurfaceInfoImpl(
    std::shared_ptr<OHOS::NWeb::NWebNativeMediaPlayerSurfaceInfo> nweb_native_media_player_surface_info)
    : nweb_native_media_player_surface_info_(nweb_native_media_player_surface_info)
{}

ArkWebString ArkWebNativeMediaPlayerSurfaceInfoImpl::GetId()
{
    return ArkWebStringClassToStruct(nweb_native_media_player_surface_info_->GetId());
}

double ArkWebNativeMediaPlayerSurfaceInfoImpl::GetX()
{
    return nweb_native_media_player_surface_info_->GetX();
}

double ArkWebNativeMediaPlayerSurfaceInfoImpl::GetY()
{
    return nweb_native_media_player_surface_info_->GetY();
}

double ArkWebNativeMediaPlayerSurfaceInfoImpl::GetWidth()
{
    return nweb_native_media_player_surface_info_->GetWidth();
}

double ArkWebNativeMediaPlayerSurfaceInfoImpl::GetHeight()
{
    return nweb_native_media_player_surface_info_->GetHeight();
}

} // namespace OHOS::ArkWeb
