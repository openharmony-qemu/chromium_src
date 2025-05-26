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

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
void MojoRendererService::SetMuted(bool muted) {
  renderer_->SetMuted(muted);
}
void MojoRendererService::SetSurfaceId(int surface_id, const gfx::Rect& rect) {
  renderer_->SetSurfaceId(surface_id, rect);
}
void MojoRendererService::SetMediaPlayerState(bool is_suspend,
                                              int suspend_type) {
  renderer_->SetMediaPlayerState(is_suspend, suspend_type);
}
void MojoRendererService::SetMediaSourceList(
    std::vector<mojom::MediaSourceInfoPtr> source_infos) {
  std::vector<media::Renderer::MediaSourceInfo> infos;
  infos.reserve(source_infos.size());
  for (const auto& info : source_infos) {
    infos.push_back({info->media_source, info->media_format});
  }
  renderer_->SetMediaSourceList(infos);
}
void MojoRendererService::SetMediaControls(
    bool show_media_controls,
    const std::vector<std::string>& controls_list) {
  renderer_->SetMediaControls(show_media_controls, controls_list);
}
void MojoRendererService::SetPoster(const std::string& poster_url) {
  renderer_->SetPoster(poster_url);
}
void MojoRendererService::SetAttributes(
    const base::flat_map<std::string, std::string>& attributes) {
  renderer_->SetAttributes(attributes);
}
void MojoRendererService::SetReferrer(const std::string& referrer) {
  renderer_->SetReferrer(referrer);
}
void MojoRendererService::SetIsAudio(bool is_audio) {
  renderer_->SetIsAudio(is_audio);
}
void MojoRendererService::SetPlaybackRateWithReason(
    double playback_rate,
    mojom::ActionReason reason) {
  renderer_->SetPlaybackRateWithReason(playback_rate,
                                       static_cast<ActionReason>(reason));
}
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void MojoRendererService::SetVideoSurface(int32_t surface_id) {
  renderer_->SetVideoSurface(surface_id);
}
#endif  // OHOS_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_PIP)
void MojoRendererService::PipEnable(bool enable) {
  renderer_->PipEnable(enable);
}
#endif

#if BUILDFLAG(ARKWEB_MEDIA)
void MojoRendererService::SetNativeWindowSurface(int32_t native_window_id) {
  renderer_->SetNativeWindowSurface(native_window_id);
}
#endif  // ARKWEB_MEDIA
