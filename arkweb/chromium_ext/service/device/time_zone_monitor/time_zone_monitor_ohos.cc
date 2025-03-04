// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>

#include "base/logging.h"
#include "base/memory/ref_counted.h"
#include "base/task/single_thread_task_runner.h"
#include "services/device/time_zone_monitor/time_zone_monitor.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

using namespace OHOS::NWeb;
namespace device {

namespace {
class TimeZoneMonitorOhosImpl;
}  // namespace

class TimeZoneMonitorOhos : public TimeZoneMonitor {
 public:
  TimeZoneMonitorOhos();
  TimeZoneMonitorOhos(const TimeZoneMonitorOhos&) = delete;
  TimeZoneMonitorOhos& operator=(const TimeZoneMonitorOhos&) = delete;

  ~TimeZoneMonitorOhos();

  bool StartListeningTimezoneChange();

  void StopListeningTimezoneChange();

  void NotifyClientsFromImpl(std::string& timezone);

 private:
  scoped_refptr<TimeZoneMonitorOhosImpl> impl_;
};

namespace {

class TimeZoneMonitorOhosImpl;

class TimezoneEventCallback : public OHOS::NWeb::TimezoneEventCallbackAdapter {
 public:
  TimezoneEventCallback(TimeZoneMonitorOhosImpl* impl) : impl_(impl) {}

  void TimezoneChanged(std::shared_ptr<WebTimezoneInfo> info) override;

 private:
  TimeZoneMonitorOhosImpl* impl_;
};

class TimeZoneMonitorOhosImpl
    : public base::RefCountedThreadSafe<TimeZoneMonitorOhosImpl> {
 public:
  static scoped_refptr<TimeZoneMonitorOhosImpl> Create(
      TimeZoneMonitorOhos* owner) {
    LOG(DEBUG) << "TimeZoneMonitorOhosImpl create.";
    auto impl = base::WrapRefCounted(new TimeZoneMonitorOhosImpl(owner));
    return impl;
  }

  TimeZoneMonitorOhosImpl(const TimeZoneMonitorOhosImpl&) = delete;
  TimeZoneMonitorOhosImpl& operator=(const TimeZoneMonitorOhosImpl&) = delete;

  explicit TimeZoneMonitorOhosImpl(TimeZoneMonitorOhos* owner)
      : owner_(owner), isListen(false) {
    timezoneClient = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                         .CreateDateTimeFormatAdapter();
    if (timezoneClient == nullptr) {
      return;
    }

    event_callback_ = std::make_shared<TimezoneEventCallback>(this);
    timezoneClient->RegTimezoneEvent(event_callback_);
    task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();

    StartListening();
  }

  bool StartListening() {
    LOG(DEBUG) << "Start listen timezone.";
    if (isListen) {
      return true;
    }
    if (timezoneClient == nullptr) {
      return false;
    }
    if (!timezoneClient->StartListen()) {
      LOG(ERROR) << "Start listen fail.";
      return false;
    }
    isListen = true;
    return true;
  }

  void StopListening() {
    if (!isListen) {
      return;
    }
    if (timezoneClient == nullptr) {
      return;
    }
    timezoneClient->StopListen();
    LOG(DEBUG) << "Stop listen timezone.";
    isListen = false;
  }

 private:
  friend class TimezoneEventCallback;

  void TimezoneChanged(std::shared_ptr<WebTimezoneInfo> info);

  std::shared_ptr<TimezoneEventCallbackAdapter> event_callback_;

  TimeZoneMonitorOhos* owner_;
  bool isListen;
  std::unique_ptr<OHOS::NWeb::DateTimeFormatAdapter> timezoneClient;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
};
}  // namespace

void TimezoneEventCallback::TimezoneChanged(
    std::shared_ptr<WebTimezoneInfo> info) {
  if (impl_) {
    impl_->TimezoneChanged(info);
  }
}

TimeZoneMonitorOhos::TimeZoneMonitorOhos() : TimeZoneMonitor(), impl_() {
  impl_ = TimeZoneMonitorOhosImpl::Create(this);
}

TimeZoneMonitorOhos::~TimeZoneMonitorOhos() {
  if (impl_.get()) {
    impl_->StopListening();
  }
}

void TimeZoneMonitorOhos::NotifyClientsFromImpl(std::string& timezone) {
  LOG(DEBUG) << "TimeZoneMonitor NotifyClientsFromImpl.";
  NotifyClients(timezone);
}

std::unique_ptr<TimeZoneMonitor> TimeZoneMonitor::Create() {
  LOG(DEBUG) << "TimeZoneMonitor Create.";
  return std::make_unique<TimeZoneMonitorOhos>();
}

void TimeZoneMonitorOhosImpl::TimezoneChanged(
    std::shared_ptr<WebTimezoneInfo> info) {
  LOG(DEBUG) << "receive timezone changed.";
  if (!task_runner_->RunsTasksInCurrentSequence()) {
    task_runner_->PostTask(
        FROM_HERE, base::BindOnce(&TimeZoneMonitorOhosImpl::TimezoneChanged,
                                  base::Unretained(this), std::move(info)));
    return;
  }
  std::string timezone = info->GetTzId();
  owner_->NotifyClientsFromImpl(timezone);
}

}  // namespace device
