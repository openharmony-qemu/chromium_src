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

#include "ohos_nweb/src/cef_delegate/nweb_select_popup_menu_impl.h"

#include <fuzzer/FuzzedDataProvider.h>

#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "ohos_nweb/include/nweb.h"
#include "ohos_nweb/src/nweb_impl.h"

using namespace OHOS::NWeb;

void NWebSelectPopupMenuItemImplFuzzTest(FuzzedDataProvider* fdp) {
  SelectPopupMenuItemType type = SelectPopupMenuItemType::SP_OPTION;
  std::string label = fdp->ConsumeRandomLengthString(256);
  uint32_t action = fdp->ConsumeIntegralInRange<uint32_t>(0, 256);
  std::string tool_tip = fdp->ConsumeRandomLengthString(256);
  bool is_checked = fdp->ConsumeBool();
  bool is_enabled = fdp->ConsumeBool();
  TextDirection text_direction = TextDirection::kLtr;
  bool has_text_direction_override = fdp->ConsumeBool();
  NWebSelectPopupMenuItemImpl impl;

  impl.SetType(type);

  impl.GetType();

  impl.SetLabel(label);

  impl.GetLabel();

  impl.SetAction(action);

  impl.GetAction();

  impl.SetToolTip(tool_tip);

  impl.GetToolTip();

  impl.SetIsChecked(is_checked);

  impl.GetIsChecked();

  impl.SetIsEnabled(is_enabled);

  impl.GetIsEnabled();

  impl.SetTextDirection(text_direction);

  impl.GetTextDirection();

  impl.SetHasTextDirectionOverride(has_text_direction_override);

  impl.GetHasTextDirectionOverride();
}

void NWebSelectPopupMenuParamImplFuzzTest(FuzzedDataProvider* fdp) {
  std::shared_ptr<NWebSelectPopupMenuItem> menu_item
      = std::make_shared<NWebSelectPopupMenuItem>();
  std::vector<std::shared_ptr<NWebSelectPopupMenuItem>> menu_items;
  menu_items.push_back(menu_item);
  int item_height = fdp->ConsumeIntegralInRange<int>(0, 256);
  int selected_item = fdp->ConsumeIntegralInRange<int>(0, 256);
  double item_font_size = fdp->ConsumeFloatingPoint<double>();
  bool is_right_aligned = fdp->ConsumeBool();

  int x = fdp->ConsumeIntegralInRange<int>(0, 256);
  int y = fdp->ConsumeIntegralInRange<int>(0, 256);
  int width = fdp->ConsumeIntegralInRange<int>(0, 256);
  int height = fdp->ConsumeIntegralInRange<int>(0, 256);
  std::shared_ptr<NWebSelectMenuBound> bounds
      = std::make_shared<NWebSelectMenuBound>(x, y, width, height);

  bool is_allow_multiple_selection = fdp->ConsumeBool();
  NWebSelectPopupMenuParamImpl impl;

  impl.SetMenuItems(menu_items);

  impl.GetMenuItems();

  impl.SetItemHeight(item_height);

  impl.GetItemHeight();

  impl.SetSelectedItem(selected_item);

  impl.GetSelectedItem();

  impl.SetItemFontSize(item_font_size);

  impl.GetItemFontSize();

  impl.SetIsRightAligned(is_right_aligned);

  impl.GetIsRightAligned();

  impl.SetSelectMenuBound(bounds);

  impl.GetSelectMenuBound();

  impl.SetIsAllowMultipleSelection(is_allow_multiple_selection);

  impl.GetIsAllowMultipleSelection();
}

class MySelectPopupCallback : public CefSelectPopupCallback {
public:
    void Continue(const std::vector<int32_t>& indices) override {}

    void Cancel() override {}

    IMPLEMENT_REFCOUNTING(MySelectPopupCallback);
};

void NWebSelectPopupMenuCallbackImplFuzzTest(FuzzedDataProvider* fdp) {
  CefRefPtr<CefSelectPopupCallback> callback = new MySelectPopupCallback();
  std::vector<int32_t> indices;
  indices.push_back(fdp->ConsumeIntegralInRange<int32_t>(0, 256));
  NWebSelectPopupMenuCallbackImpl impl(callback);

  impl.Continue(indices);

  impl.Cancel();
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (data == nullptr || size == 0) {
    return 0;
  }

  FuzzedDataProvider fdp(data, size);

  NWebSelectPopupMenuItemImplFuzzTest(&fdp);

  NWebSelectPopupMenuParamImplFuzzTest(&fdp);

  NWebSelectPopupMenuCallbackImplFuzzTest(&fdp);

  return 0;
}
