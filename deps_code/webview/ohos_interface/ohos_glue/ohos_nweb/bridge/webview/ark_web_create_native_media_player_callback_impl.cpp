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

#include "ohos_nweb/bridge/ark_web_create_native_media_player_callback_impl.h"

#include "ohos_nweb/bridge/ark_web_media_info_wrapper.h"
#include "ohos_nweb/bridge/ark_web_native_media_player_bridge_impl.h"
#include "ohos_nweb/bridge/ark_web_native_media_player_handler_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebCreateNativeMediaPlayerCallbackImpl::ArkWebCreateNativeMediaPlayerCallbackImpl(
    std::shared_ptr<OHOS::NWeb::NWebCreateNativeMediaPlayerCallback> nweb_create_native_vide_player_callback)
    : nweb_create_native_vide_player_callback_(nweb_create_native_vide_player_callback)
{}

ArkWebRefPtr<ArkWebNativeMediaPlayerBridge> ArkWebCreateNativeMediaPlayerCallbackImpl::OnCreate(
    ArkWebRefPtr<ArkWebNativeMediaPlayerHandler> handler, ArkWebRefPtr<ArkWebMediaInfo> mediaInfo)
{
    std::shared_ptr<OHOS::NWeb::NWebMediaInfo> nweb_media_info = nullptr;
    if (!CHECK_REF_PTR_IS_NULL(mediaInfo)) {
        nweb_media_info = std::make_shared<ArkWebMediaInfoWrapper>(mediaInfo);
    }

    std::shared_ptr<OHOS::NWeb::NWebNativeMediaPlayerHandler> nweb_native_media_player_handler = nullptr;
    if (!CHECK_REF_PTR_IS_NULL(handler)) {
        nweb_native_media_player_handler = std::make_shared<ArkWebNativeMediaPlayerHandlerWrapper>(handler);
    }

    std::shared_ptr<OHOS::NWeb::NWebNativeMediaPlayerBridge> nweb_native_media_player_bridge =
        nweb_create_native_vide_player_callback_->OnCreate(nweb_native_media_player_handler, nweb_media_info);
    if (CHECK_SHARED_PTR_IS_NULL(nweb_native_media_player_bridge)) {
        return nullptr;
    }

    return new ArkWebNativeMediaPlayerBridgeImpl(nweb_native_media_player_bridge);
}

} // namespace OHOS::ArkWeb
