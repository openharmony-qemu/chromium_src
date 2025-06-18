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

#ifndef UI_ACCESSIBILITY_PLATFORM_FUCHSIA_ACCESSIBILITY_BRIDGE_OHOS_IMPL_H_
#define UI_ACCESSIBILITY_PLATFORM_FUCHSIA_ACCESSIBILITY_BRIDGE_OHOS_IMPL_H_

#include <future>

#include "ui/accessibility/platform/ax_platform_node_ohos.h"
#include "ui/aura/window.h"

namespace ui {

using ohos::adapter::accessibility::NativeElement;
using ohos::adapter::accessibility::NativeElementList;

class COMPONENT_EXPORT(AX_PLATFORM) AccessibilityBridgeOhosImpl final
    : public ohos::adapter::accessibility::AccessibilityAdapter::Delegate {
 public:
  AccessibilityBridgeOhosImpl(aura::Window* root_window);
  ~AccessibilityBridgeOhosImpl();

  // Set the information for the specified node.
  void SetCurNativeUIElement(int32_t element_id,
                             NativeElementList element_list,
                             AXPlatformNodeOHOS* ax_platform_node);

  // Set the information of the parent node, including the current node.
  void SetParentNativeUIElement(int32_t element_id,
                                NativeElementList element_list,
                                AXPlatformNodeOHOS* ax_platform_node);

  // Set the information of the sibling nodes, including the current node.
  void SetSiblingsNativeUIElements(int32_t element_id,
                                   NativeElementList element_list,
                                   AXPlatformNodeOHOS* ax_platform_node);

  // Set the information of the next-level child nodes, including the current
  // node.
  void SetChildrenNativeUIElements(int32_t element_id,
                                   NativeElementList element_list,
                                   AXPlatformNodeOHOS* ax_platform_node);

  // Set the information of all child nodes of the current node, including the
  // current node itself.
  void SetNativeUITree(int32_t element_id,
                       NativeElementList element_list,
                       AXPlatformNodeOHOS* ax_platform_node);

  // ohos::adapter::accessibility::AccessibilityAdapter::Delegate overrides.
  bool FindAccessibilityNodeInfosById(int32_t element_id,
                                      ArkUI_AccessibilitySearchMode mode,
                                      NativeElementList element_list) override;

 private:
  // Retrieve the top-left corner position of root window.
  void GetRootWindowPosition();

  // Set the bounding box size of the element.
  void SetElementRect(ArkUI_AccessibilityElementInfo* element,
                      ui::AXPlatformNodeBase* node);

  // Add detailed information of the node element.
  void AddNativeUIElementDetails(NativeElement element,
                                 ui::AXPlatformNodeOHOS* node,
                                 int32_t id,
                                 int32_t parent_id);

  // Add the element information of the node.
  int AddNativeUIElement(NativeElementList element_list,
                         ui::AXPlatformNodeOHOS* node,
                         int32_t id,
                         int32_t parent_id,
                         std::queue<ui::AXPlatformNodeOHOS*>* queue = nullptr);

  // Add the element information of the root node.
  int AddNativeUIRootElement(
      NativeElementList element_list,
      ui::AXPlatformNodeOHOS* ax_platform_node,
      std::queue<ui::AXPlatformNodeOHOS*>* queue = nullptr);

  // Add the element information of nodes in the order of level-order traversal.
  void AddNativeUITreeByLevelOrder(NativeElementList element_list,
                                   ui::AXPlatformNodeOHOS* node,
                                   bool is_from_root);

  void handleFindAccessibilityNodeInfosByIdRequest(
      int32_t element_id,
      ArkUI_AccessibilitySearchMode mode,
      NativeElementList element_list,
      std::shared_ptr<std::promise<bool>> promise);

  // Root window for the ohos view for which this accessibility bridge
  // instance is responsible.
  raw_ptr<aura::Window> root_window_;

  // The top-left corner position of the root window region.
  int root_window_bound_x_ = 0;
  int root_window_bound_y_ = 0;
};

}  // namespace ui
#endif  // UI_ACCESSIBILITY_PLATFORM_FUCHSIA_ACCESSIBILITY_BRIDGE_OHOS_IMPL_H_
