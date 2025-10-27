/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "ohos_adapter/bridge/ark_event_handler_fdlistener_adapter_impl.h"

namespace OHOS::ArkWeb {

ArkEventHandlerFDListenerAdapterImpl::ArkEventHandlerFDListenerAdapterImpl(
    std::shared_ptr<OHOS::NWeb::EventHandlerFDListenerAdapter> ref)
    : real_(ref)
{}

void ArkEventHandlerFDListenerAdapterImpl::OnReadable(int32_t fileDescriptor)
{
    real_->OnReadable(fileDescriptor);
}

} // namespace OHOS::ArkWeb
