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

#ifndef ARKWEB_BROWSER_BROWSER_HOST_EXT_H_
#define ARKWEB_BROWSER_BROWSER_HOST_EXT_H_
#pragma once

#include "arkweb/build/features/features.h"
#include "base/memory/raw_ptr.h"
#include "base/observer_list.h"
#include "base/synchronization/lock.h"
#include "cef/include/cef_browser.h"
#include "cef/include/cef_client.h"
#include "cef/include/cef_unresponsive_process_callback.h"
#include "cef/include/views/cef_browser_view.h"
#include "cef/libcef/browser/browser_contents_delegate.h"
#include "cef/libcef/browser/browser_info.h"
#include "cef/libcef/browser/browser_platform_delegate.h"
#include "cef/libcef/browser/devtools/devtools_protocol_manager.h"
#include "cef/libcef/browser/devtools/devtools_window_runner.h"
#include "cef/libcef/browser/file_dialog_manager.h"
#include "cef/libcef/browser/frame_host_impl.h"
#include "cef/libcef/browser/javascript_dialog_manager.h"
#include "cef/libcef/browser/media_stream_registrar.h"
#include "cef/libcef/browser/request_context_impl.h"
#include "cef/libcef/features/features.h"
#if BUILDFLAG(ARKWEB_NETWORK_BASE)
#include "cef/ohos_cef_ext/include/arkweb_browser_ext.h"
#endif
#include "libcef/browser/arkweb_frame_host_impl_ext.h"
#if BUILDFLAG(ARKWEB_CLIPBOARD)
#include "cef/include/cef_image.h"
#include "cef/ohos_cef_ext/libcef/browser/arkweb_frame_host_impl_ext.h"
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)

#include "cef/ohos_cef_ext/libcef/browser/net_service/net_helpers.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#if BUILDFLAG(IS_ARKWEB)
#include "components/download/public/common/download_item_impl.h"
#include "libcef/browser/download_item_impl.h"
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
#include "components/input/native_web_keyboard_event.h"
#endif

#if BUILDFLAG(ARKWEB_MSGPORT)
#include "third_party/blink/public/common/messaging/web_message_port.h"
#endif

#if BUILDFLAG(ARKWEB_JS_ON_DOCUMENT_END)
#include "components/js_injection/browser/js_communication_host.h"
#endif

#if BUILDFLAG(ARKWEB_EXT_GET_ZOOM_LEVEL)
#include "components/zoom/zoom_controller.h"
#include "components/zoom/zoom_observer.h"
#endif

#if BUILDFLAG(ARKWEB_MSGPORT)
class WebMessageReceiverImpl : public blink::WebMessagePort::MessageReceiver {
 public:
  WebMessageReceiverImpl() = default;
  ~WebMessageReceiverImpl();

  // WebMessagePort::MessageReceiver implementation:
  bool OnMessage(blink::WebMessagePort::Message message) override;

  void SetOnMessageCallback(CefRefPtr<CefWebMessageReceiver> callback);
  void ConvertBlinkMsgToCefValue(blink::WebMessagePort::Message& message,
                                 CefRefPtr<CefValue> data);

 private:
  CefRefPtr<CefWebMessageReceiver> callback_;
};
#endif  // BUILDFLAG(ARKWEB_MSGPORT)

// Base class for CefBrowserHost implementations. Includes functionality that is
// shared by the alloy and chrome runtimes. All methods are thread-safe unless
// otherwise indicated.
class ArkWebBrowserHostExtImpl : public ArkWebBrowserHostExt,
                                 virtual public CefBrowserHostBase
#if BUILDFLAG(ARKWEB_EXT_GET_ZOOM_LEVEL)
    ,
                                 public zoom::ZoomObserver
