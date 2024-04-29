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

#ifndef ARK_MEDIA_ADAPTER_CAPI_H_
#define ARK_MEDIA_ADAPTER_CAPI_H_
#pragma once

#include "ohos_adapter/capi/ark_graphic_adapter_capi.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_player_callback_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  void(ARK_WEB_CALLBACK *on_info)(struct _ark_player_callback_adapter_t *self,
                                  int32_t type, int32_t extra, int32_t value);

  void(ARK_WEB_CALLBACK *on_error)(struct _ark_player_callback_adapter_t *self,
                                   int32_t errorType);
} ark_player_callback_adapter_t;

typedef struct _ark_player_adapter_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  int32_t(ARK_WEB_CALLBACK *set_player_callback)(
      struct _ark_player_adapter_t *self,
      ark_player_callback_adapter_t *callbackAdapter);

  int32_t(ARK_WEB_CALLBACK *set_source1)(struct _ark_player_adapter_t *self,
                                         const ArkWebString *url);

  int32_t(ARK_WEB_CALLBACK *set_source2)(struct _ark_player_adapter_t *self,
                                         int32_t fd, int64_t offset,
                                         int64_t size);

  int32_t(ARK_WEB_CALLBACK *set_video_surface)(
      struct _ark_player_adapter_t *self,
      ark_iconsumer_surface_adapter_t *cSurfaceAdapter);

  int32_t(ARK_WEB_CALLBACK *set_volume)(struct _ark_player_adapter_t *self,
                                        float leftVolume, float rightVolume);

  int32_t(ARK_WEB_CALLBACK *seek)(struct _ark_player_adapter_t *self,
                                  int32_t mSeconds, int32_t mode);

  int32_t(ARK_WEB_CALLBACK *play)(struct _ark_player_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *pause)(struct _ark_player_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *prepare_async)(struct _ark_player_adapter_t *self);

  int32_t(ARK_WEB_CALLBACK *get_current_time)(
      struct _ark_player_adapter_t *self, int32_t *currentTime);

  int32_t(ARK_WEB_CALLBACK *get_duration)(struct _ark_player_adapter_t *self,
                                          int32_t *duration);

  int32_t(ARK_WEB_CALLBACK *set_playback_speed)(
      struct _ark_player_adapter_t *self, int32_t mode);
} ark_player_adapter_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_MEDIA_ADAPTER_CAPI_H_
