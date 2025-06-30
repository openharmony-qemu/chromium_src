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

#include "nweb_accessibility_node_info_impl.h"

#include <fuzzer/FuzzedDataProvider.h>

using namespace OHOS::NWeb;
namespace {
    constexpr uint8_t MAX_STRING_LENGTH = 255;
    constexpr int32_t MAX_INT_SIZE = 255;
    constexpr int64_t MAX_INT64_SIZE = 511;
    constexpr int MAX_VECTOR_SIZE = 10;
    constexpr float MAX_FLOAT_SIZE = 10.0f;
}

namespace OHOS {
void NwebAccessibilityNodeInfoImpl001FuzzTest(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    std::shared_ptr<NWebAccessibilityNodeInfoImpl> nodeInfo_ = std::make_shared<NWebAccessibilityNodeInfoImpl>();
    std::string hint = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    std::string error = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    int32_t rect_x = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT_SIZE);
    int32_t rect_y = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT_SIZE);
    int32_t page_id = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT_SIZE);
    int64_t parent_id = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
    bool heading = dataProvider.ConsumeBool();
    bool checked = dataProvider.ConsumeBool();
    bool enabled = dataProvider.ConsumeBool();
    bool focused = dataProvider.ConsumeBool();
    bool visible = dataProvider.ConsumeBool();
    bool hinting = dataProvider.ConsumeBool();
    bool editable = dataProvider.ConsumeBool();
    bool selected = dataProvider.ConsumeBool();
    bool password = dataProvider.ConsumeBool();
    bool checkable = dataProvider.ConsumeBool();
    bool clickable = dataProvider.ConsumeBool();
    bool focusable = dataProvider.ConsumeBool();
    bool scrollable = dataProvider.ConsumeBool();
    bool deletable = dataProvider.ConsumeBool();
    bool popup_supported = dataProvider.ConsumeBool();
    bool content_invalid = dataProvider.ConsumeBool();
    bool accessibility_focus = dataProvider.ConsumeBool();
    bool plural_line_supported = dataProvider.ConsumeBool();
    bool accessibility_group = dataProvider.ConsumeBool();

    nodeInfo_->SetHint(hint);
    nodeInfo_->SetError(error);
    nodeInfo_->SetRectX(rect_x);
    nodeInfo_->SetRectY(rect_y);
    nodeInfo_->SetPageId(page_id);
    nodeInfo_->SetParentId(parent_id);
    nodeInfo_->SetIsHeading(heading);
    nodeInfo_->SetIsChecked(checked);
    nodeInfo_->SetIsEnabled(enabled);
    nodeInfo_->SetIsFocused(focused);
    nodeInfo_->SetIsVisible(visible);
    nodeInfo_->SetIsHinting(hinting);
    nodeInfo_->SetIsEditable(editable);
    nodeInfo_->SetIsSelected(selected);
    nodeInfo_->SetIsPassword(password);
    nodeInfo_->SetIsCheckable(checkable);
    nodeInfo_->SetIsClickable(clickable);
    nodeInfo_->SetIsFocusable(focusable);
    nodeInfo_->SetIsScrollable(scrollable);
    nodeInfo_->SetIsDeletable(deletable);
    nodeInfo_->SetIsPopupSupported(popup_supported);
    nodeInfo_->SetIsContentInvalid(content_invalid);
    nodeInfo_->SetIsAccessibilityFocus(accessibility_focus);
    nodeInfo_->SetIsPluralLineSupported(plural_line_supported);
    nodeInfo_->SetIsAccessibilityGroup(accessibility_group);
    
    std::vector<uint32_t> actions;
    int vec_size = dataProvider.ConsumeIntegralInRange<int>(0, MAX_VECTOR_SIZE);
    for (int i = 0; i< vec_size; i++) {
        uint32_t item = dataProvider.ConsumeIntegralInRange<uint32_t>(0, MAX_INT_SIZE);
        actions.push_back(item);
    }
    nodeInfo_->SetActions(actions);
}

void NwebAccessibilityNodeInfoImpl002FuzzTest(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    std::shared_ptr<NWebAccessibilityNodeInfoImpl> nodeInfo_ = std::make_shared<NWebAccessibilityNodeInfoImpl>();
    std::string content = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    nodeInfo_->SetContent(content);
    std::vector<int64_t> child_ids;
    int vec_size = dataProvider.ConsumeIntegralInRange<int>(0, MAX_VECTOR_SIZE);
    for (int i = 0; i< vec_size; i++) {
        int64_t item = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
        child_ids.push_back(item);
    }
    nodeInfo_->SetChildIds(child_ids);

    int32_t rect_width = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT_SIZE);
    nodeInfo_->SetRectWidth(rect_width);

    int32_t rect_height = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT_SIZE);
    nodeInfo_->SetRectHeight(rect_height);

    size_t item_counts = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT_SIZE);
    nodeInfo_->SetItemCounts(item_counts);

    int32_t live_region = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT_SIZE);
    nodeInfo_->SetLiveRegion(live_region);

    int32_t selection_end = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT_SIZE);
    nodeInfo_->SetSelectionEnd(selection_end);

    int32_t selection_start = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT_SIZE);
    nodeInfo_->SetSelectionStart(selection_start);

    float range_info_min = dataProvider.ConsumeFloatingPointInRange<float>(0, MAX_FLOAT_SIZE);
    nodeInfo_->SetRangeInfoMin(range_info_min);

    float range_info_max = dataProvider.ConsumeFloatingPointInRange<float>(0, MAX_FLOAT_SIZE);
    nodeInfo_->SetRangeInfoMax(range_info_max);

    float range_info_current = dataProvider.ConsumeFloatingPointInRange<float>(0, MAX_FLOAT_SIZE);
    nodeInfo_->SetRangeInfoCurrent(range_info_current);

    int64_t accessibility_id = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_INT64_SIZE);
    nodeInfo_->SetAccessibilityId(accessibility_id);
}


