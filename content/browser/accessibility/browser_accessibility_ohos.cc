// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/accessibility/browser_accessibility_ohos.h"
#include <codecvt>
#include <locale>
#include "content/browser/accessibility/browser_accessibility_manager_ohos.h"
#include "content/public/common/content_client.h"
#include "ohos_nweb/src/cef_delegate/nweb_accessibility_utils.h"
#include "skia/ext/skia_utils_base.h"
#include "third_party/blink/public/strings/grit/blink_strings.h"
#include "ui/accessibility/ax_assistant_structure.h"
#include "ui/accessibility/ax_selection.h"

namespace content {
using namespace OHOS::NWeb;

using AccessibilityIdMap = std::unordered_map<int64_t, BrowserAccessibilityOHOS*>;

base::LazyInstance<AccessibilityIdMap>::Leaky g_accessibility_id_map =
    LAZY_INSTANCE_INITIALIZER;

std::unique_ptr<BrowserAccessibility> BrowserAccessibility::Create(
    BrowserAccessibilityManager* manager,
    ui::AXNode* node) {
  return std::unique_ptr<BrowserAccessibilityOHOS>(
      new BrowserAccessibilityOHOS(manager, node));
}

BrowserAccessibilityOHOS::BrowserAccessibilityOHOS(
    BrowserAccessibilityManager* manager,
    ui::AXNode* node)
    : BrowserAccessibility(manager, node) {
  accessibility_id_ = static_cast<int64_t>(GetUniqueId().Get());
  g_accessibility_id_map.Get()[accessibility_id_] = this;
}

BrowserAccessibilityOHOS::~BrowserAccessibilityOHOS() {
  g_accessibility_id_map.Get().erase(accessibility_id_);
}

int64_t BrowserAccessibilityOHOS::GetAccessibilityId() const {
  return accessibility_id_;
}

BrowserAccessibilityOHOS* BrowserAccessibilityOHOS::GetFromAccessibilityId(
    int64_t accessibility_id) {
  AccessibilityIdMap* unique_ids = g_accessibility_id_map.Pointer();
  auto iter = unique_ids->find(accessibility_id);
  if (iter != unique_ids->end())
    return iter->second;

  return nullptr;
}

bool BrowserAccessibilityOHOS::IsEnabled() const {
  switch (GetData().GetRestriction()) {
    case ax::mojom::Restriction::kNone:
      return true;
    case ax::mojom::Restriction::kReadOnly:
    case ax::mojom::Restriction::kDisabled:
      return false;
  }
  return true;
}

std::string BrowserAccessibilityOHOS::GetHint() const {
  std::vector<std::string> strings;

  if (ShouldExposeValueAsName()) {
    std::string name = GetName();
    if (!name.empty())
      strings.emplace_back(name);
  }

  if (GetData().GetNameFrom() != ax::mojom::NameFrom::kPlaceholder) {
    std::string placeholder =
        GetStringAttribute(ax::mojom::StringAttribute::kPlaceholder);
    if (!placeholder.empty())
      strings.emplace_back(placeholder);
  }

  std::string description =
      GetStringAttribute(ax::mojom::StringAttribute::kDescription);
  if (!description.empty())
    strings.emplace_back(description);

  return base::JoinString(strings, " ");
}

bool BrowserAccessibilityOHOS::IsHint() const {
  std::vector<std::string> strings;

  if (ShouldExposeValueAsName()) {
    std::string name = GetName();
    if (!name.empty())
      strings.emplace_back(name);
  }

  if (GetData().GetNameFrom() != ax::mojom::NameFrom::kPlaceholder) {
    std::string placeholder =
        GetStringAttribute(ax::mojom::StringAttribute::kPlaceholder);
    if (!placeholder.empty())
      strings.emplace_back(placeholder);
  }

  std::string description =
      GetStringAttribute(ax::mojom::StringAttribute::kDescription);
  if (!description.empty())
    strings.emplace_back(description);
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

  if (GetData().IsRangeValueSupported())
    return false;

  if (IsTextField())
    return true;

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
      message_id = CONTENT_INVALID_TRUE;
      for (auto it = InternalChildrenBegin(); it != InternalChildrenEnd();
           ++it) {
        BrowserAccessibility* child = it.get();
        if (child && child->IsText()) {
          const std::vector<int32_t>& marker_types = child->GetIntListAttribute(
              ax::mojom::IntListAttribute::kMarkerTypes);

          for (const auto& marker_type : marker_types) {
            if (marker_type &
                static_cast<int32_t>(ax::mojom::MarkerType::kSpelling)) {
              message_id = CONTENT_INVALID_SPELLING;
              break;
            } else if (marker_type &
                       static_cast<int32_t>(ax::mojom::MarkerType::kGrammar)) {
              message_id = CONTENT_INVALID_GRAMMAR;
              break;
            }
          }
        }
      }
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

int32_t BrowserAccessibilityOHOS::OHOSInputType() const {
  std::string html_tag =
      GetStringAttribute(ax::mojom::StringAttribute::kHtmlTag);
  if (html_tag != "input")
    return static_cast<int32_t>(AceTextCategory::INPUT_TYPE_DEFAULT);

  std::string type;
  if (!node()->GetStringAttribute(ax::mojom::StringAttribute::kInputType,
                                  &type))
    return static_cast<int32_t>(AceTextCategory::INPUT_TYPE_DEFAULT);

  if (type.empty() || type == "text" || type == "search")
    return static_cast<int32_t>(AceTextCategory::INPUT_TYPE_TEXT);
  else if (type == "date")
    return static_cast<int32_t>(AceTextCategory::INPUT_TYPE_DATE);
  else if (type == "datetime" || type == "datetime-local")
    return static_cast<int32_t>(AceTextCategory::INPUT_TYPE_TIME);
  else if (type == "email")
    return static_cast<int32_t>(AceTextCategory::INPUT_TYPE_EMAIL);
  else if (type == "month")
    return static_cast<int32_t>(AceTextCategory::INPUT_TYPE_DATE);
  else if (type == "number")
    return static_cast<int32_t>(AceTextCategory::INPUT_TYPE_NUMBER);
  else if (type == "password")
    return static_cast<int32_t>(AceTextCategory::INPUT_TYPE_PASSWORD);
  else if (type == "tel")
    return static_cast<int32_t>(AceTextCategory::INPUT_TYPE_PHONENUMBER);
  else if (type == "time")
    return static_cast<int32_t>(AceTextCategory::INPUT_TYPE_TIME);
  else if (type == "url")
    return static_cast<int32_t>(AceTextCategory::INPUT_TYPE_DEFAULT);
  else if (type == "week")
    return static_cast<int32_t>(AceTextCategory::INPUT_TYPE_DATE);

  return static_cast<int32_t>(AceTextCategory::INPUT_TYPE_DEFAULT);
}

int32_t BrowserAccessibilityOHOS::OHOSLiveRegionType() const {
  std::string live =
      GetStringAttribute(ax::mojom::StringAttribute::kLiveStatus);
  if (live == "polite")
    return 1;
  else if (live == "assertive")
    return 1;
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
  while (position->GetAnchor() && position->GetAnchor() != node())
    position = position->CreateParentPosition();

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
  int count = 0;
  if (IsRangeControlWithoutAriaValueText()) {
    count = 100;
  } else {
    if (IsCollection() && node()->GetSetSize())
      count = *node()->GetSetSize();
  }
  return count;
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
          GetRole() == ax::mojom::Role::kDirectory ||
          GetRole() == ax::mojom::Role::kTree);
}

int32_t BrowserAccessibilityOHOS::RowCount() const {
  if (!IsCollection())
    return 0;

  if (node()->GetSetSize())
    return *node()->GetSetSize();

  return node()->GetTableRowCount().value_or(0);
}

int32_t BrowserAccessibilityOHOS::ColumnCount() const {
  if (IsCollection())
    return node()->GetTableColCount().value_or(0);
  return 0;
}

int32_t BrowserAccessibilityOHOS::RowIndex() const {
  absl::optional<int> pos_in_set = node()->GetPosInSet();
  if (pos_in_set && pos_in_set > 0)
    return *pos_in_set - 1;
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
          GetRole() == ax::mojom::Role::kDescriptionListTerm ||
          GetRole() == ax::mojom::Role::kListBoxOption ||
          GetRole() == ax::mojom::Role::kListItem ||
          GetRole() == ax::mojom::Role::kRowHeader ||
          GetRole() == ax::mojom::Role::kTreeItem);
}

