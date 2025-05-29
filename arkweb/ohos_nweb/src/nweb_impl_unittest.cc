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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <unistd.h>

#include <cerrno>
#include <iostream>
#include <map>
#include <string>
#include <thread>

#include "arkweb/build/features/features.h"
#include "build/build_config.h"
#include "nweb.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#define private public
#include "nweb_impl.h"

using namespace testing;
using namespace OHOS::NWeb;

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
              (override));
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
              ExecuteCreatePDFExt,
              (std::shared_ptr<NWebPDFConfigArgs> pdfConfig,
               std::shared_ptr<NWebArrayBufferValueCallback> callback),
              (override));
  MOCK_METHOD(bool,
              ScrollByWithResult,
              (float delta_x, float delta_y),
              (override));
#if BUILDFLAG(ARKWEB_MIXED_CONTENT)
  MOCK_METHOD(void, EnableMixedContentAutoUpgrades, (bool enable), (override));
  MOCK_METHOD(bool, IsMixedContentAutoUpgradesEnabled, (), (override));
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
  MOCK_METHOD(void, UnRegisterWebAppClientExtensionListener, (), (override));
  MOCK_METHOD(void,
              RegisterWebExtensionListener,
              (std::shared_ptr<NWebExtensionCallback> web_extension_listener),
              (override));
  MOCK_METHOD(void, UnRegisterWebExtensionListener, (), (override));
  MOCK_METHOD(void, GetImageFromContextNode, (), (override));
  MOCK_METHOD(void, GetImageFromCache, (const std::string& url), (override));
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
              (RotationType rotation,
               DisplayOrientation orientation,
               bool isWebinitialization),
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
  MOCK_METHOD(CefRefPtr<CefClient>, GetCefClient, (), (const, override));
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
#endif  // BUILDFLAG(ARKWEB_WEBRTC)

#if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)
  MOCK_METHOD(void, PageUp, (bool top), (override));
  MOCK_METHOD(void, PageDown, (bool bottom), (override));
#ifdef OHOS_GET_SCROLL_OFFSET
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
              ScrollByWithAnime,
              (float delta_x, float delta_y, int32_t duration),
              (override));
#if defined(OHOS_GET_SCROLL_OFFSET)
  MOCK_METHOD(void, GetOverScrollOffset, (float*, float*), (override));
#endif
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_EXT_FORCE_ZOOM)
  MOCK_METHOD(void, SetForceEnableZoom, (bool forceEnableZoom), (override));
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

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
  MOCK_METHOD(void,
              SetAudioResumeInterval,
              (int32_t resumeInterval),
              (override));
  MOCK_METHOD(void, SetAudioExclusive, (bool audioExclusive), (override));
  MOCK_METHOD(void, CloseAllMediaPresentations, (), (override));
  MOCK_METHOD(void, StopAllMedia, (), (override));
  MOCK_METHOD(void, ResumeAllMedia, (), (override));
  MOCK_METHOD(void, PauseAllMedia, (), (override));
  MOCK_METHOD(int, GetMediaPlaybackState, (), (override));
#endif  // BUILDFLAG(ARKWEB_MEDIA_POLICY)

#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
  MOCK_METHOD(void,
              PrefetchPage,
              (const std::string&, (const std::map<std::string, std::string>&)),
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
              (override));
#endif

#if BUILDFLAG(ARKWEB_PRINT)
  MOCK_METHOD(void, SetToken, (void* token), (override));
  MOCK_METHOD(void*,
              CreateWebPrintDocumentAdapter,
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
              (override));
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

#if BUILDFLAG(ARKWEB_EXT_SECURITY_STATE)
  MOCK_METHOD(int, GetSecurityLevel, (), (override));
