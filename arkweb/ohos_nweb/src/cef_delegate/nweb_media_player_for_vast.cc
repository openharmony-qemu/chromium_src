// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos_nweb/src/cef_delegate/nweb_media_player_for_vast.h"

#include <utility>

#include "base/logging.h"
#include "ohos_nweb/src/capi/nweb_media_player_callback.h"
#include "ohos_nweb/src/capi/nweb_media_player_listener.h"

namespace OHOS::NWeb {

NWebMediaPlayerListenerForVAST::NWebMediaPlayerListenerForVAST(
    std::unique_ptr<NWebMediaPlayerListener> nweb_listener)
    : nweb_listener_(std::move(nweb_listener)) {}

NWebMediaPlayerListenerForVAST::NWebMediaPlayerListenerForVAST(
    std::unique_ptr<NWebMediaPlayerCallback> arkweb_listener)
    : arkweb_listener_(std::move(arkweb_listener)) {}

NWebMediaPlayerListenerForVAST::~NWebMediaPlayerListenerForVAST() = default;

NO_SANITIZE("cfi-icall")
void NWebMediaPlayerListenerForVAST::OnStatusChanged(uint32_t status) {
  if (arkweb_listener_ && arkweb_listener_.get()->on_status_changed) {
    (arkweb_listener_.get()->on_status_changed)(arkweb_listener_->nweb_id,
                                                status);
    return;
  }
  if (!nweb_listener_) {
    return;
  }
  if (!CheckValid(nweb_listener_.get(), &nweb_listener_->on_status_changed)) {
    return;
  }
  (nweb_listener_.get()->*(nweb_listener_->on_status_changed))(status);
}

NO_SANITIZE("cfi-icall")
void NWebMediaPlayerListenerForVAST::OnMutedChanged(bool muted) {
  if (arkweb_listener_ && arkweb_listener_.get()->on_muted_changed) {
    (arkweb_listener_.get()->on_muted_changed)(arkweb_listener_->nweb_id,
                                               muted);
    return;
  }
  if (!nweb_listener_) {
    return;
  }
  if (!CheckValid(nweb_listener_.get(), &nweb_listener_->on_muted_changed)) {
    return;
  }
  (nweb_listener_.get()->*(nweb_listener_->on_muted_changed))(muted);
}

NO_SANITIZE("cfi-icall")
void NWebMediaPlayerListenerForVAST::OnPlaybackRateChanged(
    double playback_rate) {
  if (arkweb_listener_ && arkweb_listener_.get()->on_playback_rate_changed) {
    (arkweb_listener_.get()->on_playback_rate_changed)(
        arkweb_listener_->nweb_id, playback_rate);
    return;
  }
  if (!nweb_listener_) {
    return;
  }
  if (!CheckValid(nweb_listener_.get(),
                  &nweb_listener_->on_playback_rate_changed)) {
    return;
  }
  (nweb_listener_.get()->*(nweb_listener_->on_playback_rate_changed))(
      playback_rate);
}

NO_SANITIZE("cfi-icall")
void NWebMediaPlayerListenerForVAST::OnDurationChanged(double duration) {
  if (arkweb_listener_ && arkweb_listener_.get()->on_duration_changed) {
    (arkweb_listener_.get()->on_duration_changed)(arkweb_listener_->nweb_id,
                                                  duration);
    return;
  }
  if (!nweb_listener_) {
    return;
  }
  if (!CheckValid(nweb_listener_.get(), &nweb_listener_->on_duration_changed)) {
    return;
  }
  (nweb_listener_.get()->*(nweb_listener_->on_duration_changed))(duration);
}

NO_SANITIZE("cfi-icall")
void NWebMediaPlayerListenerForVAST::OnTimeUpdate(double current_time) {
  if (arkweb_listener_ && arkweb_listener_.get()->on_time_update) {
    (arkweb_listener_.get()->on_time_update)(arkweb_listener_->nweb_id,
                                             current_time);
    return;
  }
  if (!nweb_listener_) {
    return;
  }
  if (!CheckValid(nweb_listener_.get(), &nweb_listener_->on_time_update)) {
    return;
  }
  (nweb_listener_.get()->*(nweb_listener_->on_time_update))(current_time);
}

NO_SANITIZE("cfi-icall")
void NWebMediaPlayerListenerForVAST::OnBufferedEndTimeChanged(
    double buffered_time) {
  if (arkweb_listener_ &&
      arkweb_listener_.get()->on_buffered_end_time_changed) {
    (arkweb_listener_.get()->on_buffered_end_time_changed)(
        arkweb_listener_->nweb_id, buffered_time);
    return;
  }
  if (!nweb_listener_) {
    return;
  }
  if (!CheckValid(nweb_listener_.get(),
                  &nweb_listener_->on_buffered_end_time_changed)) {
    return;
  }
  (nweb_listener_.get()->*(nweb_listener_->on_buffered_end_time_changed))(
      buffered_time);
}

NO_SANITIZE("cfi-icall")
void NWebMediaPlayerListenerForVAST::OnEnded() {
  if (arkweb_listener_ && arkweb_listener_.get()->on_ended) {
    (arkweb_listener_.get()->on_ended)(arkweb_listener_->nweb_id);
    return;
  }
  if (!nweb_listener_) {
    return;
  }
  if (!CheckValid(nweb_listener_.get(), &nweb_listener_->on_ended)) {
    return;
  }
  (nweb_listener_.get()->*(nweb_listener_->on_ended))();
}

NO_SANITIZE("cfi-icall")
void NWebMediaPlayerListenerForVAST::OnFullscreenChanged(bool fullscreen) {
  if (arkweb_listener_ && arkweb_listener_.get()->on_fullscreen_changed) {
    (arkweb_listener_.get()->on_fullscreen_changed)(arkweb_listener_->nweb_id,
                                                    fullscreen);
    return;
  }
  if (!nweb_listener_) {
    return;
  }
  if (!CheckValid(nweb_listener_.get(),
                  &nweb_listener_->on_fullscreen_changed)) {
    return;
  }
  (nweb_listener_.get()->*(nweb_listener_->on_fullscreen_changed))(fullscreen);
}

NO_SANITIZE("cfi-icall")
void NWebMediaPlayerListenerForVAST::OnSeeking() {
  if (arkweb_listener_ && arkweb_listener_.get()->on_seeking) {
    (arkweb_listener_.get()->on_seeking)(arkweb_listener_->nweb_id);
    return;
  }
  if (!nweb_listener_) {
    return;
  }
  if (!CheckValid(nweb_listener_.get(), &nweb_listener_->on_seeking)) {
    return;
  }
  (nweb_listener_.get()->*(nweb_listener_->on_seeking))();
}

NO_SANITIZE("cfi-icall")
void NWebMediaPlayerListenerForVAST::OnSeekFinished() {
  if (arkweb_listener_ && arkweb_listener_.get()->on_seek_finished) {
    (arkweb_listener_.get()->on_seek_finished)(arkweb_listener_->nweb_id);
    return;
  }
  if (!nweb_listener_) {
    return;
  }
  if (!CheckValid(nweb_listener_.get(), &nweb_listener_->on_seek_finished)) {
    return;
  }
  (nweb_listener_.get()->*(nweb_listener_->on_seek_finished))();
}

NO_SANITIZE("cfi-icall")
void NWebMediaPlayerListenerForVAST::OnError(uint32_t error_code,
                                             const std::string& error_msg) {
  if (arkweb_listener_ && arkweb_listener_.get()->on_error) {
    (arkweb_listener_.get()->on_error)(arkweb_listener_->nweb_id, error_code,
                                       error_msg.c_str());
    return;
  }
  if (!nweb_listener_) {
    return;
  }
  if (!CheckValid(nweb_listener_.get(), &nweb_listener_->on_error)) {
    return;
  }
  (nweb_listener_.get()->*(nweb_listener_->on_error))(error_code, error_msg);
}

NO_SANITIZE("cfi-icall")
void NWebMediaPlayerListenerForVAST::OnVideoSizeChanged(int width, int height) {
  if (arkweb_listener_ && arkweb_listener_.get()->on_video_size_changed) {
    (arkweb_listener_.get()->on_video_size_changed)(arkweb_listener_->nweb_id,
                                                    width, height);
    return;
  }
  if (!nweb_listener_) {
    return;
  }
  if (!CheckValid(nweb_listener_.get(),
                  &nweb_listener_->on_video_size_changed)) {
    return;
  }
  (nweb_listener_.get()->*(nweb_listener_->on_video_size_changed))(width,
                                                                   height);
}

NO_SANITIZE("cfi-icall")
void NWebMediaPlayerListenerForVAST::OnFullscreenOverlayChanged(
    bool fullscreen_overlay) {
  if (arkweb_listener_ &&
      arkweb_listener_.get()->on_fullscreen_overlay_changed) {
    (arkweb_listener_.get()->on_fullscreen_overlay_changed)(
        arkweb_listener_->nweb_id, fullscreen_overlay);
    return;
  }
  if (!nweb_listener_) {
    LOG(INFO) << "nweb_listener_ is null";
    return;
  }
  if (!CheckValid(nweb_listener_.get(),
                  &nweb_listener_->on_fullscreen_overlay_changed)) {
    LOG(INFO) << "on_fullscreen_overlay_changed is invalid";
    return;
  }
  (nweb_listener_.get()->*(nweb_listener_->on_fullscreen_overlay_changed))(
      fullscreen_overlay);
}

NO_SANITIZE("cfi-icall")
void NWebMediaPlayerListenerForVAST::OnVolumeChanged(double volume)
{
  LOG(INFO) << "NWebMediaPlayerListenerForVAST::OnVolumeChanged volume = " << volume;
  if (arkweb_listener_ && arkweb_listener_.get()->on_volume_changed) {
    (arkweb_listener_.get()->on_volume_changed)(
        arkweb_listener_->nweb_id, volume);
    return;
  }
  if (!nweb_listener_) {
    LOG(INFO) << "nweb_listener_ is null";
    return;
  }
  if (!CheckValid(nweb_listener_.get(),
      &nweb_listener_->on_volume_changed)) {
    LOG(INFO) << "on_volume_changed is invalid";
    return;
  }
  (nweb_listener_.get()->*(nweb_listener_->on_volume_changed))(volume);
}
} // namespace OHOS::NWeb
