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
#ifndef MOCK_NWEB_DELEGATE_H
#define MOCK_NWEB_DELEGATE_H

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <vector>

#include "capi/nweb_devtools_message_handler.h"
#include "nweb_delegate_interface.h"

namespace OHOS::NWeb {
class MockNWebDelegate : public NWebDelegateInterface {
 public:
  virtual ~MockNWebDelegate() {}
  MOCK_METHOD(void, OnWindowShow, (), (override));
  MOCK_METHOD(void, OnWindowHide, (), (override));
  MOCK_METHOD(void, OnOnlineRenderToForeground, (), (override));
  MOCK_METHOD(bool, IsReady, (), (override));
  MOCK_METHOD(void, OnDestroy, (bool is_close_all), (override));
  MOCK_METHOD(void,
              RegisterDownLoadListener,
              (std::shared_ptr<NWebDownloadCallback> downloadListener),
              (override));
  MOCK_METHOD(void,
              RegisterAccessibilityEventListener,
              (std::shared_ptr<NWebAccessibilityEventCallback>
                   accessibilityEventListener),
              ());
  MOCK_METHOD(
      void,
      RegisterReleaseSurfaceListener,
      (std::shared_ptr<NWebReleaseSurfaceCallback> releaseSurfaceListener),
      (override));
  MOCK_METHOD(void,
              RegisterNWebHandler,
              (std::shared_ptr<NWebHandler> handler),
              (override));
  MOCK_METHOD(void,
              RegisterRenderCb,
              (std::function<void(const char*)> render_update_cb),
              (override));
#if BUILDFLAG(ARKWEB_NWEB_EX)
  MOCK_METHOD(void,
              RegisterArkWebAppClientExtensionListener,
              (std::shared_ptr<ArkWebAppClientExtensionCallback> callback),
              (override));
#endif
  MOCK_METHOD(void,
              RegisterWebAppClientExtensionListener,
              (std::shared_ptr<NWebAppClientExtensionCallback>
                   web_app_client_extension_listener),
              (override));
  MOCK_METHOD(void,
              SetInputMethodClient,
              (CefRefPtr<NWebInputMethodClient> client),
              (override));
  MOCK_METHOD(
      void,
      RegisterWebDownloadDelegateListener,
      (std::shared_ptr<NWebDownloadDelegateCallback> downloadDelegateListener),
      (override));
  MOCK_METHOD(void, StartDownload, (const char* url), (override));
  MOCK_METHOD(void,
              ResumeDownload,
              (std::shared_ptr<NWebDownloadItem> web_download),
              (override));
  MOCK_METHOD(void, NotifyForNextTouchEvent, (), (override));
  MOCK_METHOD(void,
              SetAutofillCallback,
              (std::shared_ptr<NWebMessageValueCallback> callback),
              (override));
  MOCK_METHOD(void,
              FillAutofillData,
              (std::shared_ptr<NWebMessage> data),
              (override));
  MOCK_METHOD(void,
              FillAutofillDataV2,
              (std::shared_ptr<NWebRomValue> data),
              (override));
  MOCK_METHOD(void,
              ExecuteCreatePDFExt,
              (std::shared_ptr<NWebPDFConfigArgs> pdfConfig,
               std::shared_ptr<NWebArrayBufferValueCallback> callback),
              (override));
  MOCK_METHOD(bool,
              ScrollByWithResult,
              (float delta_x, float delta_y),
              (override));
#if BUILDFLAG(ARKWEB_UNITTESTS)
  MOCK_METHOD(void,
              OnSafeBrowsingDetectionResult,
              (int code,
               int policy,
               const std::string& mappingType,
               const std::string& url),
              (override));
  MOCK_METHOD(void, RunDataDetectorJS, (), (override));
  MOCK_METHOD(void, SetDataDetectorEnable, (bool enable), (override));
  MOCK_METHOD(bool, GetDataDetectorEnable, (), (override));
  MOCK_METHOD(std::string, GetDataDetectorSelectText, (), (override));
  MOCK_METHOD(void, OnDataDetectorSelectText, (), (override));
  MOCK_METHOD(void,
              OnDataDetectorCopy,
              (const std::vector<std::string>& recordMix),
              (override));
  MOCK_METHOD(std::string, GetCurrentLanguage, (), (override));

  MOCK_METHOD(void,
              RegisterNativeAsyncThreadJSProxyWithResult,
              (const std::string& objName,
               const std::vector<std::string>& methodName,
               std::vector<std::function<std::shared_ptr<OHOS::NWeb::NWebValue>(
                   std::vector<std::vector<uint8_t>>&,
                   std::vector<size_t>&)>>&& callback,
               const std::string& permission),
              (override));

