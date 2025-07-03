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
#include "arkweb/chromium_ext/components/viz/service/frame_sinks/compositor_frame_sink_support_utils.h"

#include "arkweb/build/features/features.h"
#include "base/trace_event/common/trace_event_common.h"
#include "components/viz/service/frame_sinks/compositor_frame_sink_support.h"
#include "components/viz/service/surfaces/surface.h"

namespace viz {

CompositorFrameSinkSupportUtils::CompositorFrameSinkSupportUtils(CompositorFrameSinkSupport* support)
{
  this->compositorFrameSinkSupport = support;
}

#if BUILDFLAG(ARKWEB_FLING) && BUILDFLAG(ARKWEB_SLIDE)
void CompositorFrameSinkSupportUtils::flingSlideMaybeSubmitCompositorFrame(CompositorFrame& frame)
{
  if (g_isScrolling == false && frame.metadata.is_scrolling == true) {
    g_firstScrollingFrame = frame.metadata.frame_token;
    g_isScrolling = true;
  }

  if (g_isScrolling == true && frame.metadata.is_scrolling == false &&
      (uint32_t)(frame.metadata.frame_token - g_firstScrollingFrame) >=
          g_scrolledFrameCount) {
    g_isScrolling = false;
  }
}
#endif

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
void CompositorFrameSinkSupportUtils::maximizeResizeCompositorFrame(
    Surface* current_surface,
    const gfx::Size& frame_size)
{
  if (compositorFrameSinkSupport->surface_manager_ && current_surface &&
      !compositorFrameSinkSupport->is_root_) {
    compositorFrameSinkSupport->surface_manager_->ReenableSwapCheck(
        current_surface->surface_id(), frame_size.width(), frame_size.height());
  }
}
#endif

#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
void CompositorFrameSinkSupportUtils::videoLtpoMaybeSubmitCompositorFrame()
{
  int64_t frame_ts = GetCurrentTimeStampMS();
  frames_time_stamps_.push(frame_ts);
  if (frames_time_stamps_.size() >
      kMaxFrameCount) {
    frames_time_stamps_.pop();
  }
  int64_t diff_time = frames_time_stamps_.back() -
                      frames_time_stamps_.front();
  if (diff_time != 0) {
    estimated_frame_rate_ =
        g_milliSecondSize *
        frames_time_stamps_.size() / diff_time;
  }
}
#endif

#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
void CompositorFrameSinkSupportUtils::OnSetBypassVsyncCondition(int32_t condition) {
  compositorFrameSinkSupport->begin_frame_source_->OnSetBypassVsyncCondition(condition);
}
#endif

#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
int CompositorFrameSinkSupportUtils::GetFrameRate()
{
  return estimated_frame_rate_;
}

int64_t CompositorFrameSinkSupportUtils::GetCurrentTimeStampMS()
{
  auto currentTime = std::chrono::system_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::milliseconds>(currentTime)
      .count();
}
#endif
}  // namespace viz
