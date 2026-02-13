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

#ifndef NWEB_IMPL_H
#define NWEB_IMPL_H

#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>
#include <set>
#include <vector>

#include "arkweb/build/features/features.h"
#include "build/build_config.h"
#include "base/memory/raw_ptr.h"
#include "base/timer/timer.h"
#include "capi/nweb_app_client_extension_callback.h"
#include "capi/nweb_download_delegate_callback.h"
#include "capi/nweb_extension_api_callback.h"
#include "capi/nweb_extension_javascript_item.h"
#include "capi/nweb_extension_load_url_params.h"
#include "nweb.h"
#include "nweb_download_callback.h"
#include "nweb_errors.h"
#include "nweb_input_handler.h"
#include "nweb_inputmethod_handler.h"
#include "nweb_output_handler.h"
#include "arkweb/ohos_nweb/src/cef_delegate/nweb_extension_connect_native.h"
// #ifdef OHOS_EX_PERMISSION
#include "capi/nweb_permission_request.h"
// #endif

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#if BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)
#include "capi/nweb_screencapture_delegate_callback.h"
#endif  // defined(ARKWEB_EX_SCREEN_CAPTURE)

#include "nweb_proxy_changed_callback.h"

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
#include "capi/nweb_statistic_callback.h"
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
#include "arkweb/ohos_adapter_ndk/distributeddatamgr_adapter/ohos_web_snapshot_data_base.h"
#endif

struct OpenDevToolsParam;
struct OpenDevToolsExtOpt;
struct RunJavaScriptParam;

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#include "capi/nweb_extension_manager_callback.h"
#include "capi/nweb_extension_context_menus_callback.h"
#include "capi/nweb_offscreen_document_callback.h"
#include "capi/nweb_offscreen_document_permission_request_callback.h"
#include "capi/web_extension_tab_items.h"
#include "ohos_nweb/src/capi/nweb_context_menus_on_clicked_data.h"
#include "ohos_nweb/src/capi/web_extension_install_crx_items.h"
#endif // ARKWEB_ARKWEB_EXTENSIONS

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "ohos_nweb_ex/core/extension/nweb_app_client_extension_dispatcher.h"
#endif

#if BUILDFLAG(ARKWEB_USERAGENT)
#include "nweb_user_agent_metadata.h"
#endif

#if BUILDFLAG(ARKWEB_READER_MODE)
#include "capi/nweb_extension_distill_item.h"
#endif // ARKWEB_READER_MODE

namespace OHOS::NWeb {
#if BUILDFLAG(IS_ARKWEB)
class NWebPrintDocumentAdapterAdapterImpl :
    public NWebPrintDocumentAdapterAdapter {
public:
    explicit NWebPrintDocumentAdapterAdapterImpl(
        NWebPrintDocumentAdapterAdapter* ref) : ref_(ref) {}
    ~NWebPrintDocumentAdapterAdapterImpl();

    void OnStartLayoutWrite(const std::string& jobId,
        std::shared_ptr<NWebPrintAttributesAdapter> oldAttrs,
        std::shared_ptr<NWebPrintAttributesAdapter> newAttrs, uint32_t fd,
        std::shared_ptr<NWebPrintWriteResultCallbackAdapter> callback) override;

    void OnJobStateChanged(const std::string& jobId, uint32_t state) override;
private:
    raw_ptr<NWebPrintDocumentAdapterAdapter> ref_;
};
#endif

class NWebImpl : public NWeb {
 public:
  explicit NWebImpl(uint32_t id);
  ~NWebImpl() override;

  bool Init(std::shared_ptr<NWebCreateInfo> create_info);
  void OnDestroy() override;
#if BUILDFLAG(ARKWEB_PERFORMANCE_INC_FREQ)
  static void DisableBoost(uint32_t nweb_id);
#endif

#if BUILDFLAG(ARKWEB_GPU)
  static void UpdateGpuConfig(bool gpu_switch);
#endif // BUILDFLAG(ARKWEB_GPU)
  static void UpdateInprocessGpuArg(std::list<std::string>& web_engine_args,
                                    bool xml_gpu);

  /* event interface */
  void Resize(uint32_t width,
              uint32_t height,
              bool isKeyboard = false) override;
  void ResizeVisibleViewport(uint32_t width,
                             uint32_t height,
                             bool isKeyboard) override;
  void OnTouchPress(int32_t id, double x, double y, bool from_overlay) override;
  void OnTouchRelease(int32_t id,
                      double x,
                      double y,
                      bool from_overlay) override;
  void OnTouchMove(int32_t id, double x, double y, bool from_overlay) override;
  void OnTouchMove(
      const std::vector<std::shared_ptr<NWebTouchPointInfo>>& touch_point_infos,
      bool fromOverlay = false) override;
  void OnStylusTouchPress(
      std::shared_ptr<NWebStylusTouchPointInfo> stylus_touch_point_info,
      bool from_overlay) override;
  void OnStylusTouchRelease(
      std::shared_ptr<NWebStylusTouchPointInfo> stylus_touch_point_info,
      bool from_overlay) override;
  void OnStylusTouchMove(
      const std::vector<std::shared_ptr<NWebStylusTouchPointInfo>>& stylus_touch_point_infos,
      bool from_overlay = false) override;
  void OnTouchCancel() override;
  void OnNavigateBack() override;
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
  void FillAutofillData(std::shared_ptr<NWebMessage> data) override;
  void FillAutofillDataV2(std::shared_ptr<NWebRomValue> data) override;
  void FillAutofillDataFromTriggerType(
      std::shared_ptr<NWebRomValue> data, const NWebAutoFillTriggerType& type) override;
  void PutVaultPlainTextCallback(
      std::shared_ptr<NWebVaultPlainTextCallback> callback) override;
  void OnAutofillCancel(const std::string& fillContent) override;
  void SetNwebDelegateForTest(std::shared_ptr<NWebEngineInitArgs> init_args);

  // public api
  int Load(const std::string& url) override;
  bool IsNavigatebackwardAllowed() override;
  bool IsNavigateForwardAllowed() override;
  bool CanNavigateBackOrForward(int numSteps) override;
  void NavigateBack() override;
  void NavigateForward() override;
  void NavigateBackOrForward(int step) override;
  void DeleteNavigateHistory() override;
  void ClearSslCache() override;
  void ClearClientAuthenticationCache() override;
  void Reload() override;
  int Zoom(float zoomFactor) override;
  int ZoomIn() override;
  int ZoomOut() override;
  void Stop() override;
  void ExecuteJavaScript(const std::string& code) override;
  void PutBackgroundColor(int color) override;
  void InitialScale(float scale) override;
  void PutOptimizeParserBudgetEnabled(bool enable) override;
  void OnPause() override;
  void OnContinue() override;
  void OnDragAttach() override;
  void WebComponentsBlur() override;
  void OnOccluded() override;
  void OnUnoccluded() override;
  void SetEnableLowerFrameRate(bool enabled) override;
  void SetEnableHalfFrameRate(bool enabled) override;
  std::shared_ptr<NWebPreference> GetPreference() override;
#if BUILDFLAG(ARKWEB_AI)
  std::shared_ptr<NWebAgentManager> GetAgentManager() override;
#endif  // BUILDFLAG(ARKWEB_AI)
  void PutDownloadCallback(
      std::shared_ptr<NWebDownloadCallback> downloadListener) override;
  void PutReleaseSurfaceCallback(std::shared_ptr<NWebReleaseSurfaceCallback>
                                     releaseSurfaceListener) override;
  void SetNWebHandler(std::shared_ptr<NWebHandler> handler) override;
#if BUILDFLAG(ARKWEB_AI)
  void SetNWebAgentHandler(std::shared_ptr<NWebAgentHandler> handler) override;
#endif
  std::string Title() override;
  uint32_t GetWebId() override;
  std::shared_ptr<HitTestResult> GetHitTestResult() override;
  std::shared_ptr<HitTestResult> GetLastHitTestResult() override;
  int PageLoadProgress() override;
  int ContentHeight() override;
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
                        int32_t transition_type);
#endif

