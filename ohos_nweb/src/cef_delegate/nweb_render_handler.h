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
#include "cef/include/cef_render_handler.h"
#include "display_manager_adapter.h"
#include "nweb_delegate_interface.h"
#include "nweb_handler.h"
#include "nweb_inputmethod_client.h"
#include "nweb_touch_handle_state_impl.h"

#ifdef OHOS_DRAG_DROP
#include "cef/include/cef_drag_data.h"
#include "nweb_drag_data.h"
#endif

namespace OHOS::NWeb {

// #ifdef OHOS_SCREEN_ROTATION
struct NWebScreenInfo {
  RotationType rotation = RotationType::ROTATION_0;
  DisplayOrientation orientation = DisplayOrientation::UNKNOWN;
  int width = 0;
  int height = 0;
  double display_ratio = -1.0;
  bool default_portrait = false;
};
// #endif // #ifdef OHOS_SCREEN_ROTATION
class NWebRenderHandler : public CefRenderHandler {
 public:
  static CefRefPtr<NWebRenderHandler> Create();
  NWebRenderHandler() = default;
  ~NWebRenderHandler() = default;

  void RegisterRenderCb(std::function<void(const char*)> render_update_cb);
  void RegisterNWebHandler(std::shared_ptr<NWebHandler> handler);
  void Resize(uint32_t width, uint32_t height);
#ifdef OHOS_SCREEN_ROTATION
  void SetScreenInfo(const NWebScreenInfo& screen_info);
  NWebScreenInfo& GetLastScreenInfo();
  void SetLastScreenInfo(const NWebScreenInfo& screen_info);
  bool IsNeedCefNotifyScreenInfoChanged();
#endif  // #ifdef OHOS_SCREEN_ROTATION
  int ContentHeight();
  void SetInputMethodClient(CefRefPtr<NWebInputMethodClient> client);

#if defined(OHOS_INPUT_EVENTS)
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
#endif  // defined(OHOS_INPUT_EVENTS)
#if BUILDFLAG(IS_OHOS)
  void OnEditableChanged(CefRefPtr<CefBrowser> browser,
                         bool is_editable_node) override;
#endif
  /* CefRenderHandler method begin */
  void GetViewRect(CefRefPtr<CefBrowser> browser,
                   CefRect& rect) override;
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

  void OnImeCompositionRangeChanged(
      CefRefPtr<CefBrowser> browser,
      const CefRange& selected_range,
      const RectList& character_bounds) override;

  void OnTextSelectionChanged(CefRefPtr<CefBrowser> browser,
                                      const CefString& selected_text,
                                      const CefRange& selected_range) override;

  void OnVirtualKeyboardRequested(CefRefPtr<CefBrowser> browser,
                                          TextInputMode input_mode,
                                          TextInputType input_type,
                                          bool show_keyboard) override;
  void GetTouchHandleSize(CefRefPtr<CefBrowser> browser,
                          cef_horizontal_alignment_t orientation,
                          CefSize& size) override;
  void OnTouchSelectionChanged(
      const CefTouchHandleState& insert_handle,
      const CefTouchHandleState& start_selection_handle,
      const CefTouchHandleState& end_selection_handle,
      bool need_report) override;
#if defined(OHOS_COMPOSITE_RENDER)
  void OnCompleteSwapWithNewSize() override;
  void OnResizeNotWork() override;
#endif  // defined(OHOS_COMPOSITE_RENDER)

#ifdef OHOS_DRAG_DROP
  // update the cursor operation
  void UpdateDragCursor(CefRefPtr<CefBrowser> browser,
                        DragOperation operation) override;
  void ImageDragForFileUri(CefRefPtr<CefDragData> drag_data);
  bool StartDragging(CefRefPtr<CefBrowser> browser,
                     CefRefPtr<CefDragData> drag_data,
                     DragOperationsMask allowed_ops,
                     int x,
                     int y) override;
  void SetIrregularDragBackground(bool is_irregular_background);
  void FreePixlMapData();
  void NotifySelectAllClicked(bool select_all) override;
#endif // #ifdef OHOS_DRAG_DROP

#if BUILDFLAG(IS_OHOS)
  void OnOverScrollFlingVelocity(CefRefPtr<CefBrowser> browser,
                                 const float x,
                                 const float y,
                                 bool is_fling) override;
  void OnOverScrollFlingEnd(CefRefPtr<CefBrowser> browser) override;
  void OnScrollState(CefRefPtr<CefBrowser> browser,
                     bool scroll_state) override;
  void OnNativeEmbedGestureEvent(CefRefPtr<CefBrowser> browser,
                    const CefEmbedTouchEvent& event,
                    CefRefPtr<CefGestureEventCallback> callback) override;
  void OnNativeEmbedLifecycleChange(CefRefPtr<CefBrowser> browser,
                    const CefNativeEmbedData& info) override;
  bool FilterScrollEvent(CefRefPtr<CefBrowser> browser,
                         const float x,
                         const float y,
                         const float fling_x,
                         const float fling_y) override;
#endif

#ifdef OHOS_EX_FREE_COPY
  void GetWordSelection(CefRefPtr<CefBrowser> browser,
                        const CefString& text,
                        int8_t offset,
                        CefPoint& select) override;
#endif
  /* CefRenderHandler method end */

  std::shared_ptr<NWebTouchHandleState> GetTouchHandleState(
      NWebTouchHandleState::TouchHandleType type);

  std::shared_ptr<NWebTouchHandleState> GetDefalutTouchHandleState(
    NWebTouchHandleState::TouchHandleType type);

  CefRefPtr<CefDragData> GetDragData();

// #ifdef OHOS_PAGE_UP_DOWN
  float GetVirtualPixelRatio() const { return screen_info_.display_ratio; }
  // #endif  // #ifdef OHOS_PAGE_UP_DOWN
  // #ifdef OHOS_HTML_SELECT
  float GetCefDeviceRatio() const { return cef_device_ratio_; }
  // #endif
  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(NWebRenderHandler);

 private:
  CefTouchHandleState ConvertTouchHandleDisplayRatio(
      const CefTouchHandleState& touch_handle);

  std::function<void(const char*)> render_update_cb_ = nullptr;
  CefRefPtr<NWebInputMethodClient> inputmethod_client_ = nullptr;
  uint32_t width_ = 0;
  uint32_t height_ = 0;
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
#ifdef OHOS_DRAG_DROP
  std::shared_ptr<NWebDragData> nweb_drag_data_ = nullptr;
  bool is_irregular_drag_background_ = true;
  bool select_all_ = false;
#endif // #ifdef OHOS_DRAG_DROP

#if defined(OHOS_INPUT_EVENTS)
  std::weak_ptr<NWebDelegateInterface> delegate_interface_;
#endif  // defined(OHOS_INPUT_EVENTS)
};
}  // namespace OHOS::NWeb

#endif  // NWEB_RENDER_HANDLER_H
