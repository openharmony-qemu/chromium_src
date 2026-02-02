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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <functional>

#include "arkweb/build/features/features.h"
#include "base/functional/bind.h"
#include "base/memory/weak_ptr.h"
#include "nweb_content_change_detection.h"
#include "nweb_delegate.h"
#include "ui/base/resource/resource_bundle.h"

using namespace testing;

namespace OHOS::NWeb {

// Mock NWebDelegate for testing
// Note: This is a minimal mock that only implements methods required for
// NWebContentChangeDetection testing. Other methods return default values.
class MockNWebDelegate : public NWebDelegateInterface {
 public:
  MockNWebDelegate() = default;
  virtual ~MockNWebDelegate() = default;

  // Methods used by NWebContentChangeDetection
  MOCK_METHOD(void, ExecuteJavaScript, (const std::string& code), (const, override));

#if BUILDFLAG(ARKWEB_AI)
  MOCK_METHOD(void,
              RegisterOnLoadStartedCbForContentChange,
              (std::function<void(void)>&& callback),
              ());
#endif

  // Implement base::SupportsWeakPtr for weak_ptr support
  base::WeakPtr<MockNWebDelegate> WeakFromThis() {
    return weak_factory_.GetWeakPtr();
  }

 private:
  // Allow weak_ptr support
  base::WeakPtrFactory<MockNWebDelegate> weak_factory_{this};

