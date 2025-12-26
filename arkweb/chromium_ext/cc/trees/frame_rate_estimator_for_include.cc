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

#ifndef CC_TREES_FRAME_RATE_ESTIMATOR_H_
#error "must be in include form CC_TREES_FRAME_RATE_ESTIMATOR_H_"
#endif

namespace cc {

#if BUILDFLAG(ARKWEB_THROTTLE_FRAME)
void FrameRateEstimator::DidNotProduceFrameWithReason(FrameSkippedReason reason) {
    if (reason == FrameSkippedReason::kNoDamage) {
        num_of_consecutive_frames_with_min_delta_ = 0u;
        ++num_no_damage_did_not_produce_frame_;
        if (num_no_damage_did_not_produce_frame_ > 4 && !begin_frame_throttle_mode_) {
            begin_frame_throttle_mode_ = true;
        }
    } else {
        begin_frame_throttle_mode_ = false;
        num_of_consecutive_frames_with_min_delta_ = 0u;
        num_no_damage_did_not_produce_frame_ = 0u;
    }
}

void FrameRateEstimator::ClearBeginFrameThrottleSettings() {
    num_no_damage_did_not_produce_frame_ = 0u;
    begin_frame_throttle_mode_ = false;
}
#endif

} // namespace cc