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

#ifndef ARKWEB_INCLUDE_CEF_BROWSER_H_
#define ARKWEB_INCLUDE_CEF_BROWSER_H_
#pragma once

#include <vector>

#include "ohos_nweb/src/capi/nweb_extension_javascript_item.h"
#include "arkweb/ohos_nweb/src/capi/nweb_extension_load_url_params.h"
#include "arkweb/ohos_nweb/src/capi/nweb_prefetch_options.h"
#include "include/cef_base.h"
#include "include/cef_browser.h"
#include "include/cef_devtools_message_observer.h"
#include "include/cef_download_item.h"
#include "include/cef_drag_data.h"
#include "include/cef_frame.h"
#include "include/cef_image.h"
#include "include/cef_navigation_entry.h"
#include "include/cef_permission_request.h"
#include "include/cef_registration.h"
#include "include/cef_request_context.h"
#include "include/cef_task.h"
#include "include/internal/cef_string_map.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb/build/features/features.h"
#endif

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#include "ohos_nweb/src/capi/web_extension_tab_items.h"
#endif // #if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)

#if BUILDFLAG(ARKWEB_READER_MODE)
#include "arkweb/ohos_nweb/src/capi/nweb_extension_distill_item.h"
#endif // ARKWEB_READER_MODE

class CefClient;

///
/// Interface to implement to be notified of asynchronous completion via
/// CefBrowserHostBase::ExecuteJavaScript().
///
class CefJavaScriptResultCallback : public virtual CefBaseRefCounted {
 public:
  ///
  /// Method that will be called upon completion. |num_deleted| will be the
  /// number of cookies that were deleted.
  ///
  virtual void OnJavaScriptExeResult(CefRefPtr<CefValue> result) = 0;

  virtual void SetErrorDescription(const std::string& description) {}
};

#if BUILDFLAG(ARKWEB_READER_MODE)
class CefDistillCallback : public virtual CefBaseRefCounted {
  public:
  virtual void OnDistillCallback(const std::string& guid, const std::string& distill_info) = 0;
};
#endif // ARKWEB_READER_MODE

/* ---------- ohos webview add begin --------- */
///
/// Interface to implement to be notified of asynchronous completion via
/// CefBrowserHostBase::StoreWebArchive().
///
class CefStoreWebArchiveResultCallback : public virtual CefBaseRefCounted {
 public:
  ///
  /// Method that will be called upon completion. |result| will either be the
  /// filename under which the file was saved, or empty if saving the file
  /// failed.
  ///
  virtual void OnStoreWebArchiveDone(const CefString& result) = 0;
};

///
/// Interface to implement to be notified of asynchronous completion via
/// CefBrowserHostBase::SetGestureEventResult().
///
class CefGestureEventCallback : public virtual CefBaseRefCounted {
 public:
  ///
  /// Method that will be called upon completion.
  ///
  virtual void ContinueTask(bool result, bool stopPropagation) = 0;
};

///
/// Interface to implement to be notified of asynchronous completion via
/// CefBrowserHostBase::SetMouseEventResult().
///
class CefMouseEventCallback : public virtual CefBaseRefCounted {
 public:
  ///
  /// Method that will be called upon completion.
  ///
  virtual void ContinueTask(bool result, bool stopPropagation) = 0;
};

///
/// Interface to implement to be notified of asynchronous web message channel.
///
class CefWebMessageReceiver : public virtual CefBaseRefCounted {
 public:
  ///
  /// Method that will be called upon |PostPortMessage|. |message| will be sent
  /// to another end of web message channel.
  ///
  virtual void OnMessage(CefRefPtr<CefValue> message) = 0;

  ///
  /// The same as OnMessage, the result of the execution will be returned.
  ///
  virtual bool OnMessageWithBoolResult(CefRefPtr<CefValue> message) = 0;
};

///
/// CefSetLockCallback.
///
class CefSetLockCallback : public virtual CefBaseRefCounted {
 public:
  ///
  /// Handle.
  ///
  virtual void Handle(bool key) = 0;
};

///
/// Interface to implement to be notified of compiling javascript completion via
/// CefBrowserHostBase::PrecompileJavaScript().
///
class CefPrecompileCallback : public virtual CefBaseRefCounted {
 public:
  ///
  /// Method that will be called upon completion.
  ///
  virtual void OnPrecompileFinished(int32_t result) = 0;
};

///
/// options and info of CefBrowserHostBase::PrecompileJavaScript().
///
class CefCacheOptions : public virtual CefBaseRefCounted {
 public:
  ///
  /// Return the response headers of javascript request.
  ///
  virtual cef_string_map_t GetResponseHeaders() = 0;
};

class ArkWebBrowserExt : public virtual CefBrowser {
 public:
  ///
  /// Returns the Permission Request Delegate object.
  /// IS_OHOS extended
  ///
  virtual CefRefPtr<CefBrowserPermissionRequestDelegate>
  GetPermissionRequestDelegate() = 0;

