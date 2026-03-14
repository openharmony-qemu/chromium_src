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
    delete instance;
    OhosAdapterHelper::SetInstance(nullptr);
}

TEST_F(HiAppeventAdapterImplTest, ReportDynamicStasticWithEventTypeFault) {
    MockOhosAdapterHelper* instance = new MockOhosAdapterHelper();
    OhosAdapterHelper::SetInstance(instance);
    MockHiappevetAdapter mockAdapter;
    EXPECT_CALL(*instance, GetHiAppeventAdapterInstance())
        .WillOnce(testing::ReturnRef(mockAdapter));
    EXPECT_CALL(mockAdapter, ReportDynamicStastic(_, _, _, _))
        .WillOnce(testing::Return());
    std::string domain = "OS";
    std::string event_name = "SCROLL_ARKWEB_FLING_JANK";
    int32_t event_type = 1;
    OHOS::NWeb::HiAppeventAdapter::DynamicFrameDropInfo dynamicFrameDropInfo;
    dynamicFrameDropInfo.start_time = 1000;
    dynamicFrameDropInfo.duration = 5000;
    dynamicFrameDropInfo.max_app_frame_time = 100;
    dynamicFrameDropInfo.web_id = 1;
    OhosAdapterHelper::GetInstance().GetHiAppeventAdapterInstance()
        .ReportDynamicStastic(domain, event_name, event_type, dynamicFrameDropInfo);
    delete instance;
    OhosAdapterHelper::SetInstance(nullptr);
}

TEST_F(HiAppeventAdapterImplTest, ReportDynamicStasticWithEventTypeStatistic) {
    MockOhosAdapterHelper* instance = new MockOhosAdapterHelper();
    OhosAdapterHelper::SetInstance(instance);
    MockHiappevetAdapter mockAdapter;
    EXPECT_CALL(*instance, GetHiAppeventAdapterInstance())
        .WillOnce(testing::ReturnRef(mockAdapter));
    EXPECT_CALL(mockAdapter, ReportDynamicStastic(_, _, _, _))
        .WillOnce(testing::Return());
    std::string domain = "WEBVIEW";
    std::string event_name = "FRAME_DROP_EVENT";
    int32_t event_type = 2;
    OHOS::NWeb::HiAppeventAdapter::DynamicFrameDropInfo dynamicFrameDropInfo;
    dynamicFrameDropInfo.start_time = 2000;
    dynamicFrameDropInfo.duration = 3000;
    dynamicFrameDropInfo.max_app_frame_time = 80;
    dynamicFrameDropInfo.web_id = 2;
    OhosAdapterHelper::GetInstance().GetHiAppeventAdapterInstance()
        .ReportDynamicStastic(domain, event_name, event_type, dynamicFrameDropInfo);
    delete instance;
    OhosAdapterHelper::SetInstance(nullptr);
}

TEST_F(HiAppeventAdapterImplTest, ReportDynamicStasticWithEventTypeSecurity) {
    MockOhosAdapterHelper* instance = new MockOhosAdapterHelper();
    OhosAdapterHelper::SetInstance(instance);
    MockHiappevetAdapter mockAdapter;
    EXPECT_CALL(*instance, GetHiAppeventAdapterInstance())
        .WillOnce(testing::ReturnRef(mockAdapter));
    EXPECT_CALL(mockAdapter, ReportDynamicStastic(_, _, _, _))
        .WillOnce(testing::Return());
    std::string domain = "SECURITY_DOMAIN";
    std::string event_name = "SECURITY_EVENT";
    int32_t event_type = 3;
    OHOS::NWeb::HiAppeventAdapter::DynamicFrameDropInfo dynamicFrameDropInfo;
    dynamicFrameDropInfo.start_time = 3000;
    dynamicFrameDropInfo.duration = 2000;
    dynamicFrameDropInfo.max_app_frame_time = 60;
    dynamicFrameDropInfo.web_id = 3;
    OhosAdapterHelper::GetInstance().GetHiAppeventAdapterInstance()
        .ReportDynamicStastic(domain, event_name, event_type, dynamicFrameDropInfo);
    delete instance;
    OhosAdapterHelper::SetInstance(nullptr);
}

