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
#include "arkweb/ohos_adapter_ndk/graphic_adapter/window_adapter_impl.h"
#undef private

#include <cstring>
#include <fuzzer/FuzzedDataProvider.h>

using namespace OHOS::NWeb;
namespace OHOS {

constexpr int MAX_SET_NUMBER = 1000;

bool WindowAdapterImplFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    WindowAdapterNdkImpl &adapter = WindowAdapterNdkImpl::GetInstance();
    NWebNativeWindow window = adapter.CreateNativeWindowFromSurface(nullptr);
    int32_t width = dataProvider.ConsumeIntegralInRange<int32_t>(1, MAX_SET_NUMBER);
    int32_t height = dataProvider.ConsumeIntegralInRange<int32_t>(1, MAX_SET_NUMBER);
    adapter.NativeWindowSetBufferGeometry(window, width, height);
    adapter.NativeWindowSurfaceCleanCache(window);
    adapter.NativeWindowSurfaceCleanCacheWithPara(window, false);
    uint32_t rotation = dataProvider.ConsumeIntegralInRange<uint32_t>(1, MAX_SET_NUMBER);
    adapter.SetTransformHint(rotation, window);
    adapter.AddNativeWindowRef(window);
    adapter.NativeWindowUnRef(window);
    adapter.GetNativeWindowQueueSize(window);
    adapter.DestroyNativeWindow(window);

    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::WindowAdapterImplFuzzTest(data, size);
    return 0;
}