bool BrowserAccessibilityOHOS::IsHeading() const {
  BrowserAccessibilityOHOS* parent =
      static_cast<BrowserAccessibilityOHOS*>(PlatformGetParent());
  if (parent && parent->IsHeading())
    return true;

  return ui::IsHeading(GetRole());
}

bool BrowserAccessibilityOHOS::IsLink() const {
  return ui::IsLink(GetRole());
}

bool BrowserAccessibilityOHOS::IsHierarchical() const {
  return (GetRole() == ax::mojom::Role::kTree || IsHierarchicalList());
}

bool BrowserAccessibilityOHOS::HasOnlyTextChildren() const {
  for (auto& childId : childrenIds_) {
    BrowserAccessibilityOHOS* child = GetFromAccessibilityId(childId);
    if (child && !child->IsText() &&
        child->GetRole() != ax::mojom::Role::kStrong &&
        !child->IsEmptyContainer()) {
      return false;
    }
  }
  return true;
}

bool BrowserAccessibilityOHOS::HasClickableChildren() const {
  for (auto& childNode : PlatformChildren()) {
    BrowserAccessibilityOHOS& childNodeOHOS =
        static_cast<BrowserAccessibilityOHOS&>(childNode);
    if (childNodeOHOS.IsClickable()) {
      return true;
    }
  }
  return false;
}

