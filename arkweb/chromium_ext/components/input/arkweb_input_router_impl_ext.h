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

#ifndef ARKWEB_INPUT_ROUTER_IMPL_EXT_H_
#define ARKWEB_INPUT_ROUTER_IMPL_EXT_H_

#include "components/input/input_router_impl.h"

namespace input {

class InputRouterImpl;

class COMPONENT_EXPORT(INPUT) ArkwebInputRouterImplExt : public InputRouterImpl {
 public:
  friend class InputRouterImpl;
  ArkwebInputRouterImplExt(
      InputRouterClient* client,
      InputDispositionHandler* disposition_handler,
      FlingControllerSchedulerClient* fling_scheduler_client,
      const Config& config);

  ArkwebInputRouterImplExt* AsArkwebInputRouterImplExt() override {
    return this;
  }

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  void SetGestureEventResult(bool result,
                             bool stopPropagation,
                             int32_t fingerId) override;
  void SetNativeEmbedMode(bool flag) override;
  void SetMouseEventResult(bool result, bool stopPropagation) override;
  void SetEnableCustomVideoPlayer(bool flag) override;
#endif
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void ScrollBy(float delta_x, float delta_y) override;
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
  void SetBypassVsyncCondition(int32_t condition) override;
#endif

#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
  void SetFocusWebId(int32_t nweb_id) override;
#endif

 private:
#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
  void DynamicFrameLossEvent(const std::string& sceneId, bool isStart) override;
#endif
};

}  // namespace input

#endif  // ARKWEB_INPUT_ROUTER_IMPL_EXT_H_
