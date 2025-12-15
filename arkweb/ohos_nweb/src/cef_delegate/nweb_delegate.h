/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef NWEB_DELEGATE_H
#define NWEB_DELEGATE_H

#include <condition_variable>
#include <set>
#include <string>
#include <unordered_map>

#include "build/build_config.h"
#include "capi/nweb_app_client_extension_callback.h"
#include "capi/nweb_extension_javascript_item.h"
#include "cef/include/cef_command_line.h"
#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
#include "content/browser/accessibility/browser_accessibility_manager_ohos.h"
#include "content/browser/accessibility/browser_accessibility_ohos.h"
#include "nweb_accessibility_node_info_impl.h"
#endif

#include "nweb_application.h"
#include "nweb_delegate_interface.h"
#include "nweb_display_listener.h"
#include "nweb_errors.h"
#include "nweb_event_handler.h"
#include "nweb_handler.h"
#include "nweb_handler_delegate.h"
#include "nweb_inputmethod_client.h"
#include "nweb_render_handler.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
#include <memory>

#include "capi/nweb_download_delegate_callback.h"
#endif  //  ARKWEB_EX_DOWNLOAD

#include "build/build_config.h"
#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "ohos_nweb_ex/core/extension/nweb_app_client_extension_dispatcher.h"
#endif

struct FrameInfos;
struct IsolatedWorld;
struct RunJavaScriptParam;
struct OpenDevToolsParam;

namespace OHOS::NWeb {
using namespace ui;
class JavaScriptResultCallbackImpl;
#if BUILDFLAG(ARKWEB_AI)
class NWebAgentManagerImpl;
#endif
#if BUILDFLAG(ARKWEB_PDF)
class CefPdfValueCallbackImpl;
#endif  // BUILDFLAG(ARKWEB_PDF)
class NWebDelegate : public NWebDelegateInterface, public virtual CefRefCount {
 public:
  NWebDelegate(int argc, const char* argv[]);
  ~NWebDelegate();

  bool Init(bool is_enhance_surface,
            void* window,
            bool popup
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
            ,
            uint32_t nweb_id
#endif
#if BUILDFLAG(IS_OHOS)
            ,
            bool incognito_mode
#endif
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
            ,
            const std::string& shared_render_process_token
#endif

  );
#if BUILDFLAG(ARKWEB_AI)
  friend class NWebAgentManagerImpl;
#endif
  void OnWindowShow() override;
  void OnWindowHide() override;
  void OnOnlineRenderToForeground() override;
  void NotifyForNextTouchEvent() override;
  bool IsReady() override;
  void OnDestroy(bool is_close_all) override;
#if BUILDFLAG(ARKWEB_NWEB_EX)
  void RegisterArkWebAppClientExtensionListener(
      std::shared_ptr<ArkWebAppClientExtensionCallback> callback) override;
#endif
  void RegisterWebAppClientExtensionListener(
      std::shared_ptr<NWebAppClientExtensionCallback>
          web_app_client_extension_listener) override;
  void RegisterDownLoadListener(
      std::shared_ptr<NWebDownloadCallback> downloadListener) override;
  void RegisterReleaseSurfaceListener(
      std::shared_ptr<NWebReleaseSurfaceCallback> releaseSurfaceListener)
      override;

  void RegisterWebDownloadDelegateListener(
      std::shared_ptr<NWebDownloadDelegateCallback> downloadDelegateListener)
      override;
  void StartDownload(const char* url) override;
  void ResumeDownload(std::shared_ptr<NWebDownloadItem> web_download) override;
#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
  NWebDownloadItemState GetDownloadItemState(long item_id) override;
#endif

  void RegisterNWebHandler(std::shared_ptr<NWebHandler> handler) override;
#if BUILDFLAG(ARKWEB_AI)
  void RegisterNWebAgentHandler(
      std::shared_ptr<NWebAgentHandler> handler) override;
#endif
  void RegisterRenderCb(
      std::function<void(const char*)> render_update_cb) override;

  void SetInputMethodClient(CefRefPtr<NWebInputMethodClient> client) override;

#if BUILDFLAG(ARKWEB_SCREEN_OFFSET)
  void SetScreenOffset(double x, double y) override;
#endif  // BUILDFLAG(ARKWEB_SCREEN_OFFSET)

  void Resize(uint32_t width,
              uint32_t height,
              bool isKeyboard = false) override;
#if BUILDFLAG(ARKWEB_INPUT_EVENTS) || BUILDFLAG(ARKWEB_VIEWPORT_AVOID)
  void ResizeVisibleViewport(uint32_t width,
                             uint32_t height,
                             bool isKeyboard = false) override;
#endif
#if BUILDFLAG(ARKWEB_VIEWPORT_AVOID)
  void AvoidVisibleViewportBottom(int32_t avoidHeight) override;
  int32_t GetVisibleViewportAvoidHeight() override;
#endif
  void OnTouchPress(int32_t id, double x, double y, bool from_overlay) override;
  void OnTouchRelease(int32_t id,
                      double x,
                      double y,
                      bool from_overlay) override;
  void OnTouchMove(int32_t id, double x, double y, bool from_overlay) override;
  void OnTouchMove(
      const std::vector<std::shared_ptr<NWebTouchPointInfo>>& touch_point_infos,
      bool from_overlay = false) override;
  void OnStylusTouchPress(
      std::shared_ptr<NWebStylusTouchPointInfo> stylus_touch_point_info,
      bool from_overlay) override;
  void OnStylusTouchRelease(
      std::shared_ptr<NWebStylusTouchPointInfo> stylus_touch_point_info,
      bool from_overlay) override;
  void OnStylusTouchMove(
      const std::vector<std::shared_ptr<NWebStylusTouchPointInfo>>&
          stylus_touch_point_infos,
      bool from_overlay) override;
  void OnTouchCancel() override;
  void OnTouchCancelById(int32_t id,
                         double x,
                         double y,
                         bool from_overlay) override;
  bool SendKeyEvent(int32_t keyCode, int32_t keyAction) override;
  void SendTouchpadFlingEvent(double x,
                              double y,
                              double vx,
                              double vy) override;
  void SendMouseWheelEvent(double x,
                           double y,
                           double deltaX,
                           double deltaY) override;
  void SendMouseEvent(int x, int y, int button, int action, int count) override;
  void NotifyScreenInfoChanged(RotationType rotation,
                               DisplayOrientation orientation) override;

