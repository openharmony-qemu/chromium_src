/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "services/device/time_zone_monitor/time_zone_monitor.h"

#include <memory>
#include <string>

#include "base/logging.h"
#include "base/memory/ref_counted.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/single_thread_task_runner.h"
#include "ohos/adapter/ohos_i18n/ohos_i18n.h"

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

  void StopListening() {
    UnsubscribeTimeZoneListener();
  }

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
