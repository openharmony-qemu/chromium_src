// Copyright (c) 2014 The Chromium Embedded Framework Authors.
// Portions copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_OSR_RENDER_WIDGET_HOST_VIEW_OSR_EXT_H_
#define CEF_LIBCEF_BROWSER_OSR_RENDER_WIDGET_HOST_VIEW_OSR_EXT_H_
#pragma once

#include <map>
#include <queue>
#include <set>
#include <vector>

#include "arkweb/build/features/features.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "build/build_config.h"
#include "cc/layers/deadline_policy.h"
#include "cef/include/cef_base.h"
#include "cef/include/cef_browser.h"
#include "cef/libcef/browser/alloy/alloy_browser_host_impl.h"
#include "cef/libcef/browser/osr/host_display_client_osr.h"
#include "cef/libcef/browser/osr/motion_event_osr.h"
#include "cef/libcef/browser/osr/render_widget_host_view_osr.h"
#include "cef/ohos_cef_ext/include/arkweb_client_ext.h"
#include "components/viz/common/frame_sinks/begin_frame_source.h"
#include "components/viz/common/surfaces/parent_local_surface_id_allocator.h"
#include "content/browser/renderer_host/input/mouse_wheel_phase_handler.h"
#include "content/browser/renderer_host/render_widget_host_view_base.h"
#include "content/browser/renderer_host/text_input_manager.h"
#include "content/common/native_embed_first_paint_event.h"
#include "content/public/browser/render_frame_metadata_provider.h"
#include "content/public/common/widget_type.h"
#include "third_party/blink/public/mojom/widget/record_content_to_visible_time_request.mojom-forward.h"
#include "ui/base/cursor/cursor.h"
#include "ui/base/cursor/mojom/cursor_type.mojom-shared.h"
#include "ui/base/ime/text_input_client.h"
#include "ui/compositor/compositor.h"
#include "ui/events/base_event_utils.h"
#include "ui/events/gesture_detection/filtered_gesture_provider.h"
#include "ui/events/gesture_detection/gesture_configuration.h"
#include "ui/events/velocity_tracker/motion_event_generic.h"
#include "ui/gfx/geometry/rect.h"
#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
#include "content/browser/ohos/software_compositor_host_ohos.h"
#endif

class ArkWebRenderWidgetHostViewOSRUtils;

namespace {
constexpr std::string_view TYPE_NAME = "ArkWebOSRExt";
}  // namespace

class ArkWebRenderWidgetHostViewOSRExt : public CefRenderWidgetHostViewOSR {
 public:
  ArkWebRenderWidgetHostViewOSRExt* AsArkWebRenderWidgetHostViewOSRExt()
      override {
    return this;
  }

