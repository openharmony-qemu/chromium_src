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

#include "base/logging.h"
#include "content/browser/accessibility/browser_accessibility_manager_ohos.h"
#include "content/browser/accessibility/browser_accessibility_ohos.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/public/browser/browser_thread.h"
#include "ui/accessibility/ax_selection.h"
#include "ui/gfx/geometry/point_conversions.h"

namespace content {
const int64_t kInvalidAccessibilityId = -1;
const int64_t kArkWebId = 0;
constexpr int32_t kMaxContentChangedEventsToFire = 5;
constexpr int32_t kAccessibilityEventDelayDefault = 100;
constexpr int32_t kAccessibilityEventDelayHover = 200;
constexpr int32_t kMaxLocationChangedEventsToFire = 3;

using SearchKeyToPredicateMap =
    std::unordered_map<std::u16string, AccessibilityMatchPredicate>;
base::LazyInstance<SearchKeyToPredicateMap>::Leaky
    g_search_key_to_predicate_map = LAZY_INSTANCE_INITIALIZER;
base::LazyInstance<std::u16string>::Leaky g_all_search_keys =
    LAZY_INSTANCE_INITIALIZER;

BrowserAccessibilityManager* BrowserAccessibilityManager::Create(
    const ui::AXTreeUpdate& initial_tree,
    WebAXPlatformTreeManagerDelegate* delegate) {
  return new BrowserAccessibilityManagerOHOS(initial_tree, delegate);
}

// static
BrowserAccessibilityManager* BrowserAccessibilityManager::Create(
    WebAXPlatformTreeManagerDelegate* delegate) {
  return new BrowserAccessibilityManagerOHOS(
      BrowserAccessibilityManager::GetEmptyDocument(), delegate);
}

BrowserAccessibilityManagerOHOS::BrowserAccessibilityManagerOHOS(
    const ui::AXTreeUpdate& initial_tree,
    WebAXPlatformTreeManagerDelegate* delegate)
    : BrowserAccessibilityManager(delegate) {
  Initialize(initial_tree);
  InitializeAccessibilityEventDispatcher();
}

void BrowserAccessibilityManagerOHOS::HandleFocusChanged(
    int64_t accessibilityId) {
  if (accessibilityId == GetRootAccessibilityId()) {
    return;
  }
  SendAccessibilityEvent(accessibilityId,
                         OHOS::NWeb::AccessibilityEventType::FOCUS);
  if (accessibilityId != accessibilityFocusId_) {
    SendAccessibilityEvent(accessibilityId,
                           OHOS::NWeb::AccessibilityEventType::REQUEST_FOCUS);
  }
}

void BrowserAccessibilityManagerOHOS::FireFocusEvent(
    ui::AXNode* node) {
  ui::AXTreeManager::FireFocusEvent(node);
  BrowserAccessibilityOHOS* nodeOHOS =
      static_cast<BrowserAccessibilityOHOS*>(GetFromAXNode(node));
  if (!nodeOHOS) {
    return;
  }
  HandleFocusChanged(nodeOHOS->GetAccessibilityId()); 
}

void BrowserAccessibilityManagerOHOS::FireBlinkEvent(
    ax::mojom::Event event_type,
    BrowserAccessibility* node,
    int action_request_id) {
  BrowserAccessibilityOHOS* nodeOHOS =
      static_cast<BrowserAccessibilityOHOS*>(node);
  if (!nodeOHOS) {
    return;
  }
  int64_t accessibilityId = nodeOHOS->GetAccessibilityId();
  switch (event_type) {
    case ax::mojom::Event::kClicked:
      SendAccessibilityEvent(accessibilityId,
                             OHOS::NWeb::AccessibilityEventType::CLICK);
      break;
    case ax::mojom::Event::kBlur:
      SendAccessibilityEvent(accessibilityId,
                             OHOS::NWeb::AccessibilityEventType::BLUR);
      break;
    case ax::mojom::Event::kHover:
      HandleHover(accessibilityId);
      break;
    default:
      break;
  }
}

bool BrowserAccessibilityManagerOHOS::MoveAccessibilityFocusToId(
    int64_t newAccessibilityFocusId) {
  if (accessibilityFocusId_ == newAccessibilityFocusId) {
    return false;
  }
  MoveAccessibilityFocus(accessibilityFocusId_, newAccessibilityFocusId);
  SendAccessibilityEvent(
      accessibilityFocusId_,
      OHOS::NWeb::AccessibilityEventType::ACCESSIBILITY_FOCUS_CLEARED);
  accessibilityFocusId_ = newAccessibilityFocusId;
  SendAccessibilityEvent(
      accessibilityFocusId_,
      OHOS::NWeb::AccessibilityEventType::ACCESSIBILITY_FOCUSED);
  return true;
}

void BrowserAccessibilityManagerOHOS::MoveAccessibilityFocus(
    int64_t oldId,
    int64_t newId) const {
  auto oldNode = BrowserAccessibilityOHOS::GetFromAccessibilityId(oldId);
  if (oldNode && oldNode->manager())
    oldNode->manager()->ClearAccessibilityFocus(*oldNode);

  auto node = BrowserAccessibilityOHOS::GetFromAccessibilityId(newId);
  if (!node)
    return;
  node->manager()->SetAccessibilityFocus(*node);

  if (node != node->manager()->GetBrowserAccessibilityRoot())
    node->manager()->LoadInlineTextBoxes(*node);
}

void BrowserAccessibilityManagerOHOS::SendAccessibilityEvent(
    int64_t accessibilityId,
    OHOS::NWeb::AccessibilityEventType eventType) {
  if (accessibilityId == 0) {
    DispatchEvent(0, static_cast<int32_t>(eventType));
  } else if (eventDispatcher_ != nullptr) {
    eventDispatcher_->EnqueueEvent(accessibilityId,
                                   static_cast<int32_t>(eventType));
  }
}

bool BrowserAccessibilityManagerOHOS::DispatchEvent(int64_t accessibilityId,
                                                    int32_t eventType) {
  LOG(INFO) << "DispatchEvent accessibilityId is " << accessibilityId
            << ", eventType is " << static_cast<uint32_t>(eventType);

  if (accessibilityId != kInvalidAccessibilityId && delegate_ != nullptr) {
    auto renderFrameHost = delegate_->AccessibilityRenderFrameHost();
    if (renderFrameHost != nullptr) {
      renderFrameHost->SendAccessibilityEvent(accessibilityId, static_cast<int32_t>(eventType));
      return true;
    }
  }
  return false;
}

void BrowserAccessibilityManagerOHOS::HandleHover(int64_t accessibilityId) {
  SendAccessibilityEvent(accessibilityId,
                         OHOS::NWeb::AccessibilityEventType::HOVER_ENTER_EVENT);
}

void BrowserAccessibilityManagerOHOS::HandleEditableTextChanged(int64_t accessibilityId)
{
  SendAccessibilityEvent(accessibilityId, OHOS::NWeb::AccessibilityEventType::TEXT_CHANGE);
}

void BrowserAccessibilityManagerOHOS::HandleContentChanged(int64_t accessibilityId)
{
  // If there are a large number of changes it's too expensive to fire all of
  // them, so we just fire one on the root instead.
  content_changed_events_++;
  if (content_changed_events_ < kMaxContentChangedEventsToFire) {
    // If it's less than the max event count, fire the event on the specific
    // node that changed.
    SendAccessibilityEvent(accessibilityId,
                           OHOS::NWeb::AccessibilityEventType::CHANGE);
  } else if (content_changed_events_ == kMaxContentChangedEventsToFire) {
    // If it's equal to the max event count, fire the event on the
    // root instead.
    SendAccessibilityEvent(GetRootAccessibilityId(),
                           OHOS::NWeb::AccessibilityEventType::CHANGE);
  }
}

void BrowserAccessibilityManagerOHOS::SendDelayedWindowContentChangedEvent() {
  SendAccessibilityEvent(kArkWebId, OHOS::NWeb::AccessibilityEventType::CHANGE);
}

void BrowserAccessibilityManagerOHOS::
    MoveAccessibilityFocusToIdAndRefocusIfNeeded(
        int64_t newAccessibilityFocusId) {
  if (newAccessibilityFocusId == accessibilityFocusId_) {
    SendAccessibilityEvent(
        newAccessibilityFocusId,
        OHOS::NWeb::AccessibilityEventType::ACCESSIBILITY_FOCUS_CLEARED);
    accessibilityFocusId_ = GetRootAccessibilityId();
  }
  MoveAccessibilityFocusToId(newAccessibilityFocusId);
}

void BrowserAccessibilityManagerOHOS::OnHoverEvent(const gfx::PointF& point) {
  auto* hoverNode = static_cast<BrowserAccessibilityOHOS*>(
      ApproximateHitTest(gfx::ToFlooredPoint(point)));

  if (hoverNode) {
    HandleHover(hoverNode->GetAccessibilityId());
  } else {
    HandleHover(GetRootAccessibilityId());
  }
}

int64_t BrowserAccessibilityManagerOHOS::GetRootAccessibilityId() const {
  auto rootManager = GetManagerForRootFrame();
  if (rootManager) {
    auto root = static_cast<BrowserAccessibilityOHOS*>(
        rootManager->GetBrowserAccessibilityRoot());
    if (root) {
      return root->GetAccessibilityId();
    }
  }
  return kInvalidAccessibilityId;
}

void BrowserAccessibilityManagerOHOS::FireGeneratedEvent(
    ui::AXEventGenerator::Event event_type,
    const ui::AXNode* node) {
  ui::AXTreeManager::FireGeneratedEvent(event_type, node);

  BrowserAccessibility* wrapper = GetFromAXNode(node);
  BrowserAccessibilityOHOS* nodeOHOS =
      static_cast<BrowserAccessibilityOHOS*>(wrapper);
  if (!nodeOHOS) {
    return;
  }

  int64_t accessibilityId = nodeOHOS->GetAccessibilityId();

  if (event_type == ui::AXEventGenerator::Event::CHILDREN_CHANGED) {
    BrowserAccessibilityOHOS::ResetLeafCache();
  }

  if (event_type != ui::AXEventGenerator::Event::SUBTREE_CREATED) {
    HandleContentChanged(accessibilityId);
  }
  switch (event_type) {
    case ui::AXEventGenerator::Event::VALUE_IN_TEXT_FIELD_CHANGED:
      if (nodeOHOS->IsTextField() && GetFocus() == wrapper) {
        HandleEditableTextChanged(accessibilityId);
      }
      break;
    case ui::AXEventGenerator::Event::NAME_CHANGED:
      SendAccessibilityEvent(accessibilityId, OHOS::NWeb::AccessibilityEventType::TEXT_CHANGE);
      break;
    case ui::AXEventGenerator::Event::SCROLL_HORIZONTAL_POSITION_CHANGED:
    case ui::AXEventGenerator::Event::SCROLL_VERTICAL_POSITION_CHANGED:
      HandleScrollPositionChanged(accessibilityId);
      break;
    case ui::AXEventGenerator::Event::SELECTED_CHANGED:
      if (nodeOHOS->IsSelected()) {
        SendAccessibilityEvent(accessibilityId, OHOS::NWeb::AccessibilityEventType::SELECTED);
        if (accessibilityId != accessibilityFocusId_) {
          SendAccessibilityEvent(accessibilityId, OHOS::NWeb::AccessibilityEventType::REQUEST_FOCUS);
        }
      }
      break;
    case ui::AXEventGenerator::Event::DOCUMENT_SELECTION_CHANGED: {
      if (ax_tree() == nullptr) {
        break;
      }
      ui::AXNodeID focus_id =
          ax_tree()->GetUnignoredSelection().focus_object_id;
      BrowserAccessibility* focus_object = GetFromID(focus_id);
      if (focus_object) {
        BrowserAccessibilityOHOS* oh_focus_object =
            static_cast<BrowserAccessibilityOHOS*>(focus_object);
        if (oh_focus_object == nullptr) {
          break;
        }
        SendAccessibilityEvent(oh_focus_object->GetAccessibilityId(),
            OHOS::NWeb::AccessibilityEventType::TEXT_SELECTION_UPDATE);
      }
      break;
    }
    case ui::AXEventGenerator::Event::LIVE_REGION_NODE_CHANGED: {
      SendAccessibilityEvent(accessibilityId, OHOS::NWeb::AccessibilityEventType::SELECTED);
      break;
    }
    case ui::AXEventGenerator::Event::SUBTREE_CREATED:
      if (nodeOHOS->GetRole() == ax::mojom::Role::kDialog) {
        HandleDialogModalOpened(accessibilityId);
      }
      break;
    default:
      break;
  }
}

void BrowserAccessibilityManagerOHOS::Copy()
{
  content::WebContentsImpl* web_contents_impl = 
      static_cast<content::WebContentsImpl*>(web_contents());
  if (web_contents_impl == nullptr) {
    return;
  }
  web_contents_impl->Copy();
}

void BrowserAccessibilityManagerOHOS::Paste()
{
  content::WebContentsImpl* web_contents_impl = 
      static_cast<content::WebContentsImpl*>(web_contents());
  if (web_contents_impl == nullptr) {
    return;
  }
  web_contents_impl->Paste();
}

void BrowserAccessibilityManagerOHOS::Cut()
{
  content::WebContentsImpl* web_contents_impl = 
      static_cast<content::WebContentsImpl*>(web_contents());
  if (web_contents_impl == nullptr) {
    return;
  }
  web_contents_impl->Cut();
}

bool BrowserAccessibilityManagerOHOS::JumpToElementType(
    int64_t accessibility_id, const std::string& element_type, bool forwards, bool can_wrap) {
  std::string upper_element_type;
  std::transform(element_type.begin(), element_type.end(), upper_element_type.begin(), [](char c) {
      return std::toupper(c);
  });

  int64_t id = FindElementType(
      accessibility_id, upper_element_type, forwards, can_wrap, upper_element_type == "");
  if (id == 0) {
    return false;
  }
  SendAccessibilityEvent(id, OHOS::NWeb::AccessibilityEventType::REQUEST_FOCUS);
  return true;
}

bool BrowserAccessibilityManagerOHOS::AllInterestingNodesPredicate(BrowserAccessibility* start,
    BrowserAccessibility* node) {
  BrowserAccessibilityOHOS* ohos_node =
      static_cast<BrowserAccessibilityOHOS*>(node);
  return ohos_node->IsInterestingOnOHOS();
}

int64_t BrowserAccessibilityManagerOHOS::FindElementType(
    int64_t start_id, const std::string& element_type, bool forwards,
    bool can_wrap, bool use_default_predicate) {
  BrowserAccessibilityOHOS* start_node = BrowserAccessibilityOHOS::GetFromAccessibilityId(start_id);
  if (!start_node) {
    return 0;
  }

  BrowserAccessibility* root = GetBrowserAccessibilityRoot();
  if (!root) {
    return 0;
  }

  // If |element_type| was empty, we can skip to the default predicate.
  AccessibilityMatchPredicate predicate;
  if (use_default_predicate) {
    predicate = AllInterestingNodesPredicate;
  } else {
    std::u16string search_key = base::ASCIIToUTF16(element_type.c_str());
    predicate = PredicateForSearchKey(search_key);
  }

  OneShotAccessibilityTreeSearch tree_search(root);
  tree_search.SetStartNode(start_node);
  tree_search.SetDirection(forwards
                               ? OneShotAccessibilityTreeSearch::FORWARDS
                               : OneShotAccessibilityTreeSearch::BACKWARDS);
  tree_search.SetResultLimit(1);
  tree_search.SetImmediateDescendantsOnly(false);
  tree_search.SetCanWrapToLastElement(can_wrap);
  tree_search.SetOnscreenOnly(true);
  tree_search.AddPredicate(predicate);

  if (tree_search.CountMatches() == 0) {
    return 0;
  }

  auto* ohos_node =
      static_cast<BrowserAccessibilityOHOS*>(tree_search.GetMatchAtIndex(0));
  if (ohos_node != nullptr) {
    return ohos_node->GetAccessibilityId();
  }
  return 0;
}

void BrowserAccessibilityManagerOHOS::AddToPredicateMap(const char* search_key_ascii,
    AccessibilityMatchPredicate predicate) {
  std::u16string search_key_utf16 = base::ASCIIToUTF16(search_key_ascii);
  g_search_key_to_predicate_map.Get()[search_key_utf16] = predicate;
  if (!g_all_search_keys.Get().empty())
    g_all_search_keys.Get() += u",";
  g_all_search_keys.Get() += search_key_utf16;
}

// These are special unofficial strings sent from TalkBack/BrailleBack
// to jump to certain categories of web elements.
void BrowserAccessibilityManagerOHOS::InitSearchKeyToPredicateMapIfNeeded() {
  if (!g_search_key_to_predicate_map.Get().empty())
    return;

  AddToPredicateMap("ARTICLE", AccessibilityArticlePredicate);
  AddToPredicateMap("BLOCKQUOTE", AccessibilityBlockquotePredicate);
  AddToPredicateMap("BUTTON", AccessibilityButtonPredicate);
  AddToPredicateMap("CHECKBOX", AccessibilityCheckboxPredicate);
  AddToPredicateMap("COMBOBOX", AccessibilityComboboxPredicate);
  AddToPredicateMap("CONTROL", AccessibilityControlPredicate);
  AddToPredicateMap("FOCUSABLE", AccessibilityFocusablePredicate);
  AddToPredicateMap("FRAME", AccessibilityFramePredicate);
  AddToPredicateMap("GRAPHIC", AccessibilityGraphicPredicate);
  AddToPredicateMap("H1", AccessibilityH1Predicate);
  AddToPredicateMap("H2", AccessibilityH2Predicate);
  AddToPredicateMap("H3", AccessibilityH3Predicate);
  AddToPredicateMap("H4", AccessibilityH4Predicate);
  AddToPredicateMap("H5", AccessibilityH5Predicate);
  AddToPredicateMap("H6", AccessibilityH6Predicate);
  AddToPredicateMap("HEADING", AccessibilityHeadingPredicate);
  AddToPredicateMap("HEADING_SAME", AccessibilityHeadingSameLevelPredicate);
  AddToPredicateMap("LANDMARK", AccessibilityLandmarkPredicate);
  AddToPredicateMap("LINK", AccessibilityLinkPredicate);
  AddToPredicateMap("LIST", AccessibilityListPredicate);
  AddToPredicateMap("LIST_ITEM", AccessibilityListItemPredicate);
  AddToPredicateMap("LIVE", AccessibilityLiveRegionPredicate);
  AddToPredicateMap("MAIN", AccessibilityMainPredicate);
  AddToPredicateMap("MEDIA", AccessibilityMediaPredicate);
  AddToPredicateMap("PARAGRAPH", AccessibilityParagraphPredicate);
  AddToPredicateMap("RADIO", AccessibilityRadioButtonPredicate);
  AddToPredicateMap("RADIO_GROUP", AccessibilityRadioGroupPredicate);
  AddToPredicateMap("SECTION", AccessibilitySectionPredicate);
  AddToPredicateMap("TABLE", AccessibilityTablePredicate);
  AddToPredicateMap("TEXT_FIELD", AccessibilityTextfieldPredicate);
  AddToPredicateMap("TEXT_BOLD", AccessibilityTextStyleBoldPredicate);
  AddToPredicateMap("TEXT_ITALIC", AccessibilityTextStyleItalicPredicate);
  AddToPredicateMap("TEXT_UNDERLINE", AccessibilityTextStyleUnderlinePredicate);
  AddToPredicateMap("TREE", AccessibilityTreePredicate);
  AddToPredicateMap("UNVISITED_LINK", AccessibilityUnvisitedLinkPredicate);
  AddToPredicateMap("VISITED_LINK", AccessibilityVisitedLinkPredicate);
}

AccessibilityMatchPredicate
BrowserAccessibilityManagerOHOS::PredicateForSearchKey(
    const std::u16string& element_type) {
  InitSearchKeyToPredicateMapIfNeeded();
  const auto& iter = g_search_key_to_predicate_map.Get().find(element_type);
  if (iter != g_search_key_to_predicate_map.Get().end()) {
    return iter->second;
  }

  // If we don't recognize the selector, return any element that a
  // screen reader should navigate to.
  return AllInterestingNodesPredicate;
}

void BrowserAccessibilityManagerOHOS::ScrollToMakeNodeVisible(
    int64_t accessibility_id) {
  BrowserAccessibilityOHOS* node =
      BrowserAccessibilityOHOS::GetFromAccessibilityId(accessibility_id);
  if (node) {
    node->manager()->ScrollToMakeVisible(
        *node, gfx::Rect(node->GetUnclippedFrameBoundsRect().size()));
  }
}

ui::AXNode* BrowserAccessibilityManagerOHOS::RetargetForEvents(
    ui::AXNode* node,
    RetargetEventType type) const {
  // TODO(crbug.com/1350627): Node should not be null. But this seems to be
  // happening in the wild for reasons not yet determined. Because the only
  // consequence of node being null is that we'll fail to fire an event on a
  // non-existent object, the style guide's suggestion of using a CHECK
  // temporarily seems a bit strong. Nonetheless we should get to the bottom of
  // this. So we are temporarily using NOTREACHED in the hopes that ClusterFuzz
  // will lead to a reliably-reproducible test case.
  if (!node) {
    NOTREACHED();
    return nullptr;
  }

  // Sometimes we get events on nodes in our internal accessibility tree
  // that aren't exposed on OHOS. Get |updated| to point to the lowest
  // ancestor that is exposed.
  BrowserAccessibility* wrapper = GetFromAXNode(node);
  BrowserAccessibility* updated = wrapper->PlatformGetLowestPlatformAncestor();
  DCHECK(updated);

  switch (type) {
    case RetargetEventType::RetargetEventTypeGenerated: {
      // If the closest platform object is a password field, the event we're
      // getting is doing something in the shadow dom, for example replacing a
      // character with a dot after a short pause. On OHOS we don't want to
      // fire an event for those changes, but we do want to make sure our
      // internal state is correct, so we call OnDataChanged() and then return.
      if (updated->IsPasswordField() && wrapper != updated) {
        updated->OnDataChanged();
        return nullptr;
      }
      break;
    }
    case RetargetEventType::RetargetEventTypeBlinkGeneral:
      break;
    case RetargetEventType::RetargetEventTypeBlinkHover: {
      // If this node is uninteresting and just a wrapper around a sole
      // interesting descendant, prefer that descendant instead.
      const BrowserAccessibilityOHOS* ohos_node =
          static_cast<BrowserAccessibilityOHOS*>(updated);
      const BrowserAccessibilityOHOS* sole_interesting_node =
          ohos_node->GetSoleInterestingNodeFromSubtree();
      if (sole_interesting_node) {
        ohos_node = sole_interesting_node;
      }

      // Finally, if this node is still uninteresting, try to walk up to
      // find an interesting parent.
      while (ohos_node && !ohos_node->IsInterestingOnOHOS()) {
        ohos_node = static_cast<BrowserAccessibilityOHOS*>(
            ohos_node->PlatformGetParent());
      }
      updated = const_cast<BrowserAccessibilityOHOS*>(ohos_node);
      break;
    }
    default:
      NOTREACHED();
      break;
  }
  return updated ? updated->node() : nullptr;
}

void BrowserAccessibilityManagerOHOS::FireLocationChanged(
    BrowserAccessibility* node) {
  BrowserAccessibilityOHOS* ohos_node =
      static_cast<BrowserAccessibilityOHOS*>(node);
  HandleContentChanged(ohos_node->GetAccessibilityId());
}

void BrowserAccessibilityManagerOHOS::SendLocationChangeEvents(
    const std::vector<blink::mojom::LocationChangesPtr>& changes) {
  if (changes.size() > kMaxLocationChangedEventsToFire) {
    SendDelayedWindowContentChangedEvent();
    return;
  }
  BrowserAccessibilityManager::SendLocationChangeEvents(changes);
}

void BrowserAccessibilityManagerOHOS::HandleScrollPositionChanged(
    int64_t accessibilityId) {
  SendAccessibilityEvent(accessibilityId,
                         OHOS::NWeb::AccessibilityEventType::SCROLL_END);
}

void BrowserAccessibilityManagerOHOS::HandleDialogModalOpened(
    int64_t accessibilityId) {
  SendAccessibilityEvent(accessibilityId,
                         OHOS::NWeb::AccessibilityEventType::PAGE_CHANGE);
}

void BrowserAccessibilityManagerOHOS::OnAtomicUpdateFinished(
    ui::AXTree* tree,
    bool root_changed,
    const std::vector<ui::AXTreeObserver::Change>& changes) {
  BrowserAccessibilityManager::OnAtomicUpdateFinished(tree, root_changed,
                                                      changes);
  content_changed_events_ = 0;

  if (root_changed) {
    HandleNavigate(GetRootAccessibilityId());
  }
}

void BrowserAccessibilityManagerOHOS::HandleNavigate(int64_t newRootId) {
  if (newRootId != kInvalidAccessibilityId) {
    accessibilityFocusId_ = kInvalidAccessibilityId;
  }
}

void BrowserAccessibilityManagerOHOS::InitializeAccessibilityEventDispatcher() {
  std::unordered_map<int32_t, int32_t> eventThrottleDelays;
  eventThrottleDelays.insert(std::make_pair(
      static_cast<int32_t>(OHOS::NWeb::AccessibilityEventType::SCROLL_END),
      kAccessibilityEventDelayDefault));
  eventThrottleDelays.insert(std::make_pair(
      static_cast<int32_t>(OHOS::NWeb::AccessibilityEventType::CHANGE),
      kAccessibilityEventDelayDefault));
  eventThrottleDelays.insert(
      std::make_pair(static_cast<int32_t>(
                         OHOS::NWeb::AccessibilityEventType::HOVER_ENTER_EVENT),
                     kAccessibilityEventDelayHover));

  std::unordered_set<int32_t> viewIndependentEvents;
  viewIndependentEvents.insert(static_cast<int32_t>(
      OHOS::NWeb::AccessibilityEventType::HOVER_ENTER_EVENT));

  eventDispatcher_ = std::make_unique<AccessibilityEventDispatcher>(
      eventThrottleDelays, viewIndependentEvents, this);
}

void AccessibilityEventDispatcher::EnqueueEvent(int64_t accessibilityId,
                                                int32_t eventType) {
  // Check whether this type of event is one we want to throttle, and if not
  // then send it
  if (!manager_) {
    return;
  }
  if (eventThrottleDelays_.find(eventType) == eventThrottleDelays_.end()) {
    manager_->DispatchEvent(accessibilityId, eventType);
    return;
  }

  auto millis = std::chrono::time_point_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now());
  auto now = millis.time_since_epoch().count();
  int64_t uuid = Uuid(accessibilityId, eventType);

