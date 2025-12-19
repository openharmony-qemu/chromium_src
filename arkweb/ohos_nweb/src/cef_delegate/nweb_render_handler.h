/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NWEB_RENDER_HANDLER_H
#define NWEB_RENDER_HANDLER_H

#include <functional>
#include <memory>
#include <vector>

#include "arkweb/build/features/features.h"
#include "display_manager_adapter.h"
#include "include/arkweb_render_handler_ext.h"
#include "nweb_custom_keyboard_handler_impl.h"
#include "nweb_delegate_interface.h"
#include "nweb_handler.h"
#include "nweb_inputmethod_client.h"
#include "nweb_touch_handle_state_impl.h"
#include "ui/gfx/geometry/size.h"

#if BUILDFLAG(ARKWEB_DRAG_DROP)
#include "cef/include/cef_drag_data.h"
#include "nweb_drag_data.h"
#endif

namespace OHOS::NWeb {

// #if BUILDFLAG(ARKWEB_SCREEN_ROTATION)
struct NWebScreenInfo {
  RotationType rotation = RotationType::ROTATION_0;
  DisplayOrientation orientation = DisplayOrientation::UNKNOWN;
  int width = 0;
  int height = 0;
  double display_ratio = -1.0;
  bool default_portrait = false;
};
// #endif // #if BUILDFLAG(ARKWEB_SCREEN_ROTATION)
class NWebRenderHandler : public ArkWebRenderHandlerExt {
 public:
  static CefRefPtr<NWebRenderHandler> Create();
  NWebRenderHandler() = default;
  ~NWebRenderHandler() = default;

  void RegisterNativeScrollCallback(
      std::function<void(double, double)>&& callback);
  void RegisterRenderCb(std::function<void(const char*)> render_update_cb);
  void RegisterNWebHandler(std::shared_ptr<NWebHandler> handler);
#if BUILDFLAG(ARKWEB_AI)
  void RegisterNWebAgentHandler(std::shared_ptr<NWebAgentHandler> handler);
#endif
  void Resize(uint32_t width, uint32_t height);
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void ResizeVisibleViewport(uint32_t width, uint32_t height);
#endif
#if BUILDFLAG(ARKWEB_SCREEN_ROTATION)
  void SetScreenInfo(const NWebScreenInfo& screen_info);
#endif  // #if BUILDFLAG(ARKWEB_SCREEN_ROTATION)
  int ContentHeight();
  void SetInputMethodClient(CefRefPtr<NWebInputMethodClient> client);

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void SetNWebDelegateInterface(std::shared_ptr<NWebDelegateInterface> client);
  void OnOverscroll(CefRefPtr<CefBrowser> browser,
                    const float x,
                    const float y) override;
  void OnSelectionChanged(CefRefPtr<CefBrowser> browser,
                          const CefString& text,
                          const CefRange& selected_range) override;
  void OnCursorUpdate(CefRefPtr<CefBrowser> browser,
                      const CefRect& rect) override;
  void SetFocusStatus(bool focus_status);

  void OnUpdateTextInputStateCalled(CefRefPtr<CefBrowser> browser,
                                    const CefString& text,
                                    const CefRange& selected_range,
                                    const CefRange& compositon_range) override;
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
#if BUILDFLAG(ARKWEB_VIEWPORT_AVOID)
  void SetViewportAvoidHeight(int32_t viewportAvoidHeight);
#endif
#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
  void SendDynamicFrameLossEvent(CefRefPtr<CefBrowser> browser,
                                 const CefString& sceneId,
                                 bool isStart) override;
#endif
#if BUILDFLAG(IS_OHOS)
  void OnEditableChanged(CefRefPtr<CefBrowser> browser,
                         bool is_editable_node) override;
#endif
  /* CefRenderHandler method begin */
  void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;
#if BUILDFLAG(ARKWEB_INPUT_EVENTS) || BUILDFLAG(ARKWEB_VIEWPORT_AVOID)
  void GetVisibleViewportRect(CefRefPtr<CefBrowser> browser,
                              CefRect& rect) override;
  void SetNeedFocusViewport(bool need);
  void OnResizeScrollableViewport(CefRefPtr<CefBrowser> browser) override;
  void UpdateSecurityLayer(bool isNeedSecurityLayer) override;
  void UpdateTextFieldStatus(bool isShowKeyboard, bool isAttachIME) override;
#endif

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  void SetFillContent(const CefString& content) override;
#endif
  bool GetScreenInfo(CefRefPtr<CefBrowser> browser,
                     CefScreenInfo& screen_info) override;
  void OnPaint(CefRefPtr<CefBrowser> browser,
               PaintElementType type,
               const RectList& dirty_rects,
               const void* buffer,
               int width,
               int height) override;

