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

#include "arkweb/chromium_ext/components/viz/host/host_frame_sink_manager_utils.h"
#include "components/viz/host/host_frame_sink_manager.h"

namespace viz {

HostFrameSinkManagerUtils::HostFrameSinkManagerUtils(HostFrameSinkManager* manager)
{
    this->hostFrameSinkManager = manager;
}

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void HostFrameSinkManagerUtils::SendInternalBeginFrame(const FrameSinkId& id)
{
  hostFrameSinkManager->frame_sink_manager_->SendInternalBeginFrame(id);
}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)


#if BUILDFLAG(ARKWEB_OCCLUDED_OPT)
void HostFrameSinkManagerUtils::SetEnableLowerFrameRate(
    bool enabled,
    const FrameSinkId& frame_sink_id)
{
  hostFrameSinkManager->frame_sink_manager_->SetEnableLowerFrameRate(enabled, frame_sink_id);
}

void HostFrameSinkManagerUtils::SetEnableHalfFrameRate(
    bool enabled,
    const FrameSinkId& frame_sink_id)
{
  hostFrameSinkManager->frame_sink_manager_->SetEnableHalfFrameRate(enabled, frame_sink_id);
}

void HostFrameSinkManagerUtils::EvictFrameBackBuffers(
    const FrameSinkId& frame_sink_id,
    bool invisible)
{
  hostFrameSinkManager->frame_sink_manager_->EvictFrameBackBuffers(frame_sink_id, invisible);
}
#endif

#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
void HostFrameSinkManagerUtils::UpdateVSyncFrequency(
    const FrameSinkId& frame_sink_id)
{
  auto frame_sink_data_it = hostFrameSinkManager->frame_sink_data_map_.find(frame_sink_id);
  if (frame_sink_data_it == hostFrameSinkManager->frame_sink_data_map_.end()) {
    return;
  }
  auto children = frame_sink_data_it->second.children;
  auto children_it = children.begin();
  uint32_t client_id = 0;
  if (children_it != children.end()) {
    client_id = children_it->client_id();
  }
  hostFrameSinkManager->frame_sink_manager_->UpdateVSyncFrequency(frame_sink_id, client_id);
}

void HostFrameSinkManagerUtils::ResetVSyncFrequency(
    const FrameSinkId& frame_sink_id)
{
  hostFrameSinkManager->frame_sink_manager_->ResetVSyncFrequency(frame_sink_id);
}
#endif

#if BUILDFLAG(ARKWEB_PIP)
void HostFrameSinkManagerUtils::SetPipActive(
    bool active,
    const FrameSinkId& frame_sink_id)
{
  hostFrameSinkManager->frame_sink_manager_->SetPipActive(active, frame_sink_id);
}
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
void HostFrameSinkManagerUtils::ClearBlanklessSnapshotInfo(uint64_t blankless_key)
{
  if (hostFrameSinkManager->frame_sink_manager_) {
    hostFrameSinkManager->frame_sink_manager_->ClearBlanklessSnapshotInfo(blankless_key);
  }
}
#endif

void HostFrameSinkManagerUtils::UtilsRestoreRenderFit(uint32_t client_id,
                                                      uint32_t sink_id)
{
  FrameSinkId id(client_id, sink_id);
  auto iter = hostFrameSinkManager->frame_sink_data_map_.find(id);
  if (iter == hostFrameSinkManager->frame_sink_data_map_.end()) {
    return;
  }
  const HostFrameSinkManager::FrameSinkData& data = iter->second;
  if (data.client) {
    data.client->RestoreRenderFit();
  }
}
}   // namespace viz