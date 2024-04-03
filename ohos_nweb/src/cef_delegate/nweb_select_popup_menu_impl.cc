/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "nweb_select_popup_menu_impl.h"

namespace OHOS::NWeb {

SelectPopupMenuItemType NWebSelectPopupMenuItemImpl::GetType() {
  return type_;
}

void NWebSelectPopupMenuItemImpl::SetType(SelectPopupMenuItemType type) {
  type_ = type;
}

std::string NWebSelectPopupMenuItemImpl::GetLabel() {
  return label_;
}

void NWebSelectPopupMenuItemImpl::SetLabel(const std::string &label) {
  label_ = label;
}

uint32_t NWebSelectPopupMenuItemImpl::GetAction() {
  return action_;
}

void NWebSelectPopupMenuItemImpl::SetAction(uint32_t action) {
  action_ = action;
}

std::string NWebSelectPopupMenuItemImpl::GetToolTip() {
  return tool_tip_;
}

void NWebSelectPopupMenuItemImpl::SetToolTip(const std::string &tool_tip) {
  tool_tip_ = tool_tip;
}

bool NWebSelectPopupMenuItemImpl::GetIsChecked() {
  return is_checked_;
}

void NWebSelectPopupMenuItemImpl::SetIsChecked(bool is_checked) {
  is_checked_ = is_checked;
}

bool NWebSelectPopupMenuItemImpl::GetIsEnabled() {
  return is_enabled_;
}

void NWebSelectPopupMenuItemImpl::SetIsEnabled(bool is_enabled) {
  is_enabled_ = is_enabled;
}

TextDirection NWebSelectPopupMenuItemImpl::GetTextDirection() {
  return text_direction_;
}

void NWebSelectPopupMenuItemImpl::SetTextDirection(
    TextDirection text_direction) {
  text_direction_ = text_direction;
}

bool NWebSelectPopupMenuItemImpl::GetHasTextDirectionOverride() {
  return has_text_direction_override_;
}

void NWebSelectPopupMenuItemImpl::SetHasTextDirectionOverride(
    bool has_text_direction_override) {
  has_text_direction_override_ = has_text_direction_override;
}

std::vector<std::shared_ptr<NWebSelectPopupMenuItem>>
NWebSelectPopupMenuParamImpl::GetMenuItems() {
  return menu_items_;
}

void NWebSelectPopupMenuParamImpl::SetMenuItems(
    const std::vector<std::shared_ptr<NWebSelectPopupMenuItem>> &menu_items) {
  menu_items_ = menu_items;
}

int NWebSelectPopupMenuParamImpl::GetItemHeight() {
  return item_height_;
}

void NWebSelectPopupMenuParamImpl::SetItemHeight(int item_height) {
  item_height_ = item_height;
}

int NWebSelectPopupMenuParamImpl::GetSelectedItem() {
  return selected_item_;
}

void NWebSelectPopupMenuParamImpl::SetSelectedItem(int selected_item) {
  selected_item_ = selected_item;
}

double NWebSelectPopupMenuParamImpl::GetItemFontSize() {
  return item_font_size_;
}

void NWebSelectPopupMenuParamImpl::SetItemFontSize(double item_font_size) {
  item_font_size_ = item_font_size;
}

bool NWebSelectPopupMenuParamImpl::GetIsRightAligned() {
  return is_right_aligned_;
}

void NWebSelectPopupMenuParamImpl::SetIsRightAligned(bool is_right_aligned) {
  is_right_aligned_ = is_right_aligned;
}

std::shared_ptr<NWebSelectMenuBound>
NWebSelectPopupMenuParamImpl::GetSelectMenuBound() {
  return bounds_;
}

void NWebSelectPopupMenuParamImpl::SetSelectMenuBound(
    std::shared_ptr<NWebSelectMenuBound> bounds) {
  bounds_ = bounds;
}

bool NWebSelectPopupMenuParamImpl::GetIsAllowMultipleSelection() {
  return is_allow_multiple_selection_;
}

void NWebSelectPopupMenuParamImpl::SetIsAllowMultipleSelection(
    bool is_allow_multiple_selection) {
  is_allow_multiple_selection_ = is_allow_multiple_selection;
}

NWebSelectPopupMenuCallbackImpl::NWebSelectPopupMenuCallbackImpl(
    CefRefPtr<CefSelectPopupCallback> callback)
    : callback_(callback) {
}

void NWebSelectPopupMenuCallbackImpl::Continue(
    const std::vector<int32_t> &indices) {
  if (callback_) {
    callback_->Continue(indices);
  }
}

void NWebSelectPopupMenuCallbackImpl::Cancel() {
  if (callback_) {
    callback_->Cancel();
  }
}
} // namespace OHOS::NWeb
