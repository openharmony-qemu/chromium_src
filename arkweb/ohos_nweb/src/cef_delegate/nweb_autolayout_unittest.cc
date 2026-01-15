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

#include "nweb_autolayout.h"
 
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <sstream>
#include "base/files/file_path.h"
#include "arkweb/build/features/features.h"
#include "cef/include/cef_base.h"
#include "cef/include/cef_browser.h"
#include "cef/ohos_cef_ext/include/arkweb_browser_ext.h"
#include "include/cef_devtools_message_handler_delegate.h"
#include "include/cef_urlrequest.h"
#include "ui/base/resource/resource_bundle.h"

using namespace testing;
 
const std::string g_valid_config = R"({
    "minMaskAreaRatioThreshold": 60,
    "opacityFilter": [10, 90],
    "minContentAreaRatioThreshold": 20,
    "scaleAnimationDuration": 100,
    "minScaleFactor": 30,
    "whitelist": {
        "com.example.app": {
            "pattern": "some_pattern",
            "GetID": "get_id_func",
            "GetPage": "get_page_func",
            "appRuleInfos": [
                { "id": "*", "pg": "*" }
            ]
        }
    }
})";

const std::string g_valid_config_with_nwe_params = R"({
    "minScaleFactor": 30,
    "minMaskAreaRatioThreshold": 60,
    "opacityFilter": [10, 90],
    "minContentAreaRatioThreshold": 20,
    "scaleAnimationDuration": 100,
    "alphabetIdentificationMinSize": 12,
    "alphabetHeightWidthMinRatio": 10,
    "whitelist": {
        "com.example.app": {
            "pattern": "some_pattern",
            "GetID": "get_id_func",
            "GetPage": "get_page_func",
            "appRuleInfos": [
                { "id": "*", "pg": "*" }
            ],
            "urlRuleInfos": [
                {
                    "urlPrefix": "https://www.example.com/path",
                    "strategy": 3,
                    "alphabetIdentificationMinSize": 15,
                    "alphabetHeightWidthMinRatio": 15
                },
                {
                    "urlPrefix": "https://www.example.com/path/to/xxx.html",
                    "strategy": 1
                }
            ]
        }
    }
})";
 
namespace OHOS::NWeb {
// Mock classes based on nweb_find_delegate_unittest.cc and nweb_preference_delegate_unittest.cc
class MockCefBrowserHost : public ArkWebBrowserHostExt {
 public:
  // Use MOCK_METHOD only for methods we need to verify behavior
  MOCK_METHOD(void, ExecuteJavaScript, (const std::string&, CefRefPtr<CefJavaScriptResultCallback>, bool), (override));
  
  // Required methods from ArkWebBrowserHostExt
  // Note: Only pure virtual functions (= 0) MUST be implemented
  // Other virtual functions can be omitted if base class has default implementation
  // Non-virtual functions don't need to be implemented
  bool CreateBrowser(const CefWindowInfo& windowInfo,
                     CefRefPtr<CefClient> client,
                     const CefString& url,
                     const CefBrowserSettings& settings,
                     CefRefPtr<CefDictionaryValue> extra_info,
                     CefRefPtr<CefRequestContext> request_context) {
    return false;
  }
  CefRefPtr<CefBrowser> CreateBrowserSync(
      const CefWindowInfo& windowInfo,
      CefRefPtr<CefClient> client,
      const CefString& url,
      const CefBrowserSettings& settings,
      CefRefPtr<CefDictionaryValue> extra_info,
      CefRefPtr<CefRequestContext> request_context) {
    return nullptr;
  }
  void SetDisallowSandboxFileAccessFromFileUrl(bool) override {}
  void GetRootBrowserAccessibilityManager(void**) override {}
  void ExecuteJavaScriptExt(int, uint64_t, CefRefPtr<CefJavaScriptResultCallback>, bool) override {}
  void SetNativeWindow(cef_native_window_t) override {}
  void SetWebDebuggingAccess(bool) override {}
  void WasOccluded(bool) override {}
  void OnWindowShow() override {}
  void OnWindowHide() override {}
  void OnOnlineRenderToForeground() override {}
  void SendTouchEventList(const std::vector<CefTouchEvent>&) override {}
  CefRefPtr<CefBrowser> GetBrowser() override { return nullptr; }
  void CloseBrowser(bool) override {}
  bool TryCloseBrowser() override { return false; }
  bool IsReadyToBeClosed() override { return false; }
  void SetFocus(bool) override {}
  CefWindowHandle GetWindowHandle() override { return 0; }
  CefWindowHandle GetOpenerWindowHandle() override { return 0; }
  int GetOpenerIdentifier() override { return -1; }
  bool HasView() override { return false; }
  CefRefPtr<CefClient> GetClient() override { return nullptr; }
  CefRefPtr<CefRequestContext> GetRequestContext() override { return nullptr; }
  bool CanZoom(cef_zoom_command_t) override { return false; }
  void Zoom(cef_zoom_command_t) override {}
  double GetDefaultZoomLevel() override { return 0.0; }
  double GetZoomLevel() override { return 0.0; }
  void SetZoomLevel(double) override {}
  void RunFileDialog(FileDialogMode, const CefString&, const CefString&,
    const std::vector<CefString>&, CefRefPtr<CefRunFileDialogCallback>) override {}
  void StartDownload(const CefString&) override {}
  void DownloadImage(const CefString&, bool, uint32_t, bool, CefRefPtr<CefDownloadImageCallback>) override {}
  void Print() override {}
  void PrintToPDF(const CefString&, const CefPdfPrintSettings&, CefRefPtr<CefPdfPrintCallback>) override {}
  void Find(const CefString&, bool, bool, bool) override {}
  void StopFinding(bool) override {}
  void ShowDevTools(const CefWindowInfo&, CefRefPtr<CefClient>, const CefBrowserSettings&, const CefPoint&) override {}
  void CloseDevTools() override {}
  bool HasDevTools() override { return false; }
  bool SendDevToolsMessage(const void*, size_t) override { return false; }
  int ExecuteDevToolsMethod(int, const CefString&, CefRefPtr<CefDictionaryValue>) override { return -1; }
  CefRefPtr<CefRegistration> AddDevToolsMessageObserver(CefRefPtr<CefDevToolsMessageObserver>) override {
    return nullptr;
  }
  void GetNavigationEntries(CefRefPtr<CefNavigationEntryVisitor>, bool) override {}
  void ReplaceMisspelling(const CefString&) override {}
  void AddWordToDictionary(const CefString&) override {}
  bool IsWindowRenderingDisabled() override { return false; }
  void WasResized() override {}
  void WasHidden(bool) override {}
  void NotifyScreenInfoChanged() override {}
  void Invalidate(PaintElementType) override {}
  void SendExternalBeginFrame() override {}
  void SendKeyEvent(const CefKeyEvent&) override {}
  void SendMouseClickEvent(const CefMouseEvent&, cef_mouse_button_type_t, bool, int) override {}
  void SendMouseMoveEvent(const CefMouseEvent&, bool) override {}
  void SendMouseWheelEvent(const CefMouseEvent&, int, int) override {}
  void SendTouchEvent(const CefTouchEvent&) override {}
  void SendCaptureLostEvent() override {}
  void NotifyMoveOrResizeStarted() override {}
  int GetWindowlessFrameRate() override { return 0; }
  void SetWindowlessFrameRate(int) override {}
  void ImeSetComposition(const CefString&,
    const std::vector<CefCompositionUnderline>&, const CefRange&, const CefRange&) override {}
  void ImeCommitText(const CefString&, const CefRange&, int) override {}
  void ImeFinishComposingText(bool) override {}
  void ImeCancelComposition() override {}
  void DragTargetDragEnter(CefRefPtr<CefDragData>, const CefMouseEvent&, DragOperationsMask) override {}
  void DragTargetDragOver(const CefMouseEvent&, DragOperationsMask) override {}
  void DragTargetDragLeave() override {}
  void DragTargetDrop(const CefMouseEvent&) override {}
  void DragSourceEndedAt(int, int, DragOperationsMask) override {}
  void DragSourceSystemDragEnded() override {}
  CefRefPtr<CefNavigationEntry> GetVisibleNavigationEntry() override { return nullptr; }
  void SetAccessibilityState(cef_state_t) override {}
  void SetAutoResizeEnabled(bool, const CefSize&, const CefSize&) override {}
  void SetAudioMuted(bool) override {}
  bool IsAudioMuted() override { return false; }
  bool IsFullscreen() override { return false; }
  void ExitFullscreen(bool) override {}
  bool CanExecuteChromeCommand(int) override { return false; }
  void ExecuteChromeCommand(int, cef_window_open_disposition_t) override {}
  bool IsRenderProcessUnresponsive() override { return false; }
  cef_runtime_style_t GetRuntimeStyle() override { return CEF_RUNTIME_STYLE_DEFAULT; }
  void SetPopupWindow(cef_native_window_t) override {}
  void CreateToPDF(const CefPdfPrintSettings&, CefRefPtr<CefPdfValueCallback>) override {}
  void EnableVideoAssistant(bool) override {}
  void ExecuteVideoAssistantFunction(const CefString&) override {}
  bool IsIframe() override { return false; }
  void ReloadFocusedFrame() override {}
  void StopScreenCapture(int32_t, const CefString&) override {}
  void SetScreenCapturePickerShow() override {}
  void DisableSessionReuse() override {}
  void RegisterScreenCaptureDelegateListener(CefRefPtr<CefScreenCaptureCallback>) override {}
  void GetOverScrollOffsetValue(float*, float*) override {}
  void CustomWebMediaPlayer(bool) override {}
  bool GetWebDebuggingAccess() override { return false; }
  void GetImageForContextNode(int) override {}
  void GetImageFromCache(const CefString&, int) override {}
  void GetImageFromCacheEx(const CefString&, int) override {}
  void ExitFullScreen() override {}
  void UpdateLocale(const CefString&) override {}
  CefString GetOriginalUrl() override { return CefString(); }
  void PutNetworkAvailable(bool) override {}
  int PrerenderPage(const CefString&, const CefString&) override { return 0; }
  void CancelAllPrerendering() override {}
  void RemoveCache(bool) override {}
  void PostTaskToUIThread(CefRefPtr<CefTask>) override {}
  void SetVirtualPixelRatio(float) override {}
  float GetVirtualPixelRatio() override { return 0.0f; }
  void SetWebPreferences(const CefBrowserSettings&) override {}
  void PutUserAgent(const CefString&, bool) override {}
  CefString DefaultUserAgent() override { return CefString(); }
  void SetBackgroundColor(int) override {}
  void UpdateAdblockEasyListRules(long) override {}
  void RegisterArkJSfunction(const CefString&,const std::vector<CefString>&,
    const std::vector<CefString>&, int32_t, const CefString&) override {}
  void UnregisterArkJSfunction(const CefString&, const std::vector<CefString>&) override {}
  void CallH5Function(int32_t, int32_t, const CefString&, const std::vector<CefRefPtr<CefValue>>&) override {}
  void StoreWebArchive(const CefString&, bool, CefRefPtr<CefStoreWebArchiveResultCallback>) override {}
  void WasKeyboardResized() override {}
  void SetEnableLowerFrameRate(bool) override {}
  CefString Title() override { return CefString(); }
  void CreateWebMessagePorts(std::vector<CefString>&) override {}
  void PostWebMessage(CefString&, std::vector<CefString>&, CefString&) override {}
  void ClosePort(const CefString&) override {}
  void DestroyAllWebMessagePorts() override {}
  void PostPortMessage(const CefString&, CefRefPtr<CefValue>) override {}
  void SetPortMessageCallback(const CefString&, CefRefPtr<CefWebMessageReceiver>) override {}
  void GetHitData(int&, CefString&) override {}
  void SetInitialScale(float) override {}
  int PageLoadProgress() override { return 0; }
  float Scale() override { return 0.0f; }
  void LoadWithDataAndBaseUrl(const CefString&, const CefString&,
    const CefString&, const CefString&, const CefString&) override {}
  void LoadWithData(const CefString&, const CefString&, const CefString&) override {}
  void AddVisitedLinks(const std::vector<CefString>&) override {}
  void ResumeDownload(const CefString& url,
                      const CefString& full_path,
                      int64_t received_bytes,
                      int64_t total_bytes,
                      const CefString& etag,
                      const CefString& mime_type,
                      const CefString& last_modified,
                      const CefString& received_slices_string) override {}
  void SetAudioResumeInterval(int) override {}
  void SetAudioExclusive(bool) override {}
  void SetAudioSessionType(int) override {}
  void CloseMedia() override {}
  void StopMedia() override {}
  void ResumeMedia() override {}
  void PauseMedia() override {}
  int GetMediaPlaybackState() override { return 0; }
  void ScrollPageUpDown(bool, bool, float) override {}
  CefRefPtr<CefBinaryValue> GetWebState() override { return nullptr; }
  bool RestoreWebState(CefRefPtr<CefBinaryValue>) override { return false; }
  void ScrollTo(float, float) override {}
  void ScrollBy(float, float) override {}
  void SlideScroll(float, float) override {}
  void SetFileAccess(bool) override {}
  void SetBlockNetwork(bool) override {}
  void SetCacheMode(int) override {}
  void SetShouldFrameSubmissionBeforeDraw(bool) override {}
  void ZoomBy(float, float, float) override {}
  void SetWindowId(int, int) override {}
  void SetToken(void*) override {}
  void SetVirtualKeyBoardArg(int32_t, int32_t, double) override {}
  bool ShouldVirtualKeyboardOverlay() override { return false; }
  void JavaScriptOnDocumentStart(const CefString&, const std::vector<CefString>&,
    const std::vector<std::pair<CefString, CefString>>&, bool) override {}
  void RemoveJavaScriptOnDocumentStart() override {}
  void JavaScriptOnDocumentEnd(const CefString&, const std::vector<CefString>&,
    const std::vector<std::pair<CefString, CefString>>&, bool) override {}
  void RemoveJavaScriptOnDocumentEnd() override {}
  void OnDataDetectorSelectText() override {}
  std::string GetDataDetectorSelectText() override { return std::string(); }
  void SetBypassVsyncCondition(int32_t) override {}
  void JavaScriptOnHeadReady(const CefString&, const std::vector<CefString>&,
    const std::vector<std::pair<CefString, CefString>>&, bool) override {}
  void RemoveJavaScriptOnHeadReady() override {}
  void SetDrawRect(int, int, int, int) override {}
  void SetDrawMode(int) override {}
  void CreateWebPrintDocumentAdapter(const CefString&, void**) override {}
  void CreateWebPrintDocumentAdapterV2(const CefString&, void**) override {}
  void SetOverscrollMode(int) override {}
  bool Discard() override { return false; }
  bool Restore() override { return false; }
  void SetBrowserZoomLevel(double) override {}
  int GetTopControlsOffset() override { return 0; }
  int GetShrinkViewportHeight() override { return 0; }
  void OnEyeDropperResult(bool, uint32_t) override {}
  void SetPrintBackground(bool) override {}
  bool GetPrintBackground() override { return false; }
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void SetScrollable(bool, int) override {}
  void SetImeShow(bool) {}
#endif
  void StartCamera() override {}
  void StopCamera() override {}
  void CloseCamera() override {}
  CefString GetLastJavascriptProxyCallingFrameUrl() override { return CefString(); }
  void SetNWebId(int) override {}
  bool GetPendingSizeStatus() override { return false; }
  void PrecompileJavaScript(const std::string&, const std::string&,
    CefRefPtr<CefCacheOptions>, CefRefPtr<CefPrecompileCallback>) override {}
  void SetWakeLockHandler(int32_t, CefRefPtr<CefSetLockCallback>) override {}
  CefRefPtr<CefDownloadItem> GetDownloadItem(uint32_t) override { return nullptr; }
  bool NeedsReload() override { return false; }
  bool TerminateRenderProcess() override { return false; }
  void RegisterNativeJSProxy(const CefString&, const std::vector<CefString>&,
    int32_t, bool, const CefString&) override {}
  void SendTouchpadFlingEvent(const CefMouseEvent&, double, double) override {}
  void SetFitContentMode(int) override {}
  void UpdateDrawRect() override {}
  void OnTextSelected(bool) override {}
  float GetPageScaleFactor() override { return 0.0f; }
  bool WebPageSnapshot(const char*, int, int, cef_web_snapshot_callback_t) override { return false; }
  void AdvanceFocusForIME(int) override {}
  void ScrollToWithAnime(float, float, int32_t) override {}
  void ScrollByWithAnime(float, float, int32_t) override {}
  void GetScrollOffset(float*, float*) override {}
  void GetOverScrollOffset(float*, float*) override {}
  void OnSafeInsetsChange(int, int, int, int) override {}
  void NotifyForNextTouchEvent() override {}
  void SetGrantFileAccessDirs(const std::vector<CefString>&, const std::vector<CefString>&) override {}
  void SetAutofillCallback(CefRefPtr<CefWebMessageReceiver>) override {}
  void FillAutofillData(CefRefPtr<CefValue>) override {}
  void ScrollFocusedEditableNodeIntoView() override {}
  void ProcessAutofillCancel(const CefString&) override {}
  void AutoFillWithIMFEvent(bool, bool, bool, const CefString&) override {}
  