  MOCK_METHOD(int, GetSelectStartIndex, (), (override));
  MOCK_METHOD(int, GetSelectEndIndex, (), (override));
  MOCK_METHOD(std::string, GetAllTextInfo, (), (override));
  MOCK_METHOD(void,
              SetBorderRadiusFromWeb,
              (double borderRadiusTopLeft,
               double borderRadiusTopRight,
               double borderRadiusBottomLeft,
               double borderRadiusBottomRight),
              (override));
#endif
#if BUILDFLAG(ARKWEB_MIXED_CONTENT)
  MOCK_METHOD(void, EnableMixedContentAutoUpgrades, (bool enable), (override));
  MOCK_METHOD(bool, IsMixedContentAutoUpgradesEnabled, (), (override));
#endif
#if BUILDFLAG(IS_ARKWEB)
  MOCK_METHOD(void, EnableAppLinking, (bool enable), (override));
#endif
  MOCK_METHOD(void, SetPopupSurface, (void* popupSurface), (override));

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  MOCK_METHOD(void,
              SetNWebDelegateInterface,
              (std::shared_ptr<NWebDelegateInterface> client),
              (override));
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_ADBLOCK)
  MOCK_METHOD(void, EnableAdsBlock, (bool enable), (override));
  MOCK_METHOD(bool, IsAdsBlockEnabled, (), (override));
  MOCK_METHOD(bool, IsAdsBlockEnabledForCurPage, (), (override));
#endif

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  MOCK_METHOD(void,
              ProcessAutofillCancel,
              (const std::string& fillContent),
              (override));
#endif

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
  MOCK_METHOD(void, SetSavePasswordAutomatically, (bool enable), (override));
  MOCK_METHOD(bool, GetSavePasswordAutomatically, (), (override));
  MOCK_METHOD(void, SetSavePassword, (bool enable), (override));
  MOCK_METHOD(bool, GetSavePassword, (), (override));
  MOCK_METHOD(void, SaveOrUpdatePassword, (bool is_update), (override));
#endif  // BUILDFLAG(ARKWEB_EXT_PASSWORD)

#if BUILDFLAG(ARKWEB_EXT_PASSWORD) || BUILDFLAG(ARKWEB_DATALIST)
  MOCK_METHOD(void,
              PasswordSuggestionSelected,
              (int list_index),
              (const, override));
#endif

#if BUILDFLAG(ARKWEB_NWEB_EX)
  MOCK_METHOD(bool, CanStoreWebArchive, (), (const, override));
  MOCK_METHOD(void, UnRegisterArkWebAppClientExtensionListener, (), (override));
  MOCK_METHOD(void, UnRegisterWebAppClientExtensionListener, (), (override));
  MOCK_METHOD(void,
              RegisterWebExtensionListener,
              (std::shared_ptr<NWebExtensionCallback> web_extension_listener),
              (override));
  MOCK_METHOD(void, UnRegisterWebExtensionListener, (), (override));
  MOCK_METHOD(void, GetImageFromContextNode, (), (override));
  MOCK_METHOD(void, GetImageFromCacheEx, (const std::string& url), (override));
#endif  // BUILDFLAG(ARKWEB_NWEB_EX)

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  MOCK_METHOD(void, ShowFreeCopyMenu, (), (override));
  MOCK_METHOD(bool, ShouldShowFreeCopyMenu, (), (override));
#endif  // ARKWEB_EXT_FREE_COPY

#if BUILDFLAG(ARKWEB_EXT_GET_ZOOM_LEVEL)
  MOCK_METHOD(void, SetBrowserZoomLevel, (double zoom_factor), (override));
  MOCK_METHOD(double, GetBrowserZoomLevel, (), (override));
#endif
  MOCK_METHOD(void,
              Resize,
              (uint32_t width, uint32_t height, bool isKeyboard),
              (override));

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  MOCK_METHOD(void,
              ResizeVisibleViewport,
              (uint32_t width, uint32_t height, bool isKeyboard),
              (override));
#endif

#if BUILDFLAG(ARKWEB_ERROR_PAGE)
  MOCK_METHOD(void, SetErrorPageEnabled, (bool enable), (override));
  MOCK_METHOD(bool, GetErrorPageEnabled, (), (override));
#endif

  MOCK_METHOD(void,
              OnTouchPress,
              (int32_t id, double x, double y, bool from_overlay),
              (override));
  MOCK_METHOD(void,
              OnTouchRelease,
              (int32_t id, double x, double y, bool from_overlay),
              (override));
  MOCK_METHOD(void,
              OnTouchMove,
              (int32_t id, double x, double y, bool from_overlay),
              (override));
  MOCK_METHOD(void,
              OnTouchMove,
              (const std::vector<std::shared_ptr<NWebTouchPointInfo>>&, bool),
              (override));
  MOCK_METHOD(void,
              OnStylusTouchPress,
              (std::shared_ptr<NWebStylusTouchPointInfo> stylus_touch_point_info, bool from_overlay),
              (override));
  MOCK_METHOD(void,
              OnStylusTouchRelease,
              (std::shared_ptr<NWebStylusTouchPointInfo> stylus_touch_point_info, bool from_overlay),
              (override));
  MOCK_METHOD(void,
              OnStylusTouchMove,
              (const std::vector<std::shared_ptr<NWebStylusTouchPointInfo>>& stylus_touch_point_infos, bool from_overlay),
              (override));
  MOCK_METHOD(void, OnTouchCancel, (), (override));
  MOCK_METHOD(void,
              OnTouchCancelById,
              (int32_t id, double x, double y, bool from_overlay),
              (override));
  MOCK_METHOD(bool,
              SendKeyEvent,
              (int32_t keyCode, int32_t keyAction),
              (override));
  MOCK_METHOD(void,
              SendTouchpadFlingEvent,
              (double x, double y, double vx, double vy),
              (override));
  MOCK_METHOD(void,
              SendMouseWheelEvent,
              (double x, double y, double deltaX, double deltaY),
              (override));
  MOCK_METHOD(void,
              SendMouseEvent,
              (int x, int y, int button, int action, int count),
              (override));
  MOCK_METHOD(void,
              NotifyScreenInfoChanged,
              (RotationType rotation, DisplayOrientation orientation),
              (override));

