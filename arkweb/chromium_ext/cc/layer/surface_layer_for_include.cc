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

namespace cc {

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
void SurfaceLayer::SetVideoRectChangeCallback(RectChangeCallback callback) {
  video_rect_change_callback_ = std::move(callback);
}

void SurfaceLayer::OnLayerRectUpdate(const gfx::Rect& rect) {
  if (video_rect_change_callback_) {
    video_rect_change_callback_.Run(rect);
  }
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

}  // namespace cc