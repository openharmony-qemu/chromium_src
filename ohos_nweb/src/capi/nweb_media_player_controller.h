/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_NWEB_SRC_CAPI_NWEB_MEDIA_PLAYER_CONTROLLER_H_
#define OHOS_NWEB_SRC_CAPI_NWEB_MEDIA_PLAYER_CONTROLLER_H_

#include <memory>

#include "ohos_nweb/src/capi/common/common.h"

class NWebMediaPlayerController;

struct NWebMediaPlayerControllerBase {
  const size_t struct_size = sizeof(NWebMediaPlayerControllerBase);
  void (NWebMediaPlayerController::*play)() = nullptr;
  void (NWebMediaPlayerController::*pause)() = nullptr;
  void (NWebMediaPlayerController::*seek)(double time) = nullptr;
  void (NWebMediaPlayerController::*set_muted)(bool muted) = nullptr;
  void (NWebMediaPlayerController::*set_playback_rate)(double) = nullptr;
  void (NWebMediaPlayerController::*exit_fullscreen)() = nullptr;
  void (NWebMediaPlayerController::*set_video_surface)(void*) = nullptr;
  void (NWebMediaPlayerController::*download)() = nullptr;

  NWebMediaPlayerControllerBase() = default;
  NWebMediaPlayerControllerBase(
      const NWebMediaPlayerControllerBase& other) = delete;
  NWebMediaPlayerControllerBase(
      NWebMediaPlayerControllerBase&& other) = delete;
  NWebMediaPlayerControllerBase& operator=(
      const NWebMediaPlayerControllerBase& other) = delete;
  NWebMediaPlayerControllerBase& operator=(
      NWebMediaPlayerControllerBase&& other) = delete;
};

class NWebMediaPlayerController : public NWebMediaPlayerControllerBase {
 public:
  using Base = NWebMediaPlayerControllerBase;
  NWebMediaPlayerController() : NWebMediaPlayerControllerBase() {
    this->play = &NWebMediaPlayerController::Play;
    this->pause = &NWebMediaPlayerController::Pause;
    this->seek = &NWebMediaPlayerController::Seek;
    this->set_muted = &NWebMediaPlayerController::SetMuted;
    this->set_playback_rate = &NWebMediaPlayerController::SetPlaybackRate;
    this->exit_fullscreen = &NWebMediaPlayerController::ExitFullscreen;
    this->set_video_surface = &NWebMediaPlayerController::SetVideoSurface;
    this->download = &NWebMediaPlayerController::Download;
  }
  virtual ~NWebMediaPlayerController() = default;

 protected:
  virtual void Play() {}
  virtual void Pause() {}
  virtual void Seek(double time) {}
  virtual void SetMuted(bool muted) {}
  virtual void SetPlaybackRate(double playback_rate) {}
  virtual void ExitFullscreen() {}
  virtual void SetVideoSurface(void* native_window) {}
  virtual void Download() {}

 private:
  static_assert(offsetof(Base, struct_size) == 0,
      "Must NOT break the order of Base members.");
  static_assert(
      (offsetof(Base, play) - offsetof(Base, struct_size))
          == sizeof(Base::struct_size),
      "Must NOT break the order of Base members.");
  static_assert(
      (offsetof(Base, pause) - offsetof(Base, play))
          == sizeof(Base::play),
      "Must NOT break the order of Base members.");
  static_assert(
      (offsetof(Base, seek) - offsetof(Base, pause))
          == sizeof(Base::pause),
      "Must NOT break the order of Base members.");
  static_assert(
      (offsetof(Base, set_muted) - offsetof(Base, seek))
          == sizeof(Base::seek),
      "Must NOT break the order of Base members.");
  static_assert(
      (offsetof(Base, set_playback_rate) - offsetof(Base, set_muted))
          == sizeof(Base::set_muted),
      "Must NOT break the order of Base members.");
  static_assert(
      (offsetof(Base, exit_fullscreen) - offsetof(Base, set_playback_rate))
          == sizeof(Base::set_playback_rate),
      "Must NOT break the order of Base members.");
  static_assert(
      (offsetof(Base, set_video_surface) - offsetof(Base, exit_fullscreen))
          == sizeof(Base::exit_fullscreen),
      "Must NOT break the order of Base members.");
  static_assert(
      (offsetof(Base, download) - offsetof(Base, set_video_surface))
          == sizeof(Base::set_video_surface),
      "Must NOT break the order of Base members.");
};

#endif  // OHOS_NWEB_SRC_CAPI_NWEB_MEDIA_PLAYER_CONTROLLER_H_