#endif

  MOCK_METHOD(void,
              SetAccessibilityState,
              (cef_state_t accessibilityState),
              (override));
  MOCK_METHOD2(ExecuteAction, void(int64_t, uint32_t));
  MOCK_METHOD3(ExecuteAction,
               void(int64_t,
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

#ifdef OHOS_EX_DOWNLOAD
  MOCK_METHOD(NWebDownloadItemState,
              GetDownloadItemState,
              (long item_id),
              (override));
#endif

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
  MOCK_METHOD(void, EnableWholeWebPageDrawing, (), (override));
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
              (const std::vector<std::string>& pathList),
              (override));
#endif

#if BUILDFLAG(ARKWEB_BFCACHE)
  MOCK_METHOD(void,
              SetBackForwardCacheOptions,
              (int32_t size, int32_t timeToLive),
              (override));
#endif

  MOCK_METHOD(bool, IsCustomKeyboard, (), (const, override));

  MOCK_METHOD(std::shared_ptr<NWebCustomKeyboardHandlerImpl>,
              GetCustomKeyboardHandler,
              (),
              (const, override));

  MOCK_METHOD(void, SendAccessibilityHoverEvent, (int x, int y), (override));
  MOCK_METHOD(void, RefreshAccessibilityManagerClickEvent, (), (override));
#if BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)
  MOCK_METHOD(void,
              StopScreenCapture,
              (int32_t nweb_id, const char* session_id),
              (override));
#endif
#if BUILDFLAG(ARKWEB_EX_REFRESH_IFRAME)
  MOCK_METHOD(void, WebExtensionContextMenuIsIframe, (), (override));
  MOCK_METHOD(bool, WebExtensionContextMenuReloadFocusedFrame, (), (override));
#endif
};

class MockNWebDragEvent : public NWebDragEvent {
 public:
  ~MockNWebDragEvent() = default;
  MOCK_METHOD(DragAction, GetAction, (), (override));
  MOCK_METHOD(double, GetX, (), (override));
  MOCK_METHOD(double, GetY, (), (override));
};

class MockNWebInputHandler : public NWebInputHandler {
 public:
  MockNWebInputHandler(std::shared_ptr<NWebDelegateInterface> nweb_delegate)
      : NWebInputHandler(nweb_delegate) {}

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  MOCK_METHOD3(WebSendKeyEvent,
               bool(int32_t, int32_t, const std::vector<int32_t>&));
  MOCK_METHOD5(
      WebSendMouseWheelEvent,
      void(double, double, double, double, const std::vector<int32_t>&));
  MOCK_METHOD5(
      WebSendTouchpadFlingEvent,
      void(double, double, double, double, const std::vector<int32_t>&));
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
};

class MockNWebDragData : public NWebDragData {
 public:
  MOCK_METHOD(std::string, GetLinkURL, (), (override));
  MOCK_METHOD(std::string, GetFragmentText, (), (override));
  MOCK_METHOD(std::string, GetFragmentHtml, (), (override));
  MOCK_METHOD(bool,
              GetPixelMapSetting,
              (const void** data, size_t& len, int& width, int& height),
              (override));
  MOCK_METHOD(bool, SetFragmentHtml, (const std::string& html), (override));
  MOCK_METHOD(bool,
              SetPixelMapSetting,
              (const void* data, size_t len, int width, int height),
              (override));
  MOCK_METHOD(bool, SetLinkURL, (const std::string& url), (override));
  MOCK_METHOD(bool, SetFragmentText, (const std::string& Text), (override));
  MOCK_METHOD(std::string, GetLinkTitle, (), (override));
  MOCK_METHOD(bool, SetLinkTitle, (const std::string& title), (override));
  MOCK_METHOD(void, GetDragStartPosition, (int& x, int& y), (override));
  MOCK_METHOD(bool, IsSingleImageContent, (), (override));
  MOCK_METHOD(bool, SetFileUri, (const std::string& uri), (override));
  MOCK_METHOD(std::string, GetImageFileName, (), (override));
  MOCK_METHOD(void, ClearImageFileNames, (), (override));
};

class MockNWebInputMethodHandler : public NWebInputMethodHandler {
 public:
#if BUILDFLAG(ARKWEB_CLIPBOARD)
  MOCK_METHOD0(GetSelectInfo, std::string());
#endif
};

