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

#ifndef NWEB_ACCESSIBILITYNODE_INFO_IMPL_H
#define NWEB_ACCESSIBILITYNODE_INFO_IMPL_H

#include "nweb_accessibility_node_info.h"

namespace OHOS::NWeb {

class NWebAccessibilityNodeInfoImpl : public NWebAccessibilityNodeInfo {
public:
  NWebAccessibilityNodeInfoImpl() = default;
  ~NWebAccessibilityNodeInfoImpl() = default;

  void SetHint(const std::string &hint);

  std::string GetHint() override;

  void SetError(const std::string &error);

  std::string GetError() override;

  void SetRectX(int32_t rect_x);

  int32_t GetRectX() override;

  void SetRectY(int32_t rect_y);

  int32_t GetRectY() override;

  void SetPageId(int32_t page_id) override;

  int32_t GetPageId() override;

  void SetActions(const std::vector<uint32_t> &actions);

  std::vector<uint32_t> GetActions() override;

  void SetContent(const std::string &content);

  std::string GetContent() override;

  void SetChildIds(const std::vector<int64_t> &child_ids);

  std::vector<int64_t> GetChildIds() override;

  void SetParentId(int64_t parent_id) override;

  int64_t GetParentId() override;

  void SetIsHeading(bool heading);

  bool GetIsHeading() override;

  void SetIsChecked(bool checked);

  bool GetIsChecked() override;

  void SetIsEnabled(bool enabled);

  bool GetIsEnabled() override;

  void SetIsFocused(bool focused);

  bool GetIsFocused() override;

  void SetRectWidth(int32_t rect_width);

  int32_t GetRectWidth() override;

  void SetRectHeight(int32_t rect_height);

  int32_t GetRectHeight() override;

  void SetIsVisible(bool visible);

  bool GetIsVisible() override;

  void SetIsHinting(bool hinting);

  bool GetIsHinting() override;

  void SetIsEditable(bool editable);

  bool GetIsEditable() override;

  void SetIsSelected(bool selected);

  bool GetIsSelected() override;

  void SetItemCounts(size_t item_counts);

  size_t GetItemCounts() override;

  void SetLiveRegion(int32_t live_region);

  int32_t GetLiveRegion() override;

  void SetIsPassword(bool password);

  bool GetIsPassword() override;

  void SetIsCheckable(bool checkable);

  bool GetIsCheckable() override;

  void SetIsClickable(bool clickable);

  bool GetIsClickable() override;

  void SetIsFocusable(bool focusable);

  bool GetIsFocusable() override;

  void SetIsScrollable(bool scrollable);

  bool GetIsScrollable() override;

  void SetIsDeletable(bool deletable);

  bool GetIsDeletable() override;

  void SetAccessibilityId(int64_t accessibility_id);

  int64_t GetAccessibilityId() override;

  void SetIsPopupSupported(bool popup_supported);

  bool GetIsPopupSupported() override;

  void SetIsContentInvalid(bool content_invalid);

  bool GetIsContentInvalid() override;

  void SetSelectionEnd(int32_t selection_end);

  int32_t GetSelectionEnd() override;

  void SetSelectionStart(int32_t selection_start);

  int32_t GetSelectionStart() override;

  void SetRangeInfoMin(float range_info_min);

  float GetRangeInfoMin() override;

  void SetRangeInfoMax(float range_info_max);

  float GetRangeInfoMax() override;

  void SetRangeInfoCurrent(float range_info_current);

  float GetRangeInfoCurrent() override;

  void SetInputType(int32_t input_type);

  int32_t GetInputType() override;

  void SetComponentType(const std::string &component_type);

  std::string GetComponentType() override;

  void SetDescriptionInfo(const std::string &description_info);

  std::string GetDescriptionInfo() override;

  void SetGridRows(int32_t grid_rows);

  int32_t GetGridRows() override;

  void SetGridItemRow(int32_t grid_item_row);

  int32_t GetGridItemRow() override;

  void SetGridColumns(int32_t grid_columns);

  int32_t GetGridColumns() override;

  void SetGridItemColumn(int32_t grid_item_column);

  int32_t GetGridItemColumn() override;

  void SetGridItemRowSpan(int32_t grid_item_row_span);

  int32_t GetGridItemRowSpan() override;

  void SetGridSelectedMode(int32_t grid_selected_mode);

  int32_t GetGridSelectedMode() override;

  void SetGridItemColumnSpan(int32_t grid_item_column_span);

  int32_t GetGridItemColumnSpan() override;

  void SetIsAccessibilityFocus(bool accessibility_focus);

  bool GetIsAccessibilityFocus() override;

  void SetIsPluralLineSupported(bool plural_line_supported);

  bool GetIsPluralLineSupported() override;

  void SetIsAccessibilityGroup(bool accessibility_group);

  bool GetIsAccessibilityGroup() override;

  void SetAccessibilityLevel(const std::string& accessibility_level);

  std::string GetAccessibilityLevel() override;

  void SetAccessibilityDescription(
      const std::string& accessibility_description);

  std::string GetAccessibilityDescription() override;

  void SetAccessibilityText(const std::string& accessibility_text);

  std::string GetAccessibilityText() override;

 private:
  int64_t accessibility_id_ = -1;
  size_t item_counts_ = 0;
  int32_t selection_start_ = 0;
  int32_t selection_end_ = 0;
  float range_info_min_ = 0.0f;
  float range_info_max_ = 0.0f;
  float range_info_current_ = 0.0f;
  int32_t input_type_ = -1;
  int32_t live_region_ = -1;
  std::string component_type_;
  std::string hint_;
  std::string content_;
  std::string error_;
  std::string description_info_;
  std::string accessibility_level_;
  std::string accessibility_description_;
  std::string accessibility_text_;
  std::vector<int64_t> child_ids_;
  int64_t parent_id_ = -1;
  int32_t grid_rows_ = -1;
  int32_t grid_columns_ = -1;
  int32_t grid_selected_mode_ = -1;
  int32_t grid_item_row_ = -1;
  int32_t grid_item_row_span_ = -1;
  int32_t grid_item_column_ = -1;
  int32_t grid_item_column_span_ = -1;
  std::vector<uint32_t> actions_;
  int32_t page_id_ = -1;

  int32_t rect_x_ = 0;
  int32_t rect_y_ = 0;
  int32_t rect_width_ = 0;
  int32_t rect_height_ = 0;

  bool heading_ : 1;
  bool checked_ : 1;
  bool editable_ : 1;
  bool enabled_ : 1;
  bool focused_ : 1;
  bool selected_ : 1;
  bool checkable_ : 1;
  bool clickable_ : 1;
  bool focusable_ : 1;
  bool scrollable_ : 1;
  bool password_ : 1;
  bool visible_ : 1;
  bool hinting_ : 1;
  bool plural_line_supported_ : 1;
  bool popup_supported_ : 1;
  bool content_invalid_ : 1;
  bool deletable_ : 1;
  bool accessibility_focus_ : 1;
  bool accessibility_group_ : 1;
};

} // namespace OHOS::NWeb

#endif // NWEB_ACCESSIBILITYNODE_INFO_IMPL_H
