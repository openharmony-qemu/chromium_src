/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "base/task/single_thread_task_runner.h"
#include "base/test/task_environment.h"
#define private public
#define protected public
#include "services/device/time_zone_monitor/time_zone_monitor.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#undef private
#undef protected
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

using namespace testing;
using namespace OHOS::NWeb;

namespace device {

#if BUILDFLAG(ARKWEB_TEST)
class MockTimezoneEventCallbackAdapter 
    : public OHOS::NWeb::TimezoneEventCallbackAdapter {
public:
    MockTimezoneEventCallbackAdapter() = default;
    ~MockTimezoneEventCallbackAdapter() override = default;

    MOCK_METHOD(void, TimezoneChanged, 
                (std::shared_ptr<WebTimezoneInfo> info), 
                (override));
};

class MockWebTimezoneInfo : public OHOS::NWeb::WebTimezoneInfo {
public:
    MockWebTimezoneInfo() = default;
    ~MockWebTimezoneInfo() override = default;

    MOCK_METHOD(std::string, GetTzId, (), (override));
};

class MockDateTimeFormatAdapter 
    : public OHOS::NWeb::DateTimeFormatAdapter {
public:
    MockDateTimeFormatAdapter() = default;
    ~MockDateTimeFormatAdapter() override = default;

    MOCK_METHOD(void, RegTimezoneEvent, 
                (const std::shared_ptr<TimezoneEventCallbackAdapter> eventCallback), 
                (override));
    MOCK_METHOD(bool, StartListen, (), (override));
    MOCK_METHOD(void, StopListen, (), (override));
    MOCK_METHOD(std::string, GetTimezone, (), (override));
};

class TimeZoneMonitorOhosTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}

    base::test::SingleThreadTaskEnvironment task_environment_;
};

TEST_F(TimeZoneMonitorOhosTest, GetTimezone_OhosAdapterCall) {
    MockDateTimeFormatAdapter mock_adapter;
    
    EXPECT_CALL(mock_adapter, GetTimezone())
        .WillOnce(Return("Asia/Shanghai"));

    std::string timezone = mock_adapter.GetTimezone();
    EXPECT_EQ(timezone, "Asia/Shanghai");
}

TEST_F(TimeZoneMonitorOhosTest, RegTimezoneEvent_WithValidCallback) {
    MockDateTimeFormatAdapter mock_adapter;
    auto callback = std::make_shared<MockTimezoneEventCallbackAdapter>();
    
    EXPECT_CALL(mock_adapter, RegTimezoneEvent(_)).Times(1);
    mock_adapter.RegTimezoneEvent(callback);
}

TEST_F(TimeZoneMonitorOhosTest, TimezoneEventCallback_TimezoneChanged) {
    auto callback = std::make_shared<MockTimezoneEventCallbackAdapter>();
    auto info = std::make_shared<MockWebTimezoneInfo>();
    
    EXPECT_CALL(*info, GetTzId()).WillOnce(Return("Pacific/Auckland"));
    EXPECT_CALL(*callback, TimezoneChanged(_)).Times(1);
    
    callback->TimezoneChanged(info);
}

TEST_F(TimeZoneMonitorOhosTest, TimezoneEventCallback_WithNullInfo) {
    auto callback = std::make_shared<MockTimezoneEventCallbackAdapter>();
    std::shared_ptr<WebTimezoneInfo> null_info = nullptr;
    
    EXPECT_CALL(*callback, TimezoneChanged(_)).Times(1);
    callback->TimezoneChanged(null_info);
}

TEST_F(TimeZoneMonitorOhosTest, StartListen_ReturnsTrue) {
    MockDateTimeFormatAdapter mock_adapter;
    
    EXPECT_CALL(mock_adapter, StartListen()).WillOnce(Return(true));

    bool result = mock_adapter.StartListen();
    EXPECT_TRUE(result);
}

TEST_F(TimeZoneMonitorOhosTest, StartListen_ReturnsFalse) {
    MockDateTimeFormatAdapter mock_adapter;
    
    EXPECT_CALL(mock_adapter, StartListen()).WillOnce(Return(false));

    bool result = mock_adapter.StartListen();
    EXPECT_FALSE(result);
}

TEST_F(TimeZoneMonitorOhosTest, StopListen_CallsSuccessfully) {
    MockDateTimeFormatAdapter mock_adapter;
    
    EXPECT_CALL(mock_adapter, StopListen()).Times(1);
    mock_adapter.StopListen();
}

TEST_F(TimeZoneMonitorOhosTest, MultipleRegTimezoneEvent_Success) {
    MockDateTimeFormatAdapter mock_adapter;
    auto callback1 = std::make_shared<MockTimezoneEventCallbackAdapter>();
    auto callback2 = std::make_shared<MockTimezoneEventCallbackAdapter>();

    EXPECT_CALL(mock_adapter, RegTimezoneEvent(_)).Times(1);
    EXPECT_CALL(mock_adapter, RegTimezoneEvent(_)).Times(1);

    mock_adapter.RegTimezoneEvent(callback1);
    mock_adapter.RegTimezoneEvent(callback2);
}

TEST_F(TimeZoneMonitorOhosTest, StartStopMultipleTimes_Success) {
    MockDateTimeFormatAdapter mock_adapter;
    
    EXPECT_CALL(mock_adapter, StartListen())
        .WillOnce(Return(true))
        .WillOnce(Return(false))
        .WillOnce(Return(true));
    EXPECT_CALL(mock_adapter, StopListen()).Times(2);

    EXPECT_TRUE(mock_adapter.StartListen());
    mock_adapter.StopListen();
    EXPECT_FALSE(mock_adapter.StartListen());
    mock_adapter.StopListen();
    EXPECT_TRUE(mock_adapter.StartListen());
}

#endif

}  // namespace device
