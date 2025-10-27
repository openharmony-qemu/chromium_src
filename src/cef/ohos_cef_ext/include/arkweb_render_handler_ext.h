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

// Copyright (c) 2012 Marshall A. Greenblatt. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the name Chromium Embedded
// Framework nor the names of its contributors may be used to endorse
// or promote products derived from this software without specific prior
// written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// ---------------------------------------------------------------------------
//
// The contents of this file must follow a specific format in order to
// support the CEF translator tool. See the translator.README.txt file in the
// tools directory for more information.
//

#ifndef OHOS_CEF_EXT_INCLUDE_ARKWEB_RENDER_HANDLER_EXT_H_
#define OHOS_CEF_EXT_INCLUDE_ARKWEB_RENDER_HANDLER_EXT_H_
#pragma once

#include "arkweb/build/features/features.h"
#include "include/cef_render_handler.h"

///
/// Extended from CefRenderHandler
///
class ArkWebRenderHandlerExt : public virtual CefRenderHandler,
                               public virtual CefBaseRefCounted {
 public:
  typedef cef_text_input_type_t TextInputType;
  typedef cef_native_embed_data_t CefNativeEmbedData;
  typedef cef_embed_touch_event_t CefEmbedTouchEvent;
  typedef cef_embed_life_change_t CefEmbedLifeStatus;
  typedef cef_embed_touch_type_t CefEmbedTouchType;
  typedef cef_text_input_action_t TextInputAction;
  typedef cef_text_input_flags_t TextInputFlags;
  typedef cef_text_input_info_t TextInputInfo;
  typedef std::map<CefString, CefString> AttributesMap;
  typedef cef_embed_mouse_event_t CefEmbedMouseEvent;
  typedef cef_embed_mouse_type_t CefEmbedMouseType;
  typedef cef_embed_mouse_button_t CefEmbedMouseButton;
  typedef cef_native_param_status_t CefNativeParamStatus;
  typedef cef_native_param_item_t CefNativeParamItem;
  typedef cef_native_param_data_t CefNativeParamData;

  CefRefPtr<ArkWebRenderHandlerExt> AsArkWebRenderHandler() override {
    return this;
  }

  ///
  /// Called when touch selection is updated. The client is responsible for
  /// rendering the touch handles.
  ///
  void OnTouchSelectionChanged(
      const CefTouchHandleState& insert_handle,
      const CefTouchHandleState& start_selection_handle,
      const CefTouchHandleState& end_selection_handle,
      bool need_report) override {}

#if BUILDFLAG(ARKWEB_MENU) || BUILDFLAG(ARKWEB_PDF)
  ///
  /// Called when touch selection is updated, but select_area data id not
  /// controlled by kernel.The select_area is the shadow area between
  /// touch handles.Initially added for PDF.
  ///
  virtual void OnSelectAreaChanged(CefRect& select_area) {}
#endif

  ///
  /// Called when the RootLayer has changed.
  ///
  virtual void OnRootLayerChanged(CefRefPtr<CefBrowser> browser,
                                  int height,
                                  int width) {}

  ///
  /// Called when text selection has changed for the specified |browser|.
  /// |text| is the currently text and |selected_range| is
  /// the character range.
  ///
  virtual void OnSelectionChanged(CefRefPtr<CefBrowser> browser,
                                  const CefString& text,
                                  const CefRange& selected_range) {}

  ///
  /// Called when the cursor position has changed.
  ///
  virtual void OnCursorUpdate(CefRefPtr<CefBrowser> browser,
                              const CefRect& rect) {}

  ///
  /// Called when swap buffer completed with new size.
  ///
  virtual void OnCompleteSwapWithNewSize() {}

  ///
  /// Called when resize not work.
  ///
  virtual void OnResizeNotWork() {}

  ///
  /// Called when over scroll.
  ///
  virtual void OnOverscroll(CefRefPtr<CefBrowser> browser,
                            const float x,
                            const float y) {}
  ///
  /// Called when editable status has been changed.
  ///
  virtual void OnEditableChanged(CefRefPtr<CefBrowser> browser,
                                 bool is_editable_node) {}

  ///
  /// Called when over scroll.
  ///
  virtual void OnOverScrollFlingVelocity(CefRefPtr<CefBrowser> browser,
                                         const float x,
                                         const float y,
                                         bool is_fling) {}

  ///
  /// Called when over scroll fling end.
  ///
  virtual void OnOverScrollFlingEnd(CefRefPtr<CefBrowser> browser) {}

  ///
  /// Called when scroll begin or end.
  ///
  virtual void OnScrollState(CefRefPtr<CefBrowser> browser, bool scroll_state) {
  }

  ///
  /// Called when scroll begin or end.
  ///
  virtual void OnScrollStart(CefRefPtr<CefBrowser> browser,
                             const float x,
                             const float y) {}
  ///
  /// Called when scroll.
  ///
  virtual bool FilterScrollEvent(CefRefPtr<CefBrowser> browser,
                                 const float x,
                                 const float y,
                                 const float fling_x,
                                 const float fling_y) {
    return false;
  }

  ///
  /// Called when embed touch.
  ///
  virtual void OnNativeEmbedGestureEvent(
      CefRefPtr<CefBrowser> browser,
      const CefEmbedTouchEvent& event,
      CefRefPtr<CefGestureEventCallback> callback) {}

  ///
  /// Called when embed mouse.
  ///
  virtual void OnNativeEmbedMouseEvent(CefRefPtr<CefBrowser> browser,
                                   const CefEmbedMouseEvent& event,
                                   CefRefPtr<CefMouseEventCallback> callback) {}

  ///
  /// Called when embed touch.
  ///
  virtual void OnNativeEmbedLifecycleChange(CefRefPtr<CefBrowser> browser,
                                            const CefNativeEmbedData& info) {}

  ///
  /// Called when embed visible.
  ///
  virtual void OnNativeEmbedVisibilityChange(const CefString& embed_id,
                                             bool visibility) {}
  
  ///
  /// Called when params change.
  ///
  virtual void OnNativeEmbedObjectParamChange(CefRefPtr<CefBrowser> browser,
                                              const CefNativeParamData& paramData) {}
  

  ///
  /// Called when select all is clicked.
  ///
  virtual void NotifySelectAllClicked(bool select_all) {}

  ///
  /// Called when scroll begin or end.
  ///
  virtual void ReleaseResizeHold(CefRefPtr<CefBrowser> browser) {}

  ///
  /// Called when selecting word.
  ///
  virtual void GetWordSelection(CefRefPtr<CefBrowser> browser,
                                const CefString& text,
                                int8_t offset,
                                CefPoint& select) {}

  ///
  /// Called when creating overlay.
  ///
  virtual void CreateOverlay(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefImage> cef_image,
                             const CefRect& cef_image_rect,
                             const CefPoint& cef_touch_point) {}

  ///
  /// Called when overlay state is changed.
  ///
  virtual void OnOverlayStateChanged(CefRefPtr<CefBrowser> browser,
                                     const CefRect& cef_image_rect) {}

  ///
  /// Get data detector enable.
  ///
  virtual bool GetDataDetectorEnable() {}

  ///
  /// Called when text input state has changed for the specified |browser|.
  ///
  virtual void OnUpdateTextInputStateCalled(CefRefPtr<CefBrowser> browser,
                                            const CefString& text,
                                            const CefRange& selected_range,
                                            const CefRange& compositon_range) {}

  ///
  /// Called to retrieve the view rectangle in screen DIP coordinates. This
  /// method must always provide a non-empty rectangle.
  ///
  virtual void GetVisibleViewportRect(CefRefPtr<CefBrowser> browser,
                                      CefRect& rect) {}

  ///
  /// OnResizeScrollableViewport.
  ///
  virtual void OnResizeScrollableViewport(CefRefPtr<CefBrowser> browser) {}

  ///
  /// SetFillContent
  ///
  virtual void SetFillContent(const CefString& content) {}

  ///
  /// Called when you need to start vibrator.
  ///
  virtual void StartVibraFeedback(const std::string& vibratorType) {}

  ///
  /// Called when an on-screen keyboard should be shown or hidden for the
  /// specified |browser|. |input_mode| specifies what kind of keyboard
  /// should be opened. If |input_mode| is CEF_TEXT_INPUT_MODE_NONE, any
  /// existing keyboard for this browser should be hidden.
  ///
  virtual void OnVirtualKeyboardRequestedEx(CefRefPtr<CefBrowser> browser,
                                            TextInputInfo text_input_info,
                                            bool is_need_reset_listener,
                                            const AttributesMap& attributes) {}

  ///
  /// SetGestureEventResult
  ///
  /*--cef()--*/
  virtual void SetGestureEventResult(const bool result) {}

  ///
  /// Called when an accessibility event occurs.
  ///
  /*--cef()--*/
  virtual void OnAccessibilityEvent(int64_t accessibilityId, int32_t eventType, const CefString& argument) {}

  ///
  /// This interface is invoked to notify the upper-layer update security layer
  ///
  /*--cef()--*/
  virtual void UpdateSecurityLayer(bool isNeedSecurityLayer) {}

  ///
  /// Update focus and keyboard status in textFieldManager
  ///
  /*--cef()--*/
  virtual void UpdateTextFieldStatus(bool isShowKeyboard, bool isAttachIME) {}

#if BUILDFLAG(ARKWEB_DSS)
  ///
  /// Get Device pixel size.
  ///
  /*--cef()--*/
  virtual void GetDevicePixelSize(CefRefPtr<CefBrowser> browser,
                                  CefSize& size) {}
#endif
#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
  ///
  /// SendDynamicFrameLossEvent
  ///
  /*--cef()--*/
  virtual void SendDynamicFrameLossEvent(CefRefPtr<CefBrowser> browser,
                                         const CefString& sceneId,
                                         bool isStart) {}
#endif

#if BUILDFLAG(IS_ARKWEB)
  ///
  /// Get the visible area relative to the web.
  ///
  /*--cef()--*/
  virtual void GetVisibleRectToWeb(int& visibleX,
                                   int& visibleY,
                                   int& visibleWidth,
                                   int& visibleHeight) {}
#endif  // BUILDFLAG(IS_ARKWEB)

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
  ///
  /// restore renderfit
  ///
  virtual void RestoreRenderFit() {}
#endif  // ARKWEB_MAXIMIZE_RESIZE

#if BUILDFLAG(ARKWEB_SCREEN_OFFSET)
  ///
  /// Get Screen Offset.
  ///
  /*--cef()--*/
  virtual void GetScreenOffset(CefRefPtr<CefBrowser> browser, double& x, double& y) {}
#endif  // BUILDFLAG(ARKWEB_SCREEN_OFFSET)

#if BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
  ///
  /// Called when a blank screen is detected.
  ///
  /*--cef()--*/
  virtual void OnDetectedBlankScreen(const std::string &url,
                                     int32_t blankScreenReason,
                                     int32_t detectedContentfulNodesCount) {}
#endif

  ///
  /// Called when scroll.
  ///
  /*--cef()--*/
  virtual bool OnNestedScroll(CefRefPtr<CefBrowser> browser,
                              float& x,
                              float& y,
                              float& fling_x,
                              float& fling_y,
                              bool& isAvailable) {
    return false;
  }

#if BUILDFLAG(ARKWEB_DRAG_DROP)
  ///
  /// Called when selection changed.
  ///
  /*--cef()--*/
  virtual void SelectionBoundsChanged(const CefRect& anchor_rect,
                                      const CefRect& focus_rect,
                                      bool is_anchor_first) {}
#endif
};

#endif  // OHOS_CEF_EXT_INCLUDE_ARKWEB_RENDER_HANDLER_EXT_H_