  int Load(const std::string& url) override;
  bool IsNavigatebackwardAllowed() const override;
  bool IsNavigateForwardAllowed() const override;
  bool CanNavigateBackOrForward(int num_steps) const override;
  void NavigateBack() const override;
  void NavigateForward() const override;
  void NavigateBackOrForward(int32_t step) const override;
  void DeleteNavigateHistory() override;
  void ClearSslCache() override;
  void ClearClientAuthenticationCache() override;
  void Reload() const override;
  void ReloadOriginalUrl() const override;
  int Zoom(float zoomFactor) const override;
  int ZoomIn() const override;
  int ZoomOut() const override;
  void Stop() const override;
  void ExecuteJavaScript(const std::string& code) const override;
  void PutBackgroundColor(int color) const override;
  void InitialScale(float scale) const override;
  void PutOptimizeParserBudgetEnabled(bool enable) const override;
  void OnPause() override;
  void OnContinue() override;
  void WebComponentsBlur() override;
  void OnOccluded() override;
  void OnUnoccluded() override;
  void SetEnableLowerFrameRate(bool enabled) override;
  void SetEnableHalfFrameRate(bool enabled) override;
  std::shared_ptr<NWebPreference> GetPreference() const override;
#if BUILDFLAG(ARKWEB_AI)
  std::shared_ptr<NWebAgentManager> GetAgentManager() const override;
#endif
  std::string Title() override;
  std::shared_ptr<HitTestResult> GetHitTestResult() const override;
  std::shared_ptr<HitTestResult> GetLastHitTestResult() const override;
  int PageLoadProgress() override;
  float Scale() override;
  int Load(
      const std::string& url,
      const std::map<std::string, std::string>& additionalHttpHeaders) override;
  int LoadWithDataAndBaseUrl(const std::string& baseUrl,
                             const std::string& data,
                             const std::string& mimeType,
                             const std::string& encoding,
                             const std::string& historyUrl) override;
  int LoadWithData(const std::string& data,
                   const std::string& mimeType,
                   const std::string& encoding) override;
#if BUILDFLAG(ARKWEB_EXT_HTTPS_UPGRADES)
  int LoadUrlWithParams(const std::string& url,
                        const LoadUrlType load_type,
                        const std::string& refer,
                        const std::string& headers,
                        const std::string& post_data,
                        const bool allow_https_upgrade,
                        int32_t transition_type) override;
#endif
  int ContentHeight() override;

  void RegisterNativeArkJSFunction(
      const char* objName,
      const std::vector<std::shared_ptr<NWebJsProxyCallback>>& callbacks)
      override;
  void RegisterNativeJSProxy(
      const std::string& objName,
      const std::vector<std::string>& methodName,
      std::vector<std::function<char*(std::vector<std::vector<uint8_t>>&,
                                      std::vector<size_t>&)>>&& callback,
      bool isAsync,
      const std::string& permission) override;
  void RegisterNativeJSProxyWithResult(
      const std::string& objName,
      const std::vector<std::string>& methodName,
      std::vector<std::function<std::shared_ptr<OHOS::NWeb::NWebValue>(
          std::vector<std::vector<uint8_t>>&,
          std::vector<size_t>&)>>&& callback,
      bool isAsync,
      const std::string& permission) override;
  void RegisterNativeAsyncThreadJSProxyWithResult(
      const std::string& objName,
      const std::vector<std::string>& methodName,
      std::vector<std::function<std::shared_ptr<OHOS::NWeb::NWebValue>(
          std::vector<std::vector<uint8_t>>&,
          std::vector<size_t>&)>>&& callback,
      const std::string& permission) override;
  void UnRegisterNativeArkJSFunction(const char* objName) override;
  void RegisterNativeLoadStartCallback(
      std::function<void(void)>&& callback) override;
  void RegisterNativeLoadEndCallback(
      std::function<void(void)>&& callback) override;
  void RegisterNativeScrollCallback(
      std::function<void(double, double)>&& callback) override;

#if BUILDFLAG(ARKWEB_ADBLOCK)
  void UpdateAdblockEasyListRules(long adBlockEasyListVersion) override;
#endif

  void RegisterArkJSfunction(const std::string& object_name,
                             const std::vector<std::string>& method_list,
                             const std::vector<std::string>& async_method_list,
                             const int32_t object_id,
                             const std::string& permission) const override;

  void UnregisterArkJSfunction(
      const std::string& object_name,
      const std::vector<std::string>& method_list) const override;

#if BUILDFLAG(ARKWEB_JSPROXY)
  void JavaScriptOnDocumentStart(const ScriptItems& scriptItems) override;
  void JavaScriptOnDocumentEnd(const ScriptItems& scriptItems) override;

