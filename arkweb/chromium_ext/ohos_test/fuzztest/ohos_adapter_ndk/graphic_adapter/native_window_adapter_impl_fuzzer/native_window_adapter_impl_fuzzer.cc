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
#include "arkweb/ohos_adapter_ndk/graphic_adapter/native_window_adapter_impl.h"
#undef private

#include <cstring>
#include <fuzzer/FuzzedDataProvider.h>

using namespace OHOS::NWeb;
namespace OHOS {

constexpr int MAX_SET_NUMBER = 1000;
constexpr int MAX_SIZE = 10;

bool NativeWindowAdapterImplFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    NativeBufferAdapterImpl adapter = NativeBufferAdapterImpl(nullptr);
    adapter.GetFileDescriptor();
    adapter.GetWidth();
    adapter.GetHeight();
    adapter.GetStride();
    adapter.GetFormat();
    adapter.GetSize();
    adapter.GetVirAddr();
    adapter.GetBuffer();

    OH_NativeBuffer_config config = {
        .width = dataProvider.ConsumeIntegralInRange<uint32_t>(1, MAX_SET_NUMBER),
        .height = dataProvider.ConsumeIntegralInRange<uint32_t>(1, MAX_SET_NUMBER),
        .format = OH_NativeBuffer_Format::NATIVEBUFFER_PIXEL_FMT_RGBA_8888,
        .usage = dataProvider.ConsumeIntegralInRange<uint32_t>(1, MAX_SIZE),
        .stride = dataProvider.ConsumeIntegralInRange<uint32_t>(1, MAX_SIZE),
    };
    void* buffer = OH_NativeBuffer_Alloc(&config);
    OHNativeWindowBuffer* nativeWindowBuffer =
        OH_NativeWindow_CreateNativeWindowBufferFromNativeBuffer(static_cast<OH_NativeBuffer*>(buffer));
    NativeBufferAdapterImpl nativeAdapter = NativeBufferAdapterImpl(nativeWindowBuffer);
    nativeAdapter.GetFileDescriptor();
    nativeAdapter.GetWidth();
    nativeAdapter.GetHeight();
    nativeAdapter.GetStride();
    nativeAdapter.GetFormat();
    nativeAdapter.GetSize();
    nativeAdapter.GetVirAddr();

    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::NativeWindowAdapterImplFuzzTest(data, size);
    return 0;
}