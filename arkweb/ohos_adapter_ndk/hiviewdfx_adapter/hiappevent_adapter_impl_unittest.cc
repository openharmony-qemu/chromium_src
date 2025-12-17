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
 
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "hiappevent_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/interfaces/mock/mock_ohos_adapter_helper.h"
#include "arkweb/ohos_adapter_ndk/interfaces/ohos_adapter_helper.h"

using namespace testing;
using namespace OHOS::NWeb;

class HiAppeventAdapterImplTest : public ::testing::Test {};

class MockHiappevetAdapter : public HiAppeventAdapter {
public:
    MOCK_METHOD(void, ReportDynamicStastic,
        (const std::string& domain, const std::string& eventName, int32_t eventType,
        const HiAppeventAdapter::DynamicFrameDropInfo& dynamicFrameDropInfo), (override));
};

TEST_F(HiAppeventAdapterImplTest, GetInstanceTest) {
    HiAppeventAdapterImpl& instance1 = HiAppeventAdapterImpl::GetInstance();
    HiAppeventAdapterImpl& instance2 = HiAppeventAdapterImpl::GetInstance();
    EXPECT_EQ(&instance1, &instance2);
}

TEST_F(HiAppeventAdapterImplTest, ReportDynamicStasticTest) {
    MockOhosAdapterHelper* instance = new MockOhosAdapterHelper();
    OhosAdapterHelper::SetInstance(instance);
    MockHiappevetAdapter mockAdapter;
    EXPECT_CALL(*instance, GetHiAppeventAdapterInstance())
        .WillOnce(testing::ReturnRef(mockAdapter));
    EXPECT_CALL(mockAdapter, ReportDynamicStastic(_, _, _, _))
        .WillOnce(testing::Return());
    std::string domain = "testDomain";
    std::string event_name = "testEvent";
    int32_t event_type = 1;
    OHOS::NWeb::HiAppeventAdapter::DynamicFrameDropInfo dynamicFrameDropInfo;
    OhosAdapterHelper::GetInstance().GetHiAppeventAdapterInstance()
        .ReportDynamicStastic(domain, event_name, event_type, dynamicFrameDropInfo);
    OhosAdapterHelper::SetInstance(nullptr);
}