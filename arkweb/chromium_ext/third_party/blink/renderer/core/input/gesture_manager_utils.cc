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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/input/gesture_manager_utils.h"

#include "third_party/blink/renderer/core/editing/selection_controller.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/settings.h"
#include "third_party/blink/renderer/core/input/event_handler.h"
#include "third_party/blink/renderer/core/input/mouse_event_manager.h"
#include "third_party/blink/renderer/core/page/event_with_hit_test_results.h"
#include "ui/gfx/geometry/point_conversions.h"
#include "arkweb/build/features/features.h"
#include "build/build_config.h"

namespace blink {
GestureManagerUtils::GestureManagerUtils(GestureManager* gesture_manager)
    : gesture_manager_(gesture_manager) {}

#if BUILDFLAG(ARKWEB_AI)
void GestureManagerUtils::CloseAIOverlay(
    const GestureEventWithHitTestResults& targeted_event) {
  const WebGestureEvent& gesture_event = targeted_event.Event();
  HitTestLocation location(
      gesture_manager_->frame_->View()->ConvertFromRootFrame(
          gfx::ToFlooredPoint(gesture_event.PositionInRootFrame())));
  HitTestResult hit_test_result =
      gesture_manager_->frame_->GetEventHandler().HitTestResultAtLocation(
          location);
  if (gesture_manager_->mouse_event_manager_->AsMouseEventManagerExt()
          ->GetHitOverlayStatus(hit_test_result, true) !=
      HitOverlayStatus::kCreating) {
    LOG(INFO) << "HandleGestureShortPress CloseImageOverlay";
    gesture_manager_->mouse_event_manager_->AsMouseEventManagerExt()
        ->CloseImageOverlay();
  }
}
#endif

#if BUILDFLAG(ARKWEB_DRAG_DROP)
WebInputEventResult GestureManagerUtils::HandleGestureDragLongPress(
    const GestureEventWithHitTestResults& targeted_event) {
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
  LOG_FEEDBACK(INFO) << "DragDrop HandleGestureDragLongPress";
#endif
  const WebGestureEvent& gesture_event = targeted_event.Event();

  // FIXME: Ideally we should try to remove the extra mouse-specific hit-tests
  // here (re-using the supplied HitTestResult), but that will require some
  // overhaul of the touch drag-and-drop code and LongPress is such a special
  // scenario that it's unlikely to matter much in practice.

  HitTestLocation location(
      gesture_manager_->frame_->View()->ConvertFromRootFrame(
          gfx::ToFlooredPoint(gesture_event.PositionInRootFrame())));
  HitTestResult hit_test_result =
      gesture_manager_->frame_->GetEventHandler().HitTestResultAtLocation(
          location);

  // only image support touch drag, currently do not support background image
  // add support link
  if ((hit_test_result.AbsoluteImageURL().IsEmpty() ||
       !hit_test_result.GetImage()) &&
      hit_test_result.AbsoluteLinkURL().IsEmpty()) {
    return WebInputEventResult::kNotHandled;
  }
  if (gesture_manager_->mouse_event_manager_->HandleDragDropIfPossible(
          targeted_event)) {
    return WebInputEventResult::kHandledSystem;
  }

  return WebInputEventResult::kNotHandled;
}
#endif

#if BUILDFLAG(ARKWEB_AI)
WebInputEventResult GestureManagerUtils::HandleGestureCreateOverlay(
    const GestureEventWithHitTestResults& targeted_event) {
  gesture_manager_->mouse_event_manager_->AsMouseEventManagerExt()
      ->HandleGestureCreateOverlay(targeted_event.Event());
  return WebInputEventResult::kHandledSystem;
}

void GestureManagerUtils::UpdateContextMenuForAI(
    const HitTestResult& hit_test_result,
    const HitTestLocation& location,
    const WebGestureEvent& gesture_event) {
  unsigned modifiers = gesture_event.GetModifiers();

  WebMouseEvent fake_mouse_event(
      WebInputEvent::Type::kMouseUp, gesture_event,
      WebPointerProperties::Button::kLeft, gesture_event.TapCount(),
      static_cast<WebInputEvent::Modifiers>(
          modifiers | WebInputEvent::Modifiers::kIsCompatibilityEventForTouch),
      gesture_event.TimeStamp());

  gesture_manager_->selection_controller_->SetDataDetectorHitTest(
      MouseEventWithHitTestResults(fake_mouse_event, location,
                                   hit_test_result));
}
#endif

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
void GestureManagerUtils::UpdateContextMenuForFreeCopy(
    HitTestResult& hit_test_result,
    HitTestLocation& location) {
  Node* inner_node = hit_test_result.InnerNode();
  bool is_contextmenu_customization_enabled = false;
  if (gesture_manager_->frame_->GetSettings()) {
#if BUILDFLAG(ARKWEB_EX_FREE_COPY)
    is_contextmenu_customization_enabled =
        gesture_manager_->frame_->GetSettings()
            ->IsContextMenuCustomizationEnabled();
#endif
  }
  if (is_contextmenu_customization_enabled) {
    if (hit_test_result.IsContentEditable() ||
        (!hit_test_result.IsLiveLink())) {
      if (inner_node && inner_node->GetLayoutObject() &&
          gesture_manager_->selection_controller_->HandleGestureLongPress(
#if BUILDFLAG(ARKWEB_FOCUS)
              hit_test_result,
              gesture_manager_->frame_->GetSettings() &&
                  gesture_manager_->frame_->GetSettings()->GetGestureFocusMode() &&
                  gesture_manager_->frame_->GetSettings()->GetShowContextMenuOnMouseUp())) {
#else
              hit_test_result)) {
#endif
        gesture_manager_->mouse_event_manager_->FocusDocumentView();
      }
    }
#if BUILDFLAG(ARKWEB_EX_FREE_COPY)
    gesture_manager_->selection_controller_->SetLastLongPressHitTestResult(
        hit_test_result);
    // notify webContentImpl reset showing_context_menu_ status, to make sure
    // update contextMenu
    if (!hit_test_result.IsSelected(location)) {
      gesture_manager_->selection_controller_->NotifyContextMenuWillShow();
    }
#endif
  } else {
    if (inner_node && inner_node->GetLayoutObject() &&
        gesture_manager_->selection_controller_->HandleGestureLongPress(
#if BUILDFLAG(ARKWEB_FOCUS)
            hit_test_result,
            gesture_manager_->frame_->GetSettings() &&
                gesture_manager_->frame_->GetSettings()->GetGestureFocusMode() &&
                gesture_manager_->frame_->GetSettings()->GetShowContextMenuOnMouseUp())) {
#else
            hit_test_result)) {
#endif
      gesture_manager_->mouse_event_manager_->FocusDocumentView();
    }
  }
}
#endif
void GestureManagerUtils::Trace(Visitor* visitor) const {
  visitor->Trace(gesture_manager_);
}
}  // namespace blink