  MOCK_METHOD1(Load, int(const std::string&));
  MOCK_METHOD(bool, IsNavigatebackwardAllowed, (), (const, override));
  MOCK_METHOD(bool, IsNavigateForwardAllowed, (), (const, override));
  MOCK_METHOD(bool,
              CanNavigateBackOrForward,
              (int numSteps),
              (const, override));
  MOCK_METHOD(void, NavigateBack, (), (const, override));
  MOCK_METHOD(void, NavigateForward, (), (const, override));
  MOCK_METHOD(void, NavigateBackOrForward, (int32_t step), (const, override));
  MOCK_METHOD(void, DeleteNavigateHistory, (), (override));
  MOCK_METHOD(void, ClearSslCache, (), (override));
  MOCK_METHOD(void, ClearClientAuthenticationCache, (), (override));
  MOCK_METHOD(void, Reload, (), (const, override));
  MOCK_METHOD(void, ReloadOriginalUrl, (), (const, override));
  MOCK_METHOD(int, Zoom, (float zoomFactor), (const, override));
  MOCK_METHOD(int, ZoomIn, (), (const, override));
  MOCK_METHOD(int, ZoomOut, (), (const, override));
  MOCK_METHOD(void, Stop, (), (const, override));
  MOCK_METHOD(void,
              ExecuteJavaScript,
              (const std::string& code),
              (const, override));
  MOCK_METHOD(void, PutBackgroundColor, (int color), (const, override));
  MOCK_METHOD(void, InitialScale, (float scale), (const, override));
  MOCK_METHOD(void, OnPause, (), (override));
  MOCK_METHOD(void, OnContinue, (), (override));
  MOCK_METHOD(void, OnOccluded, (), (override));
  MOCK_METHOD(void, OnUnoccluded, (), (override));
  MOCK_METHOD(void, SetEnableLowerFrameRate, (bool enabled), (override));
  MOCK_METHOD(std::shared_ptr<NWebPreference>,
              GetPreference,
              (),
              (const, override));
  MOCK_METHOD(std::string, Title, (), (override));
  MOCK_METHOD(std::shared_ptr<HitTestResult>,
              GetHitTestResult,
              (),
              (const, override));
  MOCK_METHOD(int, PageLoadProgress, (), (override));
  MOCK_METHOD(float, Scale, (), (override));
  MOCK_METHOD2(Load,
               int(const std::string&,
                   const std::map<std::string, std::string>&));
  MOCK_METHOD(int,
              LoadWithDataAndBaseUrl,
              (const std::string&,
               const std::string&,
               const std::string&,
               const std::string&,
               const std::string&),
              (override));
  MOCK_METHOD(int,
              LoadWithData,
              (const std::string&, const std::string&, const std::string&),
              (override));
  MOCK_METHOD(int, ContentHeight, (), (override));
  MOCK_METHOD(
      void,
      RegisterNativeArkJSFunction,
      (const char* objName,
       const std::vector<std::shared_ptr<NWebJsProxyCallback>>& callbacks),
      (override));

  MOCK_METHOD(
      void,
      RegisterNativeJSProxy,
      (const std::string& objName,
       const std::vector<std::string>& methodName,
       std::vector<std::function<char*(std::vector<std::vector<uint8_t>>&,
                                       std::vector<size_t>&)>>&& callback,
       bool isAsync,
       const std::string& permission),
      (override));

  MOCK_METHOD(void,
              UnRegisterNativeArkJSFunction,
              (const char* objName),
              (override));

#if BUILDFLAG(ARKWEB_ADBLOCK)
  MOCK_METHOD(void,
              UpdateAdblockEasyListRules,
              (long adBlockEasyListVersion),
              (override));
#endif

  MOCK_METHOD(void,
              RegisterArkJSfunction,
              (const std::string& object_name,
               const std::vector<std::string>& method_list,
               const std::vector<std::string>& async_method_list,
               const int32_t object_id,
               const std::string& permission),
              (override, const));

  MOCK_METHOD(void,
              UnregisterArkJSfunction,
              (const std::string& object_name,
               const std::vector<std::string>& method_list),
              (override, const));

  MOCK_METHOD(void,
              RegisterNativeLoadStartCallback,
              (std::function<void(void)> && callback),
              (override));

  MOCK_METHOD(void,
              RegisterNativeLoadEndCallback,
              (std::function<void(void)> && callback),
              (override));

  MOCK_METHOD(void,
              JavaScriptOnDocumentStart,
              (const ScriptItems& ScriptItems),
              (override));

  MOCK_METHOD(void,
              JavaScriptOnDocumentEnd,
              (const ScriptItems& ScriptItems),
              (override));

  MOCK_METHOD(void,
              CallH5Function,
              (int32_t routing_id,
               int32_t h5_object_id,
               const std::string& h5_method_name,
               const std::vector<std::shared_ptr<NWebValue>>& args),
              (override, const));

  MOCK_METHOD(void,
              CallH5FunctionV2,
              (int32_t routing_id,
               int32_t h5_object_id,
               const std::string& h5_method_name,
               const std::vector<std::shared_ptr<NWebRomValue>>& args),
              (override, const));

  MOCK_METHOD(bool, Discard, (), (override));
  MOCK_METHOD(bool, Restore, (), (override));
  MOCK_METHOD(void,
              RegisterNWebJavaScriptCallBack,
              (std::shared_ptr<NWebJavaScriptResultCallBack> callback),
              (override));

  MOCK_METHOD(bool,
              OnFocus,
              (const FocusReason& focusReason),
              (override, const));

  MOCK_METHOD(void, OnBlur, (), (override, const));
  MOCK_METHOD(void,
              RegisterFindListener,
              (std::shared_ptr<NWebFindCallback> find_listener),
              (override));

  MOCK_METHOD(void, FindAllAsync, (const std::string& str), (override, const));

  MOCK_METHOD(void, ClearMatches, (), (override, const));
  MOCK_METHOD(void, FindNext, (const bool forward), (override, const));

  MOCK_METHOD(void, SuggestionSelected, (int index), (override, const));

  MOCK_METHOD(void,
              ExecuteJavaScriptExt,
              (const int fd,
               const size_t scriptLength,
               std::shared_ptr<NWebMessageValueCallback> callback,
               bool extention),
              (override));

#if BUILDFLAG(ARKWEB_MSGPORT)
  MOCK_METHOD(void, EraseJavaScriptCallbackImpl, (uint32_t id), (override));

  MOCK_METHOD(void,
              ExecuteJavaScript,
              (const std::string& code,
               std::shared_ptr<NWebMessageValueCallback> callback,
               bool extention),
              (override));

  MOCK_METHOD(std::vector<std::string>, CreateWebMessagePorts, (), (override));

  MOCK_METHOD(void,
              PostWebMessage,
              (const std::string& message,
               const std::vector<std::string>& ports,
               const std::string& targetUri),
              (override));

  MOCK_METHOD(void, ClosePort, (const std::string& portHandle), (override));

