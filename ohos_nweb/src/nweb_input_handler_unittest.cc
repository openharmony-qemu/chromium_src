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
#include <memory>
#include <vector>

#define private public
#include "nweb_delegate_interface.h"
#include "nweb_input_handler.h"

using namespace testing;
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
#ifdef OHOS_MIXED_CONTENT
  MOCK_METHOD(void, EnableMixedContentAutoUpgrades, (bool enable), (override));
  MOCK_METHOD(bool, IsMixedContentAutoUpgradesEnabled, (), (override));
#endif
  MOCK_METHOD(void, SetPopupSurface, (void* popupSurface), (override));

#if defined(OHOS_INPUT_EVENTS)
  MOCK_METHOD(void,
              SetNWebDelegateInterface,
              (std::shared_ptr<NWebDelegateInterface> client),
              (override));
#endif  // defined(OHOS_INPUT_EVENTS)

#ifdef OHOS_ARKWEB_ADBLOCK
  MOCK_METHOD(void, EnableAdsBlock, (bool enable), (override));
  MOCK_METHOD(bool, IsAdsBlockEnabled, (), (override));
  MOCK_METHOD(bool, IsAdsBlockEnabledForCurPage, (), (override));
#endif

#if defined(OHOS_PASSWORD_AUTOFILL)
  MOCK_METHOD(void,
              ProcessAutofillCancel,
              (const std::string& fillContent),
              (override));
#endif

#ifdef OHOS_EX_PASSWORD
  MOCK_METHOD(void, SetSavePasswordAutomatically, (bool enable), (override));
  MOCK_METHOD(bool, GetSavePasswordAutomatically, (), (override));
  MOCK_METHOD(void, SetSavePassword, (bool enable), (override));
  MOCK_METHOD(bool, GetSavePassword, (), (override));
  MOCK_METHOD(void, SaveOrUpdatePassword, (bool is_update), (override));
#endif  // defined(OHOS_EX_PASSWORD)

#if defined(OHOS_EX_PASSWORD) || (OHOS_DATALIST)
  MOCK_METHOD(void,
              PasswordSuggestionSelected,
              (int list_index),
              (const, override));
#endif

#if defined(OHOS_NWEB_EX)
  MOCK_METHOD(bool, CanStoreWebArchive, (), (const, override));
  MOCK_METHOD(void, UnRegisterWebAppClientExtensionListener, (), (override));
#endif  // defined(OHOS_NWEB_EX)

#if defined(OHOS_EX_FREE_COPY)
  MOCK_METHOD(void, SelectAndCopy, (), (override));
  MOCK_METHOD(bool, ShouldShowFreeCopy, (), (override));
#endif  // OHOS_EX_FREE_COPY

#ifdef OHOS_EX_GET_ZOOM_LEVEL
  MOCK_METHOD(void, SetBrowserZoomLevel, (double zoom_factor), (override));
  MOCK_METHOD(double, GetBrowserZoomLevel, (), (override));
#endif
  MOCK_METHOD(void,
              Resize,
              (uint32_t width, uint32_t height, bool isKeyboard),
              (override));

#if defined(OHOS_INPUT_EVENTS)
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

#ifdef OHOS_ARKWEB_ADBLOCK
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

#if defined(OHOS_MSGPORT)
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
#endif  // defined(OHOS_MSGPORT)

#ifdef OHOS_I18N
  MOCK_METHOD(void,
              UpdateLocale,
              (const std::string& language, const std::string& region),
              (override));
#endif  // #ifdef OHOS_I18N
#ifdef OHOS_DRAG_DROP
  MOCK_METHOD(std::shared_ptr<NWebDragData>,
              GetOrCreateDragData,
              (),
              (override));
  MOCK_METHOD(std::string, GetAppTempDir, (), (const, override));
  MOCK_METHOD(bool, DarkModeEnabled, (), (override));
#endif  // #ifdef OHOS_DRAG_DROP

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

#ifdef OHOS_NAVIGATION
  MOCK_METHOD(std::shared_ptr<NWebHistoryList>, GetHistoryList, (), (override));
  MOCK_METHOD(std::vector<uint8_t>, SerializeWebState, (), (override));
  MOCK_METHOD(bool,
              RestoreWebState,
              (const std::vector<uint8_t>& state),
              (override));
#endif

