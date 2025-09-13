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

#ifndef ARKWEB_INPUT_ROUTER_IMPL_UTILS_H_
#define ARKWEB_INPUT_ROUTER_IMPL_UTILS_H_

#include <string>

#include "arkweb/build/features/features.h"
#include "arkweb_input_router_impl_ext.h"
#include "base/ohos/sys_info_utils_ext.h"
#include "components/input/input_event_ack_state.h"
#include "third_party/blink/public/common/input/web_input_event.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/res_sched_client_adapter.h"
#if BUILDFLAG(ARKWEB_SLIDE_LTPO)
#include "base/ohos/ltpo/include/sliding_observer.h"
#include "content/browser/gpu/gpu_process_host.h"
#endif
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
#include "arkweb/chromium_ext/third_party/blink/common/event/input_event_ohos.h"
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
constexpr uint64_t GESTURE_MOVE_PERIOD = 250000000;
const int SOC_PERF_SLIDE_NORMAL_CONFIG_ID = 10025;
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

namespace input {

class InputRouterImpl;

class ArkwebInputRouterImplUtils {
 public:
  friend class InputRouterImpl;
  explicit ArkwebInputRouterImplUtils(InputRouterImpl* input_router_impl);
  ~ArkwebInputRouterImplUtils();

  ArkwebInputRouterImplUtils(const ArkwebInputRouterImplUtils&) = delete;
  ArkwebInputRouterImplUtils& operator=(const ArkwebInputRouterImplUtils&) =
      delete;

 private:
  const raw_ptr<InputRouterImpl> input_router_impl_;
  int64_t timeStamp_ = 0;
  int64_t prePerfTimeStamp_ = 0;
  void SendGestureEventEx(GestureEventWithLatencyInfo& gesture_event);
  void TracingAndSceneReport(const blink::WebInputEvent& input_event,
                             const ui::LatencyInfo& latency_info);
  void ProcessFilteredEvent(
      const blink::WebInputEvent& input_event);
};
}  // namespace input

#endif  // ARKWEB_INPUT_ROUTER_IMPL_UTILS_H_
