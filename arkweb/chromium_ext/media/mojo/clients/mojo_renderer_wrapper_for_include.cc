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

namespace media {

#if BUILDFLAG(ARKWEB_MEDIA)
void MojoRendererWrapper::SetNativeWindowSurface(int native_window_id) {
  mojo_renderer_->SetNativeWindowSurface(native_window_id);
}
#endif  // ARKWEB_MEDIA

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
void MojoRendererWrapper::SetMuted(bool muted) {
  mojo_renderer_->SetMuted(muted);
}
void MojoRendererWrapper::SetSurfaceId(int surface_id, const gfx::Rect& rect) {
  mojo_renderer_->SetSurfaceId(surface_id, rect);
}
void MojoRendererWrapper::SetMediaPlayerState(bool is_suspend,
                                              int suspend_type) {
  mojo_renderer_->SetMediaPlayerState(is_suspend, suspend_type);
}
void MojoRendererWrapper::SetMediaSourceList(
    const std::vector<MediaSourceInfo>& source_infos) {
  mojo_renderer_->SetMediaSourceList(source_infos);
}
void MojoRendererWrapper::SetMediaControls(
    bool show_media_controls,
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
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_PIP)
void MojoRendererWrapper::PipEnable(bool enable) {
  mojo_renderer_->PipEnable(enable);
}
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void MojoRendererWrapper::SetPreciseSeekTarget(int64_t target_timestamp) {
  mojo_renderer_->SetPreciseSeekTarget(target_timestamp);
}
#endif // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
// LCOV_EXCL_START
void MojoRendererWrapper::RecycleDmaBuffer() {
  if (mojo_renderer_) {
    LOG(INFO) << "DMABUF::MojoRendererWrapper, RecycleDmaBuffer";
    mojo_renderer_->RecycleDmaBuffer();
  }
}

void MojoRendererWrapper::ResumeDmaBuffer() {
  if (mojo_renderer_) {
    LOG(INFO) << "DMABUF::MojoRendererWrapper, ResumeDmaBuffer";
    mojo_renderer_->ResumeDmaBuffer();
  }
}
// LCOV_EXCL_STOP
#endif  // ARKWEB_MEDIA_DMABUF
}  // namespace media
