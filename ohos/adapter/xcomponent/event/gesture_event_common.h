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

#ifndef OHOS_ADAPTER_XCOMPONENT_EVENT_GESTURE_EVENT_COMMON_H_
#define OHOS_ADAPTER_XCOMPONENT_EVENT_GESTURE_EVENT_COMMON_H_

#include <cstdint>

namespace ohos::adapter::xcomponent {

enum class GestureEventSourceType : int32_t {
  kUnknown,
  kMouse,
  kTouchScreen,
};

enum class GestureEventSourceTool : int32_t {
  kUnknown,
  kFinger,
  kPen,
  kRubber,
  kBrush,
  kPencil,
  kAirBrush,
  kMouse,
  kLens,
  kTouchpad,
};

enum class PanAction {
  kStart = 0,
  kUpdate,
  kEnd,
  kCancel,
};

struct BaseGestureEvent {
  GestureEventSourceTool source_tool{GestureEventSourceTool::kUnknown};
};

struct PanEvent : public BaseGestureEvent {
  float offset_x{0.0f};
  float offset_y{0.0f};
  float velocity_x{0.0f};
  float velocity_y{0.0f};
};

struct PinchEvent : public BaseGestureEvent {
  float offset_x{0.0f};
  float offset_y{0.0f};
  float scale{1.0f};
};

}  // namespace ohos::adapter::xcomponent

#endif  // OHOS_ADAPTER_XCOMPONENT_EVENT_GESTURE_EVENT_COMMON_H_
