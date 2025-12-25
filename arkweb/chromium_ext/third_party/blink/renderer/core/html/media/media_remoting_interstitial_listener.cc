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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/html/media/media_remoting_interstitial_listener.h"

namespace {

constexpr double kLargestPercentage = 100.0;

}  // namespace

namespace blink {

void ProgressBarEventListener::Invoke(ExecutionContext* context, Event* event) {
  if (!weak_ptr_ || !progress_bar_) {
    LOG(WARNING) << "Progress bar event listener: weak_ptr_ or progress_bar_ is null.";
    return;
  }
  const AtomicString& event_type = event->type();
  
  // Dispatch and handle according to event type
  if (event_type == event_type_names::kClick) {
    auto* mouse_event = DynamicTo<MouseEvent>(event);
    HandleMouseClick(mouse_event);
  } else if (event_type == event_type_names::kMousedown) {
    auto* mouse_event = DynamicTo<MouseEvent>(event);
    HandleMouseDown(mouse_event);
  } else if (event_type == event_type_names::kMousemove) {
    auto* mouse_event = DynamicTo<MouseEvent>(event);
    HandleMouseMove(mouse_event);
  } else if (event_type == event_type_names::kMouseup) {
    auto* mouse_event = DynamicTo<MouseEvent>(event);
    HandleMouseUp(mouse_event);
  } else if (event_type == event_type_names::kMouseleave) {
    auto* mouse_event = DynamicTo<MouseEvent>(event);
    HandleMouseLeave(mouse_event);
  }   // Handle touch events
  else if (event_type == event_type_names::kTouchstart) {
    auto* touch_event = DynamicTo<TouchEvent>(event);
    if (touch_event) HandleTouchStart(touch_event);
  }
  else if (event_type == event_type_names::kTouchmove) {
    auto* touch_event = DynamicTo<TouchEvent>(event);
    if (touch_event) HandleTouchMove(touch_event);
  }
  else if (event_type == event_type_names::kTouchend) {
    auto* touch_event = DynamicTo<TouchEvent>(event);
    if (touch_event) HandleTouchEnd(touch_event);
  }
  else if (event_type == event_type_names::kTouchcancel) {
    auto* touch_event = DynamicTo<TouchEvent>(event);
    if (touch_event) HandleTouchCancel(touch_event);
  }
  else {
    LOG(INFO) << "Untreated progress bar event type: " << event_type.Utf8();
  }
}

void ProgressBarEventListener::HandleTouchStart(TouchEvent* event) {
  LOG(INFO) << "Handling touch start events";
  
  event->stopPropagation();
  event->preventDefault();
  
  // If already dragging, ignore the new touch.
  if (is_touch_dragging_ || is_dragging_) {
    LOG(INFO) << "Already in drag state, ignoring new touch.";
    return;
  }
  
  // Get the first touch point
  Touch* active_touch = GetActiveTouch(event);
  if (!active_touch) {
    LOG(WARNING) << "Unable to obtain valid touch points";
    return;
  }
  
  // Start touch and drag
  is_touch_dragging_ = true;
  active_touch_id_ = active_touch->identifier();
  
  float client_x = active_touch->clientX();
  StartDragging(client_x, client_x);
  
  LOG(INFO) << "Touch and drag to start, ID: " << active_touch_id_
            << ", X: " << client_x;
}

void ProgressBarEventListener::HandleTouchMove(TouchEvent* event) {
  if (!is_touch_dragging_) {
    return;
  }
  
  event->stopPropagation();
  event->preventDefault();
  
  // Find the touch point for the activity
  Touch* active_touch = nullptr;
  TouchList* touches = event->targetTouches();
  
  for (unsigned i = 0; i < touches->length(); ++i) {
    Touch* touch = touches->item(i);
    if (touch->identifier() == active_touch_id_) {
      active_touch = touch;
      break;
    }
  }
  
  if (!active_touch) {
    LOG(WARNING) << "Unable to find the touch point for the activity, ID: " << active_touch_id_;
    return;
  }
  
  // Update Drag Position
  float client_x = active_touch->clientX();
  UpdateDragging(client_x);
  
  LOG(INFO) << "Touch and drag in progress, ID: " << active_touch_id_
            << ", X: " << client_x;
}

void ProgressBarEventListener::UpdateDragging(float client_x) {
  double percentage = CalculatePercentageFromDrag(client_x);
  
  LOG(INFO) << "Drag to update - CurrentX: " << client_x
            << ", Percentage: " << percentage << "%";
  
  if (weak_ptr_) {
    weak_ptr_->OnProgressDragging(percentage);
  }
}

void ProgressBarEventListener::HandleTouchEnd(TouchEvent* event) {
  if (!is_touch_dragging_) {
    return;
  }
  
  LOG(INFO) << "Handling touch end events";
  
  event->stopPropagation();
  event->preventDefault();
  
  // End touch and drag
  is_touch_dragging_ = false;
  
  // Get the last location (if available)
  float client_x = drag_start_x_; // Default starting point used.
  TouchList* touches = event->changedTouches();
  
  for (unsigned i = 0; i < touches->length(); ++i) {
    Touch* touch = touches->item(i);
    if (touch->identifier() == active_touch_id_) {
      client_x = touch->clientX();
      break;
    }
  }
  
  EndDragging(client_x);
  active_touch_id_ = -1;
  
  LOG(INFO) << "Touch and drag ended - Final X: " << client_x;
}

void ProgressBarEventListener::HandleTouchCancel(TouchEvent* event) {
  LOG(INFO) << "Handling touch cancellation events";
  
  if (is_touch_dragging_) {
    is_touch_dragging_ = false;
    active_touch_id_ = -1;
    
    // Cancel dragging, do not perform jump.
    if (weak_ptr_) {
      weak_ptr_->OnProgressDragCancel();
    }
    
    LOG(INFO) << "Touch and drag canceled.";
  }
}

void ProgressBarEventListener::EndDragging(float client_x) {
  is_dragging_ = false;
  
  double percentage = CalculatePercentageFromDrag(client_x);
  
  LOG(INFO) << "Dragging ended - Final X: " << client_x
            << ", Percentage: " << percentage << "%";
  
  if (weak_ptr_) {
    weak_ptr_->OnProgressDragEnd(percentage);
  }
}

double ProgressBarEventListener::CalculatePercentageFromDrag(float client_x) {
  float left = GetProgressBarLeft();
  float width = GetProgressBarWidth();
  if (width <= 0) {
    LOG(ERROR) << "Invalid progress bar width";
    return drag_start_percentage_;
  }
  
  // Calculate the offset relative to the drag start point
  float delta_x = client_x - drag_start_x_;
  
  // Calculate Offset Percentage
  float delta_percentage = (delta_x / width) * kLargestPercentage;
  
  // Calculate New Percentage
  double new_percentage = drag_start_percentage_ + delta_percentage;
  
  // Limit between 0 and 100.
  new_percentage = std::max(0.0, std::min(kLargestPercentage, new_percentage));
  
  LOG(INFO) << "Drag Calculation - Starting Point X: " << drag_start_x_
            << ", Current X: " << client_x
            << ", Offset X: " << delta_x
            << ", Starting Point %: " << drag_start_percentage_
            << ", Offset%: " << delta_percentage
            << ", New%: " << new_percentage;
  
  return new_percentage;
}

Touch* ProgressBarEventListener::GetActiveTouch(TouchEvent* event) {
  if (!event) return nullptr;
  
  TouchList* touches = event->targetTouches();
  if (touches->length() > 0) {
    return touches->item(0); // Return to the first touch point
  }
  
  // If targetTouches is empty, try changedTouches.
  touches = event->changedTouches();
  if (touches->length() > 0) {
    return touches->item(0);
  }
  
  return nullptr;
}

void ProgressBarEventListener::StartDragging(float start_x, float client_x) {
  is_dragging_ = true;
  drag_start_x_ = start_x;
  
  // Get the current progress percentage as the drag start point
  drag_start_percentage_ = CalculatePercentage(client_x);
  
  LOG(INFO) << "Drag Start - Starting Point X: " << drag_start_x_
            << ", Starting Point Percentage: " << drag_start_percentage_ << "%";
  
  if (weak_ptr_) {
    weak_ptr_->OnProgressDragStart();
  }
}

double ProgressBarEventListener::CalculatePercentage(float client_x) {
  float left = GetProgressBarLeft();
  float width = GetProgressBarWidth();
  
  if (width <= 0) {
    LOG(ERROR) << "Invalid progress bar width";
    return 0.0;
  }
  
  // Calculate the position relative to the progress bar
  float relative_x = client_x - left;
  
  // Ensure it is within the scope.
  relative_x = std::max(0.0f, std::min(width, relative_x));
  
  // Calculate percentage
  double percentage = (relative_x / width) * kLargestPercentage;
  percentage = std::max(0.0, std::min(kLargestPercentage, percentage));
  
  return percentage;
}

float ProgressBarEventListener::GetProgressBarLeft() {
  if (!progress_bar_) return 0.0f;
  
  DOMRect* dom_rect = progress_bar_->GetBoundingClientRect();
  if (!dom_rect) return 0.0f;
  
  return static_cast<float>(dom_rect->left());
}

float ProgressBarEventListener::GetProgressBarWidth() {
  if (!progress_bar_) return 0.0f;
  
  DOMRect* dom_rect = progress_bar_->GetBoundingClientRect();
  if (!dom_rect) return 0.0f;
  
  return static_cast<float>(dom_rect->width());
}


void ProgressBarEventListener::HandleMouseClick(MouseEvent* event) {
  LOG(INFO) << "Handling progress bar click events";
  
  event->stopPropagation();
  event->preventDefault();
  
  float client_x = event->clientX();
  double percentage = CalculatePercentage(client_x);
  
  LOG(INFO) << "Percentage of mouse click position: " << percentage << "%";
  
  if (weak_ptr_) {
    weak_ptr_->OnProgressBarClicked(percentage);
  }
}

void ProgressBarEventListener::HandleMouseDown(MouseEvent* event) {
  LOG(INFO) << "Handling mouse press events";
  
  event->stopPropagation();
  event->preventDefault();
  
  // If already touching and dragging, ignore mouse events.
  if (is_touch_dragging_) {
    LOG(INFO) << "Touch and drag in progress, ignoring mouse events.";
    return;
  }
  
  float client_x = event->clientX();
  StartDragging(client_x, client_x);
  
  LOG(INFO) << "Mouse drag start - X: " << client_x;
}

void ProgressBarEventListener::HandleMouseMove(MouseEvent* event) {
  // Process mouse movement only during drag state.
  if (!is_dragging_ || is_touch_dragging_) {
    return;
  }
  
  float client_x = event->clientX();
  UpdateDragging(client_x);  
  LOG(INFO) << "Mouse dragging in progress - X: " << client_x;
}

void ProgressBarEventListener::HandleMouseUp(MouseEvent* event) {
  if (!is_dragging_ || is_touch_dragging_) {
    return;
  }
  
  LOG(INFO) << "Handling mouse release events";
  float client_x = event->clientX();
  EndDragging(client_x);
  LOG(INFO) << "Mouse drag ended - Final X: " << client_x;
}

void ProgressBarEventListener::HandleMouseLeave(MouseEvent* event) {
  if (is_dragging_ && !is_touch_dragging_) {
    LOG(INFO) << "Mouse leaves the element, ending the drag.";
    float client_x = event->clientX();
    EndDragging(client_x);
  }
}

} // namespace blink