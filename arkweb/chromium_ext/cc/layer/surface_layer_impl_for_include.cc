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

#include "cc/trees/layer_tree_impl_utils.h"

#include "base/logging.h"

namespace cc {

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void SurfaceLayerImpl::OnLayerBoundsUpdate(gfx::Rect visible_quad_rect) {
  gfx::Rect layer_bounds =
      ScreenSpaceTransform().MapRect(visible_quad_rect);
  if (!layer_bounds_.ApproximatelyEqual(layer_bounds, 1)) {
    layer_bounds_ = layer_bounds;
    layer_tree_impl()->layer_tree_impl_utils()->OnLayerBoundsUpdate(id(), layer_bounds);
  }
}
#endif // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
void SurfaceLayerImpl::OnLayerRectUpdate(gfx::Rect visible_quad_rect) {
  if (!may_contain_video()) {
    return;
  }
   visible_quad_rect.set_origin(
        ScreenSpaceTransform().MapPoint(visible_quad_rect.origin()));
  if (!visible_quad_rect_.ApproximatelyEqual(visible_quad_rect, 1)) {
    visible_quad_rect_ = visible_quad_rect;
    layer_tree_impl()->layer_tree_impl_utils()->OnLayerRectUpdate(id(), visible_quad_rect);
  }
}
#endif // ARKWEB_CUSTOM_VIDEO_PLAYER

}  // namespace cc