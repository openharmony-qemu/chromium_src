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

#include <gtest/gtest.h>

#include "screenlock_manager.h"
#define private public
#include "screenlock_manager_adapter_impl.h"
#undef private

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::ScreenLock;
using namespace OHOS::NWeb;

namespace OHOS::NWeb {
class ScreenlockManagerAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void ScreenlockManagerAdapterImplTest::SetUpTestCase(void) {}

void ScreenlockManagerAdapterImplTest::TearDownTestCase(void) {}

void ScreenlockManagerAdapterImplTest::SetUp(void) {}

void ScreenlockManagerAdapterImplTest::TearDown(void) {}

/**
 * @tc.name: ScreenlockManagerAdapterImplTest_IsLocked_001.
 * @tc.desc: test isLocked.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ScreenlockManagerAdapterImplTest, ScreenlockManagerAdapterImplTest_IsLocked_001, TestSize.Level1)
{
    ScreenlockManagerAdapterImpl impl;
    bool isLocked = impl.IsLocked();
    bool screenLocked = false;
    ScreenLockManager::GetInstance()->IsLocked(screenLocked);
    EXPECT_EQ(isLocked, screenLocked);
}
} // namespace OHOS::NWeb
