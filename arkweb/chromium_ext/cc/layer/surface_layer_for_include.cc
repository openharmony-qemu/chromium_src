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

#include "base/logging.h"

namespace cc {

#if BUILDFLAG(ARKWEB_SAME_LAYER)
scoped_refptr<SurfaceLayer> SurfaceLayer::Create(UpdateSubmissionStateCB update_submission_state_callback,
    RectChangeCallback callback,
    RectVisibilityChangeCallback visibilitycallback,
    LayerRemovedVisibilityCallback layerRemovedCallback)
{
    return base::WrapRefCounted(
        new SurfaceLayer(std::move(update_submission_state_callback),
            std::move(callback), std::move(visibilitycallback), std::move(layerRemovedCallback)));
}
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
void SurfaceLayer::SetVideoRectChangeCallback(RectChangeCallback callback) {
  video_rect_change_callback_ = std::move(callback);
}

void SurfaceLayer::OnLayerRectUpdate(const gfx::Rect& rect) {
  if (video_rect_change_callback_) {
    video_rect_change_callback_.Run(rect);
    return;
  }
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  if (!rect_change_callback_.is_null()) {
    rect_change_callback_.Run(rect);
  }
#endif
}
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void SurfaceLayer::SetLayerBoundsChangeCallback(
    LayerBoundsChangeCallback callback) {
  layer_bounds_change_callback_ = std::move(callback);
}
void SurfaceLayer::OnLayerBoundsUpdate(const gfx::Rect& bounds) {
  if (layer_bounds_change_callback_) {
    layer_bounds_change_callback_.Run(bounds);
  }
}
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_SAME_LAYER)
SurfaceLayer::SurfaceLayer(UpdateSubmissionStateCB update_submission_state_callback,
    RectChangeCallback callback,
    RectVisibilityChangeCallback visibilitycallback,
    LayerRemovedVisibilityCallback layerRemovedCallback)
    : update_submission_state_callback_(
      std::move(update_submission_state_callback)),
      may_contain_video_(false),
      deadline_in_frames_(0u),
      stretch_content_to_fill_bounds_(false),
      surface_hit_testable_(false),
      has_pointer_events_none_(false),
      is_reflection_(false),
      rect_change_callback_(std::move(callback)),
      rect_visibility_change_callback_(std::move(visibilitycallback)),
      layer_removed_visibility_callback_(std::move(layerRemovedCallback)) {}

void SurfaceLayer::OnLayerRectVisibilityChange(bool visibility)
{
  LOG(INFO) << "[NativeEmbed] SurfaceLayer::OnLayerRectVisibilityChange " << visibility;
  if (!rect_visibility_change_callback_.is_null()) {
    rect_visibility_change_callback_.Run(visibility);
  }
}

void SurfaceLayer::ResetLayerRectUpdateCallback()
{
  LOG(INFO) << "[NativeEmbed] SurfaceLayer::ResetLayerRectUpdateCallback";
  if (!rect_change_callback_.is_null()) {
    rect_change_callback_.Reset();
  }
}

void SurfaceLayer::ResetLayerRectVisibilityChangeCallback()
{
  if (!rect_visibility_change_callback_.is_null()) {
    rect_visibility_change_callback_.Reset();
  }
}

void SurfaceLayer::CleanupVisibilityForRemovedLayer(bool visibility) {
  if (!layer_removed_visibility_callback_.is_null()) {
    layer_removed_visibility_callback_.Run(visibility);
  }
}
#endif
}  // namespace cc