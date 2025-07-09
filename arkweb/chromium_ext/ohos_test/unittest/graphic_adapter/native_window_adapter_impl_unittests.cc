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
#undef private

using namespace testing;

namespace OHOS::NWeb {

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
    int32_t ret = adapter.Stride();
    EXPECT_EQ(ret, -1);
    void* buffer = nullptr;
    TestBufferAllocate(&buffer);
    OHNativeWindowBuffer* nativeWindowBuffer =
        OH_NativeWindow_CreateNativeWindowBufferFromNativeBuffer(static_cast<OH_NativeBuffer*>(buffer));
    NativeBufferAdapterImpl nativeAdapter = NativeBufferAdapterImpl(nativeWindowBuffer);
    ret = nativeAdapter.Stride();
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
    EXPECT_EQ(ret, -1);
    void* addr = afapter.GetVirAddr();
    EXPECT_EQ(addr, nullptr);
    void* buffer = nullptr;
    TestBufferAllocate(&buffer);
    OHNativeWindowBuffer* nativeWindowBuffer =
        OH_NativeWindow_CreateNativeWindowBufferFromNativeBuffer(static_cast<OH_NativeBuffer*>(buffer));
    NativeBufferAdapterImpl nativeAdapter = NativeBufferAdapterImpl(nativeWindowBuffer);
    ret = nativeAdapter.GetSize();
    EXPECT_NE(ret, -1);
    addr = nativeAdapter.GetVirAddr();
    EXPECT_NE(addr, nullptr);
}
} // namespace OHOS::NWeb