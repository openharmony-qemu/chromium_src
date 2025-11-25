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

#ifndef BASE_OHOS_DYNAMIC_FRAME_LOSS_MONITOR_H_
#define BASE_OHOS_DYNAMIC_FRAME_LOSS_MONITOR_H_

#include <mutex>

namespace base {
namespace ohos {

// Count the Frame Loss event when page is sliding
class DynamicFrameLossMonitor {
 public:
  DynamicFrameLossMonitor() = default;
  ~DynamicFrameLossMonitor() = default;

  DynamicFrameLossMonitor(const DynamicFrameLossMonitor&) = delete;
  DynamicFrameLossMonitor& operator=(const DynamicFrameLossMonitor&) = delete;

  static DynamicFrameLossMonitor& GetInstance();

  void StartMonitor(int32_t nweb_id);
  void StopMonitor();
  void OnSwapBuffer();
  void OnVsync();

 private:
  int64_t GetCurrentTimestampMS();
  void ReportToHiSysEvent();
  void ReportToHiAppEvent();
  void ResetStatus();

  std::mutex monitor_mutex_;
  bool is_monitoring_ = false;

  // record frame loss only after receive first response frame
  bool received_first_frame_ = false;

  // interaction scene begin time
  int64_t start_time_ = 0;
  // interaction scene stop time
  int64_t stop_time_ = 0;
  // total app frames during the scene
  int32_t total_app_frames_ = 0;
  // total app missed frames during the scene
  int32_t total_app_missed_frames_ = 0;

  // max single frame during the scene
  int64_t max_app_frametime_ = 0;
  // timestamp of previous swap buffer
  int64_t prev_swap_buffer_time_ = 0;

  // max successive missed frames during the scene
  int32_t max_app_seq_missed_frames_ = 0;
  // successive missed frames during the scene
  int32_t app_seq_missed_frames_ = 0;
  int32_t app_seq_frames_ = 0;

  // the number of buffer, if cached_buffer_number < 0, it will be loss frame
  int32_t cached_buffer_number_ = 0;

  // the id fo current nweb
  int32_t current_nweb_id_ = 0;
};
}  // namespace ohos
}  // namespace base

#endif  // BASE_OHOS_DYNAMIC_FRAME_LOSS_MONITOR_H_
