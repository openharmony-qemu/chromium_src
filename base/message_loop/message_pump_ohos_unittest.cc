/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#define private public
#define protected public
#include "base/message_loop/message_pump_ohos.h"
#undef private
#undef protected

#if defined(OHOS_UNITTESTS)
#define private public
#include "base/message_loop/message_pump.h"
#undef private
#include <memory>
#include <mutex>
#include "third_party/ohos_ndk/includes/ohos_adapter/event_handler_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_web_data_base_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/pasteboard_client_adapter.h"
#endif // OHOS_UNITTESTS

#include "base/message_loop/message_pump_type.h"
#include "base/run_loop.h"
#include "base/test/task_environment.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

#include <limits>

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::AtMost;
using ::testing::Invoke;
using ::testing::Return;

namespace base {

namespace {

class MockMessagePumpDelegate : public MessagePump::Delegate {
 public:
  MockMessagePumpDelegate() = default;

  ~MockMessagePumpDelegate() override {}

  MockMessagePumpDelegate(const MockMessagePumpDelegate&) = delete;
  MockMessagePumpDelegate& operator=(const MockMessagePumpDelegate&) = delete;

  MOCK_METHOD(void, OnBeginWorkItem, (), (override));
  MOCK_METHOD(void, OnEndWorkItem, (int run_level_depth), (override));
  MOCK_METHOD(int, RunDepth, (), (override));
  MOCK_METHOD(void, BeforeWait, (), (override));

  MOCK_METHOD(MessagePump::Delegate::NextWorkInfo, DoWork, (), (override));
  MOCK_METHOD(bool, DoIdleWork, (), (override));
};

#if defined(OHOS_UNITTESTS)
class MockEventHandlerAdapter : public OHOS::NWeb::EventHandlerAdapter {
 public:
  MOCK_METHOD(bool,
              AddFileDescriptorListener,
              (int32_t fileDescriptor,
               uint32_t events,
               const std::shared_ptr<OHOS::NWeb::EventHandlerFDListenerAdapter>
                   listener),
              (override));
  MOCK_METHOD(void,
              RemoveFileDescriptorListener,
              (int32_t fileDescriptor),
              (override));
};
#endif // OHOS_UNITTESTS

class MessagePumpForUITest : public ::testing::Test {
 protected:
  void SetUp() override {
    message_pump_for_ui_ = new MessagePumpForUI();
    task_environment_ = new test::SingleThreadTaskEnvironment();
  }

  void TearDown() override {
    if (message_pump_for_ui_) {
      delete message_pump_for_ui_;
      message_pump_for_ui_ = nullptr;
    }
    if (task_environment_) {
      delete task_environment_;
      task_environment_ = nullptr;
    }
  }

