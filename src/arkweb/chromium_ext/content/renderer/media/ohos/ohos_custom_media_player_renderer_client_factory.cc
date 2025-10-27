// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on media_player_renderer_client_factory.cc originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 

#include "content/renderer/media/ohos/ohos_custom_media_player_renderer_client_factory.h"

#include "base/task/sequenced_task_runner.h"
#include "base/task/single_thread_task_runner.h"
#include "content/renderer/media/ohos/ohos_custom_media_player_renderer_client.h"
#include "content/renderer/media/ohos/ohos_media_player_renderer_client.h"
#include "media/mojo/clients/mojo_renderer.h"
#include "media/mojo/clients/mojo_renderer_factory.h"
#include "media/mojo/mojom/renderer_extensions.mojom.h"

namespace content {

OHOSCustomMediaPlayerRendererClientFactory::
    OHOSCustomMediaPlayerRendererClientFactory(
        scoped_refptr<base::SingleThreadTaskRunner> compositor_task_runner,
        std::unique_ptr<media::MojoRendererFactory> mojo_renderer_factory,
        const GetNativeTextureWrapperCB& get_native_texture_wrapper_cb)
    : get_native_texture_wrapper_cb_(get_native_texture_wrapper_cb),
      compositor_task_runner_(compositor_task_runner),
      mojo_renderer_factory_(std::move(mojo_renderer_factory)) {}

OHOSCustomMediaPlayerRendererClientFactory::
    ~OHOSCustomMediaPlayerRendererClientFactory() {}

std::unique_ptr<media::Renderer>
OHOSCustomMediaPlayerRendererClientFactory::CreateRenderer(
    const scoped_refptr<base::SequencedTaskRunner>& media_task_runner,
    const scoped_refptr<base::TaskRunner>& worker_task_runner,
    media::AudioRendererSink* audio_renderer_sink,
    media::VideoRendererSink* video_renderer_sink,
    media::RequestOverlayInfoCB request_surface_cb,
    const gfx::ColorSpace& target_color_space) {
  // Used to send messages from the MPRC (Renderer process), to the MPR (Browser
  // process). The |renderer_extension_request| will be bound in
  // MediaPlayerRenderer.
  mojo::PendingRemote<media::mojom::MediaPlayerRendererExtension>
      renderer_extension_remote;
  auto renderer_extension_receiver =
      renderer_extension_remote.InitWithNewPipeAndPassReceiver();

  // Used to send messages from the MPR (Browser process), to the MPRC (Renderer
  // process). The |client_extension_request| will be bound in
  // MediaPlayerRendererClient.
  mojo::PendingRemote<media::mojom::MediaPlayerRendererClientExtension>
      client_extension_remote;
  auto client_extension_receiver =
      client_extension_remote.InitWithNewPipeAndPassReceiver();

  std::unique_ptr<media::MojoRenderer> mojo_renderer =
      mojo_renderer_factory_->CreateMediaPlayerRenderer(
          std::move(renderer_extension_receiver),
          std::move(client_extension_remote), media_task_runner,
          video_renderer_sink);

  media::ScopedNativeTextureWrapper native_texture_wrapper =
      get_native_texture_wrapper_cb_.Run();

  return std::make_unique<OHOSMediaPlayerRendererClient>(
      std::move(renderer_extension_remote),
      std::move(client_extension_receiver), media_task_runner,
      compositor_task_runner_, std::move(mojo_renderer),
      std::move(native_texture_wrapper), video_renderer_sink);
}

media::MediaResource::Type
OHOSCustomMediaPlayerRendererClientFactory::GetRequiredMediaResourceType() {
  return media::MediaResource::Type::KUrl;
}

std::unique_ptr<media::Renderer>
OHOSCustomMediaPlayerRendererClientFactory::CreateCustomRenderer(
    const scoped_refptr<base::SequencedTaskRunner>& media_task_runner,
    const scoped_refptr<base::TaskRunner>& worker_task_runner,
    media::AudioRendererSink* audio_renderer_sink,
    media::VideoRendererSink* video_renderer_sink,
    media::RequestOverlayInfoCB request_surface_cb,
    const gfx::ColorSpace& target_color_space,
    int player_id) {
  // Used to send messages from the MPRC (Renderer process), to the MPR (Browser
  // process). The |renderer_extension_request| will be bound in
  // MediaPlayerRenderer.
  mojo::PendingRemote<media::mojom::MediaPlayerRendererExtension>
      renderer_extension_remote;
  auto renderer_extension_receiver =
      renderer_extension_remote.InitWithNewPipeAndPassReceiver();

  // Used to send messages from the MPR (Browser process), to the MPRC (Renderer
  // process). The |client_extension_request| will be bound in
  // MediaPlayerRendererClient.
  mojo::PendingRemote<media::mojom::CustomMediaPlayerRendererClientExtension>
      client_extension_remote;
  auto client_extension_receiver =
      client_extension_remote.InitWithNewPipeAndPassReceiver();

  std::unique_ptr<media::MojoRenderer> mojo_renderer =
      mojo_renderer_factory_->CreateCustomMediaPlayerRenderer(
          std::move(renderer_extension_receiver),
          std::move(client_extension_remote), media_task_runner,
          video_renderer_sink, player_id);

  media::ScopedNativeTextureWrapper native_texture_wrapper =
      get_native_texture_wrapper_cb_.Run();

  return std::make_unique<OHOSCustomMediaPlayerRendererClient>(
      std::move(renderer_extension_remote),
      std::move(client_extension_receiver), media_task_runner,
      compositor_task_runner_, std::move(mojo_renderer),
      std::move(native_texture_wrapper), video_renderer_sink);
}

}  // namespace content
