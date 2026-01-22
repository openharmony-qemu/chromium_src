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
#include "arkweb/chromium_ext/third_party/blink/renderer/core/page/chrome_client_impl_ext.h"

#include "arkweb/ohos_nweb/src/sysevent/event_reporter.h"
#include "third_party/blink/renderer/core/exported/web_view_impl.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/web_frame_widget_impl.h"
#include "third_party/blink/renderer/core/frame/web_local_frame_impl.h"
#include "third_party/blink/renderer/core/html/forms/file_chooser.h"
#include "third_party/blink/renderer/core/paint/paint_layer_scrollable_area.h"
#include "ui/gfx/geometry/rect.h"

namespace blink {

ChromeClientImplExt::ChromeClientImplExt(WebViewImpl* web_view)
    : ChromeClientImpl(web_view) {}

#if BUILDFLAG(ARKWEB_DRAG_DROP)
gfx::Rect ChromeClientImplExt::GetVisibleRectToWeb(LocalFrame* frame) {
  WebLocalFrameImpl* web_frame = WebLocalFrameImpl::FromFrame(frame);
  if (!web_frame || !web_frame->LocalRootFrameWidget()) {
    return gfx::Rect();
  }
  return web_frame->LocalRootFrameWidget()->AsWebFrameWidgetImplExt()->GetVisibleRectToWeb();
}
#endif

#if BUILDFLAG(ARKWEB_FILE_UPLOAD)
// LCOV_EXCL_START
void ChromeClientImplExt::DisconnectClient() {
#if !defined(COMPONENT_BUILD)
  ReportDuplicateFileUpload("Size of file_chooser_queue_ is " + std::to_string(file_chooser_queue_.size()));
#endif
  file_chooser_queue_.front().get()->CloseChooser();
}
// LCOV_EXCL_STOP
#endif

#if BUILDFLAG(ARKWEB_AI)
void ChromeClientImplExt::CreateOverlay(
    LocalFrame* frame,
    const SkBitmap& image,
    const gfx::Point& touch_point,
    GetAbsImageRectCallback get_rect_callback,
    OnTextSelectedCallback callback,
    OnDestroyImageAnalyzerOverlayCallback destroy_callback) {
  WebLocalFrameImpl* web_frame = WebLocalFrameImpl::FromFrame(frame);
  web_frame->LocalRootFrameWidget()->AsWebFrameWidgetImplExt()->CreateOverlay(
      image, touch_point, std::move(get_rect_callback), std::move(callback),
      std::move(destroy_callback));
}

uint32_t ChromeClientImplExt::GetFoldStatus(LocalFrame* frame) {
  WebLocalFrameImpl* web_frame = WebLocalFrameImpl::FromFrame(frame);
  if (web_frame) {
    return web_frame->LocalRootFrameWidget()
        ->AsWebFrameWidgetImplExt()
        ->GetFoldStatus();
  }
  return 0;
}

WTF::Vector<int8_t> ChromeClientImplExt::GetWordSelection(
    LocalFrame* frame,
    const WTF::String& text,
    int8_t offset) {
  WebLocalFrameImpl* web_frame = WebLocalFrameImpl::FromFrame(frame);
  return web_frame->LocalRootFrameWidget()
      ->AsWebFrameWidgetImplExt()
      ->GetWordSelection(text, offset);
}
#endif

#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_AREA)
bool ChromeClientImplExt::SetScrollbarAvoidAreaTop(double margin) {
  LOG(INFO) << " func:" << __FUNCTION__ << " margin:" << margin;
  bool ret = false;
  if (web_view_) {
    LocalFrame* local_frame = web_view_->MainFrameImpl()->GetFrame();
    if (local_frame && local_frame->View()) {
      ret = local_frame->View()
                ->LayoutViewport()
                ->AsPaintLayerScrollableAreaExt()
                ->SetScrollbarAvoidAreaTop(margin);
    }
  }
  return ret;
}

bool ChromeClientImplExt::SetScrollbarAvoidAreaBottom(double margin) {
  LOG(INFO) << " func:" << __FUNCTION__;
  bool ret = false;
  if (web_view_) {
    LocalFrame* local_frame = web_view_->MainFrameImpl()->GetFrame();
    if (local_frame && local_frame->View()) {
      ret = local_frame->View()
                ->LayoutViewport()
                ->AsPaintLayerScrollableAreaExt()
                ->SetScrollbarAvoidAreaBottom(margin);
    }
  }
  return ret;
}
#endif  // ARKWEB_SCROLLBAR_AVOID_AREA
}  // namespace blink
