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

#include "ohos_nweb/bridge/ark_web_message_value_callback_wrapper.h"
#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebMessageValueCallbackWrapper::ArkWebMessageValueCallbackWrapper(
    ArkWebRefPtr<ArkWebMessageValueCallback> ark_web_message_value_callback)
    : ark_web_message_value_callback_(ark_web_message_value_callback) {
}

void ArkWebMessageValueCallbackWrapper::OnReceiveValue(
    std::shared_ptr<OHOS::NWeb::NWebMessage> value) {
  ArkWebMessage ark_web_message;
  ark_web_message.nweb_message = value;
  ark_web_message_value_callback_->OnReceiveValue(ark_web_message);
}

} // namespace OHOS::ArkWeb
