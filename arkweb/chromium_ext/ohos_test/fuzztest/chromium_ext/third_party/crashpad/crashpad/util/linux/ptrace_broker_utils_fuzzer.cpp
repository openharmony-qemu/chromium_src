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
#include <unordered_map>
#include "arkweb/chromium_ext/third_party/crashpad/crashpad/util/linux/ptrace_broker_utils.h"

namespace crashpad {
namespace {
    constexpr uint32_t MAX_UINT32_SIZE = 255;
    constexpr int32_t MAX_INT32_SIZE = 255;
    constexpr int32_t MIN_INT32_SIZE = -255;
}

// Mock PtraceBroker class for testing
class MockPtraceBroker {
public:
    explicit MockPtraceBroker(bool is_in_pid_ns) : is_in_pid_ns_(is_in_pid_ns) {
        // Add some test entries to the map
        if (is_in_pid_ns) {
            tid_nstid_map_[1] = 1001;
            tid_nstid_map_[2] = 1002;
            tid_nstid_map_[3] = 1003;
        }
    }
    
    bool is_in_pid_ns_;
    std::unordered_map<int, int> tid_nstid_map_;
};

void PtraceBrokerUtilsFuzz001Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    
    // Test ConvertRealtidToNstid with null ptraceBroker
    int real_tid = dataProvider.ConsumeIntegralInRange<int>(1, MAX_INT32_SIZE);
    int result = PtraceBrokerUtils::ConvertRealtidToNstid(real_tid, nullptr);
}

void PtraceBrokerUtilsFuzz002Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    
    // Test ConvertRealtidToNstid with ptraceBroker not in pid namespace
    MockPtraceBroker mockBroker(false);
    int real_tid = dataProvider.ConsumeIntegralInRange<int>(1, MAX_INT32_SIZE);
    int result = PtraceBrokerUtils::ConvertRealtidToNstid(real_tid, reinterpret_cast<PtraceBroker*>(&mockBroker));
}

void PtraceBrokerUtilsFuzz003Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    
    // Test ConvertRealtidToNstid with ptraceBroker in pid namespace and existing tid
    MockPtraceBroker mockBroker(true);
    int real_tid = dataProvider.ConsumeIntegralInRange<int>(1, 3); // Use existing tids in map
    int result = PtraceBrokerUtils::ConvertRealtidToNstid(real_tid, reinterpret_cast<PtraceBroker*>(&mockBroker));
}

void PtraceBrokerUtilsFuzz004Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    
    // Test ConvertRealtidToNstid with ptraceBroker in pid namespace and non-existing tid
    MockPtraceBroker mockBroker(true);
    int real_tid = dataProvider.ConsumeIntegralInRange<int>(4, MAX_INT32_SIZE); // Use non-existing tids
    int result = PtraceBrokerUtils::ConvertRealtidToNstid(real_tid, reinterpret_cast<PtraceBroker*>(&mockBroker));
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    // Test 1: ConvertRealtidToNstid with null ptraceBroker
    PtraceBrokerUtilsFuzz001Test(data, size);
    
    // Test 2: ConvertRealtidToNstid with ptraceBroker not in pid namespace
    PtraceBrokerUtilsFuzz002Test(data, size);
    
    // Test 3: ConvertRealtidToNstid with ptraceBroker in pid namespace and existing tid
    PtraceBrokerUtilsFuzz003Test(data, size);
    
    // Test 4: ConvertRealtidToNstid with ptraceBroker in pid namespace and non-existing tid
    PtraceBrokerUtilsFuzz004Test(data, size);
    
    return 0;
}

}   // namespace crashpad