#endif
{
 public:
  CefRefPtr<ArkWebBrowserHostExt> GetHost() override { return this; }

  CefRefPtr<ArkWebBrowserExt> AsArkWebBrowser() override { return this; }

#if BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
  CefRefPtr<ArkWebBrowserHostExtImpl> AsArkWebBrowserHostExtImpl() override {
    return this;
  }
#endif

#if BUILDFLAG(ARKWEB_PRECOMPILE)
  oh_code_cache::NextOp WriteResponseCache(
      const std::string& url,
      const std::string& script,
      std::shared_ptr<oh_code_cache::CacheOptions> cacheOptions);

  void OnDidWriteResponseCache(
      const std::string& url,
      const std::string& script,
      std::shared_ptr<oh_code_cache::CacheOptions> cacheOptions,
      CefRefPtr<CefPrecompileCallback> callback,
      oh_code_cache::NextOp nextOp);

  void GenerateCodeCache(
      const std::string& url,
      const std::string& script,
      std::shared_ptr<oh_code_cache::CacheOptions> cacheOptions,
      CefRefPtr<CefPrecompileCallback> callback);

  void OnDidGenerateCodeCache(CefRefPtr<CefPrecompileCallback> callback,
                              int32_t result);
#endif

#if BUILDFLAG(ARKWEB_OPTIMIZE_PARSER_BUDGET)
  void SetOptimizeParserBudgetEnabled(bool enable) override;
#endif

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
  ArkWebBrowserHostExtImpl();
#endif

#if BUILDFLAG(ARKWEB_BACKGROUND_COLOR)
  void SetBackgroundColor(int color) override;
#endif

  void FindEx(const CefString& searchText,
              bool forward,
              bool matchCase,
              bool findNext
#if BUILDFLAG(ARKWEB_FIND_IN_PAGE)
              ,
              bool newSession
#endif
              ) override;
#if BUILDFLAG(ARKWEB_TOUCHPAD_FLING)
  void SendTouchpadFlingEvent(const CefMouseEvent& event,
                              double vx,
                              double vy) override;
#endif

#if BUILDFLAG(ARKWEB_PDF)
  void CreateToPDF(const CefPdfPrintSettings& settings,
                   CefRefPtr<CefPdfValueCallback> callback) override;
#endif  // BUILDFLAG(ARKWEB_PDF)

  void PostTaskToUIThread(CefRefPtr<CefTask> task) override;
  void UpdateBrowserSettings(const CefBrowserSettings& browser_settings);
  void SetWebPreferences(const CefBrowserSettings& browser_settings) override;
  void RegisterNativeJSProxy(const CefString& object_name,
                             const std::vector<CefString>& method_list,
                             const int32_t object_id,
                             bool is_async,
                             const CefString& permission) override;
#if BUILDFLAG(ARKWEB_ADBLOCK)
  void UpdateAdblockEasyListRules(long adBlockEasyListVersion) override;
  bool IsAdsBlockEnabled() override;

  bool IsAdsBlockEnabledForCurPage() override;

  void EnableAdsBlock(bool enable) override;

  void SetAdBlockEnabledForSite(bool is_adblock_enabled,
                                int main_frame_tree_node_id) override;
#endif
  void RegisterArkJSfunction(const CefString& object_name,
                             const std::vector<CefString>& method_list,
                             const std::vector<CefString>& async_method_list,
                             const int32_t object_id,
                             const CefString& permission) override;
  void UnregisterArkJSfunction(
      const CefString& object_name,
      const std::vector<CefString>& method_list) override;
  void CallH5Function(int32_t routing_id,
                      int32_t h5_object_id,
                      const CefString& h5_method_name,
                      const std::vector<CefRefPtr<CefValue>>& args) override;
#if BUILDFLAG(ARKWEB_NETWORK_BASE)
  bool CanStoreWebArchive() override;
#endif
  void ReloadOriginalUrl() override;
#if BUILDFLAG(ARKWEB_NETWORK_BASE)
  void StoreWebArchive(
      const CefString& base_name,
      bool auto_name,
      CefRefPtr<CefStoreWebArchiveResultCallback> callback) override;
#endif
  void GetImageFromCache(const CefString& url, int command_id) override;

  void GetImageFromCacheEx(const CefString& url, int command_id) override;

  void SetBrowserUserAgentString(const CefString& user_agent) override;
  void ExitFullScreen() override;
  void GetRootBrowserAccessibilityManager(
      void** manager) override { /*TODO: IS_OHOS */
  }
#if BUILDFLAG(ARKWEB_I18N)
  void UpdateLocale(const CefString& locale) override;
#endif  // if BUILDFLAG(ARKWEB_I18N)
#if BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
  CefString GetOriginalUrl() override;
  void PutNetworkAvailable(bool available) override;
#endif  // BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  int PrerenderPage(const CefString& url,
                    const CefString& additional_headers) override;
  void CancelAllPrerendering() override;
#endif
#if BUILDFLAG(ARKWEB_NETWORK_BASE)
  void RemoveCache(bool include_disk_files) override;