  void JavaScriptOnDocumentStartByOrder(
      const ScriptItems& scriptItems,
      const ScriptRegexItems& scriptRegexItems,
      const ScriptItemsByOrder& scriptItemsByOrder) override;
  void JavaScriptOnDocumentEndByOrder(
      const ScriptItems& scriptItems,
      const ScriptRegexItems& scriptRegexItems,
      const ScriptItemsByOrder& scriptItemsByOrder) override;

  void JavaScriptOnHeadReadyByOrder(
      const ScriptItems& scriptItems,
      const ScriptRegexItems& scriptRegexItems,
      const ScriptItemsByOrder& scriptItemsByOrder) override;
#endif

  void CallH5Function(
      int32_t routing_id,
      int32_t h5_object_id,
      const std::string& h5_method_name,
      const std::vector<std::shared_ptr<NWebValue>>& args) const override;
  void CallH5FunctionV2(
      int32_t routing_id,
      int32_t h5_object_id,
      const std::string& h5_method_name,
      const std::vector<std::shared_ptr<NWebRomValue>>& args) const override;

  void RegisterNWebJavaScriptCallBack(
      std::shared_ptr<NWebJavaScriptResultCallBack> callback) override;
  bool OnFocus(const FocusReason& focusReason =
                   FocusReason::FOCUS_DEFAULT) const override;
  void OnBlur() const override;

  void RegisterFindListener(
      std::shared_ptr<NWebFindCallback> find_listener) override;
  void FindAllAsync(const std::string& search_string) const override;
  void ClearMatches() const override;
  void FindNext(const bool forward) const override;
  bool TerminateRenderProcess() override;
  std::string GetUrl() const override;
  const std::string GetOriginalUrl() override;
  bool GetFavicon(const void** data,
                  size_t& width,
                  size_t& height,
                  ImageColorType& colorType,
                  ImageAlphaType& alphaType) override;
  void PutNetworkAvailable(bool available) override;

  void SetWindowId(uint32_t window_id) override;

  void SuggestionSelected(int index) const override;

  bool SetZoomInFactor(float factor);
  bool SetZoomOutFactor(float factor);

  bool IsFileProtocol(const GURL& gurl);
  bool IsUrlFileExist(const GURL& gurl, const std::string& url);

#if BUILDFLAG(ARKWEB_PDF)
  uint32_t create_pdf_value_callback_id_ = 0;
  std::unordered_map<uint32_t, CefRefPtr<CefPdfValueCallbackImpl>>
      create_pdf_value_callback_map_;
  void EraseCreatePDFCallbackImpl(uint32_t id) override;
  void ExecuteCreatePDFExt(
      std::shared_ptr<NWebPDFConfigArgs> pdfConfig,
      std::shared_ptr<NWebArrayBufferValueCallback> callback) override;
#endif  // BUILDFLAG(ARKWEB_PDF)

  void ExecuteJavaScriptExt(const int fd,
                            const size_t scriptLength,
                            std::shared_ptr<NWebMessageValueCallback> callback,
                            bool extention) override;

  int ScaleGestureChange(double scale,
                         double centerX,
                         double centerY) const override;

  int ScaleGestureChangeV2(int type,
                           double scale,
                           double originScale,
                           double centerX,
                           double centerY) const override;

  std::vector<std::string> CreateWebMessagePorts() override;
#if BUILDFLAG(ARKWEB_MSGPORT)
  void ClosePort(const std::string& port_handle) override;
  void PostWebMessage(const std::string& message,
                      const std::vector<std::string>& ports,
                      const std::string& targetUri) override;
  void PostPortMessage(const std::string& port_handle,
                       std::shared_ptr<NWebMessage> data) override;
  void PostPortMessageV2(const std::string& port_handle,
                       std::shared_ptr<NWebRomValue> data) override;
  void SetPortMessageCallback(
      const std::string& port_handle,
      std::shared_ptr<NWebMessageValueCallback> callback) override;
  uint32_t runJSCallbackId_ = 0;
  std::unordered_map<uint32_t, CefRefPtr<JavaScriptResultCallbackImpl>>
      runJSCallbackMap_;
  void EraseJavaScriptCallbackImpl(uint32_t id) override;
  void ExecuteJavaScript(const std::string& code,
                         std::shared_ptr<NWebMessageValueCallback> callback,
                         bool extention) override;
#endif  // BUILDFLAG(ARKWEB_MSGPORT)

  void SetNWebId(uint32_t nwebId) override;

#if BUILDFLAG(IS_ARKWEB)
  void EnableAppLinking(bool enable) override;
#endif // BUILDFLAG(IS_ARKWEB)

