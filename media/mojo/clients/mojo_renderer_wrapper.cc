// Copyright 2019 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/mojo/clients/mojo_renderer_wrapper.h"

#include <utility>

namespace media {

MojoRendererWrapper::MojoRendererWrapper(
    std::unique_ptr<MojoRenderer> mojo_renderer)
    : mojo_renderer_(std::move(mojo_renderer)) {}

MojoRendererWrapper::~MojoRendererWrapper() = default;

void MojoRendererWrapper::Initialize(MediaResource* media_resource,
                                     RendererClient* client,
                                     PipelineStatusCallback init_cb) {
  mojo_renderer_->Initialize(media_resource, client, std::move(init_cb));
}

void MojoRendererWrapper::Flush(base::OnceClosure flush_cb) {
  mojo_renderer_->Flush(std::move(flush_cb));
}

void MojoRendererWrapper::StartPlayingFrom(base::TimeDelta time) {
  mojo_renderer_->StartPlayingFrom(time);
}

void MojoRendererWrapper::SetPlaybackRate(double playback_rate) {
  mojo_renderer_->SetPlaybackRate(playback_rate);
}

void MojoRendererWrapper::SetVolume(float volume) {
  mojo_renderer_->SetVolume(volume);
}

void MojoRendererWrapper::SetCdm(CdmContext* cdm_context,
                                 CdmAttachedCB cdm_attached_cb) {
  mojo_renderer_->SetCdm(cdm_context, std::move(cdm_attached_cb));
}

void MojoRendererWrapper::SetLatencyHint(
    absl::optional<base::TimeDelta> latency_hint) {
  mojo_renderer_->SetLatencyHint(latency_hint);
}

base::TimeDelta MojoRendererWrapper::GetMediaTime() {
  return mojo_renderer_->GetMediaTime();
}

#if defined(OHOS_CUSTOM_VIDEO_PLAYER)
void MojoRendererWrapper::SetMuted(bool muted) {
  mojo_renderer_->SetMuted(muted);
}
void MojoRendererWrapper::SetSurfaceId(int surface_id, const gfx::Rect& rect) {
  mojo_renderer_->SetSurfaceId(surface_id, rect);
}
void MojoRendererWrapper::SetMediaPlayerState(bool is_suspend, int suspend_type) {
  mojo_renderer_->SetMediaPlayerState(is_suspend, suspend_type);
}
void MojoRendererWrapper::SetMediaSourceList(
    const std::vector<MediaSourceInfo>& source_infos) {
  mojo_renderer_->SetMediaSourceList(source_infos);
}
void MojoRendererWrapper::SetMediaControls(bool show_media_controls,
    const std::vector<std::string>& controls_list) {
  mojo_renderer_->SetMediaControls(show_media_controls, controls_list);
}
void MojoRendererWrapper::SetPoster(const std::string& poster_url) {
  mojo_renderer_->SetPoster(poster_url);
}
void MojoRendererWrapper::SetAttributes(
    base::flat_map<std::string, std::string> attributes) {
  mojo_renderer_->SetAttributes(std::move(attributes));
}
void MojoRendererWrapper::SetReferrer(const std::string& referrer) {
  mojo_renderer_->SetReferrer(referrer);
}
void MojoRendererWrapper::SetIsAudio(bool is_audio) {
  mojo_renderer_->SetIsAudio(is_audio);
}
void MojoRendererWrapper::SetPlaybackRateWithReason(double playback_rate,
    ActionReason reason) {
  mojo_renderer_->SetPlaybackRateWithReason(playback_rate, reason);
}
#endif // OHOS_CUSTOM_VIDEO_PLAYER

}  // namespace media