  void RegisterNativeArkJSFunction(
      const char* objName,
      const std::vector<std::shared_ptr<NWebJsProxyCallback>>& callbacks)
      override;
  using NativeJSProxyCallbackFunc =
      std::function<char*(std::vector<std::vector<uint8_t>>&,
                          std::vector<size_t>&)>;
  using NativeJSProxyCallbackFuncWithResult =
      std::function<std::shared_ptr<OHOS::NWeb::NWebValue>(
          std::vector<std::vector<uint8_t>>&,
          std::vector<size_t>&)>;
  void RegisterNativeArkJSFunction(
      const std::string& objName,
      const std::vector<std::string>& methodName,
      std::vector<NativeJSProxyCallbackFunc>&& callback,
      bool isAsync,
      const std::string& permission);
  void RegisterNativeArkJSFunctionWithResult(
      const std::string& objName,
      const std::vector<std::string>& methodName,
      std::vector<NativeJSProxyCallbackFuncWithResult>&& callback,
      bool isAsync,
      const std::string& permission);
  void RegisterNativeAsyncThreadArkJSFunctionWithResult(
      const std::string& objName,
      const std::vector<std::string>& methodName,
      std::vector<NativeJSProxyCallbackFuncWithResult>&& callback,
      const std::string& permission);
  void UnRegisterNativeArkJSFunction(const char* objName) override;
  void RegisterNativeValideCallback(
      const char* webName,
      const NativeArkWebOnValidCallback callback) override;
  void RegisterNativeDestroyCallback(
      const char* webName,
      const NativeArkWebOnDestroyCallback callback) override;
  void RegisterNativeDestroyCallback(std::function<void(void)>&& callback);
  void RegisterNativeLoadStartCallback(std::function<void(void)>&& callback);
  void RegisterNativeLoadEndCallback(std::function<void(void)>&& callback);
  void RegisterNativeScrollCallback(
      std::function<void(double, double)>&& callback);

  void RegisterArkJSfunction(const std::string& object_name,
                             const std::vector<std::string>& method_list,
                             const int32_t object_id) override;
  void RegisterArkJSfunction(const std::string& object_name,
                             const std::vector<std::string>& method_list,
                             const std::vector<std::string>& async_method_list,
                             const int32_t object_id) override;
  void RegisterArkJSfunctionV2(
      const std::string& object_name,
      const std::vector<std::string>& method_list,
      const std::vector<std::string>& async_method_list,
      const int32_t object_id,
      const std::string& permission) override;
  void UnregisterArkJSfunction(
      const std::string& object_name,
      const std::vector<std::string>& method_list) override;
  void CallH5Function(
      int32_t routing_id,
      int32_t h5_object_id,
      const std::string& h5_method_name,
      const std::vector<std::shared_ptr<NWebValue>>& args) override;
  void CallH5FunctionV2(
      int32_t routing_id,
      int32_t h5_object_id,
      const std::string& h5_method_name,
      const std::vector<std::shared_ptr<NWebRomValue>>& args) override;
  void SetNWebJavaScriptResultCallBack(
      std::shared_ptr<NWebJavaScriptResultCallBack> callback) override;
  void OnFocus(
      const FocusReason& focusReason = FocusReason::FOCUS_DEFAULT) override;
  void OnBlur(const BlurReason& blurReason) override;
  void StoreWebArchive(
      const std::string& base_name,
      bool auto_name,
      std::shared_ptr<NWebStringValueCallback> callback) override;

  void PutFindCallback(std::shared_ptr<NWebFindCallback> findListener) override;
  void FindAllAsync(const std::string& search_string) override;
  void ClearMatches() override;
  void FindNext(const bool forward) override;
  const std::string GetOriginalUrl() override;
  bool GetFavicon(const void** data,
                  size_t& width,
                  size_t& height,
                  ImageColorType& colorType,
                  ImageAlphaType& alphaType) override;
  void PutNetworkAvailable(bool available) override;
  void SendDragEvent(std::shared_ptr<NWebDragEvent> dragEvent) override;
  void SendDragOverEvent();
#if BUILDFLAG(ARKWEB_I18N)
  void UpdateLocale(const std::string& language,
                    const std::string& region) override;
#endif

  void HasImages(std::shared_ptr<NWebBoolValueCallback> callback) override;
  void RemoveCache(bool include_disk_files) override;
  void PutAccessibilityEventCallback(
      std::shared_ptr<NWebAccessibilityEventCallback>
          accessibilityEventListener) override;
  void PutAccessibilityIdGenerator(
      const AccessibilityIdGenerateFunc accessibilityIdGenerator) override;
  void ExecuteAction(int64_t accessibilityId, uint32_t action) override;
  void PerformAction(
      int64_t accessibilityId,
      uint32_t action,
      const std::map<std::string, std::string>& actionArguments) override;
  bool PerformActionV2(
      int64_t accessibilityId,
      uint32_t action,
      const std::map<std::string, std::string>& actionArguments) override;
  void SendAccessibilityHoverEvent(int32_t x, int32_t y) override;
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
  void SetAccessibilityState(bool state) override;
#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_CORNER)
  void SetBorderRadiusFromWeb(double borderRadiusTopLeft,
                              double borderRadiusTopRight,
                              double borderRadiusBottomLeft,
                              double borderRadiusBottomRight) override;
#endif  // ARKWEB_SCROLLBAR_AVOID_CORNER
#if BUILDFLAG(ARKWEB_MENU)
  void UpdateSingleHandleVisible(bool isVisible) override;
#endif
  void SuggestionSelected(int index) override;
#if BUILDFLAG(ARKWEB_DISCARD)
  bool Discard() override;
  bool Restore() override;
#endif
#if BUILDFLAG(ARKWEB_MENU)
  void SetTouchHandleExistState(bool touchHandleExist) override;
  void SetViewportScaleState() override;
#endif  // BUILDFLAG(ARKWEB_MENU)
#if BUILDFLAG(ARKWEB_CLIPBOARD)
  void PutSpanstringConvertHtmlCallback(
      std::shared_ptr<NWebSpanstringConvertHtmlCallback> callback) override;
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)
#if BUILDFLAG(ARKWEB_SCREEN_LOCK)
  void RegisterScreenLockFunction(
      int32_t windowId,
      std::shared_ptr<NWebScreenLockCallback> callback) override;
  void UnRegisterScreenLockFunction(int32_t windowId) override;
#endif  // #if BUILDFLAG(ARKWEB_SCREEN_LOCK)

