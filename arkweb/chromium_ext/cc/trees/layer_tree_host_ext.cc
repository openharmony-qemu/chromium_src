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

#ifdef UNSAFE_BUFFERS_BUILD
// TODO(crbug.com/351564777): Remove this and convert code to safer constructs.
#pragma allow_unsafe_buffers
#endif

#include "arkweb/chromium_ext/cc/trees/layer_tree_host_ext.h"
#include "cc/trees/commit_state.h"

namespace cc {
LayerTreeHostExt::LayerTreeHostExt(InitParams params, CompositorMode mode)
    : LayerTreeHost(std::move(params), mode) {}

LayerTreeHostExt::~LayerTreeHostExt() {
  visible_layer_ids_.clear();
}

#if BUILDFLAG(ARKWEB_PINCH_SMOOTH)
void LayerTreeHostExt::SetPinchSmoothMode(bool isEnable) {
  proxy_->SetPinchSmoothMode(isEnable);
}
#endif

#if BUILDFLAG(ARKWEB_MENU)
void LayerTreeHostExt::RegisterClippedVisualViewportSelectionBounds(
    const gfx::Rect& clipped_selection_bounds) {
  if (pending_commit_state()->clipped_selection_bounds ==
      clipped_selection_bounds) {
    return;
  }

  pending_commit_state()->clipped_selection_bounds = clipped_selection_bounds;
  SetNeedsCommit();
}
#endif

#if BUILDFLAG(ARKWEB_SAME_LAYER)
void LayerTreeHostExt::OnLayerRectUpdate(int id, const gfx::Rect& rect) {
  DCHECK(IsMainThread());
  if (auto* layer = LayerById(id)) {
    layer->OnLayerRectUpdate(rect);
  }
}

void LayerTreeHostExt::OnLayerRectVisibilityChange(int id, bool visibility) {
  DCHECK(IsMainThread());
  if (auto* layer = LayerById(id)) {
    layer->OnLayerRectVisibilityChange(visibility);
  }
  if (visibility) {
    visible_layer_ids_.insert(id);
  } else {
    visible_layer_ids_.erase(id);
  }
}

void LayerTreeHostExt::CleanupVisibilityForRemovedLayer(Layer* layer) {
  if (!layer) {
    return;
  }
  DCHECK(IsMainThread());
  int id = layer->id();
  if (visible_layer_ids_.find(id) != visible_layer_ids_.end()) {
    if (auto* current_layer = LayerById(id)) {
      current_layer->CleanupVisibilityForRemovedLayer(false);
    }
    visible_layer_ids_.erase(id);
  }
}
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void LayerTreeHostExt::OnLayerBoundsUpdate(int id, const gfx::Rect& bounds) {
  DCHECK(IsMainThread());
  if (auto* layer = LayerById(id)) {
    layer->OnLayerBoundsUpdate(bounds);
  }
}
#endif  // ARKWEB_VIDEO_ASSISTANT
}  // namespace cc
