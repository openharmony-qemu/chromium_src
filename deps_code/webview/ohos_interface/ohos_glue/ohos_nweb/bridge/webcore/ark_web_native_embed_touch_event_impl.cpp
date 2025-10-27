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

#include "ohos_nweb/bridge/ark_web_native_embed_touch_event_impl.h"

#include "ohos_nweb/bridge/ark_web_gesture_event_result_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebNativeEmbedTouchEventImpl::ArkWebNativeEmbedTouchEventImpl(
    std::shared_ptr<OHOS::NWeb::NWebNativeEmbedTouchEvent> nweb_native_embed_touch_event)
    : nweb_native_embed_touch_event_(nweb_native_embed_touch_event)
{}

float ArkWebNativeEmbedTouchEventImpl::GetX()
{
    return nweb_native_embed_touch_event_->GetX();
}

float ArkWebNativeEmbedTouchEventImpl::GetY()
{
    return nweb_native_embed_touch_event_->GetY();
}

int32_t ArkWebNativeEmbedTouchEventImpl::GetId()
{
    return nweb_native_embed_touch_event_->GetId();
}

size_t ArkWebNativeEmbedTouchEventImpl::GetType()
{
    return static_cast<size_t>(nweb_native_embed_touch_event_->GetType());
}

float ArkWebNativeEmbedTouchEventImpl::GetOffsetX()
{
    return nweb_native_embed_touch_event_->GetOffsetX();
}

float ArkWebNativeEmbedTouchEventImpl::GetOffsetY()
{
    return nweb_native_embed_touch_event_->GetOffsetY();
}

float ArkWebNativeEmbedTouchEventImpl::GetScreenX()
{
    return nweb_native_embed_touch_event_->GetScreenX();
}

float ArkWebNativeEmbedTouchEventImpl::GetScreenY()
{
    return nweb_native_embed_touch_event_->GetScreenY();
}

ArkWebString ArkWebNativeEmbedTouchEventImpl::GetEmbedId()
{
    return ArkWebStringClassToStruct(nweb_native_embed_touch_event_->GetEmbedId());
}

ArkWebRefPtr<ArkWebGestureEventResult> ArkWebNativeEmbedTouchEventImpl::GetResult()
{
    std::shared_ptr<OHOS::NWeb::NWebGestureEventResult> result = nweb_native_embed_touch_event_->GetResult();
    if (CHECK_SHARED_PTR_IS_NULL(result)) {
        return nullptr;
    }
    return new ArkWebGestureEventResultImpl(result);
}

} // namespace OHOS::ArkWeb
