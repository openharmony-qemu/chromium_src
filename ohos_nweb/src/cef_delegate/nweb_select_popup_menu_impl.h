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

#ifndef NWEB_SELECT_POPUP_MENU_IMPL_H
#define NWEB_SELECT_POPUP_MENU_IMPL_H

#include "cef/include/cef_dialog_handler.h"
#include "nweb_select_popup_menu.h"

namespace OHOS::NWeb {

class NWebSelectPopupMenuItemImpl : public NWebSelectPopupMenuItem {
public:
  NWebSelectPopupMenuItemImpl() = default;
  ~NWebSelectPopupMenuItemImpl() = default;

  SelectPopupMenuItemType GetType();

  void SetType(SelectPopupMenuItemType type);

  std::string GetLabel();

  void SetLabel(const std::string &label);

  uint32_t GetAction();

  void SetAction(uint32_t action);

  std::string GetToolTip();

  void SetToolTip(const std::string &tool_tip);

  bool GetIsChecked();

  void SetIsChecked(bool is_checked);

  bool GetIsEnabled();

  void SetIsEnabled(bool is_enabled);

  TextDirection GetTextDirection();

  void SetTextDirection(TextDirection text_direction);

  bool GetHasTextDirectionOverride();

  void SetHasTextDirectionOverride(bool has_text_direction_override);

private:
  std::string label_ = "";
  std::string tool_tip_ = "";
  uint32_t action_ = 0;
  bool is_checked_ = false;
  bool is_enabled_ = false;
  bool has_text_direction_override_ = false;
  SelectPopupMenuItemType type_ = SP_OPTION;
  TextDirection text_direction_ = SP_UNKNOWN;
};

class NWebSelectPopupMenuParamImpl : public NWebSelectPopupMenuParam {
public:
  NWebSelectPopupMenuParamImpl() = default;
  ~NWebSelectPopupMenuParamImpl() = default;

  std::vector<std::shared_ptr<NWebSelectPopupMenuItem>> GetMenuItems();

  void SetMenuItems(
      const std::vector<std::shared_ptr<NWebSelectPopupMenuItem>> &menu_items);

  int GetItemHeight();

  void SetItemHeight(int item_height);

  int GetSelectedItem();

  void SetSelectedItem(int selected_item);

  double GetItemFontSize();

  void SetItemFontSize(double item_font_size);

  bool GetIsRightAligned();

  void SetIsRightAligned(bool is_right_aligned);

  std::shared_ptr<NWebSelectMenuBound> GetSelectMenuBound();

  void SetSelectMenuBound(std::shared_ptr<NWebSelectMenuBound> bounds);

  bool GetIsAllowMultipleSelection();

  void SetIsAllowMultipleSelection(bool is_allow_multiple_selection);

private:
  std::shared_ptr<NWebSelectMenuBound> bounds_;
  int item_height_ = -1;
  int selected_item_ = -1;
  double item_font_size_ = -1;
  bool is_right_aligned_ = false;
  bool is_allow_multiple_selection_ = false;
  std::vector<std::shared_ptr<NWebSelectPopupMenuItem>> menu_items_;
};

class NWebSelectPopupMenuCallbackImpl : public NWebSelectPopupMenuCallback {
public:
  NWebSelectPopupMenuCallbackImpl() = default;
  explicit NWebSelectPopupMenuCallbackImpl(
      CefRefPtr<CefSelectPopupCallback> callback);
  ~NWebSelectPopupMenuCallbackImpl() = default;
  void Continue(const std::vector<int32_t> &indices) override;
  void Cancel() override;

private:
  CefRefPtr<CefSelectPopupCallback> callback_ = nullptr;
};
} // namespace OHOS::NWeb
#endif
