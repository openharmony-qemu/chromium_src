// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on message_pump_android.cc originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 

#include "base/message_loop/message_pump_ohos.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/eventfd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#include <utility>

#include "base/check_op.h"
#include "base/functional/callback.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/memory/raw_ptr.h"
#include "base/notreached.h"
#include "base/run_loop.h"
#include "build/build_config.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#if defined(__MUSL__)
#include <sys/timerfd.h>
#endif

#ifndef __NR_timerfd_create
#error "Unable to find syscall for __NR_timerfd_create"
#endif

#ifndef TFD_TIMER_ABSTIME
#define TFD_TIMER_ABSTIME (1 << 0)
#endif
#define OHOS_TIMER_MONOTONIC 0

namespace base {

namespace {

class EventHandlerFileDescriptorListener
    : public OHOS::NWeb::EventHandlerFDListenerAdapter {
 public:
  explicit EventHandlerFileDescriptorListener(MessagePumpForUI* pump,
                                              int non_delay_fd,
                                              int delay_fd)
      : message_pump_(pump),
        non_delayed_fd_(non_delay_fd),
        delayed_fd_(delay_fd) {}

  ~EventHandlerFileDescriptorListener() {
    message_pump_ = nullptr;
    non_delayed_fd_ = -1;
    delayed_fd_ = -1;
  }

  void OnReadable(int32_t fileDescriptor) override {
    if (fileDescriptor < 0) {
      LOG(ERROR) << "EventHandlerFileDescriptorListener error fd";
      return;
    }

    if (fileDescriptor == non_delayed_fd_) {
      message_pump_->OnNonDelayedLooperCallback();
    } else if (fileDescriptor == delayed_fd_) {
      message_pump_->OnDelayedLooperCallback();
    } else {
      LOG(ERROR) << "EventHandlerFileDescriptorListener invalid fd: "
                 << fileDescriptor;
    }
  }

 private:
  raw_ptr<MessagePumpForUI> message_pump_;
  int non_delayed_fd_;
  int delayed_fd_;
};

int OhosTimerFdCreate(int clockid, int flags) {
#if defined(__MUSL__)
  return timerfd_create(clockid, flags);
#else
  return syscall(__NR_timerfd_create, clockid, flags);
#endif
}

int OhosTimerfdSettime(int ufc,
                       const struct itimerspec* utmr,
                       struct itimerspec* otmr) {
#if defined(__MUSL__)
  return timerfd_settime(ufc, TFD_TIMER_ABSTIME, utmr, otmr);
#else
  return syscall(__NR_timerfd_settime, ufc, TFD_TIMER_ABSTIME, utmr, otmr);
#endif
}

// A bit added to the |non_delayed_fd_| to keep it signaled when we yield to
// native tasks below.
constexpr uint64_t kTryNativeTasksBeforeIdleBit = uint64_t(1) << 32;
}  // namespace

MessagePumpForUI::MessagePumpForUI() {
  ohos_event_handler_adapter_ =
      OHOS::NWeb::OhosAdapterHelper::GetInstance().GetEventHandlerAdapter();
  if (!ohos_event_handler_adapter_) {
    LOG(ERROR) << "MessagePumpForUI creat event handler adapter failed";
    return;
  }

  non_delayed_fd_ = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  CHECK_NE(non_delayed_fd_, -1);
  DCHECK_EQ(TimeTicks::GetClock(), TimeTicks::Clock::LINUX_CLOCK_MONOTONIC);

  // We can't create the timerfd with TFD_NONBLOCK | TFD_CLOEXEC as we can't
  // include timerfd.h. See comments above on __NR_timerfd_create. It looks like
  // they're just aliases to O_NONBLOCK and O_CLOEXEC anyways, so this should be
  // fine.
  delayed_fd_ = OhosTimerFdCreate(CLOCK_MONOTONIC, O_NONBLOCK | O_CLOEXEC);
  CHECK_NE(delayed_fd_, -1);

  ohos_listener = std::make_shared<EventHandlerFileDescriptorListener>(
      this, non_delayed_fd_, delayed_fd_);
  if (!ohos_event_handler_adapter_->AddFileDescriptorListener(
          non_delayed_fd_, OHOS::NWeb::EventHandlerAdapter::INPUT_EVENT,
          ohos_listener) ||
      !ohos_event_handler_adapter_->AddFileDescriptorListener(
          delayed_fd_, OHOS::NWeb::EventHandlerAdapter::INPUT_EVENT,
          ohos_listener)) {
    LOG(ERROR) << "MessagePumpForUI AddFileDescriptorListener failed";
  };
}

MessagePumpForUI::~MessagePumpForUI() {
  ohos_event_handler_adapter_->RemoveFileDescriptorListener(non_delayed_fd_);
  ohos_event_handler_adapter_->RemoveFileDescriptorListener(delayed_fd_);

  close(non_delayed_fd_);
  close(delayed_fd_);
}

void MessagePumpForUI::OnDelayedLooperCallback() {
  if (ShouldQuit()) {
    return;
  }

  // Clear the fd.
  uint64_t value;
  long ret = read(delayed_fd_, &value, sizeof(value));

  DPCHECK(ret >= 0 || errno == EAGAIN);
  DoDelayedLooperWork();
}

void MessagePumpForUI::DoDelayedLooperWork() {
  delayed_scheduled_time_.reset();

  Delegate::NextWorkInfo next_work_info = delegate_->DoWork();

  if (ShouldQuit()) {
    return;
  }

  if (next_work_info.is_immediate()) {
    ScheduleWork();
    return;
  }

  if (!next_work_info.delayed_run_time.is_max()) {
    ScheduleDelayedWork(next_work_info);
  }
}

void MessagePumpForUI::OnNonDelayedLooperCallback() {
  if (ShouldQuit()) {
    return;
  }

  uint64_t value = 0;
  long ret = read(non_delayed_fd_, &value, sizeof(value));
  DPCHECK(ret >= 0);
  DCHECK_GT(value, 0U);
  bool do_idle_work = value == kTryNativeTasksBeforeIdleBit;
  DoNonDelayedLooperWork(do_idle_work);
}

void MessagePumpForUI::DoNonDelayedLooperWork(bool do_idle_work) {
  // Runs all application tasks scheduled to run.
  Delegate::NextWorkInfo next_work_info;
  do {
    if (ShouldQuit()) {
      return;
    }

    next_work_info = delegate_->DoWork();
    // If we are prioritizing native, and the next work would normally run
    // immediately, skip the next work and let the native work items have a
    // chance to run. This is useful when user input is waiting for native to
    // have a chance to run.
    if (next_work_info.is_immediate() && next_work_info.yield_to_native) {
      ScheduleWork();
      return;
    }
  } while (next_work_info.is_immediate());

  // Do not resignal |non_delayed_fd_| if we're quitting (this pump doesn't
  // allow nesting so needing to resume in an outer loop is not an issue
  // either).
  if (ShouldQuit()) {
    return;
  }

  // At this point, the java looper might not be idle - it's impossible to know
  // pre-Android-M, so we may end up doing Idle work while java tasks are still
  // queued up. Note that this won't cause us to fail to run java tasks using
  // QuitWhenIdle, as the JavaHandlerThread will finish running all currently
  // scheduled tasks before it quits. Also note that we can't just add an idle
  // callback to the java looper, as that will fire even if application tasks
  // are still queued up.
  delegate_->DoIdleWork();
  if (!next_work_info.delayed_run_time.is_max()) {
    ScheduleDelayedWork(next_work_info);
  }
}

void MessagePumpForUI::Run(Delegate* delegate) {
  CHECK(false) << "Unexpected call to Run()";
}

void MessagePumpForUI::Attach(Delegate* delegate) {
  DCHECK(!quit_);
  SetDelegate(delegate);
  run_loop_ = std::make_unique<RunLoop>();
  if (!run_loop_->BeforeRun()) {
    NOTREACHED();
  }
}

void MessagePumpForUI::Quit() {
  if (quit_) {
    return;
  }

  quit_ = true;

  int64_t value;
  // Clear any pending timer.
  read(delayed_fd_, &value, sizeof(value));
  // Clear the eventfd.
  read(non_delayed_fd_, &value, sizeof(value));

  if (run_loop_) {
    run_loop_->AfterRun();
    run_loop_ = nullptr;
  }
  if (on_quit_callback_) {
    std::move(on_quit_callback_).Run();
  }
}

void MessagePumpForUI::ScheduleWork() {
  ScheduleWorkInternal(/*do_idle_work=*/false);
}

void MessagePumpForUI::ScheduleWorkInternal(bool do_idle_work) {
  uint64_t value = do_idle_work ? kTryNativeTasksBeforeIdleBit : 1;
  long ret = write(non_delayed_fd_, &value, sizeof(value));
  DPCHECK(ret >= 0);
}

void MessagePumpForUI::ScheduleDelayedWork(
    const Delegate::NextWorkInfo& next_work_info) {
  if (ShouldQuit()) {
    return;
  }

  if (delayed_scheduled_time_ &&
      *delayed_scheduled_time_ == next_work_info.delayed_run_time) {
    return;
  }

  DCHECK(!next_work_info.is_immediate());
  delayed_scheduled_time_ = next_work_info.delayed_run_time;
  int64_t nanos =
      next_work_info.delayed_run_time.since_origin().InNanoseconds();
  struct itimerspec ts;
  ts.it_interval.tv_sec = 0;  // Don't repeat.
  ts.it_interval.tv_nsec = 0;
  ts.it_value.tv_sec = nanos / TimeTicks::kNanosecondsPerSecond;
  ts.it_value.tv_nsec = nanos % TimeTicks::kNanosecondsPerSecond;
  int ret = OhosTimerfdSettime(delayed_fd_, &ts, nullptr);
  DPCHECK(ret >= 0);
}

void MessagePumpForUI::QuitWhenIdle(base::OnceClosure callback) {
  DCHECK(!on_quit_callback_);
  DCHECK(run_loop_);
  on_quit_callback_ = std::move(callback);
  run_loop_->QuitWhenIdle();
  // Pump the loop in case we're already idle.
  ScheduleWork();
}

MessagePump::Delegate* MessagePumpForUI::SetDelegate(Delegate* delegate) {
  return std::exchange(delegate_, delegate);
}

bool MessagePumpForUI::SetQuit(bool quit) {
  return std::exchange(quit_, quit);
}

}  // namespace base
