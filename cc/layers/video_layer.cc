// Copyright 2010 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/video_layer.h"

#include "cc/layers/video_layer_impl.h"

namespace cc {

scoped_refptr<VideoLayer> VideoLayer::Create(
    VideoFrameProvider* provider,
    media::VideoTransformation transform) {
  return base::WrapRefCounted(new VideoLayer(provider, transform));
}

#if BUILDFLAG(IS_OHOS)
scoped_refptr<VideoLayer> VideoLayer::Create(
    VideoFrameProvider* provider,
    media::VideoTransformation transform,
    RectChangeCallback callback) {
  return base::WrapRefCounted(
      new VideoLayer(provider, transform, std::move(callback)));
}

scoped_refptr<VideoLayer> VideoLayer::Create(
    VideoFrameProvider* provider,
    media::VideoTransformation transform,
    RectChangeCallback callback,
    RectVisibilityChangeCallback visibilitycallback) {
  return base::WrapRefCounted(
      new VideoLayer(provider, transform, std::move(callback), std::move(visibilitycallback)));
}

VideoLayer::VideoLayer(VideoFrameProvider* provider,
                       media::VideoTransformation transform,
                       RectChangeCallback callback)
    : provider_(provider),
      transform_(transform),
      rect_change_callback_(std::move(callback)) {
  SetMayContainVideo(true);
  DCHECK(provider_.Read(*this));
}

VideoLayer::VideoLayer(VideoFrameProvider* provider,
                       media::VideoTransformation transform,
                       RectChangeCallback callback,
                       RectVisibilityChangeCallback visibilitycallback)
    : provider_(provider),
      transform_(transform),
      rect_change_callback_(std::move(callback)),
      rect_visibility_change_callback_(std::move(visibilitycallback)) {
  SetMayContainVideo(true);
  DCHECK(provider_.Read(*this));
}

void VideoLayer::OnLayerRectUpdate(const gfx::Rect& rect) {
  if (!rect_change_callback_.is_null()) {
    rect_change_callback_.Run(rect);
  }
}

void VideoLayer::OnLayerRectVisibilityChange(bool visibility) {
  if (!rect_visibility_change_callback_.is_null()) {
    rect_visibility_change_callback_.Run(visibility);
  }
}

void VideoLayer::ResetLayerRectUpdateCallback() {
  if (!rect_change_callback_.is_null()) {
    rect_change_callback_.Reset();
  }
}
#endif

VideoLayer::VideoLayer(VideoFrameProvider* provider,
                       media::VideoTransformation transform)
    : provider_(provider), transform_(transform) {
  SetMayContainVideo(true);
  DCHECK(provider_.Read(*this));
}

VideoLayer::~VideoLayer() = default;

std::unique_ptr<LayerImpl> VideoLayer::CreateLayerImpl(
    LayerTreeImpl* tree_impl) const {
  return VideoLayerImpl::Create(tree_impl, id(), provider_.Read(*this),
                                transform_);
}

bool VideoLayer::Update() {
  bool updated = Layer::Update();

  // Video layer doesn't update any resources from the main thread side,
  // but repaint rects need to be sent to the VideoLayerImpl via commit.
  //
  // This is the inefficient legacy redraw path for videos.  It's better to
  // communicate this directly to the VideoLayerImpl.
  updated |= !update_rect().IsEmpty();

  return updated;
}

void VideoLayer::StopUsingProvider() {
  provider_.Write(*this) = nullptr;
}

}  // namespace cc