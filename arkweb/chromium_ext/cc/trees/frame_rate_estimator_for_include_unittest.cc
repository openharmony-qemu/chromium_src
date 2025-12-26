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
#define private public
#include "cc/trees/frame_rate_estimator.h"
#undef private

#include "base/test/scoped_feature_list.h"
#include "base/time/simple_test_tick_clock.h"
#include "base/test/test_simple_task_runner.h"
#include "cc/layers/layer_impl.h"
#include "cc/test/layer_tree_impl_test_base.h"
#include "cc/test/fake_video_frame_provider.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

#include "frame_rate_estimator_for_include.cc"

using testing::_;

namespace cc {
namespace{
constexpr uint32_t NUMBER_0 = 0u;
constexpr uint32_t NUMBER_1 = 1u;
constexpr uint32_t NUMBER_5 = 5u;
class FrameRateEstimatorForIncludeTest : public testing::Test {
 public:
  FrameRateEstimatorForIncludeTest() = default;
  ~FrameRateEstimatorForIncludeTest() override = default;

  void SetUp() override {
    scoped_feature_list_.InitWithFeatures(
        /* enabled_features*/ {features::
                                   kThrottleFrameRateOnManyDidNotProduceFrame},
        /* disabled_features*/ {});
    task_runner_ = base::MakeRefCounted<base::TestSimpleTaskRunner>();
    estimator_ = std::make_unique<FrameRateEstimator>(task_runner_.get());
  }

  void TearDown() override {
    estimator_.reset();
    task_runner_.reset();
  }
 protected:
  scoped_refptr<base::TestSimpleTaskRunner> task_runner_;
  std::unique_ptr<FrameRateEstimator> estimator_;
  base::test::ScopedFeatureList scoped_feature_list_;
};

#if BUILDFLAG(ARKWEB_THROTTLE_FRAME)
TEST_F(FrameRateEstimatorForIncludeTest, DidNotProduceFrameWithkReason) {
    estimator_->DidNotProduceFrameWithReason(FrameSkippedReason::kRecoverLatency);
    ASSERT_EQ(estimator_->num_no_damage_did_not_produce_frame_since_last_draw_, NUMBER_0);
}

TEST_F(FrameRateEstimatorForIncludeTest, DidNotProduceFrameWithkNoDamage) {
    estimator_->DidNotProduceFrameWithReason(FrameSkippedReason::kNoDamage);
    ASSERT_EQ(estimator_->num_no_damage_did_not_produce_frame_since_last_draw_, NUMBER_1);
}

TEST_F(FrameRateEstimatorForIncludeTest, SetBeginFrameThrottleModeToTrue) {
    estimator_->num_no_damage_did_not_produce_frame_since_last_draw_ = NUMBER_5;
    estimator_->begin_frame_throttle_mode_ = false;
    estimator_->DidNotProduceFrameWithReason(FrameSkippedReason::kNoDamage);
    ASSERT_EQ(estimator_->begin_frame_throttle_mode_, true);
}

TEST_F(FrameRateEstimatorForIncludeTest, KeepBeginFrameThrottleMode) {
    estimator_->num_no_damage_did_not_produce_frame_since_last_draw_ = NUMBER_1;
    estimator_->begin_frame_throttle_mode_ = false;
    estimator_->DidNotProduceFrameWithReason(FrameSkippedReason::kNoDamage);
    ASSERT_EQ(estimator_->begin_frame_throttle_mode_, false);
}

TEST_F(FrameRateEstimatorForIncludeTest, KeepBeginFrameThrottleMode2) {
    estimator_->num_no_damage_did_not_produce_frame_since_last_draw_ = NUMBER_5;
    estimator_->begin_frame_throttle_mode_ = true;
    estimator_->DidNotProduceFrameWithReason(FrameSkippedReason::kNoDamage);
    ASSERT_EQ(estimator_->begin_frame_throttle_mode_, true);
}

TEST_F(FrameRateEstimatorForIncludeTest, ClearBeginFrameThrottleSettings) {
    estimator_->begin_frame_throttle_mode_ = true;
    estimator_->ClearBeginFrameThrottleSettings();
    ASSERT_EQ(estimator_->begin_frame_throttle_mode_, false);
}

#endif // ARKWEB_THROTTLE_FRAME
}
} // cc