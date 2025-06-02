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

namespace blink {

#if BUILDFLAG(ARKWEB_AI)
static constexpr int MAX_LENGTH = 100;
static constexpr int HALF_LENGTH = 50;
void SelectionController::FocusDocumentView() {
  Page* page = frame_->GetPage();
  if (!page) {
    return;
  }
  page->GetFocusController().FocusDocumentView(frame_);
}
#endif

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
void SelectionController::SetLastLongPressHitTestResult(
    const HitTestResult& other) {
  last_long_press_hit_test_result_ = HitTestResult(other);
}

void SelectionController::NotifyContextMenuWillShow() {
  if (frame_) {
    frame_->NotifyContextMenuWillShow();
  }
}

bool SelectionController::ShowSelectionByLastLongPressHitTestResult() {
  if (!Selection().IsAvailable()) {
    return false;
  }

  if (last_long_press_hit_test_result_.IsLiveLink() &&
      SelectClosestWordFromLiveLink(last_long_press_hit_test_result_)) {
    ContextMenuAllowedScope scope;
    frame_->GetEventHandler().ShowNonLocatedContextMenu(
        last_long_press_hit_test_result_.InnerElement(),
        kMenuSourceShowFreeCopyMenu);
    return true;
  }
  Node* inner_node = last_long_press_hit_test_result_.InnerNode();
  if (!inner_node || !inner_node->GetLayoutObject()) {
    return false;
  }
  inner_node->GetDocument().UpdateStyleAndLayoutTree();

  const bool did_select = SelectClosestWordFromHitTestResult(
      last_long_press_hit_test_result_, AppendTrailingWhitespace::kDontAppend,
      SelectInputEventType::kTouch);
  if (did_select) {
    ContextMenuAllowedScope scope;
    frame_->GetEventHandler().ShowNonLocatedContextMenu(
        nullptr, kMenuSourceShowFreeCopyMenu);
    return true;
  }

  if (!inner_node->isConnected() || !inner_node->GetLayoutObject()) {
    return false;
  }
  SetCaretAtHitTestResult(last_long_press_hit_test_result_);
  return true;
}
#endif

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY) || BUILDFLAG(ARKWEB_AI)
bool SelectionController::SelectClosestWordFromLiveLink(
    const HitTestResult& result) {
  Node* const inner_node = result.InnerNode();

  if (!inner_node || !inner_node->GetLayoutObject()) {
    return false;
  }
  inner_node->GetDocument().UpdateStyleAndLayoutTree();

  Element* url_element = result.URLElement();
  const PositionInFlatTreeWithAffinity pos =
      CreateVisiblePosition(PositionWithAffinityOfHitTestResult(result))
          .ToPositionWithAffinity();
  bool isCreateFlatTreeByUrlElement =
      pos.IsNotNull() && pos.AnchorNode()->IsDescendantOf(url_element);
  bool isCreateFlatTreeByAnchorNode =
      isCreateFlatTreeByUrlElement && pos.AnchorNode()->IsTextNode();

  const SelectionInFlatTree& new_selection =
      isCreateFlatTreeByAnchorNode ? SelectionInFlatTree::Builder()
                                         .SelectAllChildren(*pos.AnchorNode())
                                         .Build()
      : isCreateFlatTreeByUrlElement ? SelectionInFlatTree::Builder()
                                           .SelectAllChildren(*url_element)
                                           .Build()
                                     : SelectionInFlatTree();

  return UpdateSelectionForMouseDownDispatchingSelectStart(
      inner_node,
      ExpandSelectionToRespectUserSelectAll(inner_node, new_selection),
      SetSelectionOptions::Builder()
          .SetGranularity(TextGranularity::kWord)
          .SetShouldShowHandle(true)
          .Build());
}
#endif