class MockNWebCreateInfo : public NWebCreateInfo {
 public:
  ~MockNWebCreateInfo() = default;
  MOCK_METHOD(uint32_t, GetWidth, (), (override));
  MOCK_METHOD(uint32_t, GetHeight, (), (override));
  MOCK_METHOD(std::shared_ptr<NWebOutputFrameCallback>,
              GetOutputFrameCallback,
              (),
              (override));
  MOCK_METHOD(std::shared_ptr<NWebEngineInitArgs>,
              GetEngineInitArgs,
              (),
              (override));
  MOCK_METHOD(void*, GetProducerSurface, (), (override));
  MOCK_METHOD(void*, GetEnhanceSurfaceInfo, (), (override));
  MOCK_METHOD(bool, GetIsIncognitoMode, (), (override));
};

class NWebImplTest : public ::testing::Test {
 public:
  static void SetUpTestCase(void);
  static void TearDownTestCase(void);
  void SetUp(void);
  void TearDown(void);
  int32_t id = 1;
  std::shared_ptr<NWebImpl> nweb_impl_;
  std::shared_ptr<MockNWebDelegate> mock_delegate_;
  std::shared_ptr<MockNWebInputHandler> input_handler_;
};

void NWebImplTest::SetUpTestCase(void) {}

void NWebImplTest::TearDownTestCase(void) {}

void NWebImplTest::SetUp() {
  mock_delegate_ = std::make_shared<MockNWebDelegate>();
  nweb_impl_ = std::make_shared<NWebImpl>(id++);
  ASSERT_NE(nweb_impl_, nullptr);
}

void NWebImplTest::TearDown() {
  nweb_impl_ = nullptr;
}

TEST_F(NWebImplTest, NWebImplTest_InitialScale_001) {
  float scale = 1.5f;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, InitialScale(scale)).Times(1);

  nweb_impl_->InitialScale(scale);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_InitialScale_002) {
  float scale = 1.5f;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, InitialScale(scale)).Times(0);

  nweb_impl_->InitialScale(scale);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_Scale_001) {
  float scale = 1.5f;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, Scale()).WillOnce(::testing::Return(scale));

  auto result = nweb_impl_->Scale();
  EXPECT_EQ(result, scale);
}

TEST_F(NWebImplTest, NWebImplTest_Scale_002) {
  float scale = 1.5f;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, InitialScale(scale)).Times(0);

  auto result = nweb_impl_->Scale();
  EXPECT_EQ(result, 0);
}

TEST_F(NWebImplTest, NWebImplTest_OnFocus_001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, IsCustomKeyboard()).Times(0);

  nweb_impl_->OnFocus(OHOS::NWeb::FocusReason::FOCUS_DEFAULT);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_OnBlur_001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, OnBlur()).Times(0);

  nweb_impl_->OnBlur(OHOS::NWeb::BlurReason::FOCUS_SWITCH);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_SendDragEvent_001) {
  auto dragEvent = std::make_shared<MockNWebDragEvent>();
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*dragEvent, GetAction()).Times(0);
  EXPECT_CALL(*dragEvent, GetX()).Times(0);
  EXPECT_CALL(*dragEvent, GetY()).Times(0);
  EXPECT_CALL(*mock_delegate_, SendDragEvent(::testing::_)).Times(0);

  nweb_impl_->SendDragEvent(dragEvent);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
