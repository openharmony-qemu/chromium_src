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

#include <cstring>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <sys/mman.h>

#define private public
#include "arkweb/ohos_adapter_ndk/graphic_adapter/native_window_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/graphic_adapter/native_image_adapter_impl.h"
#undef private

using namespace testing;

namespace OHOS::NWeb {

constexpr int INVALID_COLOR_GAMUT_ADAPTER = -2;

class NativeWindowAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NativeWindowAdapterImplTest::SetUpTestCase() {}

void NativeWindowAdapterImplTest::TearDownTestCase() {}

void NativeWindowAdapterImplTest::SetUp() {}

void NativeWindowAdapterImplTest::TearDown() {}

class MockIBufferConsumerListenerAdapter : public IBufferConsumerListenerAdapter {
public:
    MockIBufferConsumerListenerAdapter() = default;
    ~MockIBufferConsumerListenerAdapter() = default;

    void OnBufferAvailable(std::shared_ptr<SurfaceBufferAdapter> buffer) override {}
};

class MockSurfaceBufferAdapter : public SurfaceBufferAdapter {
public:
    MockSurfaceBufferAdapter() = default;

    ~MockSurfaceBufferAdapter() = default;

    int32_t GetFileDescriptor() override { return 0; }

    int32_t GetWidth() override { return 0; }

    int32_t GetHeight() override { return 0; }

    int32_t GetStride() override { return 0; }

    int32_t GetFormat() override { return 0; }

    uint32_t GetSize() override { return 0; }

    void* GetVirAddr() override { return nullptr; }
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

    int64_t GetTimestamp() override { return 1; }
};

class MockBufferFlushConfigAdapter : public BufferFlushConfigAdapter {
public:
    MockBufferFlushConfigAdapter() = default;

    ~MockBufferFlushConfigAdapter() = default;

    int32_t GetX() override { return 0; }

    int32_t GetY() override { return 0; }

    int32_t GetW() override { return 0; }

    int32_t GetH() override { return 0; }