BrowserAccessibilityOHOS*
BrowserAccessibilityOHOS::GetAccessibilityNodeByFocusMove(
    int32_t direction) const {
  std::list<BrowserAccessibilityOHOS*> nodeList;
  BrowserAccessibilityOHOS* resultNode = nullptr;

  if (!manager_) {
    return resultNode;
  }
  auto root = static_cast<BrowserAccessibilityOHOS*>(manager_->GetBrowserAccessibilityRoot());
  if (!root) {
    return resultNode;
  }
  root->AddFocusableNode(nodeList);

  switch (direction) {
    case FocusMoveDirection::FORWARD:
    case FocusMoveDirection::BACKWARD:
      resultNode = FindNodeInRelativeDirection(nodeList, direction);
      break;
    case FocusMoveDirection::UP:
    case FocusMoveDirection::DOWN:
    case FocusMoveDirection::LEFT:
    case FocusMoveDirection::RIGHT:
      resultNode = FindNodeInAbsoluteDirection(nodeList, direction);
      break;
    default:
      break;
  }
  return resultNode;
}

void BrowserAccessibilityOHOS::AddFocusableNode(
    std::list<BrowserAccessibilityOHOS*>& nodeList) const {
  for (auto& childNode : PlatformChildren()) {
    BrowserAccessibilityOHOS& childNodeOHOS =
        static_cast<BrowserAccessibilityOHOS&>(childNode);
    nodeList.emplace_back(&childNodeOHOS);
    childNodeOHOS.AddFocusableNode(nodeList);
  }
}

BrowserAccessibilityOHOS*
BrowserAccessibilityOHOS::FindNodeInRelativeDirection(
    const std::list<BrowserAccessibilityOHOS*>& nodeList,
    int32_t direction) const {
  switch (direction) {
    case FocusMoveDirection::FORWARD:
      return GetNextFocusableNode(nodeList);
    case FocusMoveDirection::BACKWARD:
      return GetPreviousFocusableNode(nodeList);
    default:
      break;
  }

  return nullptr;
}

