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

#include <fuzzer/FuzzedDataProvider.h>

#include "arkweb/ohos_adapter_ndk/event_handler_adapter/event_handler_adapter_impl.h"

using namespace OHOS::NWeb;

namespace {
constexpr int MAX_SET_NUMBER = 1000;

class EventHandlerFDListenerAdapterTest : public EventHandlerFDListenerAdapter {
public:
    void OnReadable(int32_t fileDescriptor) override {}
};

void EventHandlerAdapterFuzzTest(const uint8_t* data, size_t size)
{
    FuzzedDataProvider dataProvider(data, size);
    int32_t fileDescriptor = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_SET_NUMBER);
    uint32_t events = dataProvider.ConsumeIntegralInRange<uint32_t>(0, MAX_SET_NUMBER);

    std::shared_ptr<EventHandlerFDListenerAdapter> listener = std::make_shared<EventHandlerFDListenerAdapterTest>();
    std::shared_ptr<EventHandlerAdapterImpl> eventHandlerAdapter = std::make_shared<EventHandlerAdapterImpl>();

    eventHandlerAdapter->AddFileDescriptorListener(fileDescriptor, events, listener);
    eventHandlerAdapter->RemoveFileDescriptorListener(fileDescriptor);
}
} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return 0;
    }
    EventHandlerAdapterFuzzTest(data, size);
    return 0;
}