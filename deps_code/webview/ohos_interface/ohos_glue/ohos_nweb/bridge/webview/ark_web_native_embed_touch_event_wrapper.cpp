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

#include "ohos_nweb/bridge/ark_web_native_embed_touch_event_wrapper.h"

#include "ohos_nweb/bridge/ark_web_gesture_event_result_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebNativeEmbedTouchEventWrapper::ArkWebNativeEmbedTouchEventWrapper(
    ArkWebRefPtr<ArkWebNativeEmbedTouchEvent> ark_web_native_embed_touch_event)
    : ark_web_native_embed_touch_event_(ark_web_native_embed_touch_event)
{}

float ArkWebNativeEmbedTouchEventWrapper::GetX()
{
    return ark_web_native_embed_touch_event_->GetX();
}

float ArkWebNativeEmbedTouchEventWrapper::GetY()
{
    return ark_web_native_embed_touch_event_->GetY();
}

int32_t ArkWebNativeEmbedTouchEventWrapper::GetId()
{
    return ark_web_native_embed_touch_event_->GetId();
}

ArkWebTouchType ArkWebNativeEmbedTouchEventWrapper::GetType()
{
    return static_cast<ArkWebTouchType>(ark_web_native_embed_touch_event_->GetType());
}

float ArkWebNativeEmbedTouchEventWrapper::GetOffsetX()
{
    return ark_web_native_embed_touch_event_->GetOffsetX();
}

float ArkWebNativeEmbedTouchEventWrapper::GetOffsetY()
{
    return ark_web_native_embed_touch_event_->GetOffsetY();
}

float ArkWebNativeEmbedTouchEventWrapper::GetScreenX()
{
    return ark_web_native_embed_touch_event_->GetScreenX();
}

float ArkWebNativeEmbedTouchEventWrapper::GetScreenY()
{
    return ark_web_native_embed_touch_event_->GetScreenY();
}

std::string ArkWebNativeEmbedTouchEventWrapper::GetEmbedId()
{
    ArkWebString stEmbedId = ark_web_native_embed_touch_event_->GetEmbedId();

    std::string objEmbedId = ArkWebStringStructToClass(stEmbedId);
    ArkWebStringStructRelease(stEmbedId);
    return objEmbedId;
}

std::shared_ptr<OHOS::NWeb::NWebGestureEventResult> ArkWebNativeEmbedTouchEventWrapper::GetResult()
{
    ArkWebRefPtr<ArkWebGestureEventResult> result = ark_web_native_embed_touch_event_->GetResult();
    if (CHECK_REF_PTR_IS_NULL(result)) {
        return nullptr;
    }

    return std::make_shared<ArkWebGestureEventResultWrapper>(result);
}

} // namespace OHOS::ArkWeb