BrowserAccessibilityOHOS*
BrowserAccessibilityOHOS::FindNodeInAbsoluteDirection(
    const std::list<BrowserAccessibilityOHOS*>& nodeList,
    int32_t direction) const {
  ui::AXOffscreenResult offscreen_result = ui::AXOffscreenResult::kOnscreen;
  float dip_scale = manager_->device_scale_factor();
  gfx::Rect rect = gfx::ScaleToEnclosingRect(
      GetUnclippedRootFrameBoundsRect(&offscreen_result), dip_scale, dip_scale);
  gfx::Rect tempBest = rect;
  auto nodeRect = tempBest;
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

  BrowserAccessibilityOHOS* nearestNode = nullptr;
  for (const auto& nodeItem : nodeList) {
    if (nodeItem->GetAccessibilityId() == accessibility_id_ ||
        !nodeItem->PlatformGetParent()) {
      continue;
    }
    rect = gfx::ScaleToEnclosingRect(
        nodeItem->GetUnclippedRootFrameBoundsRect(&offscreen_result), dip_scale,
        dip_scale);
    auto itemRect = rect;
    if (CheckBetterRect(nodeRect, direction, itemRect, tempBest)) {
      tempBest = itemRect;
      nearestNode = nodeItem;
    }
  }
  return nearestNode;
}

BrowserAccessibilityOHOS* BrowserAccessibilityOHOS::GetNextFocusableNode(
    const std::list<BrowserAccessibilityOHOS*>& nodeList) const {
  auto nodeItem = nodeList.begin();
  for (; nodeItem != nodeList.end(); nodeItem++) {
    if ((*nodeItem)->GetAccessibilityId() == accessibility_id_) {
      break;
    }
  }

  if (nodeItem != nodeList.end()) {
    if (++nodeItem != nodeList.end()) {
      return (*nodeItem);
    }
  }
  if (!nodeList.empty()) {
    return (*nodeList.begin());
  }

  return nullptr;
}

BrowserAccessibilityOHOS*
BrowserAccessibilityOHOS::GetPreviousFocusableNode(
    const std::list<BrowserAccessibilityOHOS*>& nodeList) const {
  auto nodeItem = nodeList.rbegin();
  for (; nodeItem != nodeList.rend(); nodeItem++) {
    if ((*nodeItem)->GetAccessibilityId() == accessibility_id_) {
      break;
    }
  }

  if (nodeItem != nodeList.rend()) {
    if (++nodeItem != nodeList.rend()) {
      return (*nodeItem);
    }
  }
  if (!nodeList.empty()) {
    return (*nodeList.rbegin());
  }

  return nullptr;
}

bool BrowserAccessibilityOHOS::CheckRectBeam(const gfx::Rect& nodeRect,
                                             const gfx::Rect& itemRect,
                                             const int32_t direction) {
  switch (direction) {
    case FocusMoveDirection::LEFT:
    case FocusMoveDirection::RIGHT:
      return nodeRect.y() < itemRect.bottom() &&
             itemRect.y() < nodeRect.bottom();
    case FocusMoveDirection::UP:
    case FocusMoveDirection::DOWN:
      return nodeRect.x() < itemRect.right() && itemRect.x() < nodeRect.right();
    default:
      break;
  }
  return false;
}

bool BrowserAccessibilityOHOS::IsToDirectionOf(const gfx::Rect& nodeRect,
                                               const gfx::Rect& itemRect,
                                               const int32_t direction) {
  switch (direction) {
    case FocusMoveDirection::LEFT:
      return nodeRect.x() >= itemRect.right();
    case FocusMoveDirection::RIGHT:
      return nodeRect.right() <= itemRect.x();
    case FocusMoveDirection::UP:
      return nodeRect.y() >= itemRect.bottom();
    case FocusMoveDirection::DOWN:
      return nodeRect.bottom() <= itemRect.y();
    default:
      break;
  }
  return false;
}

double BrowserAccessibilityOHOS::MajorAxisDistanceToFarEdge(
    const gfx::Rect& nodeRect,
    const gfx::Rect& itemRect,
    const int32_t direction) {
  double distance = 0.0;
  switch (direction) {
    case FocusMoveDirection::LEFT:
      distance = nodeRect.x() - itemRect.x();
      break;
    case FocusMoveDirection::RIGHT:
      distance = itemRect.right() - nodeRect.right();
      break;
    case FocusMoveDirection::UP:
      distance = nodeRect.y() - itemRect.y();
      break;
    case FocusMoveDirection::DOWN:
      distance = itemRect.bottom() - nodeRect.bottom();
      break;
    default:
      break;
  }

  return distance > 1.0 ? distance : 1.0;
}

