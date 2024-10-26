// Copyright 2010 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_VIDEO_LAYER_H_
#define CC_LAYERS_VIDEO_LAYER_H_

#include <memory>

#include "base/functional/callback.h"
#include "base/memory/raw_ptr.h"
#include "cc/cc_export.h"
#include "cc/layers/layer.h"
#include "media/base/video_transformation.h"

namespace media {
class VideoFrame;
}

namespace cc {

class VideoFrameProvider;
class VideoLayerImpl;

// A Layer that contains a Video element.
class CC_EXPORT VideoLayer : public Layer {
 public:
  static scoped_refptr<VideoLayer> Create(VideoFrameProvider* provider,
                                          media::VideoTransformation transform);
#if BUILDFLAG(IS_OHOS)
  using RectChangeCallback = base::RepeatingCallback<void(const gfx::Rect&)>;
  using RectVisibilityChangeCallback = base::RepeatingCallback<void(bool)>;
  static scoped_refptr<VideoLayer> Create(VideoFrameProvider* provider,
                                          media::VideoTransformation transform,
                                          RectChangeCallback callback,
                                          RectVisibilityChangeCallback visibilitycallback);
  static scoped_refptr<VideoLayer> Create(VideoFrameProvider* provider,
                                          media::VideoTransformation transform,
                                          RectChangeCallback callback);
#endif

  VideoLayer(const VideoLayer&) = delete;
  VideoLayer& operator=(const VideoLayer&) = delete;

  std::unique_ptr<LayerImpl> CreateLayerImpl(
      LayerTreeImpl* tree_impl) const override;

  bool Update() override;

#if BUILDFLAG(IS_OHOS)
  void OnLayerRectUpdate(const gfx::Rect& rect) override;
  void OnLayerRectVisibilityChange(bool visibility) override;
  void ResetLayerRectCallback();
#endif

  // Clears |provider_| to ensure it is not used after destruction.
  void StopUsingProvider();

 private:
  VideoLayer(VideoFrameProvider* provider,
             media::VideoTransformation transform);
#if BUILDFLAG(IS_OHOS)
  VideoLayer(VideoFrameProvider* provider,
             media::VideoTransformation transform,
             RectChangeCallback callback);
  VideoLayer(VideoFrameProvider* provider,
             media::VideoTransformation transform,
             RectChangeCallback callback,
             RectVisibilityChangeCallback visibilitycallback);
#endif
  ~VideoLayer() override;

  // This pointer is only for passing to VideoLayerImpl's constructor. It should
  // never be dereferenced by this class.
  ProtectedSequenceReadable<raw_ptr<VideoFrameProvider>> provider_;

  const media::VideoTransformation transform_;

#if BUILDFLAG(IS_OHOS)
  base::RepeatingCallback<void(const gfx::Rect&)> rect_change_callback_;
  base::RepeatingCallback<void(bool)> rect_visibility_change_callback_;
#endif
};

}  // namespace cc

#endif  // CC_LAYERS_VIDEO_LAYER_H_