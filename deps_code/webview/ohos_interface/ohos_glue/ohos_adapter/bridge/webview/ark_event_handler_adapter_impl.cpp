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

#include "ohos_adapter/bridge/ark_event_handler_adapter_impl.h"

#include "ohos_adapter/bridge/ark_event_handler_fdlistener_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_once_callback_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkEventHandlerAdapterImpl::ArkEventHandlerAdapterImpl(std::shared_ptr<OHOS::NWeb::EventHandlerAdapter> ref)
    : real_(ref)
{}

bool ArkEventHandlerAdapterImpl::AddFileDescriptorListener(
    int32_t fileDescriptor, uint32_t events, const ArkWebRefPtr<ArkEventHandlerFDListenerAdapter> listener)
{
    if (CHECK_REF_PTR_IS_NULL(listener)) {
        return real_->AddFileDescriptorListener(fileDescriptor, events, nullptr);
    }

    return real_->AddFileDescriptorListener(
        fileDescriptor, events, std::make_shared<ArkEventHandlerFDListenerAdapterWrapper>(listener));
}

void ArkEventHandlerAdapterImpl::RemoveFileDescriptorListener(int32_t fileDescriptor)
{
    real_->RemoveFileDescriptorListener(fileDescriptor);
}

void ArkEventHandlerAdapterImpl::PostTask(const ArkWebRefPtr<ArkOnceCallbackAdapter> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        return real_->PostTask(nullptr);
    }
    real_->PostTask(std::make_shared<ArkOnceCallbackAdapterWrapper>(callback));
}

} // namespace OHOS::ArkWeb
