// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/viz/client/frame_evictor.h"

#include <utility>

#include "base/containers/contains.h"
#include "base/feature_list.h"
#include "build/buildflag.h"
#include "components/viz/common/features.h"
#if BUILDFLAG(ARKWEB_EVICT_UNLOCK_FRAMES)
#include "components/viz/common/viz_utils.h"
#endif

namespace viz {

FrameEvictorClient::EvictIds::EvictIds() = default;
FrameEvictorClient::EvictIds::~EvictIds() = default;

FrameEvictorClient::EvictIds::EvictIds(EvictIds&& other) = default;
FrameEvictorClient::EvictIds& FrameEvictorClient::EvictIds::operator=(
    EvictIds&& other) = default;

FrameEvictor::FrameEvictor(FrameEvictorClient* client) : client_(client) {}

FrameEvictor::~FrameEvictor() {
  OnSurfaceDiscarded();
}

void FrameEvictor::OnNewSurfaceEmbedded() {
  has_surface_ = true;
  FrameEvictionManager::GetInstance()->AddFrame(this, visible_);
}

void FrameEvictor::OnSurfaceDiscarded() {
  FrameEvictionManager::GetInstance()->RemoveFrame(this);
  has_surface_ = false;
}

void FrameEvictor::SetVisible(bool visible) {
  if (visible_ == visible)
    return;
  visible_ = visible;
  if (has_surface_) {
    if (visible)
      FrameEvictionManager::GetInstance()->LockFrame(this);
    else
      FrameEvictionManager::GetInstance()->UnlockFrame(this);
  }
}

#if BUILDFLAG(ARKWEB_EVICT_UNLOCK_FRAMES)
std::vector<SurfaceId> FrameEvictor::CollectSurfaceIdsForEviction(bool isProcessMemoryPressure) const {
#else
std::vector<SurfaceId> FrameEvictor::CollectSurfaceIdsForEviction() const {
#endif
  auto ids = client_->CollectSurfaceIdsForEviction();
  std::vector<SurfaceId> output_ids = std::move(ids.embedded_ids);
#if BUILDFLAG(ARKWEB_EVICT_UNLOCK_FRAMES)
  // 1: max child surface size
  // also need to evict root frame once memory pressure is critical
  if (IsEvictUnlockFrameEnabled() && output_ids.size() > 1 && !isProcessMemoryPressure) {
    ids.ui_compositor_id = viz::SurfaceId();
    LOG(DEBUG) << "ids.ui_compositor_id: " << ids.ui_compositor_id.ToString() << "output_ids.size: " <<
      output_ids.size();
  }
#endif
  auto current = client_->GetCurrentSurfaceId();
  DCHECK(output_ids.empty() || !current.is_valid() ||
         base::Contains(output_ids, current));

  if (output_ids.empty() && current.is_valid()) {
    output_ids.push_back(current);
  }

  auto pre_nav_surface_id = client_->GetPreNavigationSurfaceId();
  if (pre_nav_surface_id.is_valid()) {
    output_ids.push_back(pre_nav_surface_id);
  }

  if (ids.ui_compositor_id.is_valid()) {
    output_ids.push_back(ids.ui_compositor_id);
  }

  base::ranges::sort(output_ids.begin(), output_ids.end());

  return output_ids;
}

#if BUILDFLAG(ARKWEB_EVICT_UNLOCK_FRAMES)
void FrameEvictor::EvictCurrentFrame(bool isProcessMemoryPressure) {
  client_->EvictDelegatedFrame(CollectSurfaceIdsForEviction(isProcessMemoryPressure));
}
#else
void FrameEvictor::EvictCurrentFrame() {
  client_->EvictDelegatedFrame(CollectSurfaceIdsForEviction());
}
#endif
}  // namespace viz
