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

#ifndef COMPONENTS_VIZ_SERVICE_FRAME_SINKS_COMPOSITOR_FRAME_SINK_IMPL_UTIL_H_
#define COMPONENTS_VIZ_SERVICE_FRAME_SINKS_COMPOSITOR_FRAME_SINK_IMPL_UTIL_H_

#include "arkweb/build/features/features.h"

#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
#include "components/viz/service/frame_sinks/compositor_frame_sink_support.h"
#endif

namespace viz {
class CompositorFrameSinkImpl;

class CompositorFrameSinkImplUtil {
 public:
 CompositorFrameSinkImpl* compositorFrameSinkImpl_;
 CompositorFrameSinkImplUtil(CompositorFrameSinkImpl* impl);
#if BUILDFLAG(ARKWEB_REPORT_SYS_EVENT)
  void SubmitCompositorFrameUtils(const CompositorFrameMetadata& metadata);
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
  void ReportKeyThreadIdsUtils(const std::vector<int32_t>& thread_ids,
      int32_t process_id,
      bool is_created);
#endif

#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
  int GetFrameRate();
#endif

#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
  void OnSetBypassVsyncCondition(int32_t condition);
#endif
};

} // namespace viz

#endif  // COMPONENTS_VIZ_SERVICE_FRAME_SINKS_COMPOSITOR_FRAME_SINK_IMPL_UTIL_H_