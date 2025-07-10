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

using namespace testing;

namespace OHOS {
namespace NWeb {

class DateTimeFormatAdapterImplTest : public testing::Test {
public:
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
} // namespace NWeb
} // namespace OHOS