#if defined(OHOS_MEDIA_MUTE_AUDIO)
  MOCK_METHOD(void, SetAudioMuted, (bool muted), (override));
#endif  // defined(OHOS_MEDIA_MUTE_AUDIO)

#if defined(OHOS_WEBRTC)
  MOCK_METHOD(void, StartCamera, (), (override));
  MOCK_METHOD(void, StopCamera, (), (override));
  MOCK_METHOD(void, CloseCamera, (), (override));
#endif  // defined(OHOS_WEBRTC)

#ifdef OHOS_PAGE_UP_DOWN
  MOCK_METHOD(void, PageUp, (bool top), (override));
  MOCK_METHOD(void, PageDown, (bool bottom), (override));
#ifdef OHOS_GET_SCROLL_OFFSET
  MOCK_METHOD(void, GetScrollOffset, (float*, float*), (override));
#endif
#endif  // #ifdef OHOS_PAGE_UP_DOWN

#if defined(OHOS_INPUT_EVENTS)
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
  MOCK_METHOD(void,
              GetOverScrollOffset,
              (float*, float*),
              (override));
#endif
#endif  // defined(OHOS_INPUT_EVENTS)

#if defined(OHOS_EX_FORCE_ZOOM)
  MOCK_METHOD(void, SetForceEnableZoom, (bool forceEnableZoom), (override));
  MOCK_METHOD(bool, GetForceEnableZoom, (), (override));
#endif

#if defined(OHOS_COMPOSITE_RENDER)
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
#endif  // defined(OHOS_COMPOSITE_RENDER)

#if defined(OHOS_MEDIA_POLICY)
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
#endif  // defined(OHOS_MEDIA_POLICY)

#if defined(OHOS_NO_STATE_PREFETCH)
  MOCK_METHOD(void,
              PrefetchPage,
              (const std::string&, (const std::map<std::string, std::string>&)),
              (override));
#endif  // defined(OHOS_NO_STATE_PREFETCH)

#if defined(OHOS_MULTI_WINDOW)
  MOCK_METHOD(void, NotifyPopupWindowResult, (bool result), (override));
#endif  // defined(OHOS_MULTI_WINDOW)

#ifdef OHOS_CA
  MOCK_METHOD(bool,
              GetCertChainDerData,
              (std::vector<std::string> & certChainData, bool isSingleCert),
              (override));
#endif
  MOCK_METHOD(void, SetWindowId, (uint32_t window_id), (override));

#ifdef OHOS_EX_BLANK_TARGET_POPUP_INTERCEPT
  MOCK_METHOD(void,
              SetEnableBlankTargetPopupIntercept,
              (bool enableBlankTargetPopup),
              (override));
#endif

#if defined(OHOS_PRINT)
  MOCK_METHOD(void, SetToken, (void* token), (override));
  MOCK_METHOD(void*,
              CreateWebPrintDocumentAdapter,
              (const std::string& jobName),
              (override));
  MOCK_METHOD(void, SetPrintBackground, (bool enable), (override));
  MOCK_METHOD(bool, GetPrintBackground, (), (override));
#endif  // defined(OHOS_PRINT)

#ifdef OHOS_SCREEN_ROTATION
  MOCK_METHOD(void, SetVirtualPixelRatio, (float ratio), (override));
#endif  // defined(OHOS_SCREEN_ROTATION)

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

#ifdef OHOS_POST_URL
  MOCK_METHOD(int,
              PostUrl,
              (const std::string& url, const std::vector<char>& postData),
              (override));
#endif

#if defined(OHOS_INPUT_EVENTS)
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

#if defined(OHOS_SECURITY_STATE)
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

#if defined(OHOS_SECURE_JAVASCRIPT_PROXY)
  MOCK_METHOD(std::string,
              GetLastJavascriptProxyCallingFrameUrl,
              (),
              (override));
#endif

#ifdef OHOS_ITP
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

#if defined(OHOS_SCREEN_LOCK)
  MOCK_METHOD(void,
              SetWakeLockCallback,
              (int32_t windowId,
               const std::shared_ptr<NWebScreenLockCallback>& callback),
              (override));
#endif

#if defined(OHOS_CUSTOM_VIDEO_PLAYER)
  MOCK_METHOD(void,
              RegisterOnCreateNativeMediaPlayerListener,
              (std::shared_ptr<NWebCreateNativeMediaPlayerCallback> callback),
              (override));
