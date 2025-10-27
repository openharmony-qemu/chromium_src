/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <cstring>
#include <cstdint>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#define private public
#include "date_time_format_adapter_impl.h"
#include "common_event_subscriber.h"
#include "common_event_data.h"
#include "common_event_manager.h"
#include "time_service_client.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace {
bool g_commonEvent = false;
bool g_unCommonEvent = false;
using Want = OHOS::AAFwk::Want;
}
namespace EventFwk {
bool CommonEventManager::SubscribeCommonEvent(const std::shared_ptr<CommonEventSubscriber> &subscriber)
{
    return g_commonEvent;
}

bool CommonEventManager::UnSubscribeCommonEvent(const std::shared_ptr<CommonEventSubscriber> &subscriber)
{
    return g_unCommonEvent;
}
}
namespace NWeb {

class DateTimeAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DateTimeAdapterImplTest::SetUpTestCase(void)
{}

void DateTimeAdapterImplTest::TearDownTestCase(void)
{}

void DateTimeAdapterImplTest::SetUp(void)
{}

void DateTimeAdapterImplTest::TearDown(void)
{}

class MockTimezoneEventCallbackAdapter : public TimezoneEventCallbackAdapter {
public:
    MockTimezoneEventCallbackAdapter() = default;
    void TimezoneChanged(std::shared_ptr<WebTimezoneInfo> info) {}
};

/**
 * @tc.name: DateTimeAdapterImplTest_NWebTimeZoneEventSubscriber_001
 * @tc.desc: NWebTimeZoneEventSubscriber.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DateTimeAdapterImplTest, DateTimeAdapterImplTest_NWebTimeZoneEventSubscriber_001, TestSize.Level1)
{
    EventFwk::CommonEventSubscribeInfo in;
    std::shared_ptr<TimezoneEventCallbackAdapter> cb = std::make_shared<MockTimezoneEventCallbackAdapter>();
    auto adapter = std::make_shared<NWebTimeZoneEventSubscriber>(in, cb);
    EXPECT_NE(adapter, nullptr);
    Want want;
    want.SetAction("test");
    EventFwk::CommonEventData data(want);
    adapter->OnReceiveEvent(data);
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_TIMEZONE_CHANGED);
    data.SetWant(want);
    adapter->OnReceiveEvent(data);
    adapter->eventCallback_ = nullptr;
    adapter->OnReceiveEvent(data);
}

/**
 * @tc.name: DateTimeAdapterImplTest_DateTimeFormatAdapterImpl_002
 * @tc.desc: DateTimeFormatAdapterImpl.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DateTimeAdapterImplTest, DateTimeAdapterImplTest_DateTimeFormatAdapterImpl_002, TestSize.Level1)
{
    auto adapter = std::make_shared<DateTimeFormatAdapterImpl>();
    EXPECT_NE(adapter, nullptr);
    std::shared_ptr<TimezoneEventCallbackAdapter> cb = std::make_shared<MockTimezoneEventCallbackAdapter>();
    adapter->RegTimezoneEvent(std::move(cb));
    bool result = adapter->StartListen();
    EXPECT_FALSE(result);
    g_commonEvent = true;
    result = adapter->StartListen();
    EXPECT_TRUE(result);
    adapter->StopListen();
    g_unCommonEvent = true;
    adapter->StopListen();
    adapter->StopListen();
    std::string timeZoneNicosia("");
    OHOS::MiscServices::TimeServiceClient::GetInstance()->SetTimeZone(timeZoneNicosia);
    auto timeStr = adapter->GetTimezone();
    EXPECT_FALSE(timeStr.empty());
}
}
} // namespace NWeb