  void OnRootLayerChanged(CefRefPtr<CefBrowser> browser,
                          int height,
                          int width) override;

  void ReleaseResizeHold(CefRefPtr<CefBrowser> browser) override;

  void OnScrollOffsetChanged(CefRefPtr<CefBrowser> browser,
                             double x,
                             double y) override;

  void OnImeCompositionRangeChanged(CefRefPtr<CefBrowser> browser,
                                    const CefRange& selected_range,
                                    const RectList& character_bounds) override;

  void OnTextSelectionChanged(CefRefPtr<CefBrowser> browser,
                              const CefString& selected_text,
                              const CefRange& selected_range) override;

  void OnVirtualKeyboardRequestedEx(CefRefPtr<CefBrowser> browser,
                                    TextInputInfo text_input_info,
                                    bool is_need_reset_listener,
                                    const AttributesMap& attributes) override;

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void HandleKeyboardAttach(
      CefRefPtr<CefBrowser> browser,
      const TextInputInfo& text_input_info,
      bool is_need_reset_listener,
      const std::map<std::string, std::string>& attributesMap);

  void HandleKeyboardDetach();
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

  void GetTouchHandleSize(CefRefPtr<CefBrowser> browser,
                          cef_horizontal_alignment_t orientation,
                          CefSize& size) override;
  void OnTouchSelectionChanged(
      const CefTouchHandleState& insert_handle,
      const CefTouchHandleState& start_selection_handle,
      const CefTouchHandleState& end_selection_handle,
      bool need_report) override;
#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
  void OnCompleteSwapWithNewSize() override;
  void OnResizeNotWork() override;
#endif  // BUILDFLAG(ARKWEB_COMPOSITE_RENDER)

#if BUILDFLAG(ARKWEB_DRAG_DROP)
  // update the cursor operation
  void UpdateDragCursor(CefRefPtr<CefBrowser> browser,
                        DragOperation operation) override;
  void ImageDragForFileUri(CefRefPtr<CefDragData> drag_data);
  void GetVisibleRectToWeb(int& visibleX,
                           int& visibleY,
                           int& visibleWidth,
                           int& visibleHeight) override;
  bool StartDragging(CefRefPtr<CefBrowser> browser,
                     CefRefPtr<CefDragData> drag_data,
                     DragOperationsMask allowed_ops,
                     int x,
                     int y) override;
  void SetIrregularDragBackground(bool is_irregular_background);
  void FreePixlMapData();
  void NotifySelectAllClicked(bool select_all) override;
#endif  // BUILDFLAG(ARKWEB_DRAG_DROP)

#if BUILDFLAG(IS_OHOS)
  void OnOverScrollFlingVelocity(CefRefPtr<CefBrowser> browser,
                                 const float x,
                                 const float y,
                                 bool is_fling) override;
  void OnOverScrollFlingEnd(CefRefPtr<CefBrowser> browser) override;
  void OnScrollState(CefRefPtr<CefBrowser> browser, bool scroll_state) override;
  void OnNativeEmbedGestureEvent(
      CefRefPtr<CefBrowser> browser,
      const CefEmbedTouchEvent& event,
      CefRefPtr<CefGestureEventCallback> callback) override;
  void OnNativeEmbedMouseEvent(
      CefRefPtr<CefBrowser> browser,
      const CefEmbedMouseEvent& event,
      CefRefPtr<CefMouseEventCallback> callback) override;
  void OnNativeEmbedLifecycleChange(CefRefPtr<CefBrowser> browser,  
                                    const CefNativeEmbedData& info) override;
  void OnNativeEmbedVisibilityChange(const CefString& embed_id,
                                     bool visibility) override;
  void OnNativeEmbedObjectParamChange(CefRefPtr<CefBrowser> browser,
                                      const CefNativeParamData& paramData) override;
  void OnScrollStart(CefRefPtr<CefBrowser> browser,
                     const float x,
                     const float y) override;
  bool FilterScrollEvent(CefRefPtr<CefBrowser> browser,
                         const float x,
                         const float y,
                         const float fling_x,
                         const float fling_y) override;
  bool OnNestedScroll(CefRefPtr<CefBrowser> browser,
                         float& x,
                         float& y,
                         float& fling_x,
                         float& fling_y,
                         bool& isAvailable) override;
  std::shared_ptr<NWebNativeEmbedDataInfo> CefEmbedDataToWeb(
      const CefNativeEmbedData& embedData);
  std::shared_ptr<NWebNativeEmbedParamDataInfo> CefEmbedParamDataToWeb(
      const CefNativeParamData& paramData);
  void SetContentSize(int width, int height);
  void SetGestureEventResult(bool result) override;
  bool GetGestureEventResult();
  gfx::Size GetSize();
  void StartVibraFeedback(const std::string& vibratorType) override;

