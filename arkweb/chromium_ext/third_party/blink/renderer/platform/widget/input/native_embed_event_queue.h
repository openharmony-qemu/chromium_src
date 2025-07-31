// Copyright 2016 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_WIDGET_INPUT_NATIVE_EMBED_EVENT_QUEUE_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_WIDGET_INPUT_NATIVE_EMBED_EVENT_QUEUE_H_

#include <memory>

#include "base/containers/circular_deque.h"
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/widget/input/event_with_callback.h"

namespace blink {

class PLATFORM_EXPORT NativeEmbedEventQueue {
 public:
  NativeEmbedEventQueue();
  NativeEmbedEventQueue(const NativeEmbedEventQueue&) = delete;
  NativeEmbedEventQueue& operator=(const NativeEmbedEventQueue&) = delete;
  ~NativeEmbedEventQueue();

  // Adds an event to the queue. The event may be coalesced with the last event.
  void Queue(std::unique_ptr<EventWithCallback> event);

  std::unique_ptr<EventWithCallback> Pop();

  bool empty() const { return queue_.empty(); }

  size_t size() const { return queue_.size(); }

  const WebInputEvent& Front();

  void SetStatus(InputHandlerProxyUtils::NativeEventState status) {
    status_ = status;
  }

  int GetStatus() const { return status_; }

  void SetLayerId(int32_t layer_id) { layer_id_ = layer_id; }

  int32_t GetLayerId() const { return layer_id_; }

  void PushFront(std::unique_ptr<EventWithCallback> event);

 private:
  using EventQueue = base::circular_deque<std::unique_ptr<EventWithCallback>>;
  EventQueue queue_;
  InputHandlerProxyUtils::NativeEventState status_ =
      InputHandlerProxyUtils::NativeEventState::INIT;
  int32_t layer_id_ = 0;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_WIDGET_INPUT_NATIVE_EMBED_EVENT_QUEUE_H_
