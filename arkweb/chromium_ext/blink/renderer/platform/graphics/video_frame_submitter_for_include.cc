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
  resource_provider_->SetHasNativeLayer(has_native_layer);
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
}  // namespace blink