double BrowserAccessibilityOHOS::MajorAxisDistance(const gfx::Rect& nodeRect,
                                                   const gfx::Rect& itemRect,
                                                   const int32_t direction) {
  double distance = 0.0;
  switch (direction) {
    case FocusMoveDirection::LEFT:
      distance = nodeRect.x() - itemRect.right();
      break;
    case FocusMoveDirection::RIGHT:
      distance = itemRect.x() - nodeRect.right();
      break;
    case FocusMoveDirection::UP:
      distance = nodeRect.y() - itemRect.bottom();
      break;
    case FocusMoveDirection::DOWN:
      distance = itemRect.y() - nodeRect.bottom();
      break;
    default:
      break;
  }

  return distance > 0.0 ? distance : 0.0;
}

double BrowserAccessibilityOHOS::MinorAxisDistance(const gfx::Rect& nodeRect,
                                                   const gfx::Rect& itemRect,
                                                   const int32_t direction) {
  double distance = 0.0;
  switch (direction) {
    case FocusMoveDirection::LEFT:
    case FocusMoveDirection::RIGHT:
      distance = fabs((nodeRect.y() + nodeRect.bottom()) / 2 -
                      (itemRect.y() + itemRect.bottom()) / 2);
      break;
    case FocusMoveDirection::UP:
    case FocusMoveDirection::DOWN:
      distance = fabs((nodeRect.x() + nodeRect.right()) / 2 -
                      (itemRect.x() + itemRect.right()) / 2);
      break;
    default:
      break;
  }

  return distance > 0.0 ? distance : -distance;
}

double BrowserAccessibilityOHOS::GetWeightedDistanceFor(
    double majorAxisDistance,
    double minorAxisDistance) {
  return WEIGHTED_VALUE * majorAxisDistance * majorAxisDistance +
         minorAxisDistance * minorAxisDistance;
}

bool BrowserAccessibilityOHOS::IsCandidateRect(const gfx::Rect& nodeRect,
                                               const gfx::Rect& itemRect,
                                               const int32_t direction) {
  switch (direction) {
    case FocusMoveDirection::LEFT:
      return nodeRect.x() > itemRect.x() && nodeRect.right() > itemRect.right();
    case FocusMoveDirection::RIGHT:
      return nodeRect.x() < itemRect.x() && nodeRect.right() < itemRect.right();
    case FocusMoveDirection::UP:
      return nodeRect.y() > itemRect.y() &&
             nodeRect.bottom() > itemRect.bottom();
    case FocusMoveDirection::DOWN:
      return nodeRect.y() < itemRect.y() &&
             nodeRect.bottom() < itemRect.bottom();
    default:
      break;
  }
  return false;
}

// Check whether rect1 is outright better than rect2.
bool BrowserAccessibilityOHOS::OutrightBetter(const gfx::Rect& nodeRect,
                                              const int32_t direction,
                                              const gfx::Rect& rect1,
                                              const gfx::Rect& rect2) {
  bool rect1InSrcBeam = CheckRectBeam(nodeRect, rect1, direction);
  bool rect2InSrcBeam = CheckRectBeam(nodeRect, rect2, direction);
  if (rect2InSrcBeam || !rect1InSrcBeam) {
    return false;
  }

  if (!IsToDirectionOf(nodeRect, rect2, direction)) {
    return true;
  }

  // for direction left or right
  if (direction == FocusMoveDirection::LEFT ||
      direction == FocusMoveDirection::RIGHT) {
    return true;
  }

  return (MajorAxisDistance(nodeRect, rect1, direction) <
          MajorAxisDistanceToFarEdge(nodeRect, rect2, direction));
}

bool BrowserAccessibilityOHOS::CheckBetterRect(const gfx::Rect& nodeRect,
                                               const int32_t direction,
                                               const gfx::Rect& itemRect,
                                               const gfx::Rect& tempBest) {
  if (!IsCandidateRect(nodeRect, itemRect, direction)) {
    return false;
  }

  if (!IsCandidateRect(nodeRect, tempBest, direction)) {
    return true;
  }

  // now both of item and tempBest are all at the direction of node.
  if (OutrightBetter(nodeRect, direction, itemRect, tempBest)) {
    return true;
  }

  if (OutrightBetter(nodeRect, direction, tempBest, itemRect)) {
    return false;
  }

  // otherwise, do fudge-tastic comparison of the major and minor axis
  return (
      GetWeightedDistanceFor(MajorAxisDistance(nodeRect, itemRect, direction),
                             MinorAxisDistance(nodeRect, itemRect, direction)) <
      GetWeightedDistanceFor(MajorAxisDistance(nodeRect, tempBest, direction),
                             MinorAxisDistance(nodeRect, tempBest, direction)));
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
  return GetSubstringTextContentUTF16(absl::nullopt);
}

