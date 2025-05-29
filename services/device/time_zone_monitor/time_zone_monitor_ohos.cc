// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>
#include <string>

#include "base/logging.h"
#include "base/memory/ref_counted.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/single_thread_task_runner.h"
#include "ohos/adapter/ohos_i18n/ohos_i18n.h"
#include "services/device/time_zone_monitor/time_zone_monitor.h"

using namespace ohos::adapter::ohos_i18n;

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

  void NotifyClientsFromImpl(std::string& timezone);

 private:
  scoped_refptr<TimeZoneMonitorOhosImpl> impl_;
};

namespace {
class TimeZoneMonitorOhosImpl
    : public base::RefCountedThreadSafe<TimeZoneMonitorOhosImpl> {
 public:
  static scoped_refptr<TimeZoneMonitorOhosImpl> Create(
      TimeZoneMonitorOhos* owner) {
    auto impl = base::WrapRefCounted(new TimeZoneMonitorOhosImpl(owner));
    return impl;
  }

  TimeZoneMonitorOhosImpl(const TimeZoneMonitorOhosImpl&) = delete;
  TimeZoneMonitorOhosImpl& operator=(const TimeZoneMonitorOhosImpl&) = delete;

  explicit TimeZoneMonitorOhosImpl(TimeZoneMonitorOhos* owner)
      : owner_(owner),
        main_task_runner_(base::SingleThreadTaskRunner::GetCurrentDefault()) {
    StartListening();
  }

  void StartListening() {
    auto callback = [this](std::string& zone_id) {
      if (!zone_id.empty()) {
        this->OnTimezoneChanged(zone_id);
        return;
      }
      LOG(ERROR) << "get empty timezone";
    };
    RegisterTimeZoneListener(callback);
  }

  void StopListening() { UnsubscribeTimeZoneListener(); }

 private:
  void OnTimezoneChanged(std::string& zone_id) {
    main_task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(&TimeZoneMonitorOhosImpl::OnTimeZoneChangedOnUIThread,
                       base::RetainedRef(this), zone_id));
  }

  void OnTimeZoneChangedOnUIThread(std::string zone_id) {
    DCHECK(main_task_runner_->RunsTasksInCurrentSequence());
    if (owner_) {
      owner_->NotifyClientsFromImpl(zone_id);
    }
  }
  TimeZoneMonitorOhos* owner_;
  scoped_refptr<base::SequencedTaskRunner> main_task_runner_;
};
}  // namespace

TimeZoneMonitorOhos::TimeZoneMonitorOhos() : TimeZoneMonitor(), impl_() {
  impl_ = TimeZoneMonitorOhosImpl::Create(this);
}

TimeZoneMonitorOhos::~TimeZoneMonitorOhos() {
  if (impl_.get()) {
    impl_->StopListening();
  }
}

void TimeZoneMonitorOhos::NotifyClientsFromImpl(std::string& zone_id) {
  NotifyClients(zone_id);
}

std::unique_ptr<TimeZoneMonitor> TimeZoneMonitor::Create(
    scoped_refptr<base::SequencedTaskRunner> file_task_runner) {
  return std::make_unique<TimeZoneMonitorOhos>();
}

}  // namespace device
                      