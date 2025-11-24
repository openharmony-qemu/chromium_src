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

#ifndef OHOS_NWEB_SRC_NWEB_MEDIA_PLAYER_CLIENT_CALLBACK_H_
#define OHOS_NWEB_SRC_NWEB_MEDIA_PLAYER_CLIENT_CALLBACK_H_

#include "ohos_nweb_ex/public/capi/nweb_basic_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct NWebMediaPlayerCallback {
  int nweb_id;

  void (*on_status_changed)(int32_t nweb_id, uint32_t status) = nullptr;
  void (*on_muted_changed)(int32_t nweb_id, bool muted) = nullptr;
  void (*on_playback_rate_changed)(int32_t nweb_id,
                                   double playback_rate) = nullptr;
  void (*on_duration_changed)(int32_t nweb_id, double duration) = nullptr;
  void (*on_time_update)(int32_t nweb_id, double current_time) = nullptr;
  void (*on_buffered_end_time_changed)(int32_t nweb_id,
                                       double buffered_time) = nullptr;
  void (*on_ended)(int32_t nweb_id) = nullptr;
  void (*on_fullscreen_changed)(int32_t nweb_id, bool fullscreen) = nullptr;
  void (*on_seeking)(int32_t nweb_id) = nullptr;
  void (*on_seek_finished)(int32_t nweb_id) = nullptr;
  void (*on_error)(int32_t nweb_id,
                   uint32_t error_code,
                   const char* error_msg) = nullptr;
  void (*on_video_size_changed)(int32_t nweb_id,
                                int width,
                                int height) = nullptr;
  void (*on_fullscreen_overlay_changed)(int32_t nweb_id,
                                        bool fullscreen_overlay) = nullptr;
  void (*on_volume_changed)(int32_t nweb_id,
                            double volume) = nullptr;
} NWebMediaPlayerCallback;

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // OHOS_NWEB_SRC_NWEB_MEDIA_PLAYER_CLIENT_CALLBACK_H_