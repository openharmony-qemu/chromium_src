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

#include "event_handler_adapter_impl.h"

#include "nweb_log.h"
#include "securec.h"

namespace OHOS::NWeb {

EventHandlerFDListenerAdapterImpl::EventHandlerFDListenerAdapterImpl(
    const std::shared_ptr<EventHandlerFDListenerAdapter>& listener)
    : listener_(listener)
{}

void EventHandlerFDListenerAdapterImpl::OnReadable(int32_t fileDescriptor)
{
    if (listener_) {
        listener_->OnReadable(fileDescriptor);
    }
}

OnceCallbackAdapterImpl::OnceCallbackAdapterImpl(
    const std::shared_ptr<OnceCallbackAdapter>& callback)
    : callback_(callback)
{}

void OnceCallbackAdapterImpl::OnRunnable()
{
    if (callback_) {
        callback_->OnRunnable();
    }
}

EventHandlerAdapterImpl::EventHandlerAdapterImpl()
{
    eventHandler_ = AppExecFwk::EventHandler::Current();
}

bool EventHandlerAdapterImpl::AddFileDescriptorListener(
    int32_t fileDescriptor, uint32_t events, const std::shared_ptr<EventHandlerFDListenerAdapter> listener)
{
    if (!listener || !eventHandler_) {
        WVLOG_E("the listener or eventHandler_ is nullptr");
        return false;
    }
    auto fileDescriptorListener = std::make_shared<EventHandlerFDListenerAdapterImpl>(listener);
    return eventHandler_->AddFileDescriptorListener(fileDescriptor, events, fileDescriptorListener,
        "webViewTask") == EOK;
}

void EventHandlerAdapterImpl::RemoveFileDescriptorListener(int32_t fileDescriptor)
{
    if (eventHandler_) {
        eventHandler_->RemoveFileDescriptorListener(fileDescriptor);
    }
}

void EventHandlerAdapterImpl::PostTask(const std::shared_ptr<OnceCallbackAdapter> callback)
{
    if (!callback || !eventHandler_) {
        return;
    }
    auto onceCallback = std::make_shared<OnceCallbackAdapterImpl>(callback);
    eventHandler_->PostTask(
            [onceCallback]() {
              onceCallback->OnRunnable();
            }, AppExecFwk::EventQueue::Priority::LOW);
}
} // namespace OHOS::NWeb
