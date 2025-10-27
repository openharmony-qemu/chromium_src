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

#define private public
#include "arkweb/ohos_adapter_ndk/date_time_format_adapter/date_time_format_adapter_impl.h"
#undef private

#include <cstring>
#include <fuzzer/FuzzedDataProvider.h>

using namespace OHOS::NWeb;
namespace OHOS {

class TimezoneEventCallbackAdapterFuzz : public TimezoneEventCallbackAdapter {
public:
TimezoneEventCallbackAdapterFuzz() = default;
    void TimezoneChanged(std::shared_ptr<WebTimezoneInfo> info) {}
};

bool DateTimeFormatAdapterImplFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    auto adapter = std::make_shared<DateTimeFormatAdapterImpl>();
    adapter->RegTimezoneEvent(nullptr);
    std::shared_ptr<TimezoneEventCallbackAdapter> cb = std::make_shared<TimezoneEventCallbackAdapterFuzz>();
    adapter->RegTimezoneEvent(std::move(cb));
    adapter->StartListen();
    adapter->StopListen();
    adapter->GetTimezone();

    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DateTimeFormatAdapterImplFuzzTest(data, size);
    return 0;
}