#if BUILDFLAG(ARKWEB_ADBLOCK)
  static void AddAdsBlockDisallowList(
      const std::vector<std::string>& domain_suffixes);
  static void AddAdsBlockAllowList(
      const std::vector<std::string>& domainSuffixes);
  static void SetAdsBlockRules(const std::string& rulesFiles,
                               const bool replace);
  static void RemoveAdsBlockDisallowedList(
      const std::vector<std::string>& domainSuffixes);
  static void RemoveAdsBlockAllowedList(
      const std::vector<std::string>& domainSuffixes);
  static void ClearAdsBlockDisallowedList();
  static void ClearAdsBlockAllowedList();
  void EnableAdsBlock(bool enable) override;
  bool IsAdsBlockEnabled() override;
  bool IsAdsBlockEnabledForCurPage() override;
  static bool IsAnyNWebAdblockEnabled();
  void UpdateAdblockEasyListRules(long adBlockEasyListVersion);
  void SetAdBlockEnabledForSite(bool is_adblock_enabled,
                                int main_frame_tree_node_id);
#endif

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
  void SetSavePasswordAutomatically(bool enable) const;
  bool GetSavePasswordAutomatically() const;
  void SetSavePassword(bool enable) const;
  bool GetSavePassword() const;
  void SaveOrUpdatePassword(bool is_update);
#endif
#if BUILDFLAG(ARKWEB_EXT_PASSWORD) || BUILDFLAG(ARKWEB_DATALIST)
  void PasswordSuggestionSelected(int list_index) const;
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
#endif

  void ExecuteJavaScriptExt(const int fd,
                            const size_t scriptLength,
                            std::shared_ptr<NWebMessageValueCallback> callback,
                            bool extention) override;
#if BUILDFLAG(ARKWEB_PDF)
  void ExecuteCreatePDFExt(
      std::shared_ptr<NWebPDFConfigArgs> pdfConfig,
      std::shared_ptr<NWebArrayBufferValueCallback> callback) override;
#endif  // BUILDFLAG(ARKWEB_PDF)

#if BUILDFLAG(ARKWEB_MSGPORT)
  void ExecuteJavaScript(const std::string& code,
                         std::shared_ptr<NWebMessageValueCallback> callback,
                         bool extention) override;
  std::vector<std::string> CreateWebMessagePorts() override;
  void PostWebMessage(const std::string& message,
                      const std::vector<std::string>& ports,
                      const std::string& targetUri) override;
  void ClosePort(const std::string& port_handle) override;
  void PostPortMessage(const std::string& port_handle,
                       std::shared_ptr<NWebMessage> data) override;
  void PostPortMessageV2(const std::string& port_handle,
                         std::shared_ptr<NWebRomValue> data) override;
  void SetPortMessageCallback(
      const std::string& port_handle,
      std::shared_ptr<NWebMessageValueCallback> callback) override;
#endif  // BUILDFLAG(ARKWEB_MSGPORT)

  void SetAutofillCallback(
      std::shared_ptr<NWebMessageValueCallback> callback) override;
  std::shared_ptr<NWebHistoryList> GetHistoryList() override;
  std::vector<uint8_t> SerializeWebState() override;
  bool RestoreWebState(const std::vector<uint8_t>& state) override;

#if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)
  void PageUp(bool top) override;
  void PageDown(bool bottom) override;
#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
  void GetScrollOffset(float* offset_x, float* offset_y) override;
  void GetPageOffset(float* offset_x, float* offset_y) override;