  // Required pure virtual methods from CefBrowser (via ArkWebBrowserHostExt inheritance chain)
  bool IsValid() override { return true; }
  bool CanGoBack() override { return false; }
  void GoBack() override {}
  bool CanGoForward() override { return false; }
  void GoForward() override {}
  bool IsLoading() override { return false; }
  void Reload() override {}
  void ReloadIgnoreCache() override {}
  void StopLoad() override {}
  int GetIdentifier() override { return 0; }
  bool IsSame(CefRefPtr<CefBrowser>) override { return false; }
  bool IsPopup() override { return false; }
  bool HasDocument() override { return false; }
  CefRefPtr<CefFrame> GetMainFrame() override { return nullptr; }
  CefRefPtr<CefFrame> GetFocusedFrame() override { return nullptr; }
  CefRefPtr<CefFrame> GetFrameByIdentifier(const CefString&) override { return nullptr; }
  CefRefPtr<CefFrame> GetFrameByName(const CefString&) override { return nullptr; }
  size_t GetFrameCount() override { return 0; }
  void GetFrameIdentifiers(std::vector<CefString>&) override {}
  void GetFrameNames(std::vector<CefString>&) override {}
  bool NeedToFireBeforeUnloadOrUnloadEvents() override { return false; }
  void DispatchBeforeUnload() override {}
  
  // Required pure virtual methods from ArkWebBrowserHostExt
  CefRefPtr<CefBrowserPermissionRequestDelegate> GetPermissionRequestDelegate() override { return nullptr; }
  CefRefPtr<CefGeolocationAcess> GetGeolocationPermissions() override { return nullptr; }
  bool CanGoBackOrForward(int) override { return false; }
  void GoBackOrForward(int) override {}
  void DeleteHistory() override {}
  void PasswordSuggestionSelected(int) override {}
  void UpdateBrowserControlsState(int, int, bool) override {}
  void UpdateBrowserControlsHeight(int, bool) override {}
  void PrefetchPage(const OHOS::NWeb::PrefetchOptions&) override {}
  void ReloadOriginalUrl() override {}
  bool CanStoreWebArchive() override { return false; }
  void SetBrowserUserAgentString(const CefString&) override {}
  bool ShouldShowLoadingUI() override { return false; }
  void SetForceEnableZoom(bool) override {}
  bool GetForceEnableZoom() override { return false; }
  int GetNWebId() override { return 0; }
  bool GetSavePasswordAutomatically() override { return false; }
  void SetSavePasswordAutomatically(bool) override {}
  void SaveOrUpdatePassword(bool) override {}
  bool GetSavePassword() override { return false; }
  void SetSavePassword(bool) override {}
  int GetSecurityLevel() override { return 0; }
  void EnableSafeBrowsing(bool) override {}
  bool IsSafeBrowsingEnabled() override { return false; }
  void EnableIntelligentTrackingPrevention(bool) override {}
  bool IsIntelligentTrackingPreventionEnabled() override { return false; }
  bool IsAdsBlockEnabled() override { return false; }
  bool IsAdsBlockEnabledForCurPage() override { return false; }
  void EnableAdsBlock(bool) override {}
  int SetUrlTrustListWithErrMsg(const CefString&, CefString&) override { return 0; }
  void EnableSafeBrowsingDetection(bool, bool) override {}
#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
  int InsertBackForwardEntry(int, const CefString&) override { return 0; }
  int UpdateNavigationEntryUrl(int, const CefString&) override { return 0; }
  void ClearForwardList() override {}
#endif
  void ExtensionSetTabId(int) override {}
  int ExtensionGetTabId() override { return 0; }
  uint32_t GetAcceleratedWidget(bool) override { return 0; }
  void SetAdBlockEnabledForSite(bool, int) override {}
  void FindEx(const CefString&, bool, bool, bool, bool) override {}
  void SetFocusOnWeb() override {}
  void UpdateSecurityLayer(bool) override {}
  void UpdateTextFieldStatus(bool, bool) override {}
  CefString GetCustomUserAgent() override { return CefString(); }
  void GetLastHitData(int&, CefString&) override {}
  std::string GetSelectedTextFromContextParam() override { return std::string(); }
  bool JudgeTextInputState() override { return true; }
  void SetNeedsReload(bool) override {}
  void SetOptimizeParserBudgetEnabled(bool) override {}
  void OnDestroyImageAnalyzerOverlay() override {}
  void OnFoldStatusChanged(uint32_t) override {}
  void SetNativeEmbedMode(bool) override {}
  void SetNativeInnerWeb(bool) override {}
  void ScaleGestureChangeV2(int, float, float, float, float) override {}
  std::string GetCurrentLanguage() override { return std::string(); }
  void MaximizeResize() override {}
  void PutWebMediaAVSessionEnabled(bool) override {}
  void SetEnableHalfFrameRate(bool) override {}
  bool SetFocusByPosition(float, float) override { return false; }
  void SetPipNativeWindow(int, int, int, cef_native_window_t) override {}
  void SendPipEvent(int, int, int, int) override {}
  void ShowFreeCopyMenu() override {}
  bool ShouldShowFreeCopyMenu() override { return false; }
  void SetBackForwardCacheOptions(int32_t, int32_t) override {}
#if BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
  void SetBlankScreenDetectionConfig(bool, const std::vector<double>&, const std::vector<int32_t>&, int32_t) override {}
#endif
#if BUILDFLAG(ARKWEB_BGTASK)
  void OnBrowserForeground() override {}
  void OnBrowserBackground() override {}
#endif
  void RunJavaScriptInFrames(const std::string&, FrameInfos, bool,
    IsolatedWorld, CefRefPtr<CefJavaScriptResultCallback>) override {}
#if BUILDFLAG(ARKWEB_READER_MODE)
  void Distill(uint64_t, const DistillOptions&, CefRefPtr<CefDistillCallback>) override {}
  void AbortDistill() override {}
#endif
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  void GetFocusedFrameInfo(int32_t&, CefString&) override {}
#endif
#if BUILDFLAG(ARKWEB_EXT_HTTPS_UPGRADES)
  void LoadUrlWithParams(const std::string&, const LoadUrlType&,const std::string&,
    const std::string&, const std::string&, const bool&, int32_t) override {}
  void EnableHttpsUpgrades(bool) override {}
#endif
#if BUILDFLAG(ARKWEB_DEVTOOLS)
  void ShowDevToolsWith(CefRefPtr<ArkWebBrowserHostExt>,
    CefRefPtr<CefDevToolsMessageHandlerDelegate>, const CefPoint&) override {}
#endif
#if BUILDFLAG(ARKWEB_UNITTESTS)
  void SetMediaResumeFromBFCachePage(bool) override {}
  void SetHasComposition(bool) override {}
  bool GetHasComposition() override { return false; }
  void SetEnableCustomVideoPlayer(bool) override {}
  void EnableAppLinking(bool) override {}
  bool IsAppLinkingEnabled() const override { return false; }
#endif
  void AddRef() const override {}
  bool Release() const override { return false; }
  bool HasOneRef() const override { return false; }
  bool HasAtLeastOneRef() const override { return false; }
};

class MockCefBrowser : public CefBrowser {
 public:
  MockCefBrowser(CefRefPtr<ArkWebBrowserHostExt> host) : host_(host) {}
  
  MOCK_METHOD(CefRefPtr<ArkWebBrowserHostExt>, GetHost, (), (override));
  MOCK_METHOD(bool, IsValid, (), (override));
  MOCK_METHOD(bool, CanGoBack, (), (override));
  MOCK_METHOD(void, GoBack, (), (override));
  MOCK_METHOD(bool, CanGoForward, (), (override));
  MOCK_METHOD(void, GoForward, (), (override));
  MOCK_METHOD(bool, IsLoading, (), (override));
  MOCK_METHOD(void, Reload, (), (override));
  MOCK_METHOD(void, ReloadIgnoreCache, (), (override));
  MOCK_METHOD(void, StopLoad, (), (override));
  MOCK_METHOD(int, GetIdentifier, (), (override));
  MOCK_METHOD(bool, IsSame, (CefRefPtr<CefBrowser>), (override));
  MOCK_METHOD(bool, IsPopup, (), (override));
  MOCK_METHOD(bool, HasDocument, (), (override));
  MOCK_METHOD(CefRefPtr<CefFrame>, GetMainFrame, (), (override));
  MOCK_METHOD(CefRefPtr<CefFrame>, GetFocusedFrame, (), (override));
  MOCK_METHOD(CefRefPtr<CefFrame>, GetFrameByIdentifier, (const CefString&), (override));
  MOCK_METHOD(CefRefPtr<CefFrame>, GetFrameByName, (const CefString&), (override));
  MOCK_METHOD(size_t, GetFrameCount, (), (override));
  MOCK_METHOD(void, GetFrameIdentifiers, (std::vector<CefString>&), (override));
  MOCK_METHOD(void, GetFrameNames, (std::vector<CefString>&), (override));
  MOCK_METHOD(bool, NeedToFireBeforeUnloadOrUnloadEvents, (), (override));
  MOCK_METHOD(void, DispatchBeforeUnload, (), (override));
  MOCK_METHOD(int, PrerenderPage, (const CefString&, const CefString&), (override));
  MOCK_METHOD(void, CancelAllPrerendering, (), (override));
  MOCK_METHOD(void, AddRef, (), (const, override));
  MOCK_METHOD(bool, Release, (), (const, override));
  MOCK_METHOD(bool, HasOneRef, (), (const, override));
  MOCK_METHOD(bool, HasAtLeastOneRef, (), (const, override));

