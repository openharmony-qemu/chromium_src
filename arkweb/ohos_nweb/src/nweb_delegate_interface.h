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

#ifndef NWEB_DELEGATE_INTERFACE_H
#define NWEB_DELEGATE_INTERFACE_H

#include <functional>
#include <string>

#include "capi/nweb_app_client_extension_callback.h"
#include "capi/nweb_download_delegate_callback.h"
#include "capi/nweb_extension_callback.h"
#include "capi/nweb_extension_javascript_item.h"
#include "cef_delegate/nweb_inputmethod_client.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/display_manager_adapter.h"
#include "include/arkweb_client_ext.h"
#include "nweb.h"
#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
#include "nweb_accessibility_node_info.h"
#endif
#include "arkweb/build/features/features.h"
#include "build/build_config.h"
#include "nweb_download_callback.h"
#include "nweb_find_callback.h"
#include "nweb_handler.h"
#include "nweb_preference.h"
#include "nweb_rom_value.h"
#include "nweb_web_message.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
#include "nweb_native_media_player.h"
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#include "cef_delegate/nweb_custom_keyboard_handler_impl.h"
#include "capi/nweb_context_menus_item.h"

#if BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)
#include "capi/nweb_screencapture_delegate_callback.h"
#endif  // defined(ARKWEB_EX_SCREEN_CAPTURE)

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "ohos_nweb_ex/core/extension/nweb_app_client_extension_dispatcher.h"
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
#include "ui/gfx/geometry/size.h"
#endif

#if BUILDFLAG(ARKWEB_READER_MODE)
#include "capi/nweb_extension_distill_item.h"
#endif // ARKWEB_READER_MODE

#if BUILDFLAG(ARKWEB_USERAGENT)
#include "nweb_user_agent_metadata.h"
#endif

struct OpenDevToolsParam;
struct OpenDevToolsExtOpt;
struct RunJavaScriptParam;

namespace OHOS::NWeb {
class NWebValue;

enum class DelegateDragAction {
  DRAG_START = 0,
  DRAG_ENTER,
  DRAG_LEAVE,
  DRAG_OVER,
  DRAG_DROP,
  DRAG_END,
  DRAG_CANCEL,
};

struct DelegateDragEvent {
  double x = 0.0;
  double y = 0.0;
  DelegateDragAction action = DelegateDragAction::DRAG_START;
  CefBrowserHost::DragOperationsMask op = CefBrowserHost::DragOperationsMask::DRAG_OPERATION_COPY;
  CefBrowserHost::DragOperationsMask allowed_op = CefBrowserHost::DragOperationsMask::DRAG_OPERATION_EVERY;
};

#if BUILDFLAG(ARKWEB_NAVIGATION)
using WebState = std::shared_ptr<std::vector<uint8_t>>;
#endif  // BUILDFLAG(ARKWEB_NAVIGATION)

class NWebDelegateInterface
    : public std::enable_shared_from_this<NWebDelegateInterface> {
 public:
  virtual ~NWebDelegateInterface() = default;
  virtual void SetIsOfflineWebComponent() = 0;
  virtual void OnWindowShow() = 0;
  virtual void OnWindowHide() = 0;
  virtual void OnOnlineRenderToForeground() = 0;
  virtual void NotifyForNextTouchEvent() = 0;
  virtual bool IsReady() = 0;
  virtual void OnDestroy(bool is_close_all) = 0;
  virtual void RegisterDownLoadListener(
      std::shared_ptr<NWebDownloadCallback> downloadListener) = 0;
  virtual void RegisterReleaseSurfaceListener(
      std::shared_ptr<NWebReleaseSurfaceCallback> releaseSurfaceListener) = 0;
  virtual void RegisterNWebHandler(std::shared_ptr<NWebHandler> handler) = 0;
#if BUILDFLAG(ARKWEB_AI)
  virtual void RegisterNWebAgentHandler(
      std::shared_ptr<NWebAgentHandler> handler) = 0;
#endif
  virtual void RegisterRenderCb(
      std::function<void(const char*)> render_update_cb) = 0;
#if BUILDFLAG(ARKWEB_NWEB_EX)
  virtual void RegisterArkWebAppClientExtensionListener(
      std::shared_ptr<ArkWebAppClientExtensionCallback> callback) = 0;
#endif
  virtual void RegisterWebAppClientExtensionListener(
      std::shared_ptr<NWebAppClientExtensionCallback>
          web_app_client_extension_listener) = 0;
  virtual void SetInputMethodClient(
      CefRefPtr<NWebInputMethodClient> client) = 0;
  virtual void RegisterWebDownloadDelegateListener(
      std::shared_ptr<NWebDownloadDelegateCallback>
          downloadDelegateListener) = 0;
  virtual void StartDownload(const char* url) = 0;
#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
  virtual void StartDownload(const char* url,
                             const DownloadUrlParameters& input_params) = 0;
#endif
  virtual void ResumeDownload(
      std::shared_ptr<NWebDownloadItem> web_download) = 0;
#if BUILDFLAG(ARKWEB_ACTIVE_POLICY)
  virtual void SetDelayDurationForBackgroundTabFreezing(int64_t delay) = 0;
#endif
  virtual void SetAutofillCallback(
      std::shared_ptr<NWebMessageValueCallback> callback) = 0;
  virtual void FillAutofillData(std::shared_ptr<NWebMessage> data) = 0;
  virtual void FillAutofillDataV2(std::shared_ptr<NWebRomValue> data) = 0;
  virtual void FillAutofillDataFromTriggerType(
      std::shared_ptr<NWebRomValue> data, int32_t type) = 0;
  virtual void PutVaultPlainTextCallback(
      std::shared_ptr<OHOS::NWeb::NWebVaultPlainTextCallback> callback) = 0;

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  virtual void SetNWebDelegateInterface(
      std::shared_ptr<NWebDelegateInterface> client) = 0;
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_ADBLOCK)
  virtual void EnableAdsBlock(bool enable) = 0;

