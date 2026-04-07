/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "components/memory_pressure/system_memory_pressure_evaluator_ohos.h"

class MemoryPressureTestHelper {
public:
    static bool TestGetSystemMemoryInfo(base::SystemMemoryInfoKB* mem_info) {
        return base::GetSystemMemoryInfo(mem_info);
    }
};

namespace OHOS::NWeb {

void SystemMemoryPressureEvaluatorOhosFuzz001Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    
    // Test GetSystemMemoryInfo
    base::SystemMemoryInfoKB mem_info;
    bool result = MemoryPressureTestHelper::TestGetSystemMemoryInfo(&mem_info);
    // The result could be true or false depending on the system, but the function should not crash
}

void SystemMemoryPressureEvaluatorOhosFuzz002Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    
    // Test multiple calls with different contexts
    for (int i = 0; i < 5; i++) {
        base::SystemMemoryInfoKB mem_info;
        bool result = MemoryPressureTestHelper::TestGetSystemMemoryInfo(&mem_info);
    }
}

void SystemMemoryPressureEvaluatorOhosFuzz003Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    
    // Test with multiple memory info objects
    base::SystemMemoryInfoKB mem_info1;
    base::SystemMemoryInfoKB mem_info2;
    base::SystemMemoryInfoKB mem_info3;
    
    bool result1 = MemoryPressureTestHelper::TestGetSystemMemoryInfo(&mem_info1);
    bool result2 = MemoryPressureTestHelper::TestGetSystemMemoryInfo(&mem_info2);
    bool result3 = MemoryPressureTestHelper::TestGetSystemMemoryInfo(&mem_info3);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    SystemMemoryPressureEvaluatorOhosFuzz001Test(data, size);
    SystemMemoryPressureEvaluatorOhosFuzz002Test(data, size);
    SystemMemoryPressureEvaluatorOhosFuzz003Test(data, size);
    return 0;
}
}   // namespace OHOS::NWeb
