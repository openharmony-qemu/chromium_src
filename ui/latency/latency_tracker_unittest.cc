/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
 
#include <memory>
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

#define private public
#include "ui/latency/latency_tracker.h"
#undef private

namespace ui {
class LatencyTrackerTest : public ::testing::Test {
 public:
  void SetUp() override {
    latency_tracker_ptr = std::make_unique<LatencyTracker>();
    latency_info_ptr = std::make_unique<LatencyInfo>();
  }

  void TearDown() override {
    latency_tracker_ptr.reset();
    latency_info_ptr.reset();
  }

  std::unique_ptr<LatencyTracker> latency_tracker_ptr;
  std::unique_ptr<LatencyInfo> latency_info_ptr;
};

TEST_F(LatencyTrackerTest, OnGpuSwapBuffersCompleted_001) {
  latency_info_ptr->AddLatencyNumber(INPUT_EVENT_LATENCY_FRAME_SWAP_COMPONENT);
  latency_info_ptr->AddLatencyNumber(INPUT_EVENT_GPU_SWAP_BUFFER_COMPONENT);
  latency_info_ptr->AddLatencyNumber(INPUT_EVENT_LATENCY_BEGIN_RWH_COMPONENT);
  latency_info_ptr->AddLatencyNumber(
      INPUT_EVENT_LATENCY_FIRST_SCROLL_UPDATE_ORIGINAL_COMPONENT);
  std::vector<ui::LatencyInfo> latency_infos = {*latency_info_ptr};
  bool top_controls_visible_height_changed = true;
  latency_tracker_ptr->OnGpuSwapBuffersCompleted(
      std::move(latency_infos), top_controls_visible_height_changed);
  EXPECT_TRUE(latency_info_ptr->FindLatency(
      INPUT_EVENT_LATENCY_FIRST_SCROLL_UPDATE_ORIGINAL_COMPONENT, nullptr));
}

TEST_F(LatencyTrackerTest, OnGpuSwapBuffersCompleted_002) {
  latency_info_ptr->AddLatencyNumber(INPUT_EVENT_LATENCY_FRAME_SWAP_COMPONENT);
  latency_info_ptr->AddLatencyNumber(INPUT_EVENT_GPU_SWAP_BUFFER_COMPONENT);
  latency_info_ptr->AddLatencyNumber(INPUT_EVENT_LATENCY_BEGIN_RWH_COMPONENT);
  latency_info_ptr->AddLatencyNumber(
      INPUT_EVENT_LATENCY_SCROLL_UPDATE_ORIGINAL_COMPONENT);
  std::vector<ui::LatencyInfo> latency_infos = {*latency_info_ptr};
  bool top_controls_visible_height_changed = true;
  latency_tracker_ptr->OnGpuSwapBuffersCompleted(
      std::move(latency_infos), top_controls_visible_height_changed);
  EXPECT_TRUE(latency_info_ptr->FindLatency(
      INPUT_EVENT_LATENCY_SCROLL_UPDATE_ORIGINAL_COMPONENT, nullptr));
}

TEST_F(LatencyTrackerTest, OnGpuSwapBuffersCompleted_003) {
  latency_info_ptr->AddLatencyNumber(INPUT_EVENT_LATENCY_FRAME_SWAP_COMPONENT);
  latency_info_ptr->AddLatencyNumber(INPUT_EVENT_GPU_SWAP_BUFFER_COMPONENT);
  latency_info_ptr->AddLatencyNumber(INPUT_EVENT_LATENCY_BEGIN_RWH_COMPONENT);
  std::vector<ui::LatencyInfo> latency_infos = {*latency_info_ptr};
  bool top_controls_visible_height_changed = true;
  latency_tracker_ptr->OnGpuSwapBuffersCompleted(
      std::move(latency_infos), top_controls_visible_height_changed);
  EXPECT_FALSE(latency_info_ptr->FindLatency(
      INPUT_EVENT_LATENCY_FIRST_SCROLL_UPDATE_ORIGINAL_COMPONENT, nullptr));
  EXPECT_FALSE(latency_info_ptr->FindLatency(
      INPUT_EVENT_LATENCY_SCROLL_UPDATE_ORIGINAL_COMPONENT, nullptr));
}
}  // namespace ui
