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
#include "base/logging.h"

namespace {
constexpr int32_t kMaxVsyncInterval = 6;
}

namespace blink {
void VideoFrameSubmitter::SetHasNativeLayer(bool has_native_layer) {
  LOG(DEBUG) << "[NativeEmbed] VideoFrameSubmitter::SetHasNativeLayer " << has_native_layer;
  if (!resource_provider_) {
    return;
  }
  has_native_layer_ = has_native_layer;
}

void VideoFrameSubmitter::StartRenderingForSameLayer()
{
  if (!has_native_layer_ ) {
    return;
  }
  TRACE_EVENT0("media", "VideoFrameSubmitter::StartRendering");
  vsync_period_cnt_without_submit_ = 0;
  if (!start_begin_frame_) {
    if (compositor_frame_sink_) {
        compositor_frame_sink_->SetNeedsBeginFrame(true);
    }
    start_begin_frame_ =  true;
  }
}

void VideoFrameSubmitter::StopRenderingForSameLayer()
{
  if (!has_native_layer_ ) {
    return;
  }
  if (!task_runner_) {
    return;
  }
  task_runner_->PostTask(
        FROM_HERE, base::BindOnce(&VideoFrameSubmitter::StopRenderingForSameLayerImpl,
                                  weak_ptr_factory_.GetWeakPtr()));
}

void VideoFrameSubmitter::StopRenderingForSameLayerImpl()
{
  TRACE_EVENT1("media", "VideoFrameSubmitter::StopRendering", "vsync_period_cnt_without_submit",
    vsync_period_cnt_without_submit_);
  if (++vsync_period_cnt_without_submit_ > kMaxVsyncInterval) {
    if (start_begin_frame_) {
      start_begin_frame_ = false;
      if (compositor_frame_sink_) {
        compositor_frame_sink_->SetNeedsBeginFrame(false);
      }
    }
    vsync_period_cnt_without_submit_ = 0;
  }
}

void VideoFrameSubmitter::UpdateDroppedFrameMetrics(const viz::BeginFrameArgs& args) {
  if (args.interval <= base::TimeDelta::Min() || args.interval.is_zero()) {
    return;
  }

  if (!is_first_frame_) {
    base::TimeTicks cur_frame_time = args.frame_time + args.interval;
    dropped_frame_count_ = (cur_frame_time - last_frame_time_).IntDiv(args.interval);
    dropped_frame_duration_ = (cur_frame_time - last_frame_time_).InMilliseconds();
    if (!!dropped_frame_count_)
      should_report_frame_dropped_ = true;
  } else {
    is_first_frame_ = false;
  }
  last_frame_time_ = args.frame_time + 2 * args.interval;
}

void VideoFrameSubmitter::SubmitDroppedFrameMetricsToMetadata(viz::CompositorFrame& compositor_frame) {
  if (should_report_frame_dropped_) {
    compositor_frame.metadata.dropped_frame_count = dropped_frame_count_;
    compositor_frame.metadata.dropped_frame_duration = dropped_frame_duration_;
    should_report_frame_dropped_ = false;
  }
}

}  // namespace blink