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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_WEB_FRAME_WIDGET_IMPL_EXT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_WEB_FRAME_WIDGET_IMPL_EXT_H_

#include "arkweb/build/features/features.h"
#include "base/types/pass_key.h"
#include "services/viz/public/mojom/hit_test/input_target_client.mojom-blink.h"
#include "third_party/blink/public/platform/cross_variant_mojo_util.h"
#include "third_party/blink/renderer/core/frame/web_frame_widget_impl.h"
#include "ui/gfx/geometry/rect.h"

namespace gfx {
class Point;
class PointF;
}  // namespace gfx

namespace blink {
class WebFrameWidgetImpl;

class WebFrameWidgetImplExt : public WebFrameWidgetImpl {
 public:
  WebFrameWidgetImplExt(
      base::PassKey<WebLocalFrame>,
      CrossVariantMojoAssociatedRemote<
          mojom::blink::FrameWidgetHostInterfaceBase> frame_widget_host,
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
      bool is_for_scalable_page);

  WebFrameWidgetImplExt* AsWebFrameWidgetImplExt() override { return this; }

#if BUILDFLAG(ARKWEB_MENU)
  void RegisterClippedVisualViewportSelectionBounds(
      gfx::Rect clipped_selection_bounds) override;
  void CleanFocusCache() override;
  void SelectRangeV2(const gfx::Point& position, bool is_base) override;
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void SetZoomLevel(float magnify_delta, const gfx::Point& anchor) override;
  void SetOverscrollMode(int mode) override;
  void GetInputElementAttributes(
      HashMap<String, String>& attributes) const override;
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

  void ArkWebHandleTouchEvent(const WebInputEvent& input_event);

#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
  gfx::Vector2dF GetOverScrollOffset() override;
#endif

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  void ShowFreeCopyMenu() override;
#endif

#if BUILDFLAG(ARKWEB_DRAG_DROP)
  gfx::Rect GetVisibleRectToWeb();
#endif

#if BUILDFLAG(ARKWEB_MEDIA_INTERACTION)
  void DisableBoost();
#endif

#if BUILDFLAG(ARKWEB_PINCH_SMOOTH)
  void SetPinchSmoothMode(bool enable);
#endif
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  void TouchHitTest(const WebPointerEvent& event, size_t i) override;
  void MouseHitTest(const WebMouseEvent& event) override;
#endif

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
  void DeterminePageLanguage();
#endif

#if BUILDFLAG(ARKWEB_AI)
  using OnTextRecognizedCallback = base::RepeatingCallback<
      void(std::vector<String>, std::vector<gfx::PointF>, float scale)>;
  using GetAbsImageRectCallback = base::RepeatingCallback<void(gfx::RectF&)>;
  using OnTextSelectedCallback = base::RepeatingCallback<void(bool)>;
  using OnDestroyImageAnalyzerOverlayCallback = base::RepeatingCallback<void()>;
  virtual void CreateOverlay(
      const SkBitmap& image,
      const gfx::Point& touch_point,
      GetAbsImageRectCallback get_rect_callback,
      OnTextSelectedCallback callback,
      OnDestroyImageAnalyzerOverlayCallback destroy_callback);
  void OnTextRecognized(WTF::Vector<mojom::blink::TextRecognizeResultPtr> res,
                        float scale) override;
  WTF::Vector<int8_t> GetWordSelection(const WTF::String& text, int8_t offset);
  void OnTextSelected(bool flag) override;
  using GetImageRectCallback = base::OnceCallback<void(const gfx::Rect&)>;
  void GetImageRect(GetImageRectCallback callback) override;
  gfx::Rect GetImageRectInner();
  void OnDestroyImageAnalyzerOverlay() override;
  virtual uint32_t GetFoldStatus();
  void OnFoldStatusChanged(uint32_t foldstatus) override;
  void NotifyOverlayStateChanged() override;
  void OnDataDetectorSelectText() override;
  OnTextRecognizedCallback on_text_recognize_callback_;
  GetAbsImageRectCallback get_rect_callback_;
  OnTextSelectedCallback on_text_selected_callback_;
  OnDestroyImageAnalyzerOverlayCallback on_destroy_image_overlay_callback_;
  uint32_t fold_status_ = 0;
#endif
#if BUILDFLAG(ARKWEB_DFX_TRACING)
  int64_t GetCurrentTimestampMS();
  void ReportBlank(int64_t startTime, int64_t endTime);
  static constexpr int64_t kMicrosecondsPerMillisecond = 1000;
#endif

#if BUILDFLAG(ARKWEB_MEDIA_INTERACTION)
  int64_t rawKeyDownTime_ = 0;
#endif
};
}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_WEB_FRAME_WIDGET_IMPL_EXT_H_
