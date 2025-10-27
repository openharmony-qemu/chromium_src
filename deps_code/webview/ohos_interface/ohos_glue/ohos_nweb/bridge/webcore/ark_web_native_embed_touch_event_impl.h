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

#ifndef ARK_WEB_NATIVE_EMBED_TOUCH_EVENT_IMPL_H_
#define ARK_WEB_NATIVE_EMBED_TOUCH_EVENT_IMPL_H_
#pragma once

#include "include/nweb_handler.h"
#include "ohos_nweb/include/ark_web_native_embed_touch_event.h"

namespace OHOS::ArkWeb {

class ArkWebNativeEmbedTouchEventImpl : public virtual ArkWebNativeEmbedTouchEvent {
    IMPLEMENT_REFCOUNTING(ArkWebNativeEmbedTouchEventImpl);

public:
    ArkWebNativeEmbedTouchEventImpl(
        std::shared_ptr<OHOS::NWeb::NWebNativeEmbedTouchEvent> nweb_native_embed_touch_event);
    ~ArkWebNativeEmbedTouchEventImpl() = default;

    float GetX() override;

    float GetY() override;

    int32_t GetId() override;

    size_t GetType() override;

    float GetOffsetX() override;

    float GetOffsetY() override;

    float GetScreenX() override;

    float GetScreenY() override;

    ArkWebString GetEmbedId() override;

    ArkWebRefPtr<ArkWebGestureEventResult> GetResult() override;

private:
    std::shared_ptr<OHOS::NWeb::NWebNativeEmbedTouchEvent> nweb_native_embed_touch_event_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_NATIVE_EMBED_TOUCH_EVENT_IMPL_H_
