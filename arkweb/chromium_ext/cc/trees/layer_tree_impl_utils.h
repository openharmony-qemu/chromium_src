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

#ifndef CC_TREES_LAYER_TREE_IMPL_UTILS_H_
#define CC_TREES_LAYER_TREE_IMPL_UTILS_H_


#include "arkweb/build/features/features.h"
#include "cc/trees/layer_tree_impl.h"

#if BUILDFLAG(ARKWEB_SYNC_RENDER)
#define MAX_VIEWPORT_HEIGHT 8000
#endif

namespace cc {

class LayerTreeImpl;

class LayerTreeImplUtils {
public:

LayerTreeImplUtils(LayerTreeImpl* layer_tree_impl);
~LayerTreeImplUtils();

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  void OnLayerRectUpdate(int id, const gfx::Rect& rect);

  void OnLayerRectVisibilityChange(int id, bool visibility);
#endif
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  void OnLayerBoundsUpdate(int id, const gfx::Rect& bounds);
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_SYNC_RENDER)
  gfx::Rect ViewportRectForTilePriority(const gfx::Rect& viewport_rect_for_tile_priority);
#endif
#if BUILDFLAG(ARKWEB_MENU)
  void RegisterClippedVisualViewportSelectionBounds(
      const gfx::Rect& clipped_selection_bounds);
  gfx::Rect GetClippedVisualViewportSelectionBounds() const;
#endif

private:
raw_ptr<LayerTreeImpl> layer_tree_impl_;
#if BUILDFLAG(ARKWEB_MENU)
  gfx::Rect clipped_selection_bounds_;
#endif
};
}
#endif  //CC_TREES_LAYER_TREE_IMPL_UTILS_H_