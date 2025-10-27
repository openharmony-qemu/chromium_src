// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/browser/alloy/cef_media_player_controller_impl.h"

#include "content/public/browser/media_player_controller.h"

CefMediaPlayerControllerImpl::CefMediaPlayerControllerImpl(
    std::unique_ptr<content::MediaPlayerController> media_player_controller)
    : media_player_controller_(std::move(media_player_controller)) {}

CefMediaPlayerControllerImpl::~CefMediaPlayerControllerImpl() = default;

void CefMediaPlayerControllerImpl::Play() {
  if (media_player_controller_) {
    media_player_controller_->Play();
  }
}
void CefMediaPlayerControllerImpl::Pause() {
  if (media_player_controller_) {
    media_player_controller_->Pause();
  }
}
void CefMediaPlayerControllerImpl::Seek(double time) {
  if (media_player_controller_) {
    media_player_controller_->Seek(time);
  }
}
void CefMediaPlayerControllerImpl::SetMuted(bool muted) {
  if (media_player_controller_) {
    media_player_controller_->SetMuted(muted);
  }
}
void CefMediaPlayerControllerImpl::SetPlaybackRate(double playback_rate) {
  if (media_player_controller_) {
    media_player_controller_->SetPlaybackRate(playback_rate);
  }
}
void CefMediaPlayerControllerImpl::ExitFullscreen() {
  if (media_player_controller_) {
    media_player_controller_->ExitFullscreen();
  }
}
bool CefMediaPlayerControllerImpl::SetVideoSurface(void* native_window) {
  if (media_player_controller_) {
    return media_player_controller_->SetVideoSurface(native_window);
  }
  return false;
}

void CefMediaPlayerControllerImpl::Download() {
  if (media_player_controller_) {
    media_player_controller_->Download();
  }
}

void CefMediaPlayerControllerImpl::SetVolume(double volume)
{
  if (media_player_controller_) {
    media_player_controller_->SetVolume(volume);
  }
}

double CefMediaPlayerControllerImpl::GetVolume()
{
  if (media_player_controller_) {
    return media_player_controller_->GetVolume();
  }
  return -1.0;
}