 public:
  // Minimal stub implementations for all other interface methods
  // These return default values and are not actually used in tests
  void OnWindowShow() override {}
  void OnWindowHide() override {}
  void OnOnlineRenderToForeground() override {}
  void NotifyForNextTouchEvent() override {}
  bool IsReady() override { return false; }
  void OnDestroy(bool) override {}
  void RegisterWebAppClientExtensionListener(std::shared_ptr<NWebAppClientExtensionCallback>) override {}
  void RegisterDownLoadListener(std::shared_ptr<NWebDownloadCallback>) override {}
  void RegisterReleaseSurfaceListener(std::shared_ptr<NWebReleaseSurfaceCallback>) override {}
  void RegisterWebDownloadDelegateListener(std::shared_ptr<NWebDownloadDelegateCallback>) override {}
  void StartDownload(const char*) override {}
  void ResumeDownload(std::shared_ptr<NWebDownloadItem>) override {}
  void RegisterNWebHandler(std::shared_ptr<NWebHandler>) override {}
  void RegisterRenderCb(std::function<void(const char*)>) override {}
  void SetInputMethodClient(CefRefPtr<NWebInputMethodClient>) override {}
#if BUILDFLAG(ARKWEB_SCREEN_OFFSET)
  void SetScreenOffset(double, double) override {}
#endif
  void Resize(uint32_t, uint32_t, bool) override {}
#if BUILDFLAG(ARKWEB_INPUT_EVENTS) || BUILDFLAG(ARKWEB_VIEWPORT_AVOID)
  void ResizeVisibleViewport(uint32_t, uint32_t, bool) override {}
#endif
#if BUILDFLAG(ARKWEB_VIEWPORT_AVOID)
  void AvoidVisibleViewportBottom(int32_t) override {}
  int32_t GetVisibleViewportAvoidHeight() override { return 0; }
#endif
  void OnTouchPress(int32_t, double, double, bool) override {}
  void OnTouchRelease(int32_t, double, double, bool) override {}
  void OnTouchMove(int32_t, double, double, bool) override {}
  void OnTouchMove(const std::vector<std::shared_ptr<NWebTouchPointInfo>>&, bool) override {}
  void OnStylusTouchPress(std::shared_ptr<NWebStylusTouchPointInfo>, bool) override {}
  void OnStylusTouchRelease(std::shared_ptr<NWebStylusTouchPointInfo>, bool) override {}
  void OnStylusTouchMove(const std::vector<std::shared_ptr<NWebStylusTouchPointInfo>>&, bool) override {}
  void OnTouchCancel() override {}
  void OnTouchCancelById(int32_t, double, double, bool) override {}
  bool SendKeyEvent(int32_t, int32_t) override { return false; }
  void SendTouchpadFlingEvent(double, double, double, double) override {}
  void SendMouseWheelEvent(double, double, double, double) override {}
  void SendMouseEvent(int, int, int, int, int) override {}
  void NotifyScreenInfoChanged(RotationType, DisplayOrientation) override {}
  int Load(const std::string&) override { return 0; }
  int Load(const std::string&, const std::map<std::string, std::string>&) override { return 0; }
  bool IsNavigatebackwardAllowed() const override { return false; }
  bool IsNavigateForwardAllowed() const override { return false; }
  bool CanNavigateBackOrForward(int) const override { return false; }
  void NavigateBack() const override {}
  void NavigateForward() const override {}
  void NavigateBackOrForward(int32_t) const override {}
  void DeleteNavigateHistory() override {}
  void ClearSslCache() override {}
  void ClearClientAuthenticationCache() override {}
  void Reload() const override {}
  void ReloadOriginalUrl() const override {}
  int Zoom(float) const override { return 0; }
  int ZoomIn() const override { return 0; }
  int ZoomOut() const override { return 0; }
  void Stop() const override {}
  void ExecuteJavaScript(const std::string&) const override {}
  void PutBackgroundColor(int) const override {}
  void InitialScale(float) const override {}
  void PutOptimizeParserBudgetEnabled(bool) const override {}
  void OnPause() override {}
  void OnContinue() override {}
  void WebComponentsBlur() override {}
  void OnOccluded() override {}
  void OnUnoccluded() override {}
  void SetEnableLowerFrameRate(bool) override {}
  void SetEnableHalfFrameRate(bool) override {}
  std::shared_ptr<NWebPreference> GetPreference() const override { return nullptr; }
#if BUILDFLAG(ARKWEB_AI)
  std::shared_ptr<NWebAgentManager> GetAgentManager() const override { return nullptr; }
#endif
  std::string Title() override { return std::string(); }
  std::shared_ptr<HitTestResult> GetHitTestResult() const override { return nullptr; }
  std::shared_ptr<HitTestResult> GetLastHitTestResult() const override { return nullptr; }
  int PageLoadProgress() override { return 0; }
  float Scale() override { return 0.0f; }
  int LoadWithData(const std::string&, const std::string&, const std::string&) override { return 0; }
  int LoadWithDataAndBaseUrl(const std::string&, const std::string&, const std::string&,
                             const std::string&, const std::string&) override { return 0; }
  const std::string GetOriginalUrl() override { return std::string(); }
  int ContentHeight() override { return 0; }
  void RegisterNativeArkJSFunction(const char*, const std::vector<std::shared_ptr<NWebJsProxyCallback>>&) override {}
  void RegisterNativeJSProxy(const std::string&, const std::vector<std::string>&,
                             std::vector<std::function<char*(std::vector<std::vector<uint8_t>>&, std::vector<size_t>&)>>&&,
                             bool, const std::string&) override {}
  void RegisterNativeJSProxyWithResult(const std::string&, const std::vector<std::string>&,
                                       std::vector<std::function<std::shared_ptr<OHOS::NWeb::NWebValue>(
                                           std::vector<std::vector<uint8_t>>&, std::vector<size_t>&)>>&&,
                                       bool, const std::string&) override {}
  void RegisterNativeAsyncThreadJSProxyWithResult(const std::string&, const std::vector<std::string>&,
                                                   std::vector<std::function<std::shared_ptr<OHOS::NWeb::NWebValue>(
                                                       std::vector<std::vector<uint8_t>>&, std::vector<size_t>&)>>&&,
                                                   const std::string&) override {}
  void UnRegisterNativeArkJSFunction(const char*) override {}
  void RegisterNativeLoadStartCallback(std::function<void(void)>&&) override {}
  void RegisterNativeLoadEndCallback(std::function<void(void)>&&) override {}
  void RegisterNativeScrollCallback(std::function<void(double, double)>&&) override {}
#if BUILDFLAG(ARKWEB_ADBLOCK)
  void UpdateAdblockEasyListRules(long) override {}
#endif
  void RegisterArkJSfunction(const std::string&, const std::vector<std::string>&,
                             const std::vector<std::string>&, const int32_t, const std::string&) const override {}
  void UnregisterArkJSfunction(const std::string&, const std::vector<std::string>&) const override {}
#if BUILDFLAG(ARKWEB_JSPROXY)
  void JavaScriptOnDocumentStart(const ScriptItems&) override {}
  void JavaScriptOnDocumentEnd(const ScriptItems&) override {}
  void JavaScriptOnDocumentStartByOrder(const ScriptItems&, const ScriptRegexItems&, const ScriptItemsByOrder&) override {}
  void JavaScriptOnDocumentEndByOrder(const ScriptItems&, const ScriptRegexItems&, const ScriptItemsByOrder&) override {}
  void JavaScriptOnHeadReadyByOrder(const ScriptItems&, const ScriptRegexItems&, const ScriptItemsByOrder&) override {}
#endif
  void CallH5Function(int32_t, int32_t, const std::string&,
                      const std::vector<std::shared_ptr<NWebValue>>&) const override {}
  void CallH5FunctionV2(int32_t, int32_t, const std::string&,
                        const std::vector<std::shared_ptr<NWebRomValue>>&) const override {}
  void RegisterNWebJavaScriptCallBack(std::shared_ptr<NWebJavaScriptResultCallBack>) override {}
  bool OnFocus(const FocusReason&) const override { return false; }
  void OnBlur() const override {}
  void RegisterFindListener(std::shared_ptr<NWebFindCallback>) override {}
  void FindAllAsync(const std::string&) const override {}
  void ClearMatches() const override {}
  void FindNext(const bool) const override {}
  bool TerminateRenderProcess() override { return false; }
  std::string GetUrl() const override { return std::string(); }
  bool GetFavicon(const void**, size_t&, size_t&, ImageColorType&, ImageAlphaType&) override { return false; }
  void PutNetworkAvailable(bool) override {}
  void SetWindowId(uint32_t) override {}
  void SuggestionSelected(int) const override {}
  bool SetZoomInFactor(float) override { return false; }
  bool SetZoomOutFactor(float) override { return false; }
  bool IsFileProtocol(const GURL&) override { return false; }
  bool IsUrlFileExist(const GURL&, const std::string&) override { return false; }
#if BUILDFLAG(ARKWEB_PDF)
  void EraseCreatePDFCallbackImpl(uint32_t) override {}
  void ExecuteCreatePDFExt(std::shared_ptr<NWebPDFConfigArgs>,
                           std::shared_ptr<NWebArrayBufferValueCallback>) override {}
#endif
  void ExecuteJavaScriptExt(const int, const size_t,
                            std::shared_ptr<NWebMessageValueCallback>, bool) override {}
  int ScaleGestureChange(double, double, double) const override { return 0; }
  int ScaleGestureChangeV2(int, double, double, double, double) const override { return 0; }
  std::vector<std::string> CreateWebMessagePorts() override { return std::vector<std::string>(); }
#if BUILDFLAG(ARKWEB_MSGPORT)
  void ClosePort(const std::string&) override {}
  void PostWebMessage(const std::string&, const std::vector<std::string>&, const std::string&) override {}
  void PostPortMessage(const std::string&, std::shared_ptr<NWebMessage>) override {}
  void PostPortMessageV2(const std::string&, std::shared_ptr<NWebRomValue>) override {}
  void SetPortMessageCallback(const std::string&, std::shared_ptr<NWebMessageValueCallback>) override {}
  void EraseJavaScriptCallbackImpl(uint32_t) override {}
  void ExecuteJavaScript(const std::string&, std::shared_ptr<NWebMessageValueCallback>, bool) override {}
#endif
  void SetNWebId(uint32_t) override {}
#if BUILDFLAG(IS_ARKWEB)
  void EnableAppLinking(bool) override {}
#endif
  void StoreWebArchive(const std::string&, bool, std::shared_ptr<NWebStringValueCallback>) const override {}
  void SetBrowserUserAgentString(const std::string&) override {}
  void SendDragEvent(const DelegateDragEvent&) const override {}
#if BUILDFLAG(ARKWEB_I18N)
  void UpdateLocale(const std::string&, const std::string&) override {}
#endif
  CefRefPtr<ArkWebClientExt> GetCefClient() const override { return nullptr; }
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  std::shared_ptr<NWebDragData> GetOrCreateDragData() override { return nullptr; }
  void ClearDragData() const override {}
  bool DarkModeEnabled() override { return false; }
#endif
  void GetImages(std::shared_ptr<NWebBoolValueCallback>) override {}
  void RemoveCache(bool) override {}
#if BUILDFLAG(ARKWEB_NAVIGATION)
  std::shared_ptr<NWebHistoryList> GetHistoryList() override { return nullptr; }
  std::vector<uint8_t> SerializeWebState() override { return std::vector<uint8_t>(); }
  bool RestoreWebState(const std::vector<uint8_t>&) override { return false; }
#endif
#if BUILDFLAG(ARKWEB_NWEB_EX)
  bool CanStoreWebArchive() const override { return false; }
  void UnRegisterArkWebAppClientExtensionListener() override {}
  void UnRegisterWebAppClientExtensionListener() override {}
  void UnRegisterWebExtensionListener() override {}
  void RegisterWebExtensionListener(std::shared_ptr<NWebExtensionCallback>) override {}
  void GetImageFromContextNode() override {}
  void GetImageFromCacheEx(const std::string&) override {}
#endif
#if BUILDFLAG(ARKWEB_MEDIA_MUTE_AUDIO)
  void SetAudioMuted(bool) override {}
#endif
#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
  void SetAudioResumeInterval(int32_t) override {}
  void SetAudioExclusive(bool) override {}
  void SetAudioSessionType(int32_t) override {}
  void CloseAllMediaPresentations() override {}
  void StopAllMedia() override {}
  void ResumeAllMedia() override {}
  void PauseAllMedia() override {}
  int GetMediaPlaybackState() override { return 0; }
#endif
  void PrefetchPage(const PrefetchOptions&) override {}
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void SetVirtualKeyBoardArg(int32_t, int32_t, double) override {}
  bool ShouldVirtualKeyboardOverlay() override { return false; }
#endif
#if BUILDFLAG(ARKWEB_SAFEBROWSING)
  bool IsSafeBrowsingEnabled() override { return false; }
  void EnableSafeBrowsing(bool) override {}
  void EnableSafeBrowsingDetection(bool, bool) override {}
  void OnSafeBrowsingDetectionResult(int, int, const std::string&, const std::string&) override {}
#endif
#if BUILDFLAG(IS_OHOS)
  void PrecompileJavaScript(const std::string&, const std::string&,
                            std::shared_ptr<CacheOptions>&, std::shared_ptr<NWebMessageValueCallback>) override {}
  void UpdateNativeEmbedInfo(std::shared_ptr<NWebNativeEmbedDataInfo>) override {}
  bool HitNativeArea(double, double) override { return false; }
#endif
#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
  void SetTransformHint(uint32_t) override {}
#endif
#if BUILDFLAG(ARKWEB_SECURITY_STATE)
  int GetSecurityLevel() override { return 0; }
#endif
#if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)
  void PageUp(bool) override {}
  void PageDown(bool) override {}
#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
  void GetScrollOffset(float*, float*) override {}
#endif
#endif
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void SetNWebDelegateInterface(std::shared_ptr<NWebDelegateInterface>) override {}
  void ScrollTo(float, float) override {}
  void ScrollBy(float, float) override {}
  void ScrollByRefScreen(float, float, float, float) override {}
  void SlideScroll(float, float) override {}
  bool WebSendKeyEvent(int32_t, int32_t, const std::vector<int32_t>&) override { return false; }
  void WebSendMouseWheelEvent(double, double, double, double, const std::vector<int32_t>&) override {}
  void WebSendMouseWheelEventV2(double, double, double, double, const std::vector<int32_t>&, int32_t) override {}
  void WebSendTouchpadFlingEvent(double, double, double, double, const std::vector<int32_t>&) override {}
  void WebSendCancelFlingEvent() override {}
  bool SendKeyboardEvent(const std::shared_ptr<OHOS::NWeb::NWebKeyboardEvent>&) override { return false; }
  bool ScrollByWithResult(float, float) override { return false; }
  void ScrollToWithAnime(float, float, int32_t) override {}
  void ScrollByWithAnime(float, float, int32_t) override {}
  void WebSendMouseEvent(const std::shared_ptr<OHOS::NWeb::NWebMouseEvent>&) override {}
#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
  void GetOverScrollOffset(float*, float*) override {}
#endif
#endif
#if BUILDFLAG(ARKWEB_ADBLOCK)
  void EnableAdsBlock(bool) override {}
  bool IsAdsBlockEnabled() override { return false; }
  bool IsAdsBlockEnabledForCurPage() override { return false; }
  void SetAdBlockEnabledForSite(bool, int) override {}
#endif
#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  void ProcessAutofillCancel(const std::string&) override {}
#endif
#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
  void SetSavePasswordAutomatically(bool) override {}
  bool GetSavePasswordAutomatically() override { return false; }
  void SetSavePassword(bool) override {}
  bool GetSavePassword() override { return false; }
  void SaveOrUpdatePassword(bool) override {}
#endif
#if BUILDFLAG(ARKWEB_EXT_PASSWORD) || BUILDFLAG(ARKWEB_DATALIST)
  void PasswordSuggestionSelected(int) const override {}
#endif
#if BUILDFLAG(ARKWEB_EXT_FORCE_ZOOM) || BUILDFLAG(ARKWEB_ZOOM)
  void SetForceEnableZoom(bool) override {}
#endif
#if BUILDFLAG(ARKWEB_EXT_FORCE_ZOOM)
  bool GetForceEnableZoom() override { return false; }
#endif
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  void ShowFreeCopyMenu() override {}
  bool ShouldShowFreeCopyMenu() override { return false; }
  std::string GetSelectedTextFromContextParam() override { return std::string(); }
#endif
};

class NWebContentChangeDetectionTest : public ::testing::Test {
 public:
  void SetUp() override {
    mock_delegate_ = std::make_shared<MockNWebDelegate>();
    weak_delegate_ = mock_delegate_->WeakFromThis();
  }

