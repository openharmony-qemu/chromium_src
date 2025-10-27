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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or wrapperied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ohos_nweb/bridge/ark_web_create_native_media_player_callback_wrapper.h"

#include "ohos_nweb/bridge/ark_web_media_info_impl.h"
#include "ohos_nweb/bridge/ark_web_native_media_player_bridge_wrapper.h"
#include "ohos_nweb/bridge/ark_web_native_media_player_handler_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebCreateNativeMediaPlayerCallbackWrapper::ArkWebCreateNativeMediaPlayerCallbackWrapper(
    ArkWebRefPtr<ArkWebCreateNativeMediaPlayerCallback> ark_web_create_native_vide_player_callback)
    : ark_web_create_native_vide_player_callback_(ark_web_create_native_vide_player_callback)
{}

std::shared_ptr<OHOS::NWeb::NWebNativeMediaPlayerBridge> ArkWebCreateNativeMediaPlayerCallbackWrapper::OnCreate(
    std::shared_ptr<OHOS::NWeb::NWebNativeMediaPlayerHandler> handler,
    std::shared_ptr<OHOS::NWeb::NWebMediaInfo> mediaInfo)
{
    ArkWebRefPtr<ArkWebMediaInfo> ark_web_media_info = nullptr;
    if (!CHECK_SHARED_PTR_IS_NULL(mediaInfo)) {
        ark_web_media_info = new ArkWebMediaInfoImpl(mediaInfo);
    }

    ArkWebRefPtr<ArkWebNativeMediaPlayerHandler> ark_web_native_media_player_handler = nullptr;
    if (!CHECK_SHARED_PTR_IS_NULL(handler)) {
        ark_web_native_media_player_handler = new ArkWebNativeMediaPlayerHandlerImpl(handler);
    }

    ArkWebRefPtr<ArkWebNativeMediaPlayerBridge> ark_web_native_media_player_bridge =
        ark_web_create_native_vide_player_callback_->OnCreate(ark_web_native_media_player_handler, ark_web_media_info);
    if (CHECK_REF_PTR_IS_NULL(ark_web_native_media_player_bridge)) {
        return nullptr;
    }

    return std::make_shared<ArkWebNativeMediaPlayerBridgeWrapper>(ark_web_native_media_player_bridge);
}

} // namespace OHOS::ArkWeb
