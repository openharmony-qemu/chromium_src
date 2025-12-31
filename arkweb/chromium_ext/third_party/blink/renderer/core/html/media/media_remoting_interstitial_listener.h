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
#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_MEDIA_REMOTING_LISTENER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_MEDIA_REMOTING_LISTENER_H_

#include "third_party/blink/renderer/core/html/media/media_remoting_interstitial.h"

namespace blink {

class MediaRemotingInterstitial;

enum class RemotingButtonType {
  kStopCasting,         // Stop media cast
  kSwitchDevice,        // Switch device
  kPlayPause,           // Play and pause
  kFullscreenToggle     // Full screen
};

class RemotingButtonEventListener : public NativeEventListener {
 public:
  RemotingButtonEventListener(base::WeakPtr<MediaRemotingInterstitial> media_remoting_interstitial,
      RemotingButtonType type)
      : media_remoting_interstitial_(media_remoting_interstitial), button_type_(type){}

  void Invoke(ExecutionContext* execution_context, Event* event) override {
    if (event->type() == event_type_names::kClick) {
      HandleClick(event);
    }
  }

 private:
  void HandleClick(Event* event);

  base::WeakPtr<MediaRemotingInterstitial> media_remoting_interstitial_;
  RemotingButtonType button_type_;
};

class ProgressBarEventListener : public NativeEventListener {
 public:
  ProgressBarEventListener(
      base::WeakPtr<MediaRemotingInterstitial> weak_ptr,
      Element* progress_bar)
      : weak_ptr_(weak_ptr), 
        progress_bar_(progress_bar),
        is_dragging_(false),
        drag_start_x_(0.0f),
        drag_start_percentage_(0.0f),
        active_touch_id_(-1) {}
  
  void Invoke(ExecutionContext* context, Event* event) override;
  
  void Trace(Visitor* visitor) const override {
    visitor->Trace(progress_bar_);
    NativeEventListener::Trace(visitor);
  }

 private:
  // Mouse Event handler
  void HandleMouseClick(MouseEvent* event);
  void HandleMouseDown(MouseEvent* event);
  void HandleMouseMove(MouseEvent* event);
  void HandleMouseUp(MouseEvent* event);
  void HandleMouseLeave(MouseEvent* event);

  // Touch Event Handling
  void HandleTouchStart(TouchEvent* event);
  void HandleTouchMove(TouchEvent* event);
  void HandleTouchEnd(TouchEvent* event);
  void HandleTouchCancel(TouchEvent* event);
  
  // General-purpose handler
  void StartDragging(float start_x, float client_x);
  void UpdateDragging(float client_x);
  void EndDragging(float client_x);

  // Helper function
  float GetProgressBarLeft();
  float GetProgressBarWidth();
  double CalculatePercentage(float client_x);
  Touch* GetActiveTouch(TouchEvent* event);

  double CalculatePercentageFromDrag(float client_x);
  
  base::WeakPtr<MediaRemotingInterstitial> weak_ptr_;
  WeakMember<Element> progress_bar_;
  bool is_dragging_;
  float drag_start_x_;
  float drag_start_percentage_;
  bool is_touch_dragging_;
  int active_touch_id_;
};
} // namespace blink

#endif // THIRD_PARTY_BLINK_RENDERER_CORE_MEDIA_REMOTING_LISTENER_H_