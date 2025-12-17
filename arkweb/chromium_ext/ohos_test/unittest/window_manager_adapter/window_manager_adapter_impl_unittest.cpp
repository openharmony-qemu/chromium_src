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

#include "window_manager_adapter_impl.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <window_manager/oh_window.h>

#include <cstdint>
#include <cstring>
#include <memory>

#include "arkweb/ohos_nweb/src/nweb_hilog.h"
#include "ohos_sdk/openharmony/native/sysroot/usr/include/AbilityKit/ability_runtime/ability_runtime_common.h"

#define private public
#include "arkweb/ohos_adapter_ndk/window_manager_adapter/include/window_manager_adapter_impl.h"
#undef private

using namespace testing;
using testing::_;
using testing::Return;

namespace OHOS::NWeb {

class WindowManagerAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void WindowManagerAdapterImplTest::SetUpTestCase() {}

void WindowManagerAdapterImplTest::TearDownTestCase() {}

void WindowManagerAdapterImplTest::SetUp() {}

void WindowManagerAdapterImplTest::TearDown() {}

TEST_F(WindowManagerAdapterImplTest, LockPointer)
{
    int32_t res = OHOS::NWeb::WindowManagerAdapterImpl::LockPointer(0);
    EXPECT_NE(res, 0);
}

TEST_F(WindowManagerAdapterImplTest, UnlockPointer)
{
    int32_t res = OHOS::NWeb::WindowManagerAdapterImpl::UnlockPointer(0);
    EXPECT_NE(res, 0);
}
}