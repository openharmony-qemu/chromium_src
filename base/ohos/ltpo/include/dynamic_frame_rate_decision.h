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

namespace base {
namespace ohos {
struct DynamicFrameRateState {
  bool enabled_ {false};
  bool dynamicState_ {false};
  int32_t onScreenSlidingRate_ {0};
  int32_t offScreenSlidingRate_ {0};
  int32_t dirtyAreaRate_ {0};
};

// dynamic frame rate
class DynamicFrameRateDecision {
public:
  DynamicFrameRateDecision();
  ~DynamicFrameRateDecision();

  DynamicFrameRateDecision(const DynamicFrameRateDecision&) = delete;
  DynamicFrameRateDecision& operator=(const DynamicFrameRateDecision&) = delete;

  static DynamicFrameRateDecision& GetInstance();

  void ReportSlidingFrameRate(int32_t frame_rate);
  void ReportDirtyRectFrameRate(int32_t frame_rate);
  void ReportVideoFrameRate(int32_t frame_rate);
private:
  void UpdateFramePreferredRate();

private:
  int32_t slidingFrameRate_ {0};
  int32_t videoFrameRate_ {0};
  int32_t curFrameRate_ {0};
};
}  // namespace ohos
}  // namespace base

#endif  // BASE_OHOS_DYNAMIC_FRAME_RATE_DECISION_H_
