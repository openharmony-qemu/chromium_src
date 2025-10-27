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

#ifndef ARKWEB_RENDER_WIDGET_HOST_VIEW_OSR_UTILS_H_
#define ARKWEB_RENDER_WIDGET_HOST_VIEW_OSR_UTILS_H_

#include "arkweb/build/features/features.h"
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "cef/libcef/browser/osr/browser_platform_delegate_osr.h"
#include "cef/ohos_cef_ext/libcef/browser/osr/arkweb_render_widget_host_view_osr_ext.h"
#include "content/browser/renderer_host/delegated_frame_host.h"
#include "content/browser/renderer_host/frame_tree.h"
#include "content/public/browser/context_factory.h"
#include "ui/gfx/text_elider.h"

#if BUILDFLAG(IS_OHOS)
#include "base/ohos/sys_info_utils_ext.h"
#include "cef/ohos_cef_ext/include/arkweb_render_handler_ext.h"
#include "content/browser/gpu/gpu_process_host.h"
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
#include "third_party/ohos_ndk/includes/ohos_adapter/res_sched_client_adapter.h"
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
#include "libcef/browser/native/cursor_util.h"
#include "ui/base/ime/text_input_flags.h"
#include "ui/events/blink/did_overscroll_params.h"
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_DRAG_RESIZE)
#include "ohos_nweb/src/nweb_resize_helper.h"
#endif

#if BUILDFLAG(ARKWEB_SCROLLBAR)
extern const int SCALE_FACTOR_CONVERT_RATIO;
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
extern const int SOC_PERF_WEB_GESTURE_ID;
extern const int TOUCH_DOWN_DELAY_TIME;
extern const int TOUCH_UP_DURATION_TIME;
#endif

class CefRenderWidgetHostViewOSR;

class ArkWebRenderWidgetHostViewOSRUtils {
 public:
  friend class CefRenderWidgetHostViewOSR;
  explicit ArkWebRenderWidgetHostViewOSRUtils(CefRenderWidgetHostViewOSR* view);
  ~ArkWebRenderWidgetHostViewOSRUtils();

  ArkWebRenderWidgetHostViewOSRUtils(
      const ArkWebRenderWidgetHostViewOSRUtils&) = delete;
  ArkWebRenderWidgetHostViewOSRUtils& operator=(
      const ArkWebRenderWidgetHostViewOSRUtils&) = delete;

  static display::mojom::ScreenOrientation ConvertOrientationType(
      cef_screen_orientation_type_t type);
  static void UpdateScreenInfoForArkweb(display::ScreenInfo& screenInfo,
                                        const CefScreenInfo& src);
  static auto GetContextFactory() { return content::GetContextFactory(); }
  void HandleCompositorCreation(base::SingleThreadTaskRunner* task_runner,
                                bool use_external_begin_frame);
  void HandleCompositeRenderRelease();
  void SetupCompositor(ui::Compositor* compositor);
  void HandleInvalidLocalSurfaceId();
  static std::u16string TruncateTooltipText(const std::u16string& tooltip_text);
  gfx::Rect CalculateViewBounds(CefRefPtr<CefRenderHandler> handler,
                                CefRect& rc);

  void ImeCommitTextEx(const CefString& text,
                       const gfx::Range& range,
                       int relative_cursor_pos);

  void ImeSetCompositionEx(const CefString& text,
                           const std::vector<ui::ImeTextSpan>& web_underlines,
                           const gfx::Range& range,
                           int selection_start,
                           int selection_end);

  void ImeCancelCompositionEx();
  void SendTouchEventEx(const CefTouchEvent& event);
  void SetCompositorVSyncParameters(int frame_rate_threshold_us);
  void SetupCompositorWithRootLayerSize();
  void SynchronizeVisualPropertiesEx(bool resized);
#if BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
  void OnTouchDown();
  void StopBoosting();
#endif
  void HandleRawKeyDownEvent(const input::NativeWebKeyboardEvent& event);
  static void AddCompositor(gfx::AcceleratedWidget widget,
                            ui::Compositor* compositor);
  static ui::Compositor* GetCompositor(gfx::AcceleratedWidget widget);
#if BUILDFLAG(ARKWEB_DSS)
  bool SetCurrentSizeInPixel();
#endif

 protected:
#if BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
  bool has_touch_point_ = false;
#endif

 private:
  const raw_ptr<CefRenderWidgetHostViewOSR> view_;
  static std::unordered_map<gfx::AcceleratedWidget, ui::Compositor*>
      compositor_map_;
  static std::unordered_map<gfx::AcceleratedWidget, uint32_t>
      accelerate_widget_map_;
#if BUILDFLAG(ARKWEB_DSS)
  gfx::Size current_size_in_pixel_ = {0, 0};
#endif  // ARKWEB_DSS
#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
  base::CancelableOnceClosure setReleaseResizeHoldDelayedTask_;
#endif
  bool SetVisibleViewportSize();
  bool SetRootLayerSizeEx(bool force, bool* visible_changed = nullptr);
  bool ResizeRootLayerEx(bool isKeyboard, bool& visible_changed);
  void ReleaseResizeHoldEx();
  void OnScrollOffsetChangedEx(CefRefPtr<CefRenderHandler> handler);
  void SetDoubleTapSupportForPlatformEnabledEx();
  void HideEx();
  void SetFocusEx();
  void ImeFinishComposingTextEx(bool keep_selection);
  void SendMouseEventEx(
    const blink::WebMouseEvent& event);
  void SendMouseWheelEventEx(
    blink::WebMouseWheelEvent& mouse_wheel_event);
  base::WeakPtrFactory<ArkWebRenderWidgetHostViewOSRUtils> weak_ptr_factory_{
      this};
};

#endif  // ARKWEB_RENDER_WIDGET_HOST_VIEW_OSR_UTILS_H_
