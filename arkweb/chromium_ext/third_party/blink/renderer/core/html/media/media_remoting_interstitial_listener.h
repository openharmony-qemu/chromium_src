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
  RemotingButtonEventListener(base::WeakPtr<MediaRemotingInterstitial> media_remoting_interstitial, RemotingButtonType type)
      : media_remoting_interstitial_(media_remoting_interstitial), button_type_(type){}

  void Invoke(ExecutionContext* execution_context, Event* event) override {
    if (event->type() == event_type_names::kClick) {
      HandleClick(event);
    }
  }

 private:
  void HandleClick(Event* event) {
    event->stopPropagation();
    event->preventDefault();

    switch (button_type_) {
      case RemotingButtonType::kStopCasting:
        media_remoting_interstitial_->OnStopCastingClicked();
        break;
      case RemotingButtonType::kSwitchDevice:
        media_remoting_interstitial_->OnSwitchDeviceClicked();
        break;
      case RemotingButtonType::kPlayPause:
        // TODO 如果isplaying 是true，要通知远端播放；否则暂停
        LOG(INFO) << "lmh2 RemotingButtonEventListener : public NativeEventListener GetPlayingState: " << media_remoting_interstitial_->GetPlayingState();
        media_remoting_interstitial_->UpdateRemotePlayState(!media_remoting_interstitial_->GetPlayingState());
        media_remoting_interstitial_->OnPlayPauseClicked();
        break;
      case RemotingButtonType::kFullscreenToggle:
        media_remoting_interstitial_->OnFullscreenClicked();
        break;
    }
  }

  base::WeakPtr<MediaRemotingInterstitial> media_remoting_interstitial_;
  RemotingButtonType button_type_;
};

// media_remoting_interstitial.h 中添加

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
  // 事件处理函数
  void HandleMouseClick(MouseEvent* event);
  void HandleMouseDown(MouseEvent* event);
  void HandleMouseMove(MouseEvent* event);
  void HandleMouseUp(MouseEvent* event);
  void HandleMouseLeave(MouseEvent* event);

  // 触摸事件处理
  void HandleTouchStart(TouchEvent* event);
  void HandleTouchMove(TouchEvent* event);
  void HandleTouchEnd(TouchEvent* event);
  void HandleTouchCancel(TouchEvent* event);
  
  // 通用处理函数
  void StartDragging(float start_x, float client_x);
  void UpdateDragging(float client_x);
  void EndDragging(float client_x);

  // 辅助函数
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

  // 触摸拖动状态
  bool is_touch_dragging_;
  int active_touch_id_;
};
} // namespace blink

#endif // THIRD_PARTY_BLINK_RENDERER_CORE_MEDIA_REMOTING_LISTENER_H_