  MOCK_METHOD(void,
              PostPortMessage,
              (const std::string& portHandle,
               std::shared_ptr<NWebMessage> data),
              (override));

  MOCK_METHOD(void,
              PostPortMessageV2,
              (const std::string& portHandle,
               std::shared_ptr<NWebRomValue> data),
              (override));

  MOCK_METHOD(void,
              SetPortMessageCallback,
              (const std::string& portHandle,
               std::shared_ptr<NWebMessageValueCallback> callback),
              (override));
#endif  // BUILDFLAG(ARKWEB_MSGPORT)

#if BUILDFLAG(ARKWEB_I18N)
  MOCK_METHOD(void,
              UpdateLocale,
              (const std::string& language, const std::string& region),
              (override));
#endif  // #if BUILDFLAG(ARKWEB_I18N)
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  MOCK_METHOD(std::shared_ptr<NWebDragData>,
              GetOrCreateDragData,
              (),
              (override));
  MOCK_METHOD(std::string, GetAppTempDir, (), (const, override));
  MOCK_METHOD(bool, DarkModeEnabled, (), (override));
#endif  // #if BUILDFLAG(ARKWEB_DRAG_DROP)

  MOCK_METHOD(void, SetNWebId, (uint32_t nwebId), (override));
  MOCK_METHOD(void,
              StoreWebArchive,
              (const std::string& base_name,
               bool auto_name,
               std::shared_ptr<NWebStringValueCallback> callback),
              (const, override));

  MOCK_METHOD(void,
              SetBrowserUserAgentString,
              (const std::string& user_agent),
              (override));
  MOCK_METHOD(void,
              SendDragEvent,
              (const DelegateDragEvent& dragEvent),
              (const, override));
  MOCK_METHOD(bool, TerminateRenderProcess, (), (override));
  MOCK_METHOD(std::string, GetUrl, (), (const, override));
  MOCK_METHOD(const std::string, GetOriginalUrl, (), (override));
  MOCK_METHOD(bool,
              GetFavicon,
              (const void** data,
               size_t& width,
               size_t& height,
               ImageColorType& colorType,
               ImageAlphaType& alphaType),
              (override));

  MOCK_METHOD(void, PutNetworkAvailable, (bool available), (override));
  MOCK_METHOD(CefRefPtr<ArkWebClientExt>, GetCefClient, (), (const, override));
  MOCK_METHOD(void,
              GetImages,
              (std::shared_ptr<NWebBoolValueCallback> callback),
              (override));
  MOCK_METHOD(void, RemoveCache, (bool include_disk_files), (override));

#if BUILDFLAG(ARKWEB_NAVIGATION)
  MOCK_METHOD(std::shared_ptr<NWebHistoryList>, GetHistoryList, (), (override));
  MOCK_METHOD(std::vector<uint8_t>, SerializeWebState, (), (override));
  MOCK_METHOD(bool,
              RestoreWebState,
              (const std::vector<uint8_t>& state),
              (override));
#endif

#if BUILDFLAG(ARKWEB_MEDIA_MUTE_AUDIO)
  MOCK_METHOD(void, SetAudioMuted, (bool muted), (override));
#endif  // BUILDFLAG(ARKWEB_MEDIA_MUTE_AUDIO)

#if BUILDFLAG(ARKWEB_WEBRTC)
  MOCK_METHOD(void, StartCamera, (), (override));
  MOCK_METHOD(void, StopCamera, (), (override));
  MOCK_METHOD(void, CloseCamera, (), (override));
  MOCK_METHOD(void, ResumeMicrophone, (), (override));
  MOCK_METHOD(void, StopMicrophone, (), (override));
  MOCK_METHOD(void, PauseMicrophone, (), (override));
#endif  // BUILDFLAG(ARKWEB_WEBRTC)

#if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)
  MOCK_METHOD(void, PageUp, (bool top), (override));
  MOCK_METHOD(void, PageDown, (bool bottom), (override));
#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
  MOCK_METHOD(void, GetScrollOffset, (float*, float*), (override));
#endif
#endif  // #if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  MOCK_METHOD(void, ScrollTo, (float x, float y), (override));
  MOCK_METHOD(void, ScrollBy, (float delta_x, float delta_y), (override));
  MOCK_METHOD(void,
              ScrollByRefScreen,
              (float delta_x, float delta_y, float vx, float vy),
              (override));
  MOCK_METHOD(void, SlideScroll, (float vx, float vy), (override));
  MOCK_METHOD(bool,
              WebSendKeyEvent,
              (int32_t keyCode,
               int32_t keyAction,
               const std::vector<int32_t>& pressedCodes),
              (override));
  MOCK_METHOD(void,
              ScrollToWithAnime,
              (float x, float y, int32_t duration),
              (override));
  MOCK_METHOD(void,
              StopFling,
              (),
              (override));
  MOCK_METHOD(void,
              ScrollByWithAnime,
              (float delta_x, float delta_y, int32_t duration),
              (override));
  MOCK_METHOD(bool,
              SendKeyboardEvent,
              (const std::shared_ptr<OHOS::NWeb::NWebKeyboardEvent>&),
              (override));
  MOCK_METHOD(void,
              WebSendMouseEvent,
              (const std::shared_ptr<OHOS::NWeb::NWebMouseEvent>&),
              (override));
  MOCK_METHOD(void,
              WebSendMouseWheelEventV2,
              (double x,
               double y,
               double deltaX,
               double deltaY,
               const std::vector<int32_t>& pressedCodes,
               int32_t source),
              (override));

#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
  MOCK_METHOD(void, GetOverScrollOffset, (float*, float*), (override));
#endif
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_EXT_FORCE_ZOOM) || BUILDFLAG(ARKWEB_ZOOM)
  MOCK_METHOD(void, SetForceEnableZoom, (bool forceEnableZoom), (override));
#endif
#if BUILDFLAG(ARKWEB_EXT_FORCE_ZOOM)
  MOCK_METHOD(bool, GetForceEnableZoom, (), (override));