  void StoreWebArchive(
      const std::string& base_name,
      bool auto_name,
      std::shared_ptr<NWebStringValueCallback> callback) const override;
  void SetBrowserUserAgentString(const std::string& user_agent) override;
  void SendDragEvent(const DelegateDragEvent& dragEvent) const override;
#if BUILDFLAG(ARKWEB_I18N)
  void UpdateLocale(const std::string& language,
                    const std::string& region) override;
#endif  // #if BUILDFLAG(ARKWEB_I18N)
  CefRefPtr<ArkWebClientExt> GetCefClient() const override {
    return handler_delegate_;
  }
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  std::string GetAppTempDir() const override { return ohos_temp_dir_; }
  void InitAppTempDir();
  std::shared_ptr<NWebDragData> GetOrCreateDragData() override;
  void ClearDragData() const;
  std::string ohos_temp_dir_;
  bool DarkModeEnabled() override;
#endif  // ARKWEB_DRAG_DROP
  void GetImages(std::shared_ptr<NWebBoolValueCallback> callback) override;
  void RemoveCache(bool include_disk_files) override;

#if BUILDFLAG(ARKWEB_NAVIGATION)
  std::shared_ptr<NWebHistoryList> GetHistoryList() override;
  std::vector<uint8_t> SerializeWebState() override;
  bool RestoreWebState(const std::vector<uint8_t>& state) override;
#endif  // BUILDFLAG(ARKWEB_NAVIGATION)

#if BUILDFLAG(ARKWEB_NWEB_EX)
  bool CanStoreWebArchive() const override;
  void UnRegisterArkWebAppClientExtensionListener() override;
  void UnRegisterWebAppClientExtensionListener() override;
  void UnRegisterWebExtensionListener() override;
  void RegisterWebExtensionListener(
      std::shared_ptr<NWebExtensionCallback> web_extension_listener) override;
  void GetImageFromContextNode() override;
  void GetImageFromCacheEx(const std::string& url) override;
#endif

#if BUILDFLAG(ARKWEB_MEDIA_MUTE_AUDIO)
  void SetAudioMuted(bool muted) override;
#endif  // BUILDFLAG(ARKWEB_MEDIA_MUTE_AUDIO)

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
  void SetAudioResumeInterval(int32_t resumeInterval) override;
  void SetAudioExclusive(bool audioExclusive) override;
  void SetAudioSessionType(int32_t audioSessionType) override;
  void CloseAllMediaPresentations() override;
  void StopAllMedia() override;
  void ResumeAllMedia() override;
  void PauseAllMedia() override;
  int GetMediaPlaybackState() override;
#endif  // BUILDFLAG(ARKWEB_MEDIA_POLICY)

  // #if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
  void PrefetchPage(const PrefetchOptions& prefetch_options) override;
  // #endif  // BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void SetVirtualKeyBoardArg(int32_t width,
                             int32_t height,
                             double keyboard) override;
  bool ShouldVirtualKeyboardOverlay() override;
#endif

#if BUILDFLAG(ARKWEB_SAFEBROWSING)
  bool IsSafeBrowsingEnabled() override;
  void EnableSafeBrowsing(bool enable) override;
  void EnableSafeBrowsingDetection(bool enable, bool strictMode) override;
  void OnSafeBrowsingDetectionResult(int code,
                                     int policy,
                                     const std::string& mappingType,
                                     const std::string& url) override;
#endif  // BUILDFLAG(ARKWEB_SAFEBROWSING)

#if BUILDFLAG(IS_OHOS)
  void PrecompileJavaScript(
      const std::string& url,
      const std::string& script,
      std::shared_ptr<CacheOptions>& cacheOptions,
      std::shared_ptr<NWebMessageValueCallback> callback) override;
  void UpdateNativeEmbedInfo(
      std::shared_ptr<NWebNativeEmbedDataInfo> info) override;
  bool HitNativeArea(double x, double y);
#endif
#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
  void SetTransformHint(uint32_t rotation) override;
#endif

#if BUILDFLAG(ARKWEB_SECURITY_STATE)
  int GetSecurityLevel() override;
#endif

#if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)
  void PageUp(bool top) override;
  void PageDown(bool bottom) override;
#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
  void GetScrollOffset(float* offset_x, float* offset_y) override;
#endif
#endif  // #if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void SetNWebDelegateInterface(
      std::shared_ptr<NWebDelegateInterface> client) override;
  void ScrollTo(float x, float y) override;
  void ScrollBy(float delta_x, float delta_y) override;
  void ScrollByRefScreen(float delta_x,
                         float delta_y,
                         float vx,
                         float vy) override;
  void SlideScroll(float vx, float vy) override;
  bool WebSendKeyEvent(int32_t keyCode,
                       int32_t keyAction,
                       const std::vector<int32_t>& pressedCodes) override;
  void WebSendMouseWheelEvent(
      double x,
      double y,
      double deltaX,
      double deltaY,
      const std::vector<int32_t>& pressedCodes) override;
  void WebSendMouseWheelEventV2(
      double x,
      double y,
      double deltaX,
      double deltaY,
      const std::vector<int32_t>& pressedCodes,
      int32_t source) override;
  void WebSendTouchpadFlingEvent(
      double x,
      double y,
      double vx,
      double vy,
      const std::vector<int32_t>& pressedCodes) override;
  bool SendKeyboardEvent(const std::shared_ptr<OHOS::NWeb::NWebKeyboardEvent>&
                             keyboardEvent) override;
  bool ScrollByWithResult(float delta_x, float delta_y) override;
  void ScrollToWithAnime(float x, float y, int32_t duration) override;
  void ScrollByWithAnime(float delta_x,
                         float delta_y,
                         int32_t duration) override;
  void WebSendMouseEvent(
      const std::shared_ptr<OHOS::NWeb::NWebMouseEvent>& mouseEvent) override;
#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
  void GetOverScrollOffset(float* offset_x, float* offset_y) override;
#endif
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_ADBLOCK)
  void EnableAdsBlock(bool enable) override;
  bool IsAdsBlockEnabled() override;
  bool IsAdsBlockEnabledForCurPage() override;
  void SetAdBlockEnabledForSite(bool is_adblock_enabled,
                                int main_frame_tree_node_id) override;
#endif

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  void ProcessAutofillCancel(const std::string& fillContent) override;
#endif

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
  void SetSavePasswordAutomatically(bool enable) override;
  bool GetSavePasswordAutomatically() override;
  void SetSavePassword(bool enable) override;
  bool GetSavePassword() override;
  void SaveOrUpdatePassword(bool is_udpate) override;