TEST_F(NWebImplTest, NWebImplTest_ScrollTo_001) {
  float x = 1.5f;
  float y = 2.5f;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ScrollTo(::testing::_, ::testing::_)).Times(0);

  nweb_impl_->ScrollTo(x, y);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_ScrollTo_002) {
  float x = 1.5f;
  float y = 2.5f;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ScrollTo(::testing::_, ::testing::_)).Times(1);

  nweb_impl_->ScrollTo(x, y);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_ScrollBy_001) {
  float x = 1.5f;
  float y = 2.5f;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ScrollBy(::testing::_, ::testing::_)).Times(0);

  nweb_impl_->ScrollBy(x, y);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_ScrollBy_002) {
  float x = 1.5f;
  float y = 2.5f;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ScrollBy(::testing::_, ::testing::_)).Times(1);

  nweb_impl_->ScrollBy(x, y);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_SlideScroll_001) {
  float x = 1.5f;
  float y = 2.5f;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SlideScroll(::testing::_, ::testing::_))
      .Times(0);

  nweb_impl_->SlideScroll(x, y);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_SlideScroll_002) {
  float x = 1.5f;
  float y = 2.5f;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SlideScroll(::testing::_, ::testing::_))
      .Times(1);

  nweb_impl_->SlideScroll(x, y);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_WebSendKeyEvent_001) {
  int32_t keyCode = 1;
  int32_t keyAction = 1;
  std::vector<int32_t> pressedCodes = {1, 2, 3};
  input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  ON_CALL(*input_handler_, WebSendKeyEvent(keyCode, keyAction, pressedCodes))
      .WillByDefault(::testing::Return(true));
  nweb_impl_->input_handler_ = nullptr;

  auto ret = nweb_impl_->WebSendKeyEvent(keyCode, keyAction, pressedCodes);
  EXPECT_EQ(ret, false);
}

TEST_F(NWebImplTest, NWebImplTest_WebSendMouseWheelEvent_001) {
  double x = 1.0;
  double y = 2.0;
  double deltaX = 3.0;
  double deltaY = 4.0;
  std::vector<int32_t> pressedCodes = {1, 2, 3};
  nweb_impl_->input_handler_ = nullptr;
  input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  EXPECT_CALL(*input_handler_,
              WebSendMouseWheelEvent(x, y, deltaX, deltaY, pressedCodes))
      .Times(0);

  nweb_impl_->WebSendMouseWheelEvent(x, y, deltaX, deltaY, pressedCodes);
  EXPECT_EQ(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_WebSendTouchpadFlingEvent_001) {
  double x = 1.0;
  double y = 2.0;
  double deltaX = 3.0;
  double deltaY = 4.0;
  std::vector<int32_t> pressedCodes = {1, 2, 3};
  nweb_impl_->input_handler_ = nullptr;
  input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  EXPECT_CALL(*input_handler_,
              WebSendTouchpadFlingEvent(x, y, deltaX, deltaY, pressedCodes))
      .Times(0);

  nweb_impl_->WebSendTouchpadFlingEvent(x, y, deltaX, deltaY, pressedCodes);
  EXPECT_EQ(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_ScrollToWithAnime_001) {
  float x = 1.5f;
  float y = 2.5f;
  int32_t duration_ = 1000;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_,
              ScrollToWithAnime(::testing::_, ::testing::_, ::testing::_))
      .Times(0);

  nweb_impl_->ScrollToWithAnime(x, y, duration_);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_ScrollToWithAnime_002) {
  float x = 1.5f;
  float y = 2.5f;
  int32_t duration_ = 1000;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_,
              ScrollToWithAnime(::testing::_, ::testing::_, ::testing::_))
      .Times(1);

  nweb_impl_->ScrollToWithAnime(x, y, duration_);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_ScrollByWithAnime_001) {
  float x = 1.5f;
  float y = 2.5f;
  int32_t duration_ = 1000;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_,
              ScrollByWithAnime(::testing::_, ::testing::_, ::testing::_))
      .Times(0);

  nweb_impl_->ScrollByWithAnime(x, y, duration_);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_ScrollByWithAnime_002) {
  float x = 1.5f;
  float y = 2.5f;
  int32_t duration_ = 1000;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_,
              ScrollByWithAnime(::testing::_, ::testing::_, ::testing::_))
      .Times(1);

  nweb_impl_->ScrollByWithAnime(x, y, duration_);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif

TEST_F(NWebImplTest, NWebImplTest_NotifyForNextTouchEvent_001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, NotifyForNextTouchEvent()).Times(0);

  nweb_impl_->NotifyForNextTouchEvent();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_NotifyForNextTouchEvent_002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, NotifyForNextTouchEvent()).Times(1);

  nweb_impl_->NotifyForNextTouchEvent();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_SetNestedScrollMode_001) {
  auto ret = OHOS::NWeb::NestedScrollMode::SELF_ONLY;
  nweb_impl_->SetNestedScrollMode(ret);
  EXPECT_NE(nweb_impl_, nullptr);
}

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
TEST_F(NWebImplTest, NWebImplTest_SetVirtualKeyBoardArg_001) {
  int32_t width = 1;
  int32_t height = 2;
  double keyboard = 1.1;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_,
              SetVirtualKeyBoardArg(::testing::_, ::testing::_, ::testing::_))
      .Times(0);

  nweb_impl_->SetVirtualKeyBoardArg(width, height, keyboard);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_SetVirtualKeyBoardArg_002) {
  int32_t width = 1;
  int32_t height = 2;
  double keyboard = 1.1;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_,
              SetVirtualKeyBoardArg(::testing::_, ::testing::_, ::testing::_))
      .Times(1);

  nweb_impl_->SetVirtualKeyBoardArg(width, height, keyboard);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_ShouldVirtualKeyboardOverlay_001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ShouldVirtualKeyboardOverlay()).Times(0);

  nweb_impl_->ShouldVirtualKeyboardOverlay();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_ShouldVirtualKeyboardOverlay_002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ShouldVirtualKeyboardOverlay()).Times(1);

  nweb_impl_->ShouldVirtualKeyboardOverlay();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif

#if BUILDFLAG(ARKWEB_DRAG_DROP)
TEST_F(NWebImplTest, NWebImplTest_GetOrCreateDragData_001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  auto drag_data_mock = std::make_shared<MockNWebDragData>();
  ON_CALL(*mock_delegate_, GetOrCreateDragData())
      .WillByDefault(::testing::Return(drag_data_mock));

  auto ret = nweb_impl_->GetOrCreateDragData();
  EXPECT_EQ(ret, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_GetOrCreateDragData_002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  auto drag_data_mock = std::make_shared<MockNWebDragData>();
  ON_CALL(*mock_delegate_, GetOrCreateDragData())
      .WillByDefault(::testing::Return(drag_data_mock));

  auto ret = nweb_impl_->GetOrCreateDragData();
  EXPECT_EQ(ret, drag_data_mock);
}
#endif

#if BUILDFLAG(ARKWEB_CLIPBOARD)
TEST_F(NWebImplTest, NWebImplTest_GetSelectInfo_001) {
  std::string select_info = "test";
  std::string select_info_empty = "";
  MockNWebInputMethodHandler input_method_handler;
  ON_CALL(input_method_handler, GetSelectInfo())
      .WillByDefault(::testing::Return(select_info));
  nweb_impl_->inputmethod_handler_ = nullptr;

  auto ret = nweb_impl_->GetSelectInfo();
  EXPECT_EQ(ret, select_info_empty);
}
#endif

TEST_F(NWebImplTest, NWebImplTest_ScaleGestureChange_001) {
  int temp = -1;
  double scale = 1;
  double centerX = 1.5;
  double centerY = 2.5;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_,
              ScaleGestureChange(::testing::_, ::testing::_, ::testing::_))
      .Times(0);

  auto ret = nweb_impl_->ScaleGestureChange(scale, centerX, centerY);
  EXPECT_EQ(ret, temp);
}

TEST_F(NWebImplTest, NWebImplTest_ScaleGestureChange_002) {
  int temp = 1;
  double scale = 1;
  double centerX = 1.5;
  double centerY = 2.5;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_,
              ScaleGestureChange(::testing::_, ::testing::_, ::testing::_))
      .WillOnce(::testing::Return(temp));

  auto ret = nweb_impl_->ScaleGestureChange(scale, centerX, centerY);
  EXPECT_EQ(ret, temp);
}

TEST_F(NWebImplTest, NWebImplTest_OnTextSelected_001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, OnTextSelected()).Times(0);

  nweb_impl_->OnTextSelected();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_OnTextSelected_002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, OnTextSelected()).Times(1);

  nweb_impl_->OnTextSelected();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebPageSnapshot) {
  const char* id = "test_id";
  int width = 1024;
  int height = 768;
  bool result = false;

  result = nweb_impl_->WebPageSnapshot(
      id, PixelUnit::PX, width, height,
      [](const char* str, bool is, float flo, void* ptr, int a, int b) {});
}