#endif

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
  MOCK_METHOD(void,
              SetShouldFrameSubmissionBeforeDraw,
              (bool should),
              (override));
  MOCK_METHOD(void,
              SetDrawRect,
              (int32_t x, int32_t y, int32_t width, int32_t height),
              (override));
  MOCK_METHOD(void, SetDrawMode, (int32_t mode), (override));
  MOCK_METHOD(bool, GetPendingSizeStatus, (), (override));
  MOCK_METHOD(void, SetFitContentMode, (int32_t mode), (override));
#endif  // BUILDFLAG(ARKWEB_COMPOSITE_RENDER)

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  MOCK_METHOD(void, SetNativeInnerWeb, (bool isInnerWeb), (override));
  MOCK_METHOD(bool, GetNativeEmbedMode, (), (override));
  MOCK_METHOD(bool, IsEnableCustomVideoPlayer, (), (override));
#endif

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
  MOCK_METHOD(void,
              SetAudioResumeInterval,
              (int32_t resumeInterval),
              (override));
  MOCK_METHOD(void, SetAudioExclusive, (bool audioExclusive), (override));
  MOCK_METHOD(void,
              SetAudioSessionType,
              (int32_t audioSessionType),
              (override));
  MOCK_METHOD(void, CloseAllMediaPresentations, (), (override));
  MOCK_METHOD(void, StopAllMedia, (), (override));
  MOCK_METHOD(void, ResumeAllMedia, (), (override));
  MOCK_METHOD(void, PauseAllMedia, (), (override));
  MOCK_METHOD(int, GetMediaPlaybackState, (), (override));
#endif  // BUILDFLAG(ARKWEB_MEDIA_POLICY)

#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
  MOCK_METHOD(void,
              PrefetchPage,
              (const PrefetchOptions& prefetch_options),
              (override));
#endif  // BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)

#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
  MOCK_METHOD(void, NotifyPopupWindowResult, (bool result), (override));
#endif  // BUILDFLAG(ARKWEB_MULTI_WINDOW)

#if BUILDFLAG(ARKWEB_CA)
  MOCK_METHOD(bool,
              GetCertChainDerData,
              (std::vector<std::string> & certChainData, bool isSingleCert),
              (override));
#endif
  MOCK_METHOD(void, SetWindowId, (uint32_t window_id), (override));

#if BUILDFLAG(ARKWEB_EXT_BLANK_TARGET_POPUP_INTERCEPT)
  MOCK_METHOD(void,
              SetEnableBlankTargetPopupIntercept,
              (bool enableBlankTargetPopup),
              ());
#endif

#if BUILDFLAG(ARKWEB_PRINT)
  MOCK_METHOD(void, SetToken, (void* token), (override));
  MOCK_METHOD(void*,
              CreateWebPrintDocumentAdapter,
              (const std::string& jobName),
              (override));
  MOCK_METHOD(void*,
              CreateWebPrintDocumentAdapterV2,
              (const std::string& jobName),
              (override));
  MOCK_METHOD(void, SetPrintBackground, (bool enable), (override));
  MOCK_METHOD(bool, GetPrintBackground, (), (override));
#endif  // BUILDFLAG(ARKWEB_PRINT)

#if BUILDFLAG(ARKWEB_SCREEN_ROTATION)
  MOCK_METHOD(void, SetVirtualPixelRatio, (float ratio), (override));
#endif  // BUILDFLAG(ARKWEB_SCREEN_ROTATION)

#if BUILDFLAG(IS_OHOS)
  MOCK_METHOD(void,
              UpdateNativeEmbedInfo,
              (std::shared_ptr<NWebNativeEmbedDataInfo> info),
              (override));
#endif

#ifdef OHOS_EX_TOPCONTROLS
  MOCK_METHOD(void,
              UpdateBrowserControlsState,
              (int constraints, int current, bool animate),
              (const, override));
  MOCK_METHOD(void,
              UpdateBrowserControlsHeight,
              (int height, bool animate),
              (override));
#endif

#if BUILDFLAG(ARKWEB_POST_URL)
  MOCK_METHOD(int,
              PostUrl,
              (const std::string& url, const std::vector<char>& postData),
              (override));
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  MOCK_METHOD(void,
              SetVirtualKeyBoardArg,
              (int32_t width, int32_t height, double keyboard),
              (override));
  MOCK_METHOD(bool, ShouldVirtualKeyboardOverlay, (), (override));
  MOCK_METHOD(void,
              WebSendMouseWheelEvent,
              (double, double, double, double, const std::vector<int32_t>&),
              (override));
  MOCK_METHOD(void,
              WebSendTouchpadFlingEvent,
              (double x,
               double y,
               double vx,
               double vy,
               const std::vector<int32_t>& pressedCodes),
              (override));
#endif

#if BUILDFLAG(IS_OHOS)
  MOCK_METHOD(bool, IsSafeBrowsingEnabled, (), (override));
  MOCK_METHOD(void, EnableSafeBrowsing, (bool enable), (override));
  MOCK_METHOD(void,
              PrecompileJavaScript,
              (const std::string& url,
               const std::string& script,
               std::shared_ptr<CacheOptions>& cacheOptions,
               std::shared_ptr<NWebMessageValueCallback> callback),
              (override));
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  MOCK_METHOD(int,
              PrerenderPage,
              (const std::string& url, const std::string& additional_headers),
              (override));
  MOCK_METHOD(void, CancelAllPrerendering, (), (override));
#endif

#if BUILDFLAG(ARKWEB_SECURITY_STATE)
  MOCK_METHOD(int, GetSecurityLevel, (), (override));
