/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "popup_utils.h"

#include "third_party/blink/public/web/web_local_frame.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/web_frame_widget_impl.h"
#include "third_party/blink/renderer/core/frame/web_local_frame_impl.h"
#include "third_party/blink/renderer/core/page/page_popup_client.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/rect_f.h"

namespace blink {
void PopupUtils::AddAvailRectInWebToData(LocalFrame* frame,
                                         SegmentedBuffer& data) {
  if (frame && frame->View() && frame->View()->GetChromeClient()) {
    gfx::Rect available_rect_in_screen =
        frame->View()->GetChromeClient()->LocalRootToScreenDIPs(
            frame->View()->FrameRect(), frame->View());
    PagePopupClient::AddProperty("availableRectInScreen",
                                 available_rect_in_screen, data);
  }
}
}  // namespace blink