#endif
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void SetVirtualKeyBoardArg(int32_t width,
                             int32_t height,
                             double keyboard) override;
  bool ShouldVirtualKeyboardOverlay() override;
#endif  // #if BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_JSPROXY)
  void JavaScriptOnDocumentStart(
      const CefString& script,
      const std::vector<CefString>& script_rules,
      bool is_transfer_finished) override;
  void RemoveJavaScriptOnDocumentStart() override;
  void JavaScriptOnDocumentEnd(
      const CefString& script,
      const std::vector<CefString>& script_rules,
      bool is_transfer_finished) override;
  void JavaScriptOnHeadReady(
      const CefString& script,
      const std::vector<CefString>& script_rules,
      bool is_transfer_finished) override;
  void RemoveJavaScriptOnHeadReady() override;
  void RemoveJavaScriptOnDocumentEnd() override;
#endif
  void SetDrawRect(int x, int y, int width, int height) override;
  void SetDrawMode(int mode) override;
  void SetFitContentMode(int mode) override;
  bool GetPendingSizeStatus() override;

  void SetBrowserZoomLevel(double zoom_factor) override;
#if BUILDFLAG(ARKWEB_EXT_GET_ZOOM_LEVEL)
  void OnZoomChanged(
      const zoom::ZoomController::ZoomChangedEventData& data) override;
  void OnZoomControllerDestroyed(
      zoom::ZoomController* zoom_controller) override {}
#endif

  void UpdateBrowserControlsState(int constraints,
                                  int current,
                                  bool animate) override;
  void UpdateBrowserControlsHeight(int height, bool animate) override;
  void StartCamera() override;
  void StopCamera() override;
  void CloseCamera() override;
  void SetNWebId(int NWebID) override;
  void PrecompileJavaScript(const std::string& url,
                            const std::string& script,
                            CefRefPtr<CefCacheOptions> cacheOptions,
                            CefRefPtr<CefPrecompileCallback> callback) override;

#if BUILDFLAG(IS_ARKWEB)
  void EnableAppLinking(bool enable) override;
  bool IsAppLinkingEnabled() const override;
#endif // BUILDFLAG(IS_ARKWEB)

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void AdvanceFocusForIME(int focusType) override;
#endif

#if BUILDFLAG(ARKWEB_NAVIGATION)
  CefRefPtr<CefBinaryValue> GetWebState() override;
  bool RestoreWebState(const CefRefPtr<CefBinaryValue> state) override;
#endif  // BUILDFLAG(ARKWEB_NAVIGATION)
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
  void SetNeedsReload(bool needs_reload) override;
  bool NeedsReload() override;
  bool needs_reload_ = false;
  bool is_hidden_ = false;
#endif

#if BUILDFLAG(IS_OHOS)
  int GetSecurityLevel() override;
  void DeleteHistory() override;
  bool CanGoBackOrForward(int num_steps) override;
  CefRefPtr<CefBrowserPermissionRequestDelegate> GetPermissionRequestDelegate()
      override;
  CefRefPtr<CefGeolocationAcess> GetGeolocationPermissions() override;

  CefString Title() override;
  void SetVirtualPixelRatio(float ratio) override;
  float GetVirtualPixelRatio() override;
  int PageLoadProgress() override;
  float Scale() override { /*TODO: IS_OHOS*/
    return 0;
  }
  void LoadWithDataAndBaseUrl(const CefString& baseUrl,
                              const CefString& data,
                              const CefString& mimeType,
                              const CefString& encoding,
                              const CefString& historyUrl) override;

  void LoadWithData(const CefString& data,
                    const CefString& mimeType,
                    const CefString& encoding) override;

#if BUILDFLAG(ARKWEB_EXT_HTTPS_UPGRADES)
  void LoadUrlWithParams(const std::string& url,
                         const LoadUrlType load_type,
                         const std::string& refer,
                         const std::string& headers,
                         const std::string& post_data,
                         const bool allow_https_upgrade,
                         int32_t transition_type) override;