#endif

  MOCK_METHOD(void,
              SetAccessibilityState,
              (cef_state_t accessibilityState),
              (override));
  MOCK_METHOD2(ExecuteAction, void(int64_t, uint32_t));
  MOCK_METHOD3(ExecuteAction,
               bool(int64_t,
                    uint32_t,
                    const std::map<std::string, std::string>&));
  MOCK_METHOD(std::shared_ptr<NWebAccessibilityNodeInfo>,
              GetFocusedAccessibilityNodeInfo,
              (int64_t accessibilityId, bool isAccessibilityFocus),
              (override));
  MOCK_METHOD(std::shared_ptr<NWebAccessibilityNodeInfo>,
              GetAccessibilityNodeInfoById,
              (int64_t accessibilityId),
              (override));
  MOCK_METHOD(std::shared_ptr<NWebAccessibilityNodeInfo>,
              GetAccessibilityNodeInfoByFocusMove,
              (int64_t accessibilityId, int32_t direction),
              (override));

#if BUILDFLAG(ARKWEB_SECURE_JAVASCRIPT_PROXY)
  MOCK_METHOD(std::string,
              GetLastJavascriptProxyCallingFrameUrl,
              (),
              (override));
#endif

#if BUILDFLAG(ARKWEB_ITP)
  MOCK_METHOD(void,
              EnableIntelligentTrackingPrevention,
              (bool enable),
              (override));
  MOCK_METHOD(bool,
              IsIntelligentTrackingPreventionEnabled,
              (),
              (const, override));
#endif

  MOCK_METHOD(int,
              ScaleGestureChange,
              (double scale, double centerX, double centerY),
              (const, override));

#if BUILDFLAG(ARKWEB_SCREEN_LOCK)
  MOCK_METHOD(void,
              SetWakeLockCallback,
              (int32_t windowId,
               const std::shared_ptr<NWebScreenLockCallback>& callback),
              (override));
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  MOCK_METHOD(void,
              RegisterOnCreateNativeMediaPlayerListener,
              (std::shared_ptr<NWebCreateNativeMediaPlayerCallback> callback),
              (override));
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_CLIPBOARD)
  MOCK_METHOD(void, SetIsRichText, (bool is_rich_text), (override));
  MOCK_METHOD(std::string, GetSelectInfo, (), (override));
#endif

#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
  MOCK_METHOD(void,
              OnSafeInsetsChange,
              (int left, int top, int right, int bottom),
              (override));
#endif

#if BUILDFLAG(ARKWEB_AI)
  MOCK_METHOD(void, OnTextSelected, (), (override));
#endif

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  MOCK_METHOD(void, EnableWholeWebPageDrawing, (), ());
  MOCK_METHOD(bool,
              WebPageSnapshot,
              (const char* id,
               PixelUnit type,
               int width,
               int height,
               const WebSnapshotCallback callback),
              (override));
#endif

#if BUILDFLAG(ARKWEB_URL_TRUST_LIST)
  MOCK_METHOD(int,
              SetUrlTrustListWithErrMsg,
              (const std::string& urlTrustList, std::string& detailErrMsg),
              (override));
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  MOCK_METHOD(void,
              SetPathAllowingUniversalAccess,
              (const std::vector<std::string>& path_list,
               const std::vector<std::string>& excluded_path_list),
              (override));
#endif

#if BUILDFLAG(ARKWEB_EXT_FILE_ACCESS)
  MOCK_METHOD(void,
              DisallowSandboxFileAccessFromFileUrl,
              (bool disallow),
              (override));
#endif

#if BUILDFLAG(ARKWEB_BFCACHE)
  MOCK_METHOD(void,
              SetBackForwardCacheOptions,
              (int32_t size, int32_t timeToLive),
              (override));
  MOCK_METHOD(void,
              SetMediaResumeFromBFCachePage,
              (bool resume),
              (override));
#endif

  MOCK_METHOD(bool, IsCustomKeyboard, (), (const, override));

  MOCK_METHOD(std::shared_ptr<NWebCustomKeyboardHandlerImpl>,
              GetCustomKeyboardHandler,
              (),
              (const, override));

  MOCK_METHOD(void,
              SendAccessibilityHoverEvent,
              (int x, int y, bool isHoverEnter),
              (override));
  MOCK_METHOD(void, RefreshAccessibilityManagerClickEvent, (), (override));
#if BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)
  MOCK_METHOD(void,
              StopScreenCapture,
              (int32_t nweb_id, const char* session_id),
              (override));

  MOCK_METHOD(void, SetScreenCapturePickerShow, (), (override));

  MOCK_METHOD(void, DisableSessionReuse, (), (override));
#endif
#if BUILDFLAG(ARKWEB_EX_REFRESH_IFRAME)
  MOCK_METHOD(bool, WebExtensionContextMenuIsIframe, (), (override));
  MOCK_METHOD(void, WebExtensionContextMenuReloadFocusedFrame, (), (override));
#endif
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  MOCK_METHOD(void,
              WebExtensionContextMenuGetFocusedFrameInfo,
              (int32_t& frame_id, std::string& frame_url),
              (override));
#endif
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  MOCK_METHOD(bool, SetFocusByPosition, (float x, float y), (override));
  MOCK_METHOD((std::pair<double, double>), GetLastTouchMousePosition, (), (override));
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_ACTIVE_POLICY)
  MOCK_METHOD(void,
              SetDelayDurationForBackgroundTabFreezing,
              (int64_t delay),
              (override));
#endif

#if BUILDFLAG(ARKWEB_JSPROXY)
  MOCK_METHOD(void,
              JavaScriptOnDocumentStartByOrder,
              (const ScriptItems& ScriptItems,
               const ScriptItemsByOrder& ScriptItemsByOrder),
              (override));
  MOCK_METHOD(void,
              JavaScriptOnDocumentEndByOrder,
              (const ScriptItems& ScriptItems,
               const ScriptItemsByOrder& ScriptItemsByOrder),
              (override));
  MOCK_METHOD(void,
              JavaScriptOnHeadReadyByOrder,
              (const ScriptItems& ScriptItems,
               const ScriptItemsByOrder& ScriptItemsByOrder),
              (override));
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
  MOCK_METHOD(void,
              SetAdBlockEnabledForSite,
              (bool is_adblock_enabled, int main_frame_tree_node_id),
              (override));