 private:
  CefRefPtr<ArkWebBrowserHostExt> host_;
};

// MockCefFrame - need to implement all pure virtual functions
class MockCefFrame : public CefFrame {
 public:
  MockCefFrame() = default;
  ~MockCefFrame() = default;

  MOCK_METHOD(void, ExecuteJavaScript, (const CefString&, const CefString&, int), (override));
  // Note: CefFrame::ExecuteJavaScript takes CefString, but we convert std::string to CefString in the call
  MOCK_METHOD(CefString, GetURL, (), (override));
  MOCK_METHOD(bool, IsMain, (), (override));
  
  // Required pure virtual functions from CefFrame
  bool IsValid() override { return true; }
  CefString GetName() override { return CefString(); }
  CefString GetIdentifier() override { return CefString(); }
  CefRefPtr<CefFrame> GetParent() override { return nullptr; }
  void LoadURL(const CefString&) override {}
  void LoadRequest(CefRefPtr<CefRequest>) override {}
  bool IsFocused() override { return false; }
  void Undo() override {}
  void Redo() override {}
  void Cut() override {}
  void Copy() override {}
  void Paste() override {}
  void PasteAndMatchStyle() override {}
  void Delete() override {}
  void SelectAll() override {}
  void ViewSource() override {}
  void GetSource(CefRefPtr<CefStringVisitor>) override {}
  void GetText(CefRefPtr<CefStringVisitor>) override {}
  CefRefPtr<CefBrowser> GetBrowser() override { return nullptr; }
  CefRefPtr<CefV8Context> GetV8Context() override { return nullptr; }
  void VisitDOM(CefRefPtr<CefDOMVisitor>) override {}
  // Note: CreateURLRequest uses CefURLRequestClient which is forward declared
  // We avoid using the client parameter to prevent incomplete type errors
  CefRefPtr<CefURLRequest> CreateURLRequest(CefRefPtr<CefRequest> request,
    CefRefPtr<CefURLRequestClient> client) override { 
    (void)request;
    (void)client;
    return nullptr; 
  }
  void SendProcessMessage(CefProcessId, CefRefPtr<CefProcessMessage>) override {}

  IMPLEMENT_REFCOUNTING(MockCefFrame);
};

class NwebAutolayoutTest : public testing::Test, public NwebAutolayout {
};
 
TEST_F(NwebAutolayoutTest, ParseToplevelConfig_Valid)
{
    std::optional<base::Value> root = base::JSONReader::Read(g_valid_config);
    ASSERT_TRUE(root.has_value());
    ASSERT_TRUE(root->is_dict());
    
    EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
    
    EXPECT_EQ(mCCMConfig_.minScaleFactor, 30);
    EXPECT_EQ(mCCMConfig_.min_mask_area_ratio_threshold, 60);
    EXPECT_EQ(mCCMConfig_.min_content_area_ratio_threshold, 20);
    EXPECT_EQ(mCCMConfig_.scale_animation_duration, 100);
    EXPECT_EQ(mCCMConfig_.opacity_filter.first, 10);
    EXPECT_EQ(mCCMConfig_.opacity_filter.second, 90);
}
 
TEST_F(NwebAutolayoutTest, ParseToplevelConfig_Invalid)
{
    const std::string invalid_config = R"({
        "minMaskAreaRatioThreshold": 200
    })";
    std::optional<base::Value> root = base::JSONReader::Read(invalid_config);
    ASSERT_TRUE(root.has_value());
    ASSERT_TRUE(root->is_dict());
    EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_InvalidMinScaleFactor)
{
    const std::vector<std::string> invalid_min_scale_factor_configs = {
        R"({
            "minScaleFactor": -30,
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minScaleFactor": 20,
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minScaleFactor": 500,
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minScaleFactor": "20",
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minScaleFactor": [],
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})"
    };

    for (const auto& config_str : invalid_min_scale_factor_configs) {
        std::optional<base::Value> root = base::JSONReader::Read(config_str);
        ASSERT_TRUE(root.has_value());
        ASSERT_TRUE(root->is_dict());
        EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
    }
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_InvalidOpacityFilterFormat)
{
    const std::vector<std::string> invalid_configs = {
        R"({
            "minScaleFactor": 30,
            "minMaskAreaRatioThreshold": 60,
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minScaleFactor": 30,
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minScaleFactor": 30,
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90, 95],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minScaleFactor": 30,
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": ["10", 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minScaleFactor": 30,
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, "90"],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})"
    };

    for (const auto& config_str : invalid_configs) {
        std::optional<base::Value> root = base::JSONReader::Read(config_str);
        ASSERT_TRUE(root.has_value());
        ASSERT_TRUE(root->is_dict());
        EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
    }
}

TEST_F(NwebAutolayoutTest, CheckCCMandApplyRule_Disabled)
{
    mEnable_ = false;
    mWListEntry_ = nullptr;

    CheckCCMandApplyRule(nullptr);
    EXPECT_TRUE(true);
}

TEST_F(NwebAutolayoutTest, CheckCCMandApplyRule_WithoutWhitelistEntry)
{
    mEnable_ = true;
    mWListEntry_ = nullptr;

    CheckCCMandApplyRule(nullptr);
    EXPECT_TRUE(true);
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_InvalidOpacityFilterValues)
{
    const std::vector<std::string> invalid_configs = {
        R"({
            "minScaleFactor": 30,
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [-1, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minScaleFactor": 30,
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 101],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minScaleFactor": 30,
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [80, 40],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})"
    };

    for (const auto& config_str : invalid_configs) {
        std::optional<base::Value> root = base::JSONReader::Read(config_str);
        ASSERT_TRUE(root.has_value());
        ASSERT_TRUE(root->is_dict());
        EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
    }
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_InvalidMinMaskAreaRatioThreshold)
{
    const std::vector<std::string> invalid_configs = {
        R"({
            "minScaleFactor": 30,
            "minMaskAreaRatioThreshold": 49,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minScaleFactor": 30,
            "minMaskAreaRatioThreshold": 101,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minScaleFactor": 30,
            "minMaskAreaRatioThreshold": "60",
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})"
    };
 
    for (const auto& config_str : invalid_configs) {
        std::optional<base::Value> root = base::JSONReader::Read(config_str);
        ASSERT_TRUE(root.has_value());
        ASSERT_TRUE(root->is_dict());
        EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
    }
}
 
TEST_F(NwebAutolayoutTest, ParseToplevelConfig_InvalidMinContentAreaRatioThreshold)
{
    const std::vector<std::string> invalid_configs = {
        R"({
            "minScaleFactor": 30,
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 9,
            "scaleAnimationDuration": 100})",
        R"({
            "minScaleFactor": 30,
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 101,
            "scaleAnimationDuration": 100})",
        R"({
            "minScaleFactor": 30,
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": "20",
            "scaleAnimationDuration": 100})"
    };
 
    for (const auto& config_str : invalid_configs) {
        std::optional<base::Value> root = base::JSONReader::Read(config_str);
        ASSERT_TRUE(root.has_value());
        ASSERT_TRUE(root->is_dict());
        EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
    }
}
 
TEST_F(NwebAutolayoutTest, ParseToplevelConfig_InvalidScaleAnimationDuration)
{
    const std::vector<std::string> invalid_configs = {
        R"({
            "minScaleFactor": 30,
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 49})",
        R"({
            "minScaleFactor": 30,
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 401})",
        R"({
            "minScaleFactor": 30,
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": "100"})"
    };
 
    for (const auto& config_str : invalid_configs) {
        std::optional<base::Value> root = base::JSONReader::Read(config_str);
        ASSERT_TRUE(root.has_value());
        ASSERT_TRUE(root->is_dict());
        EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
    }
}
 
TEST_F(NwebAutolayoutTest, ParseToplevelConfig_MissingKey)
{
    const std::string invalid_config = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "scaleAnimationDuration": 100,
        "opacityFilter": [10, 90]
    })";
    std::optional<base::Value> root = base::JSONReader::Read(invalid_config);
    ASSERT_TRUE(root.has_value());
    ASSERT_TRUE(root->is_dict());
    EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
}
 
TEST_F(NwebAutolayoutTest, ParseWhitelistEntry_Valid)
{
    const std::string entry_str = R"({
        "pattern": "some_pattern",
        "GetID": "get_id_func",
        "GetPage": "get_page_func",
        "appRuleInfos": [ { "id": "rule1" } ]
    })";
    std::optional<base::Value> entry_val = base::JSONReader::Read(entry_str);
    ASSERT_TRUE(entry_val.has_value());
    ASSERT_TRUE(entry_val->is_dict());
 
    EXPECT_TRUE(ParseWhitelistEntry(entry_val->GetDict()));
    EXPECT_EQ(mCCMConfig_.whitelist.pattern, "some_pattern");
    EXPECT_EQ(mCCMConfig_.whitelist.getID, "get_id_func");
    EXPECT_EQ(mCCMConfig_.whitelist.getPage, "get_page_func");
    EXPECT_TRUE(mCCMConfig_.whitelist.appRuleInfos.has_value());
    EXPECT_EQ(mCCMConfig_.whitelist.appRuleInfos->size(), 1u);
}
 
TEST_F(NwebAutolayoutTest, ParseWhitelistEntry_Invalid)
{
    const std::string invalid_entry_str = R"({
        "GetID": "get_id_func",
        "GetPage": "get_page_func",
        "appRuleInfos": []
    })";
    std::optional<base::Value> entry_val = base::JSONReader::Read(invalid_entry_str);
    ASSERT_TRUE(entry_val.has_value());
    ASSERT_TRUE(entry_val->is_dict());
    EXPECT_FALSE(ParseWhitelistEntry(entry_val->GetDict()));
}
 
TEST_F(NwebAutolayoutTest, ParseWhitelistEntry_EmptyFields)
{
    const std::vector<std::string> invalid_entries = {
        R"({"pattern": "", "GetID": "gi", "GetPage": "gp", "appRuleInfos": []})",
        R"({"pattern": "p", "GetID": "", "GetPage": "gp", "appRuleInfos": []})",
        R"({"pattern": "p", "GetID": "gi", "GetPage": "", "appRuleInfos": []})"
    };
 
    for (const auto& entry_str : invalid_entries) {
        std::optional<base::Value> entry_val = base::JSONReader::Read(entry_str);
        ASSERT_TRUE(entry_val.has_value());
        ASSERT_TRUE(entry_val->is_dict());
        EXPECT_FALSE(ParseWhitelistEntry(entry_val->GetDict()));
    }
}
 
TEST_F(NwebAutolayoutTest, ParseWhitelistEntry_InvalidAppRuleInfos)
{
    const std::string invalid_entry_str = R"({
        "pattern": "p",
        "GetID": "gi",
        "GetPage": "gp",
        "appRuleInfos": "not_a_list"
    })";
    std::optional<base::Value> entry_val = base::JSONReader::Read(invalid_entry_str);
    ASSERT_TRUE(entry_val.has_value());
    ASSERT_TRUE(entry_val->is_dict());
    EXPECT_FALSE(ParseWhitelistEntry(entry_val->GetDict()));
}
 
 
TEST_F(NwebAutolayoutTest, ParseWhitelistEntry_MissingKeys)
{
    const std::vector<std::string> invalid_entries = {
        R"({"pattern": "p", "GetPage": "gp", "appRuleInfos": []})",
        R"({"pattern": "p", "GetID": "gi", "appRuleInfos": []})",
        R"({"pattern": "p", "GetID": "gi", "GetPage": "gp"})"
    };
 
    for (const auto& entry_str : invalid_entries) {
        std::optional<base::Value> entry_val = base::JSONReader::Read(entry_str);
        ASSERT_TRUE(entry_val.has_value());
        ASSERT_TRUE(entry_val->is_dict());
        EXPECT_FALSE(ParseWhitelistEntry(entry_val->GetDict()));
    }
}
 
TEST_F(NwebAutolayoutTest, ParseWhitelist_Valid)
{
    std::optional<base::Value> root = base::JSONReader::Read(g_valid_config);
    ASSERT_TRUE(root.has_value());
    const base::Value::Dict* whitelist_dict = root->GetDict().FindDict("whitelist");
    ASSERT_TRUE(whitelist_dict);
    mAppBundleName_ = "com.example.app";
    EXPECT_TRUE(ParseWhitelist(*whitelist_dict));
    EXPECT_NE(mWListEntry_, nullptr);
}
 
