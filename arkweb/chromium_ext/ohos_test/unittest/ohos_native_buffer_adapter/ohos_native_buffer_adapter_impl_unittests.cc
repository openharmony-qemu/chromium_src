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
#include <memory>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <sys/mman.h>

#define private public
#include "arkweb/ohos_adapter_ndk/ohos_native_buffer_adapter/ohos_native_buffer_adapter_impl.h"
#undef private

using namespace testing;

namespace OHOS::NWeb {

class NativeBufferAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NativeBufferAdapterImplTest::SetUpTestCase() {}

void NativeBufferAdapterImplTest::TearDownTestCase() {}

void NativeBufferAdapterImplTest::SetUp() {}

void NativeBufferAdapterImplTest::TearDown() {}

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

TEST_F(NativeBufferAdapterImplTest, NativeBufferAdapterImplTest_001)
{
    std::shared_ptr<OhosNativeBufferAdapterImpl> adapter = std::make_shared<OhosNativeBufferAdapterImpl>();
    EXPECT_NE(adapter, nullptr);
    void* buffer = nullptr;
    adapter->AcquireBuffer(buffer);
    void* nativeBuffer = nullptr;
    TestAllocate(&nativeBuffer);
    EXPECT_NE(nativeBuffer, nullptr);
    adapter->AcquireBuffer(nativeBuffer);
}

TEST_F(NativeBufferAdapterImplTest, NativeBufferAdapterImplTest_002)
{
    std::shared_ptr<OhosNativeBufferAdapterImpl> adapter = std::make_shared<OhosNativeBufferAdapterImpl>();
    EXPECT_NE(adapter, nullptr);
    void* buffer = nullptr;
    adapter->Release(buffer);
    void* nativeBuffer = nullptr;
    TestAllocate(&nativeBuffer);
    EXPECT_NE(nativeBuffer, nullptr);
    adapter->Release(nativeBuffer);
}

TEST_F(NativeBufferAdapterImplTest, NativeBufferAdapterImplTest_003)
{
    std::shared_ptr<OhosNativeBufferAdapterImpl> adapter = std::make_shared<OhosNativeBufferAdapterImpl>();
    EXPECT_NE(adapter, nullptr);
    void* buffer = nullptr;
    void* eglBuffer = nullptr;
    adapter->GetEGLBuffer(buffer, &eglBuffer);
    EXPECT_EQ(eglBuffer, nullptr);
    int ret = adapter->FreeEGLBuffer(eglBuffer);
    EXPECT_EQ(ret, -1);
    void* nativeBuffer = nullptr;
    TestAllocate(&nativeBuffer);
    EXPECT_NE(nativeBuffer, nullptr);
    adapter->GetEGLBuffer(nativeBuffer, &eglBuffer);
    EXPECT_NE(eglBuffer, nullptr);
    ret = adapter->FreeEGLBuffer(eglBuffer);
    EXPECT_EQ(ret, 0);
}

TEST_F(NativeBufferAdapterImplTest, NativeBufferAdapterImplTest_004)
{
    std::shared_ptr<OhosNativeBufferAdapterImpl> adapter = std::make_shared<OhosNativeBufferAdapterImpl>();
    EXPECT_NE(adapter, nullptr);
    void* nativeWindowBuffer = nullptr;
    void* nativeBuffer = nullptr;
    int ret = adapter->NativeBufferFromNativeWindowBuffer(nativeWindowBuffer, &nativeBuffer);
    EXPECT_EQ(ret, 0);
}

TEST_F(NativeBufferAdapterImplTest, NativeBufferAdapterImplTest_005)
{
    std::shared_ptr<OhosNativeBufferAdapterImpl> adapter = std::make_shared<OhosNativeBufferAdapterImpl>();
    EXPECT_NE(adapter, nullptr);
    void* buffer = nullptr;
    int ret = adapter->GetSeqNum(buffer);
    EXPECT_EQ(ret, 0);
    void* nativeBuffer = nullptr;
    TestAllocate(&nativeBuffer);
    EXPECT_NE(nativeBuffer, nullptr);
    ret = adapter->GetSeqNum(nativeBuffer);
    EXPECT_NE(ret, 0);
}

TEST_F(NativeBufferAdapterImplTest, NativeBufferAdapterImplTest_006)
{
    std::shared_ptr<OhosNativeBufferAdapterImpl> adapter = std::make_shared<OhosNativeBufferAdapterImpl>();
    EXPECT_NE(adapter, nullptr);
    void* outBuffer = nullptr;
    adapter->Allocate(nullptr, &outBuffer);
    EXPECT_EQ(outBuffer, nullptr);
}

TEST_F(NativeBufferAdapterImplTest, NativeBufferAdapterImplTest_007)
{
    std::shared_ptr<OhosNativeBufferAdapterImpl> adapter = std::make_shared<OhosNativeBufferAdapterImpl>();
    EXPECT_NE(adapter, nullptr);
    void* buffer = nullptr;
    std::shared_ptr<NativeBufferConfigAdapter> bufferConfig = nullptr;
    adapter->Describe(bufferConfig, buffer);
    EXPECT_EQ(bufferConfig, nullptr);
}


TEST_F(NativeBufferAdapterImplTest, NativeBufferAdapterImplTest_008)
{
    std::shared_ptr<OhosNativeBufferAdapterImpl> adapter = std::make_shared<OhosNativeBufferAdapterImpl>();
    EXPECT_NE(adapter, nullptr);
    void* buffer = nullptr;
    void* address = nullptr;
    bool res = adapter->IsBufferLocked(nullptr);
    EXPECT_FALSE(res);
    int ret = adapter->Lock(buffer, 0, 0, &address);
    EXPECT_EQ(ret, -1);
    int fence = -1;
    ret = adapter->Unlock(buffer, &fence);
    EXPECT_EQ(ret, -1);

    void* nativeBuffer = nullptr;
    TestAllocate(&nativeBuffer);
    EXPECT_NE(nativeBuffer, nullptr);
    res = adapter->IsBufferLocked(static_cast<OH_NativeBuffer*>(nativeBuffer));
    EXPECT_FALSE(res);
    ret = adapter->Unlock(nativeBuffer, &fence);
    EXPECT_EQ(ret, -1);
    ret = adapter->Lock(nativeBuffer, 0, 0, &address);
    EXPECT_EQ(ret, 0);
    ret = adapter->Unlock(nativeBuffer, &fence);
    EXPECT_EQ(ret, 0);
}

TEST_F(NativeBufferAdapterImplTest, NativeBufferAdapterImplTest_009)
{
    std::shared_ptr<OhosNativeBufferAdapterImpl> adapter = std::make_shared<OhosNativeBufferAdapterImpl>();
    EXPECT_NE(adapter, nullptr);
    void* outBuffer = nullptr;
    int ret = adapter->RecvHandleFromUnixSocket(-1, &outBuffer);
    EXPECT_EQ(outBuffer, nullptr);
    EXPECT_EQ(ret, 0);
}

TEST_F(NativeBufferAdapterImplTest, NativeBufferAdapterImplTest_010)
{
    std::shared_ptr<OhosNativeBufferAdapterImpl> adapter = std::make_shared<OhosNativeBufferAdapterImpl>();
    EXPECT_NE(adapter, nullptr);
    void* buffer = nullptr;
    int ret = adapter->SendHandleToUnixSocket(buffer, -1);
    EXPECT_EQ(ret, -1);
    void* nativeBuffer = nullptr;
    TestAllocate(&nativeBuffer);
    EXPECT_NE(nativeBuffer, nullptr);
    ret = adapter->SendHandleToUnixSocket(nativeBuffer, -1);
    EXPECT_EQ(ret, 0);
}

TEST_F(NativeBufferAdapterImplTest, NativeBufferAdapterImplTest_011)
{
    std::shared_ptr<OhosNativeBufferAdapterImpl> adapter = std::make_shared<OhosNativeBufferAdapterImpl>();
    EXPECT_NE(adapter, nullptr);
    void* buffer = nullptr;
    int ret = adapter->FreeNativeBuffer(buffer);
    EXPECT_EQ(ret, -1);
    void* nativeBuffer = nullptr;
    TestAllocate(&nativeBuffer);
    EXPECT_NE(nativeBuffer, nullptr);
    ret = adapter->FreeNativeBuffer(nativeBuffer);
    EXPECT_EQ(ret, 0);
}
} // namespace OHOS::NWeb