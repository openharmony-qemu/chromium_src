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

#include "arkweb/chromium_ext/components/viz/service/frame_sinks/frame_sink_manager_impl_utils.h"
#include "arkweb/chromium_ext/components/viz/service/frame_sinks/root_compositor_frame_sink_impl_ext.h"
#include "components/viz/service/frame_sinks/frame_sink_manager_impl.h"
namespace viz {
FrameSinkManagerImplUtils::FrameSinkManagerImplUtils(FrameSinkManagerImpl* managerimpl)
{
    this->frameSinkManagerImpl = managerimpl;
}

#if BUILDFLAG(ARKWEB_OCCLUDED_OPT)
void FrameSinkManagerImplUtils::EvictFrameBackBuffers(
    const FrameSinkId& root_frame_sink_id,
    bool invisible)
{
  TRACE_EVENT1("viz", "FrameSinkManagerImpl::EvictFrameBackBuffers",
               "root_frame_sink_id", root_frame_sink_id.ToString());

  auto root_it = frameSinkManagerImpl->root_sink_map_.find(root_frame_sink_id);
  if (root_it != frameSinkManagerImpl->root_sink_map_.end()) {
    if (!root_it->second) {
      LOG(ERROR) << "RootCompositorFrameSinkImpl is null";
      return;
    }
    root_it->second->AsExt()->EvictFrameBackBuffers(invisible);
  }
}

void FrameSinkManagerImplUtils::SetEnableLowerFrameRate(
    bool enabled,
    const FrameSinkId& frame_sink_id)
{
  auto it = frameSinkManagerImpl->root_sink_map_.find(frame_sink_id);
  if (it == frameSinkManagerImpl->root_sink_map_.end()) {
    return;
  }
  it->second->AsExt()->SetEnableLowerFrameRate(enabled);
}

void FrameSinkManagerImplUtils::SetEnableHalfFrameRate(
    bool enabled,
    const FrameSinkId& frame_sink_id)
{
  auto it = frameSinkManagerImpl->root_sink_map_.find(frame_sink_id);
  if (it == frameSinkManagerImpl->root_sink_map_.end()) {
    return;
  }
  it->second->AsExt()->SetEnableHalfFrameRate(enabled);
}
#endif


#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
void FrameSinkManagerImplUtils::UpdateVSyncFrequency(
    const FrameSinkId& frame_sink_id,
    uint32_t client_id)
{
  auto sink_it = frameSinkManagerImpl->sink_map_.begin();

  int frame_rate = 0;
  while (sink_it != frameSinkManagerImpl->sink_map_.end()) {
    if (sink_it->first.client_id() == client_id) {
      int fr = sink_it->second->compositor_frame_sink_impl_util_->GetFrameRate();
      frame_rate = std::max(frame_rate, fr);
    }
    ++sink_it;
  }

  auto root_sink_it = frameSinkManagerImpl->root_sink_map_.find(frame_sink_id);
  if (root_sink_it == frameSinkManagerImpl->root_sink_map_.end()) {
    LOG(ERROR) << "UpdateVSyncFrequency Fail, no vaild root sink";
    return;
  }
  root_sink_it->second->AsExt()->UpdateVSyncFrequency(frame_rate);
}

void FrameSinkManagerImplUtils::ResetVSyncFrequency(
    const FrameSinkId& frame_sink_id)
{
  auto root_sink_it = frameSinkManagerImpl->root_sink_map_.find(frame_sink_id);
  if (root_sink_it == frameSinkManagerImpl->root_sink_map_.end()) {
    LOG(ERROR) << "ResetVSyncFrequency Fail, no vaild root sink";
    return;
  }
  root_sink_it->second->AsExt()->ResetVSyncFrequency();
}
#endif

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
void FrameSinkManagerImplUtils::RestoreRenderFit(const FrameSinkId& frame_sink_id)
{
  if (frameSinkManagerImpl->client_) {
    frameSinkManagerImpl->client_->RestoreRenderFit(frame_sink_id.client_id(),
                                                    frame_sink_id.sink_id());
  }
}
#endif  // ARKWEB_MAXIMIZE_RESIZE

#if BUILDFLAG(ARKWEB_PIP)
void FrameSinkManagerImplUtils::SetPipActive(
    bool active,
    const FrameSinkId& frame_sink_id)
{
  auto it = frameSinkManagerImpl->root_sink_map_.find(frame_sink_id);
  if (it == frameSinkManagerImpl->root_sink_map_.end()) {
    return;
  }
  it->second->AsExt()->SetPipActive(active);
}
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
GpuServiceImpl* FrameSinkManagerImplUtils::gpu_service() {
  return frameSinkManagerImpl->gpu_service_;
}

void FrameSinkManagerImplUtils::ClearBlanklessSnapshotInfo(uint64_t blankless_key) {
  if (GpuServiceImpl* gpu_device = gpu_service()) {
    gpu_device->ClearBlanklessSnapshotInfo(blankless_key);
  }
}
#endif
}