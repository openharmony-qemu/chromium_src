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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/frame/web_frame_widget_impl_ext.h"

#include "arkweb/build/features/features.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/platform/widget/widget_base_utils.h"
#include "base/command_line.h"
#include "components/translate/core/language_detection/language_detection_util.h"
#include "content/child/child_process.h"
#include "content/public/common/content_switches.h"
#include "arkweb/chromium_ext/third_party/blink/public/mojom/page/text_recognize_result.mojom-blink.h"
#include "third_party/blink/public/web/web_frame_content_dumper.h"
#include "third_party/blink/public/web/web_language_detection_details.h"
#include "third_party/blink/public/web/web_local_frame.h"
#include "third_party/blink/renderer/core/editing/frame_selection.h"
#include "third_party/blink/renderer/core/events/web_input_event_conversion.h"
#include "third_party/blink/renderer/core/exported/web_view_impl.h"
#include "third_party/blink/renderer/core/input/event_handler.h"
#include "third_party/blink/renderer/core/layout/layout_view.h"
#include "third_party/blink/renderer/core/render_mojom/render_mojom_client.h"
#include "third_party/blink/renderer/platform/widget/widget_base.h"
#if BUILDFLAG(ARKWEB_SLIDE_LTPO)
#include "base/ohos/ltpo/include/touch_observer.h"
#endif

#if BUILDFLAG(ARKWEB_SAME_LAYER)
#include "third_party/blink/renderer/core/html/html_native_loader.h"
#include "third_party/blink/renderer/core/html/html_plugin_element.h"
#endif

namespace blink {
const int WORD_CORNER_NUM = 4;
#if BUILDFLAG(ARKWEB_MEDIA_INTERACTION)
constexpr int kDisableDelayTime = 300;
#endif
#if BUILDFLAG(ARKWEB_DFX_TRACING)
constexpr int kDragBlankTime = 80;
#endif

// LCOV_EXCL_START
WebFrameWidgetImplExt::WebFrameWidgetImplExt(
    base::PassKey<WebLocalFrame> pass_key,
    CrossVariantMojoAssociatedRemote<mojom::blink::FrameWidgetHostInterfaceBase>
        frame_widget_host,
    CrossVariantMojoAssociatedReceiver<mojom::blink::FrameWidgetInterfaceBase>
        frame_widget,
    CrossVariantMojoAssociatedRemote<mojom::blink::WidgetHostInterfaceBase>
        widget_host,
    CrossVariantMojoAssociatedReceiver<mojom::blink::WidgetInterfaceBase>
        widget,
    scoped_refptr<base::SingleThreadTaskRunner> task_runner,
    const viz::FrameSinkId& frame_sink_id,
    bool hidden,
    bool never_composited,
    bool is_for_child_local_root,
    bool is_for_nested_main_frame,
    bool is_for_scalable_page)
    : WebFrameWidgetImpl(std::move(pass_key),
                         std::move(frame_widget_host),
                         std::move(frame_widget),
                         std::move(widget_host),
                         std::move(widget),
                         task_runner,
                         frame_sink_id,
                         hidden,
                         never_composited,
                         is_for_child_local_root,
                         is_for_nested_main_frame,
                         is_for_scalable_page) {}

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void WebFrameWidgetImplExt::SetZoomLevel(float magnify_delta,
                                         const gfx::Point& anchor) {
  if (!widget_base_) {
    return;
  }
  widget_base_->utils()->SetZoomLevel(magnify_delta, anchor);
}

void WebFrameWidgetImplExt::SetOverscrollMode(int mode) {
  if (!widget_base_) {
    return;
  }
  widget_base_->utils()->SetOverscrollMode(mode);
}
#endif
// LCOV_EXCL_STOP

void WebFrameWidgetImplExt::ArkWebHandleTouchEvent(
    const WebInputEvent& input_event) {
#if BUILDFLAG(ARKWEB_SLIDE_LTPO)
  if (input_event.GetType() == WebInputEvent::Type::kPointerUp ||
      input_event.GetType() == WebInputEvent::Type::kKeyUp) {
    base::ohos::TouchObserver::GetInstance().SetTouchUpTime(
        ::base::subtle::TimeTicksNowIgnoringOverride()
            .since_origin()
            .InNanoseconds());
  }
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  if (input_event.GetType() == WebInputEvent::Type::kRawKeyDown ||
      input_event.GetType() == WebInputEvent::Type::kMouseUp) {
    auto type = base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
        switches::kProcessType);
    if (type == switches::kRendererProcess) {
      blink::ResSchedReportClient report_client(
          OHOS::NWeb::SocPerfClientAdapter::SOC_PERF_WEB_GESTURE_ID);
      report_client.ApplySocByIdEx(
          OHOS::NWeb::SocPerfClientAdapter::SOC_PERF_WEB_GESTURE_ID, true);
    } else {
      OHOS::NWeb::OhosAdapterHelper::GetInstance()
          .CreateSocPerfClientAdapter()
          ->ApplySocPerfConfigByIdEx(
              OHOS::NWeb::SocPerfClientAdapter::SOC_PERF_WEB_GESTURE_ID, true);
    }
    rawKeyDownTime_++;
    base::SingleThreadTaskRunner::GetCurrentDefault()->PostDelayedTask(
        FROM_HERE,
        WTF::BindOnce(&WebFrameWidgetImplExt::DisableBoost,
                      WrapWeakPersistent(this)),
        base::Milliseconds(kDisableDelayTime));
  }
#endif
}

// LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_MENU)
void WebFrameWidgetImplExt::SelectRangeV2(const gfx::Point& position,
                                          bool is_base) {
  WebLocalFrame* focused_frame = FocusedWebLocalFrameInWidget();
  if (!focused_frame) {
    return;
  }
  focused_frame->SelectRangeV2(widget_base_->DIPsToRoundedBlinkSpace(position),
                               is_base);
}
#endif

#if BUILDFLAG(ARKWEB_DRAG_DROP)
gfx::Rect WebFrameWidgetImplExt::GetVisibleRectToWeb() {
  gfx::Rect rect;
  if (auto host = GetAssociatedFrameWidgetHost(); host) {
    host->GetVisibleRectToWeb(&rect);
  }
  return rect;
}
#endif

#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
gfx::Vector2dF WebFrameWidgetImplExt::GetOverScrollOffset() {
  gfx::Vector2dF overscroll_offset;
  overscroll_offset.set_x(0.0f);
  overscroll_offset.set_y(0.0f);
  if (!widget_base_) {
    return overscroll_offset;
  }
  return widget_base_->utils()->GetOverScrollOffset();
}

void WebFrameWidgetImplExt::OnOverScrollOffsetChanged(float offset_x,
                                                      float offset_y) {
  if (local_root_ && local_root_->GetFrame()) {
    local_root_->GetFrame()->OnOverScrollOffsetChanged(offset_x, offset_y);
  }
}
#endif
// LCOV_EXCL_STOP

#if BUILDFLAG(ARKWEB_AI)
// LCOV_EXCL_START
void WebFrameWidgetImplExt::CreateOverlay(
    const SkBitmap& image,
    const gfx::Point& touch_point,
    GetAbsImageRectCallback get_rect_callback,
    OnTextSelectedCallback callback,
    OnDestroyImageAnalyzerOverlayCallback destroy_callback) {
  get_rect_callback_ = std::move(get_rect_callback);
  on_text_selected_callback_ = std::move(callback);
  on_destroy_image_overlay_callback_ = std::move(destroy_callback);
  auto image_rect = GetImageRectInner();
  if (image_rect.IsEmpty()) {
    LOG(ERROR) << "CreateOverlay failed: image_rect is empty.";
    return;
  }
  GetAssociatedFrameWidgetHost()->CreateOverlay(image, image_rect, touch_point);
}
// LCOV_EXCL_STOP

void WebFrameWidgetImplExt::OnTextRecognized(
    WTF::Vector<mojom::blink::TextRecognizeResultPtr> res,
    float scale) {
  std::vector<String> text_lines;
  std::vector<gfx::PointF> pointfs;
  for (const auto& line : res) {
    text_lines.push_back(line.get()->raw_value);
    for (int i = 0; i < WORD_CORNER_NUM; i++) {
      float x = line.get()->corner_points[i].x();
      float y = line.get()->corner_points[i].y();
      gfx::PointF pointf = gfx::PointF(x, y);
      pointfs.push_back(pointf);
    }
  }
  if (on_text_recognize_callback_) {
    on_text_recognize_callback_.Run(text_lines, pointfs, scale);
  }
}
#endif

#if BUILDFLAG(ARKWEB_AI)
WTF::Vector<int8_t> WebFrameWidgetImplExt::GetWordSelection(
    const WTF::String& text,
    int8_t offset) {
  WTF::Vector<int8_t> select;
  if (text.IsNull() || !GetAssociatedFrameWidgetHost()->GetWordSelection(
                           text, offset, &select)) {
    select = {-1, -1};
  }
  return select;
}

// LCOV_EXCL_START
void WebFrameWidgetImplExt::OnTextSelected(bool flag) {
  if (on_text_selected_callback_) {
    on_text_selected_callback_.Run(flag);
  }
}

void WebFrameWidgetImplExt::OnDestroyImageAnalyzerOverlay() {
  if (on_destroy_image_overlay_callback_) {
    on_destroy_image_overlay_callback_.Run();
  }
}

uint32_t WebFrameWidgetImplExt::GetFoldStatus() {
  return fold_status_;
}

