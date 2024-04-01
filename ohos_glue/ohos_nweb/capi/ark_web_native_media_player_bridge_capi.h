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

#ifndef ARK_WEB_NATIVE_MEDIA_PLAYER_BRIDGE_CAPI_H_
#define ARK_WEB_NATIVE_MEDIA_PLAYER_BRIDGE_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_web_native_media_player_bridge_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  void(ARK_WEB_CALLBACK *update_rect)(
      struct _ark_web_native_media_player_bridge_t *self, double x, double y,
      double width, double height);

  void(ARK_WEB_CALLBACK *play)(
      struct _ark_web_native_media_player_bridge_t *self);

  void(ARK_WEB_CALLBACK *pause)(
      struct _ark_web_native_media_player_bridge_t *self);

  void(ARK_WEB_CALLBACK *seek)(
      struct _ark_web_native_media_player_bridge_t *self, double time);

  void(ARK_WEB_CALLBACK *set_volume)(
      struct _ark_web_native_media_player_bridge_t *self, double volume);

  void(ARK_WEB_CALLBACK *set_muted)(
      struct _ark_web_native_media_player_bridge_t *self, bool isMuted);

  void(ARK_WEB_CALLBACK *set_playback_rate)(
      struct _ark_web_native_media_player_bridge_t *self, double playbackRate);

  void(ARK_WEB_CALLBACK *release)(
      struct _ark_web_native_media_player_bridge_t *self);

  void(ARK_WEB_CALLBACK *enter_full_screen)(
      struct _ark_web_native_media_player_bridge_t *self);

  void(ARK_WEB_CALLBACK *exit_full_screen)(
      struct _ark_web_native_media_player_bridge_t *self);
} ark_web_native_media_player_bridge_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_WEB_NATIVE_MEDIA_PLAYER_BRIDGE_CAPI_H_