#endif  // OHOS_CUSTOM_VIDEO_PLAYER

#ifdef OHOS_EX_DOWNLOAD
  MOCK_METHOD(NWebDownloadItemState,
              GetDownloadItemState,
              (long item_id),
              (override));
#endif

#if defined(OHOS_CLIPBOARD)
  MOCK_METHOD(void, SetIsRichText, (bool is_rich_text), (override));
  MOCK_METHOD(std::string, GetSelectInfo, (), (override));
#endif

#ifdef OHOS_DISPLAY_CUTOUT
  MOCK_METHOD(void,
              OnSafeInsetsChange,
              (int left, int top, int right, int bottom),
              (override));
#endif

#ifdef OHOS_AI
  MOCK_METHOD(void, OnTextSelected, (), (override));
#endif

#if defined(OHOS_SOFTWARE_COMPOSITOR)
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

#if OHOS_URL_TRUST_LIST
  MOCK_METHOD(int,
              SetUrlTrustListWithErrMsg,
              (const std::string& urlTrustList, std::string& detailErrMsg),
              (override));
#endif

#ifdef OHOS_NETWORK_LOAD
  MOCK_METHOD(void,
              SetPathAllowingUniversalAccess,
              (const std::vector<std::string>& pathList),
              (override));
#endif

#ifdef OHOS_BFCACHE
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

#ifdef OHOS_EX_SCREEN_CAPTURE
  MOCK_METHOD(void, StopScreenCapture, (int32_t nweb_id, const char* session_id), (override));
#endif

#ifdef OHOS_EX_REFRESH_IFRAME
  MOCK_METHOD(void, WebExtensionContextMenuIsIframe, (), (override));
  MOCK_METHOD(bool, WebExtensionContextMenuReloadFocusedFrame, (), (override));
#endif
};

class MockNWebTouchPointInfo : public NWebTouchPointInfo {
 public:
  ~MockNWebTouchPointInfo() = default;
  MOCK_METHOD(int, GetId, (), (override));
  MOCK_METHOD(double, GetX, (), (override));
  MOCK_METHOD(double, GetY, (), (override));
};
class NWebInputHandlerTest : public ::testing::Test {
 public:
  static void SetUpTestCase(void);
  static void TearDownTestCase(void);
  void SetUp(void);
  void TearDown(void);
  std::shared_ptr<MockNWebDelegate> mock_delegate_;
  std::shared_ptr<NWebInputHandler> input_handler_;
};

void NWebInputHandlerTest::SetUpTestCase(void) {}

void NWebInputHandlerTest::TearDownTestCase(void) {}

void NWebInputHandlerTest::SetUp() {
  mock_delegate_ = std::make_shared<MockNWebDelegate>();
  input_handler_ = std::make_shared<NWebInputHandler>(mock_delegate_);
  ASSERT_NE(input_handler_, nullptr);
}

