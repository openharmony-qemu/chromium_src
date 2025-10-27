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

#ifndef ARK_WEB_CREATE_NATIVE_MEDIA_PLAYER_CALLBACK_IMPL_H_
#define ARK_WEB_CREATE_NATIVE_MEDIA_PLAYER_CALLBACK_IMPL_H_
#pragma once

#include "include/nweb_native_media_player.h"
#include "ohos_nweb/include/ark_web_create_native_media_player_callback.h"

namespace OHOS::ArkWeb {

class ArkWebCreateNativeMediaPlayerCallbackImpl : public ArkWebCreateNativeMediaPlayerCallback {
    IMPLEMENT_REFCOUNTING(ArkWebCreateNativeMediaPlayerCallbackImpl);

public:
    ArkWebCreateNativeMediaPlayerCallbackImpl(
        std::shared_ptr<OHOS::NWeb::NWebCreateNativeMediaPlayerCallback> nweb_create_native_vide_player_callback);
    ~ArkWebCreateNativeMediaPlayerCallbackImpl() = default;

    ArkWebRefPtr<ArkWebNativeMediaPlayerBridge> OnCreate(
        ArkWebRefPtr<ArkWebNativeMediaPlayerHandler> handler, ArkWebRefPtr<ArkWebMediaInfo> mediaInfo) override;

private:
    std::shared_ptr<OHOS::NWeb::NWebCreateNativeMediaPlayerCallback> nweb_create_native_vide_player_callback_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_CREATE_NATIVE_MEDIA_PLAYER_CALLBACK_IMPL_H_