#endif
#endif  // #if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
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
  bool WebSendMouseWheelEventV2(
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
  void WebSendCancelFlingEvent() override;
  bool SendKeyboardEvent(const std::shared_ptr<OHOS::NWeb::NWebKeyboardEvent>&
                             keyboardEvent) override;
  bool ScrollByWithResult(float delta_x, float delta_y) override;
  void ScrollToWithAnime(float x, float y, int32_t duration) override;
  void ScrollByWithAnime(float delta_x,
                         float delta_y,
                         int32_t duration) override;
  void WebSendMouseEvent(
      const std::shared_ptr<OHOS::NWeb::NWebMouseEvent>& mouseEvent) override;
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

  bool GetCertChainDerData(std::vector<std::string>& certChainData,
                           bool isSingleCert) override;
  void SetScreenOffSet(double x, double y) override;
  void SetAudioMuted(bool muted) override;
  void SetAudioResumeInterval(int32_t resumeInterval) override;

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
  void CloseAllMediaPresentations() override;
  void StopAllMedia() override;
  void ResumeAllMedia() override;
  void PauseAllMedia() override;
  int GetMediaPlaybackState() override;
#endif  // BUILDFLAG(ARKWEB_MEDIA_POLICY)

  void SetAudioExclusive(bool audioExclusive) override;
  void SetAudioSessionType(int32_t audioSessionType) override;
  void NotifyMemoryLevel(int32_t level) override;
  void SetIsOfflineWebComponent() override;
  void OnWebviewHide() override;
  void OnWebviewShow() override;
  void StartCamera() override;
  void StopCamera() override;
  void CloseCamera() override;
  void OnRenderToBackground() override;
  void OnRenderToForeground() override;

  void OnOnlineRenderToForeground() override;

  void NotifyForNextTouchEvent() override;

  bool IsActivePolicyDisable() override;

  static void DisableWebActivePolicy();

  WebDestroyMode GetWebDestroyMode() override;

  static void SetWebDestroyMode(WebDestroyMode mode);

  static ScrollbarMode GetScrollbarMode();
  static void SetScrollbarMode(ScrollbarMode mode);
  static bool IsScrollbarModeChanged();

  void SetDelayDurationForBackgroundTabFreezing(int64_t delay);
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  std::shared_ptr<NWebDragData> GetOrCreateDragData() override;
#endif  // BUILDFLAG(ARKWEB_DRAG_DROP)

#ifdef BUILDFLAG(IS_OHOS)
  bool TerminateRenderProcess() override;
#endif

#if BUILDFLAG(ARKWEB_SAFEBROWSING)
  bool IsSafeBrowsingEnabled() override;
  void EnableSafeBrowsing(bool enable) override;
  void EnableSafeBrowsingDetection(bool enable, bool strictMode) const;
  void OnSafeBrowsingDetectionResult(int code,
                                     int policy,
                                     const std::string& mappingType,
                                     const std::string& url);
#endif  // BUILDFLAG(ARKWEB_SAFEBROWSING)

#if BUILDFLAG(IS_OHOS)
  void SetWindowId(uint32_t window_id) override;
  void SetFocusWindowId(uint32_t focus_window_id) override;
  void SetToken(void* token) override;
  void* CreateWebPrintDocumentAdapter(const std::string& jobName) override;
  std::unique_ptr<NWebPrintDocumentAdapterAdapter>
      CreateWebPrintDocumentAdapterV2(const std::string& jobName) override;
  void SetNestedScrollMode(const NestedScrollMode& nestedScrollMode) override;
  int GetSecurityLevel() override;
  void SetPrintBackground(bool enable) override;
  bool GetPrintBackground() override;
  void PrecompileJavaScript(
      const std::string& url,
      const std::string& script,
      std::shared_ptr<CacheOptions>& cacheOptions,
      std::shared_ptr<NWebMessageValueCallback> callback) override;
  void InjectOfflineResource(
      const std::string& url,
      const std::string& origin,
      const std::vector<uint8_t>& resource,
      const std::map<std::string, std::string>& responseHeaders,
      const int type) override;
#endif

  std::string GetLastJavascriptProxyCallingFrameUrl() override;

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void SetVirtualKeyBoardArg(int32_t width,
                             int32_t height,
                             double keyboard) override;
  bool ShouldVirtualKeyboardOverlay() override;
#endif

#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
  void PrefetchPage(
      const std::string& url,
      const std::map<std::string, std::string>& additionalHttpHeaders) override;
  void PrefetchPageV2(
      const std::string& url,
      const std::map<std::string, std::string>& additionalHttpHeaders,
      int32_t min_time_between_prefetches, 
      bool ignore_cache_control_no_store) override;
#endif  // BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)

  int PostUrl(const std::string& url,
              const std::vector<char>& postData) override;

#if BUILDFLAG(ARKWEB_JSPROXY)
  void JavaScriptOnDocumentStart(const ScriptItems& scriptItems) override;
  void JavaScriptOnDocumentEnd(const ScriptItems& scriptItems) override;
  void JavaScriptOnDocumentStartByOrder(
      const ScriptItems& scriptItems,
      const ScriptItemsByOrder& scriptItemsByOrder) override;
  void JavaScriptOnDocumentEndByOrder(
      const ScriptItems& scriptItems,
      const ScriptItemsByOrder& scriptItemsByOrder) override;
  void JavaScriptOnHeadReadyByOrder(
      const ScriptItems& scriptItems,
      const ScriptItemsByOrder& scriptItemsByOrder) override;
  void JavaScriptOnDocumentStartByOrderV2(
      const ScriptItems& scriptItems,
      const ScriptRegexItems& scriptRegexItems,
      const ScriptItemsByOrder& scriptItemsByOrder) override;
  void JavaScriptOnDocumentEndByOrderV2(
      const ScriptItems& scriptItems,
      const ScriptRegexItems& scriptRegexItems,
      const ScriptItemsByOrder& scriptItemsByOrder) override;
  void JavaScriptOnHeadReadyByOrderV2(
      const ScriptItems& scriptItems,
      const ScriptRegexItems& scriptRegexItems,
      const ScriptItemsByOrder& scriptItemsByOrder) override;
#endif

  // For NWebEx
  static NWebImpl* FromID(int32_t nweb_id);
  static std::shared_ptr<NWebImpl> GetNWebSharedPtr(int32_t nweb_id);
  std::string GetUrl() override;

  CefRefPtr<CefClient> GetCefClient() const {
    return nweb_delegate_ ? nweb_delegate_->GetCefClient() : nullptr;
  }
  void AddNWebToMap(uint32_t id, std::shared_ptr<NWebImpl>& nweb);
#if BUILDFLAG(ARKWEB_NWEB_EX)
  bool CanStoreWebArchive() const;
  static const std::vector<std::string>& GetCommandLineArgsForNWebEx();
  static void InitBrowserServiceApi(std::vector<std::string>& browser_args,
                                    int32_t api_level);
  static bool GetBrowserServiceApiEnabled();
  static int32_t GetBrowserServiceSdkAPILevel();
  void PutArkWebAppClientExtensionCallback(
      std::shared_ptr<ArkWebAppClientExtensionCallback> callback);
  void PutWebAppClientExtensionCallback(
      std::shared_ptr<NWebAppClientExtensionCallback>
          web_app_client_extension_listener);
  void RemoveArkWebAppClientExtensionCallback();
  void RemoveWebAppClientExtensionCallback();

  void PutWebExtensionCallback(
      std::shared_ptr<NWebExtensionCallback> web_extension_callback);
  void RemoveWebExtensionCallback();
  void RunJavaScriptInFrames(RunJavaScriptParam param,
                             OnReceiveValueCallback callback);
  void GetAllFrameInfos(OnReceiveFrameInfosCallback callback);
  void GetLastJavaScriptProxyCallingFrameInfo(
      OnLastJavaScriptProxyCallingFrameInfoCallback callback);
  void GetImageFromContextNode();
  void GetImageFromCache(const std::string& url);
  void ReloadOriginalUrl() const;
  void SetBrowserUserAgentString(const std::string& user_agent);
  void OpenDevtools(std::unique_ptr<OpenDevToolsParam> param);
  void OpenDevtoolsByPb(std::unique_ptr<OpenDevToolsParam> param,
                        OpenDevToolsExtOpt& ext_opt);
  void CloseDevtools();
  void EnableAutoResize(
      int32_t min_width, int32_t min_height, int32_t max_width, int32_t max_height);
  void DisableAutoResize();
  std::shared_ptr<HitTestResult> GetLastHitTestResultForBrowser();
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  void EnableVideoAssistant(bool enable);
  void ExecuteVideoAssistantFunction(const std::string& cmd_id);
  static void OnReportStatisticLog(const std::string& content);
  static void SetOnReportStatisticLogCallback(OnReportStatisticLogFunc func);
  void CustomWebMediaPlayer(bool enable);
  void WebMediaPlayerControllerPlay();
  void WebMediaPlayerControllerPause();
  void WebMediaPlayerControllerSeek(double time);
  void WebMediaPlayerControllerSetMuted(bool muted);
  void WebMediaPlayerControllerSetPlaybackRate(double playback_rate);
  void WebMediaPlayerControllerExitFullscreen();
  void WebMediaPlayerControllerSetVideoSurface(void* native_window);
  void WebMediaPlayerControllerDownload();
  void WebMediaPlayerControllerSetVolume(double volume);
  double WebMediaPlayerControllerGetVolume();
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_EX_NETWORK_CONNECTION)
  static void SetConnectTimeout(int32_t seconds);
  static void BindToNetwork(int network_id);
  static void SetConnectionTimeout(int32_t timeout);
#endif

#if BUILDFLAG(ARKWEB_EXT_UA)
  static void UpdateCloudUAConfig(const std::string& file_path,
                                  const std::string& version);
  static void UpdateUAListConfig(const std::string& ua_name,
                                 const std::string& ua_string);
  static void SetUAForHosts(const std::string& ua_name,
                            const std::vector<std::string>& hosts);
  static std::string GetUANameConfig(const std::string& host);
  static void SetBrowserUA(const std::string& ua_name);
  static void EnableGlobalBrowserUAConfig(const bool enable);
  void SetBrowserUAConfigPolicy(int ua_config_policy);
#endif  // ARKWEB_EXT_UA

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  static void UpdateBrowserEngineConfig(const std::string& file_path,
                                        const std::string& version);
#endif

#if BUILDFLAG(ARKWEB_CLOUD_CONTROL)
  static void UpdateBrowserEngineGlobalConfig(const std::string& file_path,
                                              const std::string& version);
#endif

#if BUILDFLAG(ARKWEB_READER_MODE)
  static void UpdateReaderModeConfig(const std::string& file_path, const std::string& version);
  static void SetJsFilePath(const std::string& js_type, const std::string& file_path, const std::string& version);
  void Distill(char** guid, const DistillOptions& distill_options, DistillCallback callback);
  void AbortDistill();
