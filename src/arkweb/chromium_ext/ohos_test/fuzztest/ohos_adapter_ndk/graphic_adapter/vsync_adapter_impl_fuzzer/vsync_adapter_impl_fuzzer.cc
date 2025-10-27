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
#include "arkweb/ohos_adapter_ndk/graphic_adapter/vsync_adapter_impl.h"
#undef private

#include <cstring>
#include <fuzzer/FuzzedDataProvider.h>

using namespace OHOS::NWeb;
namespace OHOS {

static void OnVsyncCallback() {}

constexpr int MAX_SET_NUMBER = 1000;

bool VsyncAdapterImplFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    VSyncAdapterNdkImpl &adapter = VSyncAdapterNdkImpl::GetInstance();
    long long timestamp = dataProvider.ConsumeIntegralInRange<long long>(0, MAX_SET_NUMBER);
    adapter.Init();
    adapter.RequestVsync(nullptr, nullptr);
    adapter.hasRequestedVsync_ = false;
    adapter.RequestVsync(nullptr, nullptr);
    adapter.OnVsync(timestamp, nullptr);
    VSyncAdapterNdkImpl vsyncAdapter;
    void* client = &vsyncAdapter;
    adapter.OnVsync(timestamp, client);
    adapter.VsyncCallbackInner(timestamp);
    adapter.GetVSyncPeriod();
    adapter.SetFrameRateLinkerEnable(false);
    adapter.SetFramePreferredRate(0);
    adapter.SetOnVsyncCallback(OnVsyncCallback);
    adapter.SetIsGPUProcess(false);
    adapter.SetOnVsyncEndCallback(OnVsyncCallback);
    adapter.SetScene("", 0);
    adapter.SetDVSyncSwitch(false);

    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::VsyncAdapterImplFuzzTest(data, size);
    return 0;
}