#endif

  void ExecuteJSCallback(CefRefPtr<CefJavaScriptResultCallback> callback,
                         base::Value result);

  void ExecuteExtensionJSCallback(
      CefRefPtr<CefJavaScriptResultCallback> callback,
      base::Value result);

  void ExecuteJavaScript(const std::string& code,
                         CefRefPtr<CefJavaScriptResultCallback> callback,
                         bool extention) override;

  void ExecuteJavaScriptExt(const int fd,
                            const uint64_t scriptLength,
                            CefRefPtr<CefJavaScriptResultCallback> callback,
                            bool extention) override;

  void ResumeDownload(const CefString& url,
                      const CefString& full_path,
                      int64_t received_bytes,
                      int64_t total_bytes,
                      const CefString& etag,
                      const CefString& mime_type,
                      const CefString& last_modified,
                      const CefString& received_slices_string) override;

  // ResumeDownloadWithId is callback param in DownloadManager::GetNextId to
  // resume an interrupted download.
  void ResumeDownloadWithId(
      const GURL& url,
      const base::FilePath& full_path,
      int64_t received_bytes,
      int64_t total_bytes,
      const std::string& etag,
      const std::string& mime_type,
      const std::string& last_modified,
      std::vector<download::DownloadItem::ReceivedSlice> received_slices,
      uint32_t next_id);

#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
  CefRefPtr<CefDownloadItem> GetDownloadItem(uint32_t item_id) override;
#endif
  void WasOccluded(bool occluded) override;
  void OnWindowShow() override;
  void OnWindowHide() override;
  void OnOnlineRenderToForeground() override;
  void WasKeyboardResized() override;
  void SetWindowId(int window_id, int nweb_id) override;
  void SetWakeLockHandler(int32_t windowId,
                          CefRefPtr<CefSetLockCallback> callback) override;

#if BUILDFLAG(ARKWEB_PRINT)
  void SetToken(void* token) override;
  void CreateWebPrintDocumentAdapter(const CefString& jobName,
                                     void** webPrintDocumentAdapter) override;
  void CreateWebPrintDocumentAdapterV2(const CefString& jobName,
                                       void** adapter) override;
  void SetPrintBackground(bool enable) override;
  bool GetPrintBackground() override;
#endif  // BUILDFLAG(ARKWEB_PRINT)
  void SetEnableLowerFrameRate(bool enabled) override;
  void SetEnableHalfFrameRate(bool enabled) override;
#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
  void SetAudioResumeInterval(int resumeInterval) override { /*TODO: IS_OHOS*/
  }
  void SetAudioExclusive(bool audioExclusive) override { /*TODO: IS_OHOS*/
  }
  void SetAudioSessionType(int audioSessionType) override {
  }
#endif
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void SendTouchEventList(
      const std::vector<CefTouchEvent>& event_list) override;
#endif                                      // BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void NotifyForNextTouchEvent() override { /*TODO: IS_OHOS*/
  }
#if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)
  void ScrollPageUpDown(bool is_up, bool is_half, float view_height) override;
#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
  void GetScrollOffset(float* offset_x, float* offset_y) override;
#endif
#endif  // BUILDFLAG(ARKWEB_PAGE_UP_DOWN)
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void ScrollTo(float x, float y) override;
  void ScrollBy(float delta_x, float delta_y) override;
  void SlideScroll(float vx, float vy) override;
  void ZoomBy(float delta, float width, float height) override;
  void GetHitData(int& type, CefString& extra_data) override;
  void GetLastHitData(int& type, CefString& extra_data) override;
  uint64_t GetCurrentTimestamp();
  void SetOverscrollMode(int overScrollMode) override;
  void SetScrollable(bool enable, int scrollType) override;
#if BUILDFLAG(ARKWEB_SYNC_RENDER)
  void UpdateDrawRect() override;
#endif
  void ScrollToWithAnime(float x, float y, int32_t duration) override;
  void ScrollByWithAnime(float delta_x,
                         float delta_y,
                         int32_t duration) override;
  // void ScrollToWithAnime(float x, float y, int32_t duration) override
  // {/*TODO: IS_OHOS*/} void ScrollByWithAnime(float delta_x, float delta_y,
  // int32_t duration) override {/*TODO: IS_OHOS*/}
#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
  void GetOverScrollOffset(float* offset_x, float* offset_y) override;
#endif
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
  void SetBypassVsyncCondition(int32_t condition) override;
#endif
#if BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
  void SetFileAccess(bool flag) override;
  void SetBlockNetwork(bool flag) override;
#if BUILDFLAG(ARKWEB_EXT_FILE_ACCESS)
  void SetDisallowSandboxFileAccessFromFileUrl(bool flag) override;
#endif
  void SetCacheMode(int flag) override;
 void SetGrantFileAccessDirs(const std::vector<CefString>& dir_list,
                             const std::vector<CefString>& excluded_dir_list) override;