void NWebInputHandlerTest::TearDown() {
  input_handler_ = nullptr;
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnDestroy_001) {
  input_handler_->OnDestroy();
  EXPECT_EQ(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnTouchPress_001) {
  int32_t id = 1;
  double x = 10.0;
  double y = 20.0;
  bool from_overlay = false;

  input_handler_->OnTouchPress(id, x, y, from_overlay);

  EXPECT_TRUE(input_handler_->touch_press_id_map_[id]);
  EXPECT_EQ(input_handler_->last_touch_start_x_, x);
  EXPECT_EQ(input_handler_->last_x_, x);
  EXPECT_EQ(input_handler_->last_y_, y);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnTouchPress_002) {
  int32_t id = 1;
  double x = 10.0;
  double y = 20.0;
  bool from_overlay = false;
  input_handler_->nweb_delegate_ = nullptr;
  input_handler_->touch_press_id_map_[id] = false;

  input_handler_->OnTouchPress(id, x, y, from_overlay);

  EXPECT_FALSE(input_handler_->touch_press_id_map_[id]);
  EXPECT_NE(input_handler_->last_touch_start_x_, x);
  EXPECT_NE(input_handler_->last_x_, x);
  EXPECT_NE(input_handler_->last_y_, y);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnTouchRelease_001) {
  int32_t id = 1;
  double x = 10.0;
  double y = 20.0;
  bool from_overlay = false;
  input_handler_->nweb_delegate_ = nullptr;
  input_handler_->touch_press_id_map_[id] = false;

  input_handler_->OnTouchRelease(id, x, y, from_overlay);

  EXPECT_EQ(input_handler_->touch_press_id_map_[id], false);
  input_handler_->touch_press_id_map_.erase(id);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnTouchRelease_002) {
  int32_t id = 1;
  double x = 10.0;
  double y = 20.0;
  bool from_overlay = false;
  input_handler_->touch_press_id_map_[id] = false;

  input_handler_->OnTouchRelease(id, x, y, from_overlay);

  auto it = input_handler_->touch_press_id_map_.find(id);
  EXPECT_TRUE(it == input_handler_->touch_press_id_map_.end());
  input_handler_->touch_press_id_map_.erase(id);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnTouchRelease_003) {
  int32_t id = 1;
  double x = 0;
  double y = 20.0;
  bool from_overlay = false;
  input_handler_->touch_press_id_map_[id] = false;

  input_handler_->OnTouchRelease(id, x, y, from_overlay);

  auto it = input_handler_->touch_press_id_map_.find(id);
  EXPECT_TRUE(it == input_handler_->touch_press_id_map_.end());
  input_handler_->touch_press_id_map_.erase(id);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnTouchRelease_004) {
  int32_t id = 1;
  double x = 10.0;
  double y = 0;
  bool from_overlay = false;
  input_handler_->touch_press_id_map_[id] = false;

  input_handler_->OnTouchRelease(id, x, y, from_overlay);

  auto it = input_handler_->touch_press_id_map_.find(id);
  EXPECT_TRUE(it == input_handler_->touch_press_id_map_.end());
  input_handler_->touch_press_id_map_.erase(id);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnTouchRelease_005) {
  int32_t id = 1;
  double x = 0;
  double y = 0;
  bool from_overlay = false;
  input_handler_->touch_press_id_map_[id] = false;

  input_handler_->OnTouchRelease(id, x, y, from_overlay);

  auto it = input_handler_->touch_press_id_map_.find(id);
  EXPECT_TRUE(it == input_handler_->touch_press_id_map_.end());
  input_handler_->touch_press_id_map_.erase(id);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnTouchMove_001) {
  int32_t id = 1;
  double x = 1;
  double y = 1;
  bool from_overlay = false;
  input_handler_->nweb_delegate_ = nullptr;

  input_handler_->OnTouchMove(id, x, y, from_overlay);

  EXPECT_NE(input_handler_->last_x_, x);
  EXPECT_NE(input_handler_->last_y_, y);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnTouchMove_002) {
  int32_t id = 1;
  double x = 1;
  double y = 1;
  bool from_overlay = false;

  input_handler_->OnTouchMove(id, x, y, from_overlay);

  EXPECT_EQ(input_handler_->last_x_, x);
  EXPECT_EQ(input_handler_->last_y_, y);
}

// TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnTouchMove_003) {
//   double temp = 1.0;
//   std::vector<std::shared_ptr<NWebTouchPointInfo>> touch_point_infos;
//   auto touch_point_info = std::make_shared<MockNWebTouchPointInfo>();
//   touch_point_infos.push_back(touch_point_info);
//   ON_CALL(*touch_point_info, GetX()).WillByDefault(::testing::Return(temp));
//   ON_CALL(*touch_point_info, GetY()).WillByDefault(::testing::Return(temp));
//   input_handler_->nweb_delegate_ = nullptr;

//   input_handler_->OnTouchMove(touch_point_infos, true);

//   EXPECT_NE(input_handler_->last_x_, temp);
//   EXPECT_NE(input_handler_->last_y_, temp);
// }

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnTouchMove_004) {
  double temp = -1;
  std::vector<std::shared_ptr<NWebTouchPointInfo>> touch_point_infos = {};

  input_handler_->OnTouchMove(touch_point_infos, true);

  EXPECT_EQ(input_handler_->last_x_, temp);
  EXPECT_EQ(input_handler_->last_y_, temp);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnTouchCancel_001) {
  int32_t id = 1;
  input_handler_->nweb_delegate_ = nullptr;
  input_handler_->touch_press_id_map_[id] = false;
  EXPECT_CALL(*mock_delegate_, OnTouchCancelById(::testing::_, ::testing::_,
                                                 ::testing::_, ::testing::_))
      .Times(0);

  input_handler_->OnTouchCancel();

  EXPECT_EQ(input_handler_->touch_press_id_map_[id], false);
  input_handler_->touch_press_id_map_.erase(id);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnTouchCancel_002) {
  int32_t id = 1;
  input_handler_->touch_press_id_map_[id] = false;

  input_handler_->OnTouchCancel();

  auto it = input_handler_->touch_press_id_map_.find(id);
  EXPECT_TRUE(it == input_handler_->touch_press_id_map_.end());
  input_handler_->touch_press_id_map_.erase(id);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnTouchCancelById_001) {
  int32_t id = 1;
  double x = 1;
  double y = 1;
  bool from_overlay = false;
  input_handler_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, OnTouchCancelById(id, x, y, from_overlay))
      .Times(0);

  input_handler_->OnTouchCancelById(id, x, y, from_overlay);
  EXPECT_EQ(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnTouchCancelById_002) {
  int32_t id = 1;
  double x = 1;
  double y = 1;
  bool from_overlay = false;
  EXPECT_CALL(*mock_delegate_, OnTouchCancelById(id, x, y, from_overlay))
      .Times(1);

  input_handler_->OnTouchCancelById(id, x, y, from_overlay);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnNavigateBack_001) {
  input_handler_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, NavigateBack()).Times(0);
  input_handler_->OnNavigateBack();
  EXPECT_EQ(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnNavigateBack_002) {
  EXPECT_CALL(*mock_delegate_, IsNavigatebackwardAllowed())
      .WillOnce(::testing::Return(true));
  EXPECT_CALL(*mock_delegate_, NavigateBack()).Times(1);
  input_handler_->OnNavigateBack();
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnNavigateBack_003) {
  EXPECT_CALL(*mock_delegate_, IsNavigatebackwardAllowed())
      .WillOnce(::testing::Return(false));
  EXPECT_CALL(*mock_delegate_, NavigateBack()).Times(0);
  input_handler_->OnNavigateBack();
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

#if defined(OHOS_INPUT_EVENTS)
TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_WebSendKeyEvent_001) {
  int32_t keyCode = 1;
  int32_t keyAction = 1;
  std::vector<int32_t> pressedCodes = {1, 2, 3};
  ON_CALL(*mock_delegate_, WebSendKeyEvent(keyCode, keyAction, pressedCodes))
      .WillByDefault(::testing::Return(true));
  input_handler_->nweb_delegate_ = nullptr;

  auto ret = input_handler_->WebSendKeyEvent(keyCode, keyAction, pressedCodes);
  EXPECT_EQ(ret, false);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_WebSendKeyEvent_002) {
  int32_t keyCode = 1;
  int32_t keyAction = 1;
  std::vector<int32_t> pressedCodes = {1, 2, 3};
  EXPECT_CALL(*mock_delegate_,
              WebSendKeyEvent(keyCode, keyAction, pressedCodes))
      .WillOnce(::testing::Return(true));

  auto ret = input_handler_->WebSendKeyEvent(keyCode, keyAction, pressedCodes);
  EXPECT_EQ(ret, true);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_WebSendMouseWheelEvent_001) {
  double x = 1.0;
  double y = 2.0;
  double deltaX = 3.0;
  double deltaY = 4.0;
  std::vector<int32_t> pressedCodes = {1, 2, 3};
  input_handler_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_,
              WebSendMouseWheelEvent(x, y, deltaX, deltaY, pressedCodes))
      .Times(0);

  input_handler_->WebSendMouseWheelEvent(x, y, deltaX, deltaY, pressedCodes);
  EXPECT_EQ(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_WebSendMouseWheelEvent_002) {
  double x = 1.0;
  double y = 2.0;
  double deltaX = 3.0;
  double deltaY = 4.0;
  std::vector<int32_t> pressedCodes = {1, 2, 3};
  EXPECT_CALL(*mock_delegate_,
              WebSendMouseWheelEvent(x, y, deltaX, deltaY, pressedCodes))
      .Times(1);

  input_handler_->WebSendMouseWheelEvent(x, y, deltaX, deltaY, pressedCodes);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest,
       NWebInputHandlerTest_WebSendTouchpadFlingEvent_001) {
  double x = 1.0;
  double y = 2.0;
  double vx = 3.0;
  double vy = 4.0;
  std::vector<int32_t> pressedCodes = {1, 2, 3};
  input_handler_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_,
              WebSendTouchpadFlingEvent(x, y, vx, vy, pressedCodes))
      .Times(0);

  input_handler_->WebSendTouchpadFlingEvent(x, y, vx, vy, pressedCodes);
  EXPECT_EQ(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest,
       NWebInputHandlerTest_WebSendTouchpadFlingEvent_002) {
  double x = 1.0;
  double y = 2.0;
  double vx = 3.0;
  double vy = 4.0;
  std::vector<int32_t> pressedCodes = {1, 2, 3};
  EXPECT_CALL(*mock_delegate_,
              WebSendTouchpadFlingEvent(x, y, vx, vy, pressedCodes))
      .Times(1);

  input_handler_->WebSendTouchpadFlingEvent(x, y, vx, vy, pressedCodes);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}