  virtual bool IsAdsBlockEnabled() = 0;

  virtual bool IsAdsBlockEnabledForCurPage() = 0;

  virtual void SetAdBlockEnabledForSite(bool is_adblock_enabled,
                                        int main_frame_tree_node_id) = 0;
#endif

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  virtual void ProcessAutofillCancel(const std::string& fillContent) = 0;
#endif

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
  virtual void SetSavePasswordAutomatically(bool enable) = 0;
  virtual bool GetSavePasswordAutomatically() = 0;
  virtual void SetSavePassword(bool enable) = 0;
  virtual bool GetSavePassword() = 0;
  virtual void SaveOrUpdatePassword(bool is_update) = 0;
#endif  // BUILDFLAG(ARKWEB_EXT_PASSWORD)
#if BUILDFLAG(ARKWEB_EXT_PASSWORD) || BUILDFLAG(ARKWEB_DATALIST)
  virtual void PasswordSuggestionSelected(int list_index) const = 0;
#endif
#if BUILDFLAG(ARKWEB_NWEB_EX)
  virtual bool CanStoreWebArchive() const = 0;
  virtual void UnRegisterArkWebAppClientExtensionListener() = 0;
  virtual void UnRegisterWebAppClientExtensionListener() = 0;
  virtual void RegisterWebExtensionListener(
      std::shared_ptr<NWebExtensionCallback> web_extension_listener) = 0;
  virtual void UnRegisterWebExtensionListener() = 0;
  virtual void GetImageFromContextNode() = 0;
  virtual void GetImageFromCacheEx(const std::string& url) = 0;
#endif

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  virtual void ShowFreeCopyMenu() = 0;
  virtual bool ShouldShowFreeCopyMenu() = 0;
  virtual std::string GetSelectedTextFromContextParam() = 0;
#endif  // ARKWEB_EXT_FREE_COPY

#if BUILDFLAG(ARKWEB_EXT_GET_ZOOM_LEVEL)
  virtual void SetBrowserZoomLevel(double zoom_factor) = 0;
  virtual double GetBrowserZoomLevel() = 0;
#endif

#if BUILDFLAG(ARKWEB_SCREEN_OFFSET)
  virtual void SetScreenOffset(double x, double y) = 0;
#endif  // BUILDFLAG(ARKWEB_SCREEN_OFFSET)

  /* event interface */
  virtual void Resize(uint32_t width,
                      uint32_t height,
                      bool isKeyboard = false) = 0;
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  virtual void ResizeVisibleViewport(uint32_t width,
                                     uint32_t height,
                                     bool isKeyboard = false) = 0;
#endif
  virtual void OnTouchPress(int32_t id,
                            double x,
                            double y,
                            bool from_overlay) = 0;
  virtual void OnTouchRelease(int32_t id,
                              double x,
                              double y,
                              bool from_overlay) = 0;
  virtual void OnTouchMove(int32_t id,
                           double x,
                           double y,
                           bool from_overlay) = 0;
  virtual void OnTouchMove(
      const std::vector<std::shared_ptr<NWebTouchPointInfo>>& touch_point_infos,
      bool from_overlay) = 0;
  virtual void OnStylusTouchPress(
      std::shared_ptr<NWebStylusTouchPointInfo> stylus_touch_point_info,
      bool from_overlay) = 0;
  virtual void OnStylusTouchRelease(
      std::shared_ptr<NWebStylusTouchPointInfo> stylus_touch_point_info,
      bool from_overlay) = 0;
  virtual void OnStylusTouchMove(
      const std::vector<std::shared_ptr<NWebStylusTouchPointInfo>>&
          stylus_touch_point_infos,
      bool from_overlay) = 0;
  virtual void OnTouchCancel() = 0;
  virtual void OnTouchCancelById(int32_t id,
                                 double x,
                                 double y,
                                 bool from_overlay) = 0;

  virtual bool SendKeyEvent(int32_t keyCode, int32_t keyAction) = 0;
  virtual void SendTouchpadFlingEvent(double x,
                                      double y,
                                      double vx,
                                      double vy) = 0;
  virtual void SendMouseWheelEvent(double x,
                                   double y,
                                   double deltaX,
                                   double deltaY) = 0;
  virtual void SendMouseEvent(int x,
                              int y,
                              int button,
                              int action,
                              int count) = 0;
  virtual void NotifyScreenInfoChanged(RotationType rotation,
                                       DisplayOrientation orientation) = 0;