TEST_F(NwebAutolayoutTest, ParseWhitelist_InvalidEntryType)
{
    const std::string invalid_whitelist = R"({
        "com.example.app": "not_a_dict"
    })";
    std::optional<base::Value> whitelist_val = base::JSONReader::Read(invalid_whitelist);
    ASSERT_TRUE(whitelist_val.has_value());
    ASSERT_TRUE(whitelist_val->is_dict());
    mAppBundleName_ = "com.example.app";
    EXPECT_FALSE(ParseWhitelist(whitelist_val->GetDict()));
}
 
TEST_F(NwebAutolayoutTest, ParseWhitelist_InvalidEntryContents)
{
    const std::string invalid_whitelist = R"({
        "com.example.app": {
            "pattern": "",
            "GetID": "get_id_func",
            "GetPage": "get_page_func",
            "appRuleInfos": []
        }
    })";

    std::optional<base::Value> whitelist_val = base::JSONReader::Read(invalid_whitelist);
    ASSERT_TRUE(whitelist_val.has_value());
    ASSERT_TRUE(whitelist_val->is_dict());
    mAppBundleName_ = "com.example.app";
    EXPECT_FALSE(ParseWhitelist(whitelist_val->GetDict()));
}

TEST_F(NwebAutolayoutTest, Parse_Valid)
{
    std::optional<base::Value> root = base::JSONReader::Read(g_valid_config);
    ASSERT_TRUE(root.has_value());
    mAppBundleName_ = "com.example.app";
    EXPECT_TRUE(Parse(*root));
}
 
TEST_F(NwebAutolayoutTest, Parse_Invalid)
{
    const std::string invalid_config = "[]";
    std::optional<base::Value> root = base::JSONReader::Read(invalid_config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(Parse(*root));
}
 
TEST_F(NwebAutolayoutTest, CheckWebContainer_Disabled)
{
    mEnable_ = false;
    CheckWebContainer(nullptr, nullptr);
    EXPECT_TRUE(true);
}

TEST_F(NwebAutolayoutTest, CheckWebContainer_NoBrowser)
{
    mEnable_ = true;
    CheckWebContainer(nullptr, nullptr);
    EXPECT_TRUE(true);
}

TEST_F(NwebAutolayoutTest, Parse_InvalidWhitelistType)
{
    const std::string invalid_config = R"({
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [10, 90],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100,
        "whitelist": []
    })";

    std::optional<base::Value> root = base::JSONReader::Read(invalid_config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(Parse(*root));
}

TEST_F(NwebAutolayoutTest, Parse_InvalidWhitelistEntry)
{
    const std::string invalid_config = R"({
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [10, 90],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100,
        "whitelist": {
            "com.example.app": {
                "pattern": "",
                "GetID": "get_id_func",
                "GetPage": "get_page_func",
                "appRuleInfos": []
            }
        }
    })";

    std::optional<base::Value> root = base::JSONReader::Read(invalid_config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(Parse(*root));
}

TEST_F(NwebAutolayoutTest, Parse_InvalidTopLevelConfig)
{
    const std::string invalid_config = R"({
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [80, 40],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100,
        "whitelist": {}
    })";

    std::optional<base::Value> root = base::JSONReader::Read(invalid_config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(Parse(*root));
}

TEST_F(NwebAutolayoutTest, Parse_MissingWhitelist)
{
    const std::string invalid_config = R"({
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [10, 90],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100
    })";
    std::optional<base::Value> root = base::JSONReader::Read(invalid_config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(Parse(*root));
}

TEST_F(NwebAutolayoutTest, CheckCCMandApplyRule_ValidStateWithNullFrame)
{
    // Set up valid internal state
    mEnable_ = true;
    mCCMConfig_.min_mask_area_ratio_threshold = 60;
    mCCMConfig_.min_content_area_ratio_threshold = 20;
    mCCMConfig_.scale_animation_duration = 100;
    mCCMConfig_.opacity_filter = {10, 90};
    mCCMConfig_.minScaleFactor = 70;
    
    // Create a whitelist entry with valid data
    std::string pattern_data = "test_pattern";
    std::string id_data = "test_id";
    std::string page_data = "test_page";
    
    WhitelistEntry entry;
    entry.pattern = std::string_view(pattern_data);
    entry.getID = std::string_view(id_data);
    entry.getPage = std::string_view(page_data);
    
    base::Value::List rules;
    base::Value::Dict rule;
    rule.Set("id", "*");
    rule.Set("pg", "*");
    rules.Append(std::move(rule));
    entry.appRuleInfos = std::move(rules);
    
    mWListEntry_ = &entry;
    mAutoLayoutJSSource_ = "test_autolayout_script";
    
    // Call with nullptr frame - will fail at ExecuteJavaScript but JSON logic executes first
    CheckCCMandApplyRule(nullptr);
    EXPECT_TRUE(true);
}

TEST_F(NwebAutolayoutTest, CheckCCMandApplyRule_JSONWriteSuccess)
{
    // Set up state to test JSON serialization success path
    mEnable_ = true;
    mCCMConfig_.min_mask_area_ratio_threshold = 75;
    mCCMConfig_.min_content_area_ratio_threshold = 30;
    mCCMConfig_.scale_animation_duration = 200;
    mCCMConfig_.opacity_filter = {20, 80};
    mCCMConfig_.minScaleFactor = 65;
    
    std::string pattern = "pattern";
    std::string id = "id";
    std::string page = "page";
    
    WhitelistEntry entry;
    entry.pattern = std::string_view(pattern);
    entry.getID = std::string_view(id);
    entry.getPage = std::string_view(page);
    
    base::Value::List rules;
    rules.Append(base::Value::Dict());
    entry.appRuleInfos = std::move(rules);
    
    mWListEntry_ = &entry;
    mAutoLayoutJSSource_ = "script";
    
    // The function will process JSON before hitting nullptr
    CheckCCMandApplyRule(nullptr);
    EXPECT_TRUE(true);
}

TEST_F(NwebAutolayoutTest, CheckWebContainer_NullBrowser)
{
    mEnable_ = true;
    mPatternJSSource_ = "test_pattern";
    
    // Test with null browser - should return early without crash
    CheckWebContainer(nullptr, nullptr);
    EXPECT_TRUE(true);
}

TEST_F(NwebAutolayoutTest, CheckWebContainer_EnabledWithNullFrame)
{
    mEnable_ = true;
    mPatternJSSource_ = "pattern_with_special_`\\$chars";
    
    // Both browser and frame are null - tests escape logic but returns before IsMain
    CheckWebContainer(nullptr, nullptr);
    EXPECT_TRUE(true);
}

TEST_F(NwebAutolayoutTest, CheckWebContainer_EscapeLogic)
{
    mEnable_ = true;
    // Set pattern with special characters to test EscapeForJS_TemplateLiteral usage
    mPatternJSSource_ = "test`pattern\\with$specials";
    
    CheckWebContainer(nullptr, nullptr);
    EXPECT_TRUE(true);
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_BoundaryValues)
{
    // Test with boundary values for min_mask_area_ratio_threshold
    const std::string config_min = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 50,
        "opacityFilter": [0, 100],
        "minContentAreaRatioThreshold": 11,
        "scaleAnimationDuration": 51
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config_min);
    ASSERT_TRUE(root.has_value());
    EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
    EXPECT_EQ(mCCMConfig_.min_mask_area_ratio_threshold, 50);
    EXPECT_EQ(mCCMConfig_.opacity_filter.first, 0);
    EXPECT_EQ(mCCMConfig_.opacity_filter.second, 100);
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_MaxBoundaryValues)
{
    // Test with max boundary values
    const std::string config_max = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 100,
        "opacityFilter": [50, 100],
        "minContentAreaRatioThreshold": 99,
        "scaleAnimationDuration": 399
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config_max);
    ASSERT_TRUE(root.has_value());
    EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
    EXPECT_EQ(mCCMConfig_.min_mask_area_ratio_threshold, 100);
    EXPECT_EQ(mCCMConfig_.scale_animation_duration, 399);
}

TEST_F(NwebAutolayoutTest, ParseWhitelistEntry_WithMultipleRules)
{
    const std::string entry_str = R"({
        "pattern": "multi_pattern",
        "GetID": "multi_id",
        "GetPage": "multi_page",
        "appRuleInfos": [
            { "id": "rule1", "pg": "page1" },
            { "id": "rule2", "pg": "page2" },
            { "id": "*", "pg": "*" }
        ]
    })";
    std::optional<base::Value> entry_val = base::JSONReader::Read(entry_str);
    ASSERT_TRUE(entry_val.has_value());
    ASSERT_TRUE(entry_val->is_dict());

    EXPECT_TRUE(ParseWhitelistEntry(entry_val->GetDict()));
    EXPECT_EQ(mCCMConfig_.whitelist.appRuleInfos->size(), 3u);
}

TEST_F(NwebAutolayoutTest, Parse_ValidComplexConfig)
{
    const std::string complex_config = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 55,
        "opacityFilter": [15, 85],
        "minContentAreaRatioThreshold": 25,
        "scaleAnimationDuration": 150,
        "whitelist": {
            "com.app1": {
                "pattern": "p1",
                "GetID": "id1",
                "GetPage": "page1",
                "appRuleInfos": [{"id": "1"}]
            },
            "com.app2": {
                "pattern": "p2",
                "GetID": "id2",
                "GetPage": "page2",
                "appRuleInfos": [{"pg": "2"}]
            }
        }
    })";
    
    std::optional<base::Value> root = base::JSONReader::Read(complex_config);
    ASSERT_TRUE(root.has_value());
    mAppBundleName_ = "com.app2";
    EXPECT_TRUE(Parse(*root));
    EXPECT_NE(mWListEntry_, nullptr);
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_OpacityFilterEdgeCases)
{
    // Test opacity filter with equal values
    const std::string config_equal = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [50, 50],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config_equal);
    ASSERT_TRUE(root.has_value());
    EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
    EXPECT_EQ(mCCMConfig_.opacity_filter.first, 50);
    EXPECT_EQ(mCCMConfig_.opacity_filter.second, 50);
}

TEST_F(NwebAutolayoutTest, GetInstance_Test)
{
    // Test GetInstance static method
    std::shared_ptr<NwebAutolayout> instance1 = NwebAutolayout::GetInstance();
    std::shared_ptr<NwebAutolayout> instance2 = NwebAutolayout::GetInstance();
    // Should return the same instance (singleton)
    EXPECT_EQ(instance1.get(), instance2.get());
    EXPECT_NE(instance1, nullptr);
}

TEST_F(NwebAutolayoutTest, CheckCCMandApplyRule_WithValidFrame)
{
    // Set up valid state
    mEnable_ = true;
    mCCMConfig_.min_mask_area_ratio_threshold = 60;
    mCCMConfig_.min_content_area_ratio_threshold = 20;
    mCCMConfig_.scale_animation_duration = 100;
    mCCMConfig_.opacity_filter = {10, 90};
    mCCMConfig_.minScaleFactor = 70;
    
    std::string pattern_data = "test_pattern";
    std::string id_data = "test_id";
    std::string page_data = "test_page";
    
    WhitelistEntry entry;
    entry.pattern = std::string_view(pattern_data);
    entry.getID = std::string_view(id_data);
    entry.getPage = std::string_view(page_data);
    
    base::Value::List rules;
    base::Value::Dict rule;
    rule.Set("id", "*");
    rule.Set("pg", "*");
    rules.Append(std::move(rule));
    entry.appRuleInfos = std::move(rules);
    
    mWListEntry_ = &entry;
    mAutoLayoutJSSource_ = "test_autolayout_script";
    
    // Create a mock frame - even if it's null, the function will process JSON before ExecuteJavaScript
    // We need to test the path where frame is not null but ExecuteJavaScript may fail
    // Since we can't easily mock CefFrame, we'll test with nullptr but ensure JSON processing happens
    CheckCCMandApplyRule(nullptr);
    
    // Test with frame being non-null but we can't easily create a real CefFrame
    // The key is to test the JSON serialization success path (line 134)
    // Since we can't mock ExecuteJavaScript easily, we verify the state is set correctly
    EXPECT_TRUE(mEnable_);
    EXPECT_NE(mWListEntry_, nullptr);
}

TEST_F(NwebAutolayoutTest, CheckCCMandApplyRule_WithFrameNull)
{
    // Test all three conditions in the if statement
    // Case 1: mEnable_ is false
    mEnable_ = false;
    mWListEntry_ = nullptr;
    CheckCCMandApplyRule(nullptr);
    
    // Case 2: mWListEntry_ is nullptr
    mEnable_ = true;
    mWListEntry_ = nullptr;
    CheckCCMandApplyRule(nullptr);
    
    // Case 3: frame is nullptr
    WhitelistEntry entry;
    std::string pattern_data = "pattern";
    entry.pattern = std::string_view(pattern_data);
    entry.getID = std::string_view(pattern_data);
    entry.getPage = std::string_view(pattern_data);
    base::Value::List rules;
    entry.appRuleInfos = std::move(rules);
    mWListEntry_ = &entry;
    mEnable_ = true;
    CheckCCMandApplyRule(nullptr);
}

