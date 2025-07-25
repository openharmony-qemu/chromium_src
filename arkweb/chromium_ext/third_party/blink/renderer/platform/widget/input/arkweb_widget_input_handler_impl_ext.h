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

#ifndef ARKWEB_WIDGET_INPUT_HANDLER_IMPL_EXT_H_
#define ARKWEB_WIDGET_INPUT_HANDLER_IMPL_EXT_H_

#include "arkweb/build/features/features.h"
#include "third_party/blink/renderer/platform/widget/input/widget_input_handler_impl.h"

#if BUILDFLAG(ARKWEB_PERFORMANCE_INC_FREQ)
#include "cef/ohos_cef_ext/libcef/common/soc_perf_util.h"
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
#include "arkweb/chromium_ext/third_party/blink/common/event/input_event_ohos.h"
#endif

namespace blink {

class ArkwebWidgetInputHandlerImplExt : public WidgetInputHandlerImpl {
 public:
  friend class WidgetInputHandlerImpl;
  ArkwebWidgetInputHandlerImplExt(
      scoped_refptr<WidgetInputHandlerManager> manager,
      scoped_refptr<MainThreadEventQueue> input_event_queue,
      base::WeakPtr<WidgetBase> widget,
      base::WeakPtr<mojom::blink::FrameWidgetInputHandler>
          frame_widget_input_handler);

  ArkwebWidgetInputHandlerImplExt* AsArkwebWidgetInputHandlerImplExt()
      override {
    return this;
  }

#if BUILDFLAG(ARKWEB_PERFORMANCE_INC_FREQ)
  void TryStartFling() override;
  void TryFinishFling() override;
#endif

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  void AttachSoftwareCompositorOhos(
      mojo::PendingReceiver<mojom::blink::SoftwareCompositorOhos>
          compositor_receiver) override;
#endif
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  void SetGestureEventResult(bool result, bool stopPropagation) override;
  void SetMouseEventResult(bool result, bool stopPropagation) override;
  void SetNativeEmbedMode(bool flag) override;
#endif
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void ScrollBy(float delta_x, float delta_y) override;
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
  void SetBypassVsyncCondition(int32_t condition) override;
#endif
 private:
};

}  // namespace blink

#endif  // ARKWEB_WIDGET_INPUT_HANDLER_IMPL_EXT_H_
