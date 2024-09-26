/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "nweb_accessibility_node_info_impl.h"

namespace OHOS::NWeb {

void NWebAccessibilityNodeInfoImpl::SetHint(const std::string &hint) {
  hint_ = hint;
}

std::string NWebAccessibilityNodeInfoImpl::GetHint() {
  return hint_;
}

void NWebAccessibilityNodeInfoImpl::SetError(const std::string &error) {
  error_ = error;
}

std::string NWebAccessibilityNodeInfoImpl::GetError() {
  return error_;
}

void NWebAccessibilityNodeInfoImpl::SetRectX(int32_t rect_x) {
  rect_x_ = rect_x;
}

int32_t NWebAccessibilityNodeInfoImpl::GetRectX() {
  return rect_x_;
}

void NWebAccessibilityNodeInfoImpl::SetRectY(int32_t rect_y) {
  rect_y_ = rect_y;
}

int32_t NWebAccessibilityNodeInfoImpl::GetRectY() {
  return rect_y_;
}

void NWebAccessibilityNodeInfoImpl::SetPageId(int32_t page_id) {
  page_id_ = page_id;
}

int32_t NWebAccessibilityNodeInfoImpl::GetPageId() {
  return page_id_;
}

void NWebAccessibilityNodeInfoImpl::SetActions(
    const std::vector<uint32_t> &actions) {
  actions_ = actions;
}

std::vector<uint32_t> NWebAccessibilityNodeInfoImpl::GetActions() {
  return actions_;
}

void NWebAccessibilityNodeInfoImpl::SetContent(const std::string &content) {
  content_ = content;
}

std::string NWebAccessibilityNodeInfoImpl::GetContent() {
  return content_;
}

void NWebAccessibilityNodeInfoImpl::SetChildIds(
    const std::vector<int64_t> &child_ids) {
  child_ids_ = child_ids;
}

std::vector<int64_t> NWebAccessibilityNodeInfoImpl::GetChildIds() {
  return child_ids_;
}

void NWebAccessibilityNodeInfoImpl::SetParentId(int64_t parent_id) {
  parent_id_ = parent_id;
}

int64_t NWebAccessibilityNodeInfoImpl::GetParentId() {
  return parent_id_;
}

void NWebAccessibilityNodeInfoImpl::SetIsHeading(bool heading) {
  heading_ = heading;
}

bool NWebAccessibilityNodeInfoImpl::GetIsHeading() {
  return heading_;
}

void NWebAccessibilityNodeInfoImpl::SetIsChecked(bool checked) {
  checked_ = checked;
}

bool NWebAccessibilityNodeInfoImpl::GetIsChecked() {
  return checked_;
}

void NWebAccessibilityNodeInfoImpl::SetIsEnabled(bool enabled) {
  enabled_ = enabled;
}

bool NWebAccessibilityNodeInfoImpl::GetIsEnabled() {
  return enabled_;
}

void NWebAccessibilityNodeInfoImpl::SetIsFocused(bool focused) {
  focused_ = focused;
}

bool NWebAccessibilityNodeInfoImpl::GetIsFocused() {
  return focused_;
}

void NWebAccessibilityNodeInfoImpl::SetRectWidth(int32_t rect_width) {
  rect_width_ = rect_width;
}

int32_t NWebAccessibilityNodeInfoImpl::GetRectWidth() {
  return rect_width_;
}

void NWebAccessibilityNodeInfoImpl::SetRectHeight(int32_t rect_height) {
  rect_height_ = rect_height;
}

int32_t NWebAccessibilityNodeInfoImpl::GetRectHeight() {
  return rect_height_;
}

void NWebAccessibilityNodeInfoImpl::SetIsVisible(bool visible) {
  visible_ = visible;
}

bool NWebAccessibilityNodeInfoImpl::GetIsVisible() {
  return visible_;
}

void NWebAccessibilityNodeInfoImpl::SetIsHinting(bool hinting) {
  hinting_ = hinting;
}

bool NWebAccessibilityNodeInfoImpl::GetIsHinting() {
  return hinting_;
}

void NWebAccessibilityNodeInfoImpl::SetIsEditable(bool editable) {
  editable_ = editable;
}

bool NWebAccessibilityNodeInfoImpl::GetIsEditable() {
  return editable_;
}

void NWebAccessibilityNodeInfoImpl::SetIsSelected(bool selected) {
  selected_ = selected;
}

bool NWebAccessibilityNodeInfoImpl::GetIsSelected() {
  return selected_;
}

void NWebAccessibilityNodeInfoImpl::SetItemCounts(size_t item_counts) {
  item_counts_ = item_counts;
}

size_t NWebAccessibilityNodeInfoImpl::GetItemCounts() {
  return item_counts_;
}

void NWebAccessibilityNodeInfoImpl::SetLiveRegion(int32_t live_region) {
  live_region_ = live_region;
}

int32_t NWebAccessibilityNodeInfoImpl::GetLiveRegion() {
  return live_region_;
}

void NWebAccessibilityNodeInfoImpl::SetIsPassword(bool password) {
  password_ = password;
}

bool NWebAccessibilityNodeInfoImpl::GetIsPassword() {
  return password_;
}

void NWebAccessibilityNodeInfoImpl::SetIsCheckable(bool checkable) {
  checkable_ = checkable;
}

bool NWebAccessibilityNodeInfoImpl::GetIsCheckable() {
  return checkable_;
}

void NWebAccessibilityNodeInfoImpl::SetIsClickable(bool clickable) {
  clickable_ = clickable;
}

bool NWebAccessibilityNodeInfoImpl::GetIsClickable() {
  return clickable_;
}

void NWebAccessibilityNodeInfoImpl::SetIsFocusable(bool focusable) {
  focusable_ = focusable;
}

bool NWebAccessibilityNodeInfoImpl::GetIsFocusable() {
  return focusable_;
}

void NWebAccessibilityNodeInfoImpl::SetIsScrollable(bool scrollable) {
  scrollable_ = scrollable;
}

bool NWebAccessibilityNodeInfoImpl::GetIsScrollable() {
  return scrollable_;
}

void NWebAccessibilityNodeInfoImpl::SetIsDeletable(bool deletable) {
  deletable_ = deletable;
}

bool NWebAccessibilityNodeInfoImpl::GetIsDeletable() {
  return deletable_;
}

void NWebAccessibilityNodeInfoImpl::SetAccessibilityId(
    int64_t accessibility_id) {
  accessibility_id_ = accessibility_id;
}

int64_t NWebAccessibilityNodeInfoImpl::GetAccessibilityId() {
  return accessibility_id_;
}

void NWebAccessibilityNodeInfoImpl::SetIsPopupSupported(bool popup_supported) {
  popup_supported_ = popup_supported;
}

bool NWebAccessibilityNodeInfoImpl::GetIsPopupSupported() {
  return popup_supported_;
}

void NWebAccessibilityNodeInfoImpl::SetIsContentInvalid(bool content_invalid) {
  content_invalid_ = content_invalid;
}

bool NWebAccessibilityNodeInfoImpl::GetIsContentInvalid() {
  return content_invalid_;
}

void NWebAccessibilityNodeInfoImpl::SetSelectionEnd(int32_t selection_end) {
  selection_end_ = selection_end;
}

int32_t NWebAccessibilityNodeInfoImpl::GetSelectionEnd() {
  return selection_end_;
}

void NWebAccessibilityNodeInfoImpl::SetSelectionStart(int32_t selection_start) {
  selection_start_ = selection_start;
}

int32_t NWebAccessibilityNodeInfoImpl::GetSelectionStart() {
  return selection_start_;
}

void NWebAccessibilityNodeInfoImpl::SetRangeInfoMin(float range_info_min) {
  range_info_min_ = range_info_min;
}

float NWebAccessibilityNodeInfoImpl::GetRangeInfoMin() {
  return range_info_min_;
}

void NWebAccessibilityNodeInfoImpl::SetRangeInfoMax(float range_info_max) {
  range_info_max_ = range_info_max;
}

float NWebAccessibilityNodeInfoImpl::GetRangeInfoMax() {
  return range_info_max_;
}

void NWebAccessibilityNodeInfoImpl::SetRangeInfoCurrent(
    float range_info_current) {
  range_info_current_ = range_info_current;
}

float NWebAccessibilityNodeInfoImpl::GetRangeInfoCurrent() {
  return range_info_current_;
}

void NWebAccessibilityNodeInfoImpl::SetInputType(int32_t input_type) {
  input_type_ = input_type;
}

int32_t NWebAccessibilityNodeInfoImpl::GetInputType() {
  return input_type_;
}

void NWebAccessibilityNodeInfoImpl::SetComponentType(
    const std::string &component_type) {
  component_type_ = component_type;
}

std::string NWebAccessibilityNodeInfoImpl::GetComponentType() {
  return component_type_;
}

void NWebAccessibilityNodeInfoImpl::SetDescriptionInfo(
    const std::string &description_info) {
  description_info_ = description_info;
}

std::string NWebAccessibilityNodeInfoImpl::GetDescriptionInfo() {
  return description_info_;
}

void NWebAccessibilityNodeInfoImpl::SetGridRows(int32_t grid_rows) {
  grid_rows_ = grid_rows;
}

int32_t NWebAccessibilityNodeInfoImpl::GetGridRows() {
  return grid_rows_;
}

void NWebAccessibilityNodeInfoImpl::SetGridItemRow(int32_t grid_item_row) {
  grid_item_row_ = grid_item_row;
}

int32_t NWebAccessibilityNodeInfoImpl::GetGridItemRow() {
  return grid_item_row_;
}

void NWebAccessibilityNodeInfoImpl::SetGridColumns(int32_t grid_columns) {
  grid_columns_ = grid_columns;
}

int32_t NWebAccessibilityNodeInfoImpl::GetGridColumns() {
  return grid_columns_;
}

void NWebAccessibilityNodeInfoImpl::SetGridItemColumn(
    int32_t grid_item_column) {
  grid_item_column_ = grid_item_column;
}

int32_t NWebAccessibilityNodeInfoImpl::GetGridItemColumn() {
  return grid_item_column_;
}

void NWebAccessibilityNodeInfoImpl::SetGridItemRowSpan(
    int32_t grid_item_row_span) {
  grid_item_row_span_ = grid_item_row_span;
}

int32_t NWebAccessibilityNodeInfoImpl::GetGridItemRowSpan() {
  return grid_item_row_span_;
}

void NWebAccessibilityNodeInfoImpl::SetGridSelectedMode(
    int32_t grid_selected_mode) {
  grid_selected_mode_ = grid_selected_mode;
}

int32_t NWebAccessibilityNodeInfoImpl::GetGridSelectedMode() {
  return grid_selected_mode_;
}

void NWebAccessibilityNodeInfoImpl::SetGridItemColumnSpan(
    int32_t grid_item_column_span) {
  grid_item_column_span_ = grid_item_column_span;
}

int32_t NWebAccessibilityNodeInfoImpl::GetGridItemColumnSpan() {
  return grid_item_column_span_;
}

void NWebAccessibilityNodeInfoImpl::SetIsAccessibilityFocus(
    bool accessibility_focus) {
  accessibility_focus_ = accessibility_focus;
}

bool NWebAccessibilityNodeInfoImpl::GetIsAccessibilityFocus() {
  return accessibility_focus_;
}

void NWebAccessibilityNodeInfoImpl::SetIsPluralLineSupported(
    bool plural_line_supported) {
  plural_line_supported_ = plural_line_supported;
}

bool NWebAccessibilityNodeInfoImpl::GetIsPluralLineSupported() {
  return plural_line_supported_;
}

void NWebAccessibilityNodeInfoImpl::SetIsAccessibilityGroup(
    bool accessibility_group) {
  accessibility_group_ = accessibility_group;
}

bool NWebAccessibilityNodeInfoImpl::GetIsAccessibilityGroup() {
  return accessibility_group_;
}

void NWebAccessibilityNodeInfoImpl::SetAccessibilityLevel(
    const std::string& accessibility_level) {
  accessibility_level_ = accessibility_level;
}

std::string NWebAccessibilityNodeInfoImpl::GetAccessibilityLevel() {
  return accessibility_level_;
}

void NWebAccessibilityNodeInfoImpl::SetAccessibilityDescription(
    const std::string& accessibility_description) {
  accessibility_description_ = accessibility_description;
}

std::string NWebAccessibilityNodeInfoImpl::GetAccessibilityDescription() {
  return accessibility_description_;
}

void NWebAccessibilityNodeInfoImpl::SetAccessibilityText(
    const std::string& accessibility_text) {
  accessibility_text_ = accessibility_text;
}

std::string NWebAccessibilityNodeInfoImpl::GetAccessibilityText() {
  return accessibility_text_;
}

} // namespace OHOS::NWeb