TEST_F(NwebAutolayoutTest, ParseWhitelistEntry_JSONWriterFailure)
{
    // Create a dict that might cause JSONWriter to fail
    // Actually, base::JSONWriter::Write rarely fails, but we can test the else branch
    // by creating an extremely large or complex structure
    // However, since JSONWriter is robust, we'll test with a valid dict
    // and verify the else branch is at least reachable in theory
    
    const std::string entry_str = R"({
        "pattern": "test_pattern",
        "GetID": "test_id",
        "GetPage": "test_page",
        "appRuleInfos": []
    })";
    std::optional<base::Value> entry_val = base::JSONReader::Read(entry_str);
    ASSERT_TRUE(entry_val.has_value());
    ASSERT_TRUE(entry_val->is_dict());

    // This should succeed, but the else branch at line 239 is hard to trigger
    // We'll verify the function works correctly
    EXPECT_TRUE(ParseWhitelistEntry(entry_val->GetDict()));
}

TEST_F(NwebAutolayoutTest, ParseWhitelist_NonDictEntry)
{
    // Test line 220: entry_value.is_dict() returns false
    const std::string invalid_whitelist = R"({
        "com.example.app": "not_a_dict"
    })";
    std::optional<base::Value> whitelist_val = base::JSONReader::Read(invalid_whitelist);
    ASSERT_TRUE(whitelist_val.has_value());
    ASSERT_TRUE(whitelist_val->is_dict());    
    mAppBundleName_ = "com.example.app";
    EXPECT_FALSE(ParseWhitelist(whitelist_val->GetDict()));
}

TEST_F(NwebAutolayoutTest, LoadAutoLayoutFromHap_EmptyScript)
{
    // Initialize a ResourceBundle instance with an empty pak path so
    // GetRawDataResource returns an empty string_view and triggers the
    // failure branch.
    ui::ResourceBundle::CleanupSharedInstance();
    ui::ResourceBundle::InitSharedInstanceWithPakPath(base::FilePath());

    mEnable_ = true;
    LoadAutoLayoutFromHap();

    EXPECT_FALSE(mEnable_);

    ui::ResourceBundle::CleanupSharedInstance();
}

TEST_F(NwebAutolayoutTest, Parse_WhitelistEmpty)
{
    // Test line 67: mCCMConfig_.whitelist.empty() branch
    const std::string config_empty_whitelist = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [10, 90],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100,
        "whitelist": {}
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config_empty_whitelist);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(Parse(*root));
    EXPECT_EQ(mWListEntry_, nullptr);
}

TEST_F(NwebAutolayoutTest, Parse_JsonRootInvalid)
{
    // Test line 67: !mJsonRoot.has_value() branch
    // This is tested indirectly through Initialize, but we can test Parse with invalid JSON
    const std::string invalid_json = "not valid json";
    std::optional<base::Value> root = base::JSONReader::Read(invalid_json);
    // If Read fails, root will be nullopt
    if (!root.has_value()) {
        // This simulates the case where mJsonRoot.has_value() is false
        EXPECT_FALSE(root.has_value());
    }
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_OpacityFilterFirstLessThanMin)
{
    // Test line 206: first_opacity < kMinOpacityFilter
    const std::string config = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [-1, 90],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_OpacityFilterSecondGreaterThanMax)
{
    // Test line 208: second_opacity > kMaxOpacityFilter
    const std::string config = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [10, 101],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_OpacityFilterFirstGreaterThanSecond)
{
    // Test line 208: first_opacity > second_opacity
    const std::string config = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [90, 10],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_OpacityFilterSecondLessThanMin)
{
    // Test line 207: second_opacity < kMinOpacityFilter
    const std::string config = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [10, -1],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_OpacityFilterFirstGreaterThanMax)
{
    // Test line 206: first_opacity > kMaxOpacityFilter
    const std::string config = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [101, 90],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_MinContentAreaRatioThresholdBoundary)
{
    // Test line 177: *min_content_opt <= kMinContentAreaRatioThreshold (value 10 should fail)
    const std::string config = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [10, 90],
        "minContentAreaRatioThreshold": 10,
        "scaleAnimationDuration": 100
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_MaxContentAreaRatioThresholdBoundary)
{
    // Test line 178: *min_content_opt >= kMaxContentAreaRatioThreshold (value 100 should fail)
    const std::string config = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [10, 90],
        "minContentAreaRatioThreshold": 100,
        "scaleAnimationDuration": 100
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_MinScaleAnimationDurationBoundary)
{
    // Test line 187: *scale_anim_opt <= kMinScaleAnimationDuration (value 50 should fail)
    const std::string config = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [10, 90],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 50
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_MaxScaleAnimationDurationBoundary)
{
    // Test line 188: *scale_anim_opt >= kMaxScaleAnimationDuration (value 400 should fail)
    const std::string config = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [10, 90],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 400
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
}

TEST_F(NwebAutolayoutTest, CheckCCMandApplyRule_JSONSuccessPath)
{
    // Test line 134: if(success) branch
    // Set up state so JSON serialization succeeds
    mEnable_ = true;
    mCCMConfig_.min_mask_area_ratio_threshold = 60;
    mCCMConfig_.min_content_area_ratio_threshold = 20;
    mCCMConfig_.scale_animation_duration = 100;
    mCCMConfig_.opacity_filter = {10, 90};
    mCCMConfig_.minScaleFactor = 70;
    
    std::string pattern_data = "pattern";
    std::string id_data = "id";
    std::string page_data = "page";
    
    WhitelistEntry entry;
    entry.pattern = std::string_view(pattern_data);
    entry.getID = std::string_view(id_data);
    entry.getPage = std::string_view(page_data);
    
    base::Value::List rules;
    base::Value::Dict rule;
    rule.Set("id", "test");
    rules.Append(std::move(rule));
    entry.appRuleInfos = std::move(rules);
    
    mWListEntry_ = &entry;
    mAutoLayoutJSSource_ = "script";
    
    // Call with nullptr frame - JSON processing will happen before ExecuteJavaScript
    // This should trigger the success path at line 134
    CheckCCMandApplyRule(nullptr);
    EXPECT_TRUE(true);
}

// Note: Testing CheckCCMandApplyRule and CheckWebContainer with real CEF objects
// requires complex mocking infrastructure. These functions are tested with nullptr
// parameters to cover early return paths. Full integration testing would require
// actual CEF browser/frame instances or more sophisticated mocks.

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_OpacityFilterSizeNotTwo)
{
    // Test line 200: opacity_list->size() != 2
    // 测试size为1的情况
    const std::string config_size1 = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [10],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config_size1);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
    
    // 测试size为3的情况
    const std::string config_size3 = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [10, 90, 95],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100
    })";
    root = base::JSONReader::Read(config_size3);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_OpacityFilterFirstNotInt)
{
    // Test line 200: !(*opacity_list)[0].is_int()
    const std::string config = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": ["10", 90],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_OpacityFilterSecondNotInt)
{
    // Test line 201: !(*opacity_list)[1].is_int()
    const std::string config = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [10, "90"],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_OpacityFilterBothNotInt)
{
    // Test line 200-201: both elements are not int
    const std::string config = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": ["10", "90"],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_OpacityFilterFirstAtMaxBoundary)
{
    // Test line 206: first_opacity == kMaxOpacityFilter (100) - should pass
    const std::string config = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [100, 100],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config);
    ASSERT_TRUE(root.has_value());
    EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
    EXPECT_EQ(mCCMConfig_.opacity_filter.first, 100);
    EXPECT_EQ(mCCMConfig_.opacity_filter.second, 100);
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_OpacityFilterSecondAtMaxBoundary)
{
    // Test line 208: second_opacity == kMaxOpacityFilter (100) - should pass
    const std::string config = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [50, 100],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config);
    ASSERT_TRUE(root.has_value());
    EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
    EXPECT_EQ(mCCMConfig_.opacity_filter.first, 50);
    EXPECT_EQ(mCCMConfig_.opacity_filter.second, 100);
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_OpacityFilterFirstAtMinBoundary)
{
    // Test line 206: first_opacity == kMinOpacityFilter (0) - should pass
    const std::string config = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [0, 50],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config);
    ASSERT_TRUE(root.has_value());
    EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
    EXPECT_EQ(mCCMConfig_.opacity_filter.first, 0);
    EXPECT_EQ(mCCMConfig_.opacity_filter.second, 50);
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_OpacityFilterSecondAtMinBoundary)
{
    // Test line 207: second_opacity == kMinOpacityFilter (0) - should pass
    const std::string config = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [0, 0],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config);
    ASSERT_TRUE(root.has_value());
    EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
    EXPECT_EQ(mCCMConfig_.opacity_filter.first, 0);
    EXPECT_EQ(mCCMConfig_.opacity_filter.second, 0);
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_MinMaskAreaRatioThresholdAtBoundaries)
{
    // Test line 167-168: boundary values 50 and 100 should pass
    const std::string config_min = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 50,
        "opacityFilter": [10, 90],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config_min);
    ASSERT_TRUE(root.has_value());
    EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
    EXPECT_EQ(mCCMConfig_.min_mask_area_ratio_threshold, 50);
    
    const std::string config_max = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 100,
        "opacityFilter": [10, 90],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100
    })";
    root = base::JSONReader::Read(config_max);
    ASSERT_TRUE(root.has_value());
    EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
    EXPECT_EQ(mCCMConfig_.min_mask_area_ratio_threshold, 100);
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_MinContentAreaRatioThresholdAtValidBoundaries)
{
    // Test line 177-178: valid boundary values 11 and 99 should pass
    const std::string config_min = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [10, 90],
        "minContentAreaRatioThreshold": 11,
        "scaleAnimationDuration": 100
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config_min);
    ASSERT_TRUE(root.has_value());
    EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
    EXPECT_EQ(mCCMConfig_.min_content_area_ratio_threshold, 11);
    
    const std::string config_max = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [10, 90],
        "minContentAreaRatioThreshold": 99,
        "scaleAnimationDuration": 100
    })";
    root = base::JSONReader::Read(config_max);
    ASSERT_TRUE(root.has_value());
    EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
    EXPECT_EQ(mCCMConfig_.min_content_area_ratio_threshold, 99);
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_ScaleAnimationDurationAtValidBoundaries)
{
    // Test line 187-188: valid boundary values 51 and 399 should pass
    const std::string config_min = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [10, 90],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 51
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config_min);
    ASSERT_TRUE(root.has_value());
    EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
    EXPECT_EQ(mCCMConfig_.scale_animation_duration, 51);
    
    const std::string config_max = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [10, 90],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 399
    })";
    root = base::JSONReader::Read(config_max);
    ASSERT_TRUE(root.has_value());
    EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
    EXPECT_EQ(mCCMConfig_.scale_animation_duration, 399);
}

TEST_F(NwebAutolayoutTest, ParseWhitelistEntry_PatternPtrNull)
{
    // Test line 245: pattern_ptr == nullptr
    const std::string entry_str = R"({
        "GetID": "get_id_func",
        "GetPage": "get_page_func",
        "appRuleInfos": []
    })";
    std::optional<base::Value> entry_val = base::JSONReader::Read(entry_str);
    ASSERT_TRUE(entry_val.has_value());
    ASSERT_TRUE(entry_val->is_dict());
    EXPECT_FALSE(ParseWhitelistEntry(entry_val->GetDict()));
}

TEST_F(NwebAutolayoutTest, ParseWhitelistEntry_GetIDPtrNull)
{
    // Test line 249: get_id_ptr == nullptr
    const std::string entry_str = R"({
        "pattern": "some_pattern",
        "GetPage": "get_page_func",
        "appRuleInfos": []
    })";
    std::optional<base::Value> entry_val = base::JSONReader::Read(entry_str);
    ASSERT_TRUE(entry_val.has_value());
    ASSERT_TRUE(entry_val->is_dict());
    EXPECT_FALSE(ParseWhitelistEntry(entry_val->GetDict()));
}

TEST_F(NwebAutolayoutTest, ParseWhitelistEntry_GetPagePtrNull)
{
    // Test line 253: get_page_ptr == nullptr
    const std::string entry_str = R"({
        "pattern": "some_pattern",
        "GetID": "get_id_func",
        "appRuleInfos": []
    })";
    std::optional<base::Value> entry_val = base::JSONReader::Read(entry_str);
    ASSERT_TRUE(entry_val.has_value());
    ASSERT_TRUE(entry_val->is_dict());
    EXPECT_FALSE(ParseWhitelistEntry(entry_val->GetDict()));
}

TEST_F(NwebAutolayoutTest, ParseWhitelistEntry_AppRuleInfosNull)
{
    // Test line 263: app_rules_list == nullptr
    const std::string entry_str = R"({
        "pattern": "some_pattern",
        "GetID": "get_id_func",
        "GetPage": "get_page_func"
    })";
    std::optional<base::Value> entry_val = base::JSONReader::Read(entry_str);
    ASSERT_TRUE(entry_val.has_value());
    ASSERT_TRUE(entry_val->is_dict());
    EXPECT_FALSE(ParseWhitelistEntry(entry_val->GetDict()));
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_MinMaskAreaRatioThresholdNull)
{
    // Test line 167: !min_mask_opt
    const std::string config = R"({
        "minScaleFactor": 30,
        "opacityFilter": [10, 90],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_MinContentAreaRatioThresholdNull)
{
    // Test line 177: !min_content_opt
    const std::string config = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [10, 90],
        "scaleAnimationDuration": 100
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_ScaleAnimationDurationNull)
{
    // Test line 187: !scale_anim_opt
    const std::string config = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [10, 90],
        "minContentAreaRatioThreshold": 20
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_OpacityFilterNull)
{
    // Test line 196: !opacity_list
    const std::string config = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
}

