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

#ifndef NWEB_DELEGATE_INTERFACE_H
#define NWEB_DELEGATE_INTERFACE_H

#include <functional>
#include <string>
#include "capi/nweb_app_client_extension_callback.h"
#include "capi/nweb_download_delegate_callback.h"
#include "cef/include/cef_client.h"
#include "cef_delegate/nweb_inputmethod_client.h"
#include "display_manager_adapter.h"
#include "nweb_download_callback.h"
#include "nweb_find_callback.h"
#include "nweb_handler.h"
#include "nweb_preference.h"
#include "nweb_web_message.h"

namespace OHOS::NWeb {
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
  double x;
  double y;
  DelegateDragAction action;
};

using WebState = std::shared_ptr<std::vector<uint8_t>>;

class NWebDelegateInterface
    : public std::enable_shared_from_this<NWebDelegateInterface> {
 public:
  virtual ~NWebDelegateInterface() = default;

  virtual bool IsReady() = 0;
  virtual void OnDestroy(bool is_close_all) = 0;
  virtual void RegisterDownLoadListener(
      std::shared_ptr<NWebDownloadCallback> downloadListener) = 0;
  virtual void RegisterReleaseSurfaceListener(
      std::shared_ptr<NWebReleaseSurfaceCallback> releaseSurfaceListener) = 0;
  virtual void RegisterNWebHandler(std::shared_ptr<NWebHandler> handler) = 0;
  virtual void RegisterRenderCb(
      std::function<void(const char*)> render_update_cb) = 0;
  virtual void RegisterWebAppClientExtensionListener(
      std::shared_ptr<NWebAppClientExtensionCallback>
          web_app_client_extension_listener) = 0;
  virtual void UnRegisterWebAppClientExtensionListener() = 0;
  virtual void RegisterWebDownloadDelegateListener(
      std::shared_ptr<NWebDownloadDelegateCallback>
          downloadDelegateListener) = 0;
  virtual void StartDownload(const char* url) = 0;
  virtual void ResumeDownload(
      std::shared_ptr<NWebDownloadItem> web_download) = 0;
  virtual void SetInputMethodClient(
      CefRefPtr<NWebInputMethodClient> client) = 0;
  virtual void SetNWebDelegateInterface(
      std::shared_ptr<NWebDelegateInterface> client) = 0;

  /* event interface */
  virtual void Resize(uint32_t width,
                      uint32_t height,
                      bool isKeyboard = false) = 0;
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
  virtual void OnTouchCancel() = 0;
  virtual bool SendKeyEvent(int32_t keyCode, int32_t keyAction) = 0;
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
                                       OrientationType orientation) = 0;

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
  virtual void PasswordSuggestionSelected(int list_index) const = 0;
  virtual int Zoom(float zoomFactor) const = 0;
  virtual int ZoomIn() const = 0;
  virtual int ZoomOut() const = 0;
  virtual void Stop() const = 0;
  virtual void ExecuteJavaScript(const std::string& code) const = 0;
  virtual void ExecuteJavaScript(
      const std::string& code,
      std::shared_ptr<NWebValueCallback<std::shared_ptr<NWebMessage>>> callback,
      bool extention) const = 0;
  virtual void PutBackgroundColor(int color) const = 0;
  virtual void InitialScale(float scale) const = 0;
  virtual void OnPause() = 0;
  virtual void OnContinue() = 0;
  virtual std::shared_ptr<NWebPreference> GetPreference() const = 0;
  virtual std::string Title() = 0;
  virtual void CreateWebMessagePorts(std::vector<std::string>& ports) = 0;
  virtual void PostWebMessage(std::string& message,
                              std::vector<std::string>& ports,
                              std::string& targetUri) = 0;
  virtual void ClosePort(std::string& portHandle) = 0;
  virtual void PostPortMessage(std::string& portHandle, std::shared_ptr<NWebMessage> data) = 0;
  virtual void SetPortMessageCallback(std::string& portHandle,
      std::shared_ptr<NWebValueCallback<std::shared_ptr<NWebMessage>>> callback) = 0;
  virtual HitTestResult GetHitTestResult() const = 0;
  virtual int PageLoadProgress() = 0;
  virtual float Scale() = 0;
  virtual int Load(
      std::string& url,
      std::map<std::string, std::string> additionalHttpHeaders) = 0;
  virtual int LoadWithDataAndBaseUrl(const std::string& baseUrl,
                                      const std::string& data,
                                      const std::string& mimeType,
                                      const std::string& encoding,
                                      const std::string& historyUrl) = 0;
  virtual int LoadWithData(const std::string& data,
                            const std::string& mimeType,
                            const std::string& encoding) = 0;
  virtual int ContentHeight() = 0;
  virtual void RegisterArkJSfunction(
      const std::string& object_name,
      const std::vector<std::string>& method_list) const = 0;
  virtual void UnregisterArkJSfunction(
      const std::string& object_name,
      const std::vector<std::string>& method_list) const = 0;
  virtual void RegisterNWebJavaScriptCallBack(
      std::shared_ptr<NWebJavaScriptResultCallBack> callback) = 0;
  virtual bool OnFocus(const FocusReason& focusReason = FocusReason::FOCUS_DEFAULT) const = 0;
  virtual void OnBlur() const = 0;
  virtual void RegisterFindListener(
      std::shared_ptr<NWebFindCallback> find_listener) = 0;
  virtual void FindAllAsync(const std::string& str) const = 0;
  virtual void ClearMatches() const = 0;
  virtual void FindNext(const bool forward) const = 0;
  virtual void UpdateLocale(const std::string& language, const std::string& region) = 0;

