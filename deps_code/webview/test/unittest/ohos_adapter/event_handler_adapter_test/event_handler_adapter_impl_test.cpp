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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "event_runner.h"
#include "nweb_log.h"
#include "ohos_adapter_helper.h"

#define private public
#include "event_handler_adapter_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::NWeb {

class EventHandlerAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

class EventHandlerFDListenerTest : public EventHandlerFDListenerAdapter {
public:
    EventHandlerFDListenerTest() = default;
    ~EventHandlerFDListenerTest() override = default;
    void OnReadable(int32_t fileDescriptor) override
    {
        WVLOG_I("test OnReadable");
        isReadable_ = true;
    }
    bool VerifySuccess()
    {
        return isReadable_;
    }

private:
    bool isReadable_ = false;
};

class OnceCallbackAdapterTest : public OnceCallbackAdapter {
public:
    OnceCallbackAdapterTest() = default;
    ~OnceCallbackAdapterTest() override = default;
    void OnRunnable() override
    {
        WVLOG_I("OnceCallbackAdapterTest OnRunnable");
        isRunnable_ = true;
    }
    bool VerifySuccess()
    {
        return isRunnable_;
    }

private:
    bool isRunnable_ = false;
};

void EventHandlerAdapterImplTest::SetUpTestCase(void) {}

void EventHandlerAdapterImplTest::TearDownTestCase(void) {}

void EventHandlerAdapterImplTest::SetUp(void) {}

void EventHandlerAdapterImplTest::TearDown(void) {}

/**
 * @tc.name: EventHandlerAdapterImplTest_EventHandlerAdapterImpl_001.
 * @tc.desc: EventHandlerAdapter adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(EventHandlerAdapterImplTest, EventHandlerAdapterImplTest_EventHandlerAdapterImpl_001, TestSize.Level1)
{
    int32_t fd = -1;
    auto eventHandlerTest = std::make_unique<EventHandlerAdapterImpl>();
    ASSERT_NE(eventHandlerTest, nullptr);
    EXPECT_EQ(eventHandlerTest->eventHandler_, nullptr);
    bool res = eventHandlerTest->AddFileDescriptorListener(fd, EventHandlerAdapter::INPUT_EVENT, nullptr);
    EXPECT_FALSE(res);

    auto listener = std::make_shared<EventHandlerFDListenerTest>();
    res = eventHandlerTest->AddFileDescriptorListener(fd, EventHandlerAdapter::INPUT_EVENT, listener);
    EXPECT_FALSE(res);
    eventHandlerTest->RemoveFileDescriptorListener(fd);

    auto listenerTest = std::make_shared<EventHandlerFDListenerAdapterImpl>(nullptr);
    listenerTest->OnReadable(fd);
    EXPECT_FALSE(listener->VerifySuccess());

    auto once_callback = std::make_shared<OnceCallbackAdapterTest>();
    auto once_callback_impl = std::make_shared<OnceCallbackAdapterImpl>(nullptr);
    once_callback_impl->OnRunnable();
    eventHandlerTest->PostTask(nullptr);
    EXPECT_FALSE(once_callback->VerifySuccess());
}

/**
 * @tc.name: EventHandlerAdapterImplTest_EventHandlerAdapterImpl_002.
 * @tc.desc: EventHandlerAdapter adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(EventHandlerAdapterImplTest, EventHandlerAdapterImplTest_EventHandlerAdapterImpl_002, TestSize.Level1)
{
    int32_t fd = -1;
    auto eventHandlerTest = std::make_unique<EventHandlerAdapterImpl>();
    ASSERT_NE(eventHandlerTest, nullptr);
    auto runner = AppExecFwk::EventRunner::Create();
    eventHandlerTest->eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    ASSERT_NE(eventHandlerTest->eventHandler_, nullptr);

    auto listener = std::make_shared<EventHandlerFDListenerTest>();
    bool res = eventHandlerTest->AddFileDescriptorListener(fd, EventHandlerAdapter::INPUT_EVENT, listener);
    EXPECT_FALSE(res);
    eventHandlerTest->RemoveFileDescriptorListener(fd);

    auto listenerTest = std::make_shared<EventHandlerFDListenerAdapterImpl>(listener);
    listenerTest->OnReadable(fd);
    EXPECT_TRUE(listener->VerifySuccess());

    auto once_callback = std::make_shared<OnceCallbackAdapterTest>();
    auto once_callback_impl = std::make_shared<OnceCallbackAdapterImpl>(once_callback);
    once_callback_impl->OnRunnable();
    eventHandlerTest->PostTask(once_callback);
    EXPECT_TRUE(once_callback->VerifySuccess());
}
} // namespace OHOS::NWeb