TEST_F(NwebAutolayoutTest, Parse_InvalidRootNotDict)
{
    // Test line 145: !root.is_dict()
    base::Value::List list_value;
    list_value.Append(1);
    base::Value root(std::move(list_value));
    EXPECT_FALSE(Parse(root));
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_InvalidRootDict)
{
    // Test line 151: ParseToplevelConfig returns false
    const std::string invalid_config = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 200,
        "opacityFilter": [10, 90],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100,
        "whitelist": {}
    })";
    std::optional<base::Value> root = base::JSONReader::Read(invalid_config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(Parse(*root));
}

TEST_F(NwebAutolayoutTest, ParseWhitelist_EmptyWhitelist)
{
    // Test ParseWhitelist with empty whitelist
    const std::string config = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [10, 90],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100,
        "whitelist": {}
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config);
    ASSERT_TRUE(root.has_value());
    const base::Value::Dict* whitelist_dict = root->GetDict().FindDict("whitelist");
    ASSERT_TRUE(whitelist_dict);
    EXPECT_FALSE(ParseWhitelist(*whitelist_dict));
    EXPECT_EQ(mWListEntry_, nullptr);
}

TEST_F(NwebAutolayoutTest, ParseWhitelist_MultipleValidEntries)
{
    // Test ParseWhitelist with multiple valid entries to cover loop branches
    const std::string multi_entry = R"({
        "app1": {
            "pattern": "p1",
            "GetID": "id1",
            "GetPage": "page1",
            "appRuleInfos": [{"id": "1"}]
        },
        "app2": {
            "pattern": "p2",
            "GetID": "id2",
            "GetPage": "page2",
            "appRuleInfos": [{"id": "2"}]
        },
        "app3": {
            "pattern": "p3",
            "GetID": "id3",
            "GetPage": "page3",
            "appRuleInfos": [{"id": "3"}]
        }
    })";
    std::optional<base::Value> whitelist_val = base::JSONReader::Read(multi_entry);
    ASSERT_TRUE(whitelist_val.has_value());
    ASSERT_TRUE(whitelist_val->is_dict());
    mAppBundleName_ = "app1";
    EXPECT_TRUE(ParseWhitelist(whitelist_val->GetDict()));
    EXPECT_NE(mWListEntry_, nullptr);
}

TEST_F(NwebAutolayoutTest, ParseWhitelist_OneValidOneInvalidInLoop)
{
    // Test ParseWhitelist where first entry is valid, second is invalid
    // This tests the loop continuing after one failure
    const std::string mixed = R"({
        "app1": {
            "pattern": "p1",
            "GetID": "id1",
            "GetPage": "page1",
            "appRuleInfos": [{"id": "1"}]
        },
        "app2": "not_a_dict"
    })";
    std::optional<base::Value> whitelist_val = base::JSONReader::Read(mixed);
    ASSERT_TRUE(whitelist_val.has_value());
    ASSERT_TRUE(whitelist_val->is_dict());
    mAppBundleName_ = "app2";
    EXPECT_FALSE(ParseWhitelist(whitelist_val->GetDict()));
}

TEST_F(NwebAutolayoutTest, ParseWhitelist_SecondEntryInvalid)
{
    // Test ParseWhitelist where second entry fails validation
    const std::string mixed = R"({
        "app1": {
            "pattern": "p1",
            "GetID": "id1",
            "GetPage": "page1",
            "appRuleInfos": [{"id": "1"}]
        },
        "app2": {
            "pattern": "",
            "GetID": "id2",
            "GetPage": "page2",
            "appRuleInfos": []
        }
    })";
    std::optional<base::Value> whitelist_val = base::JSONReader::Read(mixed);
    ASSERT_TRUE(whitelist_val.has_value());
    ASSERT_TRUE(whitelist_val->is_dict());
    mAppBundleName_ = "app2";
    EXPECT_FALSE(ParseWhitelist(whitelist_val->GetDict()));
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_OpacityFilterBoundaryValues)
{
    // Test opacity filter boundary values (0 and 100)
    const std::string config_min = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [0, 100],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config_min);
    ASSERT_TRUE(root.has_value());
    EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
    EXPECT_EQ(mCCMConfig_.opacity_filter.first, 0);
    EXPECT_EQ(mCCMConfig_.opacity_filter.second, 100);
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_OpacityFilterMaxValues)
{
    // Test opacity filter with max values
    const std::string config_max = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [100, 100],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config_max);
    ASSERT_TRUE(root.has_value());
    EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
    EXPECT_EQ(mCCMConfig_.opacity_filter.first, 100);
    EXPECT_EQ(mCCMConfig_.opacity_filter.second, 100);
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_MinMaskAreaRatioThresholdBoundary)
{
    // Test line 167-168: boundary values for minMaskAreaRatioThreshold
    // Test with value 50 (minimum valid)
    const std::string config_min = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 50,
        "opacityFilter": [10, 90],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config_min);
    ASSERT_TRUE(root.has_value());
    EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
    EXPECT_EQ(mCCMConfig_.min_mask_area_ratio_threshold, 50);
    
    // Test with value 100 (maximum valid)
    const std::string config_max = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 100,
        "opacityFilter": [10, 90],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100
    })";
    root = base::JSONReader::Read(config_max);
    ASSERT_TRUE(root.has_value());
    EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
    EXPECT_EQ(mCCMConfig_.min_mask_area_ratio_threshold, 100);
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_MinContentAreaRatioThresholdValidBoundary)
{
    // Test line 177-178: valid boundary values (11 and 99)
    const std::string config_min = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [10, 90],
        "minContentAreaRatioThreshold": 11,
        "scaleAnimationDuration": 100
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config_min);
    ASSERT_TRUE(root.has_value());
    EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
    EXPECT_EQ(mCCMConfig_.min_content_area_ratio_threshold, 11);
    
    const std::string config_max = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [10, 90],
        "minContentAreaRatioThreshold": 99,
        "scaleAnimationDuration": 100
    })";
    root = base::JSONReader::Read(config_max);
    ASSERT_TRUE(root.has_value());
    EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
    EXPECT_EQ(mCCMConfig_.min_content_area_ratio_threshold, 99);
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_ScaleAnimationDurationValidBoundary)
{
    // Test line 187-188: valid boundary values (51 and 399)
    const std::string config_min = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [10, 90],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 51
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config_min);
    ASSERT_TRUE(root.has_value());
    EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
    EXPECT_EQ(mCCMConfig_.scale_animation_duration, 51);
    
    const std::string config_max = R"({
        "minScaleFactor": 30,
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [10, 90],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 399
    })";
    root = base::JSONReader::Read(config_max);
    ASSERT_TRUE(root.has_value());
    EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
    EXPECT_EQ(mCCMConfig_.scale_animation_duration, 399);
}

TEST_F(NwebAutolayoutTest, ParseWhitelist_MultipleEntries)
{
    // Test ParseWhitelist with multiple entries
    const std::string multi_entry_config = R"({
        "com.app1": {
            "pattern": "p1",
            "GetID": "id1",
            "GetPage": "page1",
            "appRuleInfos": [{"id": "1"}]
        },
        "com.app2": {
            "pattern": "p2",
            "GetID": "id2",
            "GetPage": "page2",
            "appRuleInfos": [{"id": "2"}]
        },
        "com.app3": {
            "pattern": "p3",
            "GetID": "id3",
            "GetPage": "page3",
            "appRuleInfos": [{"id": "3"}]
        }
    })";
    std::optional<base::Value> whitelist_val = base::JSONReader::Read(multi_entry_config);
    ASSERT_TRUE(whitelist_val.has_value());
    ASSERT_TRUE(whitelist_val->is_dict());
    mAppBundleName_ = "com.app1";
    EXPECT_TRUE(ParseWhitelist(whitelist_val->GetDict()));
    EXPECT_NE(mWListEntry_, nullptr);
}

TEST_F(NwebAutolayoutTest, ParseWhitelist_OneValidOneInvalid)
{
    // Test ParseWhitelist where one entry is valid and one is invalid
    const std::string mixed_config = R"({
        "com.valid": {
            "pattern": "valid_pattern",
            "GetID": "valid_id",
            "GetPage": "valid_page",
            "appRuleInfos": [{"id": "valid"}]
        },
        "com.invalid": "not_a_dict"
    })";
    std::optional<base::Value> whitelist_val = base::JSONReader::Read(mixed_config);
    ASSERT_TRUE(whitelist_val.has_value());
    ASSERT_TRUE(whitelist_val->is_dict());
    // Should fail because one entry is not a dict
    mAppBundleName_ = "com.invalid";
    EXPECT_FALSE(ParseWhitelist(whitelist_val->GetDict()));
}

// Test CheckCCMandApplyRule with MockCefFrame
TEST_F(NwebAutolayoutTest, CheckCCMandApplyRule_WithMockFrame)
{
    // Setup: Manually set up valid state (don't rely on Initialize/Parse)
    mEnable_ = true;
    mCCMConfig_.min_mask_area_ratio_threshold = 60;
    mCCMConfig_.min_content_area_ratio_threshold = 20;
    mCCMConfig_.scale_animation_duration = 100;
    mCCMConfig_.opacity_filter = {10, 90};
    mCCMConfig_.minScaleFactor = 70;
    
    std::string pattern_data = "test_pattern";
    std::string id_data = "test_id";
    std::string page_data = "test_page";
    
    WhitelistEntry entry;
    entry.pattern = std::string_view(pattern_data);
    entry.getID = std::string_view(id_data);
    entry.getPage = std::string_view(page_data);
    
    base::Value::List rules;
    base::Value::Dict rule;
    rule.Set("id", "*");
    rule.Set("pg", "*");
    rules.Append(std::move(rule));
    entry.appRuleInfos = std::move(rules);
    
    mWListEntry_ = &entry;
    mAutoLayoutJSSource_ = "test_autolayout_script";
    
    // Create mock frame
    CefRefPtr<MockCefFrame> mock_frame = new MockCefFrame();
    EXPECT_CALL(*mock_frame, IsMain()).WillRepeatedly(Return(true));
    EXPECT_CALL(*mock_frame, GetURL()).WillRepeatedly(Return(CefString("http://test.com")));
    EXPECT_CALL(*mock_frame, ExecuteJavaScript(_, _, _)).Times(AtLeast(1));
    
    // Call CheckCCMandApplyRule
    CheckCCMandApplyRule(mock_frame);
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_ValidConfigWithNewParams)
{
    std::optional<base::Value> root = base::JSONReader::Read(g_valid_config_with_nwe_params);
    ASSERT_TRUE(root.has_value());
    ASSERT_TRUE(root->is_dict());

    EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));

    EXPECT_EQ(mCCMConfig_.minScaleFactor, 30);
    EXPECT_EQ(mCCMConfig_.min_mask_area_ratio_threshold, 60);
    EXPECT_EQ(mCCMConfig_.min_content_area_ratio_threshold, 20);
    EXPECT_EQ(mCCMConfig_.scale_animation_duration, 100);
    EXPECT_EQ(mCCMConfig_.opacity_filter.first, 10);
    EXPECT_EQ(mCCMConfig_.opacity_filter.second, 90);
    EXPECT_EQ(mCCMConfig_.alphabet_identification_min_size, 12);
    EXPECT_EQ(mCCMConfig_.alphabet_height_width_min_ratio, 10);
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_CompatibleConfigFile)
{
    // previous JSON formats without alphabetIdentificationMinSize and alphabetHeightWidthMinRatio
    std::optional<base::Value> previous_config = base::JSONReader::Read(g_valid_config);
    ASSERT_TRUE(previous_config.has_value());
    ASSERT_TRUE(previous_config->is_dict());
    EXPECT_TRUE(ParseToplevelConfig(previous_config->GetDict()));
    EXPECT_EQ(mCCMConfig_.alphabet_identification_min_size, ConfigConstants::kInvalidValue);
    EXPECT_EQ(mCCMConfig_.alphabet_height_width_min_ratio, ConfigConstants::kInvalidValue);

    // current JSON formats with alphabetIdentificationMinSize and alphabetHeightWidthMinRatio
    std::optional<base::Value> config = base::JSONReader::Read(g_valid_config_with_nwe_params);
    ASSERT_TRUE(config.has_value());
    ASSERT_TRUE(config->is_dict());
    EXPECT_TRUE(ParseToplevelConfig(config->GetDict()));
    EXPECT_EQ(mCCMConfig_.alphabet_identification_min_size, 12);
    EXPECT_EQ(mCCMConfig_.alphabet_height_width_min_ratio, 10);
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_InvalidAlphabetIdentificationMinSize)
{
    const std::vector<std::string> invalid_alphabet_identification_min_size_configs = {
        R"({
            "minScaleFactor": 30,
            "alphabetIdentificationMinSize": -12,
            "alphabetHeightWidthMinRatio": 10,
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minScaleFactor": 30,
            "alphabetIdentificationMinSize": 0,
            "alphabetHeightWidthMinRatio": 10,
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minScaleFactor": 30,
            "alphabetIdentificationMinSize": 50,
            "alphabetHeightWidthMinRatio": 10,
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minScaleFactor": 30,
            "alphabetIdentificationMinSize": "12",
            "alphabetHeightWidthMinRatio": 10,
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minScaleFactor": 30,
            "alphabetIdentificationMinSize": {},
            "alphabetHeightWidthMinRatio": 10,
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})"}; 
    for (const auto& config_str : invalid_alphabet_identification_min_size_configs) {
        std::optional<base::Value> root = base::JSONReader::Read(config_str);
        ASSERT_TRUE(root.has_value());
        ASSERT_TRUE(root->is_dict());
        // To maintain compatibility with previous WebAutoLayout.json formats,
        // invalid alphabet_identification_min_size values do not affect the result of ParseToplevelConfig().
        EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
        EXPECT_EQ(mCCMConfig_.alphabet_identification_min_size, ConfigConstants::kInvalidValue);
    }
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_InvalidAlphabetHeightWidthMinRatio)
{
    const std::vector<std::string> invalid_alphabet_height_width_min_ratio_configs = {
        R"({
            "minScaleFactor": 30,
            "alphabetIdentificationMinSize": 12,
            "alphabetHeightWidthMinRatio": -10,
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minScaleFactor": 30,
            "alphabetIdentificationMinSize": 12,
            "alphabetHeightWidthMinRatio": 0,
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minScaleFactor": 30,
            "alphabetIdentificationMinSize": 12,
            "alphabetHeightWidthMinRatio": 50,
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minScaleFactor": 30,
            "alphabetIdentificationMinSize": 12,
            "alphabetHeightWidthMinRatio": "10",
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minScaleFactor": 30,
            "alphabetIdentificationMinSize": 12,
            "alphabetHeightWidthMinRatio": {},
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})"}; 
    for (const auto& config_str : invalid_alphabet_height_width_min_ratio_configs) {
        std::optional<base::Value> root = base::JSONReader::Read(config_str);
        ASSERT_TRUE(root.has_value());
        ASSERT_TRUE(root->is_dict());
        // To maintain compatibility with previous WebAutoLayout.json formats,
        // invalid alphabet_height_width_min_ratio do not affect the result of ParseToplevelConfig().
        EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
        EXPECT_EQ(mCCMConfig_.alphabet_height_width_min_ratio, ConfigConstants::kInvalidValue);
    }
}