std::u16string BrowserAccessibilityOHOS::GetSubstringTextContentUTF16(
    absl::optional<EarlyExitPredicate> predicate) const {
  if (ui::IsIframe(GetRole()) || GetRole() == ax::mojom::Role::kCell)
    return std::u16string();

  // First, always return the |value| attribute if this is an
  // input field.
  std::u16string value = GetValueForControl();
  if (ShouldExposeValueAsName())
    return value;

  // For color wells, the color is stored in separate attributes.
  // Perhaps we could return color names in the future?
  if (GetRole() == ax::mojom::Role::kColorWell) {
    unsigned int color = static_cast<unsigned int>(
        GetIntAttribute(ax::mojom::IntAttribute::kColorValue));
    return base::UTF8ToUTF16(skia::SkColorToHexString(color));
  }

  std::u16string text = GetNameAsString16();
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

  // For almost all focusable nodes we try to get text from contents, but for
  // the root node that's redundant and often way too verbose.
  if (ui::IsPlatformDocument(GetRole()))
    return text;

  // A role="separator" is a leaf, and cannot get name from contents, even if
  // author appends text children.
  if (GetRole() == ax::mojom::Role::kSplitter)
    return text;

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
      text =
          GetString16Attribute(ax::mojom::StringAttribute::kImageAnnotation);
      break;

    case ax::mojom::ImageAnnotationStatus::kNone:
    case ax::mojom::ImageAnnotationStatus::kWillNotAnnotateDueToScheme:
    case ax::mojom::ImageAnnotationStatus::kIneligibleForAnnotation:
    case ax::mojom::ImageAnnotationStatus::kSilentlyEligibleForAnnotation:
      break;
  }

  // This is called from IsLeaf, so don't call PlatformChildCount
  // from within this!
  if (text.empty() && ((HasOnlyTextChildren() && !HasListMarkerChild()) ||
                       (IsFocusable() && HasOnlyTextAndImageChildren()))) {
    for (auto it = InternalChildrenBegin(); it != InternalChildrenEnd(); ++it) {
      text += static_cast<BrowserAccessibilityOHOS*>(it.get())
                  ->GetSubstringTextContentUTF16(predicate);
      if (predicate && predicate.value().Run(text)) {
        break;
      }
    }
  }

  if (text.empty() &&
      (ui::IsLink(GetRole()) || ui::IsImageOrVideo(GetRole())) &&
      !HasExplicitlyEmptyName()) {
    std::u16string url = GetString16Attribute(ax::mojom::StringAttribute::kUrl);
    text = ui::AXUrlBaseText(url);
  }

  return text;
}

bool BrowserAccessibilityOHOS::HasOnlyTextAndImageChildren() const {
  for (auto& childId : childrenIds_) {
    BrowserAccessibilityOHOS* child = GetFromAccessibilityId(childId);
    if (child && !child->IsText() &&
        child->GetRole() != ax::mojom::Role::kStrong &&
        !ui::IsImageOrVideo(child->GetRole())) {
      return false;
    }
  }
  return true;
}

bool BrowserAccessibilityOHOS::HasListMarkerChild() const {
  // This is called from IsLeaf, so don't call PlatformChildCount
  // from within this!
  for (auto it = InternalChildrenBegin(); it != InternalChildrenEnd(); ++it) {
    if (it->GetRole() == ax::mojom::Role::kListMarker)
      return true;
  }
  return false;
}

void BrowserAccessibilityOHOS::AppendTextToString(
    std::u16string extra_text,
    std::u16string* string) const {
  if (extra_text.empty())
    return;

  if (string->empty()) {
    *string = extra_text;
    return;
  }

  *string += std::u16string(u", ") + extra_text;
}

