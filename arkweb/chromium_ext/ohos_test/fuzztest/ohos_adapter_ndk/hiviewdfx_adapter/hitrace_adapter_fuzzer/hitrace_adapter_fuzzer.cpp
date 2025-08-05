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
#include "hitrace_adapter_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "hitrace_adapter_impl.h"
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <vector>
#include <string>

using namespace OHOS::NWeb;

namespace {
  constexpr uint8_t MAX_STRING_LENGTH = UINT8_MAX;
  constexpr int32_t MAX_INT_SIZE = INT32_MAX;
  constexpr uint32_t MAX_LOG_LEVEL = 5;
  constexpr int64_t MAX_INT64_SIZE = INT64_MAX;
}

void StartTrace(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    std::string value = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    int limit = dataProvider.ConsumeIntegralInRange<int>(-1, MAX_INT_SIZE);
    float limit_float = static_cast<float>(limit);
    HiTraceAdapterImpl::GetInstance().StartTrace(value, limit_float);
}

void FinishTrace() {
    HiTraceAdapterImpl::GetInstance().FinishTrace();
}

void StartAsyncTrace(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    std::string value = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    int32_t taskId = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT_SIZE);
    int limit = dataProvider.ConsumeIntegralInRange<int>(-1, MAX_INT_SIZE);
    float limit_float = static_cast<float>(limit);
    HiTraceAdapterImpl::GetInstance().StartAsyncTrace(value, taskId, limit_float);
}

void FinishAsyncTrace(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    std::string value = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    int32_t taskId = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT_SIZE);
    HiTraceAdapterImpl::GetInstance().FinishAsyncTrace(value, taskId);
}

void CountTrace(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    std::string value = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    int64_t count = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
    HiTraceAdapterImpl::GetInstance().CountTrace(value, count);
}

void IsHiTraceEnable() {
    HiTraceAdapterImpl::GetInstance().IsHiTraceEnable();
}

void StartOHOSTrace(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    std::string value = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    int limit = dataProvider.ConsumeIntegralInRange<int>(-1, MAX_INT_SIZE);
    float limit_float = static_cast<float>(limit);
    HiTraceAdapterImpl::GetInstance().StartOHOSTrace(value, limit_float);
}

void FinishOHOSTrace() {
    HiTraceAdapterImpl::GetInstance().FinishOHOSTrace();
}

void CountOHOSTrace(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    std::string value = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    int64_t count = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
    HiTraceAdapterImpl::GetInstance().CountOHOSTrace(value, count);
}

void IsACETraceEnable() {
    HiTraceAdapterImpl::GetInstance().IsACETraceEnable();
}

// main
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    StartTrace(data, size);
    FinishTrace();
    StartAsyncTrace(data, size);
    FinishAsyncTrace(data, size);
    CountTrace(data, size);
    IsHiTraceEnable();
    
    StartOHOSTrace(data, size);
    FinishOHOSTrace();
    CountOHOSTrace(data, size);
    IsACETraceEnable();
    return 0;
}