TEST_F(NWebImplTest, OnOccludedCall_001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, OnOccluded()).Times(0);

  nweb_impl_->OnOccluded();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnOccludedCall_002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, OnOccluded()).Times(1);

  nweb_impl_->OnOccluded();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnUnoccludedCall_001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, OnUnoccluded()).Times(0);

  nweb_impl_->OnUnoccluded();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnUnoccludedCall_002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, OnUnoccluded()).Times(1);

  nweb_impl_->OnUnoccluded();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetEnableLowerFrameRateCall_001) {
  bool enabled = true;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetEnableLowerFrameRate(::testing::_)).Times(0);

  nweb_impl_->SetEnableLowerFrameRate(enabled);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetEnableLowerFrameRateCall_002) {
  bool enabled = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetEnableLowerFrameRate(::testing::_)).Times(1);

  nweb_impl_->SetEnableLowerFrameRate(enabled);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ClearPrefetchedResourceCall) {
  std::vector<std::string> cache_key_list = {"sdfslllllfds", "dfsfdsddddddd",
                                             "eeeefdsfffffffff"};
  nweb_impl_->ClearPrefetchedResource(cache_key_list);
}

TEST_F(NWebImplTest, OnRenderToBackgroundCall_001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, OnWindowHide()).Times(0);

  nweb_impl_->OnRenderToBackground();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnRenderToBackgroundCall_002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, OnWindowHide()).Times(1);

  nweb_impl_->OnRenderToBackground();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnRenderToForegroundCall_001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, OnWindowShow()).Times(0);

  nweb_impl_->OnRenderToForeground();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnRenderToForegroundCall_002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, OnWindowShow()).Times(1);

  nweb_impl_->OnRenderToForeground();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PrefetchResourceCall) {
  std::shared_ptr<NWebEnginePrefetchArgs> pre_args;
  std::map<std::string, std::string> additional_http_headers = {
      {"aaa", "bbb"}, {"ccc", "dddd"}, {"eee", "ffff"}};
  std::string cache_key = "aaaaaaaaadddddddfff";
  uint32_t cache_valid_time = 30;
  nweb_impl_->PrefetchResource(pre_args, additional_http_headers, cache_key,
                               cache_valid_time);
}
TEST_F(NWebImplTest, TestCreateNWebWithNullCreateInfo) {
  std::shared_ptr<MockNWebCreateInfo> nullCreateInfo = nullptr;
  auto result = NWebImpl::CreateNWeb(nullCreateInfo);
  EXPECT_EQ(result, nullptr);
}

TEST_F(NWebImplTest, TestGetNWebNonExistingId) {
  int32_t existingId = 1;
  auto result = NWebImpl::GetNWeb(existingId);
  EXPECT_EQ(result, nullptr);
}

