/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "power_mgr_client_adapter_impl.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::PowerMgr;
using namespace OHOS::NWeb;

namespace OHOS::NWeb {
class PowerMgrClientAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void PowerMgrClientAdapterImplTest::SetUpTestCase(void)
{}

void PowerMgrClientAdapterImplTest::TearDownTestCase(void)
{}

void PowerMgrClientAdapterImplTest::SetUp(void)
{}

void PowerMgrClientAdapterImplTest::TearDown(void)
{}

/**
 * @tc.name: PowerMgrClientAdapterImplTest_001.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PowerMgrClientAdapterImplTest, PowerMgrClientAdapterImplTest_001, TestSize.Level1)
{
    std::unique_ptr<PowerMgrClientAdapterImpl> powerMgrImpl = std::make_unique<PowerMgrClientAdapterImpl>();
    EXPECT_NE(powerMgrImpl, nullptr);
    std::shared_ptr<RunningLockAdapter> runLockAdapter = powerMgrImpl->CreateRunningLock("screen",
        RunningLockAdapterType::SCREEN);
    EXPECT_NE(runLockAdapter, nullptr);
    EXPECT_FALSE(runLockAdapter->IsUsed());
    EXPECT_NE(runLockAdapter->Lock(1), -1);
    EXPECT_NE(runLockAdapter->UnLock(), -1);
    runLockAdapter = powerMgrImpl->CreateRunningLock("backgroud", RunningLockAdapterType::BACKGROUND);
    EXPECT_NE(runLockAdapter, nullptr);
    runLockAdapter = powerMgrImpl->CreateRunningLock("proximity_screen_control",
        RunningLockAdapterType::PROXIMITY_SCREEN_CONTROL);
    EXPECT_NE(runLockAdapter, nullptr);
    runLockAdapter = powerMgrImpl->CreateRunningLock("butt", RunningLockAdapterType::BUTT);
    EXPECT_NE(runLockAdapter, nullptr);
}

/**
 * @tc.name: PowerMgrClientAdapterImplTest_002.
 * @tc.desc: test running lock, exception cases.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PowerMgrClientAdapterImplTest, PowerMgrClientAdapterImplTest_002, TestSize.Level1)
{
    std::shared_ptr<RunningLockAdapter> runLockAdapter = std::make_shared<RunningLockAdapterImpl>(nullptr);
    EXPECT_NE(runLockAdapter, nullptr);
    EXPECT_FALSE(runLockAdapter->IsUsed());
    EXPECT_EQ(runLockAdapter->Lock(1), -1);
    EXPECT_EQ(runLockAdapter->UnLock(), -1);
}
}