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

#ifndef CONTENT_BROWSER_RENDERER_HOST_RENDER_WIDGET_HOST_VIEW_CHILD_FRAME_EXT_H_
#define CONTENT_BROWSER_RENDERER_HOST_RENDER_WIDGET_HOST_VIEW_CHILD_FRAME_EXT_H_

#include "content/browser/renderer_host/render_widget_host_view_child_frame.h"
#include "third_party/blink/public/common/input/web_input_event.h"

namespace content {
class CrossProcessFrameConnector;
class RenderWidgetHost;
class RenderWidgetHostViewChildFrameTest;
class TouchSelectionControllerClientChildFrame;

class CONTENT_EXPORT RenderWidgetHostViewChildFrameExt
    : public RenderWidgetHostViewChildFrame {
 public:
  RenderWidgetHostViewChildFrameExt(
      RenderWidgetHost* widget_host,
      const display::ScreenInfos& parent_screen_infos);

  RenderWidgetHostViewChildFrameExt(const RenderWidgetHostViewChildFrameExt&) =
      delete;
  RenderWidgetHostViewChildFrameExt& operator=(
      const RenderWidgetHostViewChildFrameExt&) = delete;

  RenderWidgetHostViewChildFrameExt* AsWebRenderWidgetHostViewChildFrameExt()
      override {
    return this;
  }
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  bool GetScrollable() override;
  bool IsMarkedConsumed(blink::WebInputEvent::Type type);
#endif
};
}  // namespace content
#endif  // CONTENT_BROWSER_RENDERER_HOST_RENDER_WIDGET_HOST_VIEW_CHILD_FRAME_EXT_H_