  void OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect) override;

  void OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) override;
#endif

#if BUILDFLAG(ARKWEB_DSS)
  void GetDevicePixelSize(CefRefPtr<CefBrowser> browser,
                          CefSize& size) override;
#endif

#if BUILDFLAG(ARKWEB_AI)
  void GetWordSelection(CefRefPtr<CefBrowser> browser,
                        const CefString& text,
                        int8_t offset,
                        CefPoint& select) override;
#endif

#if BUILDFLAG(ARKWEB_AI)
  void CreateOverlay(CefRefPtr<CefBrowser> browser,
                     CefRefPtr<CefImage> cef_image,
                     const CefRect& cef_image_rect,
                     const CefPoint& cef_touch_point) override;
  void OnOverlayStateChanged(CefRefPtr<CefBrowser> browser,
                             const CefRect& cef_image_rect) override;
  bool GetDataDetectorEnable() override;
#endif

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
  void RestoreRenderFit() override;
#endif  // ARKWEB_MAXIMIZE_RESIZE
  /* CefRenderHandler method end */

  bool IsCustomKeyboard() const { return !isSystemKeyboard_; }

  std::shared_ptr<NWebCustomKeyboardHandlerImpl> GetCustomKeyboardHandler()
      const {
    return custom_keyboard_handler_;
  }

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  uint32_t GetContentHeight() const { return content_height_; }

  uint32_t GetContentWidth() const { return content_width_; }

  void OpenEyeDropper(CefRefPtr<CefBrowser> browser) override;
#endif

  std::shared_ptr<NWebTouchHandleState> GetTouchHandleState(
      NWebTouchHandleState::TouchHandleType type);
#if BUILDFLAG(ARKWEB_MENU)
  std::shared_ptr<NWebTouchHandleState> GetDefalutTouchHandleState(
      NWebTouchHandleState::TouchHandleType type);
  void OnSelectAreaChanged(CefRect& select_area) override; 
  void OnClippedSelectionBoundsChanged(const CefRect& rect,
                                       bool need_report) override;
