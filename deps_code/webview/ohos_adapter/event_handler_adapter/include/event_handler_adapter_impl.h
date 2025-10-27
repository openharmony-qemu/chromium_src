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

#ifndef EVENT_HANDLER_ADAPTER_IMPL_H
#define EVENT_HANDLER_ADAPTER_IMPL_H

#include "event_handler.h"
#include "event_handler_adapter.h"
#include "file_descriptor_listener.h"

namespace OHOS::NWeb {
class EventHandlerFDListenerAdapterImpl : public AppExecFwk::FileDescriptorListener {
public:
    explicit EventHandlerFDListenerAdapterImpl(const std::shared_ptr<EventHandlerFDListenerAdapter>& listener);

    ~EventHandlerFDListenerAdapterImpl() override = default;

    void OnReadable(int32_t fileDescriptor) override;

private:
    std::shared_ptr<EventHandlerFDListenerAdapter> listener_ = nullptr;
};

class OnceCallbackAdapterImpl {
public:
    explicit OnceCallbackAdapterImpl(const std::shared_ptr<OnceCallbackAdapter>& callback);

    ~OnceCallbackAdapterImpl() = default;

    void OnRunnable();

private:
    std::shared_ptr<OnceCallbackAdapter> callback_ = nullptr;
};

class EventHandlerAdapterImpl : public EventHandlerAdapter {
public:
    EventHandlerAdapterImpl();

    ~EventHandlerAdapterImpl() override = default;

    bool AddFileDescriptorListener(int32_t fileDescriptor, uint32_t events,
        const std::shared_ptr<EventHandlerFDListenerAdapter> listener) override;

    void RemoveFileDescriptorListener(int32_t fileDescriptor) override;

    void PostTask(const std::shared_ptr<OnceCallbackAdapter> callback) override;

private:
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler_ = nullptr;
};
} // namespace OHOS::NWeb

#endif // EVENT_HANDLER_ADAPTER_IMPL_H
