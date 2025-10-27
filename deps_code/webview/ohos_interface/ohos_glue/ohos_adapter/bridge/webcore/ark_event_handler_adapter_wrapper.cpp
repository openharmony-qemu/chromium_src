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

#include "ohos_adapter/bridge/ark_event_handler_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_event_handler_fdlistener_adapter_impl.h"

#include "ohos_adapter/bridge/ark_once_callback_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkEventHandlerAdapterWrapper::ArkEventHandlerAdapterWrapper(ArkWebRefPtr<ArkEventHandlerAdapter> ref) : ctocpp_(ref) {}

bool ArkEventHandlerAdapterWrapper::AddFileDescriptorListener(
    int32_t fileDescriptor, uint32_t events, const std::shared_ptr<OHOS::NWeb::EventHandlerFDListenerAdapter> listener)
{
    if (CHECK_SHARED_PTR_IS_NULL(listener)) {
        return ctocpp_->AddFileDescriptorListener(fileDescriptor, events, nullptr);
    }

    return ctocpp_->AddFileDescriptorListener(
        fileDescriptor, events, new ArkEventHandlerFDListenerAdapterImpl(listener));
}

void ArkEventHandlerAdapterWrapper::RemoveFileDescriptorListener(int32_t fileDescriptor)
{
    ctocpp_->RemoveFileDescriptorListener(fileDescriptor);
}

void ArkEventHandlerAdapterWrapper::PostTask(const std::shared_ptr<OHOS::NWeb::OnceCallbackAdapter> callback)
{
    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        return ctocpp_->PostTask(nullptr);
    }

    return ctocpp_->PostTask(new ArkOnceCallbackAdapterImpl(callback));
}

} // namespace OHOS::ArkWeb