#endif

#if BUILDFLAG(ARKWEB_EXT_FORCE_ZOOM) || BUILDFLAG(ARKWEB_ZOOM)
  void SetForceEnableZoom(bool forceEnableZoom) const override;
#endif
#if BUILDFLAG(ARKWEB_EXT_FORCE_ZOOM)
  bool GetForceEnableZoom() const;
#endif  // ARKWEB_EXT_FORCE_ZOOM

  void PutWebDownloadDelegateCallback(
      std::shared_ptr<NWebDownloadDelegateCallback>);
  void StartDownload(const char* url);
  void ResumeDownload(std::shared_ptr<NWebDownloadItem> web_download);
  void StopFling() override;
  void ReloadIgnoreCache() override;
  static void ResumeDownloadStatic(
      std::shared_ptr<NWebDownloadItem> download_item);

#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
  static void SetFileRenameOption(const int file_rename_option);
#endif  // BUILDFLAG(ARKWEB_EX_DOWNLOAD)

  static void ReadDownloadData(const std::string& guid, const int32_t read_size, ReadDownloadDataCallback callback);

#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
  NWebDownloadItemState GetDownloadItemState(long item_id);
  static NWebDownloadItemState GetDownloadItemStateByGuid(
      const std::string& guid);
#endif

  bool NeedSoftKeyboard() override;

  bool GetIsEditTextType();
  static std::shared_ptr<NWeb> GetNWeb(int32_t nweb_id);
  static std::shared_ptr<NWeb> CreateNWeb(
      std::shared_ptr<NWebCreateInfo> create_info);
  static void SetWebTag(int32_t nweb_id, const char* web_tag);
  static void InitializeWebEngine(
      std::shared_ptr<NWebEngineInitArgs> init_args);
  static void PrepareForPageLoad(const std::string& url,
                                 bool preconnectable,
                                 int32_t num_sockets);
  static void RemoveAllCache(bool include_disk_files);
  static void PauseAllTimers();
  static void ResumeAllTimers();
  static void PrefetchResource(
      const std::shared_ptr<NWebEnginePrefetchArgs>& pre_args,
      const std::map<std::string, std::string>& additional_http_headers,
      const std::string& cache_key,
      const uint32_t& cache_valid_time);
  static void ClearPrefetchedResource(
      const std::vector<std::string>& cache_key_list);

  static void SetProxyOverride(
      const std::vector<std::string>& proxyUrls,
      const std::vector<std::string>& proxySchemeFilters,
      const std::vector<std::string>& bypassRules,
      const bool& reverseBypass,
      std::shared_ptr<NWebProxyChangedCallback> callback);
  static void RemoveProxyOverride(std::shared_ptr<NWebProxyChangedCallback> callback);

#if BUILDFLAG(ARKWEB_COOKIE)
  static bool InitializeICUStatic(
      std::shared_ptr<NWebEngineInitArgs> init_args);
#endif  // BUILDFLAG(ARKWEB_COOKIE)

#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
  void NotifyPopupWindowResult(bool result) override {
    nweb_delegate_->NotifyPopupWindowResult(result);
  }
  void NotifyPopupWindowDisposition(
      CefLifeSpanHandler::WindowOpenDisposition disposition);
#endif  // BUILDFLAG(ARKWEB_MULTI_WINDOW)

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  void ShowFreeCopyMenu() const;
  bool ShouldShowFreeCopyMenu() const;
  std::string GetSelectedTextFromContextParam();
#endif  // BUILDFLAG(ARKWEB_EXT_FREE_COPY)

#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
  void UpdateBrowserControlsState(int constraints,
                                  int current,
                                  bool animate) const;
  void UpdateBrowserControlsHeight(int height, bool animate);
#endif

#if BUILDFLAG(ARKWEB_EX_REFRESH_IFRAME)
  bool WebExtensionContextMenuIsIframe();
  void WebExtensionContextMenuReloadFocusedFrame();
#endif

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  void WebExtensionContextMenuGetFocusedFrameInfo(int32_t& frame_id,
                                                  std::string& frame_url);
#endif

#if BUILDFLAG(ARKWEB_EXT_GET_ZOOM_LEVEL)
  static void SetDefaultBrowserZoomLevel(double zoom_factor);
  void SetBrowserZoomLevel(double zoom_factor) const;
  double GetBrowserZoomLevel() const;
#endif

#if BUILDFLAG(ARKWEB_CRASHPAD)
  static void SetDefaultCrashpadLogPath(const std::string& crashpadLogPath);
  static const std::string GetDefaultCrashpadLogPath();
  static void SetDefaultFeedbacklogsCrashPath(
      const std::string& feedbacklogs_crash_path);
  static const std::string& GetDefaultFeedbacklogsCrashPath();
#endif

#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
  bool IsIncognitoMode() override { return incognito_mode_; }
#endif

#if BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
  static bool SetExceptionListForJavaScriptEnabled(
      const std::vector<std::string>& urls,
      bool isEnabled);
  static bool SetExceptionListForAcceptCookie(
      const std::vector<std::string>& urls,
      bool isEnabled);
#endif

#if BUILDFLAG(ARKWEB_EXT_PERMISSION)
  void Grant(NWebPermissionRequest* request, int32_t resourse_id);
  void Deny(NWebPermissionRequest* request);
  std::string GetOrigin(NWebPermissionRequest* request);
  int32_t GetResourceId(NWebPermissionRequest* request);
  void GrantV2(int32_t resourse_id, int nweb_request_key);
  void DenyV2(int nweb_request_key);
  std::string GetOriginV2(int nweb_request_key);
  int32_t GetResourceIdV2(int nweb_request_key);
  void PermissionRequestDelete(int nweb_request_key);
#endif

#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
  int InsertBackForwardEntry(int index, const std::string& url);
  int UpdateNavigationEntryUrl(int index, const std::string& url);
  void ClearForwardList();
#endif

#if BUILDFLAG(ARKWEB_SCHEME_HANDLER)
  void SetWebTag(const std::string& web_tag) { web_tag_ = web_tag; }
  std::string GetWebTag() { return web_tag_; }
#endif
#if BUILDFLAG(ARKWEB_ITP)
  void EnableIntelligentTrackingPrevention(bool enable) override;
  bool IsIntelligentTrackingPreventionEnabled() const override;
  static bool IsAnyNWebIntelligentTrackingPreventionEnabled();
#endif  // BUILDFLAG(ARKWEB_ITP)

  std::string GetSelectInfo() override;

#if BUILDFLAG(ARKWEB_AI_WRITE)
  int GetSelectStartIndex() override;
  int GetSelectEndIndex() override;
  std::string GetAllTextInfo() override;
#endif // ARKWEB_AI_WRITE
#if BUILDFLAG(ARKWEB_USERAGENT)
  static std::string GetDefaultUserAgent();
#endif
  void OnCreateNativeMediaPlayer(
      std::shared_ptr<NWebCreateNativeMediaPlayerCallback> callback) override;

  static void SetLoggerReportEventCallback(void* callback);
  static void UploadCallback(const std::string& module,
                             const std::string& resource,
                             const std::string& errorCode,
                             const std::string& errorMsg);
  static void* logger_report_event_callback_;

