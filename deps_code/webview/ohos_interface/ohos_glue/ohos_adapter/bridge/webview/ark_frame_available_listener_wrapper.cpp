/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "ohos_adapter/bridge/ark_frame_available_listener_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkFrameAvailableListenerWrapper::ArkFrameAvailableListenerWrapper(ArkWebRefPtr<ArkFrameAvailableListener> ref)
    : ctocpp_(ref)
{}

void* ArkFrameAvailableListenerWrapper::GetContext()
{
    return ctocpp_->GetContext();
}

NWeb::OnFrameAvailableCb ArkFrameAvailableListenerWrapper::GetOnFrameAvailableCb()
{
    return (NWeb::OnFrameAvailableCb)ctocpp_->GetOnFrameAvailableCb();
}

} // namespace OHOS::ArkWeb
