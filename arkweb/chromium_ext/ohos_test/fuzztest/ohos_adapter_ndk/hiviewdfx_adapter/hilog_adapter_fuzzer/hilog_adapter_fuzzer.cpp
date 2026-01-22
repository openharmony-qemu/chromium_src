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
#include "hilog_adapter_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "hilog_adapter.h"
#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>
#include <algorithm>

using namespace OHOS::NWeb;

namespace {
  constexpr uint8_t MAX_STRING_LENGTH = UINT8_MAX;
  constexpr uint32_t MAX_LOG_LEVEL = 5;
}

void PrintLogFuzzTest(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
      return;
    }
    FuzzedDataProvider dataProvider(data, size);
    uint32_t num = dataProvider.ConsumeIntegralInRange<int>(0, MAX_LOG_LEVEL);
    LogLevelAdapter logLevel = static_cast<LogLevelAdapter>(num);
    
    std::string tag = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    std::string fmt = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    std::string message = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    HiLogAdapter::PrintLog(logLevel, tag.c_str(), "%s", message.c_str());
}

void PrintConsoleLogFuzzTest(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
      return;
    }
    FuzzedDataProvider dataProvider(data, size);
    uint32_t num = dataProvider.ConsumeIntegralInRange<int>(0, MAX_LOG_LEVEL);
    LogLevelAdapter logLevel = static_cast<LogLevelAdapter>(num);
    
    std::string tag = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    std::string fmt = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    std::string message = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    HiLogAdapter::PrintConsoleLog(logLevel, tag.c_str(), "%s", message.c_str());
}

// main
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    PrintLogFuzzTest(data, size);
    PrintConsoleLogFuzzTest(data, size);
    return 0;
}