// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_MOJO_CLIENTS_MOJO_RENDERER_H_
#error "must be in include form MEDIA_MOJO_CLIENTS_MOJO_RENDERER_H_"
#endif

namespace media {

void MojoRenderer::InitializeRendererFromUrlExt() {
#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  remote_renderer_->SetMediaSourceList(std::move(source_infos_));
  remote_renderer_->SetMediaControls(show_media_controls_, std::move(controls_list_));
  remote_renderer_->SetPoster(std::move(poster_url_));
  remote_renderer_->SetAttributes(std::move(attributes_));
  remote_renderer_->SetReferrer(std::move(referrer_));
  remote_renderer_->SetIsAudio(is_audio_);
  remote_renderer_->SetMuted(muted_);
#endif // ARKWEB_CUSTOM_VIDEO_PLAYER
}

void MojoRenderer::OnInitializedExt() {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
    if (!request_surface_cb_.is_null()) {
      LOG(INFO) << "component, surface_create_CB created";
      SurfaceCreatedCB surface_create_CB = base::BindPostTaskToCurrentDefault(
          base::BindRepeating(&MojoRenderer::OnRequestVideoSurfaceDone,
                              weak_factory_.GetWeakPtr()));
      std::move(request_surface_cb_).Run(
          std::move(surface_create_CB), true, "MojoRenderer");
    }
#endif  // ARKWEB_VIDEO_ASSISTANT
}

#if BUILDFLAG(ARKWEB_MEDIA)
void MojoRenderer::SetNativeWindowSurface(int native_window_id) {
  BindRemoteRendererIfNeeded();
  if (remote_renderer_.is_bound()) {
    remote_renderer_->SetNativeWindowSurface(native_window_id);
  } else {
    LOG(ERROR) << "SetNativeWindowSurface failed";
  }
}
#endif // ARKWEB_MEDIA

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
void MojoRenderer::SetMuted(bool muted) {
  muted_ = muted;
}
void MojoRenderer::SetSurfaceId(int surface_id, const gfx::Rect& rect) {
  BindRemoteRendererIfNeeded();
  if (remote_renderer_.is_bound()) {
    remote_renderer_->SetSurfaceId(surface_id, rect);
  } else {
    LOG(ERROR) << "SetSurfaceId failed";
  }
}
void MojoRenderer::SetMediaPlayerState(bool is_suspend, int suspend_type) {
  BindRemoteRendererIfNeeded();
  if (remote_renderer_.is_bound()) {
    remote_renderer_->SetMediaPlayerState(is_suspend, suspend_type);
  } else {
    LOG(ERROR) << "SetMediaPlayerState failed";
  }
}
void MojoRenderer::SetMediaSourceList(
    const std::vector<MediaSourceInfo>& source_infos) {
  source_infos_.clear();
  source_infos_.reserve(source_infos.size());
  for (const auto& source_info : source_infos) {
    mojom::MediaSourceInfoPtr info = mojom::MediaSourceInfo::New(
        source_info.media_source, source_info.media_format);
    source_infos_.push_back(std::move(info));
  }
}
void MojoRenderer::SetMediaControls(bool show_media_controls,
    const std::vector<std::string>& controls_list) {
  show_media_controls_ = show_media_controls;
  controls_list_ = controls_list;
}
void MojoRenderer::SetPoster(const std::string& poster_url) {
  poster_url_ = poster_url;
}
void MojoRenderer::SetAttributes(
    base::flat_map<std::string, std::string> attributes) {
  attributes_ = std::move(attributes);
}
void MojoRenderer::SetReferrer(const std::string& referrer) {
  referrer_ = referrer;
}
void MojoRenderer::SetIsAudio(bool is_audio) {
  is_audio_ = is_audio;
}
bool MojoRenderer::IsAudio() {
  return is_audio_;
}

void MojoRenderer::SetPlaybackRateWithReason(double playback_rate,
    ActionReason reason) {
  DVLOG(2) << __func__ << "(" << playback_rate << ")";
  DCHECK(task_runner_->RunsTasksInCurrentSequence());
  DCHECK(remote_renderer_.is_bound());

  remote_renderer_->SetPlaybackRateWithReason(playback_rate,
      static_cast<mojom::ActionReason>(reason));

  {
    base::AutoLock auto_lock(lock_);
    media_time_interpolator_.SetPlaybackRate(playback_rate);
  }
}
#endif // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void MojoRenderer::OnRequestVideoSurfaceDone(int32_t surface_id) {
  LOG(INFO) << "OnRequestVideoSurfaceDone(" << surface_id << ")";
  remote_renderer_->SetVideoSurface(surface_id);
}
#endif // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_PIP)
void MojoRenderer::PipEnable(bool enable) {
  if (remote_renderer_.is_bound()) {
    remote_renderer_->PipEnable(enable);
  } else {
    LOG(ERROR) << "PipEnable failed";
  }
}
#endif

#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
void MojoRenderer::RecycleDmaBuffer() {
  if (remote_renderer_.is_bound()) {
    LOG(INFO) << "DMABUF::MojoRenderer, RecycleDmaBuffer";
    remote_renderer_->RecycleDmaBuffer();
  } else {
    LOG(ERROR) << "DMABUF::MojoRenderer, RecycleDmaBuffer failed";
  }
}

void MojoRenderer::ResumeDmaBuffer() {
  if (remote_renderer_.is_bound()) {
    LOG(INFO) << "DMABUF::MojoRenderer, ResumeDmaBuffer";
    remote_renderer_->ResumeDmaBuffer();
  } else {
    LOG(ERROR) << "DMABUF::MojoRenderer, ResumeDmaBuffer failed";
  }
}
#endif  // ARKWEB_MEDIA_DMABUF
}