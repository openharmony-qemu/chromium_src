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
#include "base/allocator/partition_allocator/src/partition_alloc/pointers/raw_ptr.h"

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
#include "components/viz/service/gl/gpu_service_impl.h"
#endif

namespace viz {
class FrameSinkManagerImpl;

class FrameSinkManagerImplUtils {
public:
  raw_ptr<FrameSinkManagerImpl> frameSinkManagerImpl;
  FrameSinkManagerImplUtils(FrameSinkManagerImpl* managerImplUtils);
#if BUILDFLAG(ARKWEB_OCCLUDED_OPT)
  void EvictFrameBackBuffers(const FrameSinkId& root_frame_sink_id);
  void SetEnableLowerFrameRate(bool enabled, const FrameSinkId& frame_sink_id);
  void SetEnableHalfFrameRate(bool enabled, const FrameSinkId& frame_sink_id);
#endif

#if BUILDFLAG(ARKWEB_CLEAN_BUFFERS_WHEN_INVISIBLE)
  void SetIfNeedCleanBuffers(const FrameSinkId& frame_sink_id, bool need_clean_buffers);
  void UpdateIfNeedCleanBuffers(const FrameSinkId& frame_sink_id);
  void EraseIfNeedCleanBuffers(const FrameSinkId& frame_sink_id);
#endif

#if BUILDFLAG(ARKWEB_OFFLINE_WEB_EVICT_BACK_BUFFERS)
  bool is_inactive_ = false;
  void SetRootCompositorFrameSink(const FrameSinkId& frame_sink_id);
  void SetIsOfflineWebComponentInactive(bool is_inactive, const FrameSinkId& frame_sink_id);
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

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  GpuServiceImpl* gpu_service();
  void ClearBlanklessSnapshotInfo(uint64_t blankless_key);
#endif
private:
#if BUILDFLAG(ARKWEB_CLEAN_BUFFERS_WHEN_INVISIBLE)
  base::flat_map<FrameSinkId, bool> need_clean_buffers_map_;
#endif
};
}  // namespace viz
#endif