#endif
  CefRefPtr<CefDragData> GetDragData();

  // #if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)
  float GetVirtualPixelRatio() const { return screen_info_.display_ratio; }
  // #endif  // #if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)
  // #if BUILDFLAG(ARKWEB_HTML_SELECT)
  float GetCefDeviceRatio() const { return cef_device_ratio_; }

#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
  void GetScrollOffset(float& x, float& y);
  bool HasOverscroll();
#endif

#if BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
  void OnDetectedBlankScreen(const std::string& url,
                             int32_t blankScreenReason,
                             int32_t detectedContentfulNodesCount) override;
#endif

#if BUILDFLAG(ARKWEB_FIRST_SCREEN_PAINT)
  void OnFirstScreenPaint(const std::string& url,
                          int64_t navigationStartTime,
                          int64_t firstScreenPaintTime) override;
#endif

#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
  void OnAccessibilityEvent(int64_t accessibilityId,
                            int32_t eventType,
                            const CefString& argument) override;
#endif
  // #endif

#if BUILDFLAG(ARKWEB_SCREEN_OFFSET)
  void SetScreenOffset(double x, double y);
  void GetScreenOffset(CefRefPtr<CefBrowser> browser, double& x, double& y) override;
#endif  // BUILDFLAG(ARKWEB_SCREEN_OFFSET)

  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(NWebRenderHandler);

 private:
  CefTouchHandleState ConvertTouchHandleDisplayRatio(
      const CefTouchHandleState& touch_handle);
#if BUILDFLAG(ARKWEB_MENU)
  CefRect ConvertSelectAreaDisplayRatio(const CefRect& rect);
  CefRect clipped_selection_bounds_;
#endif

  std::function<void(const char*)> render_update_cb_ = nullptr;
  CefRefPtr<NWebInputMethodClient> inputmethod_client_ = nullptr;
  std::shared_ptr<NWebCustomKeyboardHandlerImpl> custom_keyboard_handler_ =
      nullptr;
  uint32_t width_ = 0;
  uint32_t height_ = 0;
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  uint32_t visible_width_ = 0;
  uint32_t visible_height_ = 0;
  bool needFocusViewport_ = false;
  int32_t node_id_ = -1;
  bool noNeedKeyboardByInput_ = false;
#endif
#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
  float scroll_offset_x_ = 0.0f;
  float scroll_offset_y_ = 0.0f;
  bool has_over_scroll_ = false;
#endif
  int content_height_ = 0;
  int content_width_ = 0;
  NWebScreenInfo screen_info_;
  NWebScreenInfo last_screen_info_;
  float cef_device_ratio_ = 1.0;

  std::weak_ptr<NWebHandler> handler_;
  CefTouchHandleState insert_handle_;
  CefTouchHandleState start_selection_handle_;
  CefTouchHandleState end_selection_handle_;
  CefRefPtr<CefDragData> drag_data_ = nullptr;
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  std::shared_ptr<NWebDragData> nweb_drag_data_ = nullptr;
  bool is_irregular_drag_background_ = true;
  bool select_all_ = false;
#endif  // BUILDFLAG(ARKWEB_DRAG_DROP)
#if BUILDFLAG(ARKWEB_AI)
  std::weak_ptr<NWebAgentHandler> nweb_agent_handler_;
#endif
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  std::weak_ptr<NWebDelegateInterface> delegate_interface_;
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
  bool isSystemKeyboard_ = true;
  bool gesture_event_result_ = false;

#if BUILDFLAG(ARKWEB_SCREEN_OFFSET)
  double screen_x_ = 0;
  double screen_y_ = 0;
#endif

  std::function<void(double, double)> on_scroll_cb_ = nullptr;

#if BUILDFLAG(ARKWEB_VIEWPORT_AVOID)
  int32_t viewportAvoidHeight_ = 0;
  int32_t viewportAvoidScrollOffset_ = 0;
#endif
};
}  // namespace OHOS::NWeb

#endif  // NWEB_RENDER_HANDLER_H
