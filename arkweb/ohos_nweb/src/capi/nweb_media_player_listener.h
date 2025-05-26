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

#ifndef OHOS_NWEB_SRC_CAPI_NWEB_MEDIA_PLAYER_LISTENER_H_
#define OHOS_NWEB_SRC_CAPI_NWEB_MEDIA_PLAYER_LISTENER_H_

#include <memory>

#include "ohos_nweb/src/capi/common/common.h"

template<typename T>

struct NWebMediaPlayerListenerBase {
  const size_t struct_size = sizeof(NWebMediaPlayerListenerBase);
  void (T::*on_status_changed)(uint32_t) = nullptr;
  void (T::*on_muted_changed)(bool) = nullptr;
  void (T::*on_playback_rate_changed)(double) = nullptr;
  void (T::*on_duration_changed)(double) = nullptr;
  void (T::*on_time_update)(double) = nullptr;
  void (T::*on_buffered_end_time_changed)(double) = nullptr;
  void (T::*on_ended)() = nullptr;
  void (T::*on_fullscreen_changed)(bool) = nullptr;
  void (T::*on_seeking)() = nullptr;
  void (T::*on_seek_finished)() = nullptr;
  void (T::*on_error)(uint32_t, const std::string&) = nullptr;
  void (T::*on_video_size_changed)(int, int) = nullptr;
  void (T::*on_fullscreen_overlay_changed)(bool) = nullptr;

  NWebMediaPlayerListenerBase() = default;
  NWebMediaPlayerListenerBase(
      const NWebMediaPlayerListenerBase& other) = delete;
  NWebMediaPlayerListenerBase(
      NWebMediaPlayerListenerBase&& other) = delete;
  NWebMediaPlayerListenerBase& operator=(
      const NWebMediaPlayerListenerBase& other) = delete;
  NWebMediaPlayerListenerBase& operator=(
      NWebMediaPlayerListenerBase&& other) = delete;
};

class NWebMediaPlayerListener : public NWebMediaPlayerListenerBase
<NWebMediaPlayerListener> {
 public:
  using Base = NWebMediaPlayerListenerBase<NWebMediaPlayerListener>;
  NWebMediaPlayerListener() : NWebMediaPlayerListenerBase<NWebMediaPlayerListener>() {
    this->on_status_changed = &NWebMediaPlayerListener::OnStatusChanged;
    this->on_muted_changed = &NWebMediaPlayerListener::OnMutedChanged;
    this->on_playback_rate_changed = &NWebMediaPlayerListener::OnPlaybackRateChanged;
    this->on_duration_changed = &NWebMediaPlayerListener::OnDurationChanged;
    this->on_time_update = &NWebMediaPlayerListener::OnTimeUpdate;
    this->on_buffered_end_time_changed = &NWebMediaPlayerListener::OnBufferedEndTimeChanged;
    this->on_ended = &NWebMediaPlayerListener::OnEnded;
    this->on_fullscreen_changed = &NWebMediaPlayerListener::OnFullscreenChanged;
    this->on_seeking = &NWebMediaPlayerListener::OnSeeking;
    this->on_seek_finished = &NWebMediaPlayerListener::OnSeekFinished;
    this->on_error = &NWebMediaPlayerListener::OnError;
    this->on_video_size_changed = &NWebMediaPlayerListener::OnVideoSizeChanged;
    this->on_fullscreen_overlay_changed =
        &NWebMediaPlayerListener::OnFullscreenOverlayChanged;
  }
  virtual ~NWebMediaPlayerListener() = default;

 protected:
  virtual void OnStatusChanged(uint32_t status) {}
  virtual void OnMutedChanged(bool muted) {}
  virtual void OnPlaybackRateChanged(double playback_rate) {}
  virtual void OnDurationChanged(double duration) {}
  virtual void OnTimeUpdate(double current_time) {}
  virtual void OnBufferedEndTimeChanged(double buffered_time) {}
  virtual void OnEnded() {}
  virtual void OnFullscreenChanged(bool fullscreen) {}
  virtual void OnSeeking() {}
  virtual void OnSeekFinished() {}
  virtual void OnError(uint32_t error_code, const std::string& error_msg) {}
  virtual void OnVideoSizeChanged(int width, int height) {}
  virtual void OnFullscreenOverlayChanged(bool fullscreen_overlay) {}

 private:
  static_assert(offsetof(Base, struct_size) == 0,
      "Must NOT break the order of Base members.");
  static_assert(
      (offsetof(Base, on_status_changed) - offsetof(Base, struct_size))
          == sizeof(Base::struct_size),
      "Must NOT break the order of Base members.");
  static_assert(
      (offsetof(Base, on_muted_changed) - offsetof(Base, on_status_changed))
          == sizeof(Base::on_status_changed),
      "Must NOT break the order of Base members.");
  static_assert(
      (offsetof(Base, on_playback_rate_changed) - offsetof(Base, on_muted_changed))
          == sizeof(Base::on_muted_changed),
      "Must NOT break the order of Base members.");
  static_assert(
      (offsetof(Base, on_duration_changed) - offsetof(Base, on_playback_rate_changed))
          == sizeof(Base::on_playback_rate_changed),
      "Must NOT break the order of Base members.");
  static_assert(
      (offsetof(Base, on_time_update) - offsetof(Base, on_duration_changed))
          == sizeof(Base::on_duration_changed),
      "Must NOT break the order of Base members.");
  static_assert(
      (offsetof(Base, on_buffered_end_time_changed) - offsetof(Base, on_time_update))
          == sizeof(Base::on_time_update),
      "Must NOT break the order of Base members.");
  static_assert(
      (offsetof(Base, on_ended) - offsetof(Base, on_buffered_end_time_changed))
          == sizeof(Base::on_buffered_end_time_changed),
      "Must NOT break the order of Base members.");
  static_assert(
      (offsetof(Base, on_fullscreen_changed) - offsetof(Base, on_ended))
          == sizeof(Base::on_ended),
      "Must NOT break the order of Base members.");
  static_assert(
      (offsetof(Base, on_seeking) - offsetof(Base, on_fullscreen_changed))
          == sizeof(Base::on_fullscreen_changed),
      "Must NOT break the order of Base members.");
  static_assert(
      (offsetof(Base, on_seek_finished) - offsetof(Base, on_seeking))
          == sizeof(Base::on_seeking),
      "Must NOT break the order of Base members.");
  static_assert(
      (offsetof(Base, on_error) - offsetof(Base, on_seek_finished))
          == sizeof(Base::on_seek_finished),
      "Must NOT break the order of Base members.");
  static_assert(
      (offsetof(Base, on_video_size_changed) - offsetof(Base, on_error))
          == sizeof(Base::on_error),
      "Must NOT break the order of Base members.");
  static_assert(
      (offsetof(Base, on_fullscreen_overlay_changed) - offsetof(Base, on_video_size_changed))
          == sizeof(Base::on_video_size_changed),
      "Must NOT break the order of Base members.");
};

#endif  // OHOS_NWEB_SRC_CAPI_NWEB_MEDIA_PLAYER_LISTENER_H_
