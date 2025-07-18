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
 
#define private public
#include "arkweb/ohos_adapter_ndk/graphic_adapter/native_image_adapter_impl.h"
#undef private
 
using namespace testing;
 
namespace OHOS::NWeb {
constexpr int BUFFER_SIZE = 10;
constexpr int BITS_PER_PIXEL = 4;
constexpr int NATIVE_ERROR_UNKNOWN = 50002000;
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
    void* GetContext() override { return nullptr; }
    OnFrameAvailableCb GetOnFrameAvailableCb() override { return nullptr; }
};

void NativeImageAdapterImplTest::SetUpTestCase() {}

void NativeImageAdapterImplTest::TearDownTestCase() {}

void NativeImageAdapterImplTest::SetUp() {}

void NativeImageAdapterImplTest::TearDown() {}

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
    int32_t ret = adapter->AcquireNativeWindowBuffer(&windowBuffer, &fenceId);
    EXPECT_EQ(ret, NATIVE_ERROR_UNKNOWN);
    adapter->NewNativeImage();
    EXPECT_NE(adapter->ohNativeImage_, nullptr);
    ret = adapter->AcquireNativeWindowBuffer(&windowBuffer, &fenceId);
    EXPECT_NE(ret, 0);
}

TEST_F(NativeImageAdapterImplTest, NativeImageAdapterImplTest_009)
{
    std::shared_ptr<NativeImageAdapterImpl> adapter = std::make_shared<NativeImageAdapterImpl>();
    EXPECT_NE(adapter, nullptr);
    void* windowBuffer = nullptr;
    void* nativeBuffer = nullptr;
    adapter->GetNativeBuffer(windowBuffer, &nativeBuffer);
}

TEST_F(NativeImageAdapterImplTest, NativeImageAdapterImplTest_010)
{
    std::shared_ptr<NativeImageAdapterImpl> adapter = std::make_shared<NativeImageAdapterImpl>();
    EXPECT_NE(adapter, nullptr);
    void* windowBuffer = nullptr;
    int fenceId = -1;
    int32_t ret = adapter->ReleaseNativeWindowBuffer(windowBuffer, fenceId);
    EXPECT_EQ(ret, NATIVE_ERROR_UNKNOWN);
    adapter->NewNativeImage();
    EXPECT_NE(adapter->ohNativeImage_, nullptr);
    ret = adapter->ReleaseNativeWindowBuffer(windowBuffer, fenceId);
    EXPECT_NE(ret, NATIVE_ERROR_UNKNOWN);
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
    adapter->GetNativeWindowBufferSize(windowBuffer, &width, &height);
    delete[] reinterpret_cast<uint8_t*>(windowBuffer);
    EXPECT_EQ(width, 0);
    EXPECT_EQ(height, 0);
}
} // namespace OHOS::NWeb