void WebFrameWidgetImplExt::OnFoldStatusChanged(uint32_t foldstatus) {
  fold_status_ = foldstatus;
}

void WebFrameWidgetImplExt::NotifyOverlayStateChanged() {
  GetAssociatedFrameWidgetHost()->OnOverlayStateChanged(GetImageRectInner());
}

void WebFrameWidgetImplExt::OnDataDetectorSelectText() {
  LOG(DEBUG) << "WebFrameWidgetImplExt::OnDataDetectorSelectText";
  WebLocalFrame* local_frame = FocusedWebLocalFrameInWidget();
  if (!local_frame) {
    return;
  }
  local_frame->OnDataDetectorSelectText();
}

void WebFrameWidgetImplExt::GetImageRect(GetImageRectCallback callback) {
  std::move(callback).Run(GetImageRectInner());
}

gfx::Rect WebFrameWidgetImplExt::GetImageRectInner() {
  auto abs_rect = gfx::RectF();
  if (get_rect_callback_) {
    get_rect_callback_.Run(abs_rect);
  }
  LOG(INFO) << "GetImageRect abs_rect: " << abs_rect.ToString();
  if (!abs_rect.IsEmpty()) {
    LocalFrame* frame = LocalRootImpl()->GetFrame();
    LocalFrameView* view = frame->View();
    auto quad_rect = gfx::Rect(ToFlooredPoint(abs_rect.origin()),
                               ToFlooredSize(abs_rect.size()));
    auto rel_rect = view->FrameToScreen(quad_rect);
    if (!widget_base_) {
      return gfx::Rect();
    }
    float dsf = widget_base_->GetScreenInfo().device_scale_factor;
    float scale = PageScaleInMainFrame();
    auto calc_rect = gfx::Rect(ScaleToFlooredPoint(rel_rect.origin(), dsf),
                               ScaleToFlooredSize(quad_rect.size(), scale));
    return calc_rect;
  } else {
    LOG(ERROR) << "GetImageRect failed: abs_rect is empty.";
    return gfx::Rect();
  }
}
// LCOV_EXCL_STOP
#endif

// LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_MENU)
void WebFrameWidgetImplExt::RegisterClippedVisualViewportSelectionBounds(
    gfx::Rect clipped_selection_bounds) {
  if (!View()->does_composite()) {
    return;
  }
  widget_base_->LayerTreeHost()
      ->AsLayerTreeHostExt()
      ->RegisterClippedVisualViewportSelectionBounds(clipped_selection_bounds);
}

void WebFrameWidgetImplExt::CleanFocusCache() {
  const LocalFrame* local_frame = FocusedLocalFrameInWidget();
  if (!local_frame) {
    return;
  }
  auto& selection = local_frame->Selection();
  selection.AsFrameSelectionExt()->NeedUpdateCursorLocation();
}
#endif

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
void WebFrameWidgetImplExt::ShowFreeCopyMenu() {
  WebLocalFrame* local_frame = FocusedWebLocalFrameInWidget();
  if (!local_frame) {
    return;
  }
  local_frame->SelectClosetWordAndShowSelectionMenu();
}
#endif

#if BUILDFLAG(ARKWEB_MEDIA_INTERACTION)
void WebFrameWidgetImplExt::DisableBoost() {
  rawKeyDownTime_--;
  if (rawKeyDownTime_ <= 0) {
    auto type = base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
        switches::kProcessType);
    if (type == switches::kRendererProcess) {
      blink::ResSchedReportClient report_client(
          OHOS::NWeb::SocPerfClientAdapter::SOC_PERF_WEB_GESTURE_ID);
      report_client.ApplySocByIdEx(
          OHOS::NWeb::SocPerfClientAdapter::SOC_PERF_WEB_GESTURE_ID, false);
    } else {
      OHOS::NWeb::OhosAdapterHelper::GetInstance()
          .CreateSocPerfClientAdapter()
          ->ApplySocPerfConfigByIdEx(
              OHOS::NWeb::SocPerfClientAdapter::SOC_PERF_WEB_GESTURE_ID, false);
    }
    rawKeyDownTime_ = 0;
  }
}
#endif

#if BUILDFLAG(ARKWEB_PINCH_SMOOTH)
void WebFrameWidgetImplExt::SetPinchSmoothMode(bool enable) {
  widget_base_->LayerTreeHost()->AsLayerTreeHostExt()->SetPinchSmoothMode(
      enable);
}
#endif
// LCOV_EXCL_STOP