#if defined(REPORT_SYS_EVENT)
  virtual void SetNWebId(uint32_t nwebId) = 0;
#endif

  virtual void StoreWebArchive(
      const std::string& base_name,
      bool auto_name,
      std::shared_ptr<NWebValueCallback<std::string>> callback) const = 0;

  virtual void SetBrowserUserAgentString(const std::string& user_agent) = 0;

  virtual void SendDragEvent(const DelegateDragEvent& dragEvent) const = 0;
  virtual std::shared_ptr<NWebDragData> GetOrCreateDragData() = 0;
  virtual std::string GetUrl() const = 0;
  virtual const std::string GetOriginalUrl() = 0;
  virtual bool GetFavicon(const void** data, size_t& width, size_t& height,
      ImageColorType& colorType, ImageAlphaType& alphaType) = 0;
  virtual void PutNetworkAvailable(bool available) = 0;

  virtual CefRefPtr<CefClient> GetCefClient() const = 0;

  virtual void GetImages(std::shared_ptr<NWebValueCallback<bool>> callback) = 0;
  virtual void RemoveCache(bool include_disk_files) = 0;
  virtual std::shared_ptr<NWebHistoryList> GetHistoryList() = 0;
  virtual WebState SerializeWebState() = 0;
  virtual bool RestoreWebState(WebState state) = 0;
  virtual void PageUp(bool top) = 0;
  virtual void PageDown(bool bottom) = 0;
  virtual void ScrollTo(float x, float y) = 0;
  virtual void ScrollBy(float delta_x, float delta_y) = 0;
  virtual void SlideScroll(float vx, float vy) = 0;
  virtual bool GetCertChainDerData(std::vector<std::string>& certChainData, bool isSingleCert) = 0;
  virtual void SetAudioMuted(bool muted) = 0;
  virtual void PrefetchPage(
      std::string& url,
      std::map<std::string, std::string> additionalHttpHeaders) = 0;
  virtual void SetVirtualPixelRatio(float ratio) = 0;

#if defined (OHOS_NWEB_EX)
  virtual void SetForceEnableZoom(bool forceEnableZoom) = 0;
  virtual bool GetForceEnableZoom() = 0;
  virtual void SelectAndCopy() = 0;
  virtual bool ShouldShowFreeCopy() = 0;
  virtual void SetEnableBlankTargetPopupIntercept(bool enableBlankTargetPopup) = 0;

  virtual void SetSavePasswordAutomatically(bool enable) = 0;
  virtual bool GetSavePasswordAutomatically() = 0;
  virtual void SetSavePassword(bool enable) = 0;
  virtual bool GetSavePassword() = 0;
  virtual void SaveOrUpdatePassword(bool is_update) = 0;
#endif

  virtual void SetShouldFrameSubmissionBeforeDraw(bool should) = 0;
  virtual void SetAudioResumeInterval(int32_t resumeInterval) = 0;
  virtual void SetAudioExclusive(bool audioExclusive) = 0;
  virtual void NotifyPopupWindowResult(bool result) = 0;
  virtual void SetWindowId(uint32_t window_id);
  virtual void SetToken(void* token) = 0;
};
}  // namespace OHOS::NWeb

#endif  // nweb_delegate_INTERFACE_H
