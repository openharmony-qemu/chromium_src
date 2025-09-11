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

using namespace testing;

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
} // namespace OHOS::NWeb