 public:
  MessagePumpForUI* message_pump_for_ui_;
  test::SingleThreadTaskEnvironment* task_environment_;
};

constexpr uint64_t kTryNativeTasksBeforeIdleBit = uint64_t(1) << 32;
}  // namespace

TEST_F(MessagePumpForUITest, Constructor) {
  EXPECT_NE(message_pump_for_ui_, nullptr);
}

TEST_F(MessagePumpForUITest, OnDelayedLooperCallback_ShouldQuit_True) {
  ASSERT_NE(message_pump_for_ui_, nullptr);

  message_pump_for_ui_->should_abort_ = true;
  message_pump_for_ui_->OnDelayedLooperCallback();
  EXPECT_TRUE(message_pump_for_ui_->ShouldQuit());
}

TEST_F(MessagePumpForUITest, OnDelayedLooperCallback_ShouldQuit_False1) {
  ASSERT_NE(message_pump_for_ui_, nullptr);

  message_pump_for_ui_->should_abort_ = false;
  message_pump_for_ui_->quit_ = false;
  auto delegate = std::make_unique<MockMessagePumpDelegate>();
  message_pump_for_ui_->delegate_ = delegate.get();
  EXPECT_CALL(*delegate, DoWork).Times(1).WillOnce(Invoke([this] {
    message_pump_for_ui_->Abort();
    return MessagePump::Delegate::NextWorkInfo{TimeTicks()};
  }));
  message_pump_for_ui_->OnDelayedLooperCallback();
  EXPECT_TRUE(message_pump_for_ui_->ShouldQuit());
}

TEST_F(MessagePumpForUITest, DoDelayedLooperWork_ShouldQuit_True) {
  ASSERT_NE(message_pump_for_ui_, nullptr);

  message_pump_for_ui_->should_abort_ = true;
  auto delegate = std::make_unique<MockMessagePumpDelegate>();
  message_pump_for_ui_->delegate_ = delegate.get();
  EXPECT_CALL(*delegate, DoWork).Times(1);
  message_pump_for_ui_->DoDelayedLooperWork();
  EXPECT_TRUE(message_pump_for_ui_->ShouldQuit());
}

TEST_F(MessagePumpForUITest, DoDelayedLooperWork_Do_Schedule_Return) {
  ASSERT_NE(message_pump_for_ui_, nullptr);

  message_pump_for_ui_->should_abort_ = false;
  message_pump_for_ui_->quit_ = false;
  auto delegate = std::make_unique<MockMessagePumpDelegate>();
  message_pump_for_ui_->delegate_ = delegate.get();
  EXPECT_CALL(*delegate, DoWork).Times(1).WillOnce(Invoke([] {
    return MessagePump::Delegate::NextWorkInfo{TimeTicks()};
  }));
  EXPECT_CALL(*delegate, DoIdleWork).Times(0);
  message_pump_for_ui_->DoDelayedLooperWork();
  EXPECT_FALSE(message_pump_for_ui_->ShouldQuit());
}

TEST_F(MessagePumpForUITest, DoDelayedLooperWork_Do_Idle_And_Delayed_End) {
  ASSERT_NE(message_pump_for_ui_, nullptr);

  message_pump_for_ui_->should_abort_ = false;
  message_pump_for_ui_->quit_ = false;
  auto delegate = std::make_unique<MockMessagePumpDelegate>();
  message_pump_for_ui_->delegate_ = delegate.get();
  EXPECT_CALL(*delegate, DoWork).Times(1).WillOnce(Invoke([] {
    return MessagePump::Delegate::NextWorkInfo{TimeTicks::Now()};
  }));
  EXPECT_CALL(*delegate, DoIdleWork).Times(1).WillOnce(Invoke([] {
    return false;
  }));
  message_pump_for_ui_->DoDelayedLooperWork();
  EXPECT_FALSE(message_pump_for_ui_->ShouldQuit());
}

TEST_F(MessagePumpForUITest, DoDelayedLooperWork_Do_Idle_End) {
  ASSERT_NE(message_pump_for_ui_, nullptr);

  message_pump_for_ui_->should_abort_ = false;
  message_pump_for_ui_->quit_ = false;
  auto delegate = std::make_unique<MockMessagePumpDelegate>();
  message_pump_for_ui_->delegate_ = delegate.get();
  EXPECT_CALL(*delegate, DoWork).Times(1).WillOnce(Invoke([] {
    return MessagePump::Delegate::NextWorkInfo{TimeTicks::Max()};
  }));
  EXPECT_CALL(*delegate, DoIdleWork).Times(1).WillOnce(Invoke([] {
    return false;
  }));
  message_pump_for_ui_->DoDelayedLooperWork();
  EXPECT_FALSE(message_pump_for_ui_->ShouldQuit());
}

TEST_F(MessagePumpForUITest, OnNonDelayedLooperCallback_ShouldQuit_True) {
  ASSERT_NE(message_pump_for_ui_, nullptr);

  message_pump_for_ui_->should_abort_ = true;
  message_pump_for_ui_->OnNonDelayedLooperCallback();
  EXPECT_TRUE(message_pump_for_ui_->ShouldQuit());
}

TEST_F(MessagePumpForUITest, OnNonDelayedLooperCallback_ShouldQuit_False) {
  ASSERT_NE(message_pump_for_ui_, nullptr);

  message_pump_for_ui_->should_abort_ = false;
  message_pump_for_ui_->quit_ = false;
  auto delegate = std::make_unique<MockMessagePumpDelegate>();
  message_pump_for_ui_->delegate_ = delegate.get();
  EXPECT_CALL(*delegate, DoWork).Times(1).WillOnce(Invoke([] {
    return MessagePump::Delegate::NextWorkInfo{TimeTicks::Now()};
  }));
  message_pump_for_ui_->OnNonDelayedLooperCallback();
  EXPECT_FALSE(message_pump_for_ui_->ShouldQuit());
}

TEST_F(MessagePumpForUITest, DoNonDelayedLooperWork_ShouldQuit_True) {
  ASSERT_NE(message_pump_for_ui_, nullptr);

  message_pump_for_ui_->should_abort_ = true;
  auto delegate = std::make_unique<MockMessagePumpDelegate>();
  message_pump_for_ui_->delegate_ = delegate.get();
  EXPECT_CALL(*delegate, DoWork).Times(0);
  message_pump_for_ui_->DoNonDelayedLooperWork(false);
  EXPECT_TRUE(message_pump_for_ui_->ShouldQuit());
}

TEST_F(MessagePumpForUITest,
       DoNonDelayedLooperWork_ShouldQuit_False_Loop_Return) {
  ASSERT_NE(message_pump_for_ui_, nullptr);

  message_pump_for_ui_->should_abort_ = false;
  message_pump_for_ui_->quit_ = false;
  EXPECT_FALSE(message_pump_for_ui_->ShouldQuit());
  auto delegate = std::make_unique<MockMessagePumpDelegate>();
  message_pump_for_ui_->delegate_ = delegate.get();
  EXPECT_CALL(*delegate, DoWork).Times(1).WillOnce(Invoke([this] {
    if (this->message_pump_for_ui_) {
      this->message_pump_for_ui_->Abort();
    }
    return MessagePump::Delegate::NextWorkInfo{};
  }));
  message_pump_for_ui_->DoNonDelayedLooperWork(false);
  EXPECT_TRUE(message_pump_for_ui_->ShouldQuit());
}

TEST_F(MessagePumpForUITest,
       DoNonDelayedLooperWork_ShouldQuit_False_Break_Return) {
  ASSERT_NE(message_pump_for_ui_, nullptr);

  message_pump_for_ui_->should_abort_ = false;
  message_pump_for_ui_->quit_ = false;
  EXPECT_FALSE(message_pump_for_ui_->ShouldQuit());
  auto delegate = std::make_unique<MockMessagePumpDelegate>();
  message_pump_for_ui_->delegate_ = delegate.get();
  EXPECT_CALL(*delegate, DoWork).Times(1).WillOnce(Invoke([this] {
    if (this->message_pump_for_ui_) {
      this->message_pump_for_ui_->Abort();
    }
    return MessagePump::Delegate::NextWorkInfo{TimeTicks::Now()};
  }));
  message_pump_for_ui_->DoNonDelayedLooperWork(false);
  EXPECT_TRUE(message_pump_for_ui_->ShouldQuit());
}

TEST_F(MessagePumpForUITest,
       DoNonDelayedLooperWork_Do_Schedule_Internel_Return) {
  ASSERT_NE(message_pump_for_ui_, nullptr);

  message_pump_for_ui_->should_abort_ = false;
  message_pump_for_ui_->quit_ = false;
  auto delegate = std::make_unique<MockMessagePumpDelegate>();
  message_pump_for_ui_->delegate_ = delegate.get();
  EXPECT_CALL(*delegate, DoWork).Times(1).WillOnce(Invoke([] {
    return MessagePump::Delegate::NextWorkInfo{TimeTicks::Now()};
  }));
  message_pump_for_ui_->DoNonDelayedLooperWork(false);
  EXPECT_FALSE(message_pump_for_ui_->ShouldQuit());
  EXPECT_CALL(*delegate, DoIdleWork).Times(0);

  uint64_t value;
  int ret = read(message_pump_for_ui_->non_delayed_fd_, &value, sizeof(value));
  EXPECT_GE(ret, 0);
  EXPECT_EQ(value, kTryNativeTasksBeforeIdleBit);
}

TEST_F(MessagePumpForUITest,
       DoNonDelayedLooperWork_Do_Idle_Do_Schedule_Delayed_End) {
  ASSERT_NE(message_pump_for_ui_, nullptr);

  message_pump_for_ui_->should_abort_ = false;
  message_pump_for_ui_->quit_ = false;
  message_pump_for_ui_->delayed_scheduled_time_.reset();
  auto delegate = std::make_unique<MockMessagePumpDelegate>();
  message_pump_for_ui_->delegate_ = delegate.get();
  EXPECT_CALL(*delegate, DoWork).Times(1).WillOnce(Invoke([] {
    return MessagePump::Delegate::NextWorkInfo{TimeTicks::Now()};
  }));
  EXPECT_CALL(*delegate, DoIdleWork).Times(1).WillOnce(Invoke([] {
    return false;
  }));
  message_pump_for_ui_->DoNonDelayedLooperWork(true);
  EXPECT_FALSE(message_pump_for_ui_->ShouldQuit());
}

TEST_F(MessagePumpForUITest, DoNonDelayedLooperWork_Do_Idle_End) {
  ASSERT_NE(message_pump_for_ui_, nullptr);

  message_pump_for_ui_->should_abort_ = false;
  message_pump_for_ui_->quit_ = false;
  auto delegate = std::make_unique<MockMessagePumpDelegate>();
  message_pump_for_ui_->delegate_ = delegate.get();
  EXPECT_CALL(*delegate, DoWork).Times(1).WillOnce(Invoke([] {
    return MessagePump::Delegate::NextWorkInfo{TimeTicks::Max()};
  }));
  EXPECT_CALL(*delegate, DoIdleWork).Times(1).WillOnce(Invoke([] {
    return false;
  }));
  message_pump_for_ui_->DoNonDelayedLooperWork(true);
  EXPECT_FALSE(message_pump_for_ui_->ShouldQuit());
}

TEST_F(MessagePumpForUITest, DoIdleWork_False_End) {
  ASSERT_NE(message_pump_for_ui_, nullptr);

  auto delegate = std::make_unique<MockMessagePumpDelegate>();
  message_pump_for_ui_->delegate_ = delegate.get();
  EXPECT_CALL(*delegate, DoIdleWork).Times(1).WillOnce(Invoke([] {
    return false;
  }));
  message_pump_for_ui_->DoIdleWork();
}

TEST_F(MessagePumpForUITest, DoIdleWork_True_Do_Schedule_End) {
  ASSERT_NE(message_pump_for_ui_, nullptr);

  auto delegate = std::make_unique<MockMessagePumpDelegate>();
  message_pump_for_ui_->delegate_ = delegate.get();
  EXPECT_CALL(*delegate, DoIdleWork).Times(1).WillOnce(Invoke([] {
    return true;
  }));
  message_pump_for_ui_->DoIdleWork();

  uint64_t value;
  int ret = read(message_pump_for_ui_->non_delayed_fd_, &value, sizeof(value));
  EXPECT_GE(ret, 0);
  EXPECT_EQ(value, uint64_t(1));
}

TEST_F(MessagePumpForUITest, Run) {}

TEST_F(MessagePumpForUITest, Attach_Quit_False) {
  ASSERT_NE(message_pump_for_ui_, nullptr);

  RunLoop().RunUntilIdle();
  auto delegate = std::make_unique<MockMessagePumpDelegate>();
  message_pump_for_ui_->quit_ = false;
  message_pump_for_ui_->Attach(delegate.get());
  EXPECT_TRUE(message_pump_for_ui_->run_loop_);
}

TEST_F(MessagePumpForUITest, Quit_Directly_Return) {
  ASSERT_NE(message_pump_for_ui_, nullptr);

  message_pump_for_ui_->quit_ = true;
  message_pump_for_ui_->Quit();
  EXPECT_TRUE(message_pump_for_ui_->ShouldQuit());
}

TEST_F(MessagePumpForUITest, Quit_Skip_RunLoop_Skip_Callback_End) {
  ASSERT_NE(message_pump_for_ui_, nullptr);

  message_pump_for_ui_->quit_ = false;
  message_pump_for_ui_->Quit();
  EXPECT_FALSE(message_pump_for_ui_->run_loop_);
  EXPECT_TRUE(message_pump_for_ui_->ShouldQuit());
  EXPECT_TRUE(message_pump_for_ui_->on_quit_callback_.is_null());
}

TEST_F(MessagePumpForUITest, Quit_Do_RunLoop_Skip_Callback_End) {
  ASSERT_NE(message_pump_for_ui_, nullptr);

  RunLoop().RunUntilIdle();
  message_pump_for_ui_->quit_ = false;
  auto delegate = std::make_unique<MockMessagePumpDelegate>();
  message_pump_for_ui_->Attach(delegate.get());
  EXPECT_TRUE(message_pump_for_ui_->run_loop_);
  message_pump_for_ui_->Quit();
  EXPECT_FALSE(message_pump_for_ui_->run_loop_);
  EXPECT_TRUE(message_pump_for_ui_->ShouldQuit());
  EXPECT_TRUE(message_pump_for_ui_->on_quit_callback_.is_null());
}

TEST_F(MessagePumpForUITest, Quit_Skip_RunLoop_Do_Callback_End) {
  ASSERT_NE(message_pump_for_ui_, nullptr);

  message_pump_for_ui_->quit_ = false;
  auto delegate = std::make_unique<MockMessagePumpDelegate>();
  message_pump_for_ui_->on_quit_callback_ = base::BindOnce(
      [](MockMessagePumpDelegate* delegate) {
        if (delegate) {
          delegate->DoWork();
        }
      },
      delegate.get());
  EXPECT_FALSE(message_pump_for_ui_->on_quit_callback_.is_null());
  EXPECT_CALL(*delegate, DoWork).Times(1);
  message_pump_for_ui_->Quit();
  EXPECT_FALSE(message_pump_for_ui_->run_loop_);
  EXPECT_TRUE(message_pump_for_ui_->ShouldQuit());
  EXPECT_TRUE(message_pump_for_ui_->on_quit_callback_.is_null());
}

TEST_F(MessagePumpForUITest, ScheduleWork_Do_Internal_False) {
  ASSERT_NE(message_pump_for_ui_, nullptr);

  message_pump_for_ui_->ScheduleWork();
  uint64_t value;
  int ret = read(message_pump_for_ui_->non_delayed_fd_, &value, sizeof(value));
  EXPECT_GE(ret, 0);
  EXPECT_EQ(value, uint64_t(1));
}

TEST_F(MessagePumpForUITest, ScheduleWorkInternal_True) {
  ASSERT_NE(message_pump_for_ui_, nullptr);

  message_pump_for_ui_->ScheduleWorkInternal(true);
  uint64_t value;
  int ret = read(message_pump_for_ui_->non_delayed_fd_, &value, sizeof(value));
  EXPECT_GE(ret, 0);
  EXPECT_EQ(value, kTryNativeTasksBeforeIdleBit);
}

TEST_F(MessagePumpForUITest, ScheduleWorkInternal_False) {
  ASSERT_NE(message_pump_for_ui_, nullptr);

  message_pump_for_ui_->ScheduleWorkInternal(false);
  uint64_t value;
  int ret = read(message_pump_for_ui_->non_delayed_fd_, &value, sizeof(value));
  EXPECT_GE(ret, 0);
  EXPECT_EQ(value, uint64_t(1));
}

TEST_F(MessagePumpForUITest, ScheduleDelayedWork_ShouldQuit_True) {
  ASSERT_NE(message_pump_for_ui_, nullptr);

  message_pump_for_ui_->quit_ = true;
  auto nextWorkInfo = MessagePump::Delegate::NextWorkInfo{TimeTicks::Max()};
  message_pump_for_ui_->ScheduleDelayedWork(nextWorkInfo);
  EXPECT_TRUE(message_pump_for_ui_->ShouldQuit());
}

TEST_F(MessagePumpForUITest,
       ScheduleDelayedWork_ShouldQuit_False_DelayTime_True) {
  ASSERT_NE(message_pump_for_ui_, nullptr);

  message_pump_for_ui_->should_abort_ = false;
  message_pump_for_ui_->quit_ = false;
  message_pump_for_ui_->delayed_scheduled_time_ = TimeTicks::Max();
  auto nextWorkInfo = MessagePump::Delegate::NextWorkInfo{TimeTicks::Max()};
  message_pump_for_ui_->ScheduleDelayedWork(nextWorkInfo);
  EXPECT_FALSE(message_pump_for_ui_->ShouldQuit());
}

TEST_F(MessagePumpForUITest,
       ScheduleDelayedWork_ShouldQuit_False_DelayTime_False) {
  ASSERT_NE(message_pump_for_ui_, nullptr);

  message_pump_for_ui_->should_abort_ = false;
  message_pump_for_ui_->quit_ = false;
  message_pump_for_ui_->delayed_scheduled_time_ = TimeTicks::Max();
  auto now = TimeTicks::Now();
  auto nextWorkInfo = MessagePump::Delegate::NextWorkInfo{now};
  message_pump_for_ui_->ScheduleDelayedWork(nextWorkInfo);
  EXPECT_FALSE(message_pump_for_ui_->ShouldQuit());
  EXPECT_EQ(message_pump_for_ui_->delayed_scheduled_time_, now);
}

TEST_F(MessagePumpForUITest, QuitWhenIdle_Do_Schedule_End) {
  ASSERT_NE(message_pump_for_ui_, nullptr);

  auto delegate = std::make_unique<MockMessagePumpDelegate>();
  message_pump_for_ui_->run_loop_ = std::make_unique<RunLoop>();
  message_pump_for_ui_->QuitWhenIdle(base::BindOnce(
      [](MockMessagePumpDelegate* delegate) {
        if (delegate) {
          delegate->DoWork();
        }
      },
      delegate.get()));

  ASSERT_TRUE(message_pump_for_ui_->on_quit_callback_);
  EXPECT_CALL(*delegate, DoWork).Times(1);
  std::move(message_pump_for_ui_->on_quit_callback_).Run();
}

TEST_F(MessagePumpForUITest, SetDelegate) {
  ASSERT_NE(message_pump_for_ui_, nullptr);

  message_pump_for_ui_->delegate_ = nullptr;
  MessagePump::Delegate* delegateNew = new MockMessagePumpDelegate();
  auto delegateOld = message_pump_for_ui_->SetDelegate(delegateNew);
  EXPECT_NE(message_pump_for_ui_->delegate_, nullptr);
  EXPECT_EQ(delegateOld, nullptr);
  if (message_pump_for_ui_->delegate_) {
    delete message_pump_for_ui_->delegate_;
    message_pump_for_ui_->delegate_ = nullptr;
  }
}

TEST_F(MessagePumpForUITest, SetQuit) {
  ASSERT_NE(message_pump_for_ui_, nullptr);

  message_pump_for_ui_->quit_ = false;
  auto quitFalse = message_pump_for_ui_->SetQuit(true);
  EXPECT_TRUE(message_pump_for_ui_->quit_);
  EXPECT_FALSE(quitFalse);

  message_pump_for_ui_->quit_ = true;
  auto quitTrue = message_pump_for_ui_->SetQuit(false);
  EXPECT_FALSE(message_pump_for_ui_->quit_);
  EXPECT_TRUE(quitTrue);
}

TEST_F(MessagePumpForUITest, DeConstructor) {
  if (message_pump_for_ui_) {
    delete message_pump_for_ui_;
    message_pump_for_ui_ = nullptr;
  }
  EXPECT_EQ(message_pump_for_ui_, nullptr);
}

#if defined(OHOS_UNITTESTS)
TEST_F(MessagePumpForUITest, MessagePumpForUI_EventNonDelayedFd) {
  std::unique_ptr<OHOS::NWeb::EventHandlerAdapter> ohos_event_handler_adapter_;
  base::MessagePumpForUI message_ui;
  EXPECT_NE(message_ui.non_delayed_fd_, -1);
}

TEST_F(MessagePumpForUITest, MessagePumpForUI_EventDelayedFd) {
  std::unique_ptr<OHOS::NWeb::EventHandlerAdapter> ohos_event_handler_adapter_ =
      std::make_unique<MockEventHandlerAdapter>();
  base::MessagePumpForUI message_ui;
  EXPECT_NE(message_ui.delayed_fd_, -1);
}

TEST_F(MessagePumpForUITest, MessagePumpForUI_IfAllFalse) {
  std::unique_ptr<OHOS::NWeb::EventHandlerAdapter> ohos_event_handler_adapter_ =
      std::make_unique<MockEventHandlerAdapter>();
  base::MessagePumpForUI message_ui;
  message_ui.non_delayed_fd_ = -1;
  message_ui.delayed_fd_ = -1;
  auto ohos_listener = std::make_shared<MockEventHandlerAdapter>();
  message_ui.ohos_event_handler_adapter_->AddFileDescriptorListener(
      0, OHOS::NWeb::EventHandlerAdapter::INPUT_EVENT,
      message_ui.ohos_listener);
  message_ui.ohos_event_handler_adapter_->AddFileDescriptorListener(
      0, OHOS::NWeb::EventHandlerAdapter::INPUT_EVENT,
      message_ui.ohos_listener);
  EXPECT_TRUE(
      !message_ui.ohos_event_handler_adapter_->AddFileDescriptorListener(
          0, OHOS::NWeb::EventHandlerAdapter::INPUT_EVENT,
          message_ui.ohos_listener));
  EXPECT_TRUE(
      !message_ui.ohos_event_handler_adapter_->AddFileDescriptorListener(
          0, OHOS::NWeb::EventHandlerAdapter::INPUT_EVENT,
          message_ui.ohos_listener));
}

TEST_F(MessagePumpForUITest, OnDelayedLooperCallback) {
  std::unique_ptr<OHOS::NWeb::EventHandlerAdapter> ohos_event_handler_adapter_;
  base::MessagePumpForUI message_ui;
  errno = EAGAIN;
  message_ui.non_delayed_fd_ = -1;
  message_ui.delayed_fd_ = -1;
  MockMessagePumpDelegate mock_delegate_;
  message_ui.delegate_ = &mock_delegate_;
  EXPECT_CALL(mock_delegate_, DoWork).Times(testing::AtLeast(1));
  message_ui.OnDelayedLooperCallback();
  EXPECT_TRUE(message_ui.delegate_);
}
#endif // OHOS_UNITTESTS
}  // namespace base