#if BUILDFLAG(ARKWEB_SAME_LAYER)
void WebFrameWidgetImplExt::TouchHitTest(const WebPointerEvent& event,
                                         size_t finger_id) {
  WebPointerEvent transformed_event =
      TransformWebPointerEvent(LocalRootImpl()->GetFrameView(), event);
  WebPointerEvent pointer_event =
      transformed_event.WebPointerEventInRootFrame();
  HitTestRequest::HitTestRequestType hit_type =
      HitTestRequest::kTouchEvent | HitTestRequest::kReadOnly;
  HitTestLocation location(
      LocalRootImpl()->GetFrameView()->ConvertFromRootFrame(
          PhysicalOffset::FromPointFRound(pointer_event.PositionInWidget())));
  HitTestResult hit_test_result =
      LocalRootImpl()->GetFrame()->GetEventHandler().HitTestResultAtLocation(
          location, hit_type);
  int layer_id = 0;
  bool is_native_type = false;
  if (auto* plugin_element =
          DynamicTo<HTMLPlugInElement>(hit_test_result.InnerElement())) {
    if (plugin_element->NativeLoader()) {
      if (auto* layer = plugin_element->NativeLoader()->CcLayer()) {
        layer_id = layer->id();
      }
      is_native_type = true;
    }
  }

  widget_base_->utils()->NativeHitTestResult(is_native_type, finger_id,
                                             layer_id);
}

void WebFrameWidgetImplExt::MouseHitTest(const WebMouseEvent& event,
                                         int32_t button) {
  auto pointEvent = WebPointerEvent(event.GetType(), event);

  WebPointerEvent transformed_event =
      TransformWebPointerEvent(LocalRootImpl()->GetFrameView(), pointEvent);
  WebPointerEvent pointer_event =
      transformed_event.WebPointerEventInRootFrame();
  HitTestRequest::HitTestRequestType hit_type = HitTestRequest::kReadOnly;
  HitTestLocation location(
      LocalRootImpl()->GetFrameView()->ConvertFromRootFrame(
          PhysicalOffset::FromPointFRound(pointer_event.PositionInWidget())));
  HitTestResult hit_test_result =
      LocalRootImpl()->GetFrame()->GetEventHandler().HitTestResultAtLocation(
          location, hit_type);
  int layer_id = 0;
  bool is_native_type = false;
  if (auto* plugin_element =
          DynamicTo<HTMLPlugInElement>(hit_test_result.InnerElement())) {
    if (plugin_element->NativeLoader()) {
      if (auto* layer = plugin_element->NativeLoader()->CcLayer()) {
        layer_id = layer->id();
      }
      is_native_type = true;
    }
  }
  widget_base_->utils()->NativeMouseHitTestResult(is_native_type, layer_id,
                                                  button);
}
#endif

// LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void WebFrameWidgetImplExt::GetInputElementAttributes(
    HashMap<String, String>& attributes) const {
  WebInputMethodController* controller = GetActiveWebInputMethodController();
  if (!controller) {
    return;
  }

  controller->GetInputElementAttributes(attributes);
}
#endif

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
void WebFrameWidgetImplExt::DeterminePageLanguage() {
  WebLocalFrame* main_frame =
      LocalRootImpl()->ViewImpl()->MainFrame()->ToWebLocalFrame();
  std::u16string contents =
      blink::WebFrameContentDumper::DumpFrameTreeAsText(main_frame, 65535)
          .Utf16();

  WebDocument document = LocalRootImpl()->GetFrame()->GetDocument();
  blink::WebLanguageDetectionDetails details =
      blink::WebLanguageDetectionDetails::CollectLanguageDetectionDetails(
          document);

  std::string model_ans;
  std::string* model_ans_ptr = &model_ans;
  bool is_reliable;
  float score;

  std::string ans = translate::DeterminePageLanguage(
      details.content_language.Utf8(), details.html_language.Utf8(), contents,
      model_ans_ptr, &is_reliable, score);
  LOG(INFO) << "DeterminePageLanguage html_language: "
            << details.html_language.Utf8()
            << ", content_language: " << details.content_language.Utf8()
            << ", cld_language: " << model_ans
            << ", is_cld_reliable: " << is_reliable
            << ", language_detected: " << ans
            << ", contents_size: " << contents.size();

  if (auto host = GetAssociatedFrameWidgetHost(); host) {
    host->SendCurrentLanguage(static_cast<WTF::String>(ans));
  }
}
#endif
// LCOV_EXCL_STOP

#if BUILDFLAG(ARKWEB_DFX_TRACING)
// LCOV_EXCL_START
int64_t WebFrameWidgetImplExt::GetCurrentTimestampMS() {
  auto currentTime = std::chrono::system_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::microseconds>(currentTime)
              .count() /
          kMicrosecondsPerMillisecond;
}
// LCOV_EXCL_STOP

void WebFrameWidgetImplExt::ReportBlank(int64_t startTime, int64_t endTime) {
  int64_t duration = endTime - startTime;
  std::string mode = "ReportDragBlank";
  if (content::ChildProcess::current() && duration > kDragBlankTime) {
    content::ChildProcess::current()->ReportHisyevent(duration, mode);
  }
}
#endif

}  // namespace blink
