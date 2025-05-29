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

}  // namespace blink
