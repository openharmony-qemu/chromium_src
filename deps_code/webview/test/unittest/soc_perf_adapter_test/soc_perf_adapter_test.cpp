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
#include "soc_perf_client_adapter_impl.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::NWeb;

namespace OHOS::NWeb {
namespace {
    const uint32_t SOC_PERF_ID = UINT32_MAX;
}

class SocPerfAdapterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void SocPerfAdapterTest::SetUpTestCase(void)
{}

void SocPerfAdapterTest::TearDownTestCase(void)
{}

void SocPerfAdapterTest::SetUp(void)
{}

void SocPerfAdapterTest::TearDown(void)
{}

/**
 * @tc.name: SocPerfAdapterTest_001.
 * @tc.desc: test apply socperf config
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SocPerfAdapterTest, SocPerfAdapterTest_001, TestSize.Level1)
{
    std::unique_ptr<SocPerfClientAdapterImpl> socPerfClient = std::make_unique<SocPerfClientAdapterImpl>();
    EXPECT_NE(socPerfClient, nullptr);
    const int SOC_PERF_CONFIG_ID = 10020;
    socPerfClient->ApplySocPerfConfigById(SOC_PERF_CONFIG_ID);
    socPerfClient->ApplySocPerfConfigById(0);
    socPerfClient->ApplySocPerfConfigByIdEx(SOC_PERF_CONFIG_ID, true);
    socPerfClient->ApplySocPerfConfigByIdEx(0, true);

    int SOC_PERF_ID = 10012;
    socPerfClient->ApplySocPerfConfigById(SOC_PERF_ID);
    socPerfClient->ApplySocPerfConfigByIdEx(SOC_PERF_ID, true);
    socPerfClient->ApplySocPerfConfigByIdEx(SOC_PERF_ID, false);

    SOC_PERF_ID = 10020;
    socPerfClient->ApplySocPerfConfigById(SOC_PERF_ID);
    socPerfClient->ApplySocPerfConfigByIdEx(SOC_PERF_ID, true);
    socPerfClient->ApplySocPerfConfigByIdEx(SOC_PERF_ID, false);

    SOC_PERF_ID = 10025;
    socPerfClient->ApplySocPerfConfigById(SOC_PERF_ID);
    socPerfClient->ApplySocPerfConfigByIdEx(SOC_PERF_ID, true);
    socPerfClient->ApplySocPerfConfigByIdEx(SOC_PERF_ID, false);

    SOC_PERF_ID = 10070;
    socPerfClient->ApplySocPerfConfigById(SOC_PERF_ID);
    socPerfClient->ApplySocPerfConfigByIdEx(SOC_PERF_ID, true);
    socPerfClient->ApplySocPerfConfigByIdEx(SOC_PERF_ID, false);

    SOC_PERF_ID = 10071;
    socPerfClient->ApplySocPerfConfigById(SOC_PERF_ID);
    socPerfClient->ApplySocPerfConfigByIdEx(SOC_PERF_ID, true);
    socPerfClient->ApplySocPerfConfigByIdEx(SOC_PERF_ID, false);

    SOC_PERF_ID = 10073;
    socPerfClient->ApplySocPerfConfigById(SOC_PERF_ID);
    socPerfClient->ApplySocPerfConfigByIdEx(SOC_PERF_ID, true);
    socPerfClient->ApplySocPerfConfigByIdEx(SOC_PERF_ID, false);

    SOC_PERF_ID = 10097; 
    socPerfClient->ApplySocPerfConfigById(SOC_PERF_ID);
    socPerfClient->ApplySocPerfConfigByIdEx(SOC_PERF_ID, true);
    socPerfClient->ApplySocPerfConfigByIdEx(SOC_PERF_ID, false);
}

/**
 * @tc.name: SocPerfAdapterTest_002.
 * @tc.desc: test apply socperf ConfigById
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SocPerfAdapterTest, SocPerfAdapterTest_002, TestSize.Level1)
{
    auto  socPerfClient = std::make_shared<SocPerfClientAdapterImpl>();
    EXPECT_NE(socPerfClient, nullptr);
    socPerfClient->ApplySocPerfConfigById(SOC_PERF_ID);
    socPerfClient->ApplySocPerfConfigById(0);
}

/**
 * @tc.name: SocPerfAdapterTest_003.
 * @tc.desc: test apply socperf ConfigByIdEx
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SocPerfAdapterTest, SocPerfAdapterTest_003, TestSize.Level1)
{
    auto  socPerfClient = std::make_shared<SocPerfClientAdapterImpl>();
    EXPECT_NE(socPerfClient, nullptr);
    socPerfClient->ApplySocPerfConfigByIdEx(SOC_PERF_ID, true);
    socPerfClient->ApplySocPerfConfigByIdEx(SOC_PERF_ID, false);
    socPerfClient->ApplySocPerfConfigByIdEx(0, true);
    socPerfClient->ApplySocPerfConfigByIdEx(0, false);
}
} // namespace NWeb
