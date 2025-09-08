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

#define private public
#include "arkweb/ohos_adapter_ndk/date_time_format_adapter/date_time_format_adapter_impl.h"
#undef private
#include "arkweb/ohos_adapter_ndk/date_time_format_adapter/date_time_format_adapter_impl.cpp"

#include "base/logging.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

using namespace testing;

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
} // namespace OHOS::NWeb
