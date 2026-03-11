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
#include "arkweb/chromium_ext/third_party/crashpad/crashpad/util/linux/proc_info_ohos.h"

namespace crashpad {
namespace {
    constexpr uint32_t MAX_UINT32_SIZE = 255;
    constexpr int32_t MAX_INT32_SIZE = 255;
    constexpr int32_t MIN_INT32_SIZE = -255;
}

void ProcInfoOhosFuzz001Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    
    // Test GetProcStatus
    crashpad::ProcInfo procInfo;

    bool result = crashpad::GetProcStatus(procInfo);
}

void ProcInfoOhosFuzz002Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    
    // Test GetProcStatusByPid
    int32_t pid = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT32_SIZE);
    crashpad::ProcInfo procInfo;
    bool result = crashpad::GetProcStatusByPid(pid, procInfo);
}

void ProcInfoOhosFuzz003Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    
    // Test TidToNstid
    int pid = dataProvider.ConsumeIntegralInRange<int>(1, MAX_INT32_SIZE);
    int tid = dataProvider.ConsumeIntegralInRange<int>(1, MAX_INT32_SIZE);
    int nstid = 0;
    TidToNstid(pid, tid, nstid);
}

void ProcInfoOhosFuzz004Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    
    // Test IsThreadInPid
    int32_t pid = dataProvider.ConsumeIntegralInRange<int32_t>(1, MAX_INT32_SIZE);
    int32_t tid = dataProvider.ConsumeIntegralInRange<int32_t>(1, MAX_INT32_SIZE);
    IsThreadInPid(pid, tid);
}

void ProcInfoOhosFuzz005Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    FuzzedDataProvider dataProvider(data, size);
    
    // Test GetTidMapByPid
    int pid = dataProvider.ConsumeIntegralInRange<int>(1, MAX_INT32_SIZE);
    std::unordered_map<pid_t, pid_t> tid_nstid_map;
    GetTidMapByPid(pid, tid_nstid_map);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    // Test 1: GetProcStatus - Get current process status
    ProcInfoOhosFuzz001Test(data, size);
    
    // Test 2: GetProcStatusByPid - Get process status by PID
    ProcInfoOhosFuzz002Test(data, size);
    
    // Test 3: TidToNstid - Convert thread ID to namespace thread ID
    ProcInfoOhosFuzz003Test(data, size);
    
    // Test 4: IsThreadInPid - Check if thread belongs to specified process
    ProcInfoOhosFuzz004Test(data, size);
    
    // Test 5: ReadDirFiles - Read files in directory
    ProcInfoOhosFuzz005Test(data, size);
    
    return 0;
}

}   // namespace crashpad