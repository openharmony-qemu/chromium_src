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
#include "arkweb/ohos_adapter_ndk/graphic_adapter/native_image_adapter_impl.h"
#undef private

#include <cstring>
#include <fuzzer/FuzzedDataProvider.h>

using namespace OHOS::NWeb;
namespace OHOS {

constexpr int MAX_SET_NUMBER = 1000;
constexpr int BUFFER_SIZE = 10;
constexpr int BITS_PER_PIXEL = 4;

class FrameAvailableListenerFuzzTest : public FrameAvailableListener {
public:
    FrameAvailableListenerFuzzTest() {}
    ~FrameAvailableListenerFuzzTest() {}
    void OnFrameAvailableListener() {}
};

bool NativeImageAdapterImplFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    std::shared_ptr<NativeImageAdapterImpl> adapter = std::make_shared<NativeImageAdapterImpl>();
    uint32_t textureId = dataProvider.ConsumeIntegralInRange<uint32_t>(1, MAX_SET_NUMBER);
    uint32_t textureTarget = dataProvider.ConsumeIntegralInRange<uint32_t>(1, MAX_SET_NUMBER);
    adapter->AquireNativeWindowFromNativeImage();
    adapter->AttachContext(textureId);
    adapter->DetachContext();
    adapter->UpdateSurfaceImage();
    adapter->GetTimestamp();
    float matrix[16] = { 0 };
    adapter->GetTransformMatrix(matrix);
    uint64_t id = dataProvider.ConsumeIntegralInRange<uint64_t>(1, MAX_SET_NUMBER);
    adapter->GetSurfaceId(&id);
    std::shared_ptr<FrameAvailableListener> listener = std::make_shared<FrameAvailableListenerFuzzTest>();
    adapter->SetOnFrameAvailableListener(listener);
    adapter->UnsetOnFrameAvailableListener();
    adapter->DestroyNativeImage();
    void* buffer = nullptr;
    int fd = dataProvider.ConsumeIntegralInRange<int>(1, MAX_SET_NUMBER);
    adapter->AcquireNativeWindowBuffer(&buffer, &fd);
    void* nativeBuffer = nullptr;
    adapter->GetNativeBuffer(buffer, &nativeBuffer);
    adapter->ReleaseNativeWindowBuffer(nativeBuffer, fd);
    adapter->CreateNativeImage(textureId, textureTarget);

    adapter->NewNativeImage();
    adapter->AquireNativeWindowFromNativeImage();
    adapter->AttachContext(textureId);
    adapter->DetachContext();
    adapter->UpdateSurfaceImage();
    adapter->GetTimestamp();
    adapter->GetTransformMatrix(matrix);
    adapter->GetSurfaceId(&id);
    adapter->SetOnFrameAvailableListener(listener);
    adapter->UnsetOnFrameAvailableListener();
    adapter->DestroyNativeImage();
    adapter->AcquireNativeWindowBuffer(&buffer, &fd);
    adapter->GetNativeBuffer(buffer, &nativeBuffer);
    adapter->ReleaseNativeWindowBuffer(nativeBuffer, fd);
    void* windowBuffer = nullptr;
    uint32_t width = 0;
    uint32_t height = 0;
    adapter->GetNativeWindowBufferSize(windowBuffer, &width, &height);

    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::NativeImageAdapterImplFuzzTest(data, size);
    return 0;
}