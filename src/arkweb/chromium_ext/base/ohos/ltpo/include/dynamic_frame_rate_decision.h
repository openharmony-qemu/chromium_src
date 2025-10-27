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

#ifndef BASE_OHOS_DYNAMIC_FRAME_RATE_DECISION_H_
#define BASE_OHOS_DYNAMIC_FRAME_RATE_DECISION_H_

#include <stdint.h>

#include <set>

#include "base/ohos/ltpo/include/ltpo_strategy.h"
#include "base/task/single_thread_task_runner.h"

namespace base {
namespace ohos {
// dynamic frame rate
class DynamicFrameRateDecision {
 public:
  DynamicFrameRateDecision();
  ~DynamicFrameRateDecision();

  DynamicFrameRateDecision(const DynamicFrameRateDecision&) = delete;
  DynamicFrameRateDecision& operator=(const DynamicFrameRateDecision&) = delete;

  static DynamicFrameRateDecision& GetInstance();
  void Init();
  void UpdateFramePreferredRate();

  void SetVisible(int nweb_id, bool visible);
  void ReportSlidingFrameRate(int32_t frame_rate);
  void ReportVideoFrameRate(int32_t frame_rate);

  void SetVsyncEnabled(bool enabled);
  void SetHasTouchPoint(bool has_touch_point);
  void SetLTPOStrategy(int32_t strategy);

 private:
  void SetMaxFrameRateThreeSec();
  void SetFrameRateLinkerEnable(bool enabled);
  int64_t GetCurrentTimestampMS();
  void ReportVideoFrameRateImpl(int32_t frame_rate);
  void SetVsyncEnabledImpl(bool enabled);
  void SetVisibleImpl(int nweb_id, bool visible);
  void ReportSlidingFrameRateImpl(int32_t frame_rate);
  void SetHasTouchPointImpl(bool has_touch_point);
  void SetLTPOStrategyImpl(int32_t strategy);

 private:
  int32_t sliding_frame_rate_{0};
  int32_t video_frame_rate_{0};
  int32_t cur_frame_rate_{0};
  bool has_touch_point_{false};
  bool frame_rate_linker_enable_{false};
  int32_t vsync_cnt_{0};
  int64_t touch_up_timestamp_{0};
  scoped_refptr<SingleThreadTaskRunner> curent_task_runner_{};
  std::set<int32_t> nwebVisibleSet_{};

  LTPOStrategy strategy_{LTPOStrategy::DISABLED};
};
}  // namespace ohos
}  // namespace base

#endif  // BASE_OHOS_DYNAMIC_FRAME_RATE_DECISION_H_
