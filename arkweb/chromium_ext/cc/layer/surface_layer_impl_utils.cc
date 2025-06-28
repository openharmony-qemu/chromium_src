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
#include "arkweb/chromium_ext/cc/layer/surface_layer_impl_utils.h"
#include "arkweb/chromium_ext/cc/trees/layer_tree_impl_utils.h"
#include "cc/trees/occlusion.h"
#include "cc/trees/layer_tree_impl.h"
#include "cc/layers/surface_layer_impl.h"
#include "ui/gfx/geometry/transform_util.h"

namespace cc {

SurfaceLayerImplUtils::SurfaceLayerImplUtils(SurfaceLayerImpl* surfaceLayerImpl)
{
  this->impl = surfaceLayerImpl;
}

void SurfaceLayerImplUtils::VisbilityChange()
{
  if (impl->may_contain_video()) {
    return;
  }
  if (impl->layer_impl_utils()->may_contain_native()) {
    bool visibility = false;
    if (impl->visible_layer_rect().IsEmpty()) {
      visibility = false;
    } else {
      visibility = true;
    }
    if (visibility != visibility_) {
      visibility_ = visibility;
      LOG(INFO) << "[NativeEmbed] rect visibility: " << visibility;
      impl->layer_tree_impl()->layer_tree_impl_utils()->OnLayerRectVisibilityChange(impl->id(), visibility);
    }
  }
}

void SurfaceLayerImplUtils::LayerRectUpdate()
{
  if (impl->may_contain_video() || !impl->layer_impl_utils()->may_contain_native()) {
      return;
  }
  gfx::Transform transform = impl->DrawTransform();
  gfx::Rect bounds_quad_rect(impl->bounds());
  Occlusion occlusion_in_video_space =
      impl->draw_properties()
          .occlusion_in_content_space.GetOcclusionWithGivenDrawTransform(transform);
  gfx::Rect visible_quad_rect =
      occlusion_in_video_space.GetUnoccludedContentRect(bounds_quad_rect);
  bounds_quad_rect.set_origin(
      impl->ScreenSpaceTransform().MapPoint(visible_quad_rect.origin()));
  if (!bounds_quad_rect_.ApproximatelyEqual(bounds_quad_rect, 1)) {
    bounds_quad_rect_ = bounds_quad_rect;
    LOG(DEBUG) << "[NativeEmbed] visible quad rect: "
               << visible_quad_rect.ToString()
               << ", bounds quad rect: " << bounds_quad_rect.ToString();
    impl->layer_tree_impl()->layer_tree_impl_utils()->OnLayerRectUpdate(impl->id(), bounds_quad_rect);
  }
}

bool SurfaceLayerImplUtils::HasNativeLayer() {
  return impl->layer_impl_utils()->may_contain_native();
}
}  // namespace cc