#if BUILDFLAG(ARKWEB_ITP)
  static void AddIntelligentTrackingPreventionBypassingList(
      const std::vector<std::string>& hosts);
  static void RemoveIntelligentTrackingPreventionBypassingList(
      const std::vector<std::string>& hosts);
  static void ClearIntelligentTrackingPreventionBypassingList();
#endif

  static void WarmupServiceWorker(const std::string& url);

  int ScaleGestureChange(double scale, double centerX, double centerY) override;
  int ScaleGestureChangeV2(int type,
                           double scale,
                           double originScale,
                           double centerX,
                           double centerY) override;
  static void SetHostIP(const std::string& hostName,
                        const std::string& address,
                        int32_t aliveTime);
  static void ClearHostIP(const std::string& hostName);

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
  static void SetRenderProcessMode(RenderProcessMode mode);
  static RenderProcessMode GetRenderProcessMode();
#endif

#if BUILDFLAG(ARKWEB_SITE_ISOLATION)
  static int32_t SetSiteIsolationMode(bool mode);
  static bool GetSiteIsolationModeResult();
#endif

#if BUILDFLAG(ARKWEB_USERAGENT)
  static void SetAppCustomUserAgent(const std::string& userAgent);
  static void SetUserAgentForHosts(const std::string& userAgent,
                                   const std::vector<std::string>& hosts);
  static bool GetUserAgentClientHintsEnabled();
  static void SetUserAgentClientHintsEnabled(bool enabled);
  void SetUserAgentMetadata(
      const std::string& user_agent,
      std::shared_ptr<NWebUserAgentMetadata> metadata) override;
  std::shared_ptr<NWebUserAgentMetadata> GetUserAgentMetadata(
      const std::string& user_agent) override;
#endif

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  static void PutWebExtensionApiCallback(
      std::shared_ptr<NWebExtensionApiCallback> web_extension_api_callback);
  static void RemoveWebExtensionApiCallback();
  static void PutWebExtensionManagerCallback(
      std::shared_ptr<NWebExtensionManagerCallBack> web_extension_api_callback);
  static void RemoveWebExtensionManagerCallback();
  static void UnLoadWebExtension(const std::string& eid);
  static void DisableWebExtension(const std::string& eid);
  static void GetExtensionInfoByTabId(int32_t tabId, std::vector<WebExtensionInfo>& extensionInfo);
  static void SetExtensionName(const std::string& extension_name);
  static bool GetExtensionName(std::string& extension_name);
  static std::string GetExtensionVersion(const std::string& extension_id);
  static void InstallExtensionFile(const std::string& file_path,
                                   OnExtensionInstallCallback callback);
  static void InstallExtensionFileV2(
      const std::string& file_path,
      const NWebExtensionInstallProperties& options,
      OnExtensionInstallCallback callback);
  static void UninstallExtension(const std::string& extension_id,
                                 OnExtensionUninstallCallback callback);
  static void SetPublisherKeys(const std::vector<std::vector<uint8_t>>& keys);
  static void WebExtensionSetForbidDisplayInSettings(
    const std::set<std::string>& extension_ids);
  void WebExtensionTabRemoved(int tab_id, bool isWindowClosing, int windowId);
  void WebExtensionTabUpdated(
      int tab_id,
      std::unique_ptr<NWebExtensionTabChangeInfo> changeInfo,
      std::unique_ptr<NWebExtensionTab> tab);
  void WebExtensionTabAttached(int tab_id,
      std::unique_ptr<NWebExtensionTabAttachInfo> attachInfo);
  void WebExtensionTabDetached(int tab_id,
      std::unique_ptr<NWebExtensionTabDetachInfo> detachInfo);
  void WebExtensionTabMoved(int32_t tab_id,
                            std::unique_ptr<NWebExtensionTabMoveInfo> moveInfo);
  void WebExtensionTabReplaced(int32_t addedTabId, int32_t removedTabId);
  void WebExtensionSetViewType(int32_t type);

  static void OnAlertDialogByJS(const std::string& extensionId,
                                const std::string& url,
                                const std::string& message,
                                CefRefPtr<CefJSDialogCallback> callback,
                                bool& suppress_message);
  static void OnConfirmDialogByJS(const std::string& extensionId,
                                  const std::string& url,
                                  const std::string& message,
                                  CefRefPtr<CefJSDialogCallback> callback,
                                  bool& suppress_message);
  static void OnPromptDialogByJS(const std::string& extensionId,
                                 const std::string& url,
                                 const std::string& message,
                                 const std::string& value,
                                 CefRefPtr<CefJSDialogCallback> callback,
                                 bool& suppress_message);
  static void SetOnOffscreenDocumentAlertCallback(OnArkWebStaticOffscreenDocumentAlertFunc func);
  static void SetOnOffscreenDocumentConfirmCallback(OnArkWebStaticOffscreenDocumentConfirmFunc func);
  static void SetOnOffscreenDocumentPromptCallback(OnArkWebStaticOffscreenDocumentPromptFunc func);

  static void AlertHandle(const int requestId);
  static void ConfirmHandle(const bool type, const int requestId);
  static void PromptHandle(const bool type, const std::string& value, const int requestId);

  static void SetOnOffscreenDocumentPermissionRequestCallback(
      OnArkWebStaticOffscreenDocumentPermissionRequestFunc func);
  static void OnOffscreenDocumentPermissionRequest(
      const std::string& extension_id,
      const std::string& origin_url,
      int resources,
      int request_key);
  static void GrantOffscreenDocumentPermission(int resources, int request_key);
  static void DenyOffscreenDocumentPermission(int resources, int request_key);
  static void OnOffscreenDocumentWindowNewEvent(
      const std::string& extensionId,
      const std::string& originUrl,
      bool isAlert,
      bool isUserTrigger,
      const std::string& targetUrl);
  static void SetOnOffscreenDocumentWindowNewCallback(
      OnArkWebStaticOffscreenDocumentWindowNewFunc func);
  static void SetOffscreenNWebId(uint32_t nweb_id);
  static void ResetOffscreenNWebId();
#endif  // ARKWEB_ARKWEB_EXTENSIONS

#if BUILDFLAG(ARKWEB_AI)
  void OnTextSelected() override;
  void OnDestroyImageAnalyzerOverlay() override;
  void RunDataDetectorJS() override;
  void SetDataDetectorEnable(bool enable) override;
  void OnDataDetectorSelectText() override;
  void OnDataDetectorCopy(const std::vector<std::string>& recordMix) override;
#endif

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
  static void PutLoggerCallback(
      std::shared_ptr<NWebLoggerCallback> logger_callback);
  static void RemoveLoggerCallback();
#endif

  int SetUrlTrustList(const std::string& urlTrustList) override;
  int SetUrlTrustListWithErrMsg(const std::string& urlTrustList,
                                std::string& detailErrMsg) override;

  int SetUrlTrustListWithErrMsg(const std::string& urlTrustList,
      bool allowOpaqueOrigin, bool supportWildcard, std::string& detailErrMsg) override;

#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
  void OnSafeInsetsChange(int left, int top, int right, int bottom) override;
#endif
  static base::Lock nweb_map_lock_;

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  static void SetWholeWebDrawing();

  bool WebPageSnapshot(const char* id,
                       PixelUnit type,
                       int width,
                       int height,
                       const WebSnapshotCallback callback) override;

  bool WebPageSnapshotV2(const char* id,
                         PixelUnit type,
                         int width,
                         int height,
                         std::shared_ptr<NWebSnapshotCallback> callback) override;
