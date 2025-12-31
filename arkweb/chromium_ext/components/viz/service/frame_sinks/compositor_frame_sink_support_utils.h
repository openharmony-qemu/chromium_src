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
#ifndef COMPONENTS_VIZ_SERVICE_FRAME_SINKS_H_
#define COMPONENTS_VIZ_SERVICE_FRAME_SINKS_H_

#include "arkweb/build/features/features.h"
#include "components/viz/service/frame_sinks/compositor_frame_sink_support.h"
#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
#include "base/containers/queue.h"
#endif
#include "base/memory/raw_ptr.h"

namespace viz {

#if BUILDFLAG(ARKWEB_FLING) && BUILDFLAG(ARKWEB_SLIDE)
static uint32_t g_firstScrollingFrame = 0;
static bool g_isScrolling = false;
constexpr int g_scrolledFrameCount = 30;
constexpr int g_milliSecondSize = 1000;
#endif

class CompositorFrameSinkSupport;

class CompositorFrameSinkSupportUtils {
public:
	raw_ptr<CompositorFrameSinkSupport> compositorFrameSinkSupport;
	CompositorFrameSinkSupportUtils(CompositorFrameSinkSupport* support);

#if BUILDFLAG(ARKWEB_FLING) && BUILDFLAG(ARKWEB_SLIDE)
	void flingSlideMaybeSubmitCompositorFrame(CompositorFrame& frame);
#endif

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
	void maximizeResizeCompositorFrame(Surface* current_surface, const gfx::Size& frame_size);
#endif

#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
	void videoLtpoMaybeSubmitCompositorFrame();
#endif

#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
	int GetFrameRate();
	int64_t GetCurrentTimeStampMS();
#endif

#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
    void OnSetBypassVsyncCondition(int32_t condition);
#endif

#if BUILDFLAG(ARKWEB_THROTTLE_FRAME)
void UpdateThrottleMode(bool is_enable);
#endif
private:
#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
  base::queue<int64_t> frames_time_stamps_;
  const int kMaxFrameCount = 30;
  int estimated_frame_rate_ = 0;
#endif
};
}
#endif