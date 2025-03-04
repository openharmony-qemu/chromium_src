// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos_nweb/src/video_assistant/nweb_media_player_controller_impl.h"

#include <utility>
#include <cstdint>
#include "base/logging.h"
#include "cef/include/cef_media_player_controller.h"
#include "gpu/ipc/common/nweb_native_window_tracker.h"

namespace OHOS::NWeb {

NWebMediaPlayerControllerImpl::NWebMediaPlayerControllerImpl(
    std::unique_ptr<CefMediaPlayerController> controller)
    : cef_controller_(std::move(controller)) {}

NWebMediaPlayerControllerImpl::~NWebMediaPlayerControllerImpl() = default;

void NWebMediaPlayerControllerImpl::Play() {
  LOG(INFO) << "NWebMediaPlayerControllerImpl::Play()";
  if (cef_controller_) {
    cef_controller_->Play();
  }
}

void NWebMediaPlayerControllerImpl::Pause() {
  LOG(INFO) << "NWebMediaPlayerControllerImpl::Pause()";
  if (cef_controller_) {
    cef_controller_->Pause();
  }
}

void NWebMediaPlayerControllerImpl::Seek(double time) {
  LOG(INFO) << "NWebMediaPlayerControllerImpl::Seek(" << time << ")";
  if (cef_controller_) {
    cef_controller_->Seek(time);
  }
}

void NWebMediaPlayerControllerImpl::SetMuted(bool muted) {
  LOG(INFO) << "NWebMediaPlayerControllerImpl::SetMuted(" << muted << ")";
  if (cef_controller_) {
    cef_controller_->SetMuted(muted);
  }
}

void NWebMediaPlayerControllerImpl::SetPlaybackRate(double playback_rate) {
  LOG(INFO) << "NWebMediaPlayerControllerImpl::SetPlaybackRate(" << playback_rate << ")";
  if (cef_controller_) {
    cef_controller_->SetPlaybackRate(playback_rate);
  }
}

void NWebMediaPlayerControllerImpl::ExitFullscreen() {
  LOG(INFO) << "NWebMediaPlayerControllerImpl::ExitFullscreen()";
  if (cef_controller_) {
    cef_controller_->ExitFullscreen();
  }
}

void NWebMediaPlayerControllerImpl::SetVideoSurface(void* native_window) {
  LOG(INFO) << "NWebMediaPlayerControllerImpl::SetVideoSurface(" << native_window << ")";
  bool success = false;
  if (cef_controller_) {
    success = cef_controller_->SetVideoSurface(native_window);
  }
  if (!success) {
    OHOS::NWeb::OhosAdapterHelper::GetInstance()
        .GetWindowAdapterInstance()
        .DestroyNativeWindow(native_window);
  }
}

void NWebMediaPlayerControllerImpl::Download() {
  LOG(INFO) << "NWebMediaPlayerControllerImpl::Download()";
  if (cef_controller_) {
    cef_controller_->Download();
  }
}
} // namespace OHOS::NWeb