  virtual int Load(const std::string& url) = 0;
  virtual bool IsNavigatebackwardAllowed() const = 0;
  virtual bool IsNavigateForwardAllowed() const = 0;
  virtual bool CanNavigateBackOrForward(int numSteps) const = 0;
  virtual void NavigateBack() const = 0;
  virtual void NavigateForward() const = 0;
  virtual void NavigateBackOrForward(int32_t step) const = 0;
  virtual void DeleteNavigateHistory() = 0;
  virtual void ClearSslCache() = 0;
  virtual void ClearClientAuthenticationCache() = 0;
  virtual void Reload() const = 0;
  virtual void ReloadOriginalUrl() const = 0;
  virtual int Zoom(float zoomFactor) const = 0;
  virtual int ZoomIn() const = 0;
  virtual int ZoomOut() const = 0;
  virtual void Stop() const = 0;
  virtual void ExecuteJavaScript(const std::string& code) const = 0;
  virtual void PutBackgroundColor(int color) const = 0;
  virtual void InitialScale(float scale) const = 0;
  virtual void PutOptimizeParserBudgetEnabled(bool enable) const = 0;
  virtual void OnPause() = 0;
  virtual void OnContinue() = 0;
  virtual void WebComponentsBlur() = 0;
  virtual void OnOccluded() = 0;
  virtual void OnUnoccluded() = 0;
  virtual void SetEnableLowerFrameRate(bool enabled) = 0;
  virtual void SetEnableHalfFrameRate(bool enabled) = 0;
  virtual std::shared_ptr<NWebPreference> GetPreference() const = 0;
#if BUILDFLAG(ARKWEB_AI)
  virtual std::shared_ptr<NWebAgentManager> GetAgentManager() const = 0;
  virtual void RegisterOnLoadStartedCbForContentChange(
      std::function<void(void)>&& callback) = 0;
#endif
  virtual std::string Title() = 0;
  virtual std::shared_ptr<HitTestResult> GetHitTestResult() const = 0;
  virtual std::shared_ptr<HitTestResult> GetLastHitTestResult() const = 0;
  virtual int PageLoadProgress() = 0;
  virtual float Scale() = 0;
  virtual int Load(
      const std::string& url,
      const std::map<std::string, std::string>& additionalHttpHeaders) = 0;
  virtual int LoadWithDataAndBaseUrl(const std::string& baseUrl,
                                     const std::string& data,
                                     const std::string& mimeType,
                                     const std::string& encoding,
                                     const std::string& historyUrl) = 0;
  virtual int LoadWithData(const std::string& data,
                           const std::string& mimeType,
                           const std::string& encoding) = 0;
#if BUILDFLAG(ARKWEB_EXT_HTTPS_UPGRADES)
  virtual int LoadUrlWithParams(const std::string& url,
                                const LoadUrlType load_type,
                                const std::string& refer,
                                const std::string& headers,
                                const std::string& post_data,
                                const bool allow_https_upgrade,
                                int32_t transition_type) = 0;
#endif
  virtual int ContentHeight() = 0;

  virtual void RegisterNativeArkJSFunction(
      const char* objName,
      const std::vector<std::shared_ptr<NWebJsProxyCallback>>& callbacks) = 0;
  virtual void RegisterNativeJSProxy(
      const std::string& objName,
      const std::vector<std::string>& methodName,
      std::vector<std::function<char*(std::vector<std::vector<uint8_t>>&,
                                      std::vector<size_t>&)>>&& callback,
      bool isAsync,
      const std::string& permission) = 0;
  virtual void RegisterNativeJSProxyWithResult(
      const std::string& objName,
      const std::vector<std::string>& methodName,
      std::vector<std::function<std::shared_ptr<OHOS::NWeb::NWebValue>(
          std::vector<std::vector<uint8_t>>&,
          std::vector<size_t>&)>>&& callback,
      bool isAsync,
      const std::string& permission) = 0;
  virtual void RegisterNativeAsyncThreadJSProxyWithResult(
      const std::string& objName,
      const std::vector<std::string>& methodName,
      std::vector<std::function<std::shared_ptr<OHOS::NWeb::NWebValue>(
          std::vector<std::vector<uint8_t>>&, std::vector<size_t>&)>>&& callback,
      const std::string& permission) = 0;
  virtual void UnRegisterNativeArkJSFunction(const char* objName) = 0;

#if BUILDFLAG(ARKWEB_ADBLOCK)
  virtual void UpdateAdblockEasyListRules(long adBlockEasyListVersion) = 0;
#endif

  virtual void RegisterArkJSfunction(
      const std::string& object_name,
      const std::vector<std::string>& method_list,
      const std::vector<std::string>& async_method_list,
      const int32_t object_id,
      const std::string& permission) const = 0;
  virtual void UnregisterArkJSfunction(
      const std::string& object_name,
      const std::vector<std::string>& method_list) const = 0;
  virtual void RegisterNativeLoadStartCallback(
      std::function<void(void)>&& callback) = 0;
  virtual void RegisterNativeLoadEndCallback(
      std::function<void(void)>&& callback) = 0;
  virtual void RegisterNativeScrollCallback(
      std::function<void(double, double)>&& callback) = 0;
  virtual void JavaScriptOnDocumentStart(const ScriptItems& ScriptItems) = 0;
  virtual void JavaScriptOnDocumentEnd(const ScriptItems& ScriptItems) = 0;
  virtual void CallH5Function(
      int32_t routing_id,
      int32_t h5_object_id,
      const std::string& h5_method_name,
      const std::vector<std::shared_ptr<NWebValue>>& args) const = 0;
  virtual void CallH5FunctionV2(
      int32_t routing_id,
      int32_t h5_object_id,
      const std::string& h5_method_name,
      const std::vector<std::shared_ptr<NWebRomValue>>& args) const = 0;
  virtual void RegisterNWebJavaScriptCallBack(
      std::shared_ptr<NWebJavaScriptResultCallBack> callback) = 0;
  virtual bool OnFocus(
      const FocusReason& focusReason = FocusReason::FOCUS_DEFAULT) const = 0;
  virtual void OnBlur() const = 0;
  virtual void RegisterFindListener(
      std::shared_ptr<NWebFindCallback> find_listener) = 0;
  virtual void FindAllAsync(const std::string& str) const = 0;
  virtual void ClearMatches() const = 0;
  virtual void FindNext(const bool forward) const = 0;
  virtual void SuggestionSelected(int index) const = 0;