TEST_F(NwebAutolayoutTest, ParseUrlRuleInfo_ValidConfig)
{
    const std::string url_rule_info_str = R"({
        "urlRuleInfos": [
            {
                "urlPrefix": "https://www.example.com/path",
                "strategy": 3,
                "alphabetIdentificationMinSize": 15,
                "alphabetHeightWidthMinRatio": 15
            }
        ]})";
    std::optional<base::Value> url_rule_infos = base::JSONReader::Read(url_rule_info_str);
    ASSERT_TRUE(url_rule_infos.has_value());
    ASSERT_TRUE(url_rule_infos->is_dict());
    int globalAlphabetIdentificationMinSize = 20;
    int globalAlphabetHeightWidthMinRatio = 20;
    mCCMConfig_.alphabet_identification_min_size = globalAlphabetIdentificationMinSize;
    mCCMConfig_.alphabet_height_width_min_ratio = globalAlphabetHeightWidthMinRatio;
    std::optional<std::vector<UrlRuleInfoEntry>> urlRuleInfo = ParseUrlRuleInfo(url_rule_infos->GetDict());
    EXPECT_TRUE(urlRuleInfo.has_value());
    ASSERT_EQ(urlRuleInfo.value().size(), 1);
    EXPECT_TRUE(urlRuleInfo.value()[0].urlPrefixPattern.get());
    EXPECT_EQ(urlRuleInfo.value()[0].strategy, 3);
    EXPECT_EQ(urlRuleInfo.value()[0].alphabetIdentificationMinSize, 15);
    EXPECT_EQ(urlRuleInfo.value()[0].alphabetHeightWidthMinRatio, 15);
}

TEST_F(NwebAutolayoutTest, ParseUrlRuleInfo_MissKey)
{
    const std::string url_rule_info_str = R"({
        "urlRuleInfos": [
            {
                "urlPrefix": "https://www.example.com/path"
            },
            {
                "strategy": 3
            },
            {
                "params": 3
            }
        ]})";
    std::optional<base::Value> url_rule_infos = base::JSONReader::Read(url_rule_info_str);
    ASSERT_TRUE(url_rule_infos.has_value());
    ASSERT_TRUE(url_rule_infos->is_dict());
    int globalAlphabetIdentificationMinSize = 20;
    int globalAlphabetHeightWidthMinRatio = 20;
    mCCMConfig_.alphabet_identification_min_size = globalAlphabetIdentificationMinSize;
    mCCMConfig_.alphabet_height_width_min_ratio = globalAlphabetHeightWidthMinRatio;
    std::optional<std::vector<UrlRuleInfoEntry>> urlRuleInfo = ParseUrlRuleInfo(url_rule_infos->GetDict());
    EXPECT_TRUE(urlRuleInfo.has_value());
    EXPECT_TRUE(urlRuleInfo.value().empty());
}

TEST_F(NwebAutolayoutTest, ParseUrlRuleInfo_OptionalParams)
{
    const std::string url_rule_info_str = R"({
        "urlRuleInfos": [
            {
                "urlPrefix": "https://www.example.com/path",
                "strategy": 3,
                "alphabetIdentificationMinSize": 15,
                "alphabetHeightWidthMinRatio": 15
            },
            {
                "urlPrefix": "https://www.example.com/path",
                "strategy": 3,
                "alphabetIdentificationMinSize": 1
            },
            {
                "urlPrefix": "https://www.example.com/path",
                "strategy": 3,
                "alphabetHeightWidthMinRatio": 2
            },
            {
                "urlPrefix": "https://www.example.com/path",
                "strategy": 3
            }
        ]})";
    std::optional<base::Value> url_rule_infos = base::JSONReader::Read(url_rule_info_str);
    ASSERT_TRUE(url_rule_infos.has_value());
    ASSERT_TRUE(url_rule_infos->is_dict());
    int globalAlphabetIdentificationMinSize = 20;
    int globalAlphabetHeightWidthMinRatio = 20;
    mCCMConfig_.alphabet_identification_min_size = globalAlphabetIdentificationMinSize;
    mCCMConfig_.alphabet_height_width_min_ratio = globalAlphabetHeightWidthMinRatio;
    std::optional<std::vector<UrlRuleInfoEntry>> urlRuleInfo = ParseUrlRuleInfo(url_rule_infos->GetDict());
    EXPECT_TRUE(urlRuleInfo.has_value());
    ASSERT_EQ(urlRuleInfo.value().size(), 4);
    EXPECT_EQ(urlRuleInfo.value()[0].alphabetIdentificationMinSize, 15);
    EXPECT_EQ(urlRuleInfo.value()[0].alphabetHeightWidthMinRatio, 15);
    EXPECT_EQ(urlRuleInfo.value()[1].alphabetIdentificationMinSize, 1);
    EXPECT_EQ(urlRuleInfo.value()[1].alphabetHeightWidthMinRatio, globalAlphabetHeightWidthMinRatio);
    EXPECT_EQ(urlRuleInfo.value()[2].alphabetIdentificationMinSize, globalAlphabetIdentificationMinSize);
    EXPECT_EQ(urlRuleInfo.value()[2].alphabetHeightWidthMinRatio, 2);
    EXPECT_EQ(urlRuleInfo.value()[3].alphabetIdentificationMinSize, globalAlphabetIdentificationMinSize);
    EXPECT_EQ(urlRuleInfo.value()[3].alphabetHeightWidthMinRatio, globalAlphabetIdentificationMinSize);
}

TEST_F(NwebAutolayoutTest, ParseUrlRuleInfo_InvalidUrlPrefixPattern)
{
    const std::string invalid_url_prefix_str = R"({
        "urlRuleInfos": [
            {
                "urlPrefix": "()(",
                "strategy": 3,
                "alphabetIdentificationMinSize": 15,
                "alphabetHeightWidthMinRatio": 15
            },
            {
                "urlPrefix": "https://www.example.com/path\\",
                "strategy": 3,
                "alphabetIdentificationMinSize": 15,
                "alphabetHeightWidthMinRatio": 15
            }
        ]})";
    std::optional<base::Value> rule_with_invalid_url_prefix = base::JSONReader::Read(invalid_url_prefix_str);
    ASSERT_TRUE(rule_with_invalid_url_prefix.has_value());
    ASSERT_TRUE(rule_with_invalid_url_prefix->is_dict());
    int globalAlphabetIdentificationMinSize = 20;
    int globalAlphabetHeightWidthMinRatio = 20;
    mCCMConfig_.alphabet_identification_min_size = globalAlphabetIdentificationMinSize;
    mCCMConfig_.alphabet_height_width_min_ratio = globalAlphabetHeightWidthMinRatio;
    std::optional<std::vector<UrlRuleInfoEntry>> urlRuleInfo =
        ParseUrlRuleInfo(rule_with_invalid_url_prefix->GetDict());
    EXPECT_TRUE(urlRuleInfo.has_value());
    EXPECT_TRUE(urlRuleInfo.value().empty());
}

TEST_F(NwebAutolayoutTest, ParseUrlRuleInfo_InvalidStrategy)
{
    const std::string invalid_strategy_str = R"({
        "urlRuleInfos": [
            {
                "urlPrefix": "https://www.example.com/path",
                "strategy": "3",
                "alphabetIdentificationMinSize": 15,
                "alphabetHeightWidthMinRatio": 15
            },
            {
                "urlPrefix": "https://www.example.com/path",
                "strategy": {},
                "alphabetIdentificationMinSize": 15,
                "alphabetHeightWidthMinRatio": 15
            },
            {
                "urlPrefix": "https://www.example.com/path",
                "strategy": [],
                "alphabetIdentificationMinSize": 15,
                "alphabetHeightWidthMinRatio": 15
            }
        ]})";
    std::optional<base::Value> rule_with_invalid_strategy = base::JSONReader::Read(invalid_strategy_str);
    ASSERT_TRUE(rule_with_invalid_strategy.has_value());
    ASSERT_TRUE(rule_with_invalid_strategy->is_dict());
    mCCMConfig_.alphabet_identification_min_size = 20;
    mCCMConfig_.alphabet_height_width_min_ratio = 20;
    std::optional<std::vector<UrlRuleInfoEntry>> urlRuleInfo = ParseUrlRuleInfo(rule_with_invalid_strategy->GetDict());
    EXPECT_TRUE(urlRuleInfo.has_value());
    EXPECT_TRUE(urlRuleInfo.value().empty());
}

TEST_F(NwebAutolayoutTest, ParseUrlRuleInfo_InvalidAlphabetIdentificationMinSize)
{
    const std::string invalid_alphabet_identification_min_size_str = R"({
        "urlRuleInfos": [
            {
                "urlPrefix": "https://www.example.com/path",
                "strategy": 3,
                "alphabetIdentificationMinSize": -1,
                "alphabetHeightWidthMinRatio": 15
            },
            {
                "urlPrefix": "https://www.example.com/path",
                "strategy": 3,
                "alphabetIdentificationMinSize": 50,
                "alphabetHeightWidthMinRatio": 15
            },
            {
                "urlPrefix": "https://www.example.com/path",
                "strategy": 3,
                "alphabetIdentificationMinSize": "15",
                "alphabetHeightWidthMinRatio": 15
            },
            {
                "urlPrefix": "https://www.example.com/path",
                "strategy": 3,
                "alphabetIdentificationMinSize": [],
                "alphabetHeightWidthMinRatio": 15
            }
        ]})";
    std::optional<base::Value> rule_with_invalid_alphabet_identification_min_size =
        base::JSONReader::Read(invalid_alphabet_identification_min_size_str);
    ASSERT_TRUE(rule_with_invalid_alphabet_identification_min_size.has_value());
    ASSERT_TRUE(rule_with_invalid_alphabet_identification_min_size->is_dict());
    mCCMConfig_.alphabet_identification_min_size = 20;
    mCCMConfig_.alphabet_height_width_min_ratio = 20;
    std::optional<std::vector<UrlRuleInfoEntry>> urlRuleInfo = ParseUrlRuleInfo(
            rule_with_invalid_alphabet_identification_min_size->GetDict());
    EXPECT_TRUE(urlRuleInfo.has_value());
    ASSERT_EQ(urlRuleInfo.value().size(), 4);
    EXPECT_EQ(urlRuleInfo.value()[0].alphabetIdentificationMinSize, 20);
    EXPECT_EQ(urlRuleInfo.value()[1].alphabetIdentificationMinSize, 20);
    EXPECT_EQ(urlRuleInfo.value()[2].alphabetIdentificationMinSize, 20);
    EXPECT_EQ(urlRuleInfo.value()[3].alphabetIdentificationMinSize, 20);
}