bool BrowserAccessibilityOHOS::IsClickable() const {
  if (HasIntAttribute(ax::mojom::IntAttribute::kDefaultActionVerb) &&
      (GetData().GetDefaultActionVerb() !=
       ax::mojom::DefaultActionVerb::kClickAncestor)) {
    return true;
  }
  if (IsHeadingLink()) {
    return true;
  }
  if (ui::IsIframe(GetRole()) || ui::IsPlatformDocument(GetRole())) {
    return false;
  }
  return ui::IsControlOnOHOS(GetRole(), IsFocusable());
}

bool BrowserAccessibilityOHOS::IsHeadingLink() const {
  if (!(GetRole() == ax::mojom::Role::kHeading && InternalChildCount() == 1))
    return false;

  BrowserAccessibilityOHOS* child =
      static_cast<BrowserAccessibilityOHOS*>(InternalChildrenBegin().get());
  return ui::IsLink(child->GetRole());
}

bool BrowserAccessibilityOHOS::IsFocusable() const {
  if (ui::IsIframe(GetRole()) ||
      (ui::IsPlatformDocument(GetRole()) && PlatformGetParent() &&
       PlatformGetParent()->GetRole() != ax::mojom::Role::kPortal)) {
    return HasStringAttribute(ax::mojom::StringAttribute::kName);
  }
  return BrowserAccessibility::IsFocusable();
}

bool BrowserAccessibilityOHOS::IsTableHeader() const {
  return ui::IsTableHeader(GetRole());
}

bool BrowserAccessibilityOHOS::HasNonEmptyValue() const {
  return IsTextField() && !GetValueForControl().empty();
}

bool BrowserAccessibilityOHOS::IsScrollSupported() const {
  if (GetRole() == ax::mojom::Role::kSlider) {
    const std::string& html_tag =
        GetStringAttribute(ax::mojom::StringAttribute::kHtmlTag);
    if (html_tag != "input") {
      return false;
    }
    return true;
  } else {
    return IsScrollable();
  }
}

bool BrowserAccessibilityOHOS::CanScrollForward() const {
  if (GetRole() == ax::mojom::Role::kSlider) {
    const std::string& html_tag =
        GetStringAttribute(ax::mojom::StringAttribute::kHtmlTag);
    if (html_tag != "input")
      return false;

    float value = GetFloatAttribute(ax::mojom::FloatAttribute::kValueForRange);
    float max = GetFloatAttribute(ax::mojom::FloatAttribute::kMaxValueForRange);
    return value < max;
  } else {
    return CanScrollRight() || CanScrollDown();
  }
}

bool BrowserAccessibilityOHOS::CanScrollBackward() const {
  if (GetRole() == ax::mojom::Role::kSlider) {
    const std::string& html_tag =
        GetStringAttribute(ax::mojom::StringAttribute::kHtmlTag);
    if (html_tag != "input")
      return false;

    float value = GetFloatAttribute(ax::mojom::FloatAttribute::kValueForRange);
    float min = GetFloatAttribute(ax::mojom::FloatAttribute::kMinValueForRange);
    return value > min;
  } else {
    return CanScrollLeft() || CanScrollUp();
  }
}

bool BrowserAccessibilityOHOS::CanScrollUp() const {
  return GetScrollY() > GetMinScrollY() && IsScrollable();
}

bool BrowserAccessibilityOHOS::CanScrollDown() const {
  return GetScrollY() < GetMaxScrollY() && IsScrollable();
}

bool BrowserAccessibilityOHOS::CanScrollLeft() const {
  return GetScrollX() > GetMinScrollX() && IsScrollable();
}

bool BrowserAccessibilityOHOS::CanScrollRight() const {
  return GetScrollX() < GetMaxScrollX() && IsScrollable();
}

int BrowserAccessibilityOHOS::GetScrollX() const {
  int value = 0;
  GetIntAttribute(ax::mojom::IntAttribute::kScrollX, &value);
  return value;
}

int BrowserAccessibilityOHOS::GetScrollY() const {
  int value = 0;
  GetIntAttribute(ax::mojom::IntAttribute::kScrollY, &value);
  return value;
}

int BrowserAccessibilityOHOS::GetMinScrollX() const {
  return GetIntAttribute(ax::mojom::IntAttribute::kScrollXMin);
}