TEST_F(HiAppeventAdapterImplTest, ReportDynamicStasticWithEventTypeBehavior) {
    MockOhosAdapterHelper* instance = new MockOhosAdapterHelper();
    OhosAdapterHelper::SetInstance(instance);
    MockHiappevetAdapter mockAdapter;
    EXPECT_CALL(*instance, GetHiAppeventAdapterInstance())
        .WillOnce(testing::ReturnRef(mockAdapter));
    EXPECT_CALL(mockAdapter, ReportDynamicStastic(_, _, _, _))
        .WillOnce(testing::Return());
    std::string domain = "BEHAVIOR_DOMAIN";
    std::string event_name = "USER_BEHAVIOR_EVENT";
    int32_t event_type = 4;
    OHOS::NWeb::HiAppeventAdapter::DynamicFrameDropInfo dynamicFrameDropInfo;
    dynamicFrameDropInfo.start_time = 4000;
    dynamicFrameDropInfo.duration = 1000;
    dynamicFrameDropInfo.max_app_frame_time = 40;
    dynamicFrameDropInfo.web_id = 4;
    OhosAdapterHelper::GetInstance().GetHiAppeventAdapterInstance()
        .ReportDynamicStastic(domain, event_name, event_type, dynamicFrameDropInfo);
    delete instance;
    OhosAdapterHelper::SetInstance(nullptr);
}

TEST_F(HiAppeventAdapterImplTest, ReportDynamicStasticWithEmptyDomain) {
    MockOhosAdapterHelper* instance = new MockOhosAdapterHelper();
    OhosAdapterHelper::SetInstance(instance);
    MockHiappevetAdapter mockAdapter;
    EXPECT_CALL(*instance, GetHiAppeventAdapterInstance())
        .WillOnce(testing::ReturnRef(mockAdapter));
    EXPECT_CALL(mockAdapter, ReportDynamicStastic(_, _, _, _))
        .WillOnce(testing::Return());
    std::string domain = "";
    std::string event_name = "testEvent";
    int32_t event_type = 1;
    OHOS::NWeb::HiAppeventAdapter::DynamicFrameDropInfo dynamicFrameDropInfo;
    OhosAdapterHelper::GetInstance().GetHiAppeventAdapterInstance()
        .ReportDynamicStastic(domain, event_name, event_type, dynamicFrameDropInfo);
    delete instance;
    OhosAdapterHelper::SetInstance(nullptr);
}

TEST_F(HiAppeventAdapterImplTest, ReportDynamicStasticWithEmptyEventName) {
    MockOhosAdapterHelper* instance = new MockOhosAdapterHelper();
    OhosAdapterHelper::SetInstance(instance);
    MockHiappevetAdapter mockAdapter;
    EXPECT_CALL(*instance, GetHiAppeventAdapterInstance())
        .WillOnce(testing::ReturnRef(mockAdapter));
    EXPECT_CALL(mockAdapter, ReportDynamicStastic(_, _, _, _))
        .WillOnce(testing::Return());
    std::string domain = "testDomain";
    std::string event_name = "";
    int32_t event_type = 1;
    OHOS::NWeb::HiAppeventAdapter::DynamicFrameDropInfo dynamicFrameDropInfo;
    OhosAdapterHelper::GetInstance().GetHiAppeventAdapterInstance()
        .ReportDynamicStastic(domain, event_name, event_type, dynamicFrameDropInfo);
    delete instance;
    OhosAdapterHelper::SetInstance(nullptr);
}

