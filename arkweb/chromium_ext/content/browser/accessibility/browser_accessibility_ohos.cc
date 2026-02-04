// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "browser_accessibility_ohos.h"

#include <codecvt>
#include <locale>

#include "base/check_deref.h"
#include "browser_accessibility_manager_ohos.h"
#include "content/public/common/content_client.h"
#include "ohos_nweb/src/cef_delegate/nweb_accessibility_utils.h"
#include "skia/ext/skia_utils_base.h"
#include "ui/accessibility/ax_assistant_structure.h"
#include "ui/accessibility/ax_role_properties.h"
#include "ui/accessibility/ax_selection.h"
#include "ui/strings/grit/ax_strings.h"

namespace ui {
using namespace OHOS::NWeb;

using AccessibilityIdMap =
    std::unordered_map<int64_t, BrowserAccessibilityOHOS*>;
namespace {
constexpr int NUMBER_TWO = 2;
constexpr int32_t LIVE_REGION_OFF = 0;
constexpr int32_t LIVE_REGION_POLITE = 1;
constexpr int32_t LIVE_REGION_ASSERTIVE = 2;
constexpr int32_t CHECKBOX_GROUP_STATUS_TRUE = 0;
constexpr int32_t CHECKBOX_GROUP_STATUS_MIXED = 1;
constexpr int32_t CHECKBOX_GROUP_STATUS_FALSE = 2;
constexpr int32_t CHECKBOX_GROUP_STATUS_DEFAULT = -1;
}

base::LazyInstance<AccessibilityIdMap>::Leaky g_accessibility_id_map =
    LAZY_INSTANCE_INITIALIZER;

base::LazyInstance<std::unordered_map<std::string, int64_t>>::Leaky
    g_html_element_id_map = LAZY_INSTANCE_INITIALIZER;

base::LazyInstance<std::map<const BrowserAccessibilityOHOS*, bool>>::Leaky
    g_leaf_map = LAZY_INSTANCE_INITIALIZER;

std::unique_ptr<BrowserAccessibility> BrowserAccessibility::Create(
    BrowserAccessibilityManager* manager,
    AXNode* node) {
  return std::unique_ptr<BrowserAccessibilityOHOS>(
      new BrowserAccessibilityOHOS(manager, node));
}

BrowserAccessibilityOHOS::BrowserAccessibilityOHOS(
    BrowserAccessibilityManager* manager,
    AXNode* node)
    : BrowserAccessibility(manager, node) {
  accessibility_id_ = static_cast<int64_t>(GetUniqueId()) + 1;
  g_accessibility_id_map.Get()[accessibility_id_] = this;
  if (node && node->GetRole() == ax::mojom::Role::kEmbeddedObject) {
    const base::StringPairs& htmlAttributes = node->GetHtmlAttributes();
    for (const auto& pair : htmlAttributes) {
      if (base::EqualsCaseInsensitiveASCII(pair.first, "id")) {
        html_element_id_ = pair.second;
        LOG(DEBUG) << "html element id map with accessibility id: "
                   << html_element_id_ << ": " << accessibility_id_;
        g_html_element_id_map.Get()[html_element_id_] = accessibility_id_;
        break;
      }
    }
  }
}

int64_t BrowserAccessibilityOHOS::GetAccessibilityIdByHtmlElementId(
    const std::string& htmlElementId) {
  auto it = g_html_element_id_map.Get().find(htmlElementId);
  if (it != g_html_element_id_map.Get().end()) {
    return it->second;
  } else {
    return -1;
  }
}

BrowserAccessibilityOHOS::~BrowserAccessibilityOHOS() {
  g_accessibility_id_map.Get().erase(accessibility_id_);
  if (!html_element_id_.empty()) {
    g_html_element_id_map.Get().erase(html_element_id_);
  }
  if (g_leaf_map.Get().find(this) != g_leaf_map.Get().end()) {
    g_leaf_map.Get().erase(this);
  }
}

int64_t BrowserAccessibilityOHOS::GetAccessibilityId() const {
  return accessibility_id_;
}

BrowserAccessibilityOHOS* BrowserAccessibilityOHOS::GetFromAccessibilityId(
    int64_t accessibility_id) {
  AccessibilityIdMap* unique_ids = g_accessibility_id_map.Pointer();
  auto iter = unique_ids->find(accessibility_id);
  if (iter != unique_ids->end()) {
    return iter->second;
  }

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

bool BrowserAccessibilityOHOS::IsExpanded() const {
  return HasState(ax::mojom::State::kExpanded);
}

bool BrowserAccessibilityOHOS::IsCollapsed() const {
  return HasState(ax::mojom::State::kCollapsed);
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

std::string BrowserAccessibilityOHOS::GetContentInvalidErrorMessage() const {
  content::ContentClient* content_client = content::GetContentClient();
  if (!content_client) {
    return std::string();
  }
  int message_id = -1;

  if (!IsContentInvalid()) {
    return std::string();
  }

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
            if (marker_type < 0) {
              continue;
            }
            if (static_cast<uint32_t>(marker_type) &
                static_cast<uint32_t>(ax::mojom::MarkerType::kSpelling)) {
              message_id = CONTENT_INVALID_SPELLING;
              break;
            } else if (static_cast<uint32_t>(marker_type) &
                       static_cast<uint32_t>(ax::mojom::MarkerType::kGrammar)) {
              message_id = CONTENT_INVALID_GRAMMAR;
              break;
            }
          }
        }
      }
      break;
  }

  if (message_id != -1) {
    return base::UTF16ToUTF8(content_client->GetLocalizedString(message_id));
  }
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
  if (html_tag != "input") {
    return static_cast<int32_t>(AceTextCategory::INPUT_TYPE_DEFAULT);
  }

  std::string type;
  if (!node()->GetStringAttribute(ax::mojom::StringAttribute::kInputType,
                                  &type)) {
    return static_cast<int32_t>(AceTextCategory::INPUT_TYPE_DEFAULT);
  }

  if (type.empty() || type == "text" || type == "search") {
    return static_cast<int32_t>(AceTextCategory::INPUT_TYPE_TEXT);
  } else if (type == "date") {
    return static_cast<int32_t>(AceTextCategory::INPUT_TYPE_DATE);
  } else if (type == "datetime" || type == "datetime-local") {
    return static_cast<int32_t>(AceTextCategory::INPUT_TYPE_TIME);
  } else if (type == "email") {
    return static_cast<int32_t>(AceTextCategory::INPUT_TYPE_EMAIL);
  } else if (type == "month") {
    return static_cast<int32_t>(AceTextCategory::INPUT_TYPE_DATE);
  } else if (type == "number") {
    return static_cast<int32_t>(AceTextCategory::INPUT_TYPE_NUMBER);
  } else if (type == "password") {
    return static_cast<int32_t>(AceTextCategory::INPUT_TYPE_PASSWORD);
  } else if (type == "tel") {
    return static_cast<int32_t>(AceTextCategory::INPUT_TYPE_PHONENUMBER);
  } else if (type == "time") {
    return static_cast<int32_t>(AceTextCategory::INPUT_TYPE_TIME);
  } else if (type == "url") {
    return static_cast<int32_t>(AceTextCategory::INPUT_TYPE_DEFAULT);
  } else if (type == "week") {
    return static_cast<int32_t>(AceTextCategory::INPUT_TYPE_DATE);
  }

  return static_cast<int32_t>(AceTextCategory::INPUT_TYPE_DEFAULT);
}

