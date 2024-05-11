/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef ARK_WEB_HANDLER_CTOCPP_H_
#define ARK_WEB_HANDLER_CTOCPP_H_
#pragma once

#include "base/ctocpp/ark_web_ctocpp_ref_counted.h"
#include "ohos_nweb/capi/ark_web_handler_capi.h"
#include "ohos_nweb/include/ark_web_handler.h"

namespace OHOS::ArkWeb {

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class ArkWebHandlerCToCpp
    : public ArkWebCToCppRefCounted<ArkWebHandlerCToCpp, ArkWebHandler,
                                    ark_web_handler_t> {
public:
  ArkWebHandlerCToCpp();
  virtual ~ArkWebHandlerCToCpp();

  // ArkWebHandler methods.
  void SetNWeb(ArkWebRefPtr<ArkWebNWeb> nweb) override;

  bool OnFocus() override;

  void OnMessage(const ArkWebString &param) override;

  void OnResource(const ArkWebString &url) override;

  void OnPageIcon(const void *data, size_t width, size_t height, int color_type,
                  int alpha_type) override;

  void OnPageTitle(const ArkWebString &title) override;

  void OnProxyDied() override;

  void
  OnHttpError(ArkWebRefPtr<ArkWebUrlResourceRequest> request,
              ArkWebRefPtr<ArkWebUrlResourceResponse> error_response) override;

  bool OnConsoleLog(ArkWebRefPtr<ArkWebConsoleLog> message) override;

  void OnRouterPush(const ArkWebString &param) override;

  void OnPageLoadEnd(int http_status_code, const ArkWebString &url) override;

  void OnPageLoadBegin(const ArkWebString &url) override;

  void OnPageLoadError(int error_code, const ArkWebString &description,
                       const ArkWebString &failing_url) override;

  void OnDesktopIconUrl(const ArkWebString &icon_url,
                        bool precomposed) override;

  void OnLoadingProgress(int new_progress) override;

  void
  OnGeolocationShow(const ArkWebString &origin,
                    ArkWebRefPtr<ArkWebGeoLocationCallback> callback) override;

  void OnGeolocationHide() override;

  bool
  OnFileSelectorShow(ArkWebRefPtr<ArkWebStringVectorValueCallback> callback,
                     ArkWebRefPtr<ArkWebFileSelectorParams> params) override;

  void OnResourceLoadError(ArkWebRefPtr<ArkWebUrlResourceRequest> request,
                           ArkWebRefPtr<ArkWebUrlResourceError> error) override;

  void OnPermissionRequest(ArkWebRefPtr<ArkWebAccessRequest> request) override;

  void OnQuickMenuDismissed() override;

  void OnContextMenuDismissed() override;

  void OnTouchSelectionChanged(
      ArkWebRefPtr<ArkWebTouchHandleState> insert_handle,
      ArkWebRefPtr<ArkWebTouchHandleState> start_selection_handle,
      ArkWebRefPtr<ArkWebTouchHandleState> end_selection_handle) override;

  bool OnHandleInterceptRequest(
      ArkWebRefPtr<ArkWebUrlResourceRequest> request,
      ArkWebRefPtr<ArkWebUrlResourceResponse> response) override;

  void OnRefreshAccessedHistory(const ArkWebString &url,
                                bool is_reload) override;

  void OnPermissionRequestCanceled(
      ArkWebRefPtr<ArkWebAccessRequest> request) override;

  bool OnHandleInterceptUrlLoading(
      ArkWebRefPtr<ArkWebUrlResourceRequest> request) override;

  bool RunQuickMenu(ArkWebRefPtr<ArkWebQuickMenuParams> params,
                    ArkWebRefPtr<ArkWebQuickMenuCallback> callback) override;

  bool
  RunContextMenu(ArkWebRefPtr<ArkWebContextMenuParams> params,
                 ArkWebRefPtr<ArkWebContextMenuCallback> callback) override;

  void UpdateDragCursor(unsigned char op) override;

  bool FilterScrollEvent(const float x, const float y, const float x_velocity,
                         const float y_velocity) override;

  ArkWebStringVector VisitedUrlHistory() override;

  void OnWindowNewByJS(const ArkWebString &target_url, bool is_alert,
                       bool is_user_trigger,
                       ArkWebRefPtr<ArkWebControllerHandler> handler) override;

  void OnWindowExitByJS() override;

  bool OnAlertDialogByJS(const ArkWebString &url, const ArkWebString &message,
                         ArkWebRefPtr<ArkWebJsDialogResult> result) override;

  bool OnBeforeUnloadByJS(const ArkWebString &url, const ArkWebString &message,
                          ArkWebRefPtr<ArkWebJsDialogResult> result) override;

  bool OnPromptDialogByJS(const ArkWebString &url, const ArkWebString &message,
                          const ArkWebString &default_value,
                          ArkWebRefPtr<ArkWebJsDialogResult> result) override;

  bool OnConfirmDialogByJS(const ArkWebString &url, const ArkWebString &message,
                           ArkWebRefPtr<ArkWebJsDialogResult> result) override;

  bool OnHttpAuthRequestByJS(ArkWebRefPtr<ArkWebJsHttpAuthResult> result,
                             const ArkWebString &host,
                             const ArkWebString &realm) override;

  bool OnSslErrorRequestByJS(ArkWebRefPtr<ArkWebJsSslErrorResult> result,
                             int error) override;

  bool
  OnSslSelectCertRequestByJS(ArkWebRefPtr<ArkWebJsSslSelectCertResult> result,
                             const ArkWebString &host, int port,
                             const ArkWebStringVector &key_types,
                             const ArkWebStringVector &issuers) override;

  void OnScroll(double x_offset, double y_offset) override;

  void OnOverScroll(float x_offset, float y_offset) override;

  void OnScrollState(bool scroll_state) override;

  void OnPageVisible(const ArkWebString &url) override;

  bool OnPreKeyEvent(ArkWebRefPtr<ArkWebKeyEvent> event) override;

  void OnScaleChanged(float old_scale_factor, float new_scale_factor) override;

  bool OnCursorChange(const int32_t &type,
                      ArkWebRefPtr<ArkWebCursorInfo> info) override;

  void OnRenderExited(int reason) override;

  void OnResizeNotWork() override;

  void OnFullScreenExit() override;

  void
  OnFullScreenEnter(ArkWebRefPtr<ArkWebFullScreenExitHandler> handler) override;

  bool OnDragAndDropData(const void *data, size_t len,
                         ArkWebRefPtr<ArkWebImageOptions> opt) override;

  void OnSelectPopupMenu(
      ArkWebRefPtr<ArkWebSelectPopupMenuParam> params,
      ArkWebRefPtr<ArkWebSelectPopupMenuCallback> callback) override;

  void OnDataResubmission(
      ArkWebRefPtr<ArkWebDataResubmissionCallback> handler) override;

  void OnRootLayerChanged(int width, int height) override;

  void OnAudioStateChanged(bool playing) override;

  void OnOverScrollFlingEnd() override;

  bool OnUnProcessedKeyEvent(ArkWebRefPtr<ArkWebKeyEvent> event) override;

  bool OnDragAndDropDataUdmf(ArkWebRefPtr<ArkWebDragData> drag_data) override;

  void OnFirstContentfulPaint(int64_t navigation_start_tick,
                              int64_t first_contentful_paint_ms) override;

  void OnDateTimeChooserPopup(
      ArkWebRefPtr<ArkWebDateTimeChooser> chooser,
      const ArkWebDateTimeSuggestionVector &suggestions,
      ArkWebRefPtr<ArkWebDateTimeChooserCallback> callback) override;

  void OnDateTimeChooserClose() override;

  void OnScreenCaptureRequest(
      ArkWebRefPtr<ArkWebScreenCaptureAccessRequest> request) override;

  void OnActivityStateChanged(int state, int type) override;

  void OnGetTouchHandleHotZone(
      ArkWebRefPtr<ArkWebTouchHandleHotZone> hot_zone) override;

  void OnCompleteSwapWithNewSize() override;

  void OnOverScrollFlingVelocity(float x_velocity, float y_velocity,
                                 bool is_fling) override;

  void OnNavigationEntryCommitted(
      ArkWebRefPtr<ArkWebLoadCommittedDetails> details) override;

  void OnNativeEmbedLifecycleChange(
      ArkWebRefPtr<ArkWebNativeEmbedDataInfo> data_info) override;

  void OnNativeEmbedGestureEvent(
      ArkWebRefPtr<ArkWebNativeEmbedTouchEvent> touch_event) override;

  void OnSafeBrowsingCheckResult(int threat_type) override;

  void OnIntelligentTrackingPreventionResult(
      const ArkWebString &website_host,
      const ArkWebString &tracker_host) override;

  void OnFullScreenEnterWithVideoSize(
      ArkWebRefPtr<ArkWebFullScreenExitHandler> handler,
      int video_natural_width, int video_natural_height) override;

  bool OnHandleOverrideUrlLoading(
      ArkWebRefPtr<ArkWebUrlResourceRequest> request) override;

  void OnFirstMeaningfulPaint(
      ArkWebRefPtr<ArkWebFirstMeaningfulPaintDetails> details) override;

  void OnLargestContentfulPaint(
      ArkWebRefPtr<ArkWebLargestContentfulPaintDetails> details) override;

  bool OnAllSslErrorRequestByJS(ArkWebRefPtr<ArkWebJsAllSslErrorResult> result,
                                int error, const ArkWebString &url,
                                const ArkWebString &originalUrl,
                                const ArkWebString &referrer, bool isFatalError,
                                bool isMainFrame) override;

  void OnTooltip(const ArkWebString &tooltip) override;

  void ReleaseResizeHold() override;

    void OnShowAutofillPopup(const float offsetX, const float offsetY,
                           const ArkWebStringVector &menu_items) override;

  void OnHideAutofillPopup() override;

  ArkWebCharVector GetWordSelection(const ArkWebString &text,
                                    int8_t offset) override;

  void UpdateClippedSelectionBounds(int x, int y, int w, int h) override;

  bool OnOpenAppLink(const ArkWebString &url,
                     ArkWebRefPtr<ArkWebAppLinkCallback> callback) override;

  void OnRenderProcessNotResponding(const ArkWebString& js_stack, int pid, int reason) override;


  void OnRenderProcessResponding() override;

  void OnViewportFitChange(int viewport_fit) override;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_HANDLER_CTOCPP_H_