#endif
  MOCK_METHOD(void,
              PutOptimizeParserBudgetEnabled,
              (bool enable),
              (const, override));
  MOCK_METHOD(void, WebComponentsBlur, (), (override));
  MOCK_METHOD(void, SetEnableHalfFrameRate, (bool enabled), (override));
  MOCK_METHOD(std::shared_ptr<HitTestResult>,
              GetLastHitTestResult,
              (),
              (const, override));
  MOCK_METHOD(void,
              RegisterNativeJSProxyWithResult,
              (const std::string& objName,
               const std::vector<std::string>& methodName,
               std::vector<std::function<std::shared_ptr<OHOS::NWeb::NWebValue>(
                   std::vector<std::vector<uint8_t>>&,
                   std::vector<size_t>&)>>&& callback,
               bool isAsync,
               const std::string& permission),
              (override));
  MOCK_METHOD(void,
              RegisterNativeScrollCallback,
              (std::function<void(double, double)> && callback),
              (override));

#if BUILDFLAG(ARKWEB_PDF)
  MOCK_METHOD(void, EraseCreatePDFCallbackImpl, (uint32_t id), (override));
#endif  // BUILDFLAG(ARKWEB_PDF)

#if BUILDFLAG(ARKWEB_SAFEBROWSING)
  MOCK_METHOD(void,
              EnableSafeBrowsingDetection,
              (bool enable, bool strictMode),
              (override));
#endif  // BUILDFLAG(ARKWEB_SAFEBROWSING)

#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
  MOCK_METHOD(int,
              InsertBackForwardEntry,
              (int index, const std::string& url),
              (override));
  MOCK_METHOD(int,
              UpdateNavigationEntryUrl,
              (int index, const std::string& url),
              (override));
  MOCK_METHOD(void, ClearForwardList, (), (override));
#endif

#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
  MOCK_METHOD(bool,
              GetAccessibilityNodeRectById,
              (int64_t accessibilityId,
               int32_t* width,
               int32_t* height,
               int32_t* offsetX,
               int32_t* offsetY),
              (override));
  MOCK_METHOD(int64_t,
              GetWebAccessibilityIdByHtmlElementId,
              (const std::string& htmlElementId),
              (override));
  MOCK_METHOD(bool,
              GetAccessibilityVisible,
              (int64_t accessibilityId),
              (override));
#endif

#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
  MOCK_METHOD(NWebDownloadItemState,
              GetDownloadItemState,
              (long item_id),
              (override));
#endif

  MOCK_METHOD(int,
              ScaleGestureChangeV2,
              (int type,
               double scale,
               double originScale,
               double centerX,
               double centerY),
              (const, override));

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  MOCK_METHOD(void, EnableVideoAssistant, (bool enable), (override));
  MOCK_METHOD(void,
              ExecuteVideoAssistantFunction,
              (const std::string& cmd_id),
              (override));
  MOCK_METHOD(void, CustomWebMediaPlayer, (bool enable), (override));
  MOCK_METHOD(void, WebMediaPlayerControllerPlay, (), (override));
  MOCK_METHOD(void, WebMediaPlayerControllerPause, (), (override));
  MOCK_METHOD(void, WebMediaPlayerControllerSeek, (double time), (override));
  MOCK_METHOD(void, WebMediaPlayerControllerSetMuted, (bool muted), (override));
  MOCK_METHOD(void,
              WebMediaPlayerControllerSetPlaybackRate,
              (double playback_rate),
              (override));
  MOCK_METHOD(void, WebMediaPlayerControllerExitFullscreen, (), (override));
  MOCK_METHOD(void,
              WebMediaPlayerControllerSetVideoSurface,
              (void* native_window),
              (override));
  MOCK_METHOD(void, WebMediaPlayerControllerDownload, (), (override));
  MOCK_METHOD(void,
              WebMediaPlayerControllerSetVolume,
              (double volume),
              (override));
  MOCK_METHOD(double, WebMediaPlayerControllerGetVolume, (), (override));
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_AI)
  MOCK_METHOD(void, OnDestroyImageAnalyzerOverlay, (), (override));
  MOCK_METHOD(void, OnFoldStatusChanged, (FoldStatus foldstatus), (override));
#endif
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  MOCK_METHOD(void,
              WebExtensionTabUpdated,
              (int tab_id,
               std::unique_ptr<NWebExtensionTabChangeInfo> changeInfo,
               std::unique_ptr<NWebExtensionTab> tab),
              (override));
  MOCK_METHOD(void,
              WebExtensionTabAttached,
              (int tab_id,
               std::unique_ptr<NWebExtensionTabAttachInfo> attachInfo),
              (override));
  MOCK_METHOD(void,
              WebExtensionTabDetached,
              (int tab_id,
               std::unique_ptr<NWebExtensionTabDetachInfo> detachInfo),
              (override));
  MOCK_METHOD(void,
              WebExtensionTabHighlighted,
              (NWebExtensionTabHighlightInfo & highlightInfo),
              (override));
  MOCK_METHOD(void,
              WebExtensionTabMoved,
              (int32_t tab_id,
               std::unique_ptr<NWebExtensionTabMoveInfo> moveInfo),
              (override));
  MOCK_METHOD(void,
              WebExtensionTabReplaced,
              (int32_t addedTabId, int32_t removedTabId),
              (override));
  MOCK_METHOD(void,
              WebExtensionTabRemoved,
              (int32_t tab_id, bool isWindowClosing, int32_t windowId),
              (override));
  MOCK_METHOD(void, WebExtensionSetViewType, (int32_t type), (override));
#endif

#if BUILDFLAG(ARKWEB_EXT_PERMISSION)
  MOCK_METHOD(void,
              PermissionRequestGrant,
              (int32_t resourse_id, int nweb_request_key),
              (override));
  MOCK_METHOD(void, PermissionRequestDeny, (int nweb_request_key), (override));
  MOCK_METHOD(std::string,
              PermissionRequestGetOrigin,
              (int nweb_request_key),
              (override));
  MOCK_METHOD(int32_t,
              PermissionRequestGetResourceId,
              (int nweb_request_key),
              (override));
  MOCK_METHOD(void,
              PermissionRequestDelete,
              (int nweb_request_key),
              (override));