int32_t BrowserAccessibilityOHOS::OHOSLiveRegionType() const {
  std::string live =
      GetStringAttribute(ax::mojom::StringAttribute::kLiveStatus);
  if (live == "polite") {
    return LIVE_REGION_POLITE;
  } else if (live == "assertive") {
    return LIVE_REGION_ASSERTIVE;
  }
  return LIVE_REGION_OFF;
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
  int count = 0;
  if (IsRangeControlWithoutAriaValueText()) {
    count = 100;
  } else {
    if (IsCollection() && node()->GetSetSize()) {
      count = *node()->GetSetSize();
    }
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
  absl::optional<int> pos_in_set = node()->GetPosInSet();
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

bool BrowserAccessibilityOHOS::HasOnlyTextChildren() const {
  // This is called from IsLeaf, so don't call PlatformChildCount
  // from within this!
  for (auto it = InternalChildrenBegin(); it != InternalChildrenEnd(); ++it) {
    if (!it->IsText())
      return false;
  }
  return true;
}

BrowserAccessibilityOHOS*
BrowserAccessibilityOHOS::GetAccessibilityNodeByFocusMove(
    int32_t direction) const {
  std::list<BrowserAccessibilityOHOS*> nodeList;
  BrowserAccessibilityOHOS* resultNode = nullptr;

  if (!manager_) {
    return resultNode;
  }

  auto manager = static_cast<BrowserAccessibilityManagerOHOS*>(manager_->GetManagerForRootFrame());
  if (!manager) {
    return resultNode;
  }
  auto root = static_cast<BrowserAccessibilityOHOS*>(
      manager_->GetBrowserAccessibilityRoot());
  if (!root) {
    return resultNode;
  }
  if (direction == FocusMoveDirection::FORWARD) {
    int id = manager->FindElementType(
        accessibility_id_, "", true, false, true);
    if (id != 0) {
      resultNode = GetFromAccessibilityId(id);
    }
  } else if (direction == FocusMoveDirection::BACKWARD) {
    int id = manager->FindElementType(
        accessibility_id_, "", false,
        accessibility_id_ == root->GetAccessibilityId(), true);
    if (id != 0) {
      resultNode = GetFromAccessibilityId(id);
    }
  } else {
    root->AddFocusableNode(nodeList);
    resultNode = FindNodeInAbsoluteDirection(nodeList, direction);
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

BrowserAccessibilityOHOS* BrowserAccessibilityOHOS::FindNodeInRelativeDirection(
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

BrowserAccessibilityOHOS* BrowserAccessibilityOHOS::FindNodeInAbsoluteDirection(
    const std::list<BrowserAccessibilityOHOS*>& nodeList,
    int32_t direction) const {
  AXOffscreenResult offscreen_result = AXOffscreenResult::kOnscreen;
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

BrowserAccessibilityOHOS* BrowserAccessibilityOHOS::GetPreviousFocusableNode(
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
  if (ui::IsIframe(GetRole()) || GetRole() == ax::mojom::Role::kRootWebArea) {
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
  if (ui::IsPlatformDocument(GetRole())) {
    return text;
  }

  // A role="separator" is a leaf, and cannot get name from contents, even if
  // author appends text children.
  if (GetRole() == ax::mojom::Role::kSplitter) {
    return text;
  }

  // Append image description strings to the text.
  auto status = GetData().GetImageAnnotationStatus();
  switch (status) {
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

    case ax::mojom::ImageAnnotationStatus::kNone:
    case ax::mojom::ImageAnnotationStatus::kWillNotAnnotateDueToScheme:
    case ax::mojom::ImageAnnotationStatus::kIneligibleForAnnotation:
    case ax::mojom::ImageAnnotationStatus::kSilentlyEligibleForAnnotation:
    case ax::mojom::ImageAnnotationStatus::kEligibleForAnnotation:
      break;
  }

  // This is called from IsLeaf, so don't call PlatformChildCount
  // from within this!
  if (text.empty() && ((HasOnlyTextChildren() && !HasListMarkerChild()) ||
                       (IsFocusable() && HasOnlyTextAndImageChildren()) ||
                       GetRole() == ax::mojom::Role::kAlert)) {
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
  if (!(GetRole() == ax::mojom::Role::kHeading && InternalChildCount() == 1)) {
    return false;
  }

  BrowserAccessibilityOHOS* child =
      static_cast<BrowserAccessibilityOHOS*>(InternalChildrenBegin().get());
  return ui::IsLink(child->GetRole());
}

bool BrowserAccessibilityOHOS::IsFocusable() const {
  if (ui::IsIframe(GetRole()) ||
      (ui::IsPlatformDocument(GetRole()) && PlatformGetParent() &&
       PlatformGetParent()->GetRole() != ax::mojom::Role::kPortalDeprecated)) {
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
    if (html_tag != "input") {
      return false;
    }

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
    if (html_tag != "input") {
      return false;
    }

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
    float slider_value =
        GetFloatAttribute(ax::mojom::FloatAttribute::kValueForRange);
    float slider_min =
        GetFloatAttribute(ax::mojom::FloatAttribute::kMinValueForRange);
    float slider_max =
        GetFloatAttribute(ax::mojom::FloatAttribute::kMaxValueForRange);
    if (slider_max <= slider_min) {
      return;
    }
    float slider_step = (slider_max - slider_min) / kDefaultStepTicksForSliders;
    if (HasFloatAttribute(ax::mojom::FloatAttribute::kStepValueForRange)) {
      slider_step =
          GetFloatAttribute(ax::mojom::FloatAttribute::kStepValueForRange);
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
  return false;
}

bool BrowserAccessibilityOHOS::Scroll(ScrollDirection direction,
                                      bool is_page_scroll) const {
  int x_initial = GetIntAttribute(ax::mojom::IntAttribute::kScrollX);
  int x_min = GetIntAttribute(ax::mojom::IntAttribute::kScrollXMin);
  int x_max = GetIntAttribute(ax::mojom::IntAttribute::kScrollXMax);
  int y_initial = GetIntAttribute(ax::mojom::IntAttribute::kScrollY);
  int y_min = GetIntAttribute(ax::mojom::IntAttribute::kScrollYMin);
  int y_max = GetIntAttribute(ax::mojom::IntAttribute::kScrollYMax);

  // Figure out the bounding box of the visible portion of this scrollable
  // view so we know how much to scroll by.
  gfx::Rect bounds = GetClippedRootFrameBoundsRect();

  // Scroll by 50% of one page, or 100% for page scrolls.
  int page_x = 0;
  int page_y = 0;
  if (is_page_scroll) {
    page_x = std::max(bounds.width(), 1);
    page_y = std::max(bounds.height(), 1);
  } else {
    page_x = std::max(bounds.width() / NUMBER_TWO, 1);
    page_y = std::max(bounds.height() / NUMBER_TWO, 1);
  }

  if (direction == ScrollDirection::FORWARD) {
    direction = y_max > y_min ? ScrollDirection::DOWN : ScrollDirection::RIGHT;
  }
  if (direction == ScrollDirection::BACKWARD) {
    direction = y_max > y_min ? ScrollDirection::UP : ScrollDirection::LEFT;
  }

  int x = x_initial;
  int y = y_initial;
  switch (direction) {
    case ScrollDirection::UP:
      if (y_initial == y_min) {
        return false;
      }
      y = std::clamp(y_initial - page_y, y_min, y_max);
      break;
    case ScrollDirection::DOWN:
      if (y_initial == y_max) {
        return false;
      }
      y = std::clamp(y_initial + page_y, y_min, y_max);
      break;
    case ScrollDirection::LEFT:
      if (x_initial == x_min) {
        return false;
      }
      x = std::clamp(x_initial - page_x, x_min, x_max);
      break;
    case ScrollDirection::RIGHT:
      if (x_initial == x_max) {
        return false;
      }
      x = std::clamp(x_initial + page_x, x_min, x_max);
      break;
    default:
      NOTREACHED();
  }

  manager()->SetScrollOffset(*this, gfx::Point(x, y));
  return true;
}

bool BrowserAccessibilityOHOS::IsInterestingOnOHOS() const
{
  // The root is not interesting if it doesn't have a title, even
  // though it's focusable.
  if (ui::IsPlatformDocument(GetRole()) &&
      GetSubstringTextContentUTF16(NonEmptyPredicate()).empty()) {
    return false;
  }

  if (IsInvisibleOrIgnored() || IsChildOfLeaf()) {
    return false;
  }

  // If it has a control role, then it's interesting.
  if (IsControl(GetRole())) {
    return true;
  }

  // Walk up the ancestry. A non-focusable child of a control is not
  // interesting. A child of an invisible iframe is also not interesting.
  // A link is never a leaf node so that its children can be navigated
  // when swiping by heading, landmark, etc. So we will also mark the
  // children of a link as not interesting to prevent double utterances.
  const BrowserAccessibility* parent = PlatformGetParent();
  while (parent) {
    if (IsControl(parent->GetRole()) && !IsFocusable()) {
      return false;
    }

    if (parent->GetRole() == ax::mojom::Role::kIframe &&
        parent->IsInvisibleOrIgnored()) {
      return false;
    }

    if (parent->GetRole() == ax::mojom::Role::kLink) {
      return false;
    }

    parent = parent->PlatformGetParent();
  }

  // Otherwise, focusable nodes are always interesting. Note that IsFocusable()
  // already skips over things like iframes and child frames that are
  // technically focusable but shouldn't be exposed as focusable on OHOS.
  if (IsFocusable()) {
    return true;
  }

  // Mark progress indicators as interesting, since they are not focusable and
  // not a control, but users should be able to swipe/navigate to them.
  if (GetRole() == ax::mojom::Role::kProgressIndicator) {
    return true;
  }

  // If we are the direct descendant of a link and have no siblings/children,
  // then we are not interesting, return false
  parent = PlatformGetParent();
  if (parent != nullptr && ui::IsLink(parent->GetRole()) &&
      parent->PlatformChildCount() == 1 && PlatformChildCount() == 0) {
    return false;
  }

  if (GetRole() == ax::mojom::Role::kEmbeddedObject) {
    return true;
  }
  return IsLeaf() && !base::ContainsOnlyChars(GetTextContentUTF16(),
                                              base::kWhitespaceUTF16);
}

bool BrowserAccessibilityOHOS::IsChildOfLeaf() const
{
  BrowserAccessibility* ancestor = InternalGetParent();

  while (ancestor) {
    if (ancestor->IsLeaf()) {
      return true;
    }
    ancestor = ancestor->InternalGetParent();
  }

  return false;
}

bool BrowserAccessibilityOHOS::IsLeaf() const {
  if (g_leaf_map.Get().find(this) != g_leaf_map.Get().end()) {
    return g_leaf_map.Get()[this];
  }

  if (BrowserAccessibility::IsLeaf()) {
    return true;
  }

  // Document roots (e.g. kRootWebArea and kPdfRoot), and iframes are always
  // allowed to contain children.
  if (ui::IsIframe(GetRole()) || ui::IsPlatformDocument(GetRole())) {
    return false;
  }

  // Button, date and time controls should not expose their children to OHOS
  // accessibility APIs.
  switch (GetRole()) {
    case ax::mojom::Role::kButton:
    case ax::mojom::Role::kDate:
    case ax::mojom::Role::kDateTime:
    case ax::mojom::Role::kInputTime:
      return true;
    default:
      break;
  }

  // Links are never leaves.
  if (ui::IsLink(GetRole())) {
    return false;
  }

  // For some nodes, we will consider children before determining if the node
  // is a leaf. For nodes with relevant children, we will return false here
  // and allow the child nodes to be set as a leaf.

  // Headings with text can drop their children (with exceptions).
  std::u16string name = GetSubstringTextContentUTF16(NonEmptyPredicate());
  if (GetRole() == ax::mojom::Role::kHeading && !name.empty()) {
    bool ret = IsLeafConsideringChildren();
    g_leaf_map.Get()[this] = ret;
    return ret;
  }

  // Focusable nodes with text can drop their children (with exceptions).
  if (HasState(ax::mojom::State::kFocusable) && !name.empty()) {
    bool ret = IsLeafConsideringChildren();
    g_leaf_map.Get()[this] = ret;
    return ret;
  }

  // Nodes with only static text can drop their children, with the exception
  // that list markers have a different role and should not be dropped.
  if (HasOnlyTextChildren() && !HasListMarkerChild()) {
    g_leaf_map.Get()[this] = true;
    return true;
  }

  g_leaf_map.Get()[this] = false;
  return false;
}

// static
void BrowserAccessibilityOHOS::ResetLeafCache()
{
  g_leaf_map.Get().clear();
}

bool BrowserAccessibilityOHOS::IsLeafConsideringChildren() const
{
  // This is called from IsLeaf, so don't call PlatformChildCount
  // from within this!

  // Check for any children that should be exposed and return false if found (by
  // returning false we are saying the parent node is NOT a leaf and this child
  // node should instead be the leaf).
  //
  // If a node has a child that meets any of these criteria, it is NOT a leaf:
  //
  //   * child is focusable, and NOT a menu option
  //   * child is a table, cell, or row
  //
  for (auto it = InternalChildrenBegin(); it != InternalChildrenEnd(); ++it) {
    BrowserAccessibility* child = it.get();

    if (child->HasState(ax::mojom::State::kFocusable) &&
        child->GetRole() != ax::mojom::Role::kMenuListOption) {
      return false;
    }

    if (child->GetRole() == ax::mojom::Role::kTable ||
        child->GetRole() == ax::mojom::Role::kCell ||
        child->GetRole() == ax::mojom::Role::kRow ||
        child->GetRole() == ax::mojom::Role::kLayoutTable ||
        child->GetRole() == ax::mojom::Role::kLayoutTableCell ||
        child->GetRole() == ax::mojom::Role::kLayoutTableRow) {
      return false;
    }

    // Check nested children and return false if any meet above criteria.
    if (!static_cast<BrowserAccessibilityOHOS*>(child)
             ->IsLeafConsideringChildren()) {
      return false;
    }
  }

  // If no such children were found, return true signaling the parent node can
  // be the leaf node.
  return true;
}

const BrowserAccessibilityOHOS* BrowserAccessibilityOHOS::GetSoleInterestingNodeFromSubtree() const
{
  if (IsInterestingOnOHOS()) {
    return this;
  }

  const BrowserAccessibilityOHOS* sole_interesting_node = nullptr;
  for (const auto& child : PlatformChildren()) {
    const BrowserAccessibilityOHOS* interesting_node =
        static_cast<const BrowserAccessibilityOHOS&>(child)
            .GetSoleInterestingNodeFromSubtree();
    if (interesting_node && sole_interesting_node) {
      // If there are two interesting nodes, return nullptr.
      return nullptr;
    } else if (interesting_node) {
      sole_interesting_node = interesting_node;
    }
  }

  return sole_interesting_node;
}

BrowserAccessibilityOHOS::EarlyExitPredicate BrowserAccessibilityOHOS::NonEmptyPredicate()
{
  return base::BindRepeating(
      [](const std::u16string& partial) { return partial.size() > 0; });
}

bool BrowserAccessibilityOHOS::CanFireEvents() const
{
  return !IsChildOfLeaf();
}

void BrowserAccessibilityOHOS::OnLocationChanged()
{
  auto* manager =
      static_cast<BrowserAccessibilityManagerOHOS*>(this->manager());
  manager->FireLocationChanged(this);
}

int32_t BrowserAccessibilityOHOS::GetCheckboxGroupSelectedStatus() const
{
  // To communicate kMixed state Checkboxes, we will rely on state description,
  // so we will not report node as checkable to avoid duplicate utterances.
  if (IsCheckable() && GetRole() == ax::mojom::Role::kMenuItemCheckBox) {
    auto status = GetData().GetCheckedState();
    switch (status) {
      case ax::mojom::CheckedState::kTrue:
        return CHECKBOX_GROUP_STATUS_TRUE;
      case ax::mojom::CheckedState::kMixed:
        return CHECKBOX_GROUP_STATUS_MIXED;
      case ax::mojom::CheckedState::kFalse:
        return CHECKBOX_GROUP_STATUS_FALSE;
      default:
        break;
    }
  }
  return CHECKBOX_GROUP_STATUS_DEFAULT;
}

std::u16string BrowserAccessibilityOHOS::GetLocalizedString(
    int message_id) const {
  return CHECK_DEREF(content::GetContentClient()).GetLocalizedString(message_id);
}

std::u16string BrowserAccessibilityOHOS::GetComboboxCollapsedText() const {
  if (IsCollapsed() && !IsExpanded()) {
    return GetLocalizedString(IDS_AX_COMBOBOX_COLLAPSED);
  }
}

std::u16string BrowserAccessibilityOHOS::GetComboboxExpandedText() const {
  const BrowserAccessibilityOHOS* input_node = nullptr;
  for (const auto& child : PlatformChildren()) {
    const BrowserAccessibilityOHOS& ohos_child =
        static_cast<const BrowserAccessibilityOHOS&>(child);
    if (ohos_child.IsTextField()) {
      input_node = &ohos_child;
      break;
    }
  }

  // If we have not found a child input element, consider aria 1.0 spec:
  //
  // <input type="text" role="combobox" aria-owns="options">
  // <ul role="listbox" id="options">...</ul>
  //
  // Check if |this| is the input, otherwise try our fallbacks.
  if (!input_node) {
    if (IsTextField()) {
      input_node = this;
    } else {
      return GetComboboxExpandedTextFallback();
    }
  }

  // Get the aria-controls nodes of |input_node|.
  std::vector<BrowserAccessibility*> controls =
      manager()->GetAriaControls(input_node);
  // |input_node| should control only one element, if it doesn't, try fallbacks.
  if (controls.size() != 1) {
    return GetComboboxExpandedTextFallback();
  }

  // |controlled_node| needs to be a combobox container, if not, try fallbacks.
  BrowserAccessibilityOHOS* controlled_node =
      static_cast<BrowserAccessibilityOHOS*>(controls[0]);
  if (!ui::IsComboBoxContainer(controlled_node->GetRole())) {
    return GetComboboxExpandedTextFallback();
  }

  // For dialogs, return special case string.
  if (controlled_node->GetRole() == ax::mojom::Role::kDialog) {
    return GetLocalizedString(IDS_AX_COMBOBOX_EXPANDED_DIALOG);
  }

  // Find |controlled_node| set size, or return default string.
  if (!controlled_node->GetSetSize()) {
    return GetLocalizedString(IDS_AX_COMBOBOX_EXPANDED_AUTOCOMPLETE_DEFAULT);
  }

  // Replace placeholder with count and return string.
  return base::ReplaceStringPlaceholders(
      GetLocalizedString(
          IDS_AX_COMBOBOX_EXPANDED_AUTOCOMPLETE_X_OPTIONS_AVAILABLE),
      base::NumberToString16(*controlled_node->GetSetSize()), nullptr);
}

std::u16string BrowserAccessibilityOHOS::GetComboboxExpandedTextFallback() const {
  // If a combobox was of an indeterminate form, attempt any special cases here,
  // or return "expanded" as a final option.

  // Check for child nodes that are collections.
  int child_collection_count = 0;
  const BrowserAccessibilityOHOS* collection_node = nullptr;
  for (const auto& child : PlatformChildren()) {
    const auto& ohos_child =
        static_cast<const BrowserAccessibilityOHOS&>(child);
    if (ohos_child.IsCollection()) {
      child_collection_count++;
      collection_node = &ohos_child;
    }
  }

  // If we find none, or more than one, we will not be able to determine the
  // correct utterance, so return a default string instead.
  if (child_collection_count != 1) {
    return GetLocalizedString(IDS_AX_COMBOBOX_EXPANDED);
  }

  // Find |collection_node| set size, or return defaul string.
  if (!collection_node->GetSetSize()) {
    return GetLocalizedString(IDS_AX_COMBOBOX_EXPANDED_AUTOCOMPLETE_DEFAULT);
  }

  // Replace placeholder with count and return string.
  return base::ReplaceStringPlaceholders(
      GetLocalizedString(
          IDS_AX_COMBOBOX_EXPANDED_AUTOCOMPLETE_X_OPTIONS_AVAILABLE),
      base::NumberToString16(*collection_node->GetSetSize()), nullptr);
}

std::u16string BrowserAccessibilityOHOS::GetRoleDescription() const {
  // If an element has an aria-roledescription set, use that value by default.
  if (HasStringAttribute(ax::mojom::StringAttribute::kRoleDescription)) {
    return GetString16Attribute(ax::mojom::StringAttribute::kRoleDescription);
  }

  if (GetRole() == ax::mojom::Role::kHeading) {
    std::vector<std::u16string> role_description;
    role_description.push_back(GetLocalizedString(IDS_AX_ROLE_HEADING));

    // For visited links, we additionally want to append "visited" to the
    // description.
    if (HasState(ax::mojom::State::kVisited)) {
      role_description.push_back(GetLocalizedString(IDS_AX_STATE_LINK_VISITED));
    }

    return base::JoinString(role_description, u" ");
  }

  // If this node is a link and the parent is a heading, return the role
  // description of the parent (e.g. "heading 1 link").
  if (ui::IsLink(GetRole()) && PlatformGetParent()) {
    BrowserAccessibilityOHOS* parent =
      static_cast<BrowserAccessibilityOHOS*>(PlatformGetParent());
    if (parent->IsHeadingLink()) {
      return parent->GetRoleDescription();
    }
  }

  // If this node is a link and visited, append "visited" to the description.
  if (ui::IsLink(GetRole())) {
    std::vector<std::u16string> role_description = {
        GetLocalizedStringForRoleDescription()};
    if (HasState(ax::mojom::State::kVisited)) {
      role_description.push_back(GetLocalizedString(IDS_AX_STATE_LINK_VISITED));
    }
    return base::JoinString(role_description, u" ");
  }

  // For buttons with a kHasPopup attribute, return a more specific role.
  if (ui::IsButton(GetRole())) {
    switch (static_cast<ax::mojom::HasPopup>(
        GetIntAttribute(ax::mojom::IntAttribute::kHasPopup))) {
      case ax::mojom::HasPopup::kTrue:
      case ax::mojom::HasPopup::kMenu:
        return GetLocalizedString(IDS_AX_ROLE_POP_UP_BUTTON_MENU);
      case ax::mojom::HasPopup::kDialog:
        return GetLocalizedString(IDS_AX_ROLE_POP_UP_BUTTON_DIALOG);
      case ax::mojom::HasPopup::kListbox:
      case ax::mojom::HasPopup::kTree:
      case ax::mojom::HasPopup::kGrid:
        return GetLocalizedString(IDS_AX_ROLE_POP_UP_BUTTON);
      case ax::mojom::HasPopup::kFalse:
        break;
    }
  }

  switch (GetRole()) {
    case ax::mojom::Role::kAlert:
    case ax::mojom::Role::kAudio:
    case ax::mojom::Role::kCode:
    case ax::mojom::Role::kDetails:
    case ax::mojom::Role::kEmphasis:
    case ax::mojom::Role::kForm:
    case ax::mojom::Role::kRowGroup:
    case ax::mojom::Role::kSectionFooter:
    case ax::mojom::Role::kSectionHeader:
    case ax::mojom::Role::kSectionWithoutName:
    case ax::mojom::Role::kStrong:
    case ax::mojom::Role::kSubscript:
    case ax::mojom::Role::kSuperscript:
    case ax::mojom::Role::kTime:
      // No role description on OHOS.
      break;
    case ax::mojom::Role::kCanvas:
      return GetLocalizedString(IDS_AX_ROLL_CANVAS_OHOS);
    case ax::mojom::Role::kComboBoxMenuButton:
    case ax::mojom::Role::kComboBoxSelect:
      return GetLocalizedString(IDS_AX_ROLE_COMBO_BOX);
    case ax::mojom::Role::kList:
      return GetLocalizedString(IDS_AX_ROLE_LIST);
    case ax::mojom::Role::kDescriptionList:
      return GetLocalizedString(IDS_AX_ROLE_DESCRIPTION_LIST);
    case ax::mojom::Role::kFigure:
      // Default is IDS_AX_ROLE_FIGURE.
      return GetLocalizedString(IDS_AX_ROLE_GRAPHIC);
    case ax::mojom::Role::kHeader:
      // Default is IDS_AX_ROLE_HEADER.
      return GetLocalizedString(IDS_AX_ROLE_BANNER);
    case ax::mojom::Role::kListGrid:
      // Default is no special role description.
      return GetLocalizedString(IDS_AX_ROLE_TABLE);
    case ax::mojom::Role::kMarquee:
      return GetLocalizedString(IDS_AX_ROLE_MARQUEE);
    case ax::mojom::Role::kMenuItemCheckBox:
      // Default is no special role description.
      return GetLocalizedString(IDS_AX_ROLE_CHECK_BOX);
    case ax::mojom::Role::kMenuItemRadio:
    case ax::mojom::Role::kRadioButton:
      // Default is no special role description.
      return GetLocalizedString(IDS_AX_ROLE_RADIO);
    case ax::mojom::Role::kTextField:
    case ax::mojom::Role::kTextFieldWithComboBox:
      return GetLocalizedString(IDS_AX_ROLE_TEXT_FIELD);
    case ax::mojom::Role::kVideo:
      // Default is no special role description.
      return GetLocalizedString(IDS_AX_MEDIA_VIDEO_ELEMENT);
    default:
      return GetLocalizedStringForRoleDescription();
  }
  return std::u16string();
}

}  // namespace content
