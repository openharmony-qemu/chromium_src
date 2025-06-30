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

#include "arkweb_input_method_controller_utils.h"

#include <tuple>

#include "third_party/blink/public/mojom/input/focus_type.mojom-blink.h"
#include "third_party/blink/public/web/web_frame_widget.h"
#include "third_party/blink/public/web/web_local_frame.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/dom/dom_node_ids.h"
#include "third_party/blink/renderer/core/dom/element.h"
#include "third_party/blink/renderer/core/dom/events/event_dispatcher.h"
#include "third_party/blink/renderer/core/dom/events/scoped_event_queue.h"
#include "third_party/blink/renderer/core/dom/node_computed_style.h"
#include "third_party/blink/renderer/core/dom/range.h"
#include "third_party/blink/renderer/core/dom/text.h"
#include "third_party/blink/renderer/core/editing/commands/delete_selection_command.h"
#include "third_party/blink/renderer/core/editing/commands/undo_stack.h"
#include "third_party/blink/renderer/core/editing/editing_utilities.h"
#include "third_party/blink/renderer/core/editing/editor.h"
#include "third_party/blink/renderer/core/editing/ephemeral_range.h"
#include "third_party/blink/renderer/core/editing/frame_selection.h"
#include "third_party/blink/renderer/core/editing/ime/edit_context.h"
#include "third_party/blink/renderer/core/editing/ime/input_method_controller.h"
#include "third_party/blink/renderer/core/editing/markers/document_marker_controller.h"
#include "third_party/blink/renderer/core/editing/markers/suggestion_marker_properties.h"
#include "third_party/blink/renderer/core/editing/reveal_selection_scope.h"
#include "third_party/blink/renderer/core/editing/selection_template.h"
#include "third_party/blink/renderer/core/editing/set_selection_options.h"
#include "third_party/blink/renderer/core/editing/spellcheck/spell_checker.h"
#include "third_party/blink/renderer/core/editing/state_machines/backward_code_point_state_machine.h"
#include "third_party/blink/renderer/core/editing/state_machines/forward_code_point_state_machine.h"
#include "third_party/blink/renderer/core/events/composition_event.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/local_frame_client.h"
#include "third_party/blink/renderer/core/html/canvas/html_canvas_element.h"
#include "third_party/blink/renderer/core/html/forms/html_input_element.h"
#include "third_party/blink/renderer/core/html/forms/html_text_area_element.h"
#include "third_party/blink/renderer/core/input/context_menu_allowed_scope.h"
#include "third_party/blink/renderer/core/input/event_handler.h"
#include "third_party/blink/renderer/core/input_type_names.h"
#include "third_party/blink/renderer/core/keywords.h"
#include "third_party/blink/renderer/core/layout/layout_object.h"
#include "third_party/blink/renderer/core/layout/layout_theme.h"
#include "third_party/blink/renderer/core/page/focus_controller.h"
#include "third_party/blink/renderer/core/page/page.h"

