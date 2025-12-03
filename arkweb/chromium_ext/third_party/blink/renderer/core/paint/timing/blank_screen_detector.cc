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

#include "blank_screen_detector.h"

#include "third_party/blink/renderer/core/frame/root_frame_viewport.h"
#include "third_party/blink/renderer/core/frame/viewport_data.h"
#include "third_party/blink/renderer/core/layout/layout_box.h"
#include "third_party/blink/renderer/core/page/page.h"
#include "third_party/blink/renderer/core/paint/timing/paint_timing_detector.h"

namespace blink {

const int32_t SECOND_TO_MILLISECOND = 1000;
const int32_t DIVIDED_BY_TWO = 2;
const int32_t DIVIDED_BY_FIVE = 5;

void BlankScreenDetector::OnInputOrScroll() {
  StopTask();
}

void BlankScreenDetector::StopTask() {
  detection_task_.Stop();
  current_task_index_ = 0;
}

void BlankScreenDetector::GenerateTaskDelays(
    const std::vector<double>& task_delays) {
  task_delays_ms_ = {};
  for (auto task_delay : task_delays) {
    if (task_delay > 0.0f) {
      task_delays_ms_.emplace_back(
          static_cast<int32_t>(task_delay * SECOND_TO_MILLISECOND));
    }
  }
  std::sort(task_delays_ms_.begin(), task_delays_ms_.end());
  if (!task_delays_ms_.size()) {
    task_delays_ms_ = default_task_delays_ms_;
  }
}

void BlankScreenDetector::DetectBlankScreen(
    const WTF::String& url,
    const std::vector<double>& detection_timing,
    const std::vector<int32_t>& detection_methods,
    int32_t contentful_nodes_count_threshold) {
  LOG(INFO) << "BlankScreenDetector::DetectBlankScreen start";
  task_timestamp_ = base::TimeTicks::Now();
  StopTask();
  url_ = url;
  contentful_nodes_count_threshold_ = contentful_nodes_count_threshold;
  GenerateDetectionMethods(detection_methods);
  GenerateTaskDelays(detection_timing);
  ScheduleNextTask();
}

void BlankScreenDetector::GenerateDetectionMethods(
    const std::vector<int32_t>& detection_methods) {
  detection_methods_ = {};
  for (auto method : detection_methods) {
    if (method ==
        static_cast<int32_t>(
            BlankScreenDetectionMethod::DETECTION_CONTENTFUL_NODES_SEVENTEEN)) {
      detection_methods_.insert(
          static_cast<BlankScreenDetectionMethod>(method));
    }
  }
  if (!detection_methods_.size()) {
    detection_methods_ = {
        BlankScreenDetectionMethod::DETECTION_CONTENTFUL_NODES_SEVENTEEN};
  }
}

void BlankScreenDetector::ScheduleNextTask() {
  if (current_task_index_ < task_delays_ms_.size()) {
    base::TimeDelta current_time_delta =
        base::TimeTicks::Now() - task_timestamp_;
    auto current_delay_ms = task_delays_ms_[current_task_index_] -
                            current_time_delta.InMilliseconds();
    current_task_index_++;
    if (current_delay_ms > 0) {
      detection_task_.Start(
          FROM_HERE, base::Milliseconds(current_delay_ms),
          base::BindOnce(&BlankScreenDetector::RunDetectionTask,
                         weak_factory_.GetWeakPtr()));
    } else {
      ScheduleNextTask();
    }
  }
}

void BlankScreenDetector::GetPaintRects() {
  if (!local_frame_ || !local_frame_->View() ||
      !local_frame_->View()
           ->GetPaintTimingDetector()
           .GetFirstScreenCalculator()) {
    return;
  }
  paint_rects_ = {};
  local_frame_->View()
      ->GetPaintTimingDetector()
      .GetFirstScreenCalculator()
      ->GetPaintRects(paint_rects_);
}

int32_t BlankScreenDetector::CalculateContentfulCount() {
  if (!local_frame_ || !local_frame_->View() || !local_frame_->GetDocument() ||
      local_frame_->GetDocument()->Url().GetString().GetString() != url_) {
    return 0;
  }
  GetPaintRects();
  int32_t contentful_point_count = 0;
  for (auto& test_point : test_points_) {
    for (auto& paint_rect : paint_rects_) {
      if (paint_rect.Contains(test_point)) {
        contentful_point_count++;
        break;
      }
    }
  }
  return contentful_point_count;
}

void BlankScreenDetector::RunDetectionTask() {
  LOG(INFO)
      << "BlankScreenDetector::RunDetectionTask start current_task_index_ "
      << current_task_index_;
  int32_t contentful_point_count = 0;
  if (GenerateTestPoints()) {
    contentful_point_count = CalculateContentfulCount();
  }
  paint_rects_ = {};
  test_points_ = {};
  if (contentful_point_count <= contentful_nodes_count_threshold_) {
    if (current_task_index_ < task_delays_ms_.size()) {
      ScheduleNextTask();
    } else {
      DetectedBlankScreenReason result_reason =
          DetectedBlankScreenReason::NO_CONTENTFUL_NODES;
      int32_t result_count = 0;
      if (contentful_nodes_count_threshold_ && contentful_point_count) {
        result_reason =
            DetectedBlankScreenReason::SUB_THRESHOLD_CONTENTFUL_NODES;
        result_count = contentful_point_count;
      }
      if (local_frame_) {
        LOG(INFO) << "BlankScreenDetector::RunDetectionTask blank screen "
                     "detected result_count "
                  << result_count;
        local_frame_->OnDetectedBlankScreen(
            url_, static_cast<int32_t>(result_reason), result_count);
      }
    }
  }
  LOG(INFO) << "BlankScreenDetector::RunDetectionTask end current_task_index_ "
            << current_task_index_ << " contentful_point_count "
            << contentful_point_count;
}

bool BlankScreenDetector::GenerateTestPoints() {
  if (!local_frame_ || !local_frame_->View()) {
    return false;
  }

  int32_t width = static_cast<int32_t>(local_frame_->View()->ViewportWidth());
  int32_t height = static_cast<int32_t>(local_frame_->View()->ViewportHeight());

  if (width <= 0 || height <= 0) {
    return false;
  }

  if (local_frame_->GetDocument()) {
    auto viewport_description =
        local_frame_->GetDocument()->GetViewportData().GetViewportDescription();
    if (viewport_description.zoom > 0.0f) {
      width /= viewport_description.zoom;
      height /= viewport_description.zoom;
    }
  }
  test_points_ = {};
  for (auto method : detection_methods_) {
    switch (method) {
      case BlankScreenDetectionMethod::DETECTION_CONTENTFUL_NODES_SEVENTEEN:
        GenerateTestPointsByMethodDetection17(width, height);
        break;
      default:
        break;
    }
  }
  return true;
}

void BlankScreenDetector::GenerateTestPointsByMethodDetection17(
    int32_t width,
    int32_t height) {
  test_points_.emplace_back(width / DIVIDED_BY_TWO, height / DIVIDED_BY_TWO);
  for (int32_t x = 1; x < DIVIDED_BY_FIVE; x++) {
    for (int32_t y = 1; y < DIVIDED_BY_FIVE; y++) {
      test_points_.emplace_back(width * x / DIVIDED_BY_FIVE,
                                height * y / DIVIDED_BY_FIVE);
    }
  }
}
}  // namespace blink