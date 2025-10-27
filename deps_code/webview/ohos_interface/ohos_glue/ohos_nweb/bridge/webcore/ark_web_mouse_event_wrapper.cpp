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

#include "ohos_nweb/bridge/ark_web_mouse_event_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebMouseEventWrapper::ArkWebMouseEventWrapper(ArkWebRefPtr<ArkWebMouseEvent> mouseEvent)
    : mouseEvent_(mouseEvent)
{}

int32_t ArkWebMouseEventWrapper::GetX()
{
    return mouseEvent_->GetX();
}

int32_t ArkWebMouseEventWrapper::GetY()
{
    return mouseEvent_->GetY();
}

int32_t ArkWebMouseEventWrapper::GetButton()
{
    return mouseEvent_->GetButton();
}

int32_t ArkWebMouseEventWrapper::GetAction()
{
    return mouseEvent_->GetAction();
}

int32_t ArkWebMouseEventWrapper::GetClickNum()
{
    return mouseEvent_->GetClickNum();
}

std::vector<int32_t> ArkWebMouseEventWrapper::GetPressKeyCodes()
{
    ArkWebInt32Vector pressKeyCodes = mouseEvent_->GetPressKeyCodes();

    std::vector<int32_t> result = ArkWebBasicVectorStructToClass<int32_t, ArkWebInt32Vector>(pressKeyCodes);

    ArkWebBasicVectorStructRelease(pressKeyCodes);

    return result;
}

int32_t ArkWebMouseEventWrapper::GetRawX()
{
    return mouseEvent_->GetRawX();
}

int32_t ArkWebMouseEventWrapper::GetRawY()
{
    return mouseEvent_->GetRawY();
}

} // namespace OHOS::ArkWeb