  virtual void ExecuteJavaScriptExt(
      const int fd,
      const size_t scriptLength,
      std::shared_ptr<NWebMessageValueCallback> callback,
      bool extention) = 0;
#if BUILDFLAG(ARKWEB_PDF)
  virtual void EraseCreatePDFCallbackImpl(uint32_t id) = 0;
  virtual void ExecuteCreatePDFExt(
      std::shared_ptr<NWebPDFConfigArgs> pdfConfig,
      std::shared_ptr<NWebArrayBufferValueCallback> callback) = 0;
#endif  // BUILDFLAG(ARKWEB_PDF)

#if BUILDFLAG(ARKWEB_MSGPORT)
  virtual void EraseJavaScriptCallbackImpl(uint32_t id) = 0;
  virtual void ExecuteJavaScript(
      const std::string& code,
      std::shared_ptr<NWebMessageValueCallback> callback,
      bool extention) = 0;
  virtual std::vector<std::string> CreateWebMessagePorts() = 0;
  virtual void PostWebMessage(const std::string& message,
                              const std::vector<std::string>& ports,
                              const std::string& targetUri) = 0;
  virtual void ClosePort(const std::string& portHandle) = 0;
  virtual void PostPortMessage(const std::string& portHandle,
                               std::shared_ptr<NWebMessage> data) = 0;
  virtual void PostPortMessageV2(const std::string& portHandle,
                                 std::shared_ptr<NWebRomValue> data) = 0;
  virtual void SetPortMessageCallback(
      const std::string& portHandle,
      std::shared_ptr<NWebMessageValueCallback> callback) = 0;
#endif  // BUILDFLAG(ARKWEB_MSGPORT)

#if BUILDFLAG(ARKWEB_I18N)
  virtual void UpdateLocale(const std::string& language,
                            const std::string& region) = 0;
#endif  // #if BUILDFLAG(ARKWEB_I18N)

#if BUILDFLAG(ARKWEB_DRAG_DROP)
  virtual std::shared_ptr<NWebDragData> GetOrCreateDragData() = 0;
  virtual std::string GetAppTempDir() const = 0;
  virtual bool DarkModeEnabled() = 0;
#endif  // #if BUILDFLAG(ARKWEB_DRAG_DROP)
  virtual void SetNWebId(uint32_t nwebId) = 0;

  virtual void StoreWebArchive(
      const std::string& base_name,
      bool auto_name,
      std::shared_ptr<NWebStringValueCallback> callback) const = 0;

  virtual void SetBrowserUserAgentString(const std::string& user_agent) = 0;

  virtual void SendDragEvent(const DelegateDragEvent& dragEvent) const = 0;
  virtual bool TerminateRenderProcess() = 0;
  virtual std::string GetUrl() const = 0;
  virtual const std::string GetOriginalUrl() = 0;
  virtual bool GetFavicon(const void** data,
                          size_t& width,
                          size_t& height,
                          ImageColorType& colorType,
                          ImageAlphaType& alphaType) = 0;
  virtual void PutNetworkAvailable(bool available) = 0;

  virtual CefRefPtr<ArkWebClientExt> GetCefClient() const = 0;

  virtual void GetImages(std::shared_ptr<NWebBoolValueCallback> callback) = 0;
  virtual void RemoveCache(bool include_disk_files) = 0;
  virtual void StopFling() = 0;
  virtual void ReloadIgnoreCache() = 0;

#if BUILDFLAG(ARKWEB_NAVIGATION)
  virtual std::shared_ptr<NWebHistoryList> GetHistoryList() = 0;
  virtual std::vector<uint8_t> SerializeWebState() = 0;
  virtual bool RestoreWebState(const std::vector<uint8_t>& state) = 0;
#endif  // BUILDFLAG(ARKWEB_NAVIGATION)

#if BUILDFLAG(ARKWEB_MEDIA_MUTE_AUDIO)
  virtual void SetAudioMuted(bool muted) = 0;
#endif  // BUILDFLAG(ARKWEB_MEDIA_MUTE_AUDIO)

#if BUILDFLAG(ARKWEB_WEBRTC)
  virtual void StartCamera() = 0;
  virtual void StopCamera() = 0;
  virtual void CloseCamera() = 0;
  virtual void ResumeMicrophone() = 0;
  virtual void StopMicrophone() = 0;
  virtual void PauseMicrophone() = 0;
#endif  // BUILDFLAG(ARKWEB_WEBRTC)

#if BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)
  virtual void StopScreenCapture(int32_t nweb_id, const char* session_id) = 0;
  virtual void SetScreenCapturePickerShow() = 0;
  virtual void DisableSessionReuse() = 0;
  virtual void RegisterScreenCaptureDelegateListener(
      std::shared_ptr<NWebScreenCaptureDelegateCallback>
          screenCaptureDelegateListener) = 0;
#endif  // defined(ARKWEB_EX_SCREEN_CAPTURE)

#if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)
  virtual void PageUp(bool top) = 0;
  virtual void PageDown(bool bottom) = 0;
#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
  virtual void GetScrollOffset(float* offset_x, float* offset_y) = 0;
#endif
#endif  // #if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  virtual void ScrollTo(float x, float y) = 0;
  virtual void ScrollBy(float delta_x, float delta_y) = 0;
  virtual void ScrollByRefScreen(float delta_x,
                                 float delta_y,
                                 float vx,
                                 float vy) = 0;
  virtual void SlideScroll(float vx, float vy) = 0;
  virtual bool WebSendKeyEvent(int32_t keyCode,
                               int32_t keyAction,
                               const std::vector<int32_t>& pressedCodes) = 0;
  virtual bool SendKeyboardEvent(
      const std::shared_ptr<OHOS::NWeb::NWebKeyboardEvent>& keyboardEvent) = 0;
  virtual bool ScrollByWithResult(float delta_x, float delta_y) = 0;
  virtual void ScrollToWithAnime(float x, float y, int32_t duration) = 0;
  virtual void ScrollByWithAnime(float delta_x,
                                 float delta_y,
                                 int32_t duration) = 0;
  virtual void WebSendMouseEvent(
      const std::shared_ptr<OHOS::NWeb::NWebMouseEvent>& mouseEvent) = 0;
#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
  virtual void GetOverScrollOffset(float* offset_x, float* offset_y) = 0;
#endif
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_EXT_FORCE_ZOOM) || BUILDFLAG(ARKWEB_ZOOM)
  virtual void SetForceEnableZoom(bool forceEnableZoom) = 0;
