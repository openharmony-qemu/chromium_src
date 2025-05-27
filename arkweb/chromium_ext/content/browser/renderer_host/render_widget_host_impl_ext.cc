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
 
#include "arkweb/chromium_ext/content/browser/renderer_host/render_widget_host_impl_ext.h"
#include "content/browser/renderer_host/dip_util.h"
#include "content/browser/renderer_host/render_view_host_delegate_view.h"
#include "arkweb/chromium_ext/third_party/blink/public/mojom/page/text_recognize_result.mojom.h"

#if BUILDFLAG(ARKWEB_SLIDE_LTPO)
#include "base/ohos/ltpo/include/sliding_observer.h"
#include "content/browser/gpu/gpu_process_host.h"
#endif
#if BUILDFLAG(ARKWEB_RENDERER_ANR_DUMP)
#include "base/ohos/sys_info_utils_ext.h"
#include "content/public/browser/web_contents_delegate.h"
#endif

namespace content {

#if BUILDFLAG(IS_OHOS)
// One pixel of the picture is made up of four bits of binary data.
constexpr int kImagePixelMap = 4;
#endif

RenderWidgetHostImplExt::RenderWidgetHostImplExt(
    FrameTree* frame_tree,
    bool self_owned,
    viz::FrameSinkId frame_sink_id,
    RenderWidgetHostDelegate* delegate,
    base::SafeRef<SiteInstanceGroup> site_instance_group,
    int32_t routing_id,
    bool hidden,
    bool renderer_initiated_creation,
    std::unique_ptr<FrameTokenMessageQueue> frame_token_message_queue)
    : RenderWidgetHostImpl(frame_tree,
                           self_owned,
                           frame_sink_id,
                           delegate,
                           site_instance_group,
                           routing_id,
                           hidden,
                           renderer_initiated_creation,
                           std::move(frame_token_message_queue)) {}

#if BUILDFLAG(ARKWEB_CLIPBOARD)
void RenderWidgetHostImplExt::ForwardTouchEventWithLatencyInfo(
    const blink::WebTouchEvent& touch_event,
    const ui::LatencyInfo& latency) {}
#endif

#if BUILDFLAG(IS_OHOS)
void RenderWidgetHostImplExt::CreateOverlay(const SkBitmap& bitmap,
                                            const gfx::Rect& image_rect,
                                            const gfx::Point& touch_point) {
  RenderViewHostDelegateView* view = delegate_->GetDelegateView();
  float scale = GetScaleFactorForView(GetView());
  gfx::ImageSkia image = gfx::ImageSkia::CreateFromBitmap(bitmap, scale);
  view->CreateOverlay(image, image_rect, touch_point);
}
#endif
 
#if BUILDFLAG(ARKWEB_AI)
void RenderWidgetHostImplExt::GetWordSelection(const std::string& text,
                                               int8_t offset,
                                               GetWordSelectionCallback callback) {
  if (!view_) {
    return;
  }
  std::vector<int8_t> select = view_->GetWordSelection(text, offset);
  std::move(callback).Run(select);
}
 
gfx::Rect RenderWidgetHostImplExt::GetImageRect() {
  gfx::Rect image_rect;
  blink_frame_widget_->GetImageRect(&image_rect);
  return image_rect;
}
 
void RenderWidgetHostImplExt::OnTextSelected(bool flag) {
  blink_frame_widget_->OnTextSelected(flag);
}
 
void RenderWidgetHostImplExt::OnDestroyImageAnalyzerOverlay() {
  blink_frame_widget_->OnDestroyImageAnalyzerOverlay();
}
 
void RenderWidgetHostImplExt::OnFoldStatusChanged(uint32_t foldstatus) {
  blink_frame_widget_->OnFoldStatusChanged(foldstatus);
}
 
void RenderWidgetHostImplExt::NotifyOverlayStateChanged() {
  blink_frame_widget_->NotifyOverlayStateChanged();
}
 
void RenderWidgetHostImplExt::OnOverlayStateChanged(const gfx::Rect& image_rect) {
  RenderViewHostDelegateView* view = delegate_->GetDelegateView();
  if (!view || !GetView()) {
    OnDestroyImageAnalyzerOverlay();
    return;
  }
  view->OnOverlayStateChanged(image_rect);
}

void RenderWidgetHostImplExt::OnDataDetectorSelectText() {
  blink_frame_widget_->OnDataDetectorSelectText();
}
#endif
 
#if BUILDFLAG(ARKWEB_DRAG_DROP)
void RenderWidgetHostImplExt::GetVisibleRectToWeb(
    GetVisibleRectToWebCallback callback) {
  RenderViewHostDelegateView* view = delegate_->GetDelegateView();
  if (!view || !GetView()) {
    std::move(callback).Run(gfx::Rect());
    return;
  }
  auto rect = view->GetVisibleRectToWeb();
  std::move(callback).Run(rect);
}
#endif

#if BUILDFLAG(ARKWEB_SLIDE_LTPO)
void RenderWidgetHostImplExt::ReportSlidingFrameRate(
    const blink::WebGestureEvent& gesture_event) {
  int32_t preferred_frame_rate = 0;
  if (gesture_event.GetType() == blink::WebInputEvent::Type::kGestureScrollBegin) {
    base::ohos::SlidingObserver::GetInstance().StartSliding();
  } else if (gesture_event.GetType() ==
             blink::WebInputEvent::Type::kGestureScrollEnd) {
    preferred_frame_rate =
        base::ohos::SlidingObserver::GetInstance().StopSliding();
  } else if (gesture_event.GetType() ==
             blink::WebInputEvent::Type::kGestureScrollUpdate) {
    preferred_frame_rate =
        base::ohos::SlidingObserver::GetInstance().OnScrollUpdate(
            gesture_event.data.scroll_update.delta_x,
            gesture_event.data.scroll_update.delta_y);
  }

  auto* host = GpuProcessHost::Get();
  viz::GpuHostImpl* host_impl = nullptr;
  if (host) {
    host_impl = host->gpu_host();
  }

  if (host_impl && preferred_frame_rate >= 0) {
    if (gesture_event.GetType() == blink::WebInputEvent::Type::kGestureScrollEnd ||
        gesture_event.GetType() == blink::WebInputEvent::Type::kGestureScrollUpdate) {
      host_impl->ReportSlidingFrameRate(preferred_frame_rate);
    }
  }
}
#endif

#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
void RenderWidgetHostImplExt::DynamicFrameLossEvent(const std::string& sceneId,
                                                    bool isStart) {
  if (view_) {
    view_->DynamicFrameLossEvent(sceneId, isStart);
  }
}
#endif

#if BUILDFLAG(ARKWEB_SAME_LAYER)
void RenderWidgetHostImplExt::DidNativeEmbedEvent(
    blink::mojom::NativeEmbedTouchEventPtr touchEvent) {
  if (view_) {
    view_->DidNativeEmbedEvent(touchEvent);
  }
}

void RenderWidgetHostImplExt::DidNativeEmbedMouseEvent(
      blink::mojom::NativeEmbedMouseEventPtr mouseEvent) {
  if (view_) {
    view_->DidNativeEmbedMouseEvent(mouseEvent);
  }
}
#endif

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
void RenderWidgetHostImplExt::SendCurrentLanguage(const std::string& ans) {
  if (view_) {
    view_->SendCurrentLanguage(ans);
  }
}
#endif

}  // namespace content
