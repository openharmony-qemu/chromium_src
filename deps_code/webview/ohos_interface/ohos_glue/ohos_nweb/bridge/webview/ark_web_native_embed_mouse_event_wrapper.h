/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef ARK_WEB_NATIVE_EMBED_MOUSE_EVENT_WRAPPER_H_
#define ARK_WEB_NATIVE_EMBED_MOUSE_EVENT_WRAPPER_H_
#pragma once

#include "include/nweb_handler.h"
#include "ohos_nweb/include/ark_web_native_embed_mouse_event.h"

namespace OHOS::ArkWeb {

using ArkWebMouseType = OHOS::NWeb::MouseType;
using ArkWebMouseButton = OHOS::NWeb::MouseButton;

class ArkWebNativeEmbedMouseEventWrapper : public OHOS::NWeb::NWebNativeEmbedMouseEvent {
public:
    ArkWebNativeEmbedMouseEventWrapper(ArkWebRefPtr<ArkWebNativeEmbedMouseEvent> ark_web_native_embed_mouse_event);
    ~ArkWebNativeEmbedMouseEventWrapper() = default;

    float GetX() override;

    float GetY() override;

    bool IsHitNativeArea() override;

    ArkWebMouseType GetType() override;

    ArkWebMouseButton GetButton() override;

    float GetOffsetX() override;

    float GetOffsetY() override;

    float GetScreenX() override;

    float GetScreenY() override;

    std::string GetEmbedId() override;

    std::shared_ptr<OHOS::NWeb::NWebMouseEventResult> GetResult() override;

private:
    ArkWebRefPtr<ArkWebNativeEmbedMouseEvent> ark_web_native_embed_mouse_event_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_NATIVE_EMBED_MOUSE_EVENT_WRAPPER_H_