TEST_F(HiAppeventAdapterImplTest, ReportDynamicStasticWithZeroValues) {
    MockOhosAdapterHelper* instance = new MockOhosAdapterHelper();
    OhosAdapterHelper::SetInstance(instance);
    MockHiappevetAdapter mockAdapter;
    EXPECT_CALL(*instance, GetHiAppeventAdapterInstance())
        .WillOnce(testing::ReturnRef(mockAdapter));
    EXPECT_CALL(mockAdapter, ReportDynamicStastic(_, _, _, _))
        .WillOnce(testing::Return());
    std::string domain = "testDomain";
    std::string event_name = "testEvent";
    int32_t event_type = 0;
    OHOS::NWeb::HiAppeventAdapter::DynamicFrameDropInfo dynamicFrameDropInfo;
    dynamicFrameDropInfo.start_time = 0;
    dynamicFrameDropInfo.duration = 0;
    dynamicFrameDropInfo.max_app_frame_time = 0;
    dynamicFrameDropInfo.web_id = 0;
    OhosAdapterHelper::GetInstance().GetHiAppeventAdapterInstance()
        .ReportDynamicStastic(domain, event_name, event_type, dynamicFrameDropInfo);
    delete instance;
    OhosAdapterHelper::SetInstance(nullptr);
}

TEST_F(HiAppeventAdapterImplTest, ReportDynamicStasticWithNegativeValues) {
    MockOhosAdapterHelper* instance = new MockOhosAdapterHelper();
    OhosAdapterHelper::SetInstance(instance);
    MockHiappevetAdapter mockAdapter;
    EXPECT_CALL(*instance, GetHiAppeventAdapterInstance())
        .WillOnce(testing::ReturnRef(mockAdapter));
    EXPECT_CALL(mockAdapter, ReportDynamicStastic(_, _, _, _))
        .WillOnce(testing::Return());
    std::string domain = "testDomain";
    std::string event_name = "testEvent";
    int32_t event_type = -1;
    OHOS::NWeb::HiAppeventAdapter::DynamicFrameDropInfo dynamicFrameDropInfo;
    dynamicFrameDropInfo.start_time = -1000;
    dynamicFrameDropInfo.duration = -500;
    dynamicFrameDropInfo.max_app_frame_time = -100;
    dynamicFrameDropInfo.web_id = -1;
    OhosAdapterHelper::GetInstance().GetHiAppeventAdapterInstance()
        .ReportDynamicStastic(domain, event_name, event_type, dynamicFrameDropInfo);
    delete instance;
    OhosAdapterHelper::SetInstance(nullptr);
}

TEST_F(HiAppeventAdapterImplTest, ReportDynamicStasticWithLargeValues) {
    MockOhosAdapterHelper* instance = new MockOhosAdapterHelper();
    OhosAdapterHelper::SetInstance(instance);
    MockHiappevetAdapter mockAdapter;
    EXPECT_CALL(*instance, GetHiAppeventAdapterInstance())
        .WillOnce(testing::ReturnRef(mockAdapter));
    EXPECT_CALL(mockAdapter, ReportDynamicStastic(_, _, _, _))
        .WillOnce(testing::Return());
    std::string domain = "testDomain";
    std::string event_name = "testEvent";
    int32_t event_type = 2;
    OHOS::NWeb::HiAppeventAdapter::DynamicFrameDropInfo dynamicFrameDropInfo;
    dynamicFrameDropInfo.start_time = 9223372036854775807LL;
    dynamicFrameDropInfo.duration = 9223372036854775807LL;
    dynamicFrameDropInfo.max_app_frame_time = 9223372036854775807LL;
    dynamicFrameDropInfo.web_id = 2147483647;
    OhosAdapterHelper::GetInstance().GetHiAppeventAdapterInstance()
        .ReportDynamicStastic(domain, event_name, event_type, dynamicFrameDropInfo);
    delete instance;
    OhosAdapterHelper::SetInstance(nullptr);
}

