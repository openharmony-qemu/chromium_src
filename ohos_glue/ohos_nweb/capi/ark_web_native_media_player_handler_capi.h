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

#ifndef ARK_WEB_NATIVE_MEDIA_PLAYER_HANDLER_CAPI_H_
#define ARK_WEB_NATIVE_MEDIA_PLAYER_HANDLER_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/include/ark_web_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_web_native_media_player_handler_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  void(ARK_WEB_CALLBACK *handle_status_changed)(
      struct _ark_web_native_media_player_handler_t *self, int status);

  void(ARK_WEB_CALLBACK *handle_volume_changed)(
      struct _ark_web_native_media_player_handler_t *self, double volume);

  void(ARK_WEB_CALLBACK *handle_muted_changed)(
      struct _ark_web_native_media_player_handler_t *self, bool isMuted);

  void(ARK_WEB_CALLBACK *handle_playback_rate_changed)(
      struct _ark_web_native_media_player_handler_t *self, double playbackRate);

  void(ARK_WEB_CALLBACK *handle_duration_changed)(
      struct _ark_web_native_media_player_handler_t *self, double duration);

  void(ARK_WEB_CALLBACK *handle_time_update)(
      struct _ark_web_native_media_player_handler_t *self, double playTime);

  void(ARK_WEB_CALLBACK *handle_buffered_end_time_changed)(
      struct _ark_web_native_media_player_handler_t *self,
      double bufferedEndTime);

  void(ARK_WEB_CALLBACK *handle_ended)(
      struct _ark_web_native_media_player_handler_t *self);

  void(ARK_WEB_CALLBACK *handle_network_state_changed)(
      struct _ark_web_native_media_player_handler_t *self, int state);

  void(ARK_WEB_CALLBACK *handle_ready_state_changed)(
      struct _ark_web_native_media_player_handler_t *self, int state);

  void(ARK_WEB_CALLBACK *handle_full_screen_changed)(
      struct _ark_web_native_media_player_handler_t *self, bool isFullScreen);

  void(ARK_WEB_CALLBACK *handle_seeking)(
      struct _ark_web_native_media_player_handler_t *self);

  void(ARK_WEB_CALLBACK *handle_seek_finished)(
      struct _ark_web_native_media_player_handler_t *self);

  void(ARK_WEB_CALLBACK *handle_error)(
      struct _ark_web_native_media_player_handler_t *self, int error,
      const ArkWebString *message);

  void(ARK_WEB_CALLBACK *handle_video_size_changed)(
      struct _ark_web_native_media_player_handler_t *self, double width,
      double height);
} ark_web_native_media_player_handler_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_WEB_NATIVE_MEDIA_PLAYER_HANDLER_CAPI_H_