void NwebAccessibilityNodeInfoImpl003FuzzTest(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    std::shared_ptr<NWebAccessibilityNodeInfoImpl> nodeInfo_ = std::make_shared<NWebAccessibilityNodeInfoImpl>();
    int32_t input_type = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT_SIZE);
    nodeInfo_->SetInputType(input_type);

    std::string component_type = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    nodeInfo_->SetComponentType(component_type);

    std::string description_info = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    nodeInfo_->SetDescriptionInfo(description_info);

    int32_t grid_rows = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT_SIZE);
    nodeInfo_->SetGridRows(grid_rows);

    int32_t grid_item_row = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT_SIZE);
    nodeInfo_->SetGridItemRow(grid_item_row);

    int32_t grid_columns = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT_SIZE);
    nodeInfo_->SetGridColumns(grid_columns);

    int32_t grid_item_column = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT_SIZE);
    nodeInfo_->SetGridItemColumn(grid_item_column);

    int32_t grid_item_row_span = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT_SIZE);
    nodeInfo_->SetGridItemRowSpan(grid_item_row_span);

    int32_t grid_selected_mode = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT_SIZE);
    nodeInfo_->SetGridSelectedMode(grid_selected_mode);

    int32_t grid_item_column_span = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT_SIZE);
    nodeInfo_->SetGridItemColumnSpan(grid_item_column_span);

    std::string accessibility_level = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    nodeInfo_->SetAccessibilityLevel(accessibility_level);

    std::string accessibility_description = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    nodeInfo_->SetAccessibilityDescription(accessibility_description);

    std::string accessibility_text = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    nodeInfo_->SetAccessibilityText(accessibility_text);

    std::string htmlElementId = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    nodeInfo_->SetHtmlElementId(htmlElementId);

    std::string component_type_description = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    nodeInfo_->SetComponentTypeDescription(component_type_description);

    int32_t checkbox_group_selected_status = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_INT_SIZE);
    nodeInfo_->SetCheckboxGroupSelectedStatus(checkbox_group_selected_status);

    std::string expanded_state = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    nodeInfo_->SetExpandedState(expanded_state);
}

void NwebAccessibilityNodeInfoImpl004FuzzTest(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    std::shared_ptr<NWebAccessibilityNodeInfoImpl> nodeInfo_ = std::make_shared<NWebAccessibilityNodeInfoImpl>();
    nodeInfo_->GetHint();
    nodeInfo_->GetError();
    nodeInfo_->GetRectX();
    nodeInfo_->GetRectY();
    nodeInfo_->GetPageId();
    nodeInfo_->GetActions();
    nodeInfo_->GetContent();
    nodeInfo_->GetChildIds();
    nodeInfo_->GetParentId();
    nodeInfo_->GetIsHeading();
    nodeInfo_->GetIsChecked();
    nodeInfo_->GetIsEnabled();
    nodeInfo_->GetIsFocused();
    nodeInfo_->GetRectWidth();
    nodeInfo_->GetRectHeight();
    nodeInfo_->GetIsVisible();
    nodeInfo_->GetIsHinting();
    nodeInfo_->GetIsEditable();
    nodeInfo_->GetIsSelected();
    nodeInfo_->GetItemCounts();
    nodeInfo_->GetLiveRegion();
    nodeInfo_->GetIsPassword();
    nodeInfo_->GetIsCheckable();
    nodeInfo_->GetIsClickable();
    nodeInfo_->GetIsFocusable();
    nodeInfo_->GetIsScrollable();
    nodeInfo_->GetIsDeletable();
    nodeInfo_->GetAccessibilityId();
    nodeInfo_->GetIsPopupSupported();
    nodeInfo_->GetIsContentInvalid();
    nodeInfo_->GetSelectionEnd();
    nodeInfo_->GetSelectionStart();
    nodeInfo_->GetRangeInfoMin();
    nodeInfo_->GetRangeInfoMax();
    nodeInfo_->GetRangeInfoCurrent();
    nodeInfo_->GetInputType();
    nodeInfo_->GetComponentType();
    nodeInfo_->GetDescriptionInfo();
    nodeInfo_->GetGridRows();
    nodeInfo_->GetGridItemRow();
    nodeInfo_->GetGridColumns();
    nodeInfo_->GetGridItemColumn();
    nodeInfo_->GetGridItemRowSpan();
    nodeInfo_->GetGridSelectedMode();
    nodeInfo_->GetGridItemColumnSpan();
    nodeInfo_->GetIsAccessibilityFocus();
    nodeInfo_->GetIsPluralLineSupported();
    nodeInfo_->GetIsAccessibilityGroup();
    nodeInfo_->GetAccessibilityLevel();
    nodeInfo_->GetAccessibilityDescription();
    nodeInfo_->GetAccessibilityText();
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    OHOS::NwebAccessibilityNodeInfoImpl001FuzzTest(data, size);
    OHOS::NwebAccessibilityNodeInfoImpl002FuzzTest(data, size);
    OHOS::NwebAccessibilityNodeInfoImpl003FuzzTest(data, size);
    OHOS::NwebAccessibilityNodeInfoImpl004FuzzTest(data, size);
    return 0;
}
}  // namespace OHOS