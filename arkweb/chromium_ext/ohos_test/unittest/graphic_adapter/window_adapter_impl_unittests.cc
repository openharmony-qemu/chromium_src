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
#include "arkweb/ohos_adapter_ndk/graphic_adapter/window_adapter_impl.h"
#undef private
#include "arkweb/ohos_adapter_ndk/mock_ndk_api/include/mock_ndk_api.h"

using namespace testing;
using namespace MockNdkApi;

namespace OHOS::NWeb {

class WindowAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void WindowAdapterImplTest::SetUpTestCase() {}

void WindowAdapterImplTest::TearDownTestCase() {}

void WindowAdapterImplTest::SetUp() {}

void WindowAdapterImplTest::TearDown() {}

TEST_F(WindowAdapterImplTest, WindowAdapterImplTest_001)
{
    WindowAdapterNdkImpl &adapter = WindowAdapterNdkImpl::GetInstance();
    NWebNativeWindow window = adapter.CreateNativeWindowFromSurface(nullptr);
    EXPECT_EQ(window, nullptr);
    adapter.AddNativeWindowRef(window);
    int ret = adapter.GetNativeWindowQueueSize(window);
    EXPECT_EQ(ret, 0);
    adapter.NativeWindowUnRef(window);
}

TEST_F(WindowAdapterImplTest, WindowAdapterImplTest_002)
{
    WindowAdapterNdkImpl &adapter = WindowAdapterNdkImpl::GetInstance();
    auto ret = adapter.GetNativeWindowRequestBuffer(nullptr, nullptr, nullptr);
    EXPECT_NE(ret, 0);

    ret = adapter.GetNativeWindowAbortBuffer(nullptr, nullptr);
    EXPECT_NE(ret, 0);
}

TEST_F(WindowAdapterImplTest, AddNativeWindowRef)
{
    WindowAdapterNdkImpl &adapter = WindowAdapterNdkImpl::GetInstance();
    g_mock_OH_NativeWindow_NativeObjectReference = [](void *obj) {
        return 0;
    };
    ASSERT_NO_FATAL_FAILURE(adapter.AddNativeWindowRef(nullptr));
    g_mock_OH_NativeWindow_NativeObjectReference = [](void *obj) {
        return __real_OH_NativeWindow_NativeObjectReference(obj);
    };
    ASSERT_NO_FATAL_FAILURE(adapter.AddNativeWindowRef(nullptr));
    g_mock_OH_NativeWindow_NativeObjectReference = nullptr;
}

TEST_F(WindowAdapterImplTest, NativeWindowUnRef)
{
    WindowAdapterNdkImpl &adapter = WindowAdapterNdkImpl::GetInstance();
    g_mock_OH_NativeWindow_NativeObjectUnreference = [](void *obj) {
        return 0;
    };
    ASSERT_NO_FATAL_FAILURE(adapter.NativeWindowUnRef(nullptr));
    g_mock_OH_NativeWindow_NativeObjectUnreference = [](void *obj) {
        return __real_OH_NativeWindow_NativeObjectUnreference(obj);
    };
    ASSERT_NO_FATAL_FAILURE(adapter.NativeWindowUnRef(nullptr));
    g_mock_OH_NativeWindow_NativeObjectUnreference = nullptr;
}

TEST_F(WindowAdapterImplTest, GetNativeWindowRequestBuffer)
{
    WindowAdapterNdkImpl &adapter = WindowAdapterNdkImpl::GetInstance();
    g_mock_OH_NativeWindow_NativeWindowRequestBuffer = [](OHNativeWindow *window,
        OHNativeWindowBuffer **buffer, int *fenceFd) {
        return 0;
    };
    int32_t ret = adapter.GetNativeWindowRequestBuffer(nullptr, nullptr, nullptr);
    EXPECT_EQ(ret, 0);
    g_mock_OH_NativeWindow_NativeWindowRequestBuffer = [](OHNativeWindow *window,
        OHNativeWindowBuffer **buffer, int *fenceFd) {
        return __real_OH_NativeWindow_NativeWindowRequestBuffer(window, buffer, fenceFd);
    };
    ret = adapter.GetNativeWindowRequestBuffer(nullptr, nullptr, nullptr);
    EXPECT_NE(ret, 0);
    g_mock_OH_NativeWindow_NativeWindowRequestBuffer = nullptr;
}

TEST_F(WindowAdapterImplTest, GetNativeWindowAbortBuffer)
{
    WindowAdapterNdkImpl &adapter = WindowAdapterNdkImpl::GetInstance();
    g_mock_OH_NativeWindow_NativeWindowAbortBuffer = [](OHNativeWindow *window,
        OHNativeWindowBuffer *buffer) {
        return 0;
    };
    int32_t ret = adapter.GetNativeWindowAbortBuffer(nullptr, nullptr);
    EXPECT_EQ(ret, 0);
    g_mock_OH_NativeWindow_NativeWindowAbortBuffer = [](OHNativeWindow *window,
        OHNativeWindowBuffer *buffer) {
        return __real_OH_NativeWindow_NativeWindowAbortBuffer(window, buffer);
    };
    ret = adapter.GetNativeWindowAbortBuffer(nullptr, nullptr);
    EXPECT_NE(ret, 0);
    g_mock_OH_NativeWindow_NativeWindowAbortBuffer = nullptr;
}
} // namespace OHOS::NWeb