#endif
#if BUILDFLAG(ARKWEB_EXT_PASSWORD) || BUILDFLAG(ARKWEB_DATALIST)
  void PasswordSuggestionSelected(int list_index) const override;
#endif
#if BUILDFLAG(ARKWEB_EXT_FORCE_ZOOM) || BUILDFLAG(ARKWEB_ZOOM)
  void SetForceEnableZoom(bool forceEnableZoom) override;
#endif
#if BUILDFLAG(ARKWEB_EXT_FORCE_ZOOM)
  bool GetForceEnableZoom() override;
#endif
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  void ShowFreeCopyMenu() override;
  bool ShouldShowFreeCopyMenu() override;
  std::string GetSelectedTextFromContextParam() override;
#endif
#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
  void SetShouldFrameSubmissionBeforeDraw(bool should) override;
  void SetDrawRect(int32_t x,
                   int32_t y,
                   int32_t width,
                   int32_t height) override;
  void SetDrawMode(int32_t mode) override;
  bool GetPendingSizeStatus() override;
  void SetFitContentMode(int32_t mode) override;
  std::string GetCurrentLanguage() override;
#endif  // BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  void SetNativeInnerWeb(bool isInnerWeb) override;
  bool GetNativeEmbedMode() override;
  bool IsEnableCustomVideoPlayer() override;
#endif
#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
  void NotifyPopupWindowResult(bool result) override;
#endif  // BUILDFLAG(ARKWEB_MULTI_WINDOW)
#if BUILDFLAG(ARKWEB_CA)
  bool GetCertChainDerData(std::vector<std::string>& certChainData,
                           bool isSingleCert) override;
#endif

#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
  void UpdateBrowserControlsState(int constraints,
                                  int current,
                                  bool animate) const override;
  void UpdateBrowserControlsHeight(int height, bool animate) override;
#endif

#if BUILDFLAG(ARKWEB_PRINT)
  void SetToken(void* token) override;
  void* CreateWebPrintDocumentAdapter(const std::string& jobName) override;
  void* CreateWebPrintDocumentAdapterV2(const std::string& jobName) override;
  void SetPrintBackground(bool enable) override;
  bool GetPrintBackground() override;
#endif  // BUILDFLAG(ARKWEB_PRINT)

#if BUILDFLAG(ARKWEB_SCREEN_ROTATION)
  void SetVirtualPixelRatio(float ratio) override;
#endif  // BUILDFLAG(ARKWEB_SCREEN_ROTATION)

#if BUILDFLAG(ARKWEB_POST_URL)
  int PostUrl(const std::string& url,
              const std::vector<char>& postData) override;
#endif  // BUILDFLAG(ARKWEB_POST_URL)
#if BUILDFLAG(ARKWEB_EXT_GET_ZOOM_LEVEL)
  void SetBrowserZoomLevel(double zoom_factor) override;
  double GetBrowserZoomLevel() override;
#endif
#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_CORNER)
  void SetBorderRadiusFromWeb(double borderRadiusTopLeft,
                              double borderRadiusTopRight,
                              double borderRadiusBottomLeft,
                              double borderRadiusBottomRight) override;
#endif  // ARKWEB_SCROLLBAR_AVOID_CORNER
#if BUILDFLAG(ARKWEB_MENU)
  void SetTouchHandleExistState(bool touchHandleExist) override;
  void SetViewportScaleState(bool viewportScale) override;
#endif  // BUILDFLAG(ARKWEB_MENU)
#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
  void SetAccessibilityState(cef_state_t accessibility_state) override;
  bool ExecuteAction(
      int64_t accessibilityId,
      uint32_t action,
      const std::map<std::string, std::string>& actionArguments) override;
  bool GetAccessibilityNodeRectById(int64_t accessibilityId,
                                    int32_t* width,
                                    int32_t* height,
                                    int32_t* offsetX,
                                    int32_t* offsetY) override;
  std::shared_ptr<NWebAccessibilityNodeInfo> GetFocusedAccessibilityNodeInfo(
      int64_t accessibilityId,
      bool isAccessibilityFocus) override;
  std::shared_ptr<NWebAccessibilityNodeInfo> GetAccessibilityNodeInfoById(
      int64_t accessibilityId) override;
  int64_t GetWebAccessibilityIdByHtmlElementId(const std::string& htmlElementId) override;
  bool GetAccessibilityVisible(int64_t accessibilityId) override;
  std::shared_ptr<NWebAccessibilityNodeInfo>
  GetAccessibilityNodeInfoByFocusMove(int64_t accessibilityId,
                                      int32_t direction) override;
  void RefreshAccessibilityManagerClickEvent() override;
  void SendAccessibilityHoverEvent(int x, int y, bool isHoverEnter) override;
#endif
  void SetAutofillCallback(
      std::shared_ptr<NWebMessageValueCallback> callback) override;
  void FillAutofillData(std::shared_ptr<NWebMessage> data) override;
  void FillAutofillDataV2(std::shared_ptr<NWebRomValue> data) override;
  void FillAutofillDataFromTriggerType(
      std::shared_ptr<NWebRomValue> data, int32_t type) override;
  void PutVaultPlainTextCallback(
      std::shared_ptr<OHOS::NWeb::NWebVaultPlainTextCallback> callback) override;
  void StopFling() override;

#if BUILDFLAG(ARKWEB_WEBRTC)
  void StartCamera() override;
  void StopCamera() override;
  void CloseCamera() override;
  void ResumeMicrophone() override;
  void StopMicrophone() override;
  void PauseMicrophone() override;