  void TearDown() override {
    mock_delegate_.reset();
  }

  std::shared_ptr<MockNWebDelegate> mock_delegate_;
  base::WeakPtr<NWebDelegate> weak_delegate_;
};

// Test: SetContentChangeDetectionEnable with null delegate
TEST_F(NWebContentChangeDetectionTest, SetEnable_NullDelegate) {
  base::WeakPtr<NWebDelegate> null_delegate;
  NWebContentChangeDetection detection(null_delegate);

  // Should not crash when delegate is null
  detection.SetContentChangeDetectionEnable(true);
  detection.SetContentChangeDetectionEnable(false);
}

// Test: SetContentChangeDetectionEnable with valid delegate, enable = true
TEST_F(NWebContentChangeDetectionTest, SetEnable_EnableTrue) {
  NWebContentChangeDetection detection(weak_delegate_);

#if BUILDFLAG(ARKWEB_AI)
  EXPECT_CALL(*mock_delegate_,
              RegisterOnLoadStartedCbForContentChange(_))
      .Times(1);
#endif

  detection.SetContentChangeDetectionEnable(true);
}

// Test: SetContentChangeDetectionEnable with valid delegate, enable = false
TEST_F(NWebContentChangeDetectionTest, SetEnable_EnableFalse) {
  NWebContentChangeDetection detection(weak_delegate_);

#if BUILDFLAG(ARKWEB_AI)
  EXPECT_CALL(*mock_delegate_,
              RegisterOnLoadStartedCbForContentChange(_))
      .Times(1);
#endif

  detection.SetContentChangeDetectionEnable(false);
}

// Test: SetContentChangeDetectionConfig with default values
TEST_F(NWebContentChangeDetectionTest, SetConfig_DefaultValues) {
  NWebContentChangeDetection detection(weak_delegate_);

  detection.SetContentChangeDetectionConfig(100, 0.15f);
}

// Test: SetContentChangeDetectionConfig with custom values
TEST_F(NWebContentChangeDetectionTest, SetConfig_CustomValues) {
  NWebContentChangeDetection detection(weak_delegate_);

  detection.SetContentChangeDetectionConfig(500, 0.5f);
  detection.SetContentChangeDetectionConfig(1000, 0.8f);
  detection.SetContentChangeDetectionConfig(50, 0.05f);
}

// Test: SetContentChangeDetectionConfig with boundary values
TEST_F(NWebContentChangeDetectionTest, SetConfig_BoundaryValues) {
  NWebContentChangeDetection detection(weak_delegate_);

  // Test minimum values
  detection.SetContentChangeDetectionConfig(0, 0.0f);

  // Test maximum values
  detection.SetContentChangeDetectionConfig(INT32_MAX, 1.0f);

  // Test negative values (should be handled)
  detection.SetContentChangeDetectionConfig(-100, -0.5f);
}

// Test: SetContentChangeDetectionConfig with null delegate
TEST_F(NWebContentChangeDetectionTest, SetConfig_NullDelegate) {
  base::WeakPtr<NWebDelegate> null_delegate;
  NWebContentChangeDetection detection(null_delegate);

  // Should not crash when delegate is null
  detection.SetContentChangeDetectionConfig(100, 0.15f);
}

// Test: Multiple calls to SetContentChangeDetectionEnable
TEST_F(NWebContentChangeDetectionTest, SetEnable_MultipleCalls) {
  NWebContentChangeDetection detection(weak_delegate_);

#if BUILDFLAG(ARKWEB_AI)
  EXPECT_CALL(*mock_delegate_,
              RegisterOnLoadStartedCbForContentChange(_))
      .Times(3);
#endif

  detection.SetContentChangeDetectionEnable(true);
  detection.SetContentChangeDetectionEnable(false);
  detection.SetContentChangeDetectionEnable(true);
}

// Test: SetConfig before SetEnable
TEST_F(NWebContentChangeDetectionTest, SetConfig_BeforeSetEnable) {
  NWebContentChangeDetection detection(weak_delegate_);

  detection.SetContentChangeDetectionConfig(200, 0.3f);

#if BUILDFLAG(ARKWEB_AI)
  EXPECT_CALL(*mock_delegate_,
              RegisterOnLoadStartedCbForContentChange(_))
      .Times(1);
#endif

  detection.SetContentChangeDetectionEnable(true);
}

// Test: SetConfig after SetEnable
TEST_F(NWebContentChangeDetectionTest, SetConfig_AfterSetEnable) {
  NWebContentChangeDetection detection(weak_delegate_);

#if BUILDFLAG(ARKWEB_AI)
  EXPECT_CALL(*mock_delegate_,
              RegisterOnLoadStartedCbForContentChange(_))
      .Times(1);
#endif

  detection.SetContentChangeDetectionEnable(true);
  detection.SetContentChangeDetectionConfig(300, 0.4f);
}

// Test: Delegate becomes invalid during operation
TEST_F(NWebContentChangeDetectionTest, DelegateBecomesInvalid) {
  NWebContentChangeDetection detection(weak_delegate_);

#if BUILDFLAG(ARKWEB_AI)
  EXPECT_CALL(*mock_delegate_,
              RegisterOnLoadStartedCbForContentChange(_))
      .Times(1);
#endif

  detection.SetContentChangeDetectionEnable(true);

  // Invalidate the delegate
  mock_delegate_.reset();

  // Should handle invalid delegate gracefully
  detection.SetContentChangeDetectionConfig(100, 0.15f);
  detection.SetContentChangeDetectionEnable(false);
}

// Test: Zero min_report_time
TEST_F(NWebContentChangeDetectionTest, SetConfig_ZeroMinReportTime) {
  NWebContentChangeDetection detection(weak_delegate_);

  detection.SetContentChangeDetectionConfig(0, 0.15f);
}

// Test: Zero text_content_ratio
TEST_F(NWebContentChangeDetectionTest, SetConfig_ZeroTextContentRatio) {
  NWebContentChangeDetection detection(weak_delegate_);

  detection.SetContentChangeDetectionConfig(100, 0.0f);
}

// Test: Both parameters zero
TEST_F(NWebContentChangeDetectionTest, SetConfig_BothZero) {
  NWebContentChangeDetection detection(weak_delegate_);

  detection.SetContentChangeDetectionConfig(0, 0.0f);
}

// Test: Large min_report_time value
TEST_F(NWebContentChangeDetectionTest, SetConfig_LargeMinReportTime) {
  NWebContentChangeDetection detection(weak_delegate_);

  detection.SetContentChangeDetectionConfig(1000000, 0.15f);
}

// Test: Text content ratio at 1.0
TEST_F(NWebContentChangeDetectionTest, SetConfig_TextRatioAtOne) {
  NWebContentChangeDetection detection(weak_delegate_);

  detection.SetContentChangeDetectionConfig(100, 1.0f);
}

// Test: Text content ratio above 1.0
TEST_F(NWebContentChangeDetectionTest, SetConfig_TextRatioAboveOne) {
  NWebContentChangeDetection test(weak_delegate_);

  // Test with ratio > 1.0 (should be handled by the implementation)
  test.SetContentChangeDetectionConfig(100, 1.5f);
}

// Test: Very small text content ratio
TEST_F(NWebContentChangeDetectionTest, SetConfig_VerySmallTextRatio) {
  NWebContentChangeDetection detection(weak_delegate_);

  detection.SetContentChangeDetectionConfig(100, 0.001f);
}

// Test: Alternating enable/disable
TEST_F(NWebContentChangeDetectionTest, SetEnable_Alternating) {
  NWebContentChangeDetection detection(weak_delegate_);

#if BUILDFLAG(ARKWEB_AI)
  EXPECT_CALL(*mock_delegate_,
              RegisterOnLoadStartedCbForContentChange(_))
      .Times(5);
#endif

  detection.SetContentChangeDetectionEnable(true);
  detection.SetContentChangeDetectionEnable(false);
  detection.SetContentChangeDetectionEnable(true);
  detection.SetContentChangeDetectionEnable(false);
  detection.SetContentChangeDetectionEnable(true);
}

// Test: Config change while enabled
TEST_F(NWebContentChangeDetectionTest, SetConfig_WhileEnabled) {
  NWebContentChangeDetection detection(weak_delegate_);

#if BUILDFLAG(ARKWEB_AI)
  EXPECT_CALL(*mock_delegate_,
              RegisterOnLoadStartedCbForContentChange(_))
      .Times(1);
#endif

  detection.SetContentChangeDetectionEnable(true);
  detection.SetContentChangeDetectionConfig(200, 0.25f);
  detection.SetContentChangeDetectionConfig(300, 0.35f);
  detection.SetContentChangeDetectionConfig(400, 0.45f);
}

}  // namespace OHOS::NWeb
