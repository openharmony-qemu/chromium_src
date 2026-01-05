/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_TIMING_BLANK_SCREEN_DETECTOR_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_TIMING_BLANK_SCREEN_DETECTOR_H_

#include "base/timer/timer.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/platform/heap/member.h"

namespace blink {

enum class DetectedBlankScreenReason : int32_t {
  NO_CONTENTFUL_NODES = 0,
  SUB_THRESHOLD_CONTENTFUL_NODES = 1,
};

enum class BlankScreenDetectionMethod : int32_t {
  DETECTION_CONTENTFUL_NODES_SEVENTEEN = 0,
};

class BlankScreenDetector : public GarbageCollected<BlankScreenDetector> {
 public:
  explicit BlankScreenDetector(LocalFrame* local_frame)
      : local_frame_(local_frame) {}
  void DetectBlankScreen(const WTF::String& url,
                         const std::vector<double>& detection_timing,
                         const std::vector<int32_t>& detection_methods,
                         int32_t contentful_nodes_count_threshold);
  void OnInputOrScroll();
  void Trace(Visitor* visitor) const;

 private:
  void GenerateTestPointsByMethodDetection17(int32_t width, int32_t height);
  void RunDetectionTask();
  int32_t CalculateContentfulCount();
  bool GenerateTestPoints();
  void GenerateDetectionMethods(const std::vector<int32_t>& detection_methods);
  void GenerateTaskDelays(const std::vector<double>& task_delays);
  void ScheduleNextTask();
  void StopTask();
  void GetPaintRects();

  base::OneShotTimer detection_task_;
  Member<LocalFrame> local_frame_;
  std::vector<int32_t> task_delays_ms_;
  std::vector<gfx::Point> test_points_;
  std::vector<gfx::Rect> paint_rects_;
  std::set<BlankScreenDetectionMethod> detection_methods_;
  size_t current_task_index_ = 0;
  int32_t contentful_nodes_count_threshold_ = 0;
  WTF::String url_;
  const std::vector<int32_t> default_task_delays_ms_ = {1000, 3000, 5000};
  base::TimeTicks task_timestamp_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_TIMING_BLANK_SCREEN_DETECTOR_H_