  ///
  /// Returns the Geolocation Permission handler object.
  /// IS_OHOS extended
  ///
  virtual CefRefPtr<CefGeolocationAcess> GetGeolocationPermissions() = 0;

  ///
  /// Returns true if the browser can navigate forwards.
  ///
  virtual bool CanGoBackOrForward(int num_steps) = 0;

  ///
  /// Navigate backwards or forwards.
  ///
  virtual void GoBackOrForward(int num_steps) = 0;

  ///
  /// DeleteHistory
  ///
  virtual void DeleteHistory() = 0;

  ///
  /// display the selection control when click Free copy interface
  ///
  virtual void ShowFreeCopyMenu() = 0;

  ///
  /// should show free copy menu
  ///
  virtual bool ShouldShowFreeCopyMenu() = 0;

  ///
  /// select password dialog to fill
  ///
  virtual void PasswordSuggestionSelected(int list_index) = 0;

  ///
  /// Update browser controls state.
  ///
  virtual void UpdateBrowserControlsState(int constraints,
                                          int current,
                                          bool animate) = 0;

  ///
  /// Update browser controls height.
  ///
  virtual void UpdateBrowserControlsHeight(int height, bool animate) = 0;
  ///
  /// Prefetch the resources required by the page, but will not execute js or
  /// render the page.
  ///
  virtual void PrefetchPage(const OHOS::NWeb::PrefetchOptions& prefetch_options) = 0;
  /* ---------- ohos_nweb_ex add begin --------- */
  ///
  /// Reload the current page with original url.
  ///
  virtual void ReloadOriginalUrl() = 0;

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
  ///
  /// Can save current page as an archive.
  ///
  virtual bool CanStoreWebArchive() = 0;
#endif

  ///
  /// Set user agent for current page.
  ///
  virtual void SetBrowserUserAgentString(const CefString& user_agent) = 0;

  ///
  /// Is loading to different document.
  ///
  virtual bool ShouldShowLoadingUI() = 0;

  ///
  /// Set force enable zoom.
  ///
  virtual void SetForceEnableZoom(bool forceEnableZoom) = 0;

  ///
  /// Whether force enable zoom had been enabled.
  ///
  virtual bool GetForceEnableZoom() = 0;

  ///
  /// Returns the NWeb Id.
  ///
  virtual int GetNWebId() = 0;

  ///
  /// Get whether Ads block is enabled.
  ///
  virtual bool IsAdsBlockEnabled() = 0;

  ///
  /// Get whether Ads block is enabled for current page.
  ///
  virtual bool IsAdsBlockEnabledForCurPage() = 0;

  ///
  /// Set enable to allow automatically save password
  ///
  virtual void EnableAdsBlock(bool enable) = 0;

  ///
  /// Whether automatically saving password had been enabled.
  ///
  virtual bool GetSavePasswordAutomatically() = 0;

  ///
  /// Set enable to allow automatically save password
  ///
  virtual void SetSavePasswordAutomatically(bool enable) = 0;

  ///
  /// save or upddate current page password
  ///
  virtual void SaveOrUpdatePassword(bool is_update) = 0;

  ///
  /// Whether saving password had been enabled.
  ///
  virtual bool GetSavePassword() = 0;

  ///
  /// Set enable to save password
  ///
  virtual void SetSavePassword(bool enable) = 0;

  ///
  /// Enable the ability to check website security risks.
  ///
  virtual void EnableSafeBrowsing(bool enable) = 0;

  ///
  /// Get whether checking website security risks is enabled.
  ///
  virtual bool IsSafeBrowsingEnabled() = 0;

  ///
  /// Enable safe browsing detection.
  ///
  virtual void EnableSafeBrowsingDetection(bool enable, bool strictMode) = 0;

  ///
  /// Get security level for current page.
  ///
  virtual int GetSecurityLevel() = 0;

#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
  ///
  /// Get the shrink viewport height.
  ///
  virtual int InsertBackForwardEntry(int index, const CefString& url) = 0;

  ///
  /// Get the shrink viewport height.
  ///
  virtual int UpdateNavigationEntryUrl(int index, const CefString& url) = 0;

  ///
  /// Get the shrink viewport height.
  ///
  virtual void ClearForwardList() = 0;
#endif

  ///
  /// Enable the ability to intelligent tracking prevention, default disabled.
  ///
  virtual void EnableIntelligentTrackingPrevention(bool enable) = 0;

  ///
  /// Get whether intelligent tracking prevention is enabled.
  ///
  virtual bool IsIntelligentTrackingPreventionEnabled() = 0;

  ///
  /// Set url trust list.
  ///
  virtual int SetUrlTrustListWithErrMsg(const CefString& urlTrustList,
                                        CefString& detailErrMsg) = 0;

  ///
  /// Set tabId.
  ///
  virtual void ExtensionSetTabId(int tab_id) = 0;

  ///
  /// Get tabId.
  ///
  virtual int ExtensionGetTabId() = 0;