#endif  // BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
  void SetShouldFrameSubmissionBeforeDraw(
      bool should) override { /*TODO: IS_OHOS*/
  }
#if BUILDFLAG(ARKWEB_DISCARD)
  bool Discard() override;
  bool Restore() override;
#endif
#if BUILDFLAG(ARKWEB_TERMINATE_RENDER)
  bool TerminateRenderProcess() override;
#endif
  void ShowFreeCopyMenu() override;
  bool ShouldShowFreeCopyMenu() override;
  std::string GetSelectedTextFromContextParam() override;
  int GetNWebId() override;
#if BUILDFLAG(ARKWEB_ITP)
  void EnableIntelligentTrackingPrevention(bool enable) override;
  bool IsIntelligentTrackingPreventionEnabled() override;
  GURL GetLastCommittedURL();
#endif  // BUILDFLAG(ARKWEB_ITP)
#endif  // BUILDFLAG(IS_OHOS)
#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
  void CloseMedia() override;
  void StopMedia() override;
  void ResumeMedia() override;
  void PauseMedia() override;
  int GetMediaPlaybackState() override;
#endif  // BUILDFLAG(ARKWEB_MEDIA_POLICY)
#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
  void PrefetchPage(const OHOS::NWeb::PrefetchOptions& prefetch_options) override;
#endif  // ARKWEB_NO_STATE_PREFETCH
#if BUILDFLAG(ARKWEB_MSGPORT)
  void CreateWebMessagePorts(std::vector<CefString>& ports) override;
  void PostWebMessage(CefString& message,
                      std::vector<CefString>& ports,
                      CefString& targetUri) override;
  void ClosePort(const CefString& port_handle) override;
  bool ConvertCefValueToBlinkMsg(CefRefPtr<CefValue>& original,
                                 blink::WebMessagePort::Message& message);
  void PostPortMessage(const CefString& port_handle,
                       CefRefPtr<CefValue> message) override;
  void SetPortMessageCallback(
      const CefString& port_handle,
      CefRefPtr<CefWebMessageReceiver> callback) override;
  void DestroyAllWebMessagePorts() override;
#endif  // BUILDFLAG(ARKWEB_MSGPORT)

#if BUILDFLAG(ARKWEB_AUTOFILL)
  void SetAutofillCallback(CefRefPtr<CefWebMessageReceiver> callback) override;
  void FillAutofillData(CefRefPtr<CefValue> message) override;
#endif

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  void ProcessAutofillCancel(const CefString& fillContent) override;
  void AutoFillWithIMFEvent(bool is_username,
                            bool is_other_account,
                            bool is_new_password,
                            const CefString& content) override;
#endif

  // #if BUILDFLAG(ARKWEB_NWEB_EX)
  // NOTE: Keep the previous line commented, add NWebEx APIs below.
  bool ShouldShowLoadingUI() override;
  void SetForceEnableZoom(bool forceEnableZoom) override;
  bool GetForceEnableZoom() override;

  void PasswordSuggestionSelected(int list_index) override;
  bool GetSavePasswordAutomatically() override { return false; }
  void SetSavePasswordAutomatically(bool enable) override {}
  void SaveOrUpdatePassword(bool is_update) override {}
  bool GetSavePassword() override { return true; }
  void SetSavePassword(bool enable) override {}

  int GetTopControlsOffset() override;
  int GetShrinkViewportHeight() override;

#if BUILDFLAG(ARKWEB_SAFEBROWSING)
  bool IsSafeBrowsingEnabled() override;
  void EnableSafeBrowsing(bool enable) override;
  void EnableSafeBrowsingDetection(bool enable, bool strictMode) override;
  bool IsSafeBrowsingDetectionDisabled() const;
  void HandleSafeBrowsingDetection(const std::string& url);
  int GetSafeBrowsingDetectionMode() const;
  int GetSafeBrowsingDetectionSwitch() const;
  void SetSafeBrowsingDetectionCallback(
      CefRefPtr<CefSafeBrowsingDetectionCallback> callback);
#endif  // BUILDFLAG(ARKWEB_SAFEBROWSING)