#if BUILDFLAG(ARKWEB_CLIPBOARD)
bool SelectionController::HandleGestureTapIfSelectionExist(
    const MouseEventWithHitTestResults& event) {
  TRACE_EVENT1("blink", "SelectionController::HandleGestureTapIfSelectionExist",
               "Selection().IsAvailable()", Selection().IsAvailable());
  if (!Selection().IsAvailable()) {
    return false;
  }

  WebLocalFrameImpl* web_local_frame = WebLocalFrameImpl::FromFrame(frame_);
  const VisibleSelectionInFlatTree& selection =
      Selection().ComputeVisibleSelectionInFlatTree();
  if (selection.IsNone() ||
      !Selection().ComputeVisibleSelectionInDOMTree().IsRange()) {
    return false;
  }

  bool single_click = event.Event().click_count <= 1;
  bool extend_selection = IsExtendingSelection(event);
  if (!single_click || extend_selection || !event.Event().FromTouch()) {
    LOG(INFO) << "Non-click events are not handled, extend_selection = "
              << extend_selection;
    return false;
  }

  if (Selection().AsFrameSelectionExt()->IsSelectAll()) {
    LOG(INFO) << "Select All does not process click events";
    return false;
  }

  LocalFrameView* view = frame_->View();
  if (!view) {
    return false;
  }
  const PhysicalOffset v_point(view->ConvertFromRootFrame(
      gfx::ToFlooredPoint(event.Event().PositionInRootFrame())));
  bool ret = false;
  if (!Selection().Contains(v_point, false)) {
    LOG(INFO) << "Tap outside the selected range to clear selection";
    if (web_local_frame) {
      const blink::WebRange& range =
          web_local_frame->GetInputMethodController()->GetSelectionOffsets();
      if (!range.IsNull()) {
        web_local_frame->SelectRange(
            blink::WebRange(range.EndOffset(), 0),
            blink::WebLocalFrame::kHideSelectionHandle,
            mojom::blink::SelectionMenuBehavior::kHide,
            WebLocalFrame::SelectionSetFocusBehavior::kSelectionSetFocus);
      }
    }
  } else if (web_local_frame && web_local_frame->Client()) {
    LOG(INFO)
        << "Tap within the selected range to change visibility of quick menu";
    web_local_frame->Client()->AsWebLocalFrameClientExt()->ChangeVisibilityOfQuickMenu();
    ret = true;
  }
  if (mouse_menu_show_) {
    mouse_menu_show_ = false;
    MouseSelectMenuShow(false);
  }
  return ret;
}
#endif  // ARKWEB_CLIPBOARD

#if BUILDFLAG(ARKWEB_MENU)
bool SelectionController::MouseSelectMenuShow(bool show) {
  WebLocalFrameImpl* web_local_frame = WebLocalFrameImpl::FromFrame(frame_);
  if (web_local_frame && web_local_frame->Client()) {
    web_local_frame->Client()->AsWebLocalFrameClientExt()->MouseSelectMenuShow(show);
    return true;
  }
  return false;
}
#endif

#if BUILDFLAG(ARKWEB_AI)
void SelectionController::SetDataDetectorHitTest(const MouseEventWithHitTestResults& event) {
  const HitTestResult& hit_test_result = event.GetHitTestResult();

  LOG(DEBUG) << "SelectionController::SetDataDetectorHitTest";
  Node* inner_node = hit_test_result.InnerNode();
  if (!inner_node || !inner_node->GetLayoutObject()) {
    return;
  }

  if (!hit_test_result.IsLiveLink()) {
    return;
  }

  LOG(INFO) << "SelectionController::SetDataDetectorHitTest success";
  last_link_hit_test_result_ = hit_test_result;
  last_link_menu_source_type_ = event.Event().GetMenuSourceType();
  LOG(DEBUG) << "SelectionController::SetDataDetectorHitTest MenuSourceType: "
             << static_cast<int32_t>(last_link_menu_source_type_);
}

bool SelectionController::ShowSelectionByLastLinkHitTestResult() {
  if (!Selection().IsAvailable()) {
    return false;
  }
  LOG(INFO) << "SelectionController::ShowSelectionByLastLinkHitTestResult";
  Node* inner_node = last_link_hit_test_result_.InnerNode();
  if (inner_node && inner_node->isConnected() && inner_node->GetLayoutObject()) {
    if (last_link_hit_test_result_.IsLiveLink() &&
        SelectClosestWordFromLiveLink(last_link_hit_test_result_)) {
      LOG(INFO)
          << "SelectionController::ShowSelectionByLastLinkHitTestResult success";
      ContextMenuAllowedScope scope;
      frame_->GetEventHandler().ShowNonLocatedContextMenu(
          last_link_hit_test_result_.InnerElement(),
          last_link_menu_source_type_);
      return true;
    }
  } else {
      LOG(ERROR) << "SelectionController::ShowSelectionByLastLinkHitTestResult node not alive.";
  }
  last_link_hit_test_result_ = HitTestResult();
  last_link_menu_source_type_ = WebMenuSourceType::kMenuSourceNone;
  return false;
}
#endif

