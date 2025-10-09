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
#include <sys/mman.h>
#include <BasicServicesKit/oh_commonevent_support.h>
#include <BasicServicesKit/oh_commonevent.h>
#include <securec.h>

#define private public
#include "arkweb/ohos_adapter_ndk/date_time_format_adapter/date_time_format_adapter_impl.h"
#undef private
#include "arkweb/ohos_adapter_ndk/date_time_format_adapter/date_time_format_adapter_impl.cpp"
#include "arkweb/ohos_adapter_ndk/mock_ndk_api/include/mock_ndk_api.h"

#include "base/logging.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

using namespace testing;
using namespace MockNdkApi;

struct CArrParameters {
  int32_t netType;
};

typedef struct CommonEvent_RcvData {
  std::string event;
  std::string bundleName;
  int32_t code;
  std::string data;
  CArrParameters* parameters = nullptr;
} CommonEvent_RcvData;

namespace OHOS::NWeb {
class DateTimeFormatAdapterImplTest : public testing::Test {
public:
    DateTimeFormatAdapterImplTest() {}
    ~DateTimeFormatAdapterImplTest() = default;

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DateTimeFormatAdapterImplTest::SetUpTestCase() {}

void DateTimeFormatAdapterImplTest::TearDownTestCase() {}

void DateTimeFormatAdapterImplTest::SetUp() {}

void DateTimeFormatAdapterImplTest::TearDown() {}

class MockTimezoneEventCallbackAdapter : public TimezoneEventCallbackAdapter {
public:
    MockTimezoneEventCallbackAdapter() = default;
    void TimezoneChanged(std::shared_ptr<WebTimezoneInfo> info) {}
};

TEST_F(DateTimeFormatAdapterImplTest, DateTimeFormatAdapterImplTest_001)
{
    auto adapter = std::make_shared<DateTimeFormatAdapterImpl>();
    EXPECT_NE(adapter, nullptr);
    adapter->RegTimezoneEvent(nullptr);
    std::shared_ptr<TimezoneEventCallbackAdapter> cb = std::make_shared<MockTimezoneEventCallbackAdapter>();
    adapter->RegTimezoneEvent(std::move(cb));
    bool result = adapter->StartListen();
    EXPECT_TRUE(result);
    adapter->StopListen();
    auto timeStr = adapter->GetTimezone();
    EXPECT_FALSE(timeStr.empty());
}

TEST_F(DateTimeFormatAdapterImplTest, DateTimeFormatReceiveCallback01) {
  ASSERT_NO_FATAL_FAILURE(DateTimeFormatReceiveCallback(nullptr));

  CommonEvent_RcvData data;
  g_mock_OH_CommonEvent_GetEventFromRcvData = [](const CommonEvent_RcvData* rcvData) {
    return COMMON_EVENT_PACKAGE_ADDED;
  };
  ASSERT_NO_FATAL_FAILURE(DateTimeFormatReceiveCallback(&data));

  g_mock_OH_CommonEvent_GetEventFromRcvData = [](const CommonEvent_RcvData* rcvData) {
    return COMMON_EVENT_TIMEZONE_CHANGED;
  };
  g_mock_OH_TimeService_GetTimeZone = [](char *timeZone, uint32_t len) {
    return TIMESERVICE_ERR_INTERNAL_ERROR;
  };
  ASSERT_NO_FATAL_FAILURE(DateTimeFormatReceiveCallback(&data));

  g_mock_OH_TimeService_GetTimeZone = [](char *timeZone, uint32_t len) {
    return TIMESERVICE_ERR_OK;
  };
  ASSERT_NO_FATAL_FAILURE(DateTimeFormatReceiveCallback(&data));
  g_mock_OH_CommonEvent_GetEventFromRcvData = nullptr;
  g_mock_OH_TimeService_GetTimeZone = nullptr;
}

TEST_F(DateTimeFormatAdapterImplTest, DateTimeFormatAdapterImplTest_002) {
  DateTimeFormatAdapterImpl dateTimeForAdapter;
  bool ret = dateTimeForAdapter.StartListen();
  EXPECT_TRUE(ret);
  EXPECT_NE(dateTimeForAdapter.commonEventSubscriberInfo_, nullptr);
  EXPECT_NE(dateTimeForAdapter.commonEventSubscriber_, nullptr);
  ret = dateTimeForAdapter.StartListen();
  EXPECT_TRUE(ret);
  dateTimeForAdapter.StopListen();
  EXPECT_EQ(dateTimeForAdapter.commonEventSubscriberInfo_, nullptr);
  EXPECT_EQ(dateTimeForAdapter.commonEventSubscriber_, nullptr);
}

TEST_F(DateTimeFormatAdapterImplTest, DateTimeFormatAdapterImplTest_003) {
  DateTimeFormatAdapterImpl dateTimeForAdapter;
  bool ret = dateTimeForAdapter.StartListen();
  EXPECT_TRUE(ret);
  EXPECT_NE(dateTimeForAdapter.commonEventSubscriberInfo_, nullptr);
  EXPECT_NE(dateTimeForAdapter.commonEventSubscriber_, nullptr);
  std::shared_ptr<TimezoneEventCallbackAdapter> cb = std::make_shared<MockTimezoneEventCallbackAdapter>();
  EXPECT_NE(cb.get(), nullptr);
  dateTimeForAdapter.cbSet_.insert(cb);
  dateTimeForAdapter.StopListen();
  EXPECT_NE(dateTimeForAdapter.commonEventSubscriberInfo_, nullptr);
  EXPECT_NE(dateTimeForAdapter.commonEventSubscriber_, nullptr);
  dateTimeForAdapter.cbSet_.erase(cb);
  dateTimeForAdapter.cbSet_.insert(dateTimeForAdapter.cb_);
  dateTimeForAdapter.StopListen();
  EXPECT_EQ(dateTimeForAdapter.commonEventSubscriberInfo_, nullptr);
  EXPECT_EQ(dateTimeForAdapter.commonEventSubscriber_, nullptr);
}

TEST_F(DateTimeFormatAdapterImplTest, DateTimeFormatAdapterImplTest_004) {
  DateTimeFormatAdapterImpl dateTimeForAdapter;
  bool ret = dateTimeForAdapter.StartListen();
  EXPECT_TRUE(ret);
  EXPECT_NE(dateTimeForAdapter.commonEventSubscriberInfo_, nullptr);
  EXPECT_NE(dateTimeForAdapter.commonEventSubscriber_, nullptr);
  dateTimeForAdapter.StopListen();
  dateTimeForAdapter.cbSet_.insert(dateTimeForAdapter.cb_);
  dateTimeForAdapter.StopListen();
  EXPECT_EQ(dateTimeForAdapter.commonEventSubscriberInfo_, nullptr);
  EXPECT_EQ(dateTimeForAdapter.commonEventSubscriber_, nullptr);
}

TEST_F(DateTimeFormatAdapterImplTest, StartListen_001) {
  DateTimeFormatAdapterImpl dateTimeForAdapter;
  g_mock_OH_CommonEvent_CreateSubscribeInfo = [](const char* events[], int32_t eventsNum) {
    return nullptr;
  };
  g_mock_OH_CommonEvent_CreateSubscriber = [](const CommonEvent_SubscribeInfo* info,
		CommonEvent_ReceiveCallback callback) {
    return nullptr;
  };
  g_mock_OH_CommonEvent_Subscribe = [](const CommonEvent_Subscriber* subscriber) {
    return COMMONEVENT_ERR_PERMISSION_ERROR;
  };
  g_mock_OH_CommonEvent_UnSubscribe = [](const CommonEvent_Subscriber* subscriber) {
    return COMMONEVENT_ERR_OK;
  };
  g_mock_OH_CommonEvent_DestroySubscribeInfo = [](CommonEvent_SubscribeInfo* info) {};
  g_mock_OH_CommonEvent_DestroySubscriber = [](CommonEvent_Subscriber* subscriber) {};
  bool ret = dateTimeForAdapter.StartListen();
  EXPECT_FALSE(ret);

  g_mock_OH_CommonEvent_CreateSubscribeInfo = nullptr;
  g_mock_OH_CommonEvent_DestroySubscribeInfo = nullptr;
  ret = dateTimeForAdapter.StartListen();
  EXPECT_FALSE(ret);

  g_mock_OH_CommonEvent_CreateSubscriber = nullptr;
  g_mock_OH_CommonEvent_DestroySubscriber = nullptr;
  ret = dateTimeForAdapter.StartListen();
  EXPECT_FALSE(ret);
  dateTimeForAdapter.cbSet_.insert(dateTimeForAdapter.cb_);
  dateTimeForAdapter.StopListen();
  EXPECT_EQ(dateTimeForAdapter.commonEventSubscriberInfo_, nullptr);
  EXPECT_EQ(dateTimeForAdapter.commonEventSubscriber_, nullptr);
  g_mock_OH_CommonEvent_Subscribe = nullptr;
  g_mock_OH_CommonEvent_UnSubscribe = nullptr;
}

TEST_F(DateTimeFormatAdapterImplTest, StopListen_001) {
  DateTimeFormatAdapterImpl dateTimeForAdapter;
  bool ret = dateTimeForAdapter.StartListen();
  EXPECT_TRUE(ret);
  EXPECT_NE(dateTimeForAdapter.commonEventSubscriberInfo_, nullptr);
  EXPECT_NE(dateTimeForAdapter.commonEventSubscriber_, nullptr);

  g_mock_OH_CommonEvent_UnSubscribe = [](const CommonEvent_Subscriber* subscriber) {
    __real_OH_CommonEvent_UnSubscribe(subscriber);
    return COMMONEVENT_ERR_PERMISSION_ERROR;
  };
  dateTimeForAdapter.StopListen();
  EXPECT_EQ(dateTimeForAdapter.commonEventSubscriberInfo_, nullptr);
  EXPECT_EQ(dateTimeForAdapter.commonEventSubscriber_, nullptr);
  g_mock_OH_CommonEvent_UnSubscribe = nullptr;
}

TEST_F(DateTimeFormatAdapterImplTest, GetTimezone_001) {
  DateTimeFormatAdapterImpl dateTimeForAdapter;
  g_mock_OH_TimeService_GetTimeZone = [](char *timeZone, uint32_t len) {
    strcpy_s(timeZone, len - 1, "timezone");
    return TIMESERVICE_ERR_INTERNAL_ERROR;
  };
  std::string timezone = dateTimeForAdapter.GetTimezone();
  EXPECT_EQ(timezone, "timezone");
  g_mock_OH_TimeService_GetTimeZone = nullptr;
}
} // namespace OHOS::NWeb
