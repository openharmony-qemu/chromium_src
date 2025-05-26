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

#ifndef COMPONENTS_VIZ_SERVICE_FRAME_SINKS_FRAME_SINK_MANAGER_IMPL_UTILS_H
#define COMPONENTS_VIZ_SERVICE_FRAME_SINKS_FRAME_SINK_MANAGER_IMPL_UTILS_H

#include <string>

#include "arkweb/build/features/features.h"
#include "components/viz/common/surfaces/frame_sink_id.h"

namespace viz {
class FrameSinkManagerImpl;

class FrameSinkManagerImplUtils {
public:
  FrameSinkManagerImpl* frameSinkManagerImpl;
  FrameSinkManagerImplUtils(FrameSinkManagerImpl* managerImplUtils);
#if BUILDFLAG(ARKWEB_OCCLUDED_OPT)
  void EvictFrameBackBuffers(const FrameSinkId& root_frame_sink_id,
                             bool invisible);
  void SetEnableLowerFrameRate(bool enabled, const FrameSinkId& frame_sink_id);
  void SetEnableHalfFrameRate(bool enabled, const FrameSinkId& frame_sink_id);
#endif
#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
  void UpdateVSyncFrequency(const FrameSinkId& frame_sink_id,
                            uint32_t client_id);
  void ResetVSyncFrequency(const FrameSinkId& frame_sink_id);
#endif
#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
  void RestoreRenderFit(const FrameSinkId& frame_sink_id);
#endif  // ARKWEB_MAXIMIZE_RESIZE
#if BUILDFLAG(ARKWEB_PIP)
void SetPipActive(bool active, const FrameSinkId& frame_sink_id);
#endif
};
}  // namespace viz
#endif