  ArkWebRenderWidgetHostViewOSRExt(
      SkColor background_color,
      bool use_shared_texture,
      bool use_external_begin_frame,
      content::RenderWidgetHost* widget,
      CefRenderWidgetHostViewOSR* parent_host_view);
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  using GestureEventCallbackTask = base::OnceCallback<void(bool)>;
#endif
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  std::u16string GetLastSelectedTextFromContextParam() {
    return mLastSelectedTextFromMenu;
  }
  void SetLastSelectedTextFromContextParam(std::u16string selectedText) {
    mLastSelectedTextFromMenu = selectedText;
  }
#endif
#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
  void SetDrawMode(int mode);
  void SetDrawRect(const gfx::Rect& rect);
  bool GetPendingSizeStatus();
  void SetFitContentMode(int mode);
  void SetShouldFrameSubmissionBeforeDraw(bool should);
  void SendCurrentLanguage(const std::string& ans) override;
  std::string GetCurrentLanguage() { return language_; }
  int32_t is_fit_content_ = 0;
  double focus_rect_width_ = 0;
  double focus_rect_height_ = 0;
  bool is_select_text_ = false;
  std::string language_;
#endif  // BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
#if BUILDFLAG(IS_ARKWEB)
  void SetDoubleTapSupportEnabled(bool enabled);
  void SetMultiTouchZoomSupportEnabled(bool enabled);
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
  void OnTouchMove();
  void BoostingPreiodly();
#endif
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  void DidNativeEmbedEvent(
      const blink::mojom::NativeEmbedTouchEventPtr& touchEvent) override;
  void OnNativeEmbedLifecycleChange(
      const ArkWebRenderHandlerExt::CefNativeEmbedData& info);
  void SetGestureEventResult(bool result, bool stopPropagation, int32_t fingerId);
  void SetNativeEmbedMode(bool flag);
  void OnNativeEmbedVisibilityChange(const std::string& embed_id,
                                     bool visibility);
  void OnNativeEmbedFirstFramePaint(
      const content::NativeEmbedFirstPaintEvent& event);
  void DidNativeEmbedMouseEvent(const blink::mojom::NativeEmbedMouseEventPtr& mouseEvent) override;
  void SetMouseEventResult(bool result, bool stopPropagation);
  void SetNativeInnerWeb(bool isInnerWeb);
  void SetEnableCustomVideoPlayer(bool flag);
  void OnNativeEmbedObjectParamChange(
      const ArkWebRenderHandlerExt::CefNativeParamData& native_param_data);
#endif

#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
  void OnSafeInsetsChange(const gfx::Insets& safe_insets);
#endif

#if BUILDFLAG(ARKWEB_AI)
  std::vector<int8_t> GetWordSelection(const std::string& text,
                                       int8_t offset) override;
  void CreateOverlay(const gfx::ImageSkia& image,
                     const gfx::Rect& image_rect,
                     const gfx::Point& touch_point);
  void OnTextSelected(bool flag);
  void OnDestroyImageAnalyzerOverlay();
  float GetPageScaleFactor();
  void OnFoldStatusChanged(uint32_t foldstatus);
  void CloseImageOverlaySelection();
  void OnOverlayStateChanged(const gfx::Rect& image_rect);
  void NotifyOverlayStateChanged();
  bool SetDataDetectorSelectText(const std::u16string& text);
  std::string GetDataDetectorSelectText();
  void OnDataDetectorSelectText();
#endif
#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
  void UpdateVSyncFrequency();
  void ResetVSyncFrequency();
#endif
#if BUILDFLAG(ARKWEB_MENU)
  void OnTouchSelectionChanged(
      const CefTouchHandleState& insert_handle,
      const CefTouchHandleState& start_selection_handle,
      const CefTouchHandleState& end_selection_handle,
      bool need_report);
  bool NeedPopupInsertTouchHandleQuickMenu();
  std::u16string GetSelectedText() override;
  std::u16string GetText();
  void ResetGestureDetection(bool is_lost_focus) override;
  void OnTextSelectionChanged(content::TextInputManager* text_input_manager,
                              RenderWidgetHostViewBase* updated_view) override;
  void OnSelectAreaChanged(CefRect& select_area, bool need_report);
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  bool IsRequestUnadjustedMovement();
  blink::mojom::PointerLockResult LockPointer(
      bool request_unadjusted_movement) override;
  blink::mojom::PointerLockResult ChangePointerLock(
      bool request_unadjusted_movement) override;
  void UnlockPointer() override;
  bool IsPointerLocked() override;
  void SelectionChanged(const std::u16string& text,
                        size_t offset,
                        const gfx::Range& range) override;
  void AdvanceFocusForIME(int focusType);
  void SelectionBoundsChanged(const gfx::Rect& anchor_rect,
                              base::i18n::TextDirection anchor_dir,
                              const gfx::Rect& focus_rect,
                              base::i18n::TextDirection focus_dir,
                              const gfx::Rect& bounding_box,
                              bool is_anchor_first) override;
  void FocusedNodeChanged(bool is_editable_node,
                          const gfx::Rect& node_bounds_in_screen) override;
  void DidOverscroll(const ui::DidOverscrollParams& params) override;
  void OnDidNavigateMainFrameToNewPage();
  void SendTouchEventList(const std::vector<CefTouchEvent>& event_list);

  void DidStopFlinging() override;
  blink::mojom::InputEventResultState FilterInputEvent(
      const blink::WebInputEvent& input_event) override;
  void ScrollBy(float delta_x, float delta_y);

  bool GetScrollable() override { return scroll_enabled_; }
  void SendTouchGestureEvent(blink::WebTouchEvent& touch_event);
  void SendGestureEvent(const ui::GestureEventData& gesture);
  // TextInputManager::Observer implementation.
  void OnUpdateTextInputStateCalled(
      content::TextInputManager* text_input_manager,
      RenderWidgetHostViewBase* updated_view,
      bool did_update_state) override;

