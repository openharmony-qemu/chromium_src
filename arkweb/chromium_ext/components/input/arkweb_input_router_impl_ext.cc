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

#include "arkweb_input_router_impl_ext.h"

namespace input {

ArkwebInputRouterImplExt::ArkwebInputRouterImplExt(
    InputRouterClient* client,
    InputDispositionHandler* disposition_handler,
    FlingControllerSchedulerClient* fling_scheduler_client,
    const Config& config)
    : InputRouterImpl(client,
                      disposition_handler,
                      fling_scheduler_client,
                      config) {}

#if BUILDFLAG(ARKWEB_SAME_LAYER)
void ArkwebInputRouterImplExt::SetGestureEventResult(bool result,
                                                     bool stopPropagation,
                                                     int32_t fingerId) {
  native_result_ = result;
  client_->GetWidgetInputHandler()->SetGestureEventResult(
      result, stopPropagation, fingerId);
}

void ArkwebInputRouterImplExt::SetNativeEmbedMode(bool flag) {
  client_->GetWidgetInputHandler()->SetNativeEmbedMode(flag);
}

void ArkwebInputRouterImplExt::SetMouseEventResult(bool result, bool stopPropagation) {
  mouse_native_result_ = result;
  client_->GetWidgetInputHandler()->SetMouseEventResult(result, stopPropagation);
}

void ArkwebInputRouterImplExt::SetEnableCustomVideoPlayer(bool flag) {
  client_->GetWidgetInputHandler()->SetEnableCustomVideoPlayer(flag);
}
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void ArkwebInputRouterImplExt::ScrollBy(float delta_x, float delta_y) {
  client_->GetWidgetInputHandler()->ScrollBy(delta_x, delta_y);
}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
void ArkwebInputRouterImplExt::SetBypassVsyncCondition(int32_t condition) {
  client_->GetWidgetInputHandler()->SetBypassVsyncCondition(condition);
}
#endif

#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
void ArkwebInputRouterImplExt::DynamicFrameLossEvent(const std::string& sceneId,
                                            bool isStart) {
  return client_->DynamicFrameLossEvent(sceneId, isStart);
}

void ArkwebInputRouterImplExt::SetFocusWebId(int32_t nweb_id) {
  gesture_event_queue_.SetFocusWebId(nweb_id);
}
#endif

}  // namespace input