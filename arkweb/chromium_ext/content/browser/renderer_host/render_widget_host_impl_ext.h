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
 
#ifndef CONTENT_BROWSER_RENDERER_HOST_RENDERE_WIDGET_HOST_IMPL_EXT_H_
#define CONTENT_BROWSER_RENDERER_HOST_RENDERE_WIDGET_HOST_IMPL_EXT_H_

#include "content/browser/renderer_host/render_widget_host_impl.h"

namespace content {
#if BUILDFLAG(ARKWEB_RENDERER_ANR_DUMP)
enum class RendererIsUnresponsiveReason {
  kOnInputEventAckTimeout = 0,
  kRendererAnrInputTimeout = 0,
  kNavigationRequestCommitTimeout = 1,
  kRendererCancellationThrottleTimeout = 2,
  kMaxValue = kRendererCancellationThrottleTimeout,
};
#endif

class RenderWidgetHostImplExt : public RenderWidgetHostImpl {
 public:
  RenderWidgetHostImplExt(
      FrameTree* frame_tree,
      bool self_owned,
      viz::FrameSinkId frame_sink_id,
      RenderWidgetHostDelegate* delegate,
      base::SafeRef<SiteInstanceGroup> site_instance_group,
      int32_t routing_id,
      bool hidden,
      bool renderer_initiated_creation,
      std::unique_ptr<FrameTokenMessageQueue> frame_token_message_queue);
      
  RenderWidgetHostImplExt* AsRenderWidgetHostImplExt() override { return this; }

#if BUILDFLAG(ARKWEB_CLIPBOARD)
  virtual void ForwardTouchEventWithLatencyInfo(
      const blink::WebTouchEvent& touch_event,
      const ui::LatencyInfo& latency);  // Virtual for testing.
#endif

#if BUILDFLAG(ARKWEB_AI)
  void CreateOverlay(const SkBitmap& bitmap,
                     const gfx::Rect& image_rect,
                     const gfx::Point& touch_point) override;
  gfx::Rect GetImageRect();
  void OnTextSelected(bool flag);
  void OnDestroyImageAnalyzerOverlay();
  void OnFoldStatusChanged(uint32_t foldstatus);
  void NotifyOverlayStateChanged();
  void OnOverlayStateChanged(const gfx::Rect& image_rect) override;
  void OnDataDetectorSelectText();
#endif

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
  void SendCurrentLanguage(const std::string& ans) override;
#endif

#if BUILDFLAG(ARKWEB_AI)
  void GetWordSelection(const std::string& text,
                        int8_t offset,
                        GetWordSelectionCallback callback) override;
#endif

#if BUILDFLAG(ARKWEB_DRAG_DROP)
  void GetVisibleRectToWeb(GetVisibleRectToWebCallback callback) override;
#endif

#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
  void DynamicFrameLossEvent(const std::string& sceneId, bool isStart) override;
#endif

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  void DidNativeEmbedEvent(
      blink::mojom::NativeEmbedTouchEventPtr touchEvent) override;
  void DidNativeEmbedMouseEvent(blink::mojom::NativeEmbedMouseEventPtr mouseEvent) override;
#endif

protected:
#if BUILDFLAG(ARKWEB_SLIDE_LTPO)
  void ReportSlidingFrameRate(const blink::WebGestureEvent& gesture_event);
#endif
};
}  // namespace content

#endif
