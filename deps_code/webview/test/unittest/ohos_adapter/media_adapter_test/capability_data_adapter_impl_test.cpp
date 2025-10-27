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

#include "gtest/gtest.h"
#define private public
#include "capability_data_adapter_impl.h"
#undef private

using namespace testing;
using namespace testing::ext;
using namespace OHOS::NWeb;

class CapabilityDataAdapterImplTest : public ::testing::Test {
    protected:
        void SetUp() override {}
        void TearDown() override {}
};

/**
 * @tc.name: GetMaxWidth_ShouldReturnMaxWidth_WhenCalled
 * @tc.number: CapabilityDataAdapterImplTest_001
 * @tc.desc: Test GetMaxWidth method of CapabilityDataAdapterImpl class
 */
HWTEST_F(CapabilityDataAdapterImplTest,
         GetMaxWidth_ShouldReturnMaxWidth_WhenCalled, TestSize.Level0)
{
    CapabilityDataAdapterImpl capabilityDataAdapterImpl;
    int64_t maxWidth = 1920;
    capabilityDataAdapterImpl.SetMaxWidth(maxWidth);
    EXPECT_EQ(capabilityDataAdapterImpl.GetMaxWidth(), maxWidth);
}

/**
 * @tc.name: GetMaxHeight_ShouldReturnMaxHeight_WhenCalled
 * @tc.number: CapabilityDataAdapterImplTest_002
 * @tc.desc: Test GetMaxHeight method of CapabilityDataAdapterImpl class
 */
HWTEST_F(CapabilityDataAdapterImplTest,
         GetMaxHeight_ShouldReturnMaxHeight_WhenCalled, TestSize.Level0)
{
    CapabilityDataAdapterImpl capabilityDataAdapterImpl;
    int64_t maxHeight = 1080;
    capabilityDataAdapterImpl.SetMaxHeight(maxHeight);
    EXPECT_EQ(capabilityDataAdapterImpl.GetMaxHeight(), maxHeight);
}

/**
 * @tc.name: GetMaxframeRate_ShouldReturnMaxframeRate_WhenCalled
 * @tc.number: CapabilityDataAdapterImplTest_003
 * @tc.desc: Test GetMaxframeRate method of CapabilityDataAdapterImpl class
 */
HWTEST_F(CapabilityDataAdapterImplTest,
         GetMaxframeRate_ShouldReturnMaxframeRate_WhenCalled, TestSize.Level0)
{
    CapabilityDataAdapterImpl capabilityDataAdapterImpl;
    int32_t maxFrameRate = 60;
    capabilityDataAdapterImpl.SetMaxframeRate(maxFrameRate);
    EXPECT_EQ(capabilityDataAdapterImpl.GetMaxframeRate(), maxFrameRate);
}