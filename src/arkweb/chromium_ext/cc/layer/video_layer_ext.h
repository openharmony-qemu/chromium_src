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

#ifndef CC_LAYERS_VIDEO_LAYER_EXT_H_
#define CC_LAYERS_VIDEO_LAYER_EXT_H_

#include "base/memory/raw_ptr.h"
#include "cc/layers/video_layer.h"
#include "media/base/video_transformation.h"

namespace media {
class VideoFrame;
}

namespace cc {
class VideoLayer;

class VideoLayerExt : public VideoLayer {
 public:
  VideoLayerExt(const VideoLayerExt&) = delete;
  VideoLayerExt& operator=(const VideoLayerExt&) = delete;

  ~VideoLayerExt() override = default;

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  using RectChangeCallback = base::RepeatingCallback<void(const gfx::Rect&)>;
  using RectVisibilityChangeCallback = base::RepeatingCallback<void(bool)>;
  using LayerRemovedVisibilityCallback = base::RepeatingCallback<void(bool)>;
  static scoped_refptr<VideoLayerExt> Create(
      VideoFrameProvider* provider,
      media::VideoTransformation transform,
      RectChangeCallback callback);
  static scoped_refptr<VideoLayerExt> Create(
      VideoFrameProvider* provider,
      media::VideoTransformation transform,
      RectChangeCallback callback,
      RectVisibilityChangeCallback visibilitycallback);
  static scoped_refptr<VideoLayerExt> Create(
      VideoFrameProvider* provider,
      media::VideoTransformation transform,
      RectChangeCallback callback,
      RectVisibilityChangeCallback visibilitycallback,
      LayerRemovedVisibilityCallback layerRemovedCallback);
#endif

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  void OnLayerRectUpdate(const gfx::Rect& rect) override;
  void OnLayerRectVisibilityChange(bool visibility) override;
  void CleanupVisibilityForRemovedLayer(bool visibility) override;
  void ResetLayerRectCallback();
#endif

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  VideoLayerExt(VideoFrameProvider* provider,
                media::VideoTransformation transform,
                RectChangeCallback callback);
  VideoLayerExt(VideoFrameProvider* provider,
                media::VideoTransformation transform,
                RectChangeCallback callback,
                RectVisibilityChangeCallback visibilitycallback);
  VideoLayerExt(VideoFrameProvider* provider,
                media::VideoTransformation transform,
                RectChangeCallback callback,
                RectVisibilityChangeCallback visibilitycallback,
                LayerRemovedVisibilityCallback layerRemovedCallback);
#endif

  VideoLayerExt* AsExt() override { return this; }

 private:
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  base::RepeatingCallback<void(const gfx::Rect&)> rect_change_callback_;
  base::RepeatingCallback<void(bool)> rect_visibility_change_callback_;
  base::RepeatingCallback<void(bool)> layer_removed_visibility_callback_;
#endif
};
}  // namespace cc
#endif  // CC_LAYERS_VIDEO_LAYER_EXT_H_