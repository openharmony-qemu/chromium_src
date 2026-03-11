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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_SELECTION_CONTROLLER_EXT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_SELECTION_CONTROLLER_EXT_H_

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_SELECTION_CONTROLLER_H_
#error \
    "must be in include form THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_SELECTION_CONTROLLER_H_"
#endif

#include "third_party/blink/renderer/core/editing/selection_controller.h"

namespace blink {

class SelectionControllerUtils {
 public:
  static void HandleMouseReleaseEventWithAIExt(
      SelectionController* obj,
      const MouseEventWithHitTestResults& event);

  static void HandleMouseReleaseEventWithMenuExt(
      SelectionController* obj,
      const MouseEventWithHitTestResults& event);

  static SelectionInFlatTree HandleArkWebAISelectionExt(SelectionController* obj,
                                                        const HitTestResult& result,
                                                        Node* inner_node,
                                                        const PositionInFlatTreeWithAffinity& pos,
                                                        bool is_double_click = false,
                                                        bool layout_change = false);

  static PositionInFlatTree HandleEmptyLine(Node* inner_node,
                                            const PositionInFlatTree& pos,
                                            int depth);

  static unsigned MaxOffsetTrimTailWhiteSpace(WTF::String& str, unsigned len);

  static void OffsetAdjustWhiteSpace(int & offset,
                                     int& temp_offset,
                                     WTF::String& str,
                                     bool permission);

  static Node* SameEditablePreviousSibling(Node* inner_node);

  static Node* SameEditableParent(Node* inner_node);

  static Node* UpdateAnchorIfWhiteSpace(Node* inner_node,
                                 const PositionInFlatTree& pos);

  static bool IsValidAISelection(const WTF::Vector<int8_t>& select);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_SELECTION_CONTROLLER_EXT_H_