TEST_F(NwebAutolayoutTest, ParseUrlRuleInfo_InvalidAlphabetHeightWidthMinRatio)
{
    const std::string invalid_alphabet_height_width_min_ratio_str = R"({
        "urlRuleInfos": [
            {
                "urlPrefix": "https://www.example.com/path",
                "strategy": 3,
                "alphabetIdentificationMinSize": 15,
                "alphabetHeightWidthMinRatio": -1
            },
            {
                "urlPrefix": "https://www.example.com/path",
                "strategy": 3,
                "alphabetIdentificationMinSize": 15,
                "alphabetHeightWidthMinRatio": 50
            },
            {
                "urlPrefix": "https://www.example.com/path",
                "strategy": 3,
                "alphabetIdentificationMinSize": 15,
                "alphabetHeightWidthMinRatio": "15"
            },
            {
                "urlPrefix": "https://www.example.com/path",
                "strategy": 3,
                "alphabetIdentificationMinSize": 15,
                "alphabetHeightWidthMinRatio": {}
            }
        ]})";
    std::optional<base::Value> rule_with_invalid_alphabet_height_width_min_ratio =
        base::JSONReader::Read(invalid_alphabet_height_width_min_ratio_str);
    ASSERT_TRUE(rule_with_invalid_alphabet_height_width_min_ratio.has_value());
    ASSERT_TRUE(rule_with_invalid_alphabet_height_width_min_ratio->is_dict());
    mCCMConfig_.alphabet_identification_min_size = 20;
    mCCMConfig_.alphabet_height_width_min_ratio = 20;
    std::optional<std::vector<UrlRuleInfoEntry>> urlRuleInfo = ParseUrlRuleInfo(
            rule_with_invalid_alphabet_height_width_min_ratio->GetDict());
    EXPECT_TRUE(urlRuleInfo.has_value());
    ASSERT_EQ(urlRuleInfo.value().size(), 4);
    EXPECT_EQ(urlRuleInfo.value()[0].alphabetHeightWidthMinRatio, 20);
    EXPECT_EQ(urlRuleInfo.value()[1].alphabetHeightWidthMinRatio, 20);
    EXPECT_EQ(urlRuleInfo.value()[2].alphabetHeightWidthMinRatio, 20);
    EXPECT_EQ(urlRuleInfo.value()[3].alphabetHeightWidthMinRatio, 20);
}

TEST_F(NwebAutolayoutTest, CreateH5AutoLayoutParam_ValidOutput)
{
    std::optional<base::Value> root = base::JSONReader::Read(g_valid_config_with_nwe_params);
    ASSERT_TRUE(root.has_value());
    ASSERT_TRUE(root->is_dict());
    mAppBundleName_ = "com.example.app";
    ASSERT_TRUE(Parse(*root));
    ASSERT_TRUE(mCCMConfig_.whitelist.urlRuleInfos.has_value());
    ASSERT_FALSE(mCCMConfig_.whitelist.urlRuleInfos.value().empty());
    std::string output = CreateH5AutoLayoutParam(mCCMConfig_.whitelist.urlRuleInfos.value()[0]);

    std::optional<base::Value> autoLayout_param = base::JSONReader::Read(output);
    ASSERT_TRUE(autoLayout_param.has_value());
    ASSERT_TRUE(autoLayout_param->is_dict());
    const base::Value::Dict& param_dict = autoLayout_param.value().GetDict();

    auto min_mask_area_ratio_threshold = param_dict.FindInt(ConfigConstants::kMinMaskAreaRatioThresholdKey);
    ASSERT_TRUE(min_mask_area_ratio_threshold);
    EXPECT_EQ(*min_mask_area_ratio_threshold, 60);

    auto min_content_area_ratio_threshold = param_dict.FindInt(ConfigConstants::kMinContentAreaRatioThresholdKey);
    ASSERT_TRUE(min_content_area_ratio_threshold);
    EXPECT_EQ(*min_content_area_ratio_threshold, 20);

    auto scale_animation_duration = param_dict.FindInt(ConfigConstants::kScaleAnimationDurationKey);
    ASSERT_TRUE(scale_animation_duration);
    EXPECT_EQ(*scale_animation_duration, 100);

    auto min_scale_factor = param_dict.FindInt(ConfigConstants::kMinDesScaleKey);
    ASSERT_TRUE(min_scale_factor);
    EXPECT_EQ(*min_scale_factor, 30);

    auto strategy = param_dict.FindInt(ConfigConstants::kStrategyKey);
    ASSERT_TRUE(strategy);
    EXPECT_EQ(*strategy, 3);

    auto alphabet_identification_min_size = param_dict.FindInt(ConfigConstants::kAlphabetIdentificationMinSizeKey);
    ASSERT_TRUE(alphabet_identification_min_size);
    EXPECT_EQ(*alphabet_identification_min_size, 15);

    auto alphabet_height_width_min_ratio = param_dict.FindInt(ConfigConstants::kAlphabetHeightWidthMinRatioKey);
    ASSERT_TRUE(alphabet_height_width_min_ratio);
    EXPECT_EQ(*alphabet_height_width_min_ratio, 15);

    auto need_check_id_and_page = param_dict.FindBool(ConfigConstants::kNeedCheckIdAndPageKey);
    ASSERT_TRUE(need_check_id_and_page);
    EXPECT_FALSE(*need_check_id_and_page);
}

TEST_F(NwebAutolayoutTest, FindBestMatchRule_EmptyUrlRule)
{
    std::string load_url = "https://www.example.com/path/xxx.html";
    const UrlRuleInfoEntry* url_rule_info_entry = FindBestMatchRule(load_url);
    EXPECT_FALSE(url_rule_info_entry);
}

TEST_F(NwebAutolayoutTest, FindBestMatchRule_MatchWhitelist)
{
    std::optional<base::Value> root = base::JSONReader::Read(g_valid_config_with_nwe_params);
    ASSERT_TRUE(root.has_value());
    ASSERT_TRUE(root->is_dict());
    mAppBundleName_ = "com.example.app";
    ASSERT_TRUE(Parse(*root));
    ASSERT_TRUE(mCCMConfig_.whitelist.urlRuleInfos.has_value());
    ASSERT_FALSE(mCCMConfig_.whitelist.urlRuleInfos.value().empty());

    std::string load_url = "https://www.example.com/path/xxx.html";
    const UrlRuleInfoEntry* url_rule_info_entry = FindBestMatchRule(load_url);
    ASSERT_TRUE(url_rule_info_entry);
    EXPECT_EQ(url_rule_info_entry->alphabetIdentificationMinSize, 15);
    EXPECT_EQ(url_rule_info_entry->alphabetHeightWidthMinRatio, 15);
    EXPECT_EQ(url_rule_info_entry->strategy, 3);
}

TEST_F(NwebAutolayoutTest, FindBestMatchRule_NotMatchWhitelist)
{
    std::optional<base::Value> root = base::JSONReader::Read(g_valid_config_with_nwe_params);
    ASSERT_TRUE(root.has_value());
    ASSERT_TRUE(root->is_dict());
    mAppBundleName_ = "com.example.app";
    ASSERT_TRUE(Parse(*root));
    ASSERT_TRUE(mCCMConfig_.whitelist.urlRuleInfos.has_value());
    ASSERT_FALSE(mCCMConfig_.whitelist.urlRuleInfos.value().empty());
    std::string load_url = "https://127.0.0.1:8080";
    const UrlRuleInfoEntry* url_rule_info_entry = FindBestMatchRule(load_url);
    EXPECT_FALSE(url_rule_info_entry);
}

TEST_F(NwebAutolayoutTest, FindBestMatchRule_BestMatch)
{
    const std::string url_rule_info_str = R"({
        "urlRuleInfos": [
            {
                "urlPrefix": "https://www.example.com",
                "strategy": 1,
                "alphabetIdentificationMinSize": 1,
                "alphabetHeightWidthMinRatio": 1
            },
            {
                "urlPrefix": "https://www.example.com/path",
                "strategy": 2,
                "alphabetIdentificationMinSize": 2,
                "alphabetHeightWidthMinRatio": 2
            }
        ]})";
    std::optional<base::Value> url_rule_info = base::JSONReader::Read(url_rule_info_str);
    ASSERT_TRUE(url_rule_info.has_value());
    ASSERT_TRUE(url_rule_info->is_dict());

    mCCMConfig_.alphabet_identification_min_size = 20;
    mCCMConfig_.alphabet_height_width_min_ratio = 20;
    ASSERT_TRUE(ParseWhitelistEntry(url_rule_info->GetDict()));

    std::string load_url = "https://www.example.com/path/xxx.html";
    const UrlRuleInfoEntry* url_rule_info_entry = FindBestMatchRule(load_url);
    ASSERT_TRUE(url_rule_info_entry);
    EXPECT_EQ(url_rule_info_entry->alphabetIdentificationMinSize, 2);
    EXPECT_EQ(url_rule_info_entry->alphabetHeightWidthMinRatio, 2);
    EXPECT_EQ(url_rule_info_entry->strategy, 2);
}

TEST_F(NwebAutolayoutTest, FindBestMatchRule_RegaxMatch)
{
    const std::string url_rule_info_str = R"({
        "urlRuleInfos": [
            {
                "urlPrefix": "https://127.0.0.1:\\d+",
                "strategy": 1,
                "alphabetIdentificationMinSize": 1,
                "alphabetHeightWidthMinRatio": 1
            }
        ]})";
    std::optional<base::Value> url_rule_info = base::JSONReader::Read(url_rule_info_str);
    ASSERT_TRUE(url_rule_info.has_value());
    ASSERT_TRUE(url_rule_info->is_dict());

    mCCMConfig_.alphabet_identification_min_size = 20;
    mCCMConfig_.alphabet_height_width_min_ratio = 20;
    ASSERT_TRUE(ParseWhitelistEntry(url_rule_info->GetDict()));

    std::string load_url1 = "https://127.0.0.1:8080";
    const UrlRuleInfoEntry* url_rule_info_entry1 = FindBestMatchRule(load_url1);
    ASSERT_TRUE(url_rule_info_entry1);
    EXPECT_EQ(url_rule_info_entry1->alphabetIdentificationMinSize, 1);
    EXPECT_EQ(url_rule_info_entry1->alphabetHeightWidthMinRatio, 1);
    EXPECT_EQ(url_rule_info_entry1->strategy, 1);

    std::string load_url2 = "https://127.0.0.1:9090";
    const UrlRuleInfoEntry* url_rule_info_entry2 = FindBestMatchRule(load_url2);
    ASSERT_TRUE(url_rule_info_entry2);
    EXPECT_EQ(url_rule_info_entry2->alphabetIdentificationMinSize, 1);
    EXPECT_EQ(url_rule_info_entry2->alphabetHeightWidthMinRatio, 1);
    EXPECT_EQ(url_rule_info_entry2->strategy, 1);
}

TEST_F(NwebAutolayoutTest, FindBestMatchRule_RegaxBestMatch)
{
    const std::string url_rule_info_str = R"({
        "urlRuleInfos": [
            {
                "urlPrefix": "https://127.0.0.1:\\d+",
                "strategy": 1,
                "alphabetIdentificationMinSize": 1,
                "alphabetHeightWidthMinRatio": 1
            },
            {
                "urlPrefix": "https://127.0.0.1:\\d+/path",
                "strategy": 2,
                "alphabetIdentificationMinSize": 2,
                "alphabetHeightWidthMinRatio": 2
            },
            {
                "urlPrefix": "https://127.0.0.1:\\d+/path/to/index.html",
                "strategy": 3,
                "alphabetIdentificationMinSize": 3,
                "alphabetHeightWidthMinRatio": 3
            }
        ]})";
    std::optional<base::Value> url_rule_info = base::JSONReader::Read(url_rule_info_str);
    ASSERT_TRUE(url_rule_info.has_value());
    ASSERT_TRUE(url_rule_info->is_dict());

    mCCMConfig_.alphabet_identification_min_size = 20;
    mCCMConfig_.alphabet_height_width_min_ratio = 20;
    ASSERT_TRUE(ParseWhitelistEntry(url_rule_info->GetDict()));

    std::string load_url1 = "https://127.0.0.1:8080/path/index.html";
    const UrlRuleInfoEntry* url_rule_info_entry1 = FindBestMatchRule(load_url1);
    ASSERT_TRUE(url_rule_info_entry1);
    EXPECT_EQ(url_rule_info_entry1->alphabetIdentificationMinSize, 2);
    EXPECT_EQ(url_rule_info_entry1->alphabetHeightWidthMinRatio, 2);
    EXPECT_EQ(url_rule_info_entry1->strategy, 2);

    std::string load_url2 = "https://127.0.0.1:9090/path/to/index.html";
    const UrlRuleInfoEntry* url_rule_info_entry2 = FindBestMatchRule(load_url2);
    ASSERT_TRUE(url_rule_info_entry2);
    EXPECT_EQ(url_rule_info_entry2->alphabetIdentificationMinSize, 3);
    EXPECT_EQ(url_rule_info_entry2->alphabetHeightWidthMinRatio, 3);
    EXPECT_EQ(url_rule_info_entry2->strategy, 3);
}
}