TEST_F(HiAppeventAdapterImplTest, ReportDynamicStasticWithLongDomainName) {
    MockOhosAdapterHelper* instance = new MockOhosAdapterHelper();
    OhosAdapterHelper::SetInstance(instance);
    MockHiappevetAdapter mockAdapter;
    EXPECT_CALL(*instance, GetHiAppeventAdapterInstance())
        .WillOnce(testing::ReturnRef(mockAdapter));
    EXPECT_CALL(mockAdapter, ReportDynamicStastic(_, _, _, _))
        .WillOnce(testing::Return());
    std::string domain(256, 'a');
    std::string event_name = "testEvent";
    int32_t event_type = 1;
    OHOS::NWeb::HiAppeventAdapter::DynamicFrameDropInfo dynamicFrameDropInfo;
    OhosAdapterHelper::GetInstance().GetHiAppeventAdapterInstance()
        .ReportDynamicStastic(domain, event_name, event_type, dynamicFrameDropInfo);
    delete instance;
    OhosAdapterHelper::SetInstance(nullptr);
}

TEST_F(HiAppeventAdapterImplTest, ReportDynamicStasticWithLongEventName) {
    MockOhosAdapterHelper* instance = new MockOhosAdapterHelper();
    OhosAdapterHelper::SetInstance(instance);
    MockHiappevetAdapter mockAdapter;
    EXPECT_CALL(*instance, GetHiAppeventAdapterInstance())
        .WillOnce(testing::ReturnRef(mockAdapter));
    EXPECT_CALL(mockAdapter, ReportDynamicStastic(_, _, _, _))
        .WillOnce(testing::Return());
    std::string domain = "testDomain";
    std::string event_name(256, 'e');
    int32_t event_type = 1;
    OHOS::NWeb::HiAppeventAdapter::DynamicFrameDropInfo dynamicFrameDropInfo;
    OhosAdapterHelper::GetInstance().GetHiAppeventAdapterInstance()
        .ReportDynamicStastic(domain, event_name, event_type, dynamicFrameDropInfo);
    delete instance;
    OhosAdapterHelper::SetInstance(nullptr);
}

TEST_F(HiAppeventAdapterImplTest, ReportDynamicStasticWithSpecialChars) {
    MockOhosAdapterHelper* instance = new MockOhosAdapterHelper();
    OhosAdapterHelper::SetInstance(instance);
    MockHiappevetAdapter mockAdapter;
    EXPECT_CALL(*instance, GetHiAppeventAdapterInstance())
        .WillOnce(testing::ReturnRef(mockAdapter));
    EXPECT_CALL(mockAdapter, ReportDynamicStastic(_, _, _, _))
        .WillOnce(testing::Return());
    std::string domain = "test_domain-123";
    std::string event_name = "test_event@456";
    int32_t event_type = 2;
    OHOS::NWeb::HiAppeventAdapter::DynamicFrameDropInfo dynamicFrameDropInfo;
    dynamicFrameDropInfo.start_time = 1000;
    dynamicFrameDropInfo.duration = 500;
    dynamicFrameDropInfo.max_app_frame_time = 100;
    dynamicFrameDropInfo.web_id = 1;
    OhosAdapterHelper::GetInstance().GetHiAppeventAdapterInstance()
        .ReportDynamicStastic(domain, event_name, event_type, dynamicFrameDropInfo);
    delete instance;
    OhosAdapterHelper::SetInstance(nullptr);
}

TEST_F(HiAppeventAdapterImplTest, ReportDynamicStasticMultipleTimes) {
    MockOhosAdapterHelper* instance = new MockOhosAdapterHelper();
    OhosAdapterHelper::SetInstance(instance);
    MockHiappevetAdapter mockAdapter;
    EXPECT_CALL(*instance, GetHiAppeventAdapterInstance())
        .Times(3)
        .WillRepeatedly(testing::ReturnRef(mockAdapter));
    EXPECT_CALL(mockAdapter, ReportDynamicStastic(_, _, _, _))
        .Times(3)
        .WillRepeatedly(testing::Return());
    std::string domain = "testDomain";
    std::string event_name = "testEvent";
    int32_t event_type = 1;
    OHOS::NWeb::HiAppeventAdapter::DynamicFrameDropInfo dynamicFrameDropInfo;
    for (int i = 0; i < 3; i++) {
        dynamicFrameDropInfo.web_id = i;
        OhosAdapterHelper::GetInstance().GetHiAppeventAdapterInstance()
            .ReportDynamicStastic(domain, event_name, event_type, dynamicFrameDropInfo);
    }
    delete instance;
    OhosAdapterHelper::SetInstance(nullptr);
}

