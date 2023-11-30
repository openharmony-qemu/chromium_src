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

#ifndef NWEB_IMPL_H
#define NWEB_IMPL_H

#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>
#include <set>
#include <vector>
#include "capi/nweb_app_client_extension_callback.h"
#include "capi/nweb_download_delegate_callback.h"
#include "nweb.h"
#include "nweb_download_callback.h"
#include "nweb_errors.h"
#include "nweb_input_handler.h"
#include "nweb_inputmethod_handler.h"
#include "nweb_output_handler.h"

namespace OHOS::NWeb {
class NWebImpl : public NWeb {
 public:
  NWebImpl(uint32_t id);
  ~NWebImpl();

  bool Init(const NWebCreateInfo& create_info);
  void OnDestroy() override;

  /* event interface */
  void Resize(uint32_t width, uint32_t height, bool isKeyboard = false) override;
  void OnTouchPress(int32_t id, double x, double y, bool from_overlay) override;
  void OnTouchRelease(int32_t id,
                      double x,
                      double y,
                      bool from_overlay) override;
  void OnTouchMove(int32_t id, double x, double y, bool from_overlay) override;
  void OnTouchCancel() override;
  void OnNavigateBack() override;
  bool SendKeyEvent(int32_t keyCode, int32_t keyAction) override;
  void SendMouseWheelEvent(double x,
                           double y,
                           double deltaX,
                           double deltaY) override;
  void SendMouseEvent(int x, int y, int button, int action, int count) override;

  // public api
  int Load(const std::string& url) const override;
  bool IsNavigatebackwardAllowed() const override;
  bool IsNavigateForwardAllowed() const override;
  bool CanNavigateBackOrForward(int numSteps) const override;
  void NavigateBack() const override;
  void NavigateForward() const override;
  void NavigateBackOrForward(int step) const override;
  void DeleteNavigateHistory() override;
  void ClearSslCache() override;
  void ClearClientAuthenticationCache() override;
  void Reload() const override;
  int Zoom(float zoomFactor) const override;
  int ZoomIn() const override;
  int ZoomOut() const override;
  void Stop() const override;
  void ExecuteJavaScript(const std::string& code) const override;
  void PutBackgroundColor(int color) const override;
  void InitialScale(float scale) const override;
  void OnPause() const override;
  void OnContinue() const override;
  void OnOccluded() const override;
  void OnUnoccluded() const override;
  void SetEnableLowerFrameRate(bool enabled) const override;
  const std::shared_ptr<NWebPreference> GetPreference() const override;
  void PutDownloadCallback(
      std::shared_ptr<NWebDownloadCallback> downloadListener) override;
  void PutReleaseSurfaceCallback(std::shared_ptr<NWebReleaseSurfaceCallback>
                                     releaseSurfaceListener) override;
  void SetNWebHandler(std::shared_ptr<NWebHandler> handler) override;
  const std::shared_ptr<NWebHandler> GetNWebHandler() const override;
  std::string Title() override;
  uint32_t GetWebId() const override;
  HitTestResult GetHitTestResult() const override;
  int PageLoadProgress() override;
  int ContentHeight() override;
  float Scale() override;
  int Load(std::string& url,
           std::map<std::string, std::string> additionalHttpHeaders) override;
  int LoadWithDataAndBaseUrl(const std::string& baseUrl,
                             const std::string& data,
                             const std::string& mimeType,
                             const std::string& encoding,
                             const std::string& historyUrl) override;
  int LoadWithData(const std::string& data,
                   const std::string& mimeType,
                   const std::string& encoding) override;
  void RegisterArkJSfunction(
      const std::string& object_name,
      const std::vector<std::string>& method_list) override;
  void UnregisterArkJSfunction(
      const std::string& object_name,
      const std::vector<std::string>& method_list) override;
  void SetNWebJavaScriptResultCallBack(
      std::shared_ptr<NWebJavaScriptResultCallBack> callback) override;
  void OnFocus(const FocusReason& focusReason =
                   FocusReason::FOCUS_DEFAULT) const override;
  void OnBlur(const BlurReason& blurReason) const override;
  void StoreWebArchive(
      const std::string& base_name,
      bool auto_name,
      std::shared_ptr<NWebValueCallback<std::string>> callback) const override;

