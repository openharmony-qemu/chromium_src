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
#include "arkweb/ohos_adapter_ndk/ohos_native_buffer_adapter/ohos_native_buffer_adapter_impl.h"
#undef private

#include <cstring>
#include <fuzzer/FuzzedDataProvider.h>

using namespace OHOS::NWeb;
namespace OHOS {

void TestAllocate(void** outBuffer)
{
    OH_NativeBuffer_Config config = {
        .width = 10,
        .height = 10,
        .format = OH_NativeBuffer_Format::NATIVEBUFFER_PIXEL_FMT_RGBA_8888,
        .usage = 1,
        .stride = 1,
    };

    OH_NativeBuffer* buffer = OH_NativeBuffer_Alloc(&config);
    if (buffer != nullptr) {
        *outBuffer = buffer;
    } else {
        *outBuffer = nullptr;
    }
}

constexpr int MAX_SET_NUMBER = 1000;

bool OhosNativeBufferAdapterImplFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    uint64_t usage = dataProvider.ConsumeIntegralInRange<uint64_t>(1, MAX_SET_NUMBER);
    int32_t fence = dataProvider.ConsumeIntegralInRange<int32_t>(1, MAX_SET_NUMBER);
    int socketFd = dataProvider.ConsumeIntegralInRange<int>(1, MAX_SET_NUMBER);
    std::shared_ptr<OhosNativeBufferAdapterImpl> adapter = std::make_shared<OhosNativeBufferAdapterImpl>();
    adapter->GetInstance();
    void* buffer = nullptr;
    adapter->AcquireBuffer(buffer);
    adapter->IsBufferLocked(static_cast<OH_NativeBuffer*>(buffer));
    adapter->Release(buffer);
    adapter->GetSeqNum(buffer);
    void* outBuffer = nullptr;
    adapter->Allocate(nullptr, &outBuffer);
    adapter->Describe(nullptr, buffer);
    adapter->RecvHandleFromUnixSocket(socketFd, &outBuffer);
    adapter->SendHandleToUnixSocket(outBuffer, socketFd);
    void* eglBuffer = nullptr;
    adapter->GetEGLBuffer(buffer, &eglBuffer);
    adapter->FreeEGLBuffer(eglBuffer);
    void* nativeWindowBuffer = nullptr;
    void* nativeBuffer = nullptr;
    adapter->NativeBufferFromNativeWindowBuffer(nativeWindowBuffer, &nativeBuffer);
    void* address = nullptr;
    adapter->Lock(buffer, usage, fence, &address);
    adapter->Unlock(buffer, &fence);
    adapter->FreeNativeBuffer(buffer);
    TestAllocate(&nativeBuffer);
    adapter->AcquireBuffer(nativeBuffer);
    adapter->IsBufferLocked(static_cast<OH_NativeBuffer*>(nativeBuffer));
    adapter->GetSeqNum(buffer);
    adapter->GetEGLBuffer(nativeBuffer, &eglBuffer);
    adapter->FreeEGLBuffer(eglBuffer);
    adapter->Release(nativeBuffer);
    TestAllocate(&buffer);
    adapter->SendHandleToUnixSocket(buffer, socketFd);
    adapter->Unlock(buffer, &fence);
    adapter->Lock(buffer, usage, fence, &address);
    adapter->Unlock(buffer, &fence);
    adapter->FreeNativeBuffer(buffer);

    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::OhosNativeBufferAdapterImplFuzzTest(data, size);
    return 0;
}