#endif
#if BUILDFLAG(ARKWEB_EXT_FORCE_ZOOM)
  virtual bool GetForceEnableZoom() = 0;
#endif

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
  virtual void SetShouldFrameSubmissionBeforeDraw(bool should) = 0;
  virtual void SetDrawRect(int32_t x,
                           int32_t y,
                           int32_t width,
                           int32_t height) = 0;
  virtual void SetDrawMode(int32_t mode) = 0;
  virtual bool GetPendingSizeStatus() = 0;
  virtual void SetFitContentMode(int32_t mode) = 0;
  virtual std::string GetCurrentLanguage() = 0;
#endif  // BUILDFLAG(ARKWEB_COMPOSITE_RENDER)

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  virtual void SetNativeInnerWeb(bool isInnerWeb) = 0;
  virtual bool GetNativeEmbedMode() = 0;
  virtual bool IsEnableCustomVideoPlayer() = 0;
#endif
#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
  virtual void SetAudioResumeInterval(int32_t resumeInterval) = 0;
  virtual void SetAudioExclusive(bool audioExclusive) = 0;
  virtual void SetAudioSessionType(int32_t audioSessionType) = 0;
  virtual void CloseAllMediaPresentations() = 0;
  virtual void StopAllMedia() = 0;
  virtual void ResumeAllMedia() = 0;
  virtual void PauseAllMedia() = 0;
  virtual int GetMediaPlaybackState() = 0;
#endif  // BUILDFLAG(ARKWEB_MEDIA_POLICY)

#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
  virtual void PrefetchPage(const PrefetchOptions& prefetch_options) = 0;
#endif  // BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)

#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
  virtual void NotifyPopupWindowResult(bool result) = 0;
#endif  // BUILDFLAG(ARKWEB_MULTI_WINDOW)
#if BUILDFLAG(ARKWEB_CA)
  virtual bool GetCertChainDerData(std::vector<std::string>& certChainData,
                                   bool isSingleCert) = 0;
#endif

  virtual void SetWindowId(uint32_t window_id) = 0;

#if BUILDFLAG(ARKWEB_PRINT)
  virtual void SetToken(void* token) = 0;
  virtual void* CreateWebPrintDocumentAdapter(const std::string& jobName) = 0;
  virtual void* CreateWebPrintDocumentAdapterV2(const std::string& jobName) = 0;
  virtual void SetPrintBackground(bool enable) = 0;
  virtual bool GetPrintBackground() = 0;
#endif  // BUILDFLAG(ARKWEB_PRINT)

#if BUILDFLAG(ARKWEB_SCREEN_ROTATION)
  virtual void SetVirtualPixelRatio(float ratio) = 0;
#endif  // BUILDFLAG(ARKWEB_SCREEN_ROTATION)

#if BUILDFLAG(IS_OHOS)
  virtual void UpdateNativeEmbedInfo(
      std::shared_ptr<NWebNativeEmbedDataInfo> info) = 0;
#endif

#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
  virtual void UpdateBrowserControlsState(int constraints,
                                          int current,
                                          bool animate) const = 0;
  virtual void UpdateBrowserControlsHeight(int height, bool animate) = 0;
#endif

#if BUILDFLAG(ARKWEB_POST_URL)
  virtual int PostUrl(const std::string& url,
                      const std::vector<char>& postData) = 0;
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  virtual void SetVirtualKeyBoardArg(int32_t width,
                                     int32_t height,
                                     double keyboard) = 0;
  virtual bool ShouldVirtualKeyboardOverlay() = 0;
  virtual void WebSendMouseWheelEvent(
      double x,
      double y,
      double deltaX,
      double deltaY,
      const std::vector<int32_t>& pressedCodes) = 0;
  virtual void WebSendMouseWheelEventV2(
      double x,
      double y,
      double deltaX,
      double deltaY,
      const std::vector<int32_t>& pressedCodes,
      int32_t source) = 0;
  virtual void WebSendTouchpadFlingEvent(
      double x,
      double y,
      double vx,
      double vy,
      const std::vector<int32_t>& pressedCodes) = 0;
  virtual void WebSendCancelFlingEvent() = 0;
#endif

#if BUILDFLAG(ARKWEB_SAFEBROWSING)
  virtual bool IsSafeBrowsingEnabled() = 0;
  virtual void EnableSafeBrowsing(bool enable) = 0;
  virtual void EnableSafeBrowsingDetection(bool enable, bool strictMode) = 0;
#endif  // BUILDFLAG(ARKWEB_SAFEBROWSING)

#if BUILDFLAG(IS_OHOS)
  virtual void PrecompileJavaScript(
      const std::string& url,
      const std::string& script,
      std::shared_ptr<CacheOptions>& cacheOptions,
      std::shared_ptr<NWebMessageValueCallback> callback) = 0;
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  virtual int PrerenderPage(const std::string& url,
                            const std::string& additional_headers) = 0;
  virtual void CancelAllPrerendering() = 0;
#endif

#if BUILDFLAG(ARKWEB_SECURITY_STATE)
  virtual int GetSecurityLevel() = 0;
#endif
#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
  virtual int InsertBackForwardEntry(int index, const std::string& url) = 0;

  virtual int UpdateNavigationEntryUrl(int index, const std::string& url) = 0;

  virtual void ClearForwardList() = 0;
