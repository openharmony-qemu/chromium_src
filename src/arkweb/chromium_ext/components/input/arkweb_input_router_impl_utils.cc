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

#include "arkweb_input_router_impl_utils.h"

#include "components/input/input_router_impl.h"

namespace input {

using blink::WebInputEvent;

ArkwebInputRouterImplUtils::ArkwebInputRouterImplUtils(
    InputRouterImpl* input_router_impl)
    : input_router_impl_(input_router_impl) {
  DCHECK(input_router_impl);
}

ArkwebInputRouterImplUtils::~ArkwebInputRouterImplUtils() = default;

void ArkwebInputRouterImplUtils::SendGestureEventEx(
    GestureEventWithLatencyInfo& gesture_event) {
  timeStamp_ = ::base::subtle::TimeTicksNowIgnoringOverride()
                   .since_origin()
                   .InNanoseconds();
  if (gesture_event.event.GetType() ==
          WebInputEvent::Type::kGestureScrollUpdate &&
      timeStamp_ - prePerfTimeStamp_ > GESTURE_MOVE_PERIOD) {
#if BUILDFLAG(ARKWEB_PERFORMANCE_INC_FREQ)
    prePerfTimeStamp_ = timeStamp_;
    LOG(INFO) << "InputRouterImpl::SendGestureEvent type=kGestureScrollUpdate "
                 "success";
    input_router_impl_->client_->GetWidgetInputHandler()->TryStartFling();
    if (base::ohos::IsMobileDevice()) {
      OHOS::NWeb::OhosAdapterHelper::GetInstance()
          .CreateSocPerfClientAdapter()
          ->ApplySocPerfConfigByIdEx(SOC_PERF_SLIDE_NORMAL_CONFIG_ID, true);
    }
#endif
  } else if (gesture_event.event.GetType() ==
             WebInputEvent::Type::kGestureScrollEnd) {
    LOG(INFO) << "InputRouterImpl::SendGestureEvent type=kGestureScrollEnd";
    input_router_impl_->client_->GetWidgetInputHandler()->TryFinishFling();
    if (base::ohos::IsMobileDevice()) {
      OHOS::NWeb::OhosAdapterHelper::GetInstance()
          .CreateSocPerfClientAdapter()
          ->ApplySocPerfConfigByIdEx(SOC_PERF_SLIDE_NORMAL_CONFIG_ID, false);
    }
    prePerfTimeStamp_ = 0;

#if BUILDFLAG(ARKWEB_SLIDE_LTPO)
    if (auto* host = content::GpuProcessHost::Get()) {
      if (auto* host_impl = host->gpu_host()) {
        host_impl->StopMonitor();
      }
    }
#endif  // BUILDFLAG(WEB_SLIDE_LTPO)
  }
}

void ArkwebInputRouterImplUtils::TracingAndSceneReport(
    const WebInputEvent& input_event,
    const ui::LatencyInfo& latency_info) {
#if BUILDFLAG(ARKWEB_DFX_TRACING)
  OHOS_TRACE_EVENT1("input", "InputRouterImpl::FilterAndSendWebInputEvent",
                    "type", WebInputEvent::GetName(input_event.GetType()));
  OHOS_TRACE_EVENT2("input,benchmark,devtools.timeline,latencyInfo",
                    "LatencyInfo.Flow", "trace_id",
                    std::to_string(latency_info.trace_id()), "step",
                    "STEP_SEND_INPUT_EVENT_UI");
  if (input_event.GetType() == WebInputEvent::Type::kGestureScrollUpdate) {
    OHOS::NWeb::ResSchedClientAdapter::ReportScene(
        OHOS::NWeb::ResSchedStatusAdapter::WEB_SCENE_ENTER,
        OHOS::NWeb::ResSchedSceneAdapter::SLIDE);
  }
#endif
}

void ArkwebInputRouterImplUtils::ProcessFilteredEvent(
    const blink::WebInputEvent& input_event) {
    if (!(input_event.GetType() == blink::WebInputEvent::Type::kGestureScrollUpdate ||
          input_event.GetType() == blink::WebInputEvent::Type::kTouchMove ||
          input_event.GetType() == blink::WebInputEvent::Type::kGesturePinchUpdate)) {
        LOG(INFO) << "InputRouterImpl::FilterAndSendWebInputEvent type="
                  << blink::InputEventOhos::GetWebEventName(input_event);
    }
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  if (input_event.GetType() == WebInputEvent::Type::kTouchStart) {
    input_router_impl_->native_result_ = false;
  }
  if (input_event.GetType() == WebInputEvent::Type::kMouseDown) {
    input_router_impl_->mouse_native_result_ = false;
  }
#endif
}

}  // namespace input
