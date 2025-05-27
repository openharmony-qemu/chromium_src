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

#ifndef CC_TREES_LAYER_TREE_HOST_EXT_H_
#define CC_TREES_LAYER_TREE_HOST_EXT_H_

#include "arkweb/build/features/features.h"
#include "cc/trees/layer_tree_host.h"
#include "ui/gfx/geometry/rect.h"

namespace cc {
class CC_EXPORT LayerTreeHostExt : public LayerTreeHost {
 public:
  LayerTreeHostExt(InitParams params, CompositorMode mode);
  ~LayerTreeHostExt() override;

  LayerTreeHostExt(const LayerTreeHostExt&) = delete;
  LayerTreeHostExt& operator=(const LayerTreeHostExt&) = delete;
  LayerTreeHostExt* AsLayerTreeHostExt() override { return this; }

#if BUILDFLAG(ARKWEB_PINCH_SMOOTH)
  void SetPinchSmoothMode(bool isEnable);
#endif

#if BUILDFLAG(ARKWEB_MENU)
  void RegisterClippedVisualViewportSelectionBounds(
      const gfx::Rect& clipped_selection_bounds);
  const gfx::Rect& clipped_selection_bounds() {
    return pending_commit_state()->clipped_selection_bounds;
  }
#endif

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  void OnLayerRectUpdate(int id, const gfx::Rect& rect);

  void OnLayerRectVisibilityChange(int id, bool visibility);
  void CleanupVisibilityForRemovedLayer(Layer* layer);
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  void OnLayerBoundsUpdate(int id, const gfx::Rect& bounds);
#endif  // ARKWEB_VIDEO_ASSISTANT

private:
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  std::unordered_set<int> visible_layer_ids_;
#endif
};
}  // namespace cc
#endif  // CC_TREES_LAYER_TREE_HOST_EXT_H_