TEST_F(HiAppeventAdapterImplTest, DynamicFrameDropInfoStructureTest) {
    OHOS::NWeb::HiAppeventAdapter::DynamicFrameDropInfo info;
    info.start_time = 1234567890LL;
    info.duration = 5000LL;
    info.max_app_frame_time = 100LL;
    info.web_id = 42;
    EXPECT_EQ(info.start_time, 1234567890LL);
    EXPECT_EQ(info.duration, 5000LL);
    EXPECT_EQ(info.max_app_frame_time, 100LL);
    EXPECT_EQ(info.web_id, 42);
}

TEST_F(HiAppeventAdapterImplTest, GetInstanceMultipleTimes) {
    HiAppeventAdapterImpl& instance1 = HiAppeventAdapterImpl::GetInstance();
    HiAppeventAdapterImpl& instance2 = HiAppeventAdapterImpl::GetInstance();
    HiAppeventAdapterImpl& instance3 = HiAppeventAdapterImpl::GetInstance();
    EXPECT_EQ(&instance1, &instance2);
    EXPECT_EQ(&instance2, &instance3);
}

TEST_F(HiAppeventAdapterImplTest, ReportDynamicStasticWithDifferentEventTypes) {
    MockOhosAdapterHelper* instance = new MockOhosAdapterHelper();
    OhosAdapterHelper::SetInstance(instance);
    MockHiappevetAdapter mockAdapter;
    EXPECT_CALL(*instance, GetHiAppeventAdapterInstance())
        .Times(4)
        .WillRepeatedly(testing::ReturnRef(mockAdapter));
    EXPECT_CALL(mockAdapter, ReportDynamicStastic(_, _, _, _))
        .Times(4)
        .WillRepeatedly(testing::Return());
    std::string domain = "testDomain";
    std::string event_name = "testEvent";
    OHOS::NWeb::HiAppeventAdapter::DynamicFrameDropInfo dynamicFrameDropInfo;
    for (int32_t event_type = 1; event_type <= 4; event_type++) {
        OhosAdapterHelper::GetInstance().GetHiAppeventAdapterInstance()
            .ReportDynamicStastic(domain, event_name, event_type, dynamicFrameDropInfo);
    }
    delete instance;
    OhosAdapterHelper::SetInstance(nullptr);
}

TEST_F(HiAppeventAdapterImplTest, ReportDynamicStasticWithChineseChars) {
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
    delete instance;
    OhosAdapterHelper::SetInstance(nullptr);
}

TEST_F(HiAppeventAdapterImplTest, ReportDynamicStasticWithUnicodeChars) {
    MockOhosAdapterHelper* instance = new MockOhosAdapterHelper();
    OhosAdapterHelper::SetInstance(instance);
    MockHiappevetAdapter mockAdapter;
    EXPECT_CALL(*instance, GetHiAppeventAdapterInstance())
        .WillOnce(testing::ReturnRef(mockAdapter));
    EXPECT_CALL(mockAdapter, ReportDynamicStastic(_, _, _, _))
        .WillOnce(testing::Return());
    std::string domain = "domain_123";
    std::string event_name = "event_456";
    int32_t event_type = 2;
    OHOS::NWeb::HiAppeventAdapter::DynamicFrameDropInfo dynamicFrameDropInfo;
    OhosAdapterHelper::GetInstance().GetHiAppeventAdapterInstance()
        .ReportDynamicStastic(domain, event_name, event_type, dynamicFrameDropInfo);
    delete instance;
    OhosAdapterHelper::SetInstance(nullptr);
}