int BrowserAccessibilityOHOS::GetMinScrollY() const {
  return GetIntAttribute(ax::mojom::IntAttribute::kScrollYMin);
}

int BrowserAccessibilityOHOS::GetMaxScrollX() const {
  return GetIntAttribute(ax::mojom::IntAttribute::kScrollXMax);
}

int BrowserAccessibilityOHOS::GetMaxScrollY() const {
  return GetIntAttribute(ax::mojom::IntAttribute::kScrollYMax);
}

void BrowserAccessibilityOHOS::Scroll(const ax::mojom::Action& action) const {
  if (GetRole() == ax::mojom::Role::kSlider) {
    if (!IsEnabled()) {
      return;
    }
    float slider_value = GetFloatAttribute(ax::mojom::FloatAttribute::kValueForRange);
    float slider_min = GetFloatAttribute(ax::mojom::FloatAttribute::kMinValueForRange);
    float slider_max = GetFloatAttribute(ax::mojom::FloatAttribute::kMaxValueForRange);
    if (slider_max <= slider_min) {
      return;
    }
    float slider_step = (slider_max - slider_min) / kDefaultStepTicksForSliders;
    if (HasFloatAttribute(ax::mojom::FloatAttribute::kStepValueForRange)) {
      slider_step = GetFloatAttribute(ax::mojom::FloatAttribute::kStepValueForRange);
    }
    float update_value;
    if (ax::mojom::Action::kScrollForward == action) {
      update_value = slider_value + slider_step;
    } else if (ax::mojom::Action::kScrollBackward == action) {
      update_value = slider_value - slider_step;
    } else {
      return;
    }
    update_value = std::clamp(update_value, slider_min, slider_max);
    if (update_value != slider_value) {
      manager()->SetValue(*this, base::NumberToString(update_value));
    }
  } else {
    manager()->Scroll(*this, action);
  }
}

bool BrowserAccessibilityOHOS::IsAccessibilityGroup() const {
  if (ui::IsLink(GetRole())) {
    return true;
  }
  if (GetRole() == ax::mojom::Role::kHeading || GetRole() == ax::mojom::Role::kStrong) {
    return true;
  }
  if (GetRole() == ax::mojom::Role::kParagraph) {
    return HasOnlyTextChildren();
  }
  return false;
}

bool BrowserAccessibilityOHOS::IsIgnoredContainer() const {
  if (GetRole() != ax::mojom::Role::kGenericContainer) {
    return false;
  }
  if (IsClickable() && !HasClickableChildren()) {
    return false;
  }
  if (!PlatformChildCount()) {
    return false;
  }
  return true;
}

bool BrowserAccessibilityOHOS::IsEmptyContainer() const {
  if (GetRole() != ax::mojom::Role::kGenericContainer) {
    return false;
  }
  if (IsClickable() && !HasClickableChildren()) {
    return false;
  }
  if (PlatformChildCount()) {
    return false;
  }
  return true;
}

int64_t BrowserAccessibilityOHOS::GetParentId() const {
  BrowserAccessibilityOHOS* parent = static_cast<content::BrowserAccessibilityOHOS*>(PlatformGetParent());
  while (parent && parent->IsIgnoredContainer()) {
    parent = static_cast<content::BrowserAccessibilityOHOS*>(parent->PlatformGetParent());
  }
  if (parent) {
    return parent->GetAccessibilityId();
  }
  return -1;
}

void BrowserAccessibilityOHOS::GetChildrenIds(std::vector<int64_t>& childrenIds) const {
  for (const auto& childNode : PlatformChildren()) {
    const content::BrowserAccessibilityOHOS& childNodeOHOS =
        static_cast<const content::BrowserAccessibilityOHOS&>(childNode);
    if (!childNodeOHOS.IsIgnoredContainer()) {
      childrenIds.emplace_back(childNodeOHOS.GetAccessibilityId());
    } else {
      childNodeOHOS.GetChildrenIds(childrenIds);
    }
  }
}

void BrowserAccessibilityOHOS::SetChildrenIds(const std::vector<int64_t>& childrenIds) {
  childrenIds_ = childrenIds;
}

}  // namespace content