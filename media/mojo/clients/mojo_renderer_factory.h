// Copyright 2014 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_MOJO_CLIENTS_MOJO_RENDERER_FACTORY_H_
#define MEDIA_MOJO_CLIENTS_MOJO_RENDERER_FACTORY_H_

#include <memory>
#include <string>

#include "base/memory/raw_ptr.h"
#include "base/task/sequenced_task_runner.h"
#include "build/build_config.h"
#include "media/base/renderer_factory.h"
#include "media/mojo/buildflags.h"
#include "media/mojo/mojom/interface_factory.mojom.h"
#include "media/mojo/mojom/renderer.mojom.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
#include "arkweb/chromium_ext/media/base/media_player_url_params.h"
#endif // ARKWEB_CUSTOM_VIDEO_PLAYER
namespace media {

class MojoRenderer;

// The default factory class for creating MojoRenderer.
//
// The MojoRenderer should be thought of as a pure communication layer between
// media::Pipeline and a media::Renderer in a different process.
//
// Implementors of new media::Renderer types are encouraged to create small
// wrapper factories that use MRF, rather than creating derived MojoRenderer
// types, or extending MRF. See DecryptingRendererFactory for and example of a
// small wrapper around MRF.
class MojoRendererFactory final : public RendererFactory {
 public:
  explicit MojoRendererFactory(
      media::mojom::InterfaceFactory* interface_factory);

  MojoRendererFactory(const MojoRendererFactory&) = delete;
  MojoRendererFactory& operator=(const MojoRendererFactory&) = delete;

  ~MojoRendererFactory() final;

  std::unique_ptr<Renderer> CreateRenderer(
      const scoped_refptr<base::SequencedTaskRunner>& media_task_runner,
      const scoped_refptr<base::TaskRunner>& worker_task_runner,
      AudioRendererSink* audio_renderer_sink,
      VideoRendererSink* video_renderer_sink,
      RequestOverlayInfoCB request_overlay_info_cb,
      const gfx::ColorSpace& target_color_space) final;

#if BUILDFLAG(IS_WIN)
  std::unique_ptr<MojoRenderer> CreateMediaFoundationRenderer(
      mojo::PendingRemote<mojom::MediaLog> media_log_remote,
      mojo::PendingReceiver<mojom::MediaFoundationRendererExtension>
          renderer_extension_receiver,
      const scoped_refptr<base::SequencedTaskRunner>& media_task_runner,
      VideoRendererSink* video_renderer_sink);
#endif  // BUILDFLAG(IS_WIN)

#if BUILDFLAG(ENABLE_CAST_RENDERER)
  std::unique_ptr<MojoRenderer> CreateCastRenderer(
      const scoped_refptr<base::SequencedTaskRunner>& media_task_runner,
      VideoRendererSink* video_renderer_sink);
#endif  // BUILDFLAG(ENABLE_CAST_RENDERER)

#if BUILDFLAG(IS_ANDROID)
  std::unique_ptr<MojoRenderer> CreateFlingingRenderer(
      const std::string& presentation_id,
      mojo::PendingRemote<mojom::FlingingRendererClientExtension>
          client_extenion_ptr,
      const scoped_refptr<base::SequencedTaskRunner>& media_task_runner,
      VideoRendererSink* video_renderer_sink);
#endif  // defined (OS_ANDROID)

#if BUILDFLAG(IS_ANDROID) || BUILDFLAG(ARKWEB_MEDIA)
  std::unique_ptr<MojoRenderer> CreateMediaPlayerRenderer(
      const scoped_refptr<base::SequencedTaskRunner>& media_task_runner,
      VideoRendererSink* video_renderer_sink);
#endif  // BUILDFLAG(IS_ANDROID) || BUILDFLAG(ARKWEB_MEDIA)

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  std::unique_ptr<MojoRenderer> CreateCustomMediaPlayerRenderer(
      mojo::PendingRemote<mojom::CustomMediaPlayerRendererClientExtension>
          client_extension_remote,
      const scoped_refptr<base::SequencedTaskRunner>& media_task_runner,
      VideoRendererSink* video_renderer_sink,
      int player_id,
      const media::MediaPlayerUrlParams& params);
#endif // ARKWEB_CUSTOM_VIDEO_PLAYER

 private:
  // InterfaceFactory or InterfaceProvider used to create or connect to remote
  // renderer.
  raw_ptr<media::mojom::InterfaceFactory, DanglingUntriaged>
      interface_factory_ = nullptr;
};

}  // namespace media

#endif  // MEDIA_MOJO_CLIENTS_MOJO_RENDERER_FACTORY_H_