#endif
#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
  virtual void SetAccessibilityState(cef_state_t accessibilityState) = 0;
  virtual bool ExecuteAction(
      int64_t accessibilityId,
      uint32_t action,
      const std::map<std::string, std::string>& actionArguments) = 0;
  virtual bool GetAccessibilityNodeRectById(int64_t accessibilityId,
                                            int32_t* width,
                                            int32_t* height,
                                            int32_t* offsetX,
                                            int32_t* offsetY) = 0;

  virtual std::shared_ptr<NWebAccessibilityNodeInfo>
  GetFocusedAccessibilityNodeInfo(int64_t accessibilityId,
                                  bool isAccessibilityFocus) = 0;
  virtual std::shared_ptr<NWebAccessibilityNodeInfo>
  GetAccessibilityNodeInfoById(int64_t accessibilityId) = 0;
  virtual int64_t GetWebAccessibilityIdByHtmlElementId(const std::string& htmlElementId) = 0;
  virtual bool GetAccessibilityVisible(int64_t accessibilityId) = 0;
  virtual std::shared_ptr<NWebAccessibilityNodeInfo>
  GetAccessibilityNodeInfoByFocusMove(int64_t accessibilityId,
                                      int32_t direction) = 0;
  virtual void RefreshAccessibilityManagerClickEvent() = 0;
  virtual void SendAccessibilityHoverEvent(int x, int y, bool isHoverEnter) = 0;
#endif

#if BUILDFLAG(ARKWEB_DISCARD)
  virtual bool Discard() = 0;
  virtual bool Restore() = 0;
#endif

#if BUILDFLAG(ARKWEB_ITP)
  virtual void EnableIntelligentTrackingPrevention(bool enable) = 0;
  virtual bool IsIntelligentTrackingPreventionEnabled() const = 0;
#endif

#if BUILDFLAG(ARKWEB_SECURE_JAVASCRIPT_PROXY)
  virtual std::string GetLastJavascriptProxyCallingFrameUrl() = 0;
#endif

#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
  virtual NWebDownloadItemState GetDownloadItemState(long item_id) = 0;
#endif

  virtual int ScaleGestureChange(double scale,
                                 double centerX,
                                 double centerY) const = 0;

  virtual int ScaleGestureChangeV2(int type,
                                   double scale,
                                   double originScale,
                                   double centerX,
                                   double centerY) const = 0;

#if BUILDFLAG(ARKWEB_SCREEN_LOCK)
  virtual void SetWakeLockCallback(
      int32_t windowId,
      const std::shared_ptr<NWebScreenLockCallback>& callback) = 0;
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  virtual void RegisterOnCreateNativeMediaPlayerListener(
      std::shared_ptr<NWebCreateNativeMediaPlayerCallback> callback) = 0;
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  virtual void EnableVideoAssistant(bool enable) = 0;
  virtual void EnableVideoAssistantAVCast(bool enable) = 0;
  virtual void ExecuteVideoAssistantFunction(const std::string& cmd_id) = 0;
  virtual void CustomWebMediaPlayer(bool enable) = 0;
  virtual void WebMediaPlayerControllerPlay() = 0;
  virtual void WebMediaPlayerControllerPause() = 0;
  virtual void WebMediaPlayerControllerSeek(double time) = 0;
  virtual void WebMediaPlayerControllerSetMuted(bool muted) = 0;
  virtual void WebMediaPlayerControllerSetPlaybackRate(
      double playback_rate) = 0;
  virtual void WebMediaPlayerControllerExitFullscreen() = 0;
  virtual void WebMediaPlayerControllerSetVideoSurface(void* native_window) = 0;
  virtual void WebMediaPlayerControllerDownload() = 0;
  virtual void WebMediaPlayerControllerSetVolume(double volume) = 0;
  virtual double WebMediaPlayerControllerGetVolume() = 0;
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_MENU)
  virtual void SetIsRichText(bool is_rich_text) = 0;
#endif
  virtual std::string GetSelectInfo() = 0;

#if BUILDFLAG(ARKWEB_AI_WRITE)
  virtual int GetSelectStartIndex() = 0;
  virtual int GetSelectEndIndex() = 0;
  virtual std::string GetAllTextInfo() = 0;
#endif // ARKWEB_AI_WRITE
#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
  virtual void OnSafeInsetsChange(int left, int top, int right, int bottom) = 0;
#endif

#if BUILDFLAG(ARKWEB_AI)
  virtual void OnTextSelected() = 0;
  virtual void OnDestroyImageAnalyzerOverlay() = 0;
  virtual void OnFoldStatusChanged(FoldStatus foldstatus) = 0;
  virtual void RunDataDetectorJS() = 0;
  virtual void SetDataDetectorEnable(bool enable) = 0;
  virtual bool GetDataDetectorEnable() = 0;
  virtual std::string GetDataDetectorSelectText() = 0;
  virtual void OnDataDetectorSelectText() = 0;
  virtual void OnDataDetectorCopy(const std::vector<std::string>& recordMix) = 0;
#endif

#if BUILDFLAG(ARKWEB_URL_TRUST_LIST)
  virtual int SetUrlTrustListWithErrMsg(const std::string& urlTrustList,
      bool allowOpaqueOrigin, bool supportWildcard, std::string& detailErrMsg) = 0;
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  virtual void SetPathAllowingUniversalAccess(
      const std::vector<std::string>& path_list,
      const std::vector<std::string>& excluded_path_list) = 0;
#endif

#if BUILDFLAG(ARKWEB_EXT_FILE_ACCESS)
  virtual void DisallowSandboxFileAccessFromFileUrl(bool disallow) = 0;
