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
 
#include "layer_tree_impl_utils.h"
#include "cc/layers/layer_impl.h"

namespace cc {

LayerTreeImplUtils::LayerTreeImplUtils(LayerTreeImpl* layer_tree_impl) : layer_tree_impl_(layer_tree_impl) {}

LayerTreeImplUtils::~LayerTreeImplUtils() {}

#if BUILDFLAG(ARKWEB_MENU)
void LayerTreeImplUtils::RegisterClippedVisualViewportSelectionBounds(
    const gfx::Rect& clipped_selection_bounds) {
  if (clipped_selection_bounds_ == clipped_selection_bounds) {
    return;
  }

  layer_tree_impl_->handle_visibility_changed_ = true;
  clipped_selection_bounds_ = clipped_selection_bounds;
}

gfx::Rect LayerTreeImplUtils::GetClippedVisualViewportSelectionBounds() const {
  return clipped_selection_bounds_;
}
#endif

#if BUILDFLAG(ARKWEB_SAME_LAYER)
void LayerTreeImplUtils::OnLayerRectUpdate(int id, const gfx::Rect& rect) {
  layer_tree_impl_->host_impl_->OnLayerRectUpdate(id, rect);
}

void LayerTreeImplUtils::OnLayerRectVisibilityChange(int id, bool visibility) {
  layer_tree_impl_->host_impl_->OnLayerRectVisibilityChange(id, visibility);
}
#endif

#if BUILDFLAG(ARKWEB_SYNC_RENDER)
gfx::Rect LayerTreeImplUtils::ViewportRectForTilePriority(const gfx::Rect& viewport_rect_for_tile_priority) {
  if (viewport_rect_for_tile_priority.IsEmpty()) {
    const gfx::Rect& deviceViewPort = layer_tree_impl_->GetDeviceViewport();
    if (deviceViewPort.height() > MAX_VIEWPORT_HEIGHT) {
      return gfx::Rect(0, 0, deviceViewPort.width(), MAX_VIEWPORT_HEIGHT);
    }
    return deviceViewPort;
  }
  return viewport_rect_for_tile_priority;
}
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void LayerTreeImplUtils::OnLayerBoundsUpdate(int id, const gfx::Rect& bounds) {
  layer_tree_impl_->host_impl_->OnLayerBoundsUpdate(id, bounds);
}
#endif  // ARKWEB_VIDEO_ASSISTANT
}