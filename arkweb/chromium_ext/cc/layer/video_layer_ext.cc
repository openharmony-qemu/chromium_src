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

#include "arkweb/chromium_ext/cc/layer/video_layer_ext.h"

namespace cc {
#if BUILDFLAG(ARKWEB_SAME_LAYER)
scoped_refptr<VideoLayerExt> VideoLayerExt::Create(
    VideoFrameProvider* provider,
    media::VideoTransformation transform,
    RectChangeCallback callback) {
  return base::WrapRefCounted(
      new VideoLayerExt(provider, transform, std::move(callback)));
}

scoped_refptr<VideoLayerExt> VideoLayerExt::Create(
    VideoFrameProvider* provider,
    media::VideoTransformation transform,
    RectChangeCallback callback,
    RectVisibilityChangeCallback visibilitycallback) {
  return base::WrapRefCounted(new VideoLayerExt(
      provider, transform, std::move(callback), std::move(visibilitycallback)));
}

scoped_refptr<VideoLayerExt> VideoLayerExt::Create(
    VideoFrameProvider* provider,
    media::VideoTransformation transform,
    RectChangeCallback callback,
    RectVisibilityChangeCallback visibilitycallback,
    LayerRemovedVisibilityCallback layerRemovedCallback) {
  return base::WrapRefCounted(new VideoLayerExt(
      provider, transform, std::move(callback), std::move(visibilitycallback),
      std::move(layerRemovedCallback)));
}

VideoLayerExt::VideoLayerExt(VideoFrameProvider* provider,
                             media::VideoTransformation transform,
                             RectChangeCallback callback)
    : VideoLayer(provider, transform),
      rect_change_callback_(std::move(callback)) {
  SetMayContainVideo(true);
  DCHECK(provider_.Read(*this));
}

VideoLayerExt::VideoLayerExt(VideoFrameProvider* provider,
                             media::VideoTransformation transform,
                             RectChangeCallback callback,
                             RectVisibilityChangeCallback visibilitycallback)
    : VideoLayer(provider, transform),
      rect_change_callback_(std::move(callback)),
      rect_visibility_change_callback_(std::move(visibilitycallback)) {
  SetMayContainVideo(true);
  DCHECK(provider_.Read(*this));
}

VideoLayerExt::VideoLayerExt(VideoFrameProvider* provider,
                             media::VideoTransformation transform,
                             RectChangeCallback callback,
                             RectVisibilityChangeCallback visibilitycallback,
                             LayerRemovedVisibilityCallback layerRemovedCallback)
    : VideoLayer(provider, transform),
      rect_change_callback_(std::move(callback)),
      rect_visibility_change_callback_(std::move(visibilitycallback)),
      layer_removed_visibility_callback_(std::move(layerRemovedCallback)) {
  SetMayContainVideo(true);
  DCHECK(provider_.Read(*this));
}

void VideoLayerExt::OnLayerRectUpdate(const gfx::Rect& rect) {
  if (!rect_change_callback_.is_null()) {
    rect_change_callback_.Run(rect);
  }
}

void VideoLayerExt::OnLayerRectVisibilityChange(bool visibility) {
  if (!rect_visibility_change_callback_.is_null()) {
    rect_visibility_change_callback_.Run(visibility);
  }
}

void VideoLayerExt::CleanupVisibilityForRemovedLayer(bool visibility) {
  if (!layer_removed_visibility_callback_.is_null()) {
    layer_removed_visibility_callback_.Run(visibility);
  }
}

void VideoLayerExt::ResetLayerRectCallback() {
  if (!rect_change_callback_.is_null()) {
    rect_change_callback_.Reset();
  }
  if (!rect_visibility_change_callback_.is_null()) {
    rect_visibility_change_callback_.Reset();
  }
  if (!layer_removed_visibility_callback_.is_null()) {
    layer_removed_visibility_callback_.Reset();
  }
}
#endif
}  // namespace cc