#endif  // ARKWEB_EXT_PERMISSION

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  MOCK_METHOD(void, SetWholePageDrawing, (), (override));
#endif  // ARKWEB_SOFTWARE_COMPOSITOR
  MOCK_METHOD(void, SetSurfaceDensity, (const double& density), (override));
  MOCK_METHOD(void,
              OpenDevtoolsWith,
              (std::shared_ptr<NWebDelegateInterface> nweb_delegate,
               std::unique_ptr<OpenDevToolsParam> param),
              (override));
  MOCK_METHOD(void,
              OpenDevtoolsWithByPb,
              (std::shared_ptr<NWebDelegateInterface> nweb_delegate,
               std::unique_ptr<OpenDevToolsParam> param,
               OpenDevToolsExtOpt* ext_opt),
              (override));
  MOCK_METHOD(void, CloseDevtools, (), (override));

#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
  MOCK_METHOD(void, SetTransformHint, (uint32_t rotation), (override));
#endif

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
  MOCK_METHOD(void, SetUsageScenario, (int32_t usage_scenario), (override));
  MOCK_METHOD(int32_t, GetUsageScenario, (), (override));
#endif

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
  MOCK_METHOD(void, MaximizeResize, (), (override));
#endif  // ARKWEB_MAXIMIZE_RESIZE

#if BUILDFLAG(ARKWEB_DISATCH_BEFORE_UNLOAD)
  MOCK_METHOD(bool, NeedToFireBeforeUnloadOrUnloadEvents, (), (override));
  MOCK_METHOD(void, DispatchBeforeUnload, (), (override));
#endif  // ARKWEB_DISATCH_BEFORE_UNLOAD

#if BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)
  MOCK_METHOD(void,
              RegisterScreenCaptureDelegateListener,
              (std::shared_ptr<NWebScreenCaptureDelegateCallback>
                   screenCaptureDelegateListener),
              (override));
#endif  // defined(ARKWEB_EX_SCREEN_CAPTURE)

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  std::string GetSelectedTextFromContextParam() override {}
#endif  // ARKWEB_EXT_FREE_COPY

#if BUILDFLAG(ARKWEB_SCREEN_OFFSET)
  MOCK_METHOD(void, SetScreenOffset, (double x, double y), (override));
#endif  // BUILDFLAG(ARKWEB_SCREEN_OFFSET)

#if BUILDFLAG(ARKWEB_NWEB_EX)
  MOCK_METHOD(void,
              EnableViewAutoResize,
              (const CefSize& min_size, const CefSize& max_size),
              (override));
  MOCK_METHOD(void, DisableViewAutoResize, (), (override));
#endif

#if BUILDFLAG(ARKWEB_PIP)
  MOCK_METHOD(
      void,
      SetPipNativeWindow,
      (int delegate_id, int child_id, int frame_routing_id, void* window),
      (override));
  MOCK_METHOD(void,
              SendPipEvent,
              (int delegate_id, int child_id, int frame_routing_id, int event),
              (override));
#endif
#if BUILDFLAG(ARKWEB_BGTASK)
  void OnBrowserForeground() override {}
  void OnBrowserBackground() override {}
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  MOCK_METHOD(void,
              SetBlanklessLoadingKey,
              (uint32_t nweb_id, uint64_t blankless_key),
              (override));
  MOCK_METHOD(int64_t, GetPreferenceHash, (), (override));
  MOCK_METHOD(int32_t, GetWidth, (), (override));
  MOCK_METHOD(int32_t, GetHeight, (), (override));
#endif

#if BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
  MOCK_METHOD(void,
              SetBlankScreenDetectionConfig,
              (bool enable,
               const std::vector<double>& detectionTiming,
               const std::vector<int32_t>& detectionMethods,
               int32_t contentfulNodesCountThreshold),
              (override));
#endif

#if BUILDFLAG(ARKWEB_MENU)
  MOCK_METHOD(void, UpdateSingleHandleVisible, (bool isVisible), (override));
  MOCK_METHOD(void,
              SetTouchHandleExistState,
              (bool touchHandleExist),
              (override));
  MOCK_METHOD(void, SetViewportScaleState, (bool viewportScale), (override));
#endif

#if BUILDFLAG(ARKWEB_VIEWPORT_AVOID)
  MOCK_METHOD(void,
              AvoidVisibleViewportBottom,
              (int32_t avoidHeight),
              (override));
  MOCK_METHOD(int32_t, GetVisibleViewportAvoidHeight, (), (override));
#endif

#if BUILDFLAG(ARKWEB_NWEB_EX)
  MOCK_METHOD(void,
              RunJavaScriptInFrames,
              (RunJavaScriptParam,
               OnReceiveValueCallback callback),
              (override));
#endif

#if BUILDFLAG(ARKWEB_READER_MODE)
  MOCK_METHOD(void, Distill, (const std::string& guid, const DistillOptions& distill_options,
    DistillCallback callback), (override));
  MOCK_METHOD(void, AbortDistill, (), (override));
#endif // ARKWEB_READER_MODE

#if BUILDFLAG(ARKWEB_EXT_HTTPS_UPGRADES)
  MOCK_METHOD(int,
              LoadUrlWithParams,
              (const std::string& url,
               const LoadUrlType load_type,
               const std::string& refer,
               const std::string& headers,
               const std::string& post_data,
               const bool allow_https_upgrade,
               int32_t transition_type),
              (override));
  MOCK_METHOD(void, EnableHttpsUpgrades, (bool enable), (override));                               
#endif

#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
  MOCK_METHOD(void, SetFocusWebId, (int32_t nweb_id), (override));
#endif 
};
}  // namespace OHOS::NWeb

#endif // MOCK_NWEB_DELEGATE_H