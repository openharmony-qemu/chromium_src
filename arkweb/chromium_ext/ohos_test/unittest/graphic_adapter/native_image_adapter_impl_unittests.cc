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

#include <cstdint>
#include <cstring>
#include <memory>
#include <gtest/gtest.h>
#include <native_image/graphic_error_code.h>
#include <native_window/external_window.h>
#include <native_buffer/native_buffer.h>
#define private public
#include "arkweb/ohos_adapter_ndk/graphic_adapter/native_image_adapter_impl.h"
#undef private
#include "arkweb/ohos_adapter_ndk/mock_ndk_api/include/mock_ndk_api.h"
 
using namespace testing;
using namespace MockNdkApi;
 
namespace OHOS::NWeb {
constexpr int BUFFER_SIZE = 10;
constexpr int BITS_PER_PIXEL = 4;
class NativeImageAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

class FrameAvailableListenerTest : public FrameAvailableListener {
public:
    FrameAvailableListenerTest() {}
    ~FrameAvailableListenerTest() {}
    void OnFrameAvailableListener() override { return nullptr; }
};

void NativeImageAdapterImplTest::SetUpTestCase() {}

void NativeImageAdapterImplTest::TearDownTestCase() {}

void NativeImageAdapterImplTest::SetUp() {}

void NativeImageAdapterImplTest::TearDown() {}

void TestBufferAllocateForNativeImageAdapter(void** outBuffer)
{
    if (outBuffer == nullptr) {
        return;
    }

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

TEST_F(NativeImageAdapterImplTest, NativeImageAdapterImplTest_001)
{
    std::shared_ptr<NativeImageAdapterImpl> adapter = std::make_shared<NativeImageAdapterImpl>();
    EXPECT_NE(adapter, nullptr);
    int32_t ret = adapter->UpdateSurfaceImage();
    EXPECT_EQ(ret, NATIVE_ERROR_UNKNOWN);
    NWebNativeWindow window = adapter->AquireNativeWindowFromNativeImage();
    EXPECT_EQ(window, nullptr);
    int32_t context = adapter->AttachContext(1);
    EXPECT_EQ(context, NATIVE_ERROR_UNKNOWN);
    context = adapter->DetachContext();
    EXPECT_EQ(context, NATIVE_ERROR_UNKNOWN);
    uint32_t textureId = 1;
    uint32_t textureTarget = 1;
    adapter->CreateNativeImage(textureId, textureTarget);
    ret = adapter->UpdateSurfaceImage();
    EXPECT_NE(ret, NATIVE_ERROR_UNKNOWN);
    window = adapter->AquireNativeWindowFromNativeImage();
    EXPECT_NE(window, nullptr);
    adapter->AttachContext(1);
    adapter->DetachContext();
}

TEST_F(NativeImageAdapterImplTest, NativeImageAdapterImplTest_002)
{
    std::shared_ptr<NativeImageAdapterImpl> adapter = std::make_shared<NativeImageAdapterImpl>();
    EXPECT_NE(adapter, nullptr);
    int32_t ret = adapter->GetTimestamp();
    EXPECT_EQ(ret, NATIVE_ERROR_UNKNOWN);
    uint32_t textureId = 1;
    uint32_t textureTarget = 1;
    adapter->CreateNativeImage(textureId, textureTarget);
    ret = adapter->GetTimestamp();
    EXPECT_EQ(ret, 0);
}

TEST_F(NativeImageAdapterImplTest, NativeImageAdapterImplTest_003)
{
    std::shared_ptr<NativeImageAdapterImpl> adapter = std::make_shared<NativeImageAdapterImpl>();
    EXPECT_NE(adapter, nullptr);
    float matrix[16] = { 0 };
    int32_t ret = adapter->GetTransformMatrix(matrix);
    EXPECT_EQ(ret, NATIVE_ERROR_UNKNOWN);
    uint32_t textureId = 1;
    uint32_t textureTarget = 1;
    adapter->CreateNativeImage(textureId, textureTarget);
    ret = adapter->GetTransformMatrix(matrix);
    EXPECT_EQ(ret, 0);
}

TEST_F(NativeImageAdapterImplTest, NativeImageAdapterImplTest_004)
{
    std::shared_ptr<NativeImageAdapterImpl> adapter = std::make_shared<NativeImageAdapterImpl>();
    EXPECT_NE(adapter, nullptr);
    uint64_t id = 1;
    int32_t ret = adapter->GetSurfaceId(&id);
    EXPECT_EQ(ret, NATIVE_ERROR_UNKNOWN);
    uint32_t textureId = 1;
    uint32_t textureTarget = 1;
    adapter->CreateNativeImage(textureId, textureTarget);
    ret = adapter->GetSurfaceId(&id);
    EXPECT_EQ(ret, 0);
}

TEST_F(NativeImageAdapterImplTest, NativeImageAdapterImplTest_005)
{
    std::shared_ptr<NativeImageAdapterImpl> adapter = std::make_shared<NativeImageAdapterImpl>();
    EXPECT_NE(adapter, nullptr);
    int32_t ret = adapter->SetOnFrameAvailableListener(nullptr);
    EXPECT_EQ(ret, NATIVE_ERROR_UNKNOWN);
    uint32_t textureId = 1;
    uint32_t textureTarget = 1;
    adapter->CreateNativeImage(textureId, textureTarget);
    ret = adapter->SetOnFrameAvailableListener(nullptr);
    EXPECT_EQ(ret, NATIVE_ERROR_UNKNOWN);
    std::shared_ptr<FrameAvailableListener> listener = std::make_shared<FrameAvailableListenerTest>();
    ret = adapter->SetOnFrameAvailableListener(listener);
    EXPECT_NE(ret, NATIVE_ERROR_UNKNOWN);
}

TEST_F(NativeImageAdapterImplTest, NativeImageAdapterImplTest_006)
{
    std::shared_ptr<NativeImageAdapterImpl> adapter = std::make_shared<NativeImageAdapterImpl>();
    EXPECT_NE(adapter, nullptr);
    int32_t ret = adapter->UnsetOnFrameAvailableListener();
    EXPECT_EQ(ret, NATIVE_ERROR_UNKNOWN);
    uint32_t textureId = 1;
    uint32_t textureTarget = 1;
    adapter->CreateNativeImage(textureId, textureTarget);
    ret = adapter->UnsetOnFrameAvailableListener();
    EXPECT_EQ(ret, 0);
}

TEST_F(NativeImageAdapterImplTest, NativeImageAdapterImplTest_007)
{
    std::shared_ptr<NativeImageAdapterImpl> adapter = std::make_shared<NativeImageAdapterImpl>();
    EXPECT_NE(adapter, nullptr);
    adapter->ohNativeImage_ = nullptr;
    adapter->DestroyNativeImage();
    adapter->NewNativeImage();
    EXPECT_NE(adapter->ohNativeImage_, nullptr);
    adapter->DestroyNativeImage();
    EXPECT_EQ(adapter->ohNativeImage_, nullptr);
}

TEST_F(NativeImageAdapterImplTest, NativeImageAdapterImplTest_008)
{
    std::shared_ptr<NativeImageAdapterImpl> adapter = std::make_shared<NativeImageAdapterImpl>();
    EXPECT_NE(adapter, nullptr);
    void* windowBuffer = nullptr;
    int fenceId = -1;
    int32_t ret = NATIVE_ERROR_OK;
    ret = adapter->AcquireNativeWindowBuffer(nullptr, &fenceId);
    EXPECT_EQ(ret, NATIVE_ERROR_UNKNOWN);
    ret = adapter->AcquireNativeWindowBuffer(&windowBuffer, nullptr);
    EXPECT_EQ(ret, NATIVE_ERROR_UNKNOWN);
    ret = adapter->AcquireNativeWindowBuffer(&windowBuffer, &fenceId);
    EXPECT_EQ(ret, NATIVE_ERROR_UNKNOWN);
    adapter->NewNativeImage();
    EXPECT_NE(adapter->ohNativeImage_, nullptr);
    ret = adapter->AcquireNativeWindowBuffer(&windowBuffer, &fenceId);
    EXPECT_NE(ret, 0);

    g_mock_OH_NativeImage_AcquireNativeWindowBuffer = [](OH_NativeImage* image,
        OHNativeWindowBuffer** nativeWindowBuffer, int* fenceFd) {
        return NATIVE_ERROR_OK;
    };
    ret = adapter->AcquireNativeWindowBuffer(&windowBuffer, &fenceId);
    EXPECT_EQ(ret, NATIVE_ERROR_OK);
    g_mock_OH_NativeImage_AcquireNativeWindowBuffer = nullptr;
}

TEST_F(NativeImageAdapterImplTest, NativeImageAdapterImplTest_009)
{
    std::shared_ptr<NativeImageAdapterImpl> adapter = std::make_shared<NativeImageAdapterImpl>();
    EXPECT_NE(adapter, nullptr);
    void* buffer = nullptr;
    TestBufferAllocateForNativeImageAdapter(&buffer);
    OHNativeWindowBuffer* windowBuffer =
        OH_NativeWindow_CreateNativeWindowBufferFromNativeBuffer(static_cast<OH_NativeBuffer*>(buffer));
    void* nativeBuffer = nullptr;
    int32_t ret = NATIVE_ERROR_OK;
    ret = adapter->GetNativeBuffer(nullptr, &nativeBuffer);
    EXPECT_EQ(ret, NATIVE_ERROR_UNKNOWN);
    ret = adapter->GetNativeBuffer(windowBuffer, nullptr);
    EXPECT_EQ(ret, NATIVE_ERROR_UNKNOWN);
    ret = adapter->GetNativeBuffer(windowBuffer, &nativeBuffer);
    EXPECT_EQ(ret, NATIVE_ERROR_OK);

    g_mock_OH_NativeBuffer_FromNativeWindowBuffer = [](OHNativeWindowBuffer* nativeWindowBuffer,
        OH_NativeBuffer** nativeBuffer) {
        return NATIVE_ERROR_INVALID_ARGUMENTS;
    };
    ret = adapter->GetNativeBuffer(windowBuffer, &nativeBuffer);
    EXPECT_EQ(ret, NATIVE_ERROR_INVALID_ARGUMENTS);
    OH_NativeWindow_DestroyNativeWindowBuffer(windowBuffer);
    windowBuffer = nullptr;
    g_mock_OH_NativeBuffer_FromNativeWindowBuffer = nullptr;
}

TEST_F(NativeImageAdapterImplTest, NativeImageAdapterImplTest_010)
{
    std::shared_ptr<NativeImageAdapterImpl> adapter = std::make_shared<NativeImageAdapterImpl>();
    EXPECT_NE(adapter, nullptr);
    void* buffer = nullptr;
    TestBufferAllocateForNativeImageAdapter(&buffer);
    OHNativeWindowBuffer* windowBuffer =
        OH_NativeWindow_CreateNativeWindowBufferFromNativeBuffer(static_cast<OH_NativeBuffer*>(buffer));
    int fenceId = -1;
    int32_t ret = adapter->ReleaseNativeWindowBuffer(nullptr, fenceId);
    EXPECT_EQ(ret, NATIVE_ERROR_UNKNOWN);
    adapter->NewNativeImage();
    EXPECT_NE(adapter->ohNativeImage_, nullptr);
    ret = adapter->ReleaseNativeWindowBuffer(nullptr, fenceId);
    EXPECT_EQ(ret, NATIVE_ERROR_UNKNOWN);
    ret = adapter->ReleaseNativeWindowBuffer(windowBuffer, fenceId);
    EXPECT_NE(ret, NATIVE_ERROR_OK);
    OH_NativeWindow_DestroyNativeWindowBuffer(windowBuffer);
    windowBuffer = nullptr;
}

TEST_F(NativeImageAdapterImplTest, NativeImageAdapterImplTest_011)
{
    std::shared_ptr<NativeImageAdapterImpl> adapter = std::make_shared<NativeImageAdapterImpl>();
    EXPECT_NE(adapter, nullptr);
    void* windowBuffer = nullptr;
    uint32_t width = 0;
    uint32_t height = 0;
    adapter->GetNativeWindowBufferSize(windowBuffer, nullptr, nullptr);
    adapter->GetNativeWindowBufferSize(windowBuffer, &width, nullptr);
    adapter->GetNativeWindowBufferSize(windowBuffer, nullptr, &height);
    adapter->GetNativeWindowBufferSize(windowBuffer, &width, &height);
    windowBuffer = new uint8_t[BUFFER_SIZE * BUFFER_SIZE * BITS_PER_PIXEL];
    adapter->GetNativeWindowBufferSize(windowBuffer, nullptr, nullptr);
    adapter->GetNativeWindowBufferSize(windowBuffer, &width, nullptr);
    adapter->GetNativeWindowBufferSize(windowBuffer, nullptr, &height);
    g_mock_OH_NativeWindow_GetBufferHandleFromNative = [](OHNativeWindowBuffer* nativeWindowBuffer) {
        return nullptr;
    };
    adapter->GetNativeWindowBufferSize(windowBuffer, &width, &height);
    EXPECT_EQ(width, static_cast<uint32_t>(0));
    EXPECT_EQ(height, static_cast<uint32_t>(0));
    BufferHandle handle;
    handle.width = 1;
    handle.height = 1;

    g_mock_OH_NativeWindow_GetBufferHandleFromNative = [&handle](OHNativeWindowBuffer* nativeWindowBuffer) {
        return &handle;
    };
    adapter->GetNativeWindowBufferSize(windowBuffer, &width, &height);

    EXPECT_EQ(width, handle.width);
    EXPECT_EQ(height, handle.height);
    g_mock_OH_NativeWindow_GetBufferHandleFromNative = nullptr;
    delete[] reinterpret_cast<uint8_t*>(windowBuffer);
}
} // namespace OHOS::NWeb
