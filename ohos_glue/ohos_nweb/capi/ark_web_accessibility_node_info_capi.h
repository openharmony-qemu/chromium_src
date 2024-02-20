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

#ifndef ARK_WEB_ACCESSIBILITY_NODE_INFO_CAPI_H_
#define ARK_WEB_ACCESSIBILITY_NODE_INFO_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/include/ark_web_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_web_accessibility_node_info_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  ArkWebString(ARK_WEB_CALLBACK *get_hint)(
      struct _ark_web_accessibility_node_info_t *self);

  ArkWebString(ARK_WEB_CALLBACK *get_error)(
      struct _ark_web_accessibility_node_info_t *self);

  int32_t(ARK_WEB_CALLBACK *get_rect_x)(
      struct _ark_web_accessibility_node_info_t *self);

  int32_t(ARK_WEB_CALLBACK *get_rect_y)(
      struct _ark_web_accessibility_node_info_t *self);

  void(ARK_WEB_CALLBACK *set_page_id)(
      struct _ark_web_accessibility_node_info_t *self, int32_t page_id);

  int32_t(ARK_WEB_CALLBACK *get_page_id)(
      struct _ark_web_accessibility_node_info_t *self);

  ArkWebUint32Vector(ARK_WEB_CALLBACK *get_actions)(
      struct _ark_web_accessibility_node_info_t *self);

  ArkWebString(ARK_WEB_CALLBACK *get_content)(
      struct _ark_web_accessibility_node_info_t *self);

  ArkWebInt64Vector(ARK_WEB_CALLBACK *get_child_ids)(
      struct _ark_web_accessibility_node_info_t *self);

  void(ARK_WEB_CALLBACK *set_parent_id)(
      struct _ark_web_accessibility_node_info_t *self, int64_t parentId_id);

  int64_t(ARK_WEB_CALLBACK *get_parent_id)(
      struct _ark_web_accessibility_node_info_t *self);

  bool(ARK_WEB_CALLBACK *get_is_heading)(
      struct _ark_web_accessibility_node_info_t *self);

  bool(ARK_WEB_CALLBACK *get_is_checked)(
      struct _ark_web_accessibility_node_info_t *self);

  bool(ARK_WEB_CALLBACK *get_is_enabled)(
      struct _ark_web_accessibility_node_info_t *self);

  bool(ARK_WEB_CALLBACK *get_is_focused)(
      struct _ark_web_accessibility_node_info_t *self);

  int32_t(ARK_WEB_CALLBACK *get_rect_width)(
      struct _ark_web_accessibility_node_info_t *self);

  int32_t(ARK_WEB_CALLBACK *get_rect_height)(
      struct _ark_web_accessibility_node_info_t *self);

  bool(ARK_WEB_CALLBACK *get_is_visible)(
      struct _ark_web_accessibility_node_info_t *self);

  bool(ARK_WEB_CALLBACK *get_is_hinting)(
      struct _ark_web_accessibility_node_info_t *self);

  bool(ARK_WEB_CALLBACK *get_is_editable)(
      struct _ark_web_accessibility_node_info_t *self);

  bool(ARK_WEB_CALLBACK *get_is_selected)(
      struct _ark_web_accessibility_node_info_t *self);

  size_t(ARK_WEB_CALLBACK *get_item_counts)(
      struct _ark_web_accessibility_node_info_t *self);

  int32_t(ARK_WEB_CALLBACK *get_live_region)(
      struct _ark_web_accessibility_node_info_t *self);

  bool(ARK_WEB_CALLBACK *get_is_password)(
      struct _ark_web_accessibility_node_info_t *self);

  bool(ARK_WEB_CALLBACK *get_is_checkable)(
      struct _ark_web_accessibility_node_info_t *self);

  bool(ARK_WEB_CALLBACK *get_is_clickable)(
      struct _ark_web_accessibility_node_info_t *self);

  bool(ARK_WEB_CALLBACK *get_is_focusable)(
      struct _ark_web_accessibility_node_info_t *self);

  bool(ARK_WEB_CALLBACK *get_is_scrollable)(
      struct _ark_web_accessibility_node_info_t *self);

  bool(ARK_WEB_CALLBACK *get_is_deletable)(
      struct _ark_web_accessibility_node_info_t *self);

  int64_t(ARK_WEB_CALLBACK *get_accessibility_id)(
      struct _ark_web_accessibility_node_info_t *self);

  bool(ARK_WEB_CALLBACK *get_is_popup_supported)(
      struct _ark_web_accessibility_node_info_t *self);

  bool(ARK_WEB_CALLBACK *get_is_content_invalid)(
      struct _ark_web_accessibility_node_info_t *self);

  int32_t(ARK_WEB_CALLBACK *get_selection_end)(
      struct _ark_web_accessibility_node_info_t *self);

  int32_t(ARK_WEB_CALLBACK *get_selection_start)(
      struct _ark_web_accessibility_node_info_t *self);

  float(ARK_WEB_CALLBACK *get_range_info_min)(
      struct _ark_web_accessibility_node_info_t *self);

  float(ARK_WEB_CALLBACK *get_range_info_max)(
      struct _ark_web_accessibility_node_info_t *self);

  float(ARK_WEB_CALLBACK *get_range_info_current)(
      struct _ark_web_accessibility_node_info_t *self);

  int32_t(ARK_WEB_CALLBACK *get_input_type)(
      struct _ark_web_accessibility_node_info_t *self);

  ArkWebString(ARK_WEB_CALLBACK *get_component_type)(
      struct _ark_web_accessibility_node_info_t *self);

  ArkWebString(ARK_WEB_CALLBACK *get_description_info)(
      struct _ark_web_accessibility_node_info_t *self);

  int32_t(ARK_WEB_CALLBACK *get_grid_rows)(
      struct _ark_web_accessibility_node_info_t *self);

  int32_t(ARK_WEB_CALLBACK *get_grid_item_row)(
      struct _ark_web_accessibility_node_info_t *self);

  int32_t(ARK_WEB_CALLBACK *get_grid_columns)(
      struct _ark_web_accessibility_node_info_t *self);

  int32_t(ARK_WEB_CALLBACK *get_grid_item_column)(
      struct _ark_web_accessibility_node_info_t *self);

  int32_t(ARK_WEB_CALLBACK *get_grid_item_row_span)(
      struct _ark_web_accessibility_node_info_t *self);

  int32_t(ARK_WEB_CALLBACK *get_grid_selected_mode)(
      struct _ark_web_accessibility_node_info_t *self);

  int32_t(ARK_WEB_CALLBACK *get_grid_item_column_span)(
      struct _ark_web_accessibility_node_info_t *self);

  bool(ARK_WEB_CALLBACK *get_is_accessibility_focus)(
      struct _ark_web_accessibility_node_info_t *self);

  bool(ARK_WEB_CALLBACK *get_is_plural_line_supported)(
      struct _ark_web_accessibility_node_info_t *self);
} ark_web_accessibility_node_info_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_WEB_ACCESSIBILITY_NODE_INFO_CAPI_H_
