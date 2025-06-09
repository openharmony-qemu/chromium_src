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

#include "ohos/adapter/xcomponent/event/gesture_event_common.h"

#include <js_native_api.h>
#include <js_native_api_types.h>
#include <string>

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/common/trace.h"
#include "ohos/adapter/xcomponent/xcomponent_manager.h"

namespace ohos::adapter::xcomponent {

void OnPanEventCB(const int action_type,
                  const std::string& xcomponent_id,
                  const aki::Value gesture_event) {
  TRACE_EVENT_0("OnPanEventCB");
  PanEvent event;
  if (!(gesture_event.IsUndefined() || gesture_event.IsNull())) {
    event.offset_x = gesture_event["offsetX"].As<float>();
    event.offset_y = gesture_event["offsetY"].As<float>();
    event.velocity_x =
        static_cast<float>(gesture_event["velocityX"].As<double>());
    event.velocity_y =
        static_cast<float>(gesture_event["velocityY"].As<double>());
    event.source_tool =
        gesture_event["sourceTool"].As<GestureEventSourceTool>();
  }
  if (event.source_tool == GestureEventSourceTool::kUnknown) {
    // skip unknow source tool which trigger by long pressing and moving mouse,
    // because it will conflict with scroll bar action
    return;
  }
  auto render = XComponentManager::GetInstance()->GetXComponent(xcomponent_id);
  if (render == nullptr) {
    LOGE("Get plugin render: %{public}s", xcomponent_id.c_str());
    return;
  }
  PanAction action = static_cast<PanAction>(action_type);
  render->OnPanEvent(action, event);
}

void OnPinchEventCB(const std::string& pinch_step,
                    const std::string& xcomponent_id,
                    const aki::Value gesture_event) {
  TRACE_EVENT_0("OnPinchEventCB");
  GestureEventSourceType source_type =
      gesture_event["source"].As<GestureEventSourceType>();
  if (source_type == GestureEventSourceType::kTouchScreen) {
    // pinch gesture should be skipped on touch screen, because it will be
    // detected by multi touch gesture.
    return;
  }

  PinchEvent pinch_event;
  pinch_event.offset_x = gesture_event["pinchCenterX"].As<float>();
  pinch_event.offset_y = gesture_event["pinchCenterY"].As<float>();
  pinch_event.scale = gesture_event["scale"].As<float>();

  auto render = XComponentManager::GetInstance()->GetXComponent(xcomponent_id);
  if (render == nullptr) {
    LOGE("[pinch-event]can not get render: %{public}s", xcomponent_id.c_str());
    return;
  }
  render->OnPinchEvent(pinch_step, pinch_event);
}

JSBIND_GLOBAL() {
  JSBIND_FUNCTION(OnPanEventCB);
  JSBIND_FUNCTION(OnPinchEventCB);
}

}  // namespace ohos::adapter::xcomponent
