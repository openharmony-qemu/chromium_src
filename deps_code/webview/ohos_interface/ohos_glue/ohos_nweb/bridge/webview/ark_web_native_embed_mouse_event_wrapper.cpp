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

#include "ohos_nweb/bridge/ark_web_native_embed_mouse_event_wrapper.h"

#include "ohos_nweb/bridge/ark_web_mouse_event_result_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebNativeEmbedMouseEventWrapper::ArkWebNativeEmbedMouseEventWrapper(
    ArkWebRefPtr<ArkWebNativeEmbedMouseEvent> ark_web_native_embed_mouse_event)
    : ark_web_native_embed_mouse_event_(ark_web_native_embed_mouse_event)
{}

float ArkWebNativeEmbedMouseEventWrapper::GetX()
{
    return ark_web_native_embed_mouse_event_->GetX();
}

float ArkWebNativeEmbedMouseEventWrapper::GetY()
{
    return ark_web_native_embed_mouse_event_->GetY();
}

bool ArkWebNativeEmbedMouseEventWrapper::IsHitNativeArea()
{
    return ark_web_native_embed_mouse_event_->IsHitNativeArea();
}

ArkWebMouseType ArkWebNativeEmbedMouseEventWrapper::GetType()
{
    return static_cast<ArkWebMouseType>(ark_web_native_embed_mouse_event_->GetType());
}

ArkWebMouseButton ArkWebNativeEmbedMouseEventWrapper::GetButton()
{
    return static_cast<ArkWebMouseButton>(ark_web_native_embed_mouse_event_->GetButton());
}

float ArkWebNativeEmbedMouseEventWrapper::GetOffsetX()
{
    return ark_web_native_embed_mouse_event_->GetOffsetX();
}

float ArkWebNativeEmbedMouseEventWrapper::GetOffsetY()
{
    return ark_web_native_embed_mouse_event_->GetOffsetY();
}

float ArkWebNativeEmbedMouseEventWrapper::GetScreenX()
{
    return ark_web_native_embed_mouse_event_->GetScreenX();
}

float ArkWebNativeEmbedMouseEventWrapper::GetScreenY()
{
    return ark_web_native_embed_mouse_event_->GetScreenY();
}

std::string ArkWebNativeEmbedMouseEventWrapper::GetEmbedId()
{
    ArkWebString stEmbedId = ark_web_native_embed_mouse_event_->GetEmbedId();

    std::string objEmbedId = ArkWebStringStructToClass(stEmbedId);
    ArkWebStringStructRelease(stEmbedId);
    return objEmbedId;
}

std::shared_ptr<OHOS::NWeb::NWebMouseEventResult> ArkWebNativeEmbedMouseEventWrapper::GetResult()
{
    ArkWebRefPtr<ArkWebMouseEventResult> result = ark_web_native_embed_mouse_event_->GetResult();
    if (CHECK_REF_PTR_IS_NULL(result)) {
        return nullptr;
    }

    return std::make_shared<ArkWebMouseEventResultWrapper>(result);
}

} // namespace OHOS::ArkWeb
