
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

#ifndef COMPONENTS_VIZ_HOST_HOST_FRAME_SINK_MANAGER_UTILS_H_
#define COMPONENTS_VIZ_HOST_HOST_FRAME_SINK_MANAGER_UTILS_H_

#include "components/viz/host/host_frame_sink_manager.h"
#include "arkweb/build/features/features.h"

namespace viz {
class HostFrameSinkManagerUtils {
public:
    HostFrameSinkManager* hostFrameSinkManager;
    HostFrameSinkManagerUtils(HostFrameSinkManager* manager);

#if BUILDFLAG(ARKWEB_OCCLUDED_OPT)
    void SetEnableLowerFrameRate(bool enabled, const FrameSinkId& frame_sink_id);
    void SetEnableHalfFrameRate(bool enabled, const FrameSinkId& frame_sink_id);
    void EvictFrameBackBuffers(const FrameSinkId& frame_sink_id, bool invisible);
#endif

#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
    void UpdateVSyncFrequency(const FrameSinkId& frame_sink_id);
    void ResetVSyncFrequency(const FrameSinkId& frame_sink_id);
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
    void SendInternalBeginFrame(const FrameSinkId& id);
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_PIP)
    void SetPipActive(bool active, const FrameSinkId& frame_sink_id);
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  void ClearBlanklessSnapshotInfo(uint64_t blankless_key);
#endif

void UtilsRestoreRenderFit(uint32_t client_id, uint32_t sink_id);
};

}  // namespace viz

#endif  // COMPONENTS_VIZ_HOST_HOST_FRAME_SINK_MANAGER_UTILS_H_