#if BUILDFLAG(IS_OHOS)

  // Geolocation API support
  void PopupGeolocationPrompt(
      std::string origin,
      cef_permission_callback_t callback) { /*TODO: IS_OHOS*/
  }
  void RemoveGeolocationPrompt(std::string origin) { /*TODO: IS_OHOS*/
  }
  void OnGeolocationShow(std::string origin) { /*TODO: IS_OHOS*/
  }

  bool IsBase64Encoded(std::string encoding);
  std::string GetDataURI(const std::string& data);

  void SetNativeWindow(cef_native_window_t window) override;
  uint32_t GetAcceleratedWidget(bool isPopup) override;

#if BUILDFLAG(ARKWEB_DEVTOOLS)
  void SetWebDebuggingAccess(bool isEnableDebug) override;
  bool GetWebDebuggingAccess() override;
#endif
#if BUILDFLAG(ARKWEB_SCREEN_ROTATION)
  void UpdatePixelRatio(float ratio);
#endif
#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
  void SetIsFling(bool is_fling);
#endif
#if BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
  bool GetFileAccess();
  bool GetBlockNetwork();
#if BUILDFLAG(ARKWEB_EXT_FILE_ACCESS)
  bool GetDisallowSandboxFileAccessFromFileUrl();
#endif
  int GetCacheMode();
  std::vector<std::string> GetGrantFileAccessDirs();
  net_service::FileAccessType IsInFileAccessList(const GURL& url);
  void GetSettingOfNetHelper(const GURL& url, struct net_service::NetHelperSetting& setting);
  bool file_access_ = false;
  bool network_blocked_ = false;
#if BUILDFLAG(ARKWEB_EXT_FILE_ACCESS)
  bool disallow_sandbox_file_access_from_file_url_ = false;
#endif
  int cache_mode_ = 0;
  std::vector<CefString> file_access_dirs_list_{};
  std::vector<CefString> file_excluded_dirs_list_{};
#endif  // BUILDFLAG(ARKWEB_NETWORK_CONNINFO)

#if BUILDFLAG(ARKWEB_SECURE_JAVASCRIPT_PROXY)
  CefString GetLastJavascriptProxyCallingFrameUrl() override;
#endif

#endif  // IS_OHOS

#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
  int InsertBackForwardEntry(int index, const CefString& url) override;
  int UpdateNavigationEntryUrl(int index, const CefString& url) override;
  void ClearForwardList() override;
#endif

#if BUILDFLAG(ARKWEB_AI)
  void OnTextSelected(bool flag) override;
  void OnDestroyImageAnalyzerOverlay() override;
  void OnFoldStatusChanged(uint32_t foldStatus) override;
  float GetPageScaleFactor() override;
  std::string GetDataDetectorSelectText() override;
  void OnDataDetectorSelectText() override;
#endif

#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
  void OnSafeInsetsChange(int left, int top, int right, int bottom) override;
#endif

#if BUILDFLAG(ARKWEB_URL_TRUST_LIST)
  int SetUrlTrustListWithErrMsg(const CefString& urlTrustList,
                                CefString& detailErrMsg) override;
#endif

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  bool WebPageSnapshot(const char* id,
                       int width,
                       int height,
                       cef_web_snapshot_callback_t callback) override;
#endif

#if BUILDFLAG(IS_ARKWEB)
  void UpdateZoomSupportEnabled();
#endif

#if BUILDFLAG(ARKWEB_BACKGROUND_COLOR)
  SkColor GetBackgroundColor() const;
#endif  // ARKWEB_BACKGROUND_COLOR

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void ScrollFocusedEditableNodeIntoView() override;
  void ScaleGestureChangeV2(int type,
                            float scale,
                            float originScale,
                            float centerX,
                            float centerY) override;
  void GoBackOrForward(int num_steps) override;
  void SetInitialScale(float scale) override;
  void SetFocusOnWeb() override;
  void SetImeShow(bool visible) override;
  bool IsNeedZoomChange(const input::NativeWebKeyboardEvent& event,
                        bool& zoom_in);
  void UpdateSecurityLayer(bool isNeedSecurityLayer) override;
  void SetHasComposition(bool has_composition) override;
  bool GetHasComposition() override;
  void UpdateTextFieldStatus(bool isShowKeyboard, bool isAttachIME) override;
#endif  // ARKWEB_INPUT_EVENTS

#if BUILDFLAG(ARKWEB_BFCACHE)
  void SetBackForwardCacheOptions(int32_t size, int32_t timeToLive) override;
