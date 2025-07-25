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

namespace blink {

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
void SurfaceLayerBridge::SetVideoRectChangeCallback(
    cc::SurfaceLayer::RectChangeCallback callback) {
  if (surface_layer_) {
    surface_layer_->SetVideoRectChangeCallback(std::move(callback));
  } else {
    LOG(WARNING) << "SetVideoRectChangeCallback, surface_layer_ is null";
  }
}
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void SurfaceLayerBridge::SetLayerBoundsChangeCallback(
    cc::SurfaceLayer::LayerBoundsChangeCallback callback) {
  if (surface_layer_) {
    surface_layer_->SetLayerBoundsChangeCallback(std::move(callback));
  } else {
    LOG(WARNING) << "SetLayerBoundsChangeCallback, surface_layer_ is null";
  }
}
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_SAME_LAYER)
scoped_refptr<cc::SurfaceLayer> SurfaceLayerBridge::CreateSurfaceLayer(cc::SurfaceLayer::RectChangeCallback callback,
    cc::SurfaceLayer::RectVisibilityChangeCallback visibilitycallback,
    cc::SurfaceLayer::LayerRemovedVisibilityCallback layerRemovedCallback)
{
  surface_layer_ = cc::SurfaceLayer::Create(update_submission_state_callback_,
      std::move(callback), std::move(visibilitycallback), std::move(layerRemovedCallback));

  // This surface_id is essentially just a placeholder for the real one we will
  // get in OnFirstSurfaceActivation. We need it so that we properly get a
  // WillDraw, which then pushes the first compositor frame.
  parent_local_surface_id_allocator_.GenerateId();
  current_surface_id_ = viz::SurfaceId(
      frame_sink_id_,
      parent_local_surface_id_allocator_.GetCurrentLocalSurfaceId());

  surface_layer_->SetSurfaceId(current_surface_id_,
                               cc::DeadlinePolicy::UseDefaultDeadline());

  surface_layer_->SetStretchContentToFillBounds(true);
  surface_layer_->SetIsDrawable(true);
  surface_layer_->SetHitTestable(true);
  surface_layer_->SetMayContainVideo(contains_video_ == ContainsVideo::kYes);

  if (observer_) {
    observer_->RegisterContentsLayer(surface_layer_.get());
  }
  // We ignore our opacity until we are sure that we have something to show,
  // as indicated by getting an OnFirstSurfaceActivation call.
  surface_layer_->SetContentsOpaque(false);

  return surface_layer_;
}
#endif  // ARKWEB_SAME_LAYER
}  // namespace blink
