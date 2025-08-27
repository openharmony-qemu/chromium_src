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

#include "ohos_nweb/src/native_media_player/nweb_native_media_player_handler_impl.h"

namespace OHOS::NWeb {

NWebNativeMediaPlayerHandlerImpl::NWebNativeMediaPlayerHandlerImpl(
    CefOwnPtr<CefMediaPlayerListener> listener)
    : listener_(std::move(listener)) {}

NWebNativeMediaPlayerHandlerImpl::~NWebNativeMediaPlayerHandlerImpl() = default;

void NWebNativeMediaPlayerHandlerImpl::HandleStatusChanged(
    PlaybackStatus status) {
  if (listener_) {
    listener_->OnStatusChanged(static_cast<uint32_t>(status));
  }
}

void NWebNativeMediaPlayerHandlerImpl::HandleVolumeChanged(double volume) {
  if (listener_) {
    listener_->OnVolumeChanged(volume);
  }
}

void NWebNativeMediaPlayerHandlerImpl::HandleMutedChanged(bool muted) {
  if (listener_) {
    listener_->OnMutedChanged(muted);
  }
}
void NWebNativeMediaPlayerHandlerImpl::HandlePlaybackRateChanged(
    double playback_rate) {
  if (listener_) {
    listener_->OnPlaybackRateChanged(playback_rate);
  }
}
void NWebNativeMediaPlayerHandlerImpl::HandleDurationChanged(double duration) {
  if (listener_) {
    listener_->OnDurationChanged(duration);
  }
}
void NWebNativeMediaPlayerHandlerImpl::HandleTimeUpdate(double current_time) {
  if (listener_) {
    listener_->OnTimeUpdate(current_time);
  }
}
void NWebNativeMediaPlayerHandlerImpl::HandleBufferedEndTimeChanged(
    double buffered_time) {
  if (listener_) {
    listener_->OnBufferedEndTimeChanged(buffered_time);
  }
}
void NWebNativeMediaPlayerHandlerImpl::HandleEnded() {
  if (listener_) {
    listener_->OnEnded();
  }
}
void NWebNativeMediaPlayerHandlerImpl::HandleNetworkStateChanged(
    NetworkState state) {
  if (listener_) {
    listener_->OnNetworkStateChanged(static_cast<uint32_t>(state));
  }
}
void NWebNativeMediaPlayerHandlerImpl::HandleReadyStateChanged(
    ReadyState state) {
  if (listener_) {
    listener_->OnReadyStateChanged(static_cast<int>(state));
  }
}
void NWebNativeMediaPlayerHandlerImpl::HandleFullScreenChanged(
    bool fullscreen) {
  if (listener_) {
    listener_->OnFullscreenChanged(fullscreen);
  }
}
void NWebNativeMediaPlayerHandlerImpl::HandleSeeking() {
  if (listener_) {
    listener_->OnSeeking();
  }
}
void NWebNativeMediaPlayerHandlerImpl::HandleSeekFinished() {
  if (listener_) {
    listener_->OnSeekFinished();
  }
}
void NWebNativeMediaPlayerHandlerImpl::HandleError(
    MediaError error_code,
    const std::string& error_msg) {
  if (listener_) {
    listener_->OnError(static_cast<uint32_t>(error_code), error_msg);
  }
}
void NWebNativeMediaPlayerHandlerImpl::HandleVideoSizeChanged(double width,
                                                              double height) {
  if (listener_) {
    listener_->OnVideoSizeChanged(static_cast<int>(width),
                                  static_cast<int>(height));
  }
}

}  // namespace OHOS::NWeb