#endif

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  void SetNativeEmbedMode(bool flag) override { /*TODO: ARKWEB_SAME_LAYER*/
  }
  void SetNativeInnerWeb(bool isInnerWeb) override {}
  void SetEnableCustomVideoPlayer(bool flag) override {}
#endif

  void OnWebPreferencesChanged();

#if BUILDFLAG(ARKWEB_CLIPBOARD)
  bool HandleContextMenu(
      content::RenderFrameHost& render_frame_host,
      const content::ContextMenuParams& params);  // override;
#endif                                            // BUILDFLAG(ARKWEB_CLIPBOARD)

#if BUILDFLAG(ARKWEB_PERMISSION) && BUILDFLAG(ARKWEB_CLIPBOARD)
  void AskClipboardReadWritePermission(
      const CefString& origin,
      cef_permission_callback_t callback) override;
  void AbortAskClipboardReadWritePermission(const CefString& origin) override;
#endif

  // #if BUILDFLAG(ARKWEB_INJECT_OFFLINE_RESOURCE)
  //   oh_code_cache::NextOp WriteResponseCache(
  //       const std::string& url,
  //       const std::string& script,
  //       std::shared_ptr<oh_code_cache::CacheOptions> cacheOptions);
  // #endif

  // TODO(ARKWEB_EXT_PERMISSION)
  //  #if BUILDFLAG(ARKWEB_EX_PERMISSION)
  //    void AskClipboardSanitizedWritePermission(const CefString& origin,
  //         cef_permission_callback_t callback) override;
  //    void AbortAskClipboardSanitizedWritePermission(
  //        const CefString& origin) override;
  //
  //    void AskAudioCapturePermission(const CefString& origin,
  //                                   cef_permission_callback_t callback)
  //                                   override;
  //    void AbortAskAudioCapturePermission(const CefString& origin) override;
  //    void AskVideoCapturePermission(const CefString& origin,
  //                                   cef_permission_callback_t callback)
  //                                   override;
  //    void AbortAskVideoCapturePermission(const CefString& origin) override;
  //  #endif

#if BUILDFLAG(ARKWEB_USERAGENT)
  void PutUserAgent(const CefString& ua, bool from_app) override;
  CefString GetCustomUserAgent() override;
  CefString DefaultUserAgent() override;
#endif

#if BUILDFLAG(ARKWEB_DRAG_DROP)
  gfx::Rect GetVisibleRectToWeb();
#endif

  void SetPopupWindow(cef_native_window_t popupWindow) override;

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
  void MaximizeResize() override;
#endif  // ARKWEB_MAXIMIZE_RESIZE

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  virtual void ExtensionSetTabId(int32_t tab_id) override {}
  virtual int32_t ExtensionGetTabId() override { return -1; }
  virtual void WebExtensionRegisterZoomObserver() override {}
  virtual void WebExtensionUnregisterZoomObserver() override {}
  virtual  void WebExtensionTabUpdated(
      int tab_id,
      std::unique_ptr<NWebExtensionTabChangeInfo> changeInfo,
      std::unique_ptr<NWebExtensionTab> tab) override {}
  virtual void WebExtensionTabRemoved(int tab_id,
    bool isWindowClosing, int windowId) override {}

  virtual void WebExtensionTabAttached(int tab_id, int new_position, int new_window_id) override {}

  virtual void WebExtensionTabDetached(int tab_id,
    const std::unique_ptr<NWebExtensionTabDetachInfo> detachInfo) override {}

  virtual void WebExtensionTabHighlighted(NWebExtensionTabHighlightInfo& highlightInfo) override {}

  virtual void WebExtensionTabMoved(int tab_id, const std::unique_ptr<NWebExtensionTabMoveInfo> moveInfo) override {}

  virtual void WebExtensionTabReplaced(int32_t addedTabId, int32_t removedTabId) override {}

  virtual void WebExtensionSetViewType(int32_t type) override {}
#endif

  void RunJavaScriptInFrames(const std::string& jsString, FrameInfos rootFrame,
                             bool recursive, IsolatedWorld world,
                             CefRefPtr<CefJavaScriptResultCallback> callback) override;

#if BUILDFLAG(ARKWEB_BGTASK)
  void OnBrowserForeground() override;
  void OnBrowserBackground() override;
#endif

#if BUILDFLAG(ARKWEB_READER_MODE)
  void Distill(const std::string& guid, const DistillOptions& distill_options,
    CefRefPtr<CefDistillCallback> callback) override;
  void AbortDistill() override;