    int64_t GetTimestamp() override { return 0; }
};

void TestBufferAllocate(void** outBuffer)
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

TEST_F(NativeWindowAdapterImplTest, NativeWindowAdapterImplTest_001)
{
    NativeBufferAdapterImpl adapter = NativeBufferAdapterImpl(nullptr);
    int32_t ret = adapter.GetFileDescriptor();
    EXPECT_EQ(ret, -1);
    void* buffer = nullptr;
    TestBufferAllocate(&buffer);
    OHNativeWindowBuffer* nativeWindowBuffer =
        OH_NativeWindow_CreateNativeWindowBufferFromNativeBuffer(static_cast<OH_NativeBuffer*>(buffer));
    NativeBufferAdapterImpl nativeAdapter = NativeBufferAdapterImpl(nativeWindowBuffer);
    ret = nativeAdapter.GetFileDescriptor();
    EXPECT_NE(ret, -1);
}

TEST_F(NativeWindowAdapterImplTest, NativeWindowAdapterImplTest_002)
{
    NativeBufferAdapterImpl adapter = NativeBufferAdapterImpl(nullptr);
    int32_t ret = adapter.GetWidth();
    EXPECT_EQ(ret, -1);
    void* buffer = nullptr;
    TestBufferAllocate(&buffer);
    OHNativeWindowBuffer* nativeWindowBuffer =
        OH_NativeWindow_CreateNativeWindowBufferFromNativeBuffer(static_cast<OH_NativeBuffer*>(buffer));
    NativeBufferAdapterImpl nativeAdapter = NativeBufferAdapterImpl(nativeWindowBuffer);
    ret = nativeAdapter.GetWidth();
    EXPECT_NE(ret, -1);
}

TEST_F(NativeWindowAdapterImplTest, NativeWindowAdapterImplTest_003)
{
    NativeBufferAdapterImpl adapter = NativeBufferAdapterImpl(nullptr);
    int32_t ret = adapter.GetHeight();
    EXPECT_EQ(ret, -1);
    void* buffer = nullptr;
    TestBufferAllocate(&buffer);
    OHNativeWindowBuffer* nativeWindowBuffer =
        OH_NativeWindow_CreateNativeWindowBufferFromNativeBuffer(static_cast<OH_NativeBuffer*>(buffer));
    NativeBufferAdapterImpl nativeAdapter = NativeBufferAdapterImpl(nativeWindowBuffer);
    ret = nativeAdapter.GetHeight();
    EXPECT_NE(ret, -1);
}

TEST_F(NativeWindowAdapterImplTest, NativeWindowAdapterImplTest_004)
{
    NativeBufferAdapterImpl adapter = NativeBufferAdapterImpl(nullptr);
    int32_t ret = adapter.GetStride();
    EXPECT_EQ(ret, -1);
    void* buffer = nullptr;
    TestBufferAllocate(&buffer);
    OHNativeWindowBuffer* nativeWindowBuffer =
        OH_NativeWindow_CreateNativeWindowBufferFromNativeBuffer(static_cast<OH_NativeBuffer*>(buffer));
    NativeBufferAdapterImpl nativeAdapter = NativeBufferAdapterImpl(nativeWindowBuffer);
    ret = nativeAdapter.GetStride();
    EXPECT_NE(ret, -1);
}

TEST_F(NativeWindowAdapterImplTest, NativeWindowAdapterImplTest_005)
{
    NativeBufferAdapterImpl adapter = NativeBufferAdapterImpl(nullptr);
    int32_t ret = adapter.GetFormat();
    EXPECT_EQ(ret, -1);
    void* buffer = nullptr;
    TestBufferAllocate(&buffer);
    OHNativeWindowBuffer* nativeWindowBuffer =
        OH_NativeWindow_CreateNativeWindowBufferFromNativeBuffer(static_cast<OH_NativeBuffer*>(buffer));
    NativeBufferAdapterImpl nativeAdapter = NativeBufferAdapterImpl(nativeWindowBuffer);
    ret = nativeAdapter.GetFormat();
    EXPECT_NE(ret, -1);
}

TEST_F(NativeWindowAdapterImplTest, NativeWindowAdapterImplTest_006)
{
    NativeBufferAdapterImpl adapter = NativeBufferAdapterImpl(nullptr);
    uint32_t ret = adapter.GetSize();
    EXPECT_EQ(ret, 0u);
    void* addr = adapter.GetVirAddr();
    EXPECT_EQ(addr, nullptr);
    void* buffer = nullptr;
    TestBufferAllocate(&buffer);
    OHNativeWindowBuffer* nativeWindowBuffer =
        OH_NativeWindow_CreateNativeWindowBufferFromNativeBuffer(static_cast<OH_NativeBuffer*>(buffer));
    NativeBufferAdapterImpl nativeAdapter = NativeBufferAdapterImpl(nativeWindowBuffer);
    ret = nativeAdapter.GetSize();
    EXPECT_NE(ret, 0u);
    addr = nativeAdapter.GetVirAddr();
    EXPECT_NE(addr, nullptr);
}

TEST_F(NativeWindowAdapterImplTest, NativeWindowAdapterImplTest_007)
{
    NativeBufferConsumerListenerImpl impl = NativeBufferConsumerListenerImpl(nullptr, nullptr);
    impl.OnBufferAvailable();
    std::shared_ptr<NativeImageAdapterImpl> adapter = std::make_shared<NativeImageAdapterImpl>();
    EXPECT_NE(adapter, nullptr);
    adapter->NewNativeImage();
    EXPECT_NE(adapter->ohNativeImage_, nullptr);
    std::shared_ptr<IBufferConsumerListenerAdapter> listener =
        std::make_shared<MockIBufferConsumerListenerAdapter>();
    NativeBufferConsumerListenerImpl nativeImpl =
        NativeBufferConsumerListenerImpl(adapter->ohNativeImage_, listener);
    nativeImpl.OnBufferAvailable();
    NativeBufferConsumerListenerImpl impl1 =
        NativeBufferConsumerListenerImpl(nullptr, listener);
    impl1.OnBufferAvailable();
    NativeBufferConsumerListenerImpl impl2 =
        NativeBufferConsumerListenerImpl(adapter->ohNativeImage_, nullptr);
    impl2.OnBufferAvailable();
}

TEST_F(NativeWindowAdapterImplTest, NativeWindowAdapterImplTest_008)
{
    ConsumerNativeAdapterImpl impl = ConsumerNativeAdapterImpl();
    EXPECT_NE(impl.cImage_, nullptr);
    std::shared_ptr<IBufferConsumerListenerAdapter> listener =
        std::make_shared<MockIBufferConsumerListenerAdapter>();
    int32_t ret = impl.RegisterConsumerListener(listener);
    EXPECT_NE(ret, -1);
    ret = impl.RegisterConsumerListener(nullptr);
    EXPECT_EQ(ret, -1);
    impl.cImage_ = nullptr;
    ret = impl.RegisterConsumerListener(listener);
    EXPECT_EQ(ret, -1);
    ret = impl.RegisterConsumerListener(nullptr);
    EXPECT_EQ(ret, -1);
}

TEST_F(NativeWindowAdapterImplTest, NativeWindowAdapterImplTest_009)
{
    ProducerNativeAdapterImpl impl = ProducerNativeAdapterImpl(nullptr);
    TransformTypeAdapter type = TransformTypeAdapter::ROTATE_90;
    OH_NativeBuffer_TransformType ohType = impl.TransToTransformType(type);
    EXPECT_EQ(ohType, OH_NativeBuffer_TransformType::NATIVEBUFFER_ROTATE_90);
    type = static_cast<TransformTypeAdapter>(-1);
    ohType = impl.TransToTransformType(type);
    EXPECT_EQ(ohType, OH_NativeBuffer_TransformType::NATIVEBUFFER_ROTATE_NONE);
}

TEST_F(NativeWindowAdapterImplTest, NativeWindowAdapterImplTest_010)
{
    ProducerNativeAdapterImpl impl = ProducerNativeAdapterImpl(nullptr);
    ColorGamutAdapter colorGamut = ColorGamutAdapter::NATIVE;
    OH_NativeBuffer_ColorGamut ohColorGamut = impl.TransToGraphicColorGamut(colorGamut);
    EXPECT_EQ(ohColorGamut, OH_NativeBuffer_ColorGamut::NATIVEBUFFER_COLOR_GAMUT_NATIVE);
    colorGamut = static_cast<ColorGamutAdapter>(INVALID_COLOR_GAMUT_ADAPTER);
    ohColorGamut = impl.TransToGraphicColorGamut(colorGamut);
    EXPECT_EQ(ohColorGamut, OH_NativeBuffer_ColorGamut::NATIVEBUFFER_COLOR_GAMUT_SRGB);
}

TEST_F(NativeWindowAdapterImplTest, NativeWindowAdapterImplTest_011)
{
    ProducerNativeAdapterImpl impl = ProducerNativeAdapterImpl(nullptr);
    impl.TransToBufferConfig(nullptr);
    int32_t fence = -1;
    std::shared_ptr<BufferRequestConfigAdapter> configAdapter = nullptr;
    auto buffer = impl.RequestBuffer(fence, configAdapter);
    EXPECT_EQ(buffer, nullptr);
    configAdapter = std::make_shared<MockBufferRequestConfigAdapter>();
    buffer = impl.RequestBuffer(fence, configAdapter);
    EXPECT_EQ(buffer, nullptr);
    impl.TransToBufferConfig(configAdapter);

    ConsumerNativeAdapterImpl consumerImpl = ConsumerNativeAdapterImpl();
    EXPECT_NE(consumerImpl.cImage_, nullptr);
    int32_t ret = consumerImpl.ReleaseBuffer(nullptr, fence);
    EXPECT_EQ(ret, -1);
    OHNativeWindow* nativeWindow = OH_NativeImage_AcquireNativeWindow(consumerImpl.cImage_);
    EXPECT_NE(nativeWindow, nullptr);
    ProducerNativeAdapterImpl nativeImpl = ProducerNativeAdapterImpl(nativeWindow);
    EXPECT_NE(nativeImpl.window_, nullptr);
    buffer = nativeImpl.RequestBuffer(fence, nullptr);
    EXPECT_EQ(buffer, nullptr);
    buffer = nativeImpl.RequestBuffer(fence, configAdapter);
    EXPECT_NE(buffer, nullptr);

    std::shared_ptr<BufferFlushConfigAdapter> flushConfigAdapter =
        std::make_shared<MockBufferFlushConfigAdapter>();
    ret = impl.FlushBuffer(nullptr, fence, nullptr);
    EXPECT_EQ(ret, -1);
    ret = impl.FlushBuffer(buffer, fence, flushConfigAdapter);
    EXPECT_EQ(ret, -1);
    ret = nativeImpl.FlushBuffer(buffer, fence, nullptr);
    EXPECT_EQ(ret, -1);
    ret = nativeImpl.FlushBuffer(buffer, fence, flushConfigAdapter);
    EXPECT_NE(ret, -1);
    ret = consumerImpl.ReleaseBuffer(buffer, fence);
    EXPECT_NE(ret, -1);
}
} // namespace OHOS::NWeb