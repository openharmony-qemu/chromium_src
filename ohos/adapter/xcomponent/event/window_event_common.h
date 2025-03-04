// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_XCOMPONENT_EVENT_WINDOW_EVENT_COMMON_H_
#define OHOS_ADAPTER_XCOMPONENT_EVENT_WINDOW_EVENT_COMMON_H_

#include "ohos/adapter/export.h"

namespace ohos::adapter::xcomponent {

enum EventType : int32_t {
  ET_UNKNOWN = -1,
  ET_SURFACE_CHANGE = 0,
  ET_SURFACE_FOCUS = 1,
  ET_SURFACE_BLUR = 2,
  ET_WINDOW_SIZE_CHANGE = 3,
  ET_WINDOW_CHANGE = 4,
  ET_WINDOW_RECT_CHANGE = 5,
};

enum class WindowEventType {
  WINDOW_SHOWN = 1,
  WINDOW_ACTIVE = 2,
  WINDOW_INACTIVE = 3,
  WINDOW_HIDDEN = 4,
  WINDOW_OCCLUDED = 5,
  WINDOW_VISIBLE = 6,
  WINDOW_DESTROYED = 7,
};

enum class WindowStatusType {
  UNDEFINED = 0,
  FULL_SCREEN,
  MAXIMIZE,
  MINIMIZE,
  FLOATING,
  SPLIT_SCREEN
};

enum class RectChangeReason {
  UNDEFINED = 0,
  MAXIMIZE,
  RECOVER,
  MOVE,
  DRAG,
  DRAG_START,
  DRAG_END
};

class ADAPTER_EXPORT_API Event {
 public:
  explicit Event(EventType type) : type_(type) {}
  virtual ~Event() {}
  virtual EventType type() { return type_; }

 private:
  EventType type_;
};

class ADAPTER_EXPORT_API SurfaceEvent : public Event {
 public:
  explicit SurfaceEvent(EventType type) : Event(type) {}
  uint64_t width = 0;
  uint64_t height = 0;
};

class ADAPTER_EXPORT_API WindowRectChangeEvent : public Event {
 public:
  explicit WindowRectChangeEvent() : Event(EventType::ET_WINDOW_RECT_CHANGE) {}
  int top = 0;
  int left = 0;
  uint64_t width = 0;
  uint64_t height = 0;
  RectChangeReason reason = RectChangeReason::UNDEFINED;
};

class ADAPTER_EXPORT_API WindowSizeChangeEvent : public Event {
 public:
  explicit WindowSizeChangeEvent() : Event(EventType::ET_WINDOW_SIZE_CHANGE) {}
  int top = 0;
  int left = 0;
  uint64_t width = 0;
  uint64_t height = 0;
};

class ADAPTER_EXPORT_API WindowEvent : public Event {
 public:
  explicit WindowEvent(WindowEventType type)
      : Event(EventType::ET_WINDOW_CHANGE), window_event_type_(type) {}
  const WindowEventType window_event_type_;
};

}  // namespace ohos::adapter::xcomponent

using WindowEventCallBack =
    std::function<void(int32_t,
                       std::shared_ptr<ohos::adapter::xcomponent::Event>)>;

#endif  // OHOS_ADAPTER_XCOMPONENT_EVENT_WINDOW_EVENT_COMMON_H_
