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

#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#define private public
#include "arkweb/chromium_ext/third_party/blink/renderer/core/paint/timing/blank_screen_detector.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/core/paint/timing/first_screen_calculator.h"
#include "third_party/blink/renderer/core/frame/frame_test_helpers.h"
#include "third_party/blink/renderer/core/paint/timing/paint_timing_detector.h"
#include "third_party/blink/renderer/core/testing/core_unit_test_helper.h"
#include "third_party/blink/renderer/core/testing/dummy_page_holder.h"

namespace blink {
namespace {

class BlankScreenDetectorTest : public RenderingTest {
 protected:
  void SetUp() override {
    EnableCompositing();
    RenderingTest::SetUp();
    web_view_helper_.Initialize();
    dummy_page_holder_ = std::make_unique<DummyPageHolder>(gfx::Size(800, 600));
    local_frame_ = &dummy_page_holder_->GetFrame();
    detector_ = std::make_unique<BlankScreenDetector>(local_frame_.get());
  }
  void TearDown() override {}

  std::unique_ptr<DummyPageHolder> dummy_page_holder_;
  raw_ptr<LocalFrame> local_frame_;
  std::unique_ptr<BlankScreenDetector> detector_;
  frame_test_helpers::WebViewHelper web_view_helper_;
};

TEST_F(BlankScreenDetectorTest, OnInputOrScroll) {
  detector_->OnInputOrScroll();
}

TEST_F(BlankScreenDetectorTest, StopTask) {
  detector_->current_task_index_ = 1;
  detector_->StopTask();
  EXPECT_EQ(detector_->current_task_index_, 0);
}

TEST_F(BlankScreenDetectorTest, GenerateTaskDelays) {
  std::vector<double> task_delays = {-1.0, 2.0, 1.0};
  detector_->GenerateTaskDelays(task_delays);
  EXPECT_EQ(detector_->task_delays_ms_.size(), 2);

  task_delays = {};
  detector_->GenerateTaskDelays(task_delays);
  EXPECT_EQ(detector_->task_delays_ms_.size(), 3);
}

TEST_F(BlankScreenDetectorTest, DetectBlankScreen) {
  std::vector<double> task_delays = {-1.0, 2.0, 1.0};
  std::vector<int32_t> detection_methods = {0, 1, 2};
  detector_->DetectBlankScreen("1", task_delays, detection_methods, 0);
}

TEST_F(BlankScreenDetectorTest, GenerateDetectionMethods) {
  std::vector<int32_t> detection_methods = {0, 1, 2};
  detector_->GenerateDetectionMethods(detection_methods);
  EXPECT_EQ(detector_->detection_methods_.size(), 1);

  detection_methods = {};
  detector_->GenerateDetectionMethods(detection_methods);
  EXPECT_EQ(detector_->detection_methods_.size(), 1);
}

TEST_F(BlankScreenDetectorTest, ScheduleNextTask) {
  detector_->current_task_index_ = 0;
  detector_->task_delays_ms_ = {1000};
  detector_->task_timestamp_ = base::TimeTicks::Now() - base::Milliseconds(100);
  detector_->ScheduleNextTask();
  EXPECT_EQ(detector_->current_task_index_, 1);

  detector_->current_task_index_ = 0;
  detector_->task_timestamp_ = base::TimeTicks::Now() + base::Milliseconds(100);
  detector_->ScheduleNextTask();
  EXPECT_EQ(detector_->current_task_index_, 1);

  detector_->current_task_index_ = 1;
  detector_->ScheduleNextTask();
  EXPECT_EQ(detector_->current_task_index_, 1);
}

TEST_F(BlankScreenDetectorTest, GetPaintRects) {
  detector_->paint_rects_ = {};
  local_frame_->View()
      ->GetPaintTimingDetector()
      .GetFirstScreenCalculator()
      ->image_rects_map_.insert(
          {0, {gfx::Rect(5, 5, 5, 5), base::TimeTicks::Now()}});
  local_frame_->View()
      ->GetPaintTimingDetector()
      .GetFirstScreenCalculator()
      ->image_rects_map_.insert(
          {1, {gfx::Rect(5, 5, 5, 5), base::TimeTicks::Now()}});
  detector_->GetPaintRects();
  EXPECT_EQ(detector_->paint_rects_.size(), 2);

  detector_->paint_rects_ = {};
  local_frame_->View()
      ->GetPaintTimingDetector()
      .GetFirstScreenCalculator()
      ->image_rects_map_.insert(
          {0, {gfx::Rect(5, 5, 5, 5), base::TimeTicks::Now()}});
  local_frame_->View()
      ->GetPaintTimingDetector()
      .GetFirstScreenCalculator()
      ->image_rects_map_.insert(
          {1, {gfx::Rect(5, 5, 5, 5), base::TimeTicks::Now()}});
  detector_->local_frame_ = nullptr;
  detector_->GetPaintRects();
  EXPECT_EQ(detector_->paint_rects_.size(), 0);
}

TEST_F(BlankScreenDetectorTest, CalculateContentfulCount) {
  detector_->test_points_.emplace_back(gfx::Point(10, 10));
  detector_->test_points_.emplace_back(gfx::Point(20, 20));
  local_frame_->View()
      ->GetPaintTimingDetector()
      .GetFirstScreenCalculator()
      ->image_rects_map_.insert(
          {1, {gfx::Rect(5, 5, 10, 10), base::TimeTicks::Now()}});
  EXPECT_EQ(detector_->CalculateContentfulCount(), 1);

  detector_->url_ = "1";
  EXPECT_EQ(detector_->CalculateContentfulCount(), 0);

  detector_->url_ = "";
  detector_->local_frame_ = nullptr;
  EXPECT_EQ(detector_->CalculateContentfulCount(), 0);
}

TEST_F(BlankScreenDetectorTest, RunDetectionTask) {
  detector_->test_points_.emplace_back(gfx::Point(10, 10));
  detector_->test_points_.emplace_back(gfx::Point(20, 20));
  detector_->paint_rects_.emplace_back(gfx::Rect(5, 5, 10, 10));
  detector_->contentful_nodes_count_threshold_ = 1;
  detector_->task_delays_ms_ = {1000};
  detector_->current_task_index_ = 0;
  detector_->RunDetectionTask();
  EXPECT_EQ(detector_->current_task_index_, 1);

  detector_->current_task_index_ = 1;
  detector_->RunDetectionTask();
  EXPECT_EQ(detector_->current_task_index_, 1);

  detector_->current_task_index_ = 1;
  detector_->contentful_nodes_count_threshold_ = 0;
  detector_->RunDetectionTask();
  EXPECT_EQ(detector_->current_task_index_, 1);

  detector_->current_task_index_ = 1;
  detector_->contentful_nodes_count_threshold_ = 1;
  detector_->paint_rects_ = {};
  detector_->RunDetectionTask();
  EXPECT_EQ(detector_->current_task_index_, 1);

  detector_->local_frame_ = nullptr;
  detector_->current_task_index_ = 1;
  detector_->RunDetectionTask();
  EXPECT_EQ(detector_->current_task_index_, 1);
}

TEST_F(BlankScreenDetectorTest, GenerateTestPoints) {
  detector_->test_points_ = {};
  detector_->detection_methods_.insert(
      BlankScreenDetectionMethod::DETECTION_CONTENTFUL_NODES_SEVENTEEN);
  EXPECT_EQ(detector_->GenerateTestPoints(), true);
  EXPECT_EQ(detector_->test_points_.size(), 17);

  detector_->local_frame_ = nullptr;
  detector_->test_points_ = {};
  EXPECT_EQ(detector_->GenerateTestPoints(), false);
  EXPECT_EQ(detector_->test_points_.size(), 0);
}

TEST_F(BlankScreenDetectorTest, GenerateTestPointsByMethodDetection17) {
  detector_->test_points_ = {};
  detector_->GenerateTestPointsByMethodDetection17(0, 0);
  EXPECT_EQ(detector_->test_points_.size(), 17);
}
}  // namespace
}  // namespace blink