  void PutFindCallback(std::shared_ptr<NWebFindCallback> findListener) override;
  void FindAllAsync(const std::string& search_string) const override;
  void ClearMatches() const override;
  void FindNext(const bool forward) const override;
  const std::string GetOriginalUrl() const override;
  bool GetFavicon(const void** data,
                  size_t& width,
                  size_t& height,
                  ImageColorType& colorType,
                  ImageAlphaType& alphaType) override;
  void PutNetworkAvailable(bool available) override;
  void SendDragEvent(const DragEvent& dragEvent) const override;
  void UpdateLocale(const std::string& language,
                    const std::string& region) override;

  void HasImages(std::shared_ptr<NWebValueCallback<bool>> callback) override;
  void RemoveCache(bool include_disk_files) override;
#ifdef OHOS_SCREEN_LOCK
  void RegisterScreenLockFunction(int32_t windowId,
                                  const SetKeepScreenOn&& handle) override;
  void UnRegisterScreenLockFunction(int32_t windowId) override;
#endif  // #ifdef OHOS_SCREEN_LOCK

#if defined(OHOS_EX_PASSWORD)
  void PasswordSuggestionSelected(int list_index) const;
  void SetSavePasswordAutomatically(bool enable) const;
  bool GetSavePasswordAutomatically() const;
  void SetSavePassword(bool enable) const;
  bool GetSavePassword() const;
  void SaveOrUpdatePassword(bool is_update);
#endif
#if defined(OHOS_COMPOSITE_RENDER)
  void SetShouldFrameSubmissionBeforeDraw(bool should) override;
#endif  // defined(OHOS_COMPOSITE_RENDER)

#if defined(OHOS_MSGPORT)
  void ExecuteJavaScript(
      const std::string& code,
      std::shared_ptr<NWebValueCallback<std::shared_ptr<NWebMessage>>> callback,
      bool extention) const override;
  void CreateWebMessagePorts(std::vector<std::string>& ports) override;
  void PostWebMessage(std::string& message,
                      std::vector<std::string>& ports,
                      std::string& targetUri) override;
  void ClosePort(std::string& port_handle) override;
  void PostPortMessage(std::string& port_handle,
                       std::shared_ptr<NWebMessage> data) override;
  void SetPortMessageCallback(
      std::string& port_handle,
      std::shared_ptr<NWebValueCallback<std::shared_ptr<NWebMessage>>> callback)
      override;
#endif  // defined(OHOS_MSGPORT)
#ifdef OHOS_EX_BLANK_TARGET_POPUP_INTERCEPT
  void SetEnableBlankTargetPopupIntercept(bool enableBlankTargetPopup) const;
#endif

  std::shared_ptr<NWebHistoryList> GetHistoryList() override;
  WebState SerializeWebState() override;
  bool RestoreWebState(WebState state) override;
#ifdef OHOS_PAGE_UP_DOWN
  void PageUp(bool top) override;
  void PageDown(bool bottom) override;
#endif  // #endif OHOS_PAGE_UP_DOWN

#if defined(OHOS_INPUT_EVENTS)
  void ScrollTo(float x, float y) override;
  void ScrollBy(float delta_x, float delta_y) override;
  void SlideScroll(float vx, float vy) override;
#endif  // defined(OHOS_INPUT_EVENTS)