#endif

#ifdef BUILDFLAG(IS_OHOS)
  void OnConfigurationUpdated(
      std::shared_ptr<NWebSystemConfiguration> configuration) override;
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  enum class PathType {
      kDirResource,
      kDirFile,
      kDirCache,
      kDirTemp,
  };

  void SetPathAllowingUniversalAccess(
      const std::vector<std::string>& pathList,
      const std::vector<std::string>& moduleName,
      std::string& errorPath) override;
  int PrerenderPage(const std::string& url,
                    const std::string& additional_headers);
  void CancelAllPrerendering();
  static void SetExtraHeadersMap(const std::string& url,
                                 const std::string& additional_headers);
#endif

#if BUILDFLAG(ARKWEB_EXT_FILE_ACCESS)
  void DisallowSandboxFileAccessFromFileUrl(bool disallow) const;
#endif  // BUILDFLAG(ARKWEB_EXT_FILE_ACCESS)

#if BUILDFLAG(ARKWEB_BFCACHE)
  void SetBackForwardCacheOptions(int32_t size, int32_t timeToLive) override;
  void SetMediaResumeFromBFCachePage(bool resume);
#endif

#if BUILDFLAG(ARKWEB_MIXED_CONTENT)
  void EnableMixedContentAutoUpgrades(bool enable);
  bool IsMixedContentAutoUpgradesEnabled();
#endif

#if BUILDFLAG(ARKWEB_MEDIA_NETWORK_TRAFFIC_PROMPT)
  void EnableMediaNetworkTrafficPrompt(bool enable);
#endif  // ARKWEB_MEDIA_NETWORK_TRAFFIC_PROMPT
  void SendAccessibilityHoverEventV2(int32_t x,
                                     int32_t y,
                                     bool isHoverEnter) override;
  static void TrimMemoryByPressureLevel(int32_t memoryLevel);
#if BUILDFLAG(IS_ARKWEB)
  void SetSurfaceDensity(const double& density) override;
  void EnableAppLinking(bool enable);
#endif

#if BUILDFLAG(ARKWEB_DFX_DUMP)
  void getTotalSize(float size);
  float DumpGpuInfo() override;
#endif
#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
  void SetTransformHint(uint32_t rotation) override;
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
  void SetPopupSurface(void* popupSurface) override;
#endif

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
  void SetUsageScenario(int32_t usage_scenario);
  int32_t GetUsageScenario();
#endif  // ARKWEB_MEDIA_CAPABILITIES_ENHANCE

#if BUILDFLAG(ARKWEB_DRAG_RESIZE)
  void DragResize(uint32_t width,
                  uint32_t height,
                  uint32_t pre_height,
                  uint32_t pre_width) override;
  bool IsNWebEx() override;
#endif

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
  void MaximizeResize() override;
#endif  // ARKWEB_MAXIMIZE_RESIZE

#if BUILDFLAG(ARKWEB_DISATCH_BEFORE_UNLOAD)
  bool NeedToFireBeforeUnloadOrUnloadEvents();
  void DispatchBeforeUnload();
#endif  // ARKWEB_DISATCH_BEFORE_UNLOAD

#if BUILDFLAG(ARKWEB_SAVE_PAGE)
  bool SavePage(int32_t type, const std::string& filePath);
#endif // ARKWEB_SAVE_PAGE

#if BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)
  void StopScreenCapture(int32_t nweb_id, const char* session_id);
  void SetScreenCapturePickerShow();
  void DisableSessionReuse();
  void PutWebScreenCaptureDelegateCallback(
      std::shared_ptr<NWebScreenCaptureDelegateCallback> callback);
#endif // defined(ARKWEB_EX_SCREEN_CAPTURE)

  static void SetMigrationPasswordReady(const bool migrationReady);

#if BUILDFLAG(ARKWEB_ANGLE) && BUILDFLAG(ARKWEB_NWEB_EX)
  static void UpdateAngleConfig(bool angle_switch);
#endif // ARKWEB_ANGLE && ARKWEB_NWEB_EX

#if BUILDFLAG(ARKWEB_EDM_POLICY)
  static void SetEnterprisePolicy(const std::string& policy, int version);
#endif
#if BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
  void SetBlankScreenDetectionConfig(
      bool enable,
      const std::vector<double>& detectionTiming,
      const std::vector<int32_t>& detectionMethods,
      int32_t contentfulNodesCountThreshold) override;
#endif
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  bool SetFocusByPosition(float x, float y) override;
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
#if BUILDFLAG(ARKWEB_SOFTKEYBOARD_AVOID)
  void SetSoftKeyboardBehaviorMode(WebSoftKeyboardBehaviorMode mode) override;
#endif
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
#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  void SetBlanklessLoadingKey(const std::string& key) override;
  void SetPrivacyStatus(bool isPrivate) override;
  int32_t GetBlanklessInfoWithKey(const std::string& key, double* similarity, int32_t* loadingTime) override;
  int32_t SetBlanklessLoadingWithKey(const std::string& key, bool isStart) override;
  int64_t GetPreferenceHash();
  static int64_t GetPreferenceHashByNwebId(int32_t nweb_id);
  bool TriggerBlanklessForUrl(const std::string& url) override;
  void SetVisibility(bool isVisible) override;
  int32_t SetBlanklessLoadingParams(const std::string& key, bool enable, int32_t duration,
                                    int64_t expirationTime,
                                    std::shared_ptr<NWebBlanklessCallback> callback) override;
  void CallExecuteBlanklessCallback(int32_t state, const std::string& reason) override;
#endif

#if BUILDFLAG(ARKWEB_ERROR_PAGE)
  void SetErrorPageEnabled(bool enable) override;
  bool GetErrorPageEnabled() override;
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  static void EnablePrivateNetworkAccess(bool enable);
  static bool IsPrivateNetworkAccessEnabled();
  static void EnableRewriteUrlForNavigation(bool enable);
#endif
#if BUILDFLAG(ARKWEB_BGTASK)
  void OnBrowserForeground() override;
  void OnBrowserBackground() override;
#endif

#if BUILDFLAG(ARKWEB_EXT_HTTPS_UPGRADES)
  void EnableHttpsUpgrades(bool enable);
#endif

#if BUILDFLAG(ARKWEB_NETWORK_SERVICE)
  static void SetSocketIdleTimeout(int32_t timeout);
#endif

#if BUILDFLAG(ARKWEB_CLIPBOARD)
  static void SetClipboardSitePermissionEnabled(bool enable);
  static bool IsClipboardSitePermissionEnabled();
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)

#if BUILDFLAG(ARKWEB_WEBRTC)
  void ResumeMicrophone() override;
  void StopMicrophone() override;
  void PauseMicrophone() override;
#endif
#if BUILDFLAG(ARKWEB_COOKIE)
  static void LibraryLoaded(std::shared_ptr<NWebEngineInitArgs> init_args,
                            bool lazy);
  static bool ShouldLazyInitWebEngine();
  static std::shared_ptr<NWebEngineInitArgs> GetSaveInitargs();
#endif