#endif  // BUILDFLAG(ARKWEB_EXT_FILE_ACCESS)

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  virtual void WebExtensionTabRemoved(
      int tab_id,
      bool isWindowClosing,
      int windowId) = 0;
  virtual void WebExtensionTabUpdated(
      int tab_id,
      std::unique_ptr<NWebExtensionTabChangeInfo> changeInfo,
      std::unique_ptr<NWebExtensionTab> tab) = 0;
  virtual void WebExtensionTabAttached(
      int tab_id,
      std::unique_ptr<NWebExtensionTabAttachInfo> attachInfo) = 0;
  virtual void WebExtensionTabDetached(
      int tab_id,
      std::unique_ptr<NWebExtensionTabDetachInfo> detachInfo) = 0;
  virtual void WebExtensionTabMoved(
      int32_t tab_id,
      std::unique_ptr<NWebExtensionTabMoveInfo> moveInfo) = 0;
  virtual void WebExtensionTabReplaced(int32_t addedTabId,
                                       int32_t removedTabId) = 0;
  virtual void WebExtensionSetViewType(int32_t type) = 0;
#endif

#if BUILDFLAG(ARKWEB_EXT_PERMISSION)
  virtual void PermissionRequestGrant(int32_t resourse_id,
                                      int nweb_request_key) = 0;
  virtual void PermissionRequestDeny(int nweb_request_key) = 0;
  virtual std::string PermissionRequestGetOrigin(int nweb_request_key) = 0;
  virtual int32_t PermissionRequestGetResourceId(int nweb_request_key) = 0;
  virtual void PermissionRequestDelete(int nweb_request_key) = 0;
#endif  // ARKWEB_EXT_PERMISSION

#if BUILDFLAG(ARKWEB_BFCACHE)
  virtual void SetBackForwardCacheOptions(int32_t size, int32_t timeToLive) = 0;
  virtual void SetMediaResumeFromBFCachePage(bool resume) = 0;
#endif

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  virtual void SetWholePageDrawing() = 0;

  virtual bool WebPageSnapshot(const char* id,
                               PixelUnit type,
                               int width,
                               int height,
                               const WebSnapshotCallback callback) = 0;
#endif  // ARKWEB_SOFTWARE_COMPOSITOR
  virtual bool IsCustomKeyboard() const = 0;

  virtual std::shared_ptr<NWebCustomKeyboardHandlerImpl>
  GetCustomKeyboardHandler() const = 0;

#if BUILDFLAG(ARKWEB_MIXED_CONTENT)
  virtual void EnableMixedContentAutoUpgrades(bool enable) = 0;
  virtual bool IsMixedContentAutoUpgradesEnabled() = 0;
#endif

#if BUILDFLAG(IS_ARKWEB)
  virtual void EnableAppLinking(bool enable) = 0;
#endif // BUILDFLAG(IS_ARKWEB)

#if BUILDFLAG(ARKWEB_MEDIA_NETWORK_TRAFFIC_PROMPT)
  virtual void EnableMediaNetworkTrafficPrompt(bool enable) = 0;
#endif  // ARKWEB_MEDIA_NETWORK_TRAFFIC_PROMPT
  virtual void SetSurfaceDensity(const double& density) = 0;

  virtual void OpenDevtoolsWith(
      std::shared_ptr<NWebDelegateInterface> nweb_delegate,
      std::unique_ptr<OpenDevToolsParam> param) = 0;
  virtual void OpenDevtoolsWithByPb(
      std::shared_ptr<NWebDelegateInterface> nweb_delegate,
      std::unique_ptr<OpenDevToolsParam> param,
      OpenDevToolsExtOpt& ext_opt) = 0;
  virtual void CloseDevtools() = 0;

#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
  virtual void SetTransformHint(uint32_t rotation) = 0;
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
  virtual void SetPopupSurface(void* popupSurface) = 0;
#endif

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
  virtual void SetUsageScenario(int32_t usage_scenario) = 0;
  virtual int32_t GetUsageScenario() = 0;
#endif
#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
  virtual void MaximizeResize() = 0;
#endif  // ARKWEB_MAXIMIZE_RESIZE

#if BUILDFLAG(ARKWEB_DISATCH_BEFORE_UNLOAD)
  virtual bool NeedToFireBeforeUnloadOrUnloadEvents() = 0;
  virtual void DispatchBeforeUnload() = 0;
#endif  // ARKWEB_DISATCH_BEFORE_UNLOAD

#if BUILDFLAG(ARKWEB_EX_REFRESH_IFRAME)
  virtual bool WebExtensionContextMenuIsIframe() = 0;
  virtual void WebExtensionContextMenuReloadFocusedFrame() = 0;
#endif

#if BUILDFLAG(ARKWEB_SAVE_PAGE)
  virtual bool SavePage(int32_t type, const std::string& filePath) = 0;
#endif // ARKWEB_SAVE_PAGE

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  virtual void WebExtensionContextMenuGetFocusedFrameInfo(
      int32_t& frame_id,
      std::string& frame_url) = 0;
#endif

#if BUILDFLAG(ARKWEB_NWEB_EX)
  virtual void EnableViewAutoResize(
      const CefSize& min_size,
      const CefSize& max_size) = 0;
  virtual void DisableViewAutoResize() = 0;
#endif

#if BUILDFLAG(ARKWEB_JSPROXY)
  virtual void JavaScriptOnDocumentStartByOrder(
      const ScriptItems& ScriptItems,
      const ScriptRegexItems& scriptRegexItems,
      const ScriptItemsByOrder& ScriptItemsByOrder) = 0;
  virtual void JavaScriptOnDocumentEndByOrder(
      const ScriptItems& ScriptItems,
      const ScriptRegexItems& scriptRegexItems,
      const ScriptItemsByOrder& ScriptItemsByOrder) = 0;
  virtual void JavaScriptOnHeadReadyByOrder(
      const ScriptItems& ScriptItems,
      const ScriptRegexItems& scriptRegexItems,
      const ScriptItemsByOrder& ScriptItemsByOrder) = 0;
#endif
  virtual bool SetFocusByPosition(float x, float y) = 0;
  virtual std::pair<double, double> GetLastTouchMousePosition() = 0;

