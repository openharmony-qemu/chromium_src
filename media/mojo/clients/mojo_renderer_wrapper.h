// Copyright 2019 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_MOJO_CLIENTS_MOJO_RENDERER_WRAPPER_H_
#define MEDIA_MOJO_CLIENTS_MOJO_RENDERER_WRAPPER_H_

#include <memory>

#include "media/base/renderer.h"
#include "media/mojo/clients/mojo_renderer.h"

namespace media {

// Simple wrapper around a MojoRenderer.
// Provides a default behavior for forwarding all Renderer calls to a
// Renderer instance in a different process, through |mojo_renderer_|.
// Used as a base class to reduce boiler plate code for derived types, which can
// override only the methods they need to specialize.
class MojoRendererWrapper : public Renderer {
 public:
  explicit MojoRendererWrapper(std::unique_ptr<MojoRenderer> mojo_renderer);

  MojoRendererWrapper(const MojoRendererWrapper&) = delete;
  MojoRendererWrapper& operator=(const MojoRendererWrapper&) = delete;

  ~MojoRendererWrapper() override;

  // Renderer implementation.
  void Initialize(MediaResource* media_resource,
                  RendererClient* client,
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
                  RequestSurfaceCB request_surface_cb,
                  VideoDecoderChangedCB decoder_changed_cb,
#endif // ARKWEB_VIDEO_ASSISTANT
                  PipelineStatusCallback init_cb) override;
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  void Initialize(CreateTextureCB create_texture_cb,
                  DestroyTextureCB destroy_texture_cb) override {}
#endif
  void SetCdm(CdmContext* cdm_context, CdmAttachedCB cdm_attached_cb) override;
  void SetLatencyHint(std::optional<base::TimeDelta> latency_hint) override;
  void Flush(base::OnceClosure flush_cb) override;
  void StartPlayingFrom(base::TimeDelta time) override;
  void SetPlaybackRate(double playback_rate) override;
  void SetVolume(float volume) override;
  base::TimeDelta GetMediaTime() override;

#if BUILDFLAG(ARKWEB_MEDIA)
  void SetNativeWindowSurface(int native_window_id) override;
#endif // ARKWEB_MEDIA

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  void SetMuted(bool muted) override;
  void SetSurfaceId(int surface_id, const gfx::Rect& rect) override;
  void SetMediaPlayerState(bool is_suspend, int suspend_type) override;
  void SetMediaSourceList(
      const std::vector<MediaSourceInfo>& source_infos) override;
  void SetMediaControls(bool show_media_controls,
      const std::vector<std::string>& controls_list) override;
  void SetPoster(const std::string& poster_url) override;
  void SetAttributes(
      base::flat_map<std::string, std::string> attributes) override;
  void SetReferrer(const std::string& referrer) override;
  void SetIsAudio(bool is_audio) override;
  void SetPlaybackRateWithReason(double playback_rate,
      ActionReason reason) override;
#endif // ARKWEB_CUSTOM_VIDEO_PLAYER
#if BUILDFLAG(ARKWEB_PIP)
  void PipEnable(bool enable) override;
#endif
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  void SetPreciseSeekTarget(int64_t target_timestamp) override;
#endif // ARKWEB_VIDEO_ASSISTANT
#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
  void RecycleDmaBuffer() override;
  void ResumeDmaBuffer() override;
#endif  // ARKWEB_MEDIA_DMABUF
 private:
  std::unique_ptr<MojoRenderer> mojo_renderer_;
};

}  // namespace media

#endif  // MEDIA_MOJO_CLIENTS_MOJO_RENDERER_WRAPPER_H_