namespace blink {

using mojom::blink::FormControlType;

ArkwebInputMethodControllerUtils::ArkwebInputMethodControllerUtils(
    InputMethodController* input_method_controller)
    : input_method_controller_(input_method_controller) {
  DCHECK(input_method_controller);
}

ArkwebInputMethodControllerUtils::~ArkwebInputMethodControllerUtils() = default;

void ArkwebInputMethodControllerUtils::Trace(Visitor* visitor) const {
  visitor->Trace(input_method_controller_);
}

Document& ArkwebInputMethodControllerUtils::GetDocument() const {
  return input_method_controller_->GetDocument();
}

EphemeralRange ArkwebInputMethodControllerUtils::CompositionEphemeralRange()
    const {
  return input_method_controller_->CompositionEphemeralRange();
}

LocalFrame& ArkwebInputMethodControllerUtils::GetFrame() const {
  return input_method_controller_->GetFrame();
}

bool ArkwebInputMethodControllerUtils::HasComposition() const {
  return input_method_controller_->HasComposition();
}

bool ArkwebInputMethodControllerUtils::IsAvailable() const {
  return input_method_controller_->IsAvailable();
}

void ArkwebInputMethodControllerUtils::Clear() {
  input_method_controller_->Clear();
}

void ArkwebInputMethodControllerUtils::InsertTextDuringCompositionWithEvents(
    LocalFrame& frame,
    const String& text,
    TypingCommand::Options options,
    TypingCommand::TextCompositionType composition_type) {
  input_method_controller_->InsertTextDuringCompositionWithEvents(
      frame, text, options, composition_type);
}

void ArkwebInputMethodControllerUtils::DispatchCompositionEndEvent(
    LocalFrame& frame,
    const String& text) {
  input_method_controller_->DispatchCompositionEndEvent(frame, text);
}

void ArkwebInputMethodControllerUtils::SelectCompositionEx(
    bool is_max_length_overflow) const {
  const EphemeralRange range = CompositionEphemeralRange();
  if (range.IsNull()) {
    return;
  }

  // When we select the composition (to be able to replace it), we must not
  // claim that the selection is the result of an input event, even though
  // the act of committing the composition _is_ an input event in itself.
  // Otherwise, X11 clients would interpret the selection as a command to
  // replace the primary selection (on the clipboard) with the contents
  // of the composition.
  bool old_handling_input_event = false;
  WebFrameWidget* widget = nullptr;
  if (GetFrame().Client() && GetFrame().Client()->GetWebFrame()) {
    widget = GetFrame().Client()->GetWebFrame()->FrameWidget();
  }
  if (widget) {
    old_handling_input_event = widget->HandlingInputEvent();
    widget->SetHandlingInputEvent(false);
  }

  // The composition can start inside a composed character sequence, so we have
  // to override checks. See <http://bugs.webkit.org/show_bug.cgi?id=15781>

  // The SetSelectionOptions() parameter is necessary because without it,
  // FrameSelection::SetSelection() will actually call
  // SetShouldClearTypingStyle(true), which will cause problems applying
  // formatting during composition. See https://crbug.com/803278.
  GetFrame().Selection().SetSelection(
      SelectionInDOMTree::Builder().SetBaseAndExtent(range).Build(),
      SetSelectionOptions());

  if (widget) {
    widget->SetHandlingInputEvent(old_handling_input_event);
  }
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  if (is_max_length_overflow) {
    GetFrame().Selection().AsFrameSelectionExt()->SetSelectionMarkMaxLengthOverflow();
  }
#endif
}

bool ArkwebInputMethodControllerUtils::ReplaceCompositionEx(
    const String& text,
    bool is_max_length_overflow) {
  // Verify that the caller is using an EventQueueScope to suppress the input
  // event from being fired until the proper time (e.g. after applying an IME
  // selection update, if necessary).
  DCHECK(ScopedEventQueue::Instance()->ShouldQueueEvents());

  if (!HasComposition()) {
    return false;
  }

  // Select the text that will be deleted or replaced.
  SelectCompositionEx(is_max_length_overflow);

  if (GetFrame().Selection().ComputeVisibleSelectionInDOMTree().IsNone()) {
    return false;
  }

  if (!IsAvailable()) {
    return false;
  }

  Clear();

  InsertTextDuringCompositionWithEvents(
      GetFrame(), text, 0,
      TypingCommand::TextCompositionType::kTextCompositionConfirm);

  // textInput event handler might destroy document (input event is queued
  // until later).
  if (!IsAvailable()) {
    return false;
  }

  // No DOM update after 'compositionend'.
  DispatchCompositionEndEvent(GetFrame(), text);

  return true;
}

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void ArkwebInputMethodControllerUtils::GetInputElementAttributes(
    HashMap<String, String>& attributes) const {
  Element* element = GetDocument().FocusedElement();
  if (!element) {
    return;
  }

  bool query_attribute = false;
  if (auto* input = DynamicTo<HTMLInputElement>(*element)) {
    query_attribute = true;
  } else if (IsA<HTMLTextAreaElement>(*element)) {
    query_attribute = true;
  } else {
    element->GetDocument().UpdateStyleAndLayoutTree();
    if (IsEditable(*element)) {
      query_attribute = true;
    }
  }

  if (!query_attribute) {
    return;
  }

  auto attributes_collection = element->Attributes();
  for (const Attribute& attribute : attributes_collection) {
    attributes.insert(attribute.LocalName().GetString(),
                      attribute.Value().GetString());
  }
}
#endif

}  // namespace blink
