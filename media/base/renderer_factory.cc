// Copyright 2014 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/renderer_factory.h"

namespace media {

RendererFactory::RendererFactory() = default;

RendererFactory::~RendererFactory() = default;

MediaResource::Type RendererFactory::GetRequiredMediaResourceType() {
  return MediaResource::Type::STREAM;
}

#if defined(OHOS_CUSTOM_VIDEO_PLAYER)
std::unique_ptr<Renderer> RendererFactory::CreateCustomRenderer(
    const scoped_refptr<base::SequencedTaskRunner>& media_task_runner,
    const scoped_refptr<base::TaskRunner>& worker_task_runner,
    media::AudioRendererSink* audio_renderer_sink,
    media::VideoRendererSink* video_renderer_sink,
    media::RequestOverlayInfoCB request_overlay_info_cb,
    const gfx::ColorSpace& target_color_space,
    int player_id) {
  return CreateRenderer(media_task_runner, worker_task_runner,
            audio_renderer_sink, video_renderer_sink,
            request_overlay_info_cb, target_color_space);
}
#endif // OHOS_CUSTOM_VIDEO_PLAYER

}  // namespace media