  ///
  /// Set back forward cache options.
  ///
  virtual void SetBackForwardCacheOptions(int32_t size, int32_t timeToLive) = 0;

  ///
  /// Get widget.
  ///
  /*--cef()--*/
  virtual uint32_t GetAcceleratedWidget(bool isPopup) = 0;

  ///
  /// Set adblock switch
  ///
  /*--cef()--*/
  virtual void SetAdBlockEnabledForSite(bool is_adblock_enabled,
                                        int main_frame_tree_node_id) = 0;

  ///
  /// Stop web fling.
  ///
  virtual void StopFling() {};
};

///
/// Class used to represent the browser process aspects of a browser. The
/// methods of this class can only be called in the browser process. They may be
/// called on any thread in that process unless otherwise indicated in the
/// comments.
///
class ArkWebBrowserHostExt : public virtual CefBrowserHost,
                             public virtual ArkWebBrowserExt {
 public:
  ///
  /// Search for |searchText|. |forward| indicates whether to search forward or
  /// backward within the page. |matchCase| indicates whether the search should
  /// be case-sensitive. |findNext| indicates whether this is the first request
  /// or a follow-up. The search will be restarted if |searchText| or
  /// |matchCase| change. The search will be stopped if |searchText| is empty.
  /// The CefFindHandler instance, if any, returned via
  /// CefClient::GetFindHandler will be called to report find results.
  ///
  virtual void FindEx(const CefString& searchText,
                      bool forward,
                      bool matchCase,
                      bool findNext,
                      bool newSession) = 0;

  ///
  /// Notify the browser that it has been occluded or unoccluded. Layouting and
  /// CefRenderHandler::OnPaint notification will stop when the browser is
  /// occluded. This method is only used when window rendering is disabled.
  /// IS_OHOS extended
  ///
  virtual void WasOccluded(bool occluded) = 0;

  ///
  /// Running and do something when the window show
  /// IS_OHOS extended
  ///
  virtual void OnWindowShow() = 0;

  ///
  /// Running and do something when the window hide
  /// IS_OHOS extended
  ///
  virtual void OnWindowHide() = 0;

  ///
  /// Running and do something when the render visible
  /// IS_OHOS extended
  ///
  virtual void OnOnlineRenderToForeground() = 0;

  ///
  /// Send touch event list to the browser for a windowless browser.
  /// IS_OHOS extended
  ///
  virtual void SendTouchEventList(
      const std::vector<CefTouchEvent>& event_list) = 0;

  ///
  /// GetRootBrowserAccessibilityManager
  ///
  virtual void GetRootBrowserAccessibilityManager(void** manager) = 0;

  ///
  /// Execute a string of JavaScript code, return result by callback
  ///
  virtual void ExecuteJavaScript(
      const std::string& code,
      CefRefPtr<CefJavaScriptResultCallback> callback,
      bool extention) = 0;
  ///
  /// Execute a string of JavaScript code, return result by callback
  ///
  virtual void ExecuteJavaScriptExt(
      const int fd,
      const uint64_t scriptLength,
      CefRefPtr<CefJavaScriptResultCallback> callback,
      bool extention) = 0;

  ///
  /// Set native window from ohos rs
  ///
  virtual void SetNativeWindow(cef_native_window_t window) = 0;

  ///
  /// Set web debugging access
  ///
  virtual void SetWebDebuggingAccess(bool isEnableDebug) = 0;

  ///
  /// Get web debugging access
  ///
  virtual bool GetWebDebuggingAccess() = 0;

  ///
  /// GetImageForContextNode
  ///
  virtual void GetImageForContextNode(CefRefPtr<CefFrame> frame, int command_id) = 0;

  ///
  /// GetImageFromCache
  ///
  virtual void GetImageFromCache(const CefString& url, int command_id) = 0;

  ///
  /// GetImageFromCacheEX
  ///
  virtual void GetImageFromCacheEx(const CefString& url, int command_id) = 0;

  ///
  /// ExitFullScreen
  ///
  virtual void ExitFullScreen() = 0;

  ///
  /// SetFocusOnWeb
  ///
  virtual void SetFocusOnWeb() = 0;

  ///
  /// Send a isNeedSecurityLayer bool to the browser.
  ///
  /*--cef()--*/
  virtual void UpdateSecurityLayer(bool isNeedSecurityLayer) = 0;

  ///
  /// UpdateTextFieldStatus
  ///
  /*--cef()--*/
  virtual void UpdateTextFieldStatus(bool isShowKeyboard, bool isAttachIME) = 0;

  ///
  /// Set HasComposition
  ///
  virtual void SetHasComposition(bool has_composition) = 0;

  ///
  /// Set HasComposition
  ///
  virtual bool GetHasComposition() = 0;

  ///
  /// UpdateLocale
  ///
  virtual void UpdateLocale(const CefString& locale) = 0;

  ///
  /// Returns the original url of the request.
  ///
  virtual CefString GetOriginalUrl() = 0;

  ///
  /// Set network status
  ///
  virtual void PutNetworkAvailable(bool available) = 0;

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  ///
  /// Prerender the page includes loading subresources and excute javascript.
  ///
  virtual int PrerenderPage(const CefString& url,
                    const CefString& additional_headers) = 0;

  ///
  /// Cancel All Prerendered Pages.
  ///
  virtual void CancelAllPrerendering() = 0;
#endif

  ///
  /// Remove web cache
  ///
  virtual void RemoveCache(bool include_disk_files) = 0;

  ///
  /// Post task to ui thread.
  ///
  virtual void PostTaskToUIThread(CefRefPtr<CefTask> task) = 0;

  ///
  /// Set the virtual pixel ratio
  ///
  virtual void SetVirtualPixelRatio(float ratio) = 0;

  ///
  /// Get the virtual pixel ratio
  ///
  virtual float GetVirtualPixelRatio() = 0;

  ///
  /// Recompute the WebPreferences based on the current state of the
  /// CefSettings, we will also call SetWebPreferences and send the updated
  /// WebPreferences to all RenderViews by WebContents.
  ///
  virtual void SetWebPreferences(
      const CefBrowserSettings& browser_settings) = 0;

  ///
  /// PutUserAgent
  ///
  virtual void PutUserAgent(const CefString& ua, bool from_app) = 0;

  ///
  /// DefaultUserAgent
  ///
  virtual CefString DefaultUserAgent() = 0;

  ///
  /// GetCustomUserAgent
  ///
  virtual CefString GetCustomUserAgent() = 0;

  ///
  /// SetBackgroundColor
  ///
  virtual void SetBackgroundColor(int color) = 0;

  ///
  /// UpdateEasyListRules
  ///
  virtual void UpdateAdblockEasyListRules(long adBlockEasyListVersion) = 0;

  ///
  /// RegisterArkJSfunction
  ///
  virtual void RegisterArkJSfunction(
      const CefString& object_name,
      const std::vector<CefString>& method_list,
      const std::vector<CefString>& async_method_list,
      const int32_t object_id,
      const CefString& permission) = 0;

  ///
  /// UnregisterArkJSfunction
  ///
  virtual void UnregisterArkJSfunction(
      const CefString& object_name,
      const std::vector<CefString>& method_list) = 0;

  ///
  /// CallH5Function
  ///
  virtual void CallH5Function(int32_t routing_id,
                              int32_t h5_object_id,
                              const CefString& h5_method_name,
                              const std::vector<CefRefPtr<CefValue>>& args) = 0;

  ///
  /// Saves the current view as a web archive.
  ///
  virtual void StoreWebArchive(
      const CefString& base_name,
      bool auto_name,
      CefRefPtr<CefStoreWebArchiveResultCallback> callback) = 0;

  ///
  /// Notify the browser that the widget has been resized because of virtual
  /// keyboard.
  ///
  virtual void WasKeyboardResized() = 0;

  ///
  /// Set if lower the frame rate.
  ///
  virtual void SetEnableLowerFrameRate(bool enabled) = 0;
  /* ---------- ohos webview add end --------- */

  ///
  /// Gets the title for the current page.
  ///
  virtual CefString Title() = 0;

  ///
  /// Create a message channel, which include two message ports.
  ///
  virtual void CreateWebMessagePorts(std::vector<CefString>& ports) = 0;

  ///
  /// Posts a MessageEvent to the main frame.
  ///
  virtual void PostWebMessage(CefString& message,
                              std::vector<CefString>& ports,
                              CefString& targetUri) = 0;

  ///
  /// Close the web message port.
  ///
  virtual void ClosePort(const CefString& port_handle) = 0;

  ///
  /// Destroy all web message ports.
  ///
  virtual void DestroyAllWebMessagePorts() = 0;

  ///
  /// Post a message to the port.
  ///
  virtual void PostPortMessage(const CefString& port_handle,
                               CefRefPtr<CefValue> message) = 0;

  ///
  /// Set the callback of the port.
  ///
  virtual void SetPortMessageCallback(
      const CefString& port_handle,
      CefRefPtr<CefWebMessageReceiver> callback) = 0;

  ///
  /// Gets the latest hitdata
  ///
  virtual void GetHitData(int& type, CefString& extra_data) = 0;

  ///
  /// Gets the latest hitdata
  ///
  /*--cef()--*/
  virtual void GetLastHitData(int& type, CefString& extra_data) = 0;

  ///
  /// Set the inital page scale
  ///
  virtual void SetInitialScale(float scale) = 0;

  ///
  /// Gets the progress for the current page.
  ///
  virtual int PageLoadProgress() = 0;

  ///
  /// Gets the progress for the current page.
  ///
  virtual float Scale() = 0;

  ///
  /// Loads the given data into this WebView, using baseUrl as the base URL for
  /// the content. The base URL is used both to resolve relative URLs and when
  /// applying JavaScript's same origin policy. The historyUrl is used for the
  /// history entry.
  /// optional_param=baseUrl, optional_param=data, optional_param=mimeType,
  /// optional_param=encoding, optional_param=historyUrl
  ///
  virtual void LoadWithDataAndBaseUrl(const CefString& baseUrl,
                                      const CefString& data,
                                      const CefString& mimeType,
                                      const CefString& encoding,
                                      const CefString& historyUrl) = 0;

  ///
  /// Loads the given data into this WebView
  /// optional_param=data, optional_param=mimeType, optional_param=encoding,
  ///
  virtual void LoadWithData(const CefString& data,
                            const CefString& mimeType,
                            const CefString& encoding) = 0;
#if BUILDFLAG(ARKWEB_EXT_HTTPS_UPGRADES)
  ///
  /// Load the url with loadURLParams into this WebView
  ///
  virtual void LoadUrlWithParams(const std::string& url,
                                 const LoadUrlType load_type,
                                 const std::string& refer,
                                 const std::string& headers,
                                 const std::string& post_data,
                                 const bool allow_https_upgrade,
                                 int32_t transition_type) = 0;
#endif
  ///
  /// add visited url.
  ///
  virtual void AddVisitedLinks(const std::vector<CefString>& urls) {}

  ///
  /// Resume download after interrupted.
  ///
  virtual void ResumeDownload(const CefString& url,
                              const CefString& full_path,
                              int64_t received_bytes,
                              int64_t total_bytes,
                              const CefString& etag,
                              const CefString& mime_type,
                              const CefString& last_modified,
                              const CefString& received_slices_string) = 0;
  ///
  ///  Set the audio resume interval of the broswer.
  ///
  virtual void SetAudioResumeInterval(int resumeInterval) = 0;

  ///
  ///  Set whether the browser's audio is exclusive.
  ///
  virtual void SetAudioExclusive(bool audioExclusive) = 0;

  ///
  ///  Set the audio session type of the browser.
  ///
  virtual void SetAudioSessionType(int audioSessionType) = 0;

  ///
  /// Close fullScreen video.
  ///
  virtual void CloseMedia() = 0;

  ///
  /// Stop all audio and video playback on the web page.
  ///
  virtual void StopMedia() = 0;

  ///
  /// Restart playback of all audio and video on the web page.
  ///
  virtual void ResumeMedia() = 0;

  ///
  /// Pause all audio and video playback on the web page.
  ///
  virtual void PauseMedia() = 0;

  ///
  /// View the playback status of all audio and video on the web page.
  ///
  virtual int GetMediaPlaybackState() = 0;

  ///
  /// Scroll page up or down
  ///
  virtual void ScrollPageUpDown(bool is_up,
                                bool is_half,
                                float view_height) = 0;

  ///
  /// Get web history state
  ///
  virtual CefRefPtr<CefBinaryValue> GetWebState() = 0;

  ///
  /// Restore web history state
  ///
  virtual bool RestoreWebState(const CefRefPtr<CefBinaryValue> state) = 0;

  ///
  /// Scroll to the position.
  ///
  virtual void ScrollTo(float x, float y) = 0;

  ///
  /// Scroll by the delta distance.
  ///
  virtual void ScrollBy(float delta_x, float delta_y) = 0;

#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
  ///
  /// Set the bypass vsync condition.
  ///
  virtual void SetBypassVsyncCondition(int32_t condition) = 0;
#endif

  ///
  /// Slide Scroll by the speed.
  ///
  virtual void SlideScroll(float vx, float vy) = 0;

  ///
  /// Set whether webview can access files
  ///
  virtual void SetFileAccess(bool falg) = 0;

  ///
  /// Set whether webview can diallow sandbox file access from file url
  ///
  virtual void SetDisallowSandboxFileAccessFromFileUrl(bool falg) {};

  ///
  /// Set whether webview can access network
  ///
  virtual void SetBlockNetwork(bool falg) = 0;

  ///
  /// Set the cache mode of webview
  ///
  virtual void SetCacheMode(int falg) = 0;

  ///
  /// Set should frame submission before draw
  ///
  virtual void SetShouldFrameSubmissionBeforeDraw(bool should) = 0;

  ///
  /// Set zoom with the dela facetor
  ///
  virtual void ZoomBy(float delta, float width, float height) = 0;

  ///
  /// Set the window id of the UI framework
  ///
  virtual void SetWindowId(int window_id, int nweb_id) = 0;

  ///
  /// Set the token of the UI framework
  ///
  virtual void SetToken(void* token) = 0;

  ///
  /// Set the property values for width, height, and keyboard height
  ///
  virtual void SetVirtualKeyBoardArg(int32_t width,
                                     int32_t height,
                                     double keyboard) = 0;

  ///
  /// Set the virtual keyboard to override the web status
  ///
  virtual bool ShouldVirtualKeyboardOverlay() = 0;

  ///
  /// JavaScriptOnDocumentStart
  ///
  virtual void JavaScriptOnDocumentStart(
      const CefString& script,
      const std::vector<CefString>& script_rules,
      bool is_transfer_finished) = 0;

  ///
  /// RemoveJavaScriptOnDocumentStart
  ///
  virtual void RemoveJavaScriptOnDocumentStart() = 0;

  ///
  /// JavaScriptOnDocumentEnd
  ///
  virtual void JavaScriptOnDocumentEnd(
      const CefString& script,
      const std::vector<CefString>& script_rules,
      bool is_transfer_finished) = 0;

  ///
  /// RemoveJavaScriptOnDocumentEnd
  ///
  virtual void RemoveJavaScriptOnDocumentEnd() = 0;

  ///
  /// Set the draw rect
  ///
  virtual void SetDrawRect(int x, int y, int width, int height) = 0;

  ///
  /// Set the draw mode
  ///
  virtual void SetDrawMode(int mode) = 0;

  ///
  /// Create the Web print document adapter of the UI framework
  ///
  virtual void CreateWebPrintDocumentAdapter(
      const CefString& jobName,
      void** webPrintDocumentAdapter) = 0;

  ///
  /// Create the Web print document adapter v2 of the UI framework
  ///
  virtual void CreateWebPrintDocumentAdapterV2(
      const CefString& jobName,
      void** adapter) = 0;

  ///
  /// Set the over-scroll mode of web
  ///
  virtual void SetOverscrollMode(int mode) = 0;

  ///
  /// Change the zoom factor for browser zoom.
  /// If called on the UI thread the change will be applied immediately.
  /// Otherwise, the change will be applied asynchronously on the UI thread.
  ///
  virtual void SetBrowserZoomLevel(double zoomFactor) = 0;

  ///
  /// Get last selected text by parameters carried when showing context menu.
  ///
  virtual std::string GetSelectedTextFromContextParam() = 0;

#if BUILDFLAG(ARKWEB_DISCARD)
  ///
  /// Discard a webview window
  ///
  virtual bool Discard() = 0;

  ///
  /// Restore the discarded webview window
  ///
  virtual bool Restore() = 0;
#endif

  ///
  /// Get the top controls offset.
  ///
  virtual int GetTopControlsOffset() = 0;

  ///
  /// Get the shrink viewport height.
  ///
  virtual int GetShrinkViewportHeight() = 0;

#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
  ///
  /// Get the shrink viewport height.
  ///
  virtual int InsertBackForwardEntry(int index, const CefString& url) = 0;

  ///
  /// Get the shrink viewport height.
  ///
  virtual int UpdateNavigationEntryUrl(int index, const CefString& url) = 0;

  ///
  /// Get the shrink viewport height.
  ///
  virtual void ClearForwardList() = 0;
#endif

  ///
  /// Set background print enable.
  ///
  virtual void SetPrintBackground(bool enable) = 0;

  ///
  /// Get whether print background.
  ///
  virtual bool GetPrintBackground() = 0;

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  ///
  /// set Scrollable
  ///
  virtual void SetScrollable(bool enable, int scrollType) = 0;

  ///
  /// Set whether the soft keyboard is displayed.
  ///
  virtual void SetImeShow(bool visible) = 0;
#endif

  ///
  /// Get the last javascript proxy calling frame url.
  ///
  virtual CefString GetLastJavascriptProxyCallingFrameUrl() = 0;

  ///
  /// Start current camera.
  ///
  virtual void StartCamera() = 0;

  ///
  ///  Stop current camera.
  ///
  virtual void StopCamera() = 0;

  ///
  ///  Close current camera.
  ///
  virtual void CloseCamera() = 0;

  ///
  ///  Set NWebID.
  ///
  virtual void SetNWebId(int nWebId) = 0;

  ///
  /// get pendingSizeStatus.
  ///
  virtual bool GetPendingSizeStatus() = 0;

  ///
  /// Get CefDownloadItem by download_item_id.
  ///
  virtual CefRefPtr<CefDownloadItem> GetDownloadItem(uint32_t item_id) = 0;

  ///
  /// SetWakeLockHandler.
  ///
  virtual void SetWakeLockHandler(int32_t windowId,
                                  CefRefPtr<CefSetLockCallback> callback) = 0;

  ///
  /// Notify browser host needs reoload when the render process terminated.
  ///
  virtual void SetNeedsReload(bool needs_reload) = 0;

  ///
  /// Return true if needs reload page, or false if needs not reload.
  ///
  virtual bool NeedsReload() = 0;

  ///
  ///  precompile javascript and generate code cache.
  ///
  virtual void PrecompileJavaScript(
      const std::string& url,
      const std::string& script,
      CefRefPtr<CefCacheOptions> cacheOptions,
      CefRefPtr<CefPrecompileCallback> callback) = 0;

  ///
  /// Set whether use optimized HTML parser budget to reduce FCP time.
  ///
  virtual void SetOptimizeParserBudgetEnabled(bool enable) = 0;

  ///
  ///  update DrawRect.
  ///
  virtual void UpdateDrawRect() = 0;

  ///
  /// SendTouchpadFlingEvent
  ///
  virtual void SendTouchpadFlingEvent(const CefMouseEvent& event,
                                      double vx,
                                      double vy) = 0;

  ///
  /// Set the fit content mode
  ///
  virtual void SetFitContentMode(int mode) = 0;

  ///
  /// Terminate render process
  ///
  virtual bool TerminateRenderProcess() = 0;

  ///
  /// RegisterNativeJSProxy
  ///
  virtual void RegisterNativeJSProxy(const CefString& object_name,
                                     const std::vector<CefString>& method_list,
                                     const int32_t object_id,
                                     bool is_async,
                                     const CefString& permission) = 0;

  ///
  /// Called when text is selected.
  ///
  virtual void OnTextSelected(bool flag) = 0;

  ///
  /// Get page scale factor.
  ///
  virtual float GetPageScaleFactor() = 0;

  ///
  /// WebPageSnapshot, return result by callback
  ///
  virtual bool WebPageSnapshot(const char* id,
                               int width,
                               int height,
                               cef_web_snapshot_callback_t callback) = 0;

  ///
  /// Advance focus for IME to the browser.
  ///
  virtual void AdvanceFocusForIME(int focusType) = 0;

  ///
  /// Called when image analyzer overlay is destoryed.
  ///
  virtual void OnDestroyImageAnalyzerOverlay() = 0;

  ///
  /// Scroll to the position with anime.
  ///
  /*--cef()--*/
  virtual void ScrollToWithAnime(float x, float y, int32_t duration) = 0;

  ///
  /// Scroll by the delta with anime.
  ///
  /*--cef()--*/
  virtual void ScrollByWithAnime(float delta_x,
                                 float delta_y,
                                 int32_t duration) = 0;

#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
  ///
  /// Get the current scroll offset of the webpage.
  ///
  /*--cef()--*/
  virtual void GetScrollOffset(float* offset_x, float* offset_y) = 0;

  ///
  /// Get the current overscroll offset of the webpage.
  ///
  /*--cef()--*/
  virtual void GetOverScrollOffset(float* offset_x, float* offset_y) = 0;
#endif

  ///
  /// Called when the folding status of the phone screen changes.
  ///
  /*--cef()--*/
  virtual void OnFoldStatusChanged(uint32_t foldStatus) = 0;

  ///
  /// OnSafeInsetsChange
  ///
  virtual void OnSafeInsetsChange(int left, int top, int right, int bottom) = 0;

  ///
  /// Notify for next touch move event.
  ///
  virtual void NotifyForNextTouchEvent() = 0;

  ///
  /// Set grant file access dirs.
  ///
  virtual void SetGrantFileAccessDirs(
      const std::vector<CefString>& dir_list,
      const std::vector<CefString>& excluded_dir_list) = 0;

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  ///
  /// register the TabsEventRouter ZoomObserver.
  ///
  /*--cef()--*/
  virtual void WebExtensionRegisterZoomObserver() {}
 
  ///
  /// unregister the TabsEventRouter ZoomObserver
  ///
  /*--cef()--*/
  virtual void WebExtensionUnregisterZoomObserver() {}

  virtual void WebExtensionTabUpdated(
      int tab_id,
      std::unique_ptr<NWebExtensionTabChangeInfo> changeInfo,
      std::unique_ptr<NWebExtensionTab> tab) {}

  ///
  /// Receiving the tab removed notification.
  ///
  /*--cef()--*/
  virtual void WebExtensionTabRemoved(int tab_id,
    bool isWindowClosing, int windowId) {}

  ///
  /// Receiving the tab attached notification.
  ///
  /*--cef()--*/
  virtual void WebExtensionTabAttached(int tab_id,
    int new_position, int new_window_id) {}

  ///
  /// Receiving the tab detached notification.
  ///
  /*--cef()--*/
  virtual void WebExtensionTabDetached(int tab_id,
    const std::unique_ptr<NWebExtensionTabDetachInfo> detachInfo) {}

  ///
  /// Receiving the tab highlighted notification.
  ///
  /*--cef()--*/
  virtual void WebExtensionTabHighlighted(NWebExtensionTabHighlightInfo& highlightInfo) {}

  ///
  /// Receiving the tab moved notification.
  ///
  /*--cef()--*/
  virtual void WebExtensionTabMoved(int tab_id, const std::unique_ptr<NWebExtensionTabMoveInfo> moveInfo) {}

  ///
  /// Receiving the tab replaced notification.
  ///
  /*--cef()--*/
  virtual void WebExtensionTabReplaced(int32_t addedTabId, int32_t removedTabId) {}

  ///
  /// Receiving the view updated type.
  ///
  /*--cef()--*/
  virtual void WebExtensionSetViewType(int32_t type) {}
#endif

  ///
  /// ScrollFocusedEditableNodeIntoView.
  ///
  virtual void ScrollFocusedEditableNodeIntoView() = 0;

  ///
  /// Set the callback of the autofill event.
  ///
  virtual void SetAutofillCallback(
      CefRefPtr<CefWebMessageReceiver> callback) = 0;

  ///
  /// Fill autofill data.
  ///
  virtual void FillAutofillData(CefRefPtr<CefValue> message) = 0;

  ///
  /// Process autofill cancel content.
  ///
  virtual void ProcessAutofillCancel(const CefString& fillContent) = 0;

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  ///
  /// SetNativeEmbedMode.
  ///
  /*--cef()--*/
  virtual void SetNativeEmbedMode(bool flag) = 0;

  ///
  /// Set the native innner web
  ///
  /*--cef()--*/
  virtual void SetNativeInnerWeb(bool isInnerWeb) = 0;

  ///
  /// Set Enable Custom Video Player
  ///
  /*--cef()--*/
  virtual void SetEnableCustomVideoPlayer(bool flag) = 0;
#endif
  ///
  /// request autofill from IMF event.
  ///
  virtual void AutoFillWithIMFEvent(bool is_username,
                                    bool is_other_account,
                                    bool is_new_password,
                                    const CefString& content) = 0;

#if BUILDFLAG(ARKWEB_JSPROXY)
  ///
  /// JavaScriptOnHeadReady
  ///
  /*--cef()--*/
  virtual void JavaScriptOnHeadReady(
      const CefString& script,
      const std::vector<CefString>& script_rules,
      bool is_transfer_finished) = 0;

  ///
  /// RemoveJavaScriptOnHeadReady
  ///
  /*--cef()--*/
  virtual void RemoveJavaScriptOnHeadReady() = 0;
#endif

  ///
  /// Set zoom with the dela facetor
  ///
  /*--cef()--*/
  virtual void ScaleGestureChangeV2(int type,
                                    float scale,
                                    float originScale,
                                    float width,
                                    float height) = 0;

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
  ///
  /// Get language of the webview.
  ///
  /*--cef()--*/
  virtual std::string GetCurrentLanguage() = 0;
#endif

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
  ///
  /// Web maximize resize optimize.
  ///
  virtual void MaximizeResize() = 0;
#endif  // ARKWEB_MAXIMIZE_RESIZE

#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
  ///
  /// Set whether to connect to media avsession.
  ///
  /*--cef()--*/
  virtual void PutWebMediaAVSessionEnabled(bool enable) = 0;
#endif  // ARKWEB_MEDIA_AVSESSION

#if BUILDFLAG(ARKWEB_OCCLUDED_OPT)
  ///
  /// Set if half the frame rate.
  ///
  virtual void SetEnableHalfFrameRate(bool enabled) = 0;
#endif
  ///
  /// Set focus by position.
  ///
  virtual bool SetFocusByPosition(float x, float y) = 0;

#if BUILDFLAG(ARKWEB_AI)
  /// 
  /// get data detector select text
  ///
  virtual std::string GetDataDetectorSelectText() = 0;

  ///
  /// On data detector select text.
  ///
  virtual void OnDataDetectorSelectText() = 0;
#endif

#if BUILDFLAG(IS_ARKWEB)
  /// 
  /// set applink enable
  ///
  virtual void EnableAppLinking(bool enable) = 0;
 
  /// 
  /// get app link status
  ///
  virtual bool IsAppLinkingEnabled() const = 0;
#endif

  ///
  /// Execute a string of JavaScript code in frames.
  ///
  virtual void RunJavaScriptInFrames(const std::string& jsString, FrameInfos rootFrame,
                                     bool recursive, IsolatedWorld world,
                                     CefRefPtr<CefJavaScriptResultCallback> callback) = 0;

#if BUILDFLAG(ARKWEB_BGTASK)
  ///
  /// Notify browser is foreground.
  ///
  /*--cef()--*/
  virtual void OnBrowserForeground() = 0;

  ///
  /// Notify browser is background.
  ///
  /*--cef()--*/
  virtual void OnBrowserBackground() = 0;
#endif

#if BUILDFLAG(ARKWEB_READER_MODE)
  virtual void Distill(const std::string& guid, const DistillOptions& distill_options,
    CefRefPtr<CefDistillCallback> callback) = 0;
  virtual void AbortDistill() = 0;
#endif // ARKWEB_READER_MODE

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  ///
  /// get the focused frame info
  ///
  virtual void GetFocusedFrameInfo(int32_t& frame_id, CefString& frame_url) = 0;
#endif

#if BUILDFLAG(ARKWEB_EXT_HTTPS_UPGRADES)
  ///
  /// enable or disable https upgrades function
  ///
  virtual void EnableHttpsUpgrades(bool enable) = 0;
#endif

  ///
  /// Handle extend action from input method.
  ///
  virtual void HandleInputMethodExtendAction(int32_t action) {}
};

#endif  // ARKWEB_INCLUDE_CEF_BROWSER_H_