  // ui::GestureProviderClient implementation.
  void OnGestureEvent(const ui::GestureEventData& gesture) override;

  void KeyboardReDispatch(const input::NativeWebKeyboardEvent& event, bool isUsed) override;
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
  void SetBypassVsyncCondition(int32_t condition);
#endif

#if BUILDFLAG(ARKWEB_ZOOM)
  bool RequiresDoubleTapGestureEvents() const override;
#endif

#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
  void DynamicFrameLossEvent(const std::string& sceneId, bool isStart) override;
#endif

#if BUILDFLAG(ARKWEB_OCCLUDED_OPT)
  void WasOccluded() override;
  void SetEnableLowerFrameRate(bool enabled);
  void SetEnableHalfFrameRate(bool enabled);
  void EvictFrameBackBuffers() override;
#endif

#if BUILDFLAG(IS_ARKWEB)
  void OnRenderFrameMetadataChangedBeforeActivation(
      const cc::RenderFrameMetadata& metadata) override;
  void OnRenderFrameMetadataChangedAfterActivation(
      base::TimeTicks activation_time) override;
#endif

#if BUILDFLAG(ARKWEB_DSS)
  gfx::Size SizeInPixels() override;
#endif

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  bool WebPageSnapshot(const char* id,
                       int width,
                       int height,
                       cef_web_snapshot_callback_t callback);
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  gfx::Size GetPhysicalVisibleViewportSize();
  gfx::Size GetVisibleViewportSize() override;
  void WasKeyboardResizedEx() override {
    WasKeyboardResized();
  }
  void WasKeyboardResized();
  void OnUpdateTextInputStateCalledInner(
      const ui::mojom::TextInputState* state);
  void ScaleGestureChangeV2(int type,
                            float scale,
                            float originScale,
                            float centerX,
                            float centerY);
  void NotifyVirtualKeyboardOverlayRect(
      const gfx::Rect& keyboard_rect) override;
  ui::mojom::VirtualKeyboardMode GetVirtualKeyboardMode() override;
  void SetVirtualKeyBoardArg(int32_t width, int32_t height, double keyboard);
  void FilterScrollEventImpl(const ui::GestureEventData& gesture);
  void SetScrollable(bool enable);
  bool UpdateEditBounds();
  std::pair<int, int> HandleCursorOffset();
  void UpdateSecurityLayer(bool isNeedSecurityLayer);
  void UpdateTextFieldStatus(bool isShowKeyboard, bool isAttachIME);
#endif

#if BUILDFLAG(ARKWEB_TOUCHPAD_FLING)
  void SendTouchpadFlingEvent(blink::WebGestureEvent event);
#endif
#if BUILDFLAG(ARKWEB_MENU)
  void MouseSelectMenuShow(bool show);
  void ChangeVisibilityOfQuickMenu();
  bool IsQuickMenuShow();
#endif

#if BUILDFLAG(ARKWEB_PULL_TO_REFRESH)
  // ui::OverscrollRefreshHandler implementation
  bool PullToRefreshAction(ui::PullToRefreshAction action) override;
  void PullToRefreshUpdate(float x_delta, float y_delta) override;

  void DidStopRefresh();
  bool IsDisplayingInterstitial();
  bool FilterInputEventForPullToRefresh(
      const blink::WebInputEvent& input_event);
#endif

#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
  void TransformPointToRootSurface(gfx::PointF* point) override;
  int GetShrinkViewportHeight() override;
  int GetTopControlsOffset() const override;
  void OnTopControlsHeightChanged();
  void OnTopControlsChanged(float top_controls_offset,
                            float top_content_offset);
#endif

#if BUILDFLAG(ARKWEB_FOCUS)
  void Focus() override;
  bool HasFocus() override;
#endif

#if BUILDFLAG(ARKWEB_BACKGROUND_COLOR)
  void UpdateBackgroundColor() override;
#endif // BUILDFLAG(ARKWEB_BACKGROUND_COLOR)

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void UpdateCursor(const ui::Cursor& cursor) override;
  constexpr std::string_view GetViewType() override { return TYPE_NAME; }
#endif // BUILDFLAG(ARKWEB_INPUT_EVENTS)

