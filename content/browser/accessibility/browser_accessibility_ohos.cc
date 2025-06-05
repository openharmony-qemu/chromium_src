/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "content/browser/accessibility/browser_accessibility_ohos.h"

#include <codecvt>
#include <locale>

#include "content/browser/accessibility/browser_accessibility_manager_ohos.h"
#include "content/public/common/content_client.h"
#include "skia/ext/skia_utils_base.h"
#include "third_party/blink/public/strings/grit/blink_strings.h"
#include "ui/accessibility/ax_assistant_structure.h"
#include "ui/accessibility/ax_selection.h"
#include "ui/strings/grit/ax_strings.h"

namespace ui {
std::unique_ptr<BrowserAccessibility> BrowserAccessibility::Create(
    BrowserAccessibilityManager* manager,
    ui::AXNode* node) {
  return std::unique_ptr<content::BrowserAccessibilityOHOS>(
      new content::BrowserAccessibilityOHOS(manager, node));
}
}  // namespace ui

namespace content {

using AccessibilityIdMap =
    std::unordered_map<int64_t, BrowserAccessibilityOHOS*>;
base::LazyInstance<AccessibilityIdMap>::Leaky g_accessibility_id_map =
    LAZY_INSTANCE_INITIALIZER;

BrowserAccessibilityOHOS::BrowserAccessibilityOHOS(
    ui::BrowserAccessibilityManager* manager,
    ui::AXNode* node)
    : BrowserAccessibility(manager, node) {
  accessibility_id_ =
      BrowserAccessibilityManagerOHOS::GenerateAccessibilityId();
  g_accessibility_id_map.Get()[accessibility_id_] = this;
  platform_node_ =
      static_cast<ui::AXPlatformNodeOHOS*>(ui::AXPlatformNode::Create(this));
}

BrowserAccessibilityOHOS::~BrowserAccessibilityOHOS() {
  g_accessibility_id_map.Get().erase(accessibility_id_);
  platform_node_->Destroy();
}

int64_t BrowserAccessibilityOHOS::GetAccessibilityId() const {
  return accessibility_id_;
}

bool BrowserAccessibilityOHOS::IsEnabled() const {
  switch (GetData().GetRestriction()) {
    case ax::mojom::Restriction::kNone:
      return true;
    case ax::mojom::Restriction::kReadOnly:
    case ax::mojom::Restriction::kDisabled:
      return false;
    default:
      return true;
  }
}

std::string BrowserAccessibilityOHOS::GetHint() const {
  std::vector<std::string> strings;

  if (ShouldExposeValueAsName()) {
    std::string name = GetName();
    if (!name.empty()) {
      strings.emplace_back(name);
    }
  }

  if (GetData().GetNameFrom() != ax::mojom::NameFrom::kPlaceholder) {
    std::string placeholder =
        GetStringAttribute(ax::mojom::StringAttribute::kPlaceholder);
    if (!placeholder.empty()) {
      strings.emplace_back(placeholder);
    }
  }

  std::string description =
      GetStringAttribute(ax::mojom::StringAttribute::kDescription);
  if (!description.empty()) {
    strings.emplace_back(description);
  }

  return base::JoinString(strings, " ");
}

bool BrowserAccessibilityOHOS::IsHint() const {
  std::vector<std::string> strings;

  if (ShouldExposeValueAsName()) {
    std::string name = GetName();
    if (!name.empty()) {
      strings.emplace_back(name);
    }
  }

  if (GetData().GetNameFrom() != ax::mojom::NameFrom::kPlaceholder) {
    std::string placeholder =
        GetStringAttribute(ax::mojom::StringAttribute::kPlaceholder);
    if (!placeholder.empty()) {
      strings.emplace_back(placeholder);
    }
  }

  std::string description =
      GetStringAttribute(ax::mojom::StringAttribute::kDescription);
  if (!description.empty()) {
    strings.emplace_back(description);
  }
  return !strings.empty();
}

bool BrowserAccessibilityOHOS::IsChecked() const {
  return GetData().GetCheckedState() == ax::mojom::CheckedState::kTrue;
}

bool BrowserAccessibilityOHOS::IsSelected() const {
  return GetBoolAttribute(ax::mojom::BoolAttribute::kSelected);
}

bool BrowserAccessibilityOHOS::IsScrollable() const {
  return GetBoolAttribute(ax::mojom::BoolAttribute::kScrollable);
}

bool BrowserAccessibilityOHOS::ShouldExposeValueAsName() const {
  switch (GetRole()) {
    case ax::mojom::Role::kDate:
    case ax::mojom::Role::kDateTime:
    case ax::mojom::Role::kInputTime:
      return true;
    case ax::mojom::Role::kColorWell:
      return false;
    default:
      break;
  }

  if (GetData().IsRangeValueSupported()) {
    return false;
  }

  if (IsTextField()) {
    return true;
  }

  if (GetRole() == ax::mojom::Role::kPopUpButton &&
      !GetValueForControl().empty()) {
    return true;
  }

  return false;
}

bool BrowserAccessibilityOHOS::IsCheckable() const {
  return GetData().HasCheckedState();
}

bool BrowserAccessibilityOHOS::IsMultiLine() const {
  return HasState(ax::mojom::State::kMultiline);
}

bool BrowserAccessibilityOHOS::CanOpenPopup() const {
  return HasIntAttribute(ax::mojom::IntAttribute::kHasPopup);
}

int CheckMarkerTypes(const std::vector<int32_t>& marker_types) {
  for (const auto& marker_type : marker_types) {
    if (marker_type & static_cast<int32_t>(ax::mojom::MarkerType::kSpelling)) {
      return CONTENT_INVALID_SPELLING;
    }
    if (marker_type & static_cast<int32_t>(ax::mojom::MarkerType::kGrammar)) {
      return CONTENT_INVALID_GRAMMAR;
    }
  }
  return CONTENT_INVALID_TRUE;
}

int BrowserAccessibilityOHOS::GetContentTrueMessage() const {
  int message_id = CONTENT_INVALID_TRUE;
  for (auto it = InternalChildrenBegin(); it != InternalChildrenEnd(); ++it) {
    BrowserAccessibility* child = it.get();
    if (child && child->IsText()) {
      const std::vector<int32_t>& marker_types =
          child->GetIntListAttribute(ax::mojom::IntListAttribute::kMarkerTypes);
      int current_message = CheckMarkerTypes(marker_types);
      if (current_message != CONTENT_INVALID_TRUE) {
        message_id = current_message;
      }
    }
  }
  return message_id;
}

std::string BrowserAccessibilityOHOS::GetContentInvalidErrorMessage() const {
  ContentClient* content_client = GetContentClient();
  if (!content_client)
    return std::string();
  int message_id = -1;

  if (!IsContentInvalid())
    return std::string();

  switch (GetData().GetInvalidState()) {
    case ax::mojom::InvalidState::kNone:
    case ax::mojom::InvalidState::kFalse:
      break;

    case ax::mojom::InvalidState::kTrue:
      message_id = GetContentTrueMessage();
      break;
    default:
      break;
  }

  if (message_id != -1)
    return base::UTF16ToUTF8(content_client->GetLocalizedString(message_id));
  return std::string();
}

bool BrowserAccessibilityOHOS::IsContentInvalid() const {
  if (HasIntAttribute(ax::mojom::IntAttribute::kInvalidState)) {
    return GetData().GetInvalidState() != ax::mojom::InvalidState::kFalse;
  }
  return false;
}

AceTextCategory BrowserAccessibilityOHOS::GetInputType(
    const std::string& name) const {
  if (name.empty()) {
    return AceTextCategory::INPUT_TYPE_TEXT;
  }
  for (const auto& ace_map : kAceTextCategoryMap) {
    if (name.compare(ace_map.ace_text) == 0) {
      return ace_map.ace_type;
    }
  }
  return AceTextCategory::INPUT_TYPE_DEFAULT;
}

int32_t BrowserAccessibilityOHOS::OHOSInputType() const {
  std::string html_tag =
      GetStringAttribute(ax::mojom::StringAttribute::kHtmlTag);
  if (html_tag != "input") {
    return static_cast<int32_t>(AceTextCategory::INPUT_TYPE_DEFAULT);
  }

  std::string type =
      node()->GetStringAttribute(ax::mojom::StringAttribute::kInputType);
  if (type == base::EmptyString()) {
    return static_cast<int32_t>(AceTextCategory::INPUT_TYPE_DEFAULT);
  }

  return static_cast<int32_t>(GetInputType(type));
}

int32_t BrowserAccessibilityOHOS::OHOSLiveRegionType() const {
  std::string live =
      GetStringAttribute(ax::mojom::StringAttribute::kLiveStatus);
  if (live == "polite") {
    return 1;
  } else if (live == "assertive") {
    return 1;
  }
  return 0;
}

int32_t BrowserAccessibilityOHOS::GetSelectionStart() const {
  int sel_start = 0;
  if (IsAtomicTextField() &&
      GetIntAttribute(ax::mojom::IntAttribute::kTextSelStart, &sel_start)) {
    return sel_start;
  }
  ui::AXSelection unignored_selection =
      manager()->ax_tree()->GetUnignoredSelection();
  int32_t anchor_id = unignored_selection.anchor_object_id;
  BrowserAccessibility* anchor_object = manager()->GetFromID(anchor_id);
  if (!anchor_object) {
    return 0;
  }

  AXPosition position = anchor_object->CreateTextPositionAt(
      unignored_selection.anchor_offset, unignored_selection.anchor_affinity);
  while (position->GetAnchor() && position->GetAnchor() != node()) {
    position = position->CreateParentPosition();
  }

  return !position->IsNullPosition() ? position->text_offset() : 0;
}

int32_t BrowserAccessibilityOHOS::GetSelectionEnd() const {
  int sel_end = 0;
  if (IsAtomicTextField() &&
      GetIntAttribute(ax::mojom::IntAttribute::kTextSelEnd, &sel_end)) {
    return sel_end;
  }
  return sel_end;
}

size_t BrowserAccessibilityOHOS::GetItemCount() const {
  int item_count = 0;
  if (IsRangeControlWithoutAriaValueText()) {
    item_count = 100;  // max aria-value
  } else if (IsCollection() && node()->GetSetSize()) {
    item_count = *node()->GetSetSize();
  }
  return item_count;
}

bool BrowserAccessibilityOHOS::IsRangeControlWithoutAriaValueText() const {
  return GetData().IsRangeValueSupported() &&
         !HasStringAttribute(ax::mojom::StringAttribute::kValue) &&
         HasFloatAttribute(ax::mojom::FloatAttribute::kValueForRange);
}

bool BrowserAccessibilityOHOS::IsCollection() const {
  return (ui::IsTableLike(GetRole()) || GetRole() == ax::mojom::Role::kList ||
          GetRole() == ax::mojom::Role::kListBox ||
          GetRole() == ax::mojom::Role::kDescriptionList ||
          GetRole() == ax::mojom::Role::kDirectoryDeprecated ||
          GetRole() == ax::mojom::Role::kTree);
}

int32_t BrowserAccessibilityOHOS::RowCount() const {
  if (!IsCollection()) {
    return 0;
  }

  if (node()->GetSetSize()) {
    return *node()->GetSetSize();
  }

  return node()->GetTableRowCount().value_or(0);
}

int32_t BrowserAccessibilityOHOS::ColumnCount() const {
  if (IsCollection()) {
    return node()->GetTableColCount().value_or(0);
  }
  return 0;
}

int32_t BrowserAccessibilityOHOS::RowIndex() const {
  std::optional<int> pos_in_set = node()->GetPosInSet();
  if (pos_in_set && pos_in_set > 0) {
    return *pos_in_set - 1;
  }
  return node()->GetTableCellRowIndex().value_or(0);
}

int32_t BrowserAccessibilityOHOS::RowSpan() const {
  return node()->GetTableCellRowSpan().value_or(0);
}

int32_t BrowserAccessibilityOHOS::ColumnIndex() const {
  return node()->GetTableCellColIndex().value_or(0);
}

int32_t BrowserAccessibilityOHOS::ColumnSpan() const {
  return node()->GetTableCellColSpan().value_or(0);
}

bool BrowserAccessibilityOHOS::IsCollectionItem() const {
  return (GetRole() == ax::mojom::Role::kCell ||
          GetRole() == ax::mojom::Role::kColumnHeader ||
          GetRole() == ax::mojom::Role::kDescriptionListTermDeprecated ||
          GetRole() == ax::mojom::Role::kListBoxOption ||
          GetRole() == ax::mojom::Role::kListItem ||
          GetRole() == ax::mojom::Role::kRowHeader ||
          GetRole() == ax::mojom::Role::kTreeItem);
}

bool BrowserAccessibilityOHOS::IsHeading() const {
  BrowserAccessibilityOHOS* parent =
      static_cast<BrowserAccessibilityOHOS*>(PlatformGetParent());
  if (parent && parent->IsHeading()) {
    return true;
  }

  return ui::IsHeading(GetRole());
}

bool BrowserAccessibilityOHOS::IsLink() const {
  return ui::IsLink(GetRole());
}

bool BrowserAccessibilityOHOS::IsHierarchical() const {
  return (GetRole() == ax::mojom::Role::kTree || IsHierarchicalList());
}

BrowserAccessibilityOHOS* BrowserAccessibilityOHOS::GetFromAccessibilityId(
    int64_t accessibility_id) {
  AccessibilityIdMap* accessibility_ids = g_accessibility_id_map.Pointer();
  auto iter = accessibility_ids->find(accessibility_id);
  if (iter != accessibility_ids->end()) {
    return iter->second;
  }

  return nullptr;
}

bool BrowserAccessibilityOHOS::HasOnlyTextChildren() const {
  for (auto it = InternalChildrenBegin(); it != InternalChildrenEnd(); ++it) {
    if (!it->IsText()) {
      return false;
    }
  }
  return true;
}

const BrowserAccessibilityOHOS*
BrowserAccessibilityOHOS::GetAccessibilityNodeByFocusMove(
    int32_t direction) const {
  std::list<const BrowserAccessibilityOHOS*> node_list;
  const BrowserAccessibilityOHOS* result_node = nullptr;

  if (!manager_) {
    return result_node;
  }
  auto root = static_cast<BrowserAccessibilityOHOS*>(
      manager_->GetBrowserAccessibilityRoot());
  if (!root) {
    return result_node;
  }
  root->AddFocusableNode(node_list);

  switch (direction) {
    case FocusMoveDirection::FORWARD:
    case FocusMoveDirection::BACKWARD:
      result_node = FindNodeInRelativeDirection(node_list, direction);
      break;
    case FocusMoveDirection::UP:
    case FocusMoveDirection::DOWN:
    case FocusMoveDirection::LEFT:
    case FocusMoveDirection::RIGHT:
      result_node = FindNodeInAbsoluteDirection(node_list, direction);
      break;
    default:
      break;
  }
  return result_node;
}

void BrowserAccessibilityOHOS::AddFocusableNode(
    std::list<const BrowserAccessibilityOHOS*>& node_list) const {
  for (const auto& childNode : PlatformChildren()) {
    const BrowserAccessibilityOHOS& childNodeOHOS =
        static_cast<const BrowserAccessibilityOHOS&>(childNode);
    node_list.emplace_back(&childNodeOHOS);
    childNodeOHOS.AddFocusableNode(node_list);
  }
}

const BrowserAccessibilityOHOS*
BrowserAccessibilityOHOS::FindNodeInRelativeDirection(
    const std::list<const BrowserAccessibilityOHOS*>& node_list,
    int32_t direction) const {
  switch (direction) {
    case FocusMoveDirection::FORWARD:
      return GetNextFocusableNode(node_list);
    case FocusMoveDirection::BACKWARD:
      return GetPreviousFocusableNode(node_list);
    default:
      break;
  }

  return nullptr;
}

const BrowserAccessibilityOHOS*
BrowserAccessibilityOHOS::FindNodeInAbsoluteDirection(
    const std::list<const BrowserAccessibilityOHOS*>& node_list,
    int32_t direction) const {
  ui::AXOffscreenResult offscreen_result = ui::AXOffscreenResult::kOnscreen;
  float dip_scale = manager_->device_scale_factor();
  gfx::Rect rect = gfx::ScaleToEnclosingRect(
      GetUnclippedRootFrameBoundsRect(&offscreen_result), dip_scale, dip_scale);
  gfx::Rect tempBest = rect;
  auto node_rect = tempBest;
  auto left = rect.x();
  auto top = rect.y();
  auto width = rect.width();
  auto height = rect.height();
  switch (direction) {
    case FocusMoveDirection::LEFT:
      tempBest.set_x(left + width + 1);
      break;
    case FocusMoveDirection::RIGHT:
      tempBest.set_x(left - width - 1);
      break;
    case FocusMoveDirection::UP:
      tempBest.set_y(top + height + 1);
      break;
    case FocusMoveDirection::DOWN:
      tempBest.set_y(top - height - 1);
      break;
    default:
      break;
  }

  const BrowserAccessibilityOHOS* nearestNode = nullptr;
  for (const auto& nodeItem : node_list) {
    if (nodeItem->GetAccessibilityId() == accessibility_id_ ||
        !nodeItem->PlatformGetParent()) {
      continue;
    }
    rect = gfx::ScaleToEnclosingRect(
        nodeItem->GetUnclippedRootFrameBoundsRect(&offscreen_result), dip_scale,
        dip_scale);
    auto item_rect = rect;
    if (CheckBetterRect(node_rect, direction, item_rect, tempBest)) {
      tempBest = item_rect;
      nearestNode = nodeItem;
    }
  }
  return nearestNode;
}

const BrowserAccessibilityOHOS* BrowserAccessibilityOHOS::GetNextFocusableNode(
    const std::list<const BrowserAccessibilityOHOS*>& node_list) const {
  auto nodeItem = node_list.begin();
  for (; nodeItem != node_list.end(); nodeItem++) {
    if ((*nodeItem)->GetAccessibilityId() == accessibility_id_) {
      break;
    }
  }

  if (nodeItem != node_list.end()) {
    if (++nodeItem != node_list.end()) {
      return (*nodeItem);
    }
  }
  if (!node_list.empty()) {
    return (*node_list.begin());
  }

  return nullptr;
}

const BrowserAccessibilityOHOS*
BrowserAccessibilityOHOS::GetPreviousFocusableNode(
    const std::list<const BrowserAccessibilityOHOS*>& node_list) const {
  auto nodeItem = node_list.rbegin();
  for (; nodeItem != node_list.rend(); nodeItem++) {
    if ((*nodeItem)->GetAccessibilityId() == accessibility_id_) {
      break;
    }
  }

  if (nodeItem != node_list.rend()) {
    if (++nodeItem != node_list.rend()) {
      return (*nodeItem);
    }
  }
  if (!node_list.empty()) {
    return (*node_list.rbegin());
  }

  return nullptr;
}

bool BrowserAccessibilityOHOS::CheckRectBeam(const gfx::Rect& node_rect,
                                             const gfx::Rect& item_rect,
                                             const int32_t direction) {
  switch (direction) {
    case FocusMoveDirection::LEFT:
    case FocusMoveDirection::RIGHT:
      return node_rect.y() < item_rect.bottom() &&
             item_rect.y() < node_rect.bottom();
    case FocusMoveDirection::UP:
    case FocusMoveDirection::DOWN:
      return node_rect.x() < item_rect.right() &&
             item_rect.x() < node_rect.right();
    default:
      break;
  }
  return false;
}

bool BrowserAccessibilityOHOS::IsToDirectionOf(const gfx::Rect& node_rect,
                                               const gfx::Rect& item_rect,
                                               const int32_t direction) {
  switch (direction) {
    case FocusMoveDirection::LEFT:
      return node_rect.x() >= item_rect.right();
    case FocusMoveDirection::RIGHT:
      return node_rect.right() <= item_rect.x();
    case FocusMoveDirection::UP:
      return node_rect.y() >= item_rect.bottom();
    case FocusMoveDirection::DOWN:
      return node_rect.bottom() <= item_rect.y();
    default:
      break;
  }
  return false;
}

double BrowserAccessibilityOHOS::MajorAxisDistanceToFarEdge(
    const gfx::Rect& node_rect,
    const gfx::Rect& item_rect,
    const int32_t direction) {
  double distance = 0.0;
  switch (direction) {
    case FocusMoveDirection::LEFT:
      distance = node_rect.x() - item_rect.x();
      break;
    case FocusMoveDirection::RIGHT:
      distance = item_rect.right() - node_rect.right();
      break;
    case FocusMoveDirection::UP:
      distance = node_rect.y() - item_rect.y();
      break;
    case FocusMoveDirection::DOWN:
      distance = item_rect.bottom() - node_rect.bottom();
      break;
    default:
      break;
  }

  return distance > 1.0 ? distance : 1.0;
}

double BrowserAccessibilityOHOS::MajorAxisDistance(const gfx::Rect& node_rect,
                                                   const gfx::Rect& item_rect,
                                                   const int32_t direction) {
  double distance = 0.0;
  switch (direction) {
    case FocusMoveDirection::LEFT:
      distance = node_rect.x() - item_rect.right();
      break;
    case FocusMoveDirection::RIGHT:
      distance = item_rect.x() - node_rect.right();
      break;
    case FocusMoveDirection::UP:
      distance = node_rect.y() - item_rect.bottom();
      break;
    case FocusMoveDirection::DOWN:
      distance = item_rect.y() - node_rect.bottom();
      break;
    default:
      break;
  }

  return distance > 0.0 ? distance : 0.0;
}

double BrowserAccessibilityOHOS::MinorAxisDistance(const gfx::Rect& node_rect,
                                                   const gfx::Rect& item_rect,
                                                   const int32_t direction) {
  double distance = 0.0;
  switch (direction) {
    case FocusMoveDirection::LEFT:
    case FocusMoveDirection::RIGHT:
      // / 2  is to get the intermediate value
      distance = fabs((node_rect.y() + node_rect.bottom()) / 2 -
                      (item_rect.y() + item_rect.bottom()) / 2);
      break;
    case FocusMoveDirection::UP:
    case FocusMoveDirection::DOWN:
      distance = fabs((node_rect.x() + node_rect.right()) / 2 -
                      (item_rect.x() + item_rect.right()) / 2);
      break;
    default:
      break;
  }

  return distance > 0.0 ? distance : -distance;
}

double BrowserAccessibilityOHOS::GetWeightedDistanceFor(
    double major_axis_distance,
    double minor_axis_distance) {
  return WEIGHTED_VALUE * major_axis_distance * major_axis_distance +
         minor_axis_distance * minor_axis_distance;
}

bool BrowserAccessibilityOHOS::IsCandidateRect(const gfx::Rect& node_rect,
                                               const gfx::Rect& item_rect,
                                               const int32_t direction) {
  switch (direction) {
    case FocusMoveDirection::LEFT:
      return node_rect.x() > item_rect.x() &&
             node_rect.right() > item_rect.right();
    case FocusMoveDirection::RIGHT:
      return node_rect.x() < item_rect.x() &&
             node_rect.right() < item_rect.right();
    case FocusMoveDirection::UP:
      return node_rect.y() > item_rect.y() &&
             node_rect.bottom() > item_rect.bottom();
    case FocusMoveDirection::DOWN:
      return node_rect.y() < item_rect.y() &&
             node_rect.bottom() < item_rect.bottom();
    default:
      break;
  }
  return false;
}

// Check whether rect1 is outright better than rect2.
bool BrowserAccessibilityOHOS::OutrightBetter(const gfx::Rect& node_rect,
                                              const int32_t direction,
                                              const gfx::Rect& rect1,
                                              const gfx::Rect& rect2) {
  bool rect1InSrcBeam = CheckRectBeam(node_rect, rect1, direction);
  bool rect2InSrcBeam = CheckRectBeam(node_rect, rect2, direction);
  if (rect2InSrcBeam || !rect1InSrcBeam) {
    return false;
  }

  if (!IsToDirectionOf(node_rect, rect2, direction)) {
    return true;
  }

  // for direction left or right
  if (direction == FocusMoveDirection::LEFT ||
      direction == FocusMoveDirection::RIGHT) {
    return true;
  }

  return (MajorAxisDistance(node_rect, rect1, direction) <
          MajorAxisDistanceToFarEdge(node_rect, rect2, direction));
}

bool BrowserAccessibilityOHOS::CheckBetterRect(const gfx::Rect& node_rect,
                                               const int32_t direction,
                                               const gfx::Rect& item_rect,
                                               const gfx::Rect& tempBest) {
  if (!IsCandidateRect(node_rect, item_rect, direction)) {
    return false;
  }

  if (!IsCandidateRect(node_rect, tempBest, direction)) {
    return true;
  }

  // now both of item and tempBest are all at the direction of node.
  if (OutrightBetter(node_rect, direction, item_rect, tempBest)) {
    return true;
  }

  if (OutrightBetter(node_rect, direction, tempBest, item_rect)) {
    return false;
  }

  // otherwise, do fudge-tastic comparison of the major and minor axis
  return (GetWeightedDistanceFor(
      MajorAxisDistance(node_rect, item_rect, direction),
      MinorAxisDistance(node_rect, item_rect, direction)) <
  GetWeightedDistanceFor(
      MajorAxisDistance(node_rect, tempBest, direction),
      MinorAxisDistance(node_rect, tempBest, direction)));
}

float BrowserAccessibilityOHOS::RangeMin() const {
  return GetFloatAttribute(ax::mojom::FloatAttribute::kMinValueForRange);
}

float BrowserAccessibilityOHOS::RangeMax() const {
  return GetFloatAttribute(ax::mojom::FloatAttribute::kMaxValueForRange);
}

float BrowserAccessibilityOHOS::RangeCurrentValue() const {
  return GetFloatAttribute(ax::mojom::FloatAttribute::kValueForRange);
}

std::string BrowserAccessibilityOHOS::GetRoleString() const {
  return ui::ToString(GetRole());
}

std::string BrowserAccessibilityOHOS::GetTargetUrl() const {
  if (ui::IsImageOrVideo(GetRole()) || ui::IsLink(GetRole())) {
    return GetStringAttribute(ax::mojom::StringAttribute::kUrl);
  }
  return {};
}

std::u16string BrowserAccessibilityOHOS::GetTextContentUTF16() const {
  return GetSubstringTextContentUTF16(std::nullopt);
}

std::u16string BrowserAccessibilityOHOS::CombineTextAndValue(
    const std::u16string& value,
    const std::u16string& name) const {
  std::u16string text = name;
  if (ui::IsRangeValueSupported(GetRole())) {
    // To prevent extra commas, only add if the text is non-empty
    if (!text.empty() && !value.empty()) {
      text = value + u", " + text;
    } else if (!value.empty()) {
      text = value;
    }
  } else if (text.empty()) {
    // When a node does not have a name (e.g. a label), use its value instead.
    text = value;
  }
  return text;
}

bool BrowserAccessibilityOHOS::ShouldUseChildTextContent() const {
  return (HasOnlyTextChildren() && !HasListMarkerChild()) ||
         (IsFocusable() && HasOnlyTextAndImageChildren());
}

bool BrowserAccessibilityOHOS::ShouldUseUrlAsText() const {
  return (ui::IsLink(GetRole()) || ui::IsImageOrVideo(GetRole())) &&
         !HasExplicitlyEmptyName();
}

std::u16string BrowserAccessibilityOHOS::GetSubstringTextContentUTF16(
    std::optional<EarlyExitPredicate> predicate) const {
  if (ui::IsIframe(GetRole())) {
    return std::u16string();
  }

  // First, always return the |value| attribute if this is an
  // input field.
  std::u16string value = GetValueForControl();
  if (ShouldExposeValueAsName()) {
    return value;
  }

  // For color wells, the color is stored in separate attributes.
  // Perhaps we could return color names in the future?
  if (GetRole() == ax::mojom::Role::kColorWell) {
    unsigned int color = static_cast<unsigned int>(
        GetIntAttribute(ax::mojom::IntAttribute::kColorValue));
    return base::UTF8ToUTF16(skia::SkColorToHexString(color));
  }

  std::u16string text = CombineTextAndValue(value, GetNameAsString16());

  // For almost all focusable nodes we try to get text from contents, but for
  // the root node that's redundant and often way too verbose.
  // A role="separator" is a leaf, and cannot get name from contents, even if
  // author appends text children.
  if (ui::IsPlatformDocument(GetRole()) ||
      GetRole() == ax::mojom::Role::kSplitter) {
    return text;
  }

  // Append image description strings to the text.
  auto status = GetData().GetImageAnnotationStatus();
  switch (status) {
    case ax::mojom::ImageAnnotationStatus::kEligibleForAnnotation:
    case ax::mojom::ImageAnnotationStatus::kAnnotationPending:
    case ax::mojom::ImageAnnotationStatus::kAnnotationEmpty:
    case ax::mojom::ImageAnnotationStatus::kAnnotationAdult:
    case ax::mojom::ImageAnnotationStatus::kAnnotationProcessFailed:
      AppendTextToString(GetLocalizedStringForImageAnnotationStatus(status),
                         &text);
      break;
    case ax::mojom::ImageAnnotationStatus::kAnnotationSucceeded:
      text = GetString16Attribute(ax::mojom::StringAttribute::kImageAnnotation);
      break;
    default:
      break;
  }

  // This is called from IsLeaf, so don't call PlatformChildCount
  // from within this!
  if (text.empty() && ShouldUseChildTextContent()) {
    for (auto it = InternalChildrenBegin(); it != InternalChildrenEnd(); ++it) {
      text += static_cast<BrowserAccessibilityOHOS*>(it.get())
                  ->GetSubstringTextContentUTF16(predicate);
      if (predicate && predicate.value().Run(text)) {
        break;
      }
    }
  }

  if (text.empty() && ShouldUseUrlAsText()) {
    std::u16string url = GetString16Attribute(ax::mojom::StringAttribute::kUrl);
    text = ui::AXUrlBaseText(url);
  }

  return text;
}

bool BrowserAccessibilityOHOS::HasOnlyTextAndImageChildren() const {
  // This is called from IsLeaf, so don't call PlatformChildCount
  // from within this!
  for (auto it = InternalChildrenBegin(); it != InternalChildrenEnd(); ++it) {
    BrowserAccessibility* child = it.get();
    if (!child->IsText() && !ui::IsImageOrVideo(child->GetRole())) {
      return false;
    }
  }
  return true;
}

bool BrowserAccessibilityOHOS::HasListMarkerChild() const {
  // This is called from IsLeaf, so don't call PlatformChildCount
  // from within this!
  for (auto it = InternalChildrenBegin(); it != InternalChildrenEnd(); ++it) {
    if (it->GetRole() == ax::mojom::Role::kListMarker) {
      return true;
    }
  }
  return false;
}

void BrowserAccessibilityOHOS::AppendTextToString(
    std::u16string extra_text,
    std::u16string* string) const {
  if (extra_text.empty()) {
    return;
  }

  if (string->empty()) {
    *string = extra_text;
    return;
  }

  *string += std::u16string(u", ") + extra_text;
}

ui::AXPlatformNode* BrowserAccessibilityOHOS::GetAXPlatformNode() const {
  return platform_node_;
}

gfx::NativeViewAccessible BrowserAccessibilityOHOS::GetNativeViewAccessible() {
  DCHECK(platform_node_);
  return platform_node_->GetNativeViewAccessible();
}

}  // namespace content
