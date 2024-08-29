/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "base/ohos/dynamic_frame_loss_monitor.h"

#include <chrono>

#include "base/logging.h"
#include "base/no_destructor.h"
#include "base/task/thread_pool.h"
#include "ohos_nweb/src/sysevent/event_reporter.h"
#include "base/trace_event/trace_event.h"

namespace {
const int kSuccessiveFrameLossThreshold = 1;
const int kMicrosecondsPerMillisecond = 1000;
}

namespace base {
namespace ohos {
DynamicFrameLossMonitor& DynamicFrameLossMonitor::GetInstance() {
    static base::NoDestructor<DynamicFrameLossMonitor> instance;
    return *instance.get();
}

void DynamicFrameLossMonitor::StartMonitor() {
  std::unique_lock<std::mutex> lock(monitor_mutex_);
  if (is_monitoring_) {
    return;
  }
  is_monitoring_ = true;
  start_time_ = GetCurrentTimestampMS();
}

void DynamicFrameLossMonitor::StopMonitor() {
  std::unique_lock<std::mutex> lock(monitor_mutex_);
  if (!is_monitoring_) {
    return;
  }
  stop_time_ = GetCurrentTimestampMS();
  Report();
  ResetStatus();
}

void DynamicFrameLossMonitor::OnVsync() {
  std::unique_lock<std::mutex> lock(monitor_mutex_);
  if (!is_monitoring_) {
    return;
  }

  ++total_app_frames_;
  if (!received_first_frame_) {
    return;
  }

  if (cached_buffer_number_ <= 0) {
    // frame loss occurs
    ++total_app_missed_frames_;
    ++app_seq_missed_frames_;
    if (app_seq_frames_ <= kSuccessiveFrameLossThreshold) {
      app_seq_frames_ = 0;
    }
    TRACE_EVENT0("base", "WEBVIEW::DYNAMIC_FRAME_DROP_STATISTICS");
  } else {
    ++app_seq_frames_;
    if (app_seq_frames_ > kSuccessiveFrameLossThreshold) {
     app_seq_missed_frames_ = 0;
    }
    --cached_buffer_number_;
  }

  max_app_seq_missed_frames_ = std::max(max_app_seq_missed_frames_, app_seq_missed_frames_);
}

void DynamicFrameLossMonitor::OnSwapBuffer() {
  std::unique_lock<std::mutex> lock(monitor_mutex_);
  if (!is_monitoring_) {
    return;
  }
  received_first_frame_ = true;
  ++cached_buffer_number_;
  if (prev_swap_buffer_time_ == 0) {
    prev_swap_buffer_time_ = GetCurrentTimestampMS();
    return;
  }
  auto current = GetCurrentTimestampMS();

  max_app_frametime_ = std::max(max_app_frametime_, current - prev_swap_buffer_time_);
  prev_swap_buffer_time_ = current;
}

int64_t DynamicFrameLossMonitor::GetCurrentTimestampMS() {
  auto currentTime = std::chrono::system_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::microseconds>(currentTime)
    .count() / kMicrosecondsPerMillisecond;
}

void DynamicFrameLossMonitor::Report() {
  if(total_app_missed_frames_ == 0) {
    return;
  }
#if defined(REPORT_SYS_EVENT)
  base::ThreadPool::PostTask(
    FROM_HERE,
    {base::TaskPriority::LOWEST},
    base::BindOnce(&ReportSlideJankStats, start_time_, stop_time_ - start_time_, total_app_frames_,
      total_app_missed_frames_, max_app_frametime_, max_app_seq_missed_frames_)
  );
#endif
}

void DynamicFrameLossMonitor::ResetStatus() {
  is_monitoring_ = false;
  received_first_frame_ = false;

  start_time_ = 0;
  stop_time_ = 0;
  total_app_frames_ = 0;
  total_app_missed_frames_ = 0;

  max_app_seq_missed_frames_=0;
  app_seq_missed_frames_ = 0;
  app_seq_frames_ = 0;

  prev_swap_buffer_time_ = 0;
  max_app_frametime_ = 0;

  cached_buffer_number_ = 0;
}
}  // namespace ohos
}  // namespace base