  viz::FrameSinkId GetRootFrameSinkId() override;

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  void OnRendererWidgetCreated() override;
#endif
#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
  void SendAccessibilityEvent(int64_t accessibilityId,
                              int32_t eventType,
                              const std::string& argument) override;
#endif

#if BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
  void OnDetectedBlankScreen(const std::string& url,
                             int32_t blankScreenReason,
                             int32_t detectedContentfulNodesCount) override;
#endif

#if BUILDFLAG(ARKWEB_DRAG_DROP)
  void SetTextHandlesTemporarilyHiddenByDrag(bool hide_handles, bool dragging);
#endif
#if BUILDFLAG(IS_OHOS)
  void MaximizeResize();
  void RestoreRenderFit() override;
#endif
#if BUILDFLAG(ARKWEB_SCREEN_OFFSET)
  void OnScreenInfoChangedV2();
#endif
#if BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
  void OnVsync();
#endif
#if BUILDFLAG(IS_OHOS)
  ui::Compositor* GetCompositor() override;
#endif
#if BUILDFLAG(ARKWEB_PIP)
  void SetPipActive(bool active);
#endif
#if BUILDFLAG(ARKWEB_AI)
  ui::FilteredGestureProvider& GetGestureProvider();
#endif
 private:
  bool is_popup = false;

#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
  int32_t condition_ = 0;
#endif
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  std::u16string mLastSelectedTextFromMenu;
#endif //BUILDFLAG(ARKWEB_EXT_FREE_COPY)
#if BUILDFLAG(IS_ARKWEB)
  float page_scale_factor_ = 0.f;
  gfx::Size viewport_size_in_pixels_;
  float device_scale_factor_ = 1.0f;
  void OnRootLayerChanged();
  void OnScaleChanged(float old_page_scale_factor,
                      float new_page_scale_factor);
#endif
#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  gfx::Rect clipped_selection_bounds_;
  gfx::SizeF scrollable_viewport_size_;
  std::unique_ptr<content::SoftwareCompositorHostOhos> software_compositor_;
  gfx::SizeF root_layer_size_;
#endif
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  int32_t node_id_ = -1;
  int last_key_code_ = -1;
  bool scroll_enabled_ = true;
#endif
#if BUILDFLAG(ARKWEB_AI)
  bool overlay_in_progress_ = false;
  std::u16string data_detector_select_text_ = std::u16string();
#endif
#if BUILDFLAG(ARKWEB_ZOOM)
  std::string device_type_;
  bool requires_double_tap_gesture_events_ = false;
#endif
#if BUILDFLAG(ARKWEB_SYNC_RENDER)
  void UpdateDrawRect(const gfx::Rect& rect);
#endif
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void GestureEventAck(const blink::WebGestureEvent& event,
                       blink::mojom::InputEventResultSource ack_source,
                       blink::mojom::InputEventResultState ack_result) override;
  void SendInternalBeginFrame() override;
  void OnScrollState(bool scroll_state);
  void SetFocusOnGestureEvent(const ui::GestureEventData& gesture);
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_PULL_TO_REFRESH)
  void CreateOverscrollControllerIfPossible();
  void OnFocusInternal() override;
  void LostFocusInternal() override;
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  bool is_request_unadjusted_movement_ = false;
  bool is_pointer_locked_ = false;
  double focus_rect_x_ = 0;
  double focus_rect_y_ = 0;
  int edit_bounds_x_ = 0;
  int edit_bounds_y_ = 0;
  int edit_bounds_width_ = 0;
  int edit_bounds_height_ = 0;
  bool is_scroll_consumed_ = false;
  bool is_mouse_wheel_scroll_ = false;
  std::queue<ui::GestureEventData> pending_touchpad_pinch_events_;
  bool is_event_from_touch_ = false;
  bool is_tap_down_twice_ = false;
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_AI)
  bool is_scrolling_ = false;
#endif

#if BUILDFLAG(ARKWEB_PULL_TO_REFRESH)
  bool pull_to_refreshing_ = false;
  float pull_to_refresh_offset_x_ = 0;
  float pull_to_refresh_offset_y_ = 0;
  gfx::Transform root_layer_transform_{};
#endif

#if BUILDFLAG(IS_ARKWEB)
  bool enable_nweb_ex_ = false;
#endif

  base::WeakPtrFactory<ArkWebRenderWidgetHostViewOSRExt> weak_ptr_factory_;
};

#endif  // CEF_LIBCEF_BROWSER_OSR_RENDER_WIDGET_HOST_VIEW_OSR_EXT_H_