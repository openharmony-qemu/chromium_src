/* Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "arkweb/chromium_ext/content/browser/renderer_host/render_widget_host_view_child_frame_ext.h"

#include "content/browser/renderer_host/cross_process_frame_connector.h"
namespace content {
RenderWidgetHostViewChildFrameExt::RenderWidgetHostViewChildFrameExt(
    RenderWidgetHost* widget_host,
    const display::ScreenInfos& parent_screen_infos)
    : RenderWidgetHostViewChildFrame(widget_host, parent_screen_infos) {}

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
bool RenderWidgetHostViewChildFrameExt::GetScrollable() {
  if (!frame_connector_) {
    return true;
  }
  auto* root_view = frame_connector_->GetRootRenderWidgetHostView();
  if (root_view && !root_view->GetScrollable()) {
    return false;
  }
  return true;
}

bool RenderWidgetHostViewChildFrameExt::IsMarkedConsumed(
    blink::WebInputEvent::Type type) {
  if (!GetScrollable() &&
      type == blink::WebInputEvent::Type::kGestureScrollUpdate) {
    return true;
  }
  return false;
}

#endif
}  // namespace content