#endif

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_SendKeyEvent_001) {
  int32_t keyCode = 1;
  int32_t keyAction = 1;
  ON_CALL(*mock_delegate_, SendKeyEvent(keyCode, keyAction))
      .WillByDefault(::testing::Return(true));
  input_handler_->nweb_delegate_ = nullptr;

  auto ret = input_handler_->SendKeyEvent(keyCode, keyAction);
  EXPECT_EQ(ret, false);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_SendKeyEvent_002) {
  int32_t keyCode = 1;
  int32_t keyAction = 1;
  EXPECT_CALL(*mock_delegate_, SendKeyEvent(keyCode, keyAction))
      .WillOnce(::testing::Return(true));

  auto ret = input_handler_->SendKeyEvent(keyCode, keyAction);
  EXPECT_EQ(ret, true);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_SendTouchpadFlingEvent_001) {
  double x = 1.0;
  double y = 2.0;
  double vx = 3.0;
  double vy = 4.0;
  input_handler_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SendTouchpadFlingEvent(x, y, vx, vy)).Times(0);

  input_handler_->SendTouchpadFlingEvent(x, y, vx, vy);
  EXPECT_EQ(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_SendTouchpadFlingEvent_002) {
  double x = 1.0;
  double y = 2.0;
  double vx = 3.0;
  double vy = 4.0;
  EXPECT_CALL(*mock_delegate_, SendTouchpadFlingEvent(x, y, vx, vy)).Times(1);

  input_handler_->SendTouchpadFlingEvent(x, y, vx, vy);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_SendMouseWheelEvent_001) {
  double x = 1.0;
  double y = 2.0;
  double deltaX = 3.0;
  double deltaY = 4.0;
  input_handler_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SendMouseWheelEvent(x, y, deltaX, deltaY))
      .Times(0);

  input_handler_->SendMouseWheelEvent(x, y, deltaX, deltaY);
  EXPECT_EQ(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_SendMouseWheelEvent_002) {
  double x = 1.0;
  double y = 2.0;
  double deltaX = 3.0;
  double deltaY = 4.0;
  EXPECT_CALL(*mock_delegate_, SendMouseWheelEvent(x, y, deltaX, deltaY))
      .Times(1);

  input_handler_->SendMouseWheelEvent(x, y, deltaX, deltaY);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_SendMouseEvent_001) {
  int x = 1;
  int y = 2;
  int button = 3;
  int action = 4;
  int count = 5;
  input_handler_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SendMouseEvent(x, y, button, action, count))
      .Times(0);

  input_handler_->SendMouseEvent(x, y, button, action, count);
  EXPECT_EQ(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_SendMouseEvent_002) {
  int x = 1;
  int y = 2;
  int button = 3;
  int action = 4;
  int count = 5;
  EXPECT_CALL(*mock_delegate_, SendMouseEvent(x, y, button, action, count))
      .Times(1);

  input_handler_->SendMouseEvent(x, y, button, action, count);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_CheckSlideNavigation_001) {
  int16_t start_x = 1;
  int16_t end_x = 2;
  input_handler_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, IsNavigatebackwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateBack()).Times(0);
  EXPECT_CALL(*mock_delegate_, IsNavigateForwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateForward()).Times(0);

  input_handler_->CheckSlideNavigation(start_x, end_x);
  EXPECT_EQ(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_CheckSlideNavigation_002) {
  int16_t start_x = 1;
  int16_t end_x = 200;

  EXPECT_CALL(*mock_delegate_, IsNavigatebackwardAllowed())
      .WillOnce(::testing::Return(true));
  EXPECT_CALL(*mock_delegate_, NavigateBack()).Times(1);
  EXPECT_CALL(*mock_delegate_, IsNavigateForwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateForward()).Times(0);

  input_handler_->CheckSlideNavigation(start_x, end_x);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_CheckSlideNavigation_003) {
  int16_t start_x = 1;
  int16_t end_x = 200;

  EXPECT_CALL(*mock_delegate_, IsNavigatebackwardAllowed())
      .WillOnce(::testing::Return(false));
  EXPECT_CALL(*mock_delegate_, NavigateBack()).Times(0);
  EXPECT_CALL(*mock_delegate_, IsNavigateForwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateForward()).Times(0);

  input_handler_->CheckSlideNavigation(start_x, end_x);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_CheckSlideNavigation_004) {
  int16_t start_x = 1;
  int16_t end_x = 10;

  EXPECT_CALL(*mock_delegate_, IsNavigatebackwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateBack()).Times(0);
  EXPECT_CALL(*mock_delegate_, IsNavigateForwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateForward()).Times(0);

  input_handler_->CheckSlideNavigation(start_x, end_x);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_CheckSlideNavigation_005) {
  int16_t start_x = 11;
  int16_t end_x = 200;

  EXPECT_CALL(*mock_delegate_, IsNavigatebackwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateBack()).Times(0);
  EXPECT_CALL(*mock_delegate_, IsNavigateForwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateForward()).Times(0);

  input_handler_->CheckSlideNavigation(start_x, end_x);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_CheckSlideNavigation_006) {
  int16_t start_x = 11;
  int16_t end_x = 12;

  EXPECT_CALL(*mock_delegate_, IsNavigatebackwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateBack()).Times(0);
  EXPECT_CALL(*mock_delegate_, IsNavigateForwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateForward()).Times(0);

  input_handler_->CheckSlideNavigation(start_x, end_x);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_CheckSlideNavigation_007) {
  int16_t start_x = 3000;
  int16_t end_x = 2000;

  EXPECT_CALL(*mock_delegate_, IsNavigatebackwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateBack()).Times(0);
  EXPECT_CALL(*mock_delegate_, IsNavigateForwardAllowed())
      .WillOnce(::testing::Return(true));
  EXPECT_CALL(*mock_delegate_, NavigateForward()).Times(1);

  input_handler_->CheckSlideNavigation(start_x, end_x);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_CheckSlideNavigation_008) {
  int16_t start_x = 3000;
  int16_t end_x = 2000;

  EXPECT_CALL(*mock_delegate_, IsNavigatebackwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateBack()).Times(0);
  EXPECT_CALL(*mock_delegate_, IsNavigateForwardAllowed())
      .WillOnce(::testing::Return(false));
  EXPECT_CALL(*mock_delegate_, NavigateForward()).Times(0);

  input_handler_->CheckSlideNavigation(start_x, end_x);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_CheckSlideNavigation_009) {
  int16_t start_x = 3000;
  int16_t end_x = 2950;

  EXPECT_CALL(*mock_delegate_, IsNavigatebackwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateBack()).Times(0);
  EXPECT_CALL(*mock_delegate_, IsNavigateForwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateForward()).Times(0);

  input_handler_->CheckSlideNavigation(start_x, end_x);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_CheckSlideNavigation_010) {
  int16_t start_x = 2500;
  int16_t end_x = 2000;

  EXPECT_CALL(*mock_delegate_, IsNavigatebackwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateBack()).Times(0);
  EXPECT_CALL(*mock_delegate_, IsNavigateForwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateForward()).Times(0);

  input_handler_->CheckSlideNavigation(start_x, end_x);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_CheckSlideNavigation_011) {
  int16_t start_x = 2500;
  int16_t end_x = 2450;

  EXPECT_CALL(*mock_delegate_, IsNavigatebackwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateBack()).Times(0);
  EXPECT_CALL(*mock_delegate_, IsNavigateForwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateForward()).Times(0);

  input_handler_->CheckSlideNavigation(start_x, end_x);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}
}  // namespace OHOS::NWeb