  bool GetCertChainDerData(std::vector<std::string>& certChainData,
                           bool isSingleCert) override;
  void SetScreenOffSet(double x, double y) override;
  void SetAudioMuted(bool muted) override;
  void SetAudioResumeInterval(int32_t resumeInterval) override;
  void SetAudioExclusive(bool audioExclusive) override;
  void NotifyMemoryLevel(int32_t level) override;
  void OnWebviewHide() const override;
  void OnWebviewShow() const override;
#ifdef OHOS_DRAG_DROP
  std::shared_ptr<NWebDragData> GetOrCreateDragData() override;
#endif // #ifdef OHOS_DRAG_DROP

#if BUILDFLAG(IS_OHOS)
  void SetWindowId(uint32_t window_id) override;
  void SetToken(void* token) override;
  void SetNestedScrollMode(const NestedScrollMode& nestedScrollMode) override;
#endif

#if defined(OHOS_NO_STATE_PREFETCH)
  void PrefetchPage(
      std::string& url,
      std::map<std::string, std::string> additionalHttpHeaders) override;
#endif  // defined(OHOS_NO_STATE_PREFETCH)

  // For NWebEx
  static NWebImpl* FromID(int32_t nweb_id);
  std::string GetUrl() const override;

  CefRefPtr<CefClient> GetCefClient() const {
    return nweb_delegate_ ? nweb_delegate_->GetCefClient() : nullptr;
  }
  void AddNWebToMap(uint32_t id, std::shared_ptr<NWebImpl>& nweb);
#if defined (OHOS_NWEB_EX)
  static const std::vector<std::string>& GetCommandLineArgsForNWebEx();
  static void InitBrowserServiceApi(std::vector<std::string>& browser_args);
  static bool GetBrowserServiceApiEnabled();

  void PutWebAppClientExtensionCallback(
      std::shared_ptr<NWebAppClientExtensionCallback>
          web_app_client_extension_listener);
  void RemoveWebAppClientExtensionCallback();

  void ReloadOriginalUrl() const;
  void SetBrowserUserAgentString(const std::string& user_agent);
#endif  // defined(OHOS_NWEB_EX)

#if defined(OHOS_EX_FORCE_ZOOM)
  void SetForceEnableZoom(bool forceEnableZoom) const;
  bool GetForceEnableZoom() const;
#endif  // OHOS_EX_FORCE_ZOOM

#if defined(OHOS_EX_DOWNLOAD)
  void PutWebDownloadDelegateCallback(
      std::shared_ptr<NWebDownloadDelegateCallback>);
  void StartDownload(const char* url);
  void ResumeDownload(std::shared_ptr<NWebDownloadItem>);
  static void ResumeDownloadStatic(std::shared_ptr<NWebDownloadItem> download_item);
#endif  // OHOS_EX_DOWNLOAD

#if defined(OHOS_MULTI_WINDOW)
  void NotifyPopupWindowResult(bool result) override {
    nweb_delegate_->NotifyPopupWindowResult(result);
  }
#endif  // defined(OHOS_MULTI_WINDOW)

#if defined(OHOS_EX_FREE_COPY)
  void SelectAndCopy() const;
  bool ShouldShowFreeCopy() const;
#endif  // defined(OHOS_EX_FREE_COPY)
 private:
  void ProcessInitArgs(const NWebInitArgs& init_args);
  void InitWebEngineArgs(const NWebInitArgs& init_args);
  bool InitWebEngine(const NWebCreateInfo& create_info);
  bool SetVirtualDeviceRatio();
#if defined(OHOS_WEBRTC)
  void StopCameraSession() const;
  void RestartCameraSession() const;
#endif // defined(OHOS_WEBRTC)

 private:
  uint32_t nweb_id_ = 0;
  std::shared_ptr<NWebHandler> nweb_handle_ = nullptr;
  std::shared_ptr<NWebOutputHandler> output_handler_ = nullptr;
  std::shared_ptr<NWebInputHandler> input_handler_ = nullptr;
  CefRefPtr<NWebInputMethodHandler> inputmethod_handler_ = nullptr;

  std::shared_ptr<NWebDelegateInterface> nweb_delegate_ = nullptr;
  std::list<std::string> web_engine_args_;
  float device_pixel_ratio_ = 0.f;
  bool is_enhance_surface_ = false;
};
}  // namespace OHOS::NWeb

#endif  // NWEB_IMPL_H
