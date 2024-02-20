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

#ifndef ARK_WEB_DRAG_DATA_CAPI_H_
#define ARK_WEB_DRAG_DATA_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/include/ark_web_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_web_drag_data_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  bool(ARK_WEB_CALLBACK *set_file_uri)(struct _ark_web_drag_data_t *self,
                                       const ArkWebString *uri);

  ArkWebString(ARK_WEB_CALLBACK *get_link_url)(
      struct _ark_web_drag_data_t *self);

  bool(ARK_WEB_CALLBACK *set_link_url)(struct _ark_web_drag_data_t *self,
                                       const ArkWebString *url);

  ArkWebString(ARK_WEB_CALLBACK *get_link_title)(
      struct _ark_web_drag_data_t *self);

  bool(ARK_WEB_CALLBACK *set_link_title)(struct _ark_web_drag_data_t *self,
                                         const ArkWebString *title);

  ArkWebString(ARK_WEB_CALLBACK *get_fragment_text)(
      struct _ark_web_drag_data_t *self);

  bool(ARK_WEB_CALLBACK *set_fragment_text)(struct _ark_web_drag_data_t *self,
                                            const ArkWebString *text);

  ArkWebString(ARK_WEB_CALLBACK *get_fragment_html)(
      struct _ark_web_drag_data_t *self);

  bool(ARK_WEB_CALLBACK *set_fragment_html)(struct _ark_web_drag_data_t *self,
                                            const ArkWebString *html);

  ArkWebString(ARK_WEB_CALLBACK *get_image_file_name)(
      struct _ark_web_drag_data_t *self);

  bool(ARK_WEB_CALLBACK *get_pixel_map_setting)(
      struct _ark_web_drag_data_t *self, const void **data, size_t *len,
      int *width, int *height);

  bool(ARK_WEB_CALLBACK *set_pixel_map_setting)(
      struct _ark_web_drag_data_t *self, const void *data, size_t len,
      int width, int height);

  void(ARK_WEB_CALLBACK *clear_image_file_names)(
      struct _ark_web_drag_data_t *self);

  bool(ARK_WEB_CALLBACK *is_single_image_content)(
      struct _ark_web_drag_data_t *self);

  void(ARK_WEB_CALLBACK *get_drag_start_position)(
      struct _ark_web_drag_data_t *self, int *x, int *y);
} ark_web_drag_data_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_WEB_DRAG_DATA_CAPI_H_
