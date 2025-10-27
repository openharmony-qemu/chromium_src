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

#ifndef ARK_WEB_NATIVE_EMBED_TOUCH_EVENT_WRAPPER_H_
#define ARK_WEB_NATIVE_EMBED_TOUCH_EVENT_WRAPPER_H_
#pragma once

#include "include/nweb_handler.h"
#include "ohos_nweb/include/ark_web_native_embed_touch_event.h"

namespace OHOS::ArkWeb {

using ArkWebTouchType = OHOS::NWeb::TouchType;

class ArkWebNativeEmbedTouchEventWrapper : public OHOS::NWeb::NWebNativeEmbedTouchEvent {
public:
    ArkWebNativeEmbedTouchEventWrapper(ArkWebRefPtr<ArkWebNativeEmbedTouchEvent> ark_web_native_embed_touch_event);
    ~ArkWebNativeEmbedTouchEventWrapper() = default;

    float GetX() override;

    float GetY() override;

    int32_t GetId() override;

    ArkWebTouchType GetType() override;

    float GetOffsetX() override;

    float GetOffsetY() override;

    float GetScreenX() override;

    float GetScreenY() override;

    std::string GetEmbedId() override;

    std::shared_ptr<OHOS::NWeb::NWebGestureEventResult> GetResult() override;

private:
    ArkWebRefPtr<ArkWebNativeEmbedTouchEvent> ark_web_native_embed_touch_event_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_NATIVE_EMBED_TOUCH_EVENT_WRAPPER_H_
