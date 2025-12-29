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

#include "arkweb/build/features/features.h"

namespace blink {

#if BUILDFLAG(ARKWEB_MENU)
void FrameWidgetInputHandlerImpl::SelectRangeV2(const gfx::Point& position,
                                                bool is_base) {
  RunOnMainThread(base::BindOnce(
      [](base::WeakPtr<WidgetBase> widget,
         base::WeakPtr<mojom::blink::FrameWidgetInputHandler> handler,
         const gfx::Point& position, bool is_base) {
        DCHECK_EQ(!!widget, !!handler);
        if (!widget || !handler) {
          return;
        }
        HandlingState handling_state(widget, UpdateState::kIsSelectingRange);
        handler->SelectRangeV2(position, is_base);
      },
      widget_, main_thread_frame_widget_input_handler_, position, is_base));
}
#endif

// LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
void FrameWidgetInputHandlerImpl::ShowFreeCopyMenu() {
  RunOnMainThread(base::BindOnce(
      [](base::WeakPtr<WidgetBase> widget,
         base::WeakPtr<mojom::blink::FrameWidgetInputHandler> handler) {
        LOG(INFO) << "Entering ShowFreeCopyMenu";
        DCHECK_EQ(!!widget, !!handler);
        if (!widget || !handler) {
          LOG(INFO) << "Widget is null, returning early";
          return;
        }
        HandlingState handling_state(widget, UpdateState::kIsSelectingRange);
        handler->ShowFreeCopyMenu();
      },
      widget_, main_thread_frame_widget_input_handler_));
}
#endif

#if BUILDFLAG(ARKWEB_AI)
void FrameWidgetInputHandlerImpl::OnDataDetectorSelectText() {
  RunOnMainThread(base::BindOnce(
      [](base::WeakPtr<WidgetBase> widget,
         base::WeakPtr<mojom::blink::FrameWidgetInputHandler> handler) {
        LOG(INFO) << "Entering OnDataDetectorSelectText";
        DCHECK_EQ(!!widget, !!handler);
        if (!widget || !handler) {
          LOG(INFO) << "Widget is null, returning early";
          return;
        }
        HandlingState handling_state(widget, UpdateState::kIsSelectingRange);
        handler->OnDataDetectorSelectText();
      },
      widget_, main_thread_frame_widget_input_handler_));
}
#endif  // BUILDFLAG(ARKWEB_AI)
// LCOV_EXCL_STOP
}  // namespace blink
