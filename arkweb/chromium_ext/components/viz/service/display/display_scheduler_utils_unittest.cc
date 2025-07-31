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

#include "arkweb/chromium_ext/components/viz/service/display/display_scheduler_utils.h"

#include "arkweb/build/features/features.h"
#include "base/containers/contains.h"
#include "base/memory/raw_ptr.h"
#include "base/task/single_thread_task_runner.h"
#include "base/test/null_task_runner.h"
#include "cc/test/scheduler_test_common.h"
#include "components/viz/common/features.h"
#include "components/viz/common/frame_sinks/begin_frame_args.h"
#include "components/viz/common/surfaces/surface_info.h"
#include "components/viz/service/display/display.h"
#include "components/viz/service/display/display_resource_provider_software.h"
#include "components/viz/service/display_embedder/server_shared_bitmap_manager.h"
#include "components/viz/test/begin_frame_args_test.h"
#include "components/viz/test/fake_external_begin_frame_source.h"
#include "gpu/command_buffer/service/scheduler.h"
#include "gpu/command_buffer/service/shared_image/shared_image_manager.h"
#include "gpu/command_buffer/service/sync_point_manager.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace viz {

class TestDisplayDamageTracker : public DisplayDamageTracker {
 public:
  using DisplayDamageTracker::DisplayDamageTracker;
  ~TestDisplayDamageTracker() override = default;

  void SetRootFrameMissingForTest(bool missing) {
    SetRootFrameMissing(missing);
  }

  bool SurfaceHasUnackedFrame(const SurfaceId& surface_id) const override {
    return false;
  }

  void UpdateRootFrameMissing() override {
    SetRootFrameMissing(false);
  }
};

class FakeDisplaySchedulerClient : public DisplaySchedulerClient {
 public:
  explicit FakeDisplaySchedulerClient(TestDisplayDamageTracker* damage_tracker)
      : damage_tracker_(std::move(damage_tracker)) {}

  ~FakeDisplaySchedulerClient() override = default;

  bool DrawAndSwap(const DrawAndSwapParams& params) override {
    return false;
  }

  void DidFinishFrame(const BeginFrameAck& ack) override {}

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
  void ReenableSwapCheck(const SurfaceId& surface_id,
                         int width,
                         int height) override {}
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  void SetClientId(const uint32_t client_id) override {
    client_id_ = client_id;
  }

  uint32_t GetClientId() {
    return client_id_;
  }
#endif

 protected:
  raw_ptr<TestDisplayDamageTracker> damage_tracker_ = nullptr;
  uint32_t client_id_ = 0;
};

class TestDisplayScheduler : public DisplayScheduler {
 public:
  TestDisplayScheduler(DisplayDamageTracker* damage_tracker,
                       BeginFrameSource* begin_frame_source,
                       SurfaceManager* surface_manager,
                       base::SingleThreadTaskRunner* task_runner,
                       int max_pending_swaps,
                       bool wait_for_all_surfaces_before_draw)
      : DisplayScheduler(begin_frame_source,
                         task_runner,
                         PendingSwapParams(max_pending_swaps),
                         nullptr,
                         wait_for_all_surfaces_before_draw) {
    SetDamageTracker(damage_tracker);
  }

  void ScheduleBeginFrameDeadline() override {
    DisplayScheduler::ScheduleBeginFrameDeadline();
  }
};

class DisplaySchedulerUtilsTest : public testing::Test {
 public:
  DisplaySchedulerUtilsTest()
      : fake_begin_frame_source_(0.f, false),
        task_runner_(new base::NullTaskRunner),
        surface_manager_(nullptr, 4u, 0),
        resource_provider_(&shared_bitmap_manager_,
                           &shared_image_manager_,
                           &sync_point_manager_,
                           &gpu_scheduler_),
        aggregator_(&surface_manager_, &resource_provider_, false),
        damage_tracker_(
            std::make_unique<TestDisplayDamageTracker>(&surface_manager_,
                                                       &aggregator_)),
        client_(damage_tracker_.get()) {}
  ~DisplaySchedulerUtilsTest() = default;

  void SetUp() override {
    scheduler_ = std::make_unique<TestDisplayScheduler>(
        damage_tracker_.get(), &fake_begin_frame_source_, &surface_manager_,
        task_runner_.get(), 1, false);
    damage_tracker_->SetRootFrameMissingForTest(false);
  }

  FakeExternalBeginFrameSource fake_begin_frame_source_;
  scoped_refptr<base::NullTaskRunner> task_runner_;
  SurfaceManager surface_manager_;
  ServerSharedBitmapManager shared_bitmap_manager_;
  gpu::SharedImageManager shared_image_manager_;
  gpu::SyncPointManager sync_point_manager_;
  gpu::Scheduler gpu_scheduler_{&sync_point_manager_};
  DisplayResourceProviderSoftware resource_provider_;
  SurfaceAggregator aggregator_;
  std::unique_ptr<TestDisplayDamageTracker> damage_tracker_;
  FakeDisplaySchedulerClient client_;
  std::unique_ptr<TestDisplayScheduler> scheduler_;
};

TEST_F(DisplaySchedulerUtilsTest, OnDisplayDamaged) {
  DisplaySchedulerUtils utils(scheduler_.get());
  const LocalSurfaceId local_id;
  const FrameSinkId sink_id_valid(1, 0);
  SurfaceId surface_id_valid(sink_id_valid, local_id);

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
  const FrameSinkId sink_id_invalid(0, 0);
  SurfaceId surface_id_invalid(sink_id_invalid, local_id);

  utils.ResetShouldFrameSubmissionBeforeDraw();
  EXPECT_FALSE(utils.wait_before_draw());
  utils.OnDisplayDamaged(surface_id_valid);
  EXPECT_FALSE(utils.wait_before_draw());

  utils.ResetShouldFrameSubmissionBeforeDraw();
  EXPECT_FALSE(utils.wait_before_draw());
  utils.OnDisplayDamaged(surface_id_invalid);
  EXPECT_FALSE(utils.wait_before_draw());

  utils.SetShouldFrameSubmissionBeforeDraw(true);
  EXPECT_TRUE(utils.wait_before_draw());
  utils.OnDisplayDamaged(surface_id_valid);
  EXPECT_FALSE(utils.wait_before_draw());

  utils.SetShouldFrameSubmissionBeforeDraw(true);
  EXPECT_TRUE(utils.wait_before_draw());
  utils.OnDisplayDamaged(surface_id_invalid);
  EXPECT_TRUE(utils.wait_before_draw());
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  utils.OnDisplayDamaged(surface_id_valid);
  EXPECT_EQ(client_.GetClientId(), 0u);
  scheduler_->SetClient(&client_);
  utils.OnDisplayDamaged(surface_id_valid);
  EXPECT_EQ(client_.GetClientId(),
            surface_id_valid.frame_sink_id().client_id());
#endif
}
}  // namespace viz