void SelectionControllerUtils::HandleMouseReleaseEventWithAIExt(
    raw_ptr<SelectionController> obj,
    const MouseEventWithHitTestResults& event) {
  obj->SetDataDetectorHitTest(event);
}

void SelectionControllerUtils::HandleMouseReleaseEventWithMenuExt(raw_ptr<SelectionController> obj,
                                                                  const MouseEventWithHitTestResults& event) {
  bool is_rang = obj->Selection().ComputeVisibleSelectionInDOMTree().IsRange();
  if (obj->mouse_down_was_single_click_in_selection_ || !is_rang ||
      !obj->mouse_down_may_start_select_) {
    if (obj->mouse_click_down_allows_ && obj->mouse_menu_show_) {
      obj->mouse_menu_show_ = false;
      obj->MouseSelectMenuShow(false);
    }
  } else if (is_rang && !event.Event().FromTouch()) {
    obj->mouse_menu_show_ = obj->MouseSelectMenuShow(true);
  }
  obj->mouse_click_down_allows_ = false;
}

SelectionInFlatTree SelectionControllerUtils::HandleArkWebAISelectionExt(raw_ptr<SelectionController> obj,
                                                                         const HitTestResult& result,
                                                                         Node* inner_node,
                                                                         const PositionInFlatTreeWithAffinity& pos) {
  WTF::String str;
  if (pos.IsNotNull()) {
    str = pos.AnchorNode()->textContent();
  } else if (inner_node != nullptr) {
    str = inner_node->textContent();
  }
  unsigned len = SelectionControllerUtils::MaxOffsetTrimTailWhiteSpace(str);

  int offset = pos.GetPosition().OffsetInContainerNode();
  int temp_offset = offset;

  if (len < offset || inner_node->getNodeType() != Node::NodeType::kTextNode) {
    offset = len;
    temp_offset = offset;
  }

  if (len > MAX_LENGTH) {
    if (temp_offset < HALF_LENGTH) {
      str = str.Left(MAX_LENGTH);
    } else if (len - temp_offset <= HALF_LENGTH) {
      str = str.Right(MAX_LENGTH);
      temp_offset -= len - MAX_LENGTH;
    } else {
      str = str.Substring(temp_offset - HALF_LENGTH, MAX_LENGTH);
      temp_offset = HALF_LENGTH;
    }
  }

  WTF::Vector<int8_t> select =
      obj->frame_->View()->GetChromeClient()->AsChromeClientExt()->GetWordSelection(
          obj->frame_, str, temp_offset);
  LOG(INFO) << "GetWordSelection, start: "
            << static_cast<int>(select.at(0) - temp_offset + offset)
            << ", end: "
            << static_cast<int>(select.at(1) - temp_offset + offset);

  SelectionInFlatTree temp_selection;
  TextControlElement* text_control =
      EnclosingTextControl(obj->Selection().GetSelectionInDOMTree().Anchor());
  if (pos.IsNotNull()) {
    if (select.at(0) != -1 && select.at(1) != -1 &&
        (!text_control ||
         text_control->type() != input_type_names::kPassword)) {
      temp_selection =
          SelectionInFlatTree::Builder()
              .Collapse(PositionInFlatTree::CreateWithoutValidation(
                  *pos.AnchorNode(), select.at(0) - temp_offset + offset))
              .Extend(PositionInFlatTree::CreateWithoutValidation(
                  *pos.AnchorNode(), select.at(1) - temp_offset + offset))
              .Build();
    } else {
      temp_selection = ExpandWithGranularity(
          SelectionInFlatTree::Builder().Collapse(pos).Build(),
          TextGranularity::kWord);
    }
  } else {
    temp_selection = SelectionInFlatTree();
  }

  return temp_selection;
}

unsigned SelectionControllerUtils::MaxOffsetTrimTailWhiteSpace(WTF::String& str) {
  unsigned len = str.length();
  do {
    --len;
  } while (len >= 0 && IsASCIISpace(str[len]));
  return len;
}

} // namespace blink