  if (eventLastFiredTimes_.find(uuid) == eventLastFiredTimes_.end() ||
      now - eventLastFiredTimes_[uuid] >= eventThrottleDelays_[eventType]) {
    // Attempt to dispatch an event, can fail and return false if node is
    // invalid etc.
    if (manager_->DispatchEvent(accessibilityId, eventType)) {
      // Record time of last fired event if the dispatch was successful.
      eventLastFiredTimes_[uuid] = now;
    }

    // Remove any lingering callbacks and pending events regardless of success.
    pendingEvents_.erase(uuid);
  } else {
    if (eventType ==
        static_cast<int32_t>(
            OHOS::NWeb::AccessibilityEventType::HOVER_ENTER_EVENT)) {
      eventLastFiredTimes_[uuid] = now;
      return;
    }
    // We have fired an event of |eventType| for this |virtualViewId| within our
    // |mEventThrottleDelays| delay window. Store this event, replacing any
    // events in |mPendingEvents| of the same |uuid|, and set a delay equal.
    if (pendingEvents_.find(uuid) != pendingEvents_.end() &&
        pendingEvents_[uuid].IsValid()) {
      pendingEvents_[uuid].CancelTask();
    }

    auto task = content::GetUIThreadTaskRunner({})->PostCancelableDelayedTask(
        base::subtle::PostDelayedTaskPassKeyForTesting(), FROM_HERE,
        base::BindOnce(&AccessibilityEventDispatcher::RunTask,
                       base::Unretained(this), accessibilityId, eventType,
                       uuid),
        base::Milliseconds(eventLastFiredTimes_[uuid] +
                           eventThrottleDelays_[eventType] - now));
    pendingEvents_[uuid] = std::move(task);
  }
}

void AccessibilityEventDispatcher::RunTask(int64_t accessibilityId,
                                           int32_t eventType,
                                           int64_t uuid) {
  // We have delayed firing this event, so accessibility may not be enabled or
  // the node may be invalid, in which case dispatch will return false.
  if (manager_ && manager_->DispatchEvent(accessibilityId, eventType)) {
    // After sending event, record time it was sent
    auto millis = std::chrono::time_point_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now());
    auto now = millis.time_since_epoch().count();
    eventLastFiredTimes_[uuid] = now;
  }

  // Remove any lingering callbacks and pending events regardless of success.
  if (pendingEvents_.find(uuid) != pendingEvents_.end() &&
      pendingEvents_[uuid].IsValid()) {
    pendingEvents_[uuid].CancelTask();
  }
  pendingEvents_.erase(uuid);
}

int64_t AccessibilityEventDispatcher::Uuid(int64_t accessibilityId,
                                           int32_t eventType) {
  if (viewIndependentEvents_.find(eventType) != viewIndependentEvents_.end()) {
    return eventType;
  }
  return (accessibilityId << 32) | eventType;
}
}  // namespace content