TEST_F(NWebImplTest, OnWebviewHideWithNullInputHandler) {
  nweb_impl_->OnWebviewHide();
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, OnWebviewShowWithNullInputHandler) {
  nweb_impl_->OnWebviewShow();
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, TestOnTextSelected) {
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->OnTextSelected();
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, ResizeWithNullInputHandler) {
  uint32_t width = 100;
  uint32_t height = 200;
  bool isKeyboard = true;
  nweb_impl_->input_handler_ = nullptr;
  nweb_impl_->output_handler_ = nullptr;
  nweb_impl_->Resize(width, height, isKeyboard);
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, OnTouchPressWithoutHandler) {
  nweb_impl_->input_handler_ = nullptr;
  int32_t id = 1;
  double x = 1.0;
  double y = 1.0;
  bool from_overlay = true;
  nweb_impl_->OnTouchPress(id, x, y, from_overlay);
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, TestSetDrawRect) {
  int x = 0;
  int y = 0;
  int width = 0;
  int height = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->SetDrawRect(x, y, width, height);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, TestOnTouchMoveWithNullInputHandlerSingleParams) {
  nweb_impl_->input_handler_ = nullptr;
  int32_t id = 1;
  double x = 2.0;
  double y = 3.0;
  bool from_overlay = true;
  nweb_impl_->OnTouchMove(id, x, y, from_overlay);
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, TestOnTouchMoveWithNullInputHandlerVectorParams) {
  nweb_impl_->input_handler_ = nullptr;
  std::vector<std::shared_ptr<NWebTouchPointInfo>> touch_point_infos;
  bool from_overlay = true;
  nweb_impl_->OnTouchMove(touch_point_infos, from_overlay);
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, OnTouchCancelnputHandler) {
  nweb_impl_->input_handler_ = nullptr;
  nweb_impl_->OnTouchCancel();
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, SendKeyEventWithNullInputHandler) {
  nweb_impl_->input_handler_ = nullptr;
  int32_t keyCode = 1;
  int32_t keyAction = 2;
  nweb_impl_->SendKeyEvent(keyCode, keyAction);
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, SendTouchpadFlingEventWithNullInputHandler) {
  nweb_impl_->input_handler_ = nullptr;
  double x = 1.0;
  double y = 1.0;
  double vx = 1.0;
  double vy = 1.0;
  nweb_impl_->SendTouchpadFlingEvent(x, y, vx, vy);
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, SendMouseWheelEventWithNullInputHandler) {
  nweb_impl_->input_handler_ = nullptr;
  double x = 1.0;
  double y = 1.0;
  double deltaX = 1.0;
  double deltaY = 1.0;
  nweb_impl_->SendMouseWheelEvent(x, y, deltaX, deltaY);
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, SendMouseEventWithNullInputHandler) {
  nweb_impl_->input_handler_ = nullptr;
  int x = 1;
  int y = 1;
  int button = 1;
  int action = 1;
  int count = 1;
  nweb_impl_->SendMouseEvent(x, y, button, action, count);
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, LoadWithNullInputHandler) {
  std::string url = "https://example.com";
  auto temp = OHOS::NWeb::NWEB_ERR;
  nweb_impl_->input_handler_ = nullptr;
  nweb_impl_->output_handler_ = nullptr;
  int ret = nweb_impl_->Load(url);
  EXPECT_EQ(ret, temp);
}

TEST_F(NWebImplTest, ZoomWithNullNwebDelegate) {
  float zoomFactor = 1.0;
  auto temp = OHOS::NWeb::NWEB_ERR;
  nweb_impl_->nweb_delegate_ = nullptr;
  int ret = nweb_impl_->Zoom(zoomFactor);
  EXPECT_EQ(ret, temp);
}

TEST_F(NWebImplTest, ZoomInWithNullNwebDelegate) {
  auto temp = OHOS::NWeb::NWEB_ERR;
  nweb_impl_->nweb_delegate_ = nullptr;
  int ret = nweb_impl_->ZoomIn();
  EXPECT_EQ(ret, temp);
}

TEST_F(NWebImplTest, ZoomOutWithNullNwebDelegate) {
  auto temp = OHOS::NWeb::NWEB_ERR;
  nweb_impl_->nweb_delegate_ = nullptr;
  int ret = nweb_impl_->ZoomOut();
  EXPECT_EQ(ret, temp);
}
TEST_F(NWebImplTest, GetRenderProcessModeWithNullInputHandler) {
  nweb_impl_->nweb_delegate_ = nullptr;
  auto temp = RenderProcessMode::SINGLE_MODE;
  auto ret = nweb_impl_->GetRenderProcessMode();
  EXPECT_EQ(ret, temp);
}

TEST_F(NWebImplTest, OnDestroyWithNullInitArgs) {
  nweb_impl_->destroyCallback_ = [](const char* str) {};
  nweb_impl_->OnDestroy();
  EXPECT_EQ(nweb_impl_->destroyCallback_, nullptr);
}
}  // namespace OHOS::NWeb
                          