#if BUILDFLAG(ARKWEB_EXT_RECEIVE_RESPONSE)
  std::map<std::string, std::string> GetRequestHeader(int32_t nweb_request_key);
  std::string GetRequestUrl(int32_t nweb_request_key);
  bool IsRequestGesture(int32_t nweb_request_key);
  bool IsMainFrame(int32_t nweb_request_key);
  bool IsRedirect(int32_t nweb_request_key);
  std::string GetRequestMethod(int32_t nweb_request_key);
  int32_t GetPageTransition(int32_t nweb_request_key);
  int32_t GetRequestType(int32_t nweb_request_key);
 
  std::string GetMimeType(int32_t nweb_response_key);
  std::string GetEncoding(int nweb_response_key);
  int32_t GetStatusCode(int nweb_response_key);
  std::string GetReasonPhrase(int nweb_response_key);
  std::map<std::string, std::string> GetResponseHeader(int32_t nweb_response_key);
  bool GetIsFromNetwork(int nweb_response_key);
  void ResourceRequestDelete(int nweb_request_key);
  void ResourceResponseDelete(int nweb_response_key);
  int32_t GetLastCommittedEntryPageTransition();
#endif

 private:
  void ProcessInitArgs(std::shared_ptr<NWebEngineInitArgs> init_args);
  void InitWebEngineArgs(std::shared_ptr<NWebEngineInitArgs> init_args);
  bool InitWebEngine(std::shared_ptr<NWebCreateInfo> create_info);
  bool SetVirtualDeviceRatio();
#if BUILDFLAG(ARKWEB_I18N)
  void UpdateAcceptLanguageInternal();
#endif
#if BUILDFLAG(ARKWEB_WEBRTC)
  void StopCameraSession() const;
  void RestartCameraSession() const;
#endif  // BUILDFLAG(ARKWEB_WEBRTC)

  std::function<void(const char*)> validCallback_ = nullptr;
  std::function<void(const char*)> destroyCallback_ = nullptr;
  std::function<void(void)> nativeDestroyCallback_ = nullptr;
  std::string webName_;
  base::Lock state_lock_;

#if BUILDFLAG(ARKWEB_PERFORMANCE_INC_FREQ)
  int64_t ResizeTime_ = 0;
#endif
  uint32_t nweb_id_ = 0;
  int32_t draw_mode_ = 0;
  std::shared_ptr<NWebHandler> nweb_handle_ = nullptr;
  std::shared_ptr<NWebOutputHandler> output_handler_ = nullptr;
  std::shared_ptr<NWebInputHandler> input_handler_ = nullptr;
  CefRefPtr<NWebInputMethodHandler> inputmethod_handler_ = nullptr;

  std::shared_ptr<NWebDelegateInterface> nweb_delegate_ = nullptr;
  std::list<std::string> web_engine_args_;
  float device_pixel_ratio_ = 0.f;
  bool is_enhance_surface_ = false;
  bool is_richtext_value_ = false;
  static bool disableWebActivePolicy_;
  static WebDestroyMode webDestroyMode_;
  static ScrollbarMode scrollbarMode_;
  static bool scrollbarModeChanged_;

  bool incognito_mode_ = false;
  raw_ptr<void> window_;
#if BUILDFLAG(ARKWEB_DFX_DUMP)
  float totalSize_ = 0;
#endif
#if BUILDFLAG(ARKWEB_SCHEME_HANDLER)
  std::string web_tag_{""};
#endif
#if BUILDFLAG(ARKWEB_SOFTKEYBOARD_AVOID)
  WebSoftKeyboardBehaviorMode keyboardBehaviorMode_ = WebSoftKeyboardBehaviorMode::DEFAULT;
#endif
  bool is_pause_ = false;
  struct ReSizeType {
    uint32_t width_ = 0;
    uint32_t height_ = 0;
    bool is_keyboard_ = false;
  };
  std::optional<ReSizeType> pending_size_ = std::nullopt;

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  static OnReportStatisticLogFunc on_report_statistic_log_callback_;
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  static OnArkWebStaticOffscreenDocumentAlertFunc on_off_screen_alert_callback_;
  static OnArkWebStaticOffscreenDocumentConfirmFunc on_off_screen_confirm_callback_;
  static OnArkWebStaticOffscreenDocumentPromptFunc on_off_screen_prompt_callback_;

  static OnArkWebStaticOffscreenDocumentPermissionRequestFunc
      on_offscreen_document_permission_request_callback_;
  static OnArkWebStaticOffscreenDocumentWindowNewFunc on_off_screen_window_new_callback_;
  static uint32_t off_screen_nweb_id_;
#endif  // ARKWEB_ARKWEB_EXTENSIONS

#if BUILDFLAG(ARKWEB_JAVASCRIPT_BRIDGE)
  void RegisterNativeJavaScriptProxy(const std::string& objName,
                                     const std::vector<std::string>& methodName,
                                     std::shared_ptr<OHOS::NWeb::NWebJsProxyMethod> data,
                                     bool isAsync,
                                     const std::string& permission) override;
#endif

  void OnTouchCancelById(int32_t id, double x, double y, bool from_overlay) override {};

  std::unique_ptr<base::RetainingOneShotTimer> drag_over_timer_;
  DelegateDragEvent drag_over_event_;

#if BUILDFLAG(ARKWEB_VIEWPORT_AVOID)
  void AvoidVisibleViewportBottom(int32_t avoidHeight) override;
  int32_t GetVisibleViewportAvoidHeight() override;
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  void MarkUserEnableBlankless()
  {
    is_user_enable_ = true;
  }
  void ResetUserEnableBlankless()
  {
    is_user_enable_ = false;
  }
  bool IsUserEnableBlankless()
  {
    return is_user_enable_;
  }
  bool ProcessBlanklessForUrl(uint64_t blanklessKey, bool isAnime = false);
  void ClearBlanklessKey();
  bool CheckNetAvailable();
  void CallBlanklessFrameFunc(uint64_t blankless_key, SnapshotDataItem& dataItem, bool isAnime = false);
  void CallBlanklessFrameFuncV2(uint64_t blankless_key, SnapshotDataItem& dataItem,
                                int32_t duration, bool isAnime = false);
  void CallBlanklessFrameFuncForWhiteList(uint64_t blankless_key, SnapshotDataItem& dataItem, bool isAnime = false);
  void ExecuteBlanklessCallback(const std::string& key, int32_t state, const std::string& reason);
  // To avoid include blankless_controller.h in nweb_impl.h, we use UINT64_MAX instead of INVALID_BLANKLESS_KEY.
  std::atomic<uint64_t> blankless_key_ = UINT64_MAX;
  std::atomic<bool> is_private_ = false;
  std::atomic<bool> is_visible_ = false;
  std::atomic<bool> is_user_enable_ = false;
  std::string string_key_;
  std::shared_ptr<NWebBlanklessCallback> blankless_callback_ = nullptr;
#endif

#if BUILDFLAG(ARKWEB_COOKIE)
  static std::shared_ptr<NWebEngineInitArgs> save_initargs_;
  static bool should_lazy_init_web_engine_;
#endif

  base::WeakPtrFactory<NWebImpl> weak_factory_{this};
};
}  // namespace OHOS::NWeb

#endif  // NWEB_IMPL_H
