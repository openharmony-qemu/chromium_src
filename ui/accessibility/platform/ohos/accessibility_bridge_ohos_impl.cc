// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/accessibility/platform/ohos/accessibility_bridge_ohos_impl.h"

#include <arkui/native_interface_accessibility.h>

#include <functional>
#include <memory>

#include "base/functional/callback.h"
#include "base/ohos/task_scheduler/task_runner_ohos.h"
#include "base/trace_event/trace_event.h"
#include "content/browser/accessibility/browser_accessibility_ohos.h"
#include "ohos/adapter/accessibility/accessibility_adapter.h"
#include "ohos/adapter/accessibility/accessibility_delegate_ohos_registry.h"
#include "ui/accessibility/platform/ax_platform_node.h"
#include "ui/aura/window_tree_host_platform.h"
#include "ui/display/display.h"
#include "ui/display/screen.h"

namespace ui {

// In the ohos system, an element_id of -1 for queries indicates that the search
// should start from the root node of the accessibility tree and retrieve
// information for all nodes.
const int32_t kSearchFromRootMode = -1;
// The ID of the root node of the accessibility tree on the ohos system.
const int32_t kNativeUIRootId = 0;
// The ID of the parent node of the root node of the accessibility tree in the
// ohos system.
const int32_t kNativeUIRootParentId = -2100000;

AccessibilityBridgeOhosImpl::AccessibilityBridgeOhosImpl(
    aura::Window* root_window)
    : root_window_(root_window) {
  root_window_->AddObserver(this);
}

AccessibilityBridgeOhosImpl::~AccessibilityBridgeOhosImpl() {
  if (root_window_) {
    root_window_->RemoveObserver(this);
    root_window_ = nullptr;
  }
}

void AccessibilityBridgeOhosImpl::GetRootWindowPosition() {
  if (root_window_) {
    auto root_node = ui::AXPlatformNode::FromNativeWindow(root_window_);
    if (root_node) {
      AXPlatformNodeDelegate* delegate = root_node->GetDelegate();
      if (delegate) {
        gfx::Rect rect_pixel =
            delegate->GetBoundsRect(AXCoordinateSystem::kScreenPhysicalPixels,
                                    AXClippingBehavior::kUnclipped);
        root_window_bound_x_ = rect_pixel.x();
        root_window_bound_y_ = rect_pixel.y();
      }
    }
  } else {
    LOG(ERROR) << __func__ << " [Accessibility] root_window_ not exist";
    return;
  }
}

void AccessibilityBridgeOhosImpl::SetElementRect(
    ArkUI_AccessibilityElementInfo* element,
    ui::AXPlatformNodeBase* node) {
  AXPlatformNodeDelegate* delegate = node->GetDelegate();
  if (!delegate) {
    return;
  }

  gfx::Rect rect_pixel =
      delegate->GetBoundsRect(AXCoordinateSystem::kScreenPhysicalPixels,
                              AXClippingBehavior::kUnclipped);
  ArkUI_AccessibleRect ax_rect;
  ax_rect.leftTopX = rect_pixel.x() - root_window_bound_x_;
  ax_rect.leftTopY = rect_pixel.y() - root_window_bound_y_;
  ax_rect.rightBottomX = ax_rect.leftTopX + rect_pixel.width();
  ax_rect.rightBottomY = ax_rect.leftTopY + rect_pixel.height();
  OH_ArkUI_AccessibilityElementInfoSetScreenRect(element, &ax_rect);
}

void AccessibilityBridgeOhosImpl::AddNativeUIElementDetails(
    NativeElement element,
    ui::AXPlatformNodeOHOS* node,
    int32_t id,
    int32_t parent_id) {
  OH_ArkUI_AccessibilityElementInfoSetElementId(element, id);
  OH_ArkUI_AccessibilityElementInfoSetParentId(element, parent_id);
  OH_ArkUI_AccessibilityElementInfoSetComponentType(
      element, node->GetRoleString().c_str());
  OH_ArkUI_AccessibilityElementInfoSetContents(element,
                                               node->GetText().c_str());
  OH_ArkUI_AccessibilityElementInfoSetAccessibilityDescription(
      element, node->GetDescription().c_str());
  OH_ArkUI_AccessibilityElementInfoSetHintText(element,
                                               node->GetHint().c_str());
  OH_ArkUI_AccessibilityElementInfoSetIsHint(element, node->IsHint());

  OH_ArkUI_AccessibilityElementInfoSetVisible(element,
                                              !node->IsInvisibleOrIgnored());
  SetElementRect(element, node);
  OH_ArkUI_AccessibilityElementInfoSetCheckable(element,
                                                node->IsPlatformCheckable());
  OH_ArkUI_AccessibilityElementInfoSetChecked(element, node->IsChecked());
  OH_ArkUI_AccessibilityElementInfoSetClickable(element, node->IsClickable());
  OH_ArkUI_AccessibilityElementInfoSetLongClickable(element,
                                                    node->IsLongClickable());
  OH_ArkUI_AccessibilityElementInfoSetFocused(element, node->IsFocused());
  OH_ArkUI_AccessibilityElementInfoSetScrollable(element, node->IsScrollable());
  OH_ArkUI_AccessibilityElementInfoSetSelected(
      element, node->ISelectionItemProviderIsSelected());
  OH_ArkUI_AccessibilityElementInfoSetEnabled(element, node->IsEnabled());

  OH_ArkUI_AccessibilityElementInfoSetBackgroundColor(
      element, node->GetBackgroundColor().c_str());
  OH_ArkUI_AccessibilityElementInfoSetBackgroundImage(
      element, node->GetBackgroundImage().c_str());

  OH_ArkUI_AccessibilityElementInfoSetAccessibilityOpacity(element,
                                                           node->GetOpacity());

  std::vector<ArkUI_AccessibleAction> actions;
  if (node->IsClickable()) {
    actions.push_back(
        {.actionType = ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CLICK,
         .description = ui::ToString(ax::mojom::BoolAttribute::kClickable)});
  }

  if (node->IsFocused()) {
    actions.push_back(
        {.actionType =
             ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_GAIN_ACCESSIBILITY_FOCUS,
         .description = ui::ToString(ax::mojom::Action::kFocus)});
    actions.push_back(
        {.actionType =
             ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_CLEAR_ACCESSIBILITY_FOCUS,
         .description =
             ui::ToString(ax::mojom::Action::kClearAccessibilityFocus)});
  }

  if (node->IsLongClickable()) {
    actions.push_back(
        {.actionType = ARKUI_ACCESSIBILITY_NATIVE_ACTION_TYPE_LONG_CLICK,
         .description =
             ui::ToString(ax::mojom::BoolAttribute::kLongClickable)});
  }

  if (!actions.empty()) {
    OH_ArkUI_AccessibilityElementInfoSetOperationActions(
        element, static_cast<int32_t>(actions.size()), actions.data());
  }
}

int SetElementChildIds(ArkUI_AccessibilityElementInfo* element,
                       ui::AXPlatformNodeOHOS* node,
                       std::queue<ui::AXPlatformNodeOHOS*>* queue = nullptr) {
  std::vector<int64_t> children_ids;
  for (AXPlatformNodeBase::AXPlatformNodeChildIterator iter =
           node->AXPlatformNodeChildrenBegin();
       iter != node->AXPlatformNodeChildrenEnd(); ++iter) {
    ui::AXPlatformNodeOHOS* child =
        static_cast<ui::AXPlatformNodeOHOS*>(iter.get());
    if (queue) {
      queue->push(child);
    }
    children_ids.push_back(child->GetUniqueId());
  }
  OH_ArkUI_AccessibilityElementInfoSetChildNodeIds(element, children_ids.size(),
                                                   children_ids.data());
  return children_ids.size();
}

int AccessibilityBridgeOhosImpl::AddNativeUIElement(
    NativeElementList element_list,
    ui::AXPlatformNodeOHOS* node,
    int32_t id,
    int32_t parent_id,
    std::queue<ui::AXPlatformNodeOHOS*>* queue) {
  auto element = OH_ArkUI_AddAndGetAccessibilityElementInfo(element_list);
  AddNativeUIElementDetails(element, node, id, parent_id);
  return SetElementChildIds(element, node, queue);
}

int AccessibilityBridgeOhosImpl::AddNativeUIRootElement(
    NativeElementList element_list,
    ui::AXPlatformNodeOHOS* ax_platform_node,
    std::queue<ui::AXPlatformNodeOHOS*>* queue) {
  return AddNativeUIElement(element_list, ax_platform_node, kNativeUIRootId,
                            kNativeUIRootParentId, queue);
}

void AccessibilityBridgeOhosImpl::SetCurNativeUIElement(
    int32_t element_id,
    NativeElementList element_list,
    AXPlatformNodeOHOS* ax_platform_node) {
  if (element_id == kSearchFromRootMode) {
    AddNativeUIRootElement(element_list, ax_platform_node);
  } else {
    ui::AXPlatformNodeBase* parent = ax_platform_node->GetPlatformParent();
    if (!parent) {
      LOG(ERROR) << "AccessibilityBridgeOhosImpl::SetCurNativeUIElement get "
                    "parent fail";
      return;
    }
    AddNativeUIElement(element_list, ax_platform_node,
                       ax_platform_node->GetUniqueId(), parent->GetUniqueId());
  }
}

void AccessibilityBridgeOhosImpl::SetParentNativeUIElement(
    int32_t element_id,
    NativeElementList element_list,
    AXPlatformNodeOHOS* ax_platform_node) {
  if (element_id == kSearchFromRootMode) {
    AddNativeUIRootElement(element_list, ax_platform_node);
  } else {
    if (root_window_ == nullptr) {
      LOG(ERROR) << __func__ << " [Accessibility] root_window_ not exist";
      return;
    }
    ui::AXPlatformNodeOHOS* parent = static_cast<ui::AXPlatformNodeOHOS*>(
        ax_platform_node->GetPlatformParent());
    if (!parent) {
      return;
    }
    auto root_node = static_cast<ui::AXPlatformNodeOHOS*>(
        ui::AXPlatformNode::FromNativeWindow(root_window_));
    if (!root_node) {
      return;
    }
    if (parent->GetUniqueId() == root_node->GetUniqueId()) {
      AddNativeUIRootElement(element_list, parent);
      AddNativeUIElement(element_list, ax_platform_node,
                         ax_platform_node->GetUniqueId(), kNativeUIRootId);
    } else {
      ui::AXPlatformNodeOHOS* grand_parent =
          static_cast<ui::AXPlatformNodeOHOS*>(parent->GetPlatformParent());
      if (!grand_parent) {
        return;
      }
      int grandparent_id =
          grand_parent->GetUniqueId() == root_node->GetUniqueId()
              ? kNativeUIRootId
              : grand_parent->GetUniqueId();
      AddNativeUIElement(element_list, parent, parent->GetUniqueId(),
                         grandparent_id);
      AddNativeUIElement(element_list, ax_platform_node,
                         ax_platform_node->GetUniqueId(),
                         parent->GetUniqueId());
    }
  }
}

void AccessibilityBridgeOhosImpl::SetSiblingsNativeUIElements(
    int32_t element_id,
    NativeElementList element_list,
    AXPlatformNodeOHOS* ax_platform_node) {
  if (element_id == kSearchFromRootMode) {
    AddNativeUIRootElement(element_list, ax_platform_node);
  } else {
    if (root_window_ == nullptr) {
      LOG(ERROR) << __func__ << " [Accessibility] root_window_ not exist";
      return;
    }
    ui::AXPlatformNodeOHOS* parent = static_cast<ui::AXPlatformNodeOHOS*>(
        ax_platform_node->GetPlatformParent());
    if (!parent) {
      return;
    }

    for (AXPlatformNodeBase::AXPlatformNodeChildIterator iter =
             parent->AXPlatformNodeChildrenBegin();
         iter != parent->AXPlatformNodeChildrenEnd(); ++iter) {
      ui::AXPlatformNodeOHOS* child =
          static_cast<ui::AXPlatformNodeOHOS*>(iter.get());

      auto root_node = static_cast<ui::AXPlatformNodeOHOS*>(
          ui::AXPlatformNode::FromNativeWindow(root_window_));
      if (!root_node) {
        return;
      }
      int32_t parent_id = parent->GetUniqueId() == root_node->GetUniqueId()
                              ? kNativeUIRootId
                              : parent->GetUniqueId();
      AddNativeUIElement(element_list, child, child->GetUniqueId(), parent_id);
    }
  }
}

void AccessibilityBridgeOhosImpl::SetChildrenNativeUIElements(
    int32_t element_id,
    NativeElementList element_list,
    AXPlatformNodeOHOS* ax_platform_node) {
  std::queue<ui::AXPlatformNodeOHOS*> queue;
  if (element_id == kSearchFromRootMode) {
    AddNativeUIRootElement(element_list, ax_platform_node, &queue);
  } else {
    ui::AXPlatformNodeBase* parent = ax_platform_node->GetPlatformParent();
    if (!parent) {
      return;
    }
    AddNativeUIElement(element_list, ax_platform_node,
                       ax_platform_node->GetUniqueId(), parent->GetUniqueId(),
                       &queue);
  }

  while (!queue.empty()) {
    ui::AXPlatformNodeOHOS* current = queue.front();
    int32_t parent_id = element_id == kSearchFromRootMode
                            ? kNativeUIRootId
                            : ax_platform_node->GetUniqueId();
    AddNativeUIElement(element_list, current, current->GetUniqueId(),
                       parent_id);
    queue.pop();
  }
}

void AccessibilityBridgeOhosImpl::AddNativeUITreeByLevelOrder(
    NativeElementList element_list,
    ui::AXPlatformNodeOHOS* node,
    bool is_from_root) {
  std::queue<ui::AXPlatformNodeOHOS*> queue;
  int second_level_count = 0;
  if (is_from_root) {
    second_level_count = AddNativeUIRootElement(element_list, node, &queue);
  } else {
    queue.push(node);
  }

  int handled_count = 0;
  while (!queue.empty()) {
    ui::AXPlatformNodeOHOS* current = queue.front();
    ui::AXPlatformNodeBase* parent = current->GetPlatformParent();
    if (parent) {
      int32_t parent_id = handled_count < second_level_count
                              ? kNativeUIRootId
                              : parent->GetUniqueId();
      AddNativeUIElement(element_list, current, current->GetUniqueId(),
                         parent_id, &queue);
    } else {
      LOG(WARNING)
          << "AccessibilityBridgeOhosImpl::AddNativeUITreeByLevelOrder "
             "current_node's parent can not find";
    }
    queue.pop();
    handled_count++;
  }
}

void AccessibilityBridgeOhosImpl::SetNativeUITree(
    int32_t element_id,
    NativeElementList element_list,
    AXPlatformNodeOHOS* ax_platform_node) {
  AddNativeUITreeByLevelOrder(element_list, ax_platform_node,
                              element_id == kSearchFromRootMode);
}

void AccessibilityBridgeOhosImpl::handleFindAccessibilityNodeInfosByIdRequest(
    int32_t element_id,
    ArkUI_AccessibilitySearchMode mode,
    NativeElementList element_list,
    std::shared_ptr<std::promise<bool>> promise) {
  TRACE_EVENT2("accessibility", "AccessibilityBridgeOhosImpl::handleRequest",
               "element_id", element_id, "mode", mode);

  GetRootWindowPosition();
  ui::AXPlatformNodeOHOS* current_node = nullptr;
  if (element_id == kSearchFromRootMode) {
    if (!root_window_) {
      promise->set_value(false);
      return;
    }
    current_node = static_cast<ui::AXPlatformNodeOHOS*>(
        ui::AXPlatformNode::FromNativeWindow(root_window_));
  } else {
    current_node = static_cast<ui::AXPlatformNodeOHOS*>(
        ui::AXPlatformNodeOHOS::GetFromUniqueId(element_id));
  }
  if (!current_node) {
    LOG(ERROR) << "AccessibilityBridgeOhosImpl::"
                  "handleFindAccessibilityNodeInfosByIdRequest can't find "
                  "such platform node by id: "
               << element_id;
    promise->set_value(false);
    return;
  }

  bool success = true;
  switch (mode) {
    case ARKUI_ACCESSIBILITY_NATIVE_SEARCH_MODE_PREFETCH_CURRENT:
      SetCurNativeUIElement(element_id, element_list, current_node);
      break;
    case ARKUI_ACCESSIBILITY_NATIVE_SEARCH_MODE_PREFETCH_PREDECESSORS:
      SetParentNativeUIElement(element_id, element_list, current_node);
      break;
    case ARKUI_ACCESSIBILITY_NATIVE_SEARCH_MODE_PREFETCH_SIBLINGS:
      SetSiblingsNativeUIElements(element_id, element_list, current_node);
      break;
    case ARKUI_ACCESSIBILITY_NATIVE_SEARCH_MODE_PREFETCH_CHILDREN:
      SetChildrenNativeUIElements(element_id, element_list, current_node);
      break;
    case ARKUI_ACCESSIBILITY_NATIVE_SEARCH_MODE_PREFETCH_RECURSIVE_CHILDREN:
      SetNativeUITree(element_id, element_list, current_node);
      break;
    default:
      success = false;
  }

  promise->set_value(success);
}

bool AccessibilityBridgeOhosImpl::FindAccessibilityNodeInfosById(
    int32_t element_id,
    ArkUI_AccessibilitySearchMode mode,
    NativeElementList element_list) {
  TRACE_EVENT2("accessibility",
               "AccessibilityBridgeOhosImpl::FindAccessibilityNodeInfosById",
               "element_id", element_id, "mode", mode);
  auto promise = std::make_shared<std::promise<bool>>();
  auto future = promise->get_future();
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(
      FROM_HERE,
      base::BindOnce(&AccessibilityBridgeOhosImpl::
                         handleFindAccessibilityNodeInfosByIdRequest,
                     base::Unretained(this), element_id, mode,
                     std::move(element_list), promise));
  return future.get();
}

}  // namespace ui