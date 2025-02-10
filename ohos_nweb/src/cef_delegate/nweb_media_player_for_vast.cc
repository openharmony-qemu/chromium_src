// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos_nweb/src/cef_delegate/nweb_media_player_for_vast.h"

#include <utility>
#include "base/logging.h"
#include "ohos_nweb/src/capi/nweb_media_player_listener.h"

namespace OHOS::NWeb {

NWebMediaPlayerListenerForVAST::NWebMediaPlayerListenerForVAST(
    std::unique_ptr<NWebMediaPlayerListener> nweb_listener)
    : nweb_listener_(std::move(nweb_listener)) {}

NWebMediaPlayerListenerForVAST::~NWebMediaPlayerListenerForVAST() = default;

void NWebMediaPlayerListenerForVAST::OnStatusChanged(uint32_t status) {
  if (!nweb_listener_) {
    return;
  }
  if (!CheckValid(nweb_listener_.get(), &nweb_listener_->on_status_changed)) {
    return;
  }
  (nweb_listener_.get()->*(nweb_listener_->on_status_changed))(status);
}

void NWebMediaPlayerListenerForVAST::OnMutedChanged(bool muted) {
  if (!nweb_listener_) {
    return;
  }
  if (!CheckValid(nweb_listener_.get(), &nweb_listener_->on_muted_changed)) {
    return;
  }
  (nweb_listener_.get()->*(nweb_listener_->on_muted_changed))(muted);
}

void NWebMediaPlayerListenerForVAST::OnPlaybackRateChanged(
    double playback_rate) {
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

void NWebMediaPlayerListenerForVAST::OnDurationChanged(double duration) {
  if (!nweb_listener_) {
    return;
  }
  if (!CheckValid(nweb_listener_.get(), &nweb_listener_->on_duration_changed)) {
    return;
  }
  (nweb_listener_.get()->*(nweb_listener_->on_duration_changed))(duration);
}

void NWebMediaPlayerListenerForVAST::OnTimeUpdate(double current_time) {
  if (!nweb_listener_) {
    return;
  }
  if (!CheckValid(nweb_listener_.get(), &nweb_listener_->on_time_update)) {
    return;
  }
  (nweb_listener_.get()->*(nweb_listener_->on_time_update))(current_time);
}

void NWebMediaPlayerListenerForVAST::OnBufferedEndTimeChanged(
    double buffered_time) {
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

void NWebMediaPlayerListenerForVAST::OnEnded() {
  if (!nweb_listener_) {
    return;
  }
  if (!CheckValid(nweb_listener_.get(), &nweb_listener_->on_ended)) {
    return;
  }
  (nweb_listener_.get()->*(nweb_listener_->on_ended))();
}

void NWebMediaPlayerListenerForVAST::OnFullscreenChanged(bool fullscreen) {
  if (!nweb_listener_) {
    return;
  }
  if (!CheckValid(nweb_listener_.get(),
      &nweb_listener_->on_fullscreen_changed)) {
    return;
  }
  (nweb_listener_.get()->*(nweb_listener_->on_fullscreen_changed))(fullscreen);
}

void NWebMediaPlayerListenerForVAST::OnSeeking() {
  if (!nweb_listener_) {
    return;
  }
  if (!CheckValid(nweb_listener_.get(), &nweb_listener_->on_seeking)) {
    return;
  }
  (nweb_listener_.get()->*(nweb_listener_->on_seeking))();
}

void NWebMediaPlayerListenerForVAST::OnSeekFinished() {
  if (!nweb_listener_) {
    return;
  }
  if (!CheckValid(nweb_listener_.get(), &nweb_listener_->on_seek_finished)) {
    return;
  }
  (nweb_listener_.get()->*(nweb_listener_->on_seek_finished))();
}

void NWebMediaPlayerListenerForVAST::OnError(
    uint32_t error_code,
    const std::string& error_msg) {
  if (!nweb_listener_) {
    return;
  }
  if (!CheckValid(nweb_listener_.get(), &nweb_listener_->on_error)) {
    return;
  }
  (nweb_listener_.get()->*(nweb_listener_->on_error))(error_code, error_msg);
}

void NWebMediaPlayerListenerForVAST::OnVideoSizeChanged(int width, int height) {
  if (!nweb_listener_) {
    return;
  }
  if (!CheckValid(nweb_listener_.get(),
      &nweb_listener_->on_video_size_changed)) {
    return;
  }
  (nweb_listener_.get()->*(nweb_listener_->on_video_size_changed))(
      width, height);
}

void NWebMediaPlayerListenerForVAST::OnFullscreenOverlayChanged(
    bool fullscreen_overlay) {
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
} // namespace OHOS::NWeb