#endif  // BUILDFLAG(ARKWEB_WEBRTC)

#if BUILDFLAG(ARKWEB_DISCARD)
  bool Discard() override;
  bool Restore() override;
#endif

#if BUILDFLAG(ARKWEB_ITP)
  void EnableIntelligentTrackingPrevention(bool enable) override;
  bool IsIntelligentTrackingPreventionEnabled() const override;
#endif

#if BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)
  void StopScreenCapture(int32_t nweb_id, const char* session_id) override;
  void SetScreenCapturePickerShow() override;
  void DisableSessionReuse() override;
  void RegisterScreenCaptureDelegateListener(
      std::shared_ptr<NWebScreenCaptureDelegateCallback> listener) override;
#endif  // defined(ARKWEB_EX_SCREEN_CAPTURE)

#if BUILDFLAG(ARKWEB_SCREEN_LOCK)
  void SetWakeLockCallback(
      int32_t windowId,
      const std::shared_ptr<NWebScreenLockCallback>& callback) override;
#endif

#if BUILDFLAG(ARKWEB_SECURE_JAVASCRIPT_PROXY)
  std::string GetLastJavascriptProxyCallingFrameUrl() override;
#endif

#if BUILDFLAG(ARKWEB_MENU)
  void SetIsRichText(bool is_rich_text) override;
#endif
  std::string GetSelectInfo() override;

#if BUILDFLAG(ARKWEB_AI_WRITE)
  int GetSelectStartIndex() override;
  int GetSelectEndIndex() override;
  std::string GetAllTextInfo() override;
#endif // ARKWEB_AI_WRITE
#if BUILDFLAG(ARKWEB_ACTIVE_POLICY)
  void SetDelayDurationForBackgroundTabFreezing(int64_t delay) override;
#endif

#if BUILDFLAG(ARKWEB_AI)
  void OnTextSelected() override;
  void OnDestroyImageAnalyzerOverlay() override;
  void OnFoldStatusChanged(FoldStatus foldstatus) override;
  void RunDataDetectorJS() override;
  void SetDataDetectorEnable(bool enable) override;
  bool GetDataDetectorEnable() override;
  std::string GetDataDetectorSelectText() override;
  void OnDataDetectorSelectText() override;
  void OnDataDetectorCopy(const std::vector<std::string>& recordMix) override;
#endif

#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
  void OnSafeInsetsChange(int left, int top, int right, int bottom) override;
#endif

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  void SetWholePageDrawing() override;

  bool WebPageSnapshot(const char* id,
                       PixelUnit type,
                       int width,
                       int height,
                       const WebSnapshotCallback callback) override;
#endif

#if BUILDFLAG(ARKWEB_URL_TRUST_LIST)
  int SetUrlTrustListWithErrMsg(const std::string& urlTrustList,
                                std::string& detailErrMsg) override;
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  void SetPathAllowingUniversalAccess(
      const std::vector<std::string>& path_list,
      const std::vector<std::string>& excluded_path_list) override;
  int PrerenderPage(const std::string& url,
                    const std::string& additional_headers) override;
  void CancelAllPrerendering() override;
#endif

#if BUILDFLAG(ARKWEB_EXT_FILE_ACCESS)
  void DisallowSandboxFileAccessFromFileUrl(bool disallow) override;
#endif  // BUILDFLAG(ARKWEB_EXT_FILE_ACCESS)

#if BUILDFLAG(ARKWEB_MIXED_CONTENT)
  void EnableMixedContentAutoUpgrades(bool enable) override;
  bool IsMixedContentAutoUpgradesEnabled() override;
#endif

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
  void SetUsageScenario(int32_t usage_scenario) override;
  int32_t GetUsageScenario() override;
#endif

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  void WebExtensionTabRemoved(
      int tab_id,
      bool isWindowClosing,
      int windowId) override;
  void WebExtensionTabUpdated(
      int tab_id,
      std::unique_ptr<NWebExtensionTabChangeInfo> changeInfo,
      std::unique_ptr<NWebExtensionTab> tab) override;
  void WebExtensionTabAttached(
      int tab_id,
      std::unique_ptr<NWebExtensionTabAttachInfo> attachInfo) override;
  void WebExtensionTabDetached(
      int tab_id,
      std::unique_ptr<NWebExtensionTabDetachInfo> detachInfo) override;
  void WebExtensionTabHighlighted(NWebExtensionTabHighlightInfo& highlightInfo) override;
  void WebExtensionTabMoved(
      int32_t tab_id,
      std::unique_ptr<NWebExtensionTabMoveInfo> moveInfo) override;
  void WebExtensionTabReplaced(int32_t addedTabId,
                               int32_t removedTabId) override;
  void WebExtensionSetViewType(int32_t type) override;
#endif

#if BUILDFLAG(ARKWEB_EXT_PERMISSION)
  void PermissionRequestGrant(int32_t resourse_id,
                              int nweb_request_key) override;
  void PermissionRequestDeny(int nweb_request_key) override;
  std::string PermissionRequestGetOrigin(int nweb_request_key) override;
  int32_t PermissionRequestGetResourceId(int nweb_request_key) override;
  void PermissionRequestDelete(int nweb_request_key) override;
#endif  // ARKWEB_EXT_PERMISSION

#if BUILDFLAG(ARKWEB_BFCACHE)
  void SetBackForwardCacheOptions(int32_t size, int32_t timeToLive) override;
  void SetMediaResumeFromBFCachePage(bool resume) override;
#endif

#if BUILDFLAG(ARKWEB_MEDIA_NETWORK_TRAFFIC_PROMPT)
  void EnableMediaNetworkTrafficPrompt(bool enable) override;
