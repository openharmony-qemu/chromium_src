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

#include "ohos_adapter/bridge/ark_mmi_input_listener_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkMMIInputListenerAdapterWrapper::ArkMMIInputListenerAdapterWrapper(ArkWebRefPtr<ArkMMIInputListenerAdapter> ref)
    : ctocpp_(ref)
{}

void ArkMMIInputListenerAdapterWrapper::OnInputEvent(int32_t keyCode, int32_t keyAction)
{
    ctocpp_->OnInputEvent(keyCode, keyAction);
}

} // namespace OHOS::ArkWeb
