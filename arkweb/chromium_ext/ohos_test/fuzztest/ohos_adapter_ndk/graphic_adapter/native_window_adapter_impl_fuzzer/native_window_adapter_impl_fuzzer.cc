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

#include <memory>
#define private public
#include "arkweb/ohos_adapter_ndk/graphic_adapter/native_window_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/graphic_adapter/native_image_adapter_impl.h"
#undef private

#include <cstring>
#include <fuzzer/FuzzedDataProvider.h>

using namespace OHOS::NWeb;
namespace OHOS {

class MockIBufferConsumerListenerAdapter : public IBufferConsumerListenerAdapter {
public:
    MockIBufferConsumerListenerAdapter() = default;
    ~MockIBufferConsumerListenerAdapter() = default;

    void OnBufferAvailable(std::shared_ptr<SurfaceBufferAdapter> buffer) override {}
};

class MockBufferRequestConfigAdapter : public BufferRequestConfigAdapter {
public:
    MockBufferRequestConfigAdapter() = default;

    ~MockBufferRequestConfigAdapter() = default;

    int32_t GetWidth() override { return 1; }

    int32_t GetHeight() override { return 1; }

    int32_t GetStrideAlignment() override { return 1; }

    int32_t GetFormat() override { return 1; }

    uint64_t GetUsage() override { return 1; }

    int32_t GetTimeout() override { return 1; }

    ColorGamutAdapter GetColorGamut() override { return ColorGamutAdapter::NATIVE; }

    TransformTypeAdapter GetTransformType() override { return TransformTypeAdapter::ROTATE_90; }

    int64 GetTimestamp() override { return 1; }
};

constexpr int INVALID_VALUE = -2;
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

    OH_NativeBuffer_Config config = {
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

bool NativeBufferConsumerListenerImplFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    NativeBufferConsumerListenerImpl impl =
        NativeBufferConsumerListenerImpl(nullptr, nullptr);
    impl.OnBufferAvailable();
    std::shared_ptr<NativeImageAdapterImpl> adapter =
        std::make_shared<NativeImageAdapterImpl>();
    if (adapter == nullptr) {
        return false;
    }
    adapter->NewNativeImage();
    std::shared_ptr<IBufferConsumerListenerAdapter> listener =
        std::make_shared<MockIBufferConsumerListenerAdapter>();
    NativeBufferConsumerListenerImpl nativeImpl =
        NativeBufferConsumerListenerImpl(adapter->ohNativeImage_, listener);
    nativeImpl.OnBufferAvailable();

    return true;
}

bool ConsumerNativeAdapterImplFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    ConsumerNativeAdapterImpl impl = ConsumerNativeAdapterImpl();
    std::shared_ptr<IBufferConsumerListenerAdapter> listener =
        std::make_shared<MockIBufferConsumerListenerAdapter>();
    impl.RegisterConsumerListener(listener);
    impl.RegisterConsumerListener(nullptr);
    int32_t fence = dataProvider.ConsumeIntegralInRange<int32_t>(1, MAX_SIZE);
    impl.ReleaseBuffer(nullptr, fence);
    std::string key = "key";
    std::string val = "val";
    impl.SetUserData(key, val);
    uint32_t queueSize = dataProvider.ConsumeIntegralInRange<uint32_t>(1, MAX_SET_NUMBER);
    impl.SetQueueSize(queueSize);
    impl.GetConsumerSurface();

    return true;
}

bool ProducerNativeAdapterImplFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    ProducerNativeAdapterImpl impl = ProducerNativeAdapterImpl(nullptr);
    int32_t random = dataProvider.ConsumeIntegralInRange<int32_t>(INVALID_VALUE, MAX_SIZE);
    TransformTypeAdapter type = static_cast<TransformTypeAdapter>(random);
    impl.TransToTransformType(type);
    ColorGamutAdapter colorGamut = static_cast<ColorGamutAdapter>(random);
    impl.TransToColorGamut(colorGamut);
    impl.TransToBufferConfig(nullptr);
    int32_t fence = dataProvider.ConsumeIntegralInRange<int32_t>(-1, MAX_SIZE);
    impl.RequestBuffer(fence, nullptr);
    std::shared_ptr<BufferRequestConfigAdapter> configAdapter =
        std::make_shared<MockBufferRequestConfigAdapter>();
    impl.RequestBuffer(fence, configAdapter);
    impl.TransToBufferConfig(configAdapter);
    impl.FlushBuffer(nullptr, fence, nullptr);

    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::NativeWindowAdapterImplFuzzTest(data, size);
    OHOS::NativeBufferConsumerListenerImplFuzzTest(data, size);
    OHOS::ConsumerNativeAdapterImplFuzzTest(data, size);
    OHOS::ProducerNativeAdapterImplFuzzTest(data, size);
    return 0;
}