#if BUILDFLAG(ARKWEB_SAFEBROWSING)
  virtual void OnSafeBrowsingDetectionResult(int code,
                                             int policy,
                                             const std::string& mappingType,
                                             const std::string& url) = 0;
#endif

#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_CORNER)
  virtual void SetBorderRadiusFromWeb(double borderRadiusTopLeft,
                                      double borderRadiusTopRight,
                                      double borderRadiusBottomLeft,
                                      double borderRadiusBottomRight) = 0;
#endif  // ARKWEB_SCROLLBAR_AVOID_CORNER
#if BUILDFLAG(ARKWEB_PIP)
  virtual void SetPipNativeWindow(int delegate_id,
                                  int child_id,
                                  int frame_routing_id,
                                  void* window) = 0;
  virtual void SendPipEvent(int delegate_id,
                            int child_id,
                            int frame_routing_id,
                            int event) = 0;
#endif

#if BUILDFLAG(ARKWEB_VIEWPORT_AVOID)
  virtual void AvoidVisibleViewportBottom(int32_t avoidHeight) = 0;
  virtual int32_t GetVisibleViewportAvoidHeight() = 0;
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  virtual void SetBlanklessLoadingKey(uint32_t nweb_id, uint64_t blankless_key) = 0;
  virtual int64_t GetPreferenceHash() = 0;
  virtual int32_t GetWidth() = 0;
  virtual int32_t GetHeight() = 0;
#endif

#if BUILDFLAG(ARKWEB_MENU)
  virtual void UpdateSingleHandleVisible(bool isVisible) = 0;
  virtual void SetTouchHandleExistState(bool touchHandleExist) = 0;
  virtual void SetViewportScaleState(bool viewportScale) = 0;
#endif  // BUILDFLAG(ARKWEB_MENU)

#if BUILDFLAG(ARKWEB_NWEB_EX)
  virtual void RunJavaScriptInFrames(RunJavaScriptParam param,
                                     OnReceiveValueCallback callback) = 0;
  virtual void GetAllFrameInfos(OnReceiveFrameInfosCallback callback) = 0;
  virtual void GetLastJavaScriptProxyCallingFrameInfo(
      OnLastJavaScriptProxyCallingFrameInfoCallback callback) = 0;
#endif

#if BUILDFLAG(ARKWEB_READER_MODE)
  virtual void Distill(const std::string& guid, const DistillOptions& distill_options, DistillCallback callback) = 0;
  virtual void AbortDistill() = 0;
#endif // ARKWEB_READER_MODE

#if BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
  virtual void SetBlankScreenDetectionConfig(
      bool enable,
      const std::vector<double>& detectionTiming,
      const std::vector<int32_t>& detectionMethods,
      int32_t contentfulNodesCountThreshold) = 0;
#endif

#if BUILDFLAG(ARKWEB_ERROR_PAGE)
  virtual void SetErrorPageEnabled(bool enable) = 0;
  virtual bool GetErrorPageEnabled() = 0;
#endif

#if BUILDFLAG(ARKWEB_EXT_HTTPS_UPGRADES)
  virtual void EnableHttpsUpgrades(bool enable) = 0;
#endif

#if BUILDFLAG(ARKWEB_EXT_RECEIVE_RESPONSE)
  virtual std::map<std::string, std::string> ResourceRequestGetRequestHeader(int nweb_request_key) = 0;
  virtual std::string ResourceRequestGetRequestUrl(int nweb_request_key) = 0;
  virtual bool ResourceRequestIsRequestGesture(int nweb_request_key) = 0;
  virtual bool ResourceRequestIsMainFrame(int nweb_request_key) = 0;
  virtual bool ResourceRequestIsRedirect(int nweb_request_key) = 0;
  virtual std::string ResourceRequestGetRequestMethod(int nweb_request_key) = 0;
  virtual int32_t ResourceRequestGetPageTransition(int nweb_request_key) = 0;
  virtual int32_t ResourceRequestGetRequestType(int nweb_request_key) = 0;
  virtual void ResourceRequestDelete(int nweb_request_key) = 0;
 
  virtual std::string ResourceResponseGetMimeType(int nweb_response_key) = 0;
  virtual std::string ResourceResponseGetEncoding(int nweb_response_key) = 0;
  virtual int32_t ResourceResponseGetStatusCode(int nweb_response_key) = 0;
  virtual std::string ResourceResponseGetReasonPhrase(int nweb_response_key) = 0;
  virtual std::map<std::string, std::string> ResourceResponseGetResponseHeader(int nweb_response_key) = 0;
  virtual bool ResourceResponseGetIsFromNetwork(int nweb_response_key) = 0;
  virtual void ResourceResponseDelete(int nweb_response_key) = 0;
  virtual int32_t GetLastCommittedEntryPageTransition() = 0;
#endif
  virtual std::vector<WebExtensionContextMenusItem> GetContextMenuItem() = 0;
  virtual void OnContextMenuSelected(int command_id) = 0;
  virtual void OnContextMenuClosed() = 0;

#if BUILDFLAG(ARKWEB_BGTASK)
  virtual void OnBrowserForeground() = 0;
  virtual void OnBrowserBackground() = 0;
#endif

#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
virtual void SetFocusWebId(int32_t nweb_id) = 0;
#endif


#if BUILDFLAG(ARKWEB_USERAGENT)
  virtual void SetUserAgentMetadata(
      const std::string& user_agent,
      std::shared_ptr<NWebUserAgentMetadata> metadata) = 0;
  virtual std::shared_ptr<NWebUserAgentMetadata> GetUserAgentMetadata(
      const std::string& user_agent) = 0;
#endif
};
}  // namespace OHOS::NWeb

#endif  // nweb_delegate_INTERFACE_H