#endif  // ARKWEB_MEDIA_NETWORK_TRAFFIC_PROMPT

#if BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
  void SetPopupSurface(void* popupSurface) override;
#endif

#if BUILDFLAG(ARKWEB_NWEB_EX)
  void EnableViewAutoResize(
      const CefSize& min_size,
      const CefSize& max_size) override;
  void DisableViewAutoResize() override;
#endif

#if BUILDFLAG(ARKWEB_DISATCH_BEFORE_UNLOAD)
  bool NeedToFireBeforeUnloadOrUnloadEvents() override;
  void DispatchBeforeUnload() override;
#endif  // ARKWEB_DISATCH_BEFORE_UNLOAD

#if BUILDFLAG(ARKWEB_EX_REFRESH_IFRAME)
  bool WebExtensionContextMenuIsIframe() override;
  void WebExtensionContextMenuReloadFocusedFrame() override;
#endif

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  void WebExtensionContextMenuGetFocusedFrameInfo(
      int32_t& frame_id,
      std::string& frame_url) override;
#endif

  void SetSurfaceDensity(const double& density) override;

  void OpenDevtoolsWith(
      std::shared_ptr<NWebDelegateInterface> nweb_delegate,
      std::unique_ptr<OpenDevToolsParam> param) override;
  void CloseDevtools() override;

  int32_t GetArgumentByKey(
      const std::map<std::string, std::string>& actionArguments,
      const std::string& checkKey) const;

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
  void MaximizeResize() override;
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  bool SetFocusByPosition(float x, float y) override;
  std::pair<double, double> GetLastTouchMousePosition() override {
    return last_touch_mouse_position_;
  }
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
#if BUILDFLAG(ARKWEB_PIP)
  void SetPipNativeWindow(int delegate_id,
                          int child_id,
                          int frame_routing_id,
                          void* window) override;
  void SendPipEvent(int delegate_id,
                    int child_id,
                    int frame_routing_id,
                    int event) override;
#endif

#if BUILDFLAG(ARKWEB_MENU)
  void UpdateSingleHandleVisible(bool isVisible) override;
#endif

#if BUILDFLAG(ARKWEB_NWEB_EX)
  void RunJavaScriptInFrames(RunJavaScriptParam param,
                             OnReceiveValueCallback callback) override;
#endif

#if BUILDFLAG(ARKWEB_READER_MODE)
void Distill(const std::string& guid, const DistillOptions& distill_options, DistillCallback callback) override;
void AbortDistill() override;
#endif // ARKWEB_READER_MODE

#if BUILDFLAG(ARKWEB_ERROR_PAGE)
  void SetErrorPageEnabled(bool enable) override;
  bool GetErrorPageEnabled() override;
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  void SetBlanklessLoadingKey(uint32_t nweb_id, uint64_t blankless_key) override;
  int64_t GetPreferenceHash() override;
  int32_t GetWidth() override;
  int32_t GetHeight() override;
  void SetRotationType(RotationType rotation);
#endif
#if BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
  void SetBlankScreenDetectionConfig(
      bool enable,
      const std::vector<double>& detectionTiming,
      const std::vector<int32_t>& detectionMethods,
      int32_t contentfulNodesCountThreshold) override;
#endif
#if BUILDFLAG(ARKWEB_BGTASK)
  void OnBrowserForeground() override;
  void OnBrowserBackground() override;
#endif

#if BUILDFLAG(ARKWEB_EXT_HTTPS_UPGRADES)
  void EnableHttpsUpgrades(bool enable) override;
#endif

#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
void SetFocusWebId(int32_t nweb_id) override;
#endif

 public:
  int argc_;
  RAW_PTR_EXCLUSION const char** argv_;

 private:
  void RunMessageLoop();

  void InitializeCef(std::string url,
                     bool is_enhance_surface,
                     void* window,
                     bool popup
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
                     ,
                     uint32_t nweb_id
#endif
#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
                     ,
                     bool incognito_mode
#endif
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
                     ,
                     const std::string& shared_render_process_token
#endif

  );

  const CefRefPtr<ArkWebBrowserExt> GetBrowser() const;
  void RequestVisitedHistory();
  bool HasBackgroundColorWithInit(int32_t& backgroundColor);
  void InitRichtextIdentifier();
#if BUILDFLAG(ARKWEB_API_INIT_WEB_ENGINE)
  void OnContextInitializeComplete(
      const std::string& url,
      void* windows
#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
      ,
      bool incognito_mode
#endif
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
      ,
      const std::string& shared_render_process_token
#endif

  );
#endif  // BUILDFLAG(ARKWEB_API_INIT_WEB_ENGINE)
#if BUILDFLAG(ARKWEB_MSGPORT)
  void ConvertNWebMsgToCefValue(std::shared_ptr<NWebMessage> data,
                                CefRefPtr<CefValue> message);
  CefRefPtr<CefValue> ConvertRomValueToCefValue(std::shared_ptr<NWebRomValue> data);
#endif
#if BUILDFLAG(ARKWEB_CA)
  bool GetCertChainDerDataInner(CefRefPtr<CefX509Certificate> cert,
                                std::vector<std::string>& certChainData,
                                bool isSingleCert);