#endif // ARKWEB_READER_MODE
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  void GetFocusedFrameInfo(int32_t& frame_id, CefString& frame_url) override;
#endif

#if BUILDFLAG(ARKWEB_EXT_HTTPS_UPGRADES)
  void EnableHttpsUpgrades(bool enable) override;
#endif

  void HandleInputMethodExtendAction(int32_t action) override;
  void StopFling() override;

 private:
#if BUILDFLAG(ARKWEB_MSGPORT)
  using MessagePipe = std::pair<blink::WebMessagePort, blink::WebMessagePort>;
  using PortHandle = std::pair<uint64_t, uint64_t>;
  base::Lock web_message_lock_;
  std::map<PortHandle, MessagePipe> portMap_ GUARDED_BY(web_message_lock_);
  std::set<std::string> postedPorts_ GUARDED_BY(web_message_lock_);
  std::unordered_map<std::string, std::shared_ptr<WebMessageReceiverImpl>>
      receiverMap_ GUARDED_BY(web_message_lock_);
  scoped_refptr<base::SequencedTaskRunner> sequenced_task_runner_ =
      base::ThreadPool::CreateSequencedTaskRunner({});
  void ClosePortInternal(const CefString& portHandle);
  void SetPortMessageCallbackInternal(
      const CefString& portHandle,
      CefRefPtr<CefWebMessageReceiver> callback);
  void PostPortMessageInternal(const CefString& portHandle,
                               CefRefPtr<CefValue> data);
#endif
#if BUILDFLAG(ARKWEB_NETWORK_BASE)
  void StoreWebArchiveInternal(
      CefRefPtr<CefStoreWebArchiveResultCallback> callback,
      const CefString& path);
#endif
#if BUILDFLAG(ARKWEB_JS_ON_DOCUMENT_END)
  js_injection::JsCommunicationHost* GetJsCommunicationHost();
#endif
#if BUILDFLAG(IS_ARKWEB)
  bool is_arkweb_applinking_enabled_ = true;
#endif // BUILDFLAG(IS_ARKWEB)
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  float virtual_pixel_ratio_ = 2.0;
  uint64_t last_zoom_time_ = 0;
  bool scrollable_ = true;
  int scrollType_ = -1;
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
#if BUILDFLAG(ARKWEB_BACKGROUND_COLOR)
  void UpdateBackgroundColor(int color);
#endif  // ARKWEB_BACKGROUND_COLOR
#if BUILDFLAG(ARKWEB_USERAGENT)
  std::string custom_user_agent_;
#endif
  cef_accelerated_widget_t widget_;
  cef_accelerated_widget_t popup_widget_;
#if BUILDFLAG(ARKWEB_BACKGROUND_COLOR)
  int base_background_color_ = 0xffffffff;
#endif  // ARKWEB_BACKGROUND_COLOR
#if BUILDFLAG(ARKWEB_CLIPBOARD)
  void GetImageForContextNode(CefRefPtr<CefFrame> frame, int command_id) override;
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void SendMouseClickEvent(const CefMouseEvent& event,
                           MouseButtonType type,
                           bool mouseUp,
                           int clickCount) override;
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
#if BUILDFLAG(ARKWEB_JSPROXY)
  std::unique_ptr<js_injection::JsCommunicationHost> js_communication_host_;
#endif // BUILDFLAG(ARKWEB_JSPROXY)
  mutable base::Lock lock_;
#if BUILDFLAG(ARKWEB_ITP)
  bool intelligent_tracking_prevention_cookies_enabled_ GUARDED_BY(lock_) =
      false;
#endif
#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
  void PutWebMediaAVSessionEnabled(bool enable) override;
#endif // ARKWEB_MEDIA_AVSESSION
#if BUILDFLAG(ARKWEB_DEVTOOLS)
  bool is_web_debugging_access_ = false;
#endif
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  bool SetFocusByPosition(float x, float y) override;
  bool has_composition_ = false;
#endif // BUILDFLAG(ARKWEB_INPUT_EVENTS)
#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  base::circular_deque<std::unique_ptr<content::PrerenderHandle>>
      prerender_handles_;
#endif
#if BUILDFLAG(ARKWEB_NETWORK_BASE)
  base::WeakPtrFactory<ArkWebBrowserHostExtImpl> weak_ptr_factory_;
#endif
};

#endif  // ARKWEB_BROWSER_BROWSER_HOST_EXT_H_
