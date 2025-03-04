// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_RENDERER_MEDIA_OHOS_MEDIA_PLAYER_RENDERER_CLIENT_FACTORY_H_
#define CONTENT_RENDERER_MEDIA_OHOS_MEDIA_PLAYER_RENDERER_CLIENT_FACTORY_H_

#include <memory>

#include "base/functional/callback.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/single_thread_task_runner.h"
#include "content/common/content_export.h"
#include "media/base/renderer_factory.h"
#include "media/mojo/clients/mojo_renderer_factory.h"

namespace content {

// The default class for creating a MediaPlayerRendererClient
// and its associated MediaPlayerRenderer.
class CONTENT_EXPORT OHOSMediaPlayerRendererClientFactory
    : public media::RendererFactory {
 public:
  OHOSMediaPlayerRendererClientFactory(
      std::unique_ptr<media::MojoRendererFactory> mojo_renderer_factory);
  ~OHOSMediaPlayerRendererClientFactory() override;

  std::unique_ptr<media::Renderer> CreateRenderer(
      const scoped_refptr<base::SequencedTaskRunner>& media_task_runner,
      const scoped_refptr<base::TaskRunner>& worker_task_runner,
      media::AudioRendererSink* audio_renderer_sink,
      media::VideoRendererSink* video_renderer_sink,
      media::RequestOverlayInfoCB request_surface_cb,
      const gfx::ColorSpace& target_color_space) override;

  // The MediaPlayerRenderer uses a Type::URL.
  media::MediaResource::Type GetRequiredMediaResourceType() override;

 private:
  std::unique_ptr<media::MojoRendererFactory> mojo_renderer_factory_;
};

}  // namespace content

#endif  // CONTENT_RENDERER_MEDIA_OHOS_MEDIA_PLAYER_RENDERER_CLIENT_FACTORY_H_