#endif
#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  void RegisterOnCreateNativeMediaPlayerListener(
      std::shared_ptr<NWebCreateNativeMediaPlayerCallback> callback) override;
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  void EnableVideoAssistant(bool enable) override;
  void ExecuteVideoAssistantFunction(const std::string& cmd_id) override;
  void CustomWebMediaPlayer(bool enable) override;
  void WebMediaPlayerControllerPlay() override;
  void WebMediaPlayerControllerPause() override;
  void WebMediaPlayerControllerSeek(double time) override;
  void WebMediaPlayerControllerSetMuted(bool muted) override;
  void WebMediaPlayerControllerSetPlaybackRate(double playback_rate) override;
  void WebMediaPlayerControllerExitFullscreen() override;
  void WebMediaPlayerControllerSetVideoSurface(void* native_window) override;
  void WebMediaPlayerControllerDownload() override;
  void WebMediaPlayerControllerSetVolume(double volume) override;
  double WebMediaPlayerControllerGetVolume() override;
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
  BrowserAccessibilityManagerOHOS* GetAccessibilityManager() const;
  int64_t GetRealAccessibilityId(int64_t accessibilityId) const;
  void AddAccessibilityNodeInfoAttributes(
      std::shared_ptr<NWebAccessibilityNodeInfoImpl> nodeInfo,
      const BrowserAccessibilityOHOS* node) const;
  void AddAccessibilityNodeInfoRect(
      std::shared_ptr<NWebAccessibilityNodeInfoImpl> nodeInfo,
      const BrowserAccessibilityOHOS* node) const;
  void AddAccessibilityNodeInfoCollection(
      std::shared_ptr<NWebAccessibilityNodeInfoImpl> nodeInfo,
      const BrowserAccessibilityOHOS* node) const;
  std::shared_ptr<NWebAccessibilityNodeInfo> PopulateAccessibilityNodeInfo(
      BrowserAccessibilityOHOS* node);
  void AddAccessibilityNodeInfoActions(
      std::shared_ptr<NWebAccessibilityNodeInfoImpl> nodeInfo,
      const BrowserAccessibilityOHOS* node) const;
  float GetViewPointHeight() const;
#endif

#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
  int InsertBackForwardEntry(int index, const std::string& url) override;
  int UpdateNavigationEntryUrl(int index, const std::string& url) override;
  void ClearForwardList() override;
#endif

  std::shared_ptr<NWebCustomKeyboardHandlerImpl> GetCustomKeyboardHandler()
      const override {
    if (render_handler_) {
      return render_handler_->GetCustomKeyboardHandler();
    }
    return nullptr;
  }

  bool IsCustomKeyboard() const override {
    if (render_handler_) {
      return render_handler_->IsCustomKeyboard();
    }
    return true;
  }

  float zoom_in_factor_ = 1.25f;
  float zoom_out_factor_ = 0.8f;
  float default_virtual_pixel_ratio_ = 2.0;
  float intial_scale_ = 0;
  bool has_requested_visited_history = false;
  CefRefPtr<NWebApplication> nweb_app_ = nullptr;
  CefRefPtr<NWebHandlerDelegate> handler_delegate_ = nullptr;
  CefRefPtr<NWebRenderHandler> render_handler_ = nullptr;
  std::shared_ptr<NWebEventHandler> event_handler_ = nullptr;
  std::shared_ptr<NWebPreferenceDelegate> preference_delegate_ = nullptr;
  std::shared_ptr<NWebFindDelegate> find_delegate_ = nullptr;
  std::unique_ptr<OHOS::NWeb::DisplayManagerAdapter> display_manager_adapter_ =
      nullptr;
  std::shared_ptr<OHOS::NWeb::DisplayScreenListener> display_listener_ =
      nullptr;
  int32_t display_listener_id_ = 0;
#if BUILDFLAG(ARKWEB_AI)
  std::shared_ptr<OHOS::NWeb::FoldStatusScreenListener> foldstatus_listener_ =
      nullptr;
  int32_t foldstatus_listener_id_ = 0;
  std::shared_ptr<NWebAgentManagerImpl> agent_manager_ = nullptr;
#endif
  // Members only accessed on the main thread.
  bool hidden_ = false;
  bool occluded_ = false;
  bool is_popup_ready_ = false;
#if BUILDFLAG(ARKWEB_OFFLINE_WEB_EVICT_BACK_BUFFERS)
  bool hasEvictedBufferWhenHidden_ = false;
#endif
#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER) || BUILDFLAG(ARKWEB_PAGE_UP_DOWN) || BUILDFLAG(ARKWEB_VIEWPORT_AVOID) || \
    BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  uint32_t width_ = 0;
  uint32_t height_ = 0;
#endif  // BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  uint32_t visible_width_ = 0;
  uint32_t visible_height_ = 0;
#endif

  uint32_t nweb_id_ = 0;

#if BUILDFLAG(IS_OHOS)
  std::map<std::string, std::shared_ptr<NWebNativeEmbedDataInfo>>
      embedDataInfo_;
#endif

  bool is_enhance_surface_ = false;
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  bool is_onPause_ = false;
  std::pair<double, double> last_touch_mouse_position_;
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
  bool accessibility_state_ = false;
  bool is_hovering_ = false;
#endif
  std::string richtext_data_str_ = "";
  bool is_discarded_ = false;
  // The number of fingers that trigger the down event
  int pressing_num_ = 0;
  double display_ratio_ = 0.0;

#if BUILDFLAG(ARKWEB_AI)
  bool data_detector_enable_ = false;
#endif

#if BUILDFLAG(ARKWEB_VIEWPORT_AVOID)
  int32_t avoid_height_ = 0;
#endif
#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  int32_t nearest_snapshot_width_ = 0;
  int32_t nearest_snapshot_height_ = 0;
#endif
  base::WeakPtrFactory<NWebDelegate> weak_factory_{this};
};
}  // namespace OHOS::NWeb
#endif
