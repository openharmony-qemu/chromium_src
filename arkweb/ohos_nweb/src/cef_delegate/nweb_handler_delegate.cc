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

#include "nweb_handler_delegate.h"

#include <sys/mman.h>

#include <thread>

#include "arkweb/build/features/features.h"
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "base/functional/bind.h"
#include "base/functional/callback.h"
#include "base/task/thread_pool.h"
#include "cef/include/cef_app.h"
#include "cef/include/cef_cookie.h"
#include "cef/include/cef_frame.h"
#include "cef/include/cef_parser.h"
#include "cef/include/wrapper/cef_closure_task.h"
#include "cef/include/wrapper/cef_helpers.h"
#include "content/browser/gpu/gpu_process_host.h"
#include "content/public/browser/browser_thread.h"
#include "libcef/common/arkweb_request_impl_ext.h"
#include "third_party/bounds_checking_function/include/securec.h"
#include "net/base/net_errors.h"
#include "net/cookies/site_for_cookies.h"
#include "net/cookies/static_cookie_policy.h"
#include "nweb_access_request_delegate.h"
#include "nweb_console_log_impl.h"
#include "nweb_context_menu_params_impl.h"
#include "nweb_controller_handler_impl.h"
#include "nweb_core_value.h"
#include "nweb_cursor_info_impl.h"
#include "nweb_data_resubmission_callback_impl.h"
#include "nweb_date_time_chooser_impl.h"
#include "nweb_engine_impl.h"
#include "nweb_file_selector_params_impl.h"
#include "nweb_find_delegate.h"
#include "nweb_first_meaningful_paint_details_impl.h"
#include "nweb_full_screen_exit_handler_impl.h"
#include "nweb_geolocation_callback.h"
#include "nweb_impl.h"
#include "nweb_js_dialog_result_impl.h"
#include "nweb_js_http_auth_result_impl.h"
#include "nweb_js_ssl_error_result_impl.h"
#include "nweb_js_ssl_select_cert_result_impl.h"
#include "nweb_key_event_impl.h"
#include "nweb_largest_contentful_paint_details_impl.h"
#include "nweb_load_committed_details_impl.h"
#include "nweb_preference_delegate.h"
#include "nweb_resource_handler.h"
#include "nweb_select_menu_bound_impl.h"
#include "nweb_select_popup_menu_impl.h"
#include "nweb_url_resource_error_impl.h"
#include "nweb_url_resource_request_impl.h"
#include "nweb_url_resource_response_impl.h"
#include "nweb_value_callback.h"
#include "nweb_value_convert.h"
#include "ohos_adapter_helper.h"
#include "ohos_glue/base/include/ark_web_errno.h"
#include "url/gurl.h"
#include "arkweb/ohos_adapter_ndk/ohos_adapter_helper_ext.h"

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "base/command_line.h"
#include "content/public/common/content_switches.h"
#endif

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#include "base/command_line.h"
#include "content/public/common/content_switches.h"
#endif // ARKWEB_ARKWEB_EXTENSIONS

#if defined(REPORT_SYS_EVENT)
#include "event_reporter.h"
#endif

#if BUILDFLAG(ARKWEB_CSS_INPUT_TIME)
#include "third_party/blink/renderer/platform/wtf/date_math.h"
#endif  // #if BUILDFLAG(ARKWEB_CSS_INPUT_TIME)

#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
#include <string.h>

#include "cef/include/cef_download_handler.h"
#include "nweb_download_handler_delegate.h"
#include "ohos_nweb/src/capi/nweb_download_delegate_callback.h"
#endif  //  OHOS_EX_DOWNLOAD

#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "cef/include/cef_command_line.h"
#endif

#if BUILDFLAG(ARKWEB_EXT_PERMISSION)
#include "base/command_line.h"
#include "ohos_nweb/src/capi/nweb_permission_request.h"
#include "third_party/blink/public/common/switches.h"
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
#include "base/strings/string_number_conversions.h"
#endif

#include "ui/base/clipboard/ohos/clip_board_image_data_adapter_impl.h"

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
#include "cef/ohos_cef_ext/include/cef_media_player_listener.h"
#include "ohos_nweb/src/native_media_player/nweb_media_info_impl.h"
#include "ohos_nweb/src/native_media_player/nweb_native_media_player_handler_impl.h"
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
#include "content/browser/gpu/gpu_process_host.h"
#endif

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "arkweb/ohos_nweb_ex/third_party/securec/include/securec.h"
#endif
#include "cef/ohos_cef_ext/include/arkweb_frame_ext.h"

#if BUILDFLAG(ARKWEB_NWEB_EX) && BUILDFLAG(ARKWEB_CRASHPAD)
#include "ohos_nweb_ex/overrides/ohos_nweb/src/cef_delegate/custom_crashpad_handler.h"
#endif

#if BUILDFLAG(ARKWEB_SAME_LAYER)
#include "ohos_nweb/src/capi/nweb_native_embed_first_frame_paint_event.h"
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
#include "include/cef_media_player_controller.h"
#include "include/cef_media_player_listener_for_vast.h"
#endif // ARKWEB_VIDEO_ASSISTANT

#include "ohos_nweb/src/capi/nweb_media_player_listener.h"
#include "ohos_nweb/src/cef_delegate/nweb_media_player_for_vast.h"
#include "ohos_nweb/src/video_assistant/nweb_media_player_controller_impl.h"

#if BUILDFLAG(ARKWEB_SAFEBROWSING)
#include "ohos_nweb_ex/overrides/ohos_nweb/src/cef_delegate/nweb_safe_browsing_detection_handler.h"
#endif

namespace OHOS::NWeb {
namespace {

const int MAX_FLOWBUF_DATA_SIZE = 52428800; /* 50 MB */
const int MAX_ENTRIES = 10;
const int HEADER_SIZE =
    (MAX_ENTRIES * 8); /* 10 * (int position + int length) */
const int INDEX_SIZE = 2;

#if BUILDFLAG(ARKWEB_CSS_INPUT_TIME)
const int kEpochBeginYear = 1970;
const int kMonthPerYear = 12;
#endif

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
const int WEB_CAN_SNAPSHOT_DELAY_TIME = 1500;
#endif

const int VIEW_PORT_DIFF = 5;

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
std::shared_ptr<NWebExtensionApiCallback> g_extension_api_listener = nullptr;
static std::map<int, TabCreatedCallback> g_tab_created_map_;
#endif // ARKWEB_ARKWEB_EXTENSIONS


ImageColorType TransformColorType(cef_color_type_t color_type) {
  switch (color_type) {
    case CEF_COLOR_TYPE_RGBA_8888:
      return ImageColorType::COLOR_TYPE_RGBA_8888;
    case CEF_COLOR_TYPE_BGRA_8888:
      return ImageColorType::COLOR_TYPE_BGRA_8888;
    default:
      return ImageColorType::COLOR_TYPE_UNKNOWN;
  }
}

ImageAlphaType TransformAlphaType(cef_alpha_type_t alpha_type) {
  switch (alpha_type) {
    case CEF_ALPHA_TYPE_OPAQUE:
      return ImageAlphaType::ALPHA_TYPE_OPAQUE;
    case CEF_ALPHA_TYPE_PREMULTIPLIED:
      return ImageAlphaType::ALPHA_TYPE_PREMULTIPLIED;
    case CEF_ALPHA_TYPE_POSTMULTIPLIED:
      return ImageAlphaType::ALPHA_TYPE_POSTMULTIPLIED;
    default:
      return ImageAlphaType::ALPHA_TYPE_UNKNOWN;
  }
}

SslError SslErrorConvert(cef_errorcode_t cert_error) {
  SslError err;
  switch (cert_error) {
    case ERR_CERT_COMMON_NAME_INVALID:
      err = SslError::HOSTMISMATCH;
      break;
    case ERR_CERT_DATE_INVALID:
      err = SslError::DATEINVALID;
      break;
    case ERR_CERT_KNOWN_INTERCEPTION_BLOCKED:
    case ERR_CERT_AUTHORITY_INVALID:
      err = SslError::UNTRUSTED;
      break;
    default:
      // all other codes to INVALID
      err = SslError::INVALID;
      break;
  }

  LOG(INFO) << "NWebHandlerDelegate::OnCertificateError SslErrorConvert: "
            << "cef err = " << cert_error
            << ", nweb err = " << static_cast<int32_t>(err);
  return err;
}

int TransformColorTypeToInt(ImageColorType color_type) {
  switch (color_type) {
    case ImageColorType::COLOR_TYPE_RGBA_8888:
      return 0;
    case ImageColorType::COLOR_TYPE_BGRA_8888:
      return 1;
    default:
      return -1;
  }
}

int TransformAlphaTypeToInt(ImageAlphaType alpha_type) {
  switch (alpha_type) {
    case ImageAlphaType::ALPHA_TYPE_OPAQUE:
      return 0;
    case ImageAlphaType::ALPHA_TYPE_PREMULTIPLIED:
      return 1;
    case ImageAlphaType::ALPHA_TYPE_POSTMULTIPLIED:
      return 2;
    default:
      return -1;
  }
}

void ConvertMapToHeaderMap(const CefRequest::HeaderMap& headers,
                           std::map<std::string, std::string>& map) {
  for (auto iter = headers.begin(); iter != headers.end(); ++iter) {
    map.emplace(iter->first.ToString(), iter->second.ToString());
  }
}

NWebConsoleLog::NWebConsoleLogLevel ConvertConsoleMessageLevel(
    cef_log_severity_t message_level) {
  switch (message_level) {
    case LOGSEVERITY_DEBUG:
      return NWebConsoleLog::NWebConsoleLogLevel::DEBUG;
    case LOGSEVERITY_INFO:
      return NWebConsoleLog::NWebConsoleLogLevel::INFO;
    case LOGSEVERITY_WARNING:
      return NWebConsoleLog::NWebConsoleLogLevel::WARNING;
    case LOGSEVERITY_ERROR:
      return NWebConsoleLog::NWebConsoleLogLevel::ERROR;
    default:
      return NWebConsoleLog::NWebConsoleLogLevel::UNKNOWN;
  }
}

NWebFileSelectorParams::FileSelectorMode ConvertFileSelectorMode(
    CefDialogHandler::FileDialogMode mode) {
  NWebFileSelectorParams::FileSelectorMode result_mode =
      NWebFileSelectorParams::FileSelectorMode::FILE_OPEN_MODE;
  switch (mode) {
    case FILE_DIALOG_OPEN:
      return NWebFileSelectorParams::FileSelectorMode::FILE_OPEN_MODE;
    case FILE_DIALOG_OPEN_MULTIPLE:
      return NWebFileSelectorParams::FileSelectorMode::FILE_OPEN_MULTIPLE_MODE;
    case FILE_DIALOG_OPEN_FOLDER:
      return NWebFileSelectorParams::FileSelectorMode::FILE_OPEN_FOLDER_MODE;
    case FILE_DIALOG_SAVE:
      return NWebFileSelectorParams::FileSelectorMode::FILE_SAVE_MODE;
    default:
      break;
  }
  return result_mode;
}

#if BUILDFLAG(ARKWEB_CSS_INPUT_TIME)
DateTimeChooserType ConvertDateTimeChooserType(cef_text_input_type_t type) {
  switch (type) {
    case CEF_TEXT_INPUT_TYPE_DATE:
      return DTC_DATE;
    case CEF_TEXT_INPUT_TYPE_DATE_TIME:
      return DTC_DATETIME;
    case CEF_TEXT_INPUT_TYPE_DATE_TIME_LOCAL:
      return DTC_DATETIME_LOCAL;
    case CEF_TEXT_INPUT_TYPE_MONTH:
      return DTC_MONTH;
    case CEF_TEXT_INPUT_TYPE_TIME:
      return DTC_TIME;
    case CEF_TEXT_INPUT_TYPE_WEEK:
      return DTC_WEEK;
    default:
      return DTC_UNKNOWN;
  }
}

double ConvertDateTimeToMs(const DateTime& datetime) {
  double result =
      WTF::DateToDaysFrom1970(datetime.year, datetime.month, datetime.day) *
      WTF::kMsPerDay;
  result += WTF::kMsPerHour * datetime.hour;
  result += WTF::kMsPerMinute * datetime.minute;
  return result;
}

DateTime ConvertMsToDateTime(double ms) {
  int year = WTF::MsToYear(ms);
  int year_day = WTF::DayInYear(ms, year);
  int month = WTF::MonthFromDayInYear(year_day, IsLeapYear(year));
  int day = WTF::DayInMonthFromDayInYear(year_day, IsLeapYear(year));
  double value = std::floor(fmod(ms, WTF::kMsPerDay) / WTF::kMsPerSecond);
  int second = static_cast<int>(fmod(value, WTF::kSecondsPerMinute));
  value = std::floor(value / WTF::kSecondsPerMinute);
  int minute = static_cast<int>(fmod(value, WTF::kMinutesPerHour));
  int hour = static_cast<int>(value / WTF::kMinutesPerHour);
  return {year, month, day, hour, minute, second};
}

double ConvertDateTimeToMonth(const DateTime& datetime) {
  return (datetime.year - kEpochBeginYear) * kMonthPerYear + datetime.month;
}

DateTime ConvertMonthToDateTime(double month) {
  int month_value = static_cast<int>(month);
  return {.year = month_value / kMonthPerYear + kEpochBeginYear,
          .month = month_value % kMonthPerYear};
}
#endif  // #if BUILDFLAG(ARKWEB_CSS_INPUT_TIME)

char* CopyCefStringToChar(const CefString& str) {
  if (str.empty()) {
    return nullptr;
  }
  int strLen = str.size() + 1;
  char* result = new char[strLen]{0};
  if (strcpy_s(result, strLen, str.ToString().c_str()) != EOK) {
    delete[] result;
    return nullptr;
  }
  return result;
}

#if BUILDFLAG(ARKWEB_SCREEN_LOCK)
class SetKeepScreenOnCallback : public CefSetLockCallback {
 public:
  explicit SetKeepScreenOnCallback(
      const std::shared_ptr<NWebScreenLockCallback>& callback)
      : callback_(callback) {}

  ~SetKeepScreenOnCallback() override {}

  void Handle(bool key) override {
    if (callback_) {
      callback_->Handle(key);
    }
  }

 private:
  std::shared_ptr<NWebScreenLockCallback> callback_;

  IMPLEMENT_REFCOUNTING(SetKeepScreenOnCallback);
};
#endif

#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
const char kOffScreenFrameRate[] = "off-screen-frame-rate";
#endif  // BUILDFLAG(ARKWEB_MULTI_WINDOW)

bool AllowCookies(const GURL& url,
                  const net::SiteForCookies& site_for_cookies,
                  bool block_all_cookies,
                  bool block_thirdparty_cookies) {
  net::StaticCookiePolicy::Type policy =
      net::StaticCookiePolicy::ALLOW_ALL_COOKIES;
  if (block_all_cookies) {
    policy = net::StaticCookiePolicy::BLOCK_ALL_COOKIES;
  } else if (block_thirdparty_cookies) {
    policy = net::StaticCookiePolicy::BLOCK_ALL_THIRD_PARTY_COOKIES;
  } else {
    return true;
  }
  return net::StaticCookiePolicy(policy).CanAccessCookies(
             url, site_for_cookies) == net::OK;
}

void ParseNativeProxyArgs(CefRefPtr<CefListValue> args,
                          std::vector<std::vector<uint8_t>>& dataList,
                          std::vector<size_t>& dataSize) {
  size_t argsSize = args->GetSize();

  for (size_t i = 0; i < argsSize; i++) {
    CefValueType type = args->GetType(i);
    CefRefPtr<CefValue> value = args->GetValue(i);
    if (!value) {
      LOG(ERROR) << "value is nullptr";
      continue;
    }

    if (type == VTYPE_STRING) {
      auto argString = value->GetString().ToString();
      size_t size = argString.size();

      dataList[i] = std::vector<uint8_t>(argString.begin(), argString.end());
      dataSize[i] = size;
    } else if (type == VTYPE_BINARY) {
      auto argBinary = value->GetBinary();
      size_t size = argBinary->GetSize();

      std::vector<uint8_t> data(size);
      argBinary->GetData(&data[0], size, 0);

      dataList[i] = std::move(data);
      dataSize[i] = size;
    } else {
      std::string jsonString =
          CefWriteJSON(value, JSON_WRITER_OMIT_BINARY_VALUES);
      dataList[i] = std::vector<uint8_t>(jsonString.begin(), jsonString.end());
      dataSize[i] = jsonString.size();
    }
  }
}
}  // namespace

class NWebDateTimeSuggestionImpl : public NWebDateTimeSuggestion {
 public:
  NWebDateTimeSuggestionImpl() = default;
  NWebDateTimeSuggestionImpl(const DateTime& value,
                             const std::string& label,
                             const std::string& localized_value)
      : value_(value), label_(label), localized_value_(localized_value) {}
  ~NWebDateTimeSuggestionImpl() = default;

  std::string GetLabel() override { return label_; }

  DateTime GetValue() override { return value_; }

  std::string GetLocalizedValue() override { return localized_value_; }

 private:
  DateTime value_;
  std::string label_;
  std::string localized_value_;
};

#if BUILDFLAG(ARKWEB_CSS_INPUT_TIME)
class NWebDateTimeChooserCallbackImpl : public NWebDateTimeChooserCallback {
 public:
  NWebDateTimeChooserCallbackImpl() = default;
  NWebDateTimeChooserCallbackImpl(
      DateTimeChooserType type,
      CefRefPtr<CefDateTimeChooserCallback> callback)
      : type_(type), callback_(callback) {}
  ~NWebDateTimeChooserCallbackImpl() = default;
  void Continue(bool success, const DateTime& value) override {
    if (!callback_ || is_executed) {
      return;
    }
    if (!success) {
      callback_->Continue(false, 0);
    } else {
      double result = (type_ == DateTimeChooserType::DTC_MONTH)
                          ? ConvertDateTimeToMonth(value)
                          : ConvertDateTimeToMs(value);
      callback_->Continue(true, result);
    }
    is_executed = true;
  }

 private:
  bool is_executed = false;
  DateTimeChooserType type_;
  CefRefPtr<CefDateTimeChooserCallback> callback_ = nullptr;
};
#endif  // #if BUILDFLAG(ARKWEB_CSS_INPUT_TIME)

class NWebAppLinkCallbackImpl : public NWebAppLinkCallback {
 public:
  explicit NWebAppLinkCallbackImpl(CefRefPtr<CefOpenAppLinkCallback> callback)
      : callback_(callback) {}

  void ContinueLoad() override {
    if (callback_) {
      callback_->Continue();
    }
  }

  void CancelLoad() override {
    if (callback_) {
      callback_->Cancel();
    }
  }

 private:
  CefRefPtr<CefOpenAppLinkCallback> callback_ = nullptr;
};

// static
CefRefPtr<NWebHandlerDelegate> NWebHandlerDelegate::Create(
    std::shared_ptr<NWebPreferenceDelegate> preference_delegate,
    CefRefPtr<NWebRenderHandler> render_handler,
    std::shared_ptr<NWebEventHandler> event_handler,
    std::shared_ptr<NWebFindDelegate> find_delegate,
    bool is_enhance_surface,
    void* window) {
  CefRefPtr<NWebHandlerDelegate> handler_delegate = new NWebHandlerDelegate(
      preference_delegate, render_handler, event_handler, find_delegate,
      is_enhance_surface, window);
  if (handler_delegate == nullptr) {
    LOG(ERROR) << "fail to create NWebHandlerDelegate instance";
    return nullptr;
  }
  return handler_delegate;
}

int32_t NWebHandlerDelegate::popIndex_ = 0;
NWebHandlerDelegate::NWebHandlerDelegate(
    std::shared_ptr<NWebPreferenceDelegate> preference_delegate,
    CefRefPtr<NWebRenderHandler> render_handler,
    std::shared_ptr<NWebEventHandler> event_handler,
    std::shared_ptr<NWebFindDelegate> find_delegate,
    bool is_enhance_surface,
    void* window)
    : preference_delegate_(preference_delegate),
      render_handler_(render_handler),
      event_handler_(event_handler),
      find_delegate_(find_delegate),
      is_enhance_surface_(is_enhance_surface) {
#if defined(REPORT_SYS_EVENT)
  access_sum_count_ = 0;
  access_success_count_ = 0;
  access_fail_count_ = 0;
#endif
  if (!is_enhance_surface_) {
    window_ = window;
  }
}

void NWebHandlerDelegate::OnDestroy() {
#if BUILDFLAG(ARKWEB_JSPROXY)
  RemoveTransientJavaScriptObject();
#endif
  if (main_browser_) {
    main_browser_->GetHost()->CloseBrowser(true);
    main_browser_ = nullptr;
  }
  if (event_handler_) {
    event_handler_->OnDestroy();
  }
#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  setWebPaintedTask_.Cancel();
#endif
}

void NWebHandlerDelegate::RegisterDownLoadListener(
    std::shared_ptr<NWebDownloadCallback> download_listener) {
  download_listener_ = download_listener;
}

void NWebHandlerDelegate::RegisterReleaseSurfaceListener(
    std::shared_ptr<NWebReleaseSurfaceCallback> releaseSurfaceListener) {
  releaseSurfaceListener_ = releaseSurfaceListener;
}

void NWebHandlerDelegate::RegisterWebAppClientExtensionListener(
    std::shared_ptr<NWebAppClientExtensionCallback>
        web_app_client_extension_listener) {
  web_app_client_extension_listener_ = web_app_client_extension_listener;
}

#if BUILDFLAG(ARKWEB_NWEB_EX)
void NWebHandlerDelegate::UnRegisterWebAppClientExtensionListener() {
  web_app_client_extension_listener_ = nullptr;
}
void NWebHandlerDelegate::UnRegisterWebExtensionListener() {
  web_extension_listener_ = nullptr;
}

void NWebHandlerDelegate::RegisterWebExtensionListener(
    std::shared_ptr<NWebExtensionCallback> web_extension_listener) {
  web_extension_listener_ = web_extension_listener;
}
#endif  // BUILDFLAG(ARKWEB_NWEB_EX)

void NWebHandlerDelegate::RegisterNWebHandler(
    std::shared_ptr<NWebHandler> handler) {
  LOG(INFO) << "RegisterNWebHandler";
  nweb_handler_ = handler;
  if (render_handler_ != nullptr) {
    render_handler_->RegisterNWebHandler(handler);
  }
}

void NWebHandlerDelegate::SetInputMethodClient(
    CefRefPtr<NWebInputMethodClient> client) {
  LOG(INFO) << "SetInputMethodClient";
  input_method_client_ = client;
}

void NWebHandlerDelegate::RegisterNWebJavaScriptCallBack(
    std::shared_ptr<NWebJavaScriptResultCallBack> callback) {
  nweb_javascript_callback_ = callback;
}

const CefRefPtr<ArkWebBrowserExt> NWebHandlerDelegate::GetBrowser() {
  if (main_browser_ == nullptr) {
    return nullptr;
  }
  return main_browser_->AsArkWebBrowser();
}

bool NWebHandlerDelegate::IsClosing() const {
  return is_closing_;
}

void NWebHandlerDelegate::CloseAllBrowsers(bool force_close) {
  LOG(INFO) << "NWebHandlerDelegate::CloseAllBrowsers";
  if (!CefCurrentlyOn(TID_UI)) {
// Execute on the UI thread.
#ifndef OHOS_FUZZ_COMPILE_ERROR_FIX
    CefPostTask(TID_UI, base::BindOnce(&NWebHandlerDelegate::CloseAllBrowsers,
                                       this, force_close));
#endif
    return;
  }

  if (browser_list_.empty()) {
    return;
  }

  BrowserList::const_iterator it = browser_list_.begin();
  for (; it != browser_list_.end(); ++it) {
    (*it)->GetHost()->CloseBrowser(force_close);
  }
}

/* CefClient methods begin */
CefRefPtr<CefDownloadHandler> NWebHandlerDelegate::GetDownloadHandler() {
  return this;
}

CefRefPtr<CefLifeSpanHandler> NWebHandlerDelegate::GetLifeSpanHandler() {
  return this;
}

CefRefPtr<ArkWebLoadHandlerExt> NWebHandlerDelegate::GetLoadHandler() {
  return this;
}

CefRefPtr<ArkWebRenderHandlerExt> NWebHandlerDelegate::GetRenderHandler() {
  return render_handler_;
}

CefRefPtr<CefRequestHandler> NWebHandlerDelegate::GetRequestHandler() {
  return this;
}

CefRefPtr<ArkWebDisplayHandlerExt> NWebHandlerDelegate::GetDisplayHandler() {
  return this;
}

CefRefPtr<CefFocusHandler> NWebHandlerDelegate::GetFocusHandler() {
  return this;
}

CefRefPtr<CefPermissionRequest> NWebHandlerDelegate::GetPermissionRequest() {
  return this;
}

CefRefPtr<CefJSDialogHandler> NWebHandlerDelegate::GetJSDialogHandler() {
  return this;
}

CefRefPtr<CefDialogHandlerExt> NWebHandlerDelegate::GetDialogHandler() {
  return this;
}

CefRefPtr<CefContextMenuHandlerExt>
NWebHandlerDelegate::GetContextMenuHandler() {
  return this;
}

#if BUILDFLAG(ARKWEB_MEDIA_MUTE_AUDIO)
CefRefPtr<CefMediaHandler> NWebHandlerDelegate::GetMediaHandler() {
  return this;
}
#endif  // BUILDFLAG(ARKWEB_MEDIA_MUTE_AUDIO)

#if BUILDFLAG(ARKWEB_SAME_LAYER)
CefRefPtr<CefWebClientExtensionHandler>
NWebHandlerDelegate::GetWebClientExtensionHandler() {
  return this;
}

void NWebHandlerDelegate::OnNativeEmbedFirstFramePaint(
    CefRefPtr<CefNativeEmbedFirstFramePaintEvent> request) {
  if (!web_app_client_extension_listener_) {
    LOG(WARNING) << "OnNativeEmbedFirstFramePaint failed, no listener";
    return;
  }
  if (!web_app_client_extension_listener_->OnNativeEmbedFirstFramePaint) {
    LOG(WARNING) << "OnNativeEmbedFirstFramePaint failed, no function";
    return;
  }
  LOG(INFO) << "dpf NWebHandlerDelegate::OnNativeEmbedFirstFramePaint embedid "
            << request->GetEmbedId().ToString().c_str() << " surfaceId is "
            << request->GetSurfaceId().ToString().c_str();
  std::shared_ptr<NWebNativeEmbedFirstFramePaintEvent> nweb_request =
      std::make_shared<NWebNativeEmbedFirstFramePaintEvent>(
          request->GetEmbedId().ToString(), request->GetSurfaceId().ToString(),
          request->GetEmbedIdAttribute().ToString());
  web_app_client_extension_listener_->OnNativeEmbedFirstFramePaint(
      web_app_client_extension_listener_->nweb_id, nweb_request);
}
#endif

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
CefRefPtr<CefWebExtensionApiHandler>
NWebHandlerDelegate::GetWebExtensionApiHandler() {
  return this;
}
#endif

CefRefPtr<CefCookieAccessFilter> NWebHandlerDelegate::GetCookieAccessFilter(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request) {
  return this;
}

bool NWebHandlerDelegate::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message) {
  const std::string& messageName = message->GetName();

  if (messageName == "router.push") {
    CefRefPtr<CefListValue> pushMsgArgs = message->GetArgumentList();
    CefString url = pushMsgArgs->GetString(0);
    nweb_handler_->OnRouterPush(url.ToString());
    return true;
  }

  if (messageName == "web.postmessage") {
    CefRefPtr<CefListValue> postMsgArgs = message->GetArgumentList();
    CefString postMsg = postMsgArgs->GetString(0);
    nweb_handler_->OnMessage(postMsg.ToString());
    return true;
  }
#if BUILDFLAG(ARKWEB_FIT_CONTENT)
  if (messageName == "ContentSize.Message") {
    CefRefPtr<CefListValue> postMsgArgs = message->GetArgumentList();
    int height = postMsgArgs->GetInt(1);
    int viewport_width = postMsgArgs->GetInt(2);
    int viewport_height = postMsgArgs->GetInt(3);

    gfx::Size current_viewport_size = render_handler_->GetSize();

    nweb_handler_->OnRootLayerChanged(current_viewport_size.width(), height);
    render_handler_->SetContentSize(current_viewport_size.width(), height);

    if (std::abs(current_viewport_size.width() - viewport_width) >
            VIEW_PORT_DIFF ||
        std::abs(current_viewport_size.height() - viewport_height) >
            VIEW_PORT_DIFF) {
      LOG(DEBUG) << "current viewport is different than last, current width:"
                 << current_viewport_size.width()
                 << ", height :" << current_viewport_size.height()
                 << ", last width :" << viewport_width
                 << ", height :" << viewport_height;
    }
    return true;
  }
#endif

  return false;
}

CefRefPtr<CefFindHandler> NWebHandlerDelegate::GetFindHandler() {
  return this;
}

CefRefPtr<CefKeyboardHandler> NWebHandlerDelegate::GetKeyboardHandler() {
  return this;
}

#if BUILDFLAG(ARKWEB_PRINT)
CefRefPtr<CefPrintHandler> NWebHandlerDelegate::GetPrintHandler() {
  return this;
}
#endif  // BUILDFLAG(ARKWEB_PRINT)

CefRefPtr<CefFormHandler> NWebHandlerDelegate::GetFormHandler() {
  return this;
}

CefRefPtr<CefFrameHandler> NWebHandlerDelegate::GetFrameHandler() {
  return this;
}
/* CefClient methods end */

#if BUILDFLAG(ARKWEB_SCREEN_LOCK)
void NWebHandlerDelegate::SetWakeLockCallback(
    int32_t windowId,
    const std::shared_ptr<NWebScreenLockCallback>& callback) {
  if (main_browser_ && main_browser_->GetHost()) {
    main_browser_->GetHost()->SetWakeLockHandler(
        windowId, callback ? new SetKeepScreenOnCallback(callback) : nullptr);
  } else {
    screen_lock_callback_ = callback;
    screen_lock_window_id_ = windowId;
  }
}
#endif

/* CefFrameHandler methods begin */
void NWebHandlerDelegate::OnMainFrameChanged(CefRefPtr<CefBrowser> browser,
                                             CefRefPtr<CefFrame> old_frame,
                                             CefRefPtr<CefFrame> new_frame) {
  LOG(DEBUG) << "NWebHandlerDelegate::OnMainFrameChanged";
  if (new_frame && browser && browser->IsValid() &&
      preference_delegate_.get()) {
    preference_delegate_->WebPreferencesChanged();
  }
  if (nweb_handler_) {
    nweb_handler_->OnCursorChange(OHOS::NWeb::CursorType::CT_POINTER,
                                  std::make_shared<NWebCursorInfoImpl>());
    nweb_handler_->OnQuickMenuDismissed();
  }
}
/* CefFrameHandler methods end */

/* CefLifeSpanHandler methods begin */

#if BUILDFLAG(ARKWEB_JSPROXY)
void NWebHandlerDelegate::InjectJsToWebInner(
    JsRunTime time,
    ScriptItems& scriptItems,
    ScriptItemsByOrder& scriptItemsByOrder) {
  switch (time) {
    case JsRunTime::Start:
      scriptItems = preference_delegate_->GetJavaScriptOnDocumentStart();
      scriptItemsByOrder =
          preference_delegate_->GetJavaScriptOnDocumentStartByOrder();
      if (scriptItems.size() == 0) {
        main_browser_->GetHost()->JavaScriptOnDocumentStart("", std::vector<CefString>(),
                                                            true);
      }
      break;
    case JsRunTime::End:
      scriptItems = preference_delegate_->GetJavaScriptOnDocumentEnd();
      scriptItemsByOrder =
          preference_delegate_->GetJavaScriptOnDocumentEndByOrder();
      if (scriptItems.size() == 0) {
        main_browser_->GetHost()->JavaScriptOnDocumentEnd("", std::vector<CefString>(),
                                                          true);
      }
      break;
    case JsRunTime::HEAD_READY:
      scriptItems = preference_delegate_->GetJavaScriptOnHeadReady();
      scriptItemsByOrder = preference_delegate_->GetJavaScriptOnHeadReadyByOrder();
      if (scriptItems.size() == 0) {
        main_browser_->GetHost()->JavaScriptOnHeadReady("", std::vector<CefString>(),
                                                        true);
      }
      break;
  }

  if (scriptItemsByOrder.size() == 0) {
    for (const auto& item : scriptItems) {
      scriptItemsByOrder.push_back(item.first);
    }
  }
}

void NWebHandlerDelegate::InjectJsToWeb(JsRunTime time) {
  ScriptItems scriptItems;
  ScriptItemsByOrder scriptItemsByOrder;

  InjectJsToWebInner(time, scriptItems, scriptItemsByOrder);

  int count = 0;
  for (const auto& item : scriptItemsByOrder) {
    if (scriptItems.find(item) == scriptItems.end()) {
      continue;
    }
    CefString script = item;
    std::vector<CefString> scriptRules;
    for (const std::string& rule : scriptItems[item]) {
      CefString cefRule;
      cefRule.FromString(rule);
      scriptRules.push_back(cefRule);
    }
    switch (time) {
      case JsRunTime::Start:
        count++;
        main_browser_->GetHost()->JavaScriptOnDocumentStart(script, scriptRules,
                                                            count == scriptItems.size());
        break;
      case JsRunTime::End:
        count++;
        main_browser_->GetHost()->JavaScriptOnDocumentEnd(script, scriptRules,
                                                          count == scriptItems.size());
        break;
      case JsRunTime::HEAD_READY:
        count++;
        main_browser_->GetHost()->JavaScriptOnHeadReady(script, scriptRules,
                                                        count == scriptItems.size());
        break;
    }
  }
}
#endif

void NWebHandlerDelegate::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
  LOG(INFO) << "NWebHandlerDelegate::OnAfterCreated IsPopup "
            << browser->IsPopup();
  CEF_REQUIRE_UI_THREAD();

  if (browser && browser->GetHost() && window_id_ != 0 && nweb_id_ != 0) {
    browser->GetHost()->SetWindowId(window_id_, nweb_id_);
#if BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)
    browser->GetHost()->SetNWebId(nweb_id_);
#endif
  }

#if BUILDFLAG(ARKWEB_SCREEN_LOCK)
  if (screen_lock_callback_ && browser && browser->GetHost()) {
    browser->GetHost()->SetWakeLockHandler(
        screen_lock_window_id_,
        new SetKeepScreenOnCallback(screen_lock_callback_));
    screen_lock_callback_ = nullptr;
  }
#endif

#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
  if (!main_browser_ && browser->IsPopup()) {
    main_browser_ = browser;
    if (preference_delegate_.get()) {
      preference_delegate_->SetBrowser(main_browser_);
      preference_delegate_->WebPreferencesChanged();
    }
    if (event_handler_.get()) {
      event_handler_->SetBrowser(main_browser_);
    }
    if (main_browser_ && main_browser_->GetHost()) {
      if (preference_delegate_.get()) {
        main_browser_->GetHost()->SetVirtualPixelRatio(
            preference_delegate_->GetVirtualPixelRatio());
        main_browser_->GetHost()->PutUserAgent(
            preference_delegate_->UserAgent());
#if BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
        main_browser_->GetHost()->SetFileAccess(
            preference_delegate_->EnableRawFileAccess());
#endif
#if BUILDFLAG(ARKWEB_BACKGROUND_COLOR)
        main_browser_->GetHost()->SetBackgroundColor(
            preference_delegate_->GetBackgroundColor());
#endif  // BUILDFLAG(ARKWEB_BACKGROUND_COLOR)
#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
        main_browser_->GetHost()->SetAudioExclusive(
            preference_delegate_->GetAudioExclusive());
        main_browser_->GetHost()->SetAudioResumeInterval(
            preference_delegate_->GetAudioResumeInterval());
#endif  // BUILDFLAG(ARKWEB_MEDIA_POLICY)
#if BUILDFLAG(ARKWEB_RENDERER_ANR_DUMP)
        if (popup_window_) {
          SetPopupSurface(popup_window_);
        }
#endif
#if BUILDFLAG(ARKWEB_PRINT)
        main_browser_->GetHost()->SetToken(
            preference_delegate_->GetPrintToken());
#endif
#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
        main_browser_->GetHost()->SetAutofillCallback(
            preference_delegate_->GetAutofillCallback());
#endif

#if BUILDFLAG(ARKWEB_JSPROXY)
        InjectJsToWeb(JsRunTime::Start);
        InjectJsToWeb(JsRunTime::HEAD_READY);
        InjectJsToWeb(JsRunTime::End);
#endif
      }
      main_browser_->GetHost()->SetNativeWindow(window_);

      // window new case, register ark js functions
      ObjectMethodMap::iterator it;
      for (it = javascript_sync_method_map_.begin();
           it != javascript_sync_method_map_.end(); ++it) {
        std::vector<CefString> method_vector;
        for (std::string method : it->second.second) {
          method_vector.push_back(method);
        }
        if (main_browser_ && main_browser_->GetHost()) {
          if (javascript_sync_permission_map_.find(it->first) !=
              javascript_sync_permission_map_.end()) {
            main_browser_->GetHost()->RegisterNativeJSProxy(
                it->second.first, method_vector, it->first, false,
                javascript_sync_permission_map_[it->first]);
          }
        }
      }
      // async method
      for (it = javascript_async_method_map_.begin();
           it != javascript_async_method_map_.end(); ++it) {
        std::vector<CefString> async_method_vector;
        for (std::string method : it->second.second) {
          async_method_vector.push_back(method);
        }
        if (main_browser_ && main_browser_->GetHost()) {
          if (javascript_async_permission_map_.find(it->first) !=
              javascript_async_permission_map_.end()) {
            main_browser_->GetHost()->RegisterNativeJSProxy(
                it->second.first, async_method_vector, it->first, true,
                javascript_async_permission_map_[it->first]);
          }
        }
      }
#if BUILDFLAG(ARKWEB_ADBLOCK)
      if (main_browser_ && main_browser_->GetHost()) {
        main_browser_->GetHost()->EnableAdsBlock(is_global_adblock_enabled_);
      }
#endif
    }
#if BUILDFLAG(ARKWEB_BFCACHE)
    if (main_browser_) {
      int cache_size = preference_delegate_->GetCacheSize();
      int cache_time_to_live = preference_delegate_->GetTimeToLive();
      if (cache_size != -1 && cache_time_to_live != -1) {
        GetBrowser()->SetBackForwardCacheOptions(cache_size,
                                                 cache_time_to_live);
      }
    }
#endif  // BUILDFLAG(ARKWEB_BFCACHE)
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
    if (video_assistant_enabled_) {
      if (main_browser_ && main_browser_->GetHost()) {
        main_browser_->GetHost()->EnableVideoAssistant(
            *video_assistant_enabled_);
      }
    }
    if (custom_web_media_player_enabled_) {
      if (main_browser_ && main_browser_->GetHost()) {
        main_browser_->GetHost()->CustomWebMediaPlayer(
            *custom_web_media_player_enabled_);
      }
    }
#endif  // ARKWEB_VIDEO_ASSISTANT
#if BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)
    if (screen_capture_cb_) {
      if (main_browser_ && main_browser_->GetHost()) {
        LOG(INFO) << "nweb handler delegate register screen capture callback";
        main_browser_->GetHost()->SetNWebId(GetNWebId());
        main_browser_->GetHost()->RegisterScreenCaptureDelegateListener(screen_capture_cb_);
      }
    }
#endif // ARKWEB_EX_SCREEN_CAPTURE
    return;
  }
#endif  // BUILDFLAG(ARKWEB_MULTI_WINDOW)

  if (!main_browser_) {
    main_browser_ = browser;
    if (event_handler_.get()) {
      event_handler_->SetBrowser(browser);
    }
  } else if (browser->IsPopup()) {
    // Add to the list of existing browsers.
    browser_list_.push_back(browser);
  }

  if (preference_delegate_.get()) {
    preference_delegate_->SetBrowser(main_browser_);
  } else {
    LOG(ERROR) << "Failed to set browser to settings delegate";
  }
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (video_assistant_enabled_) {
    if (main_browser_ && main_browser_->GetHost()) {
      main_browser_->GetHost()->EnableVideoAssistant(*video_assistant_enabled_);
    }
  }
  if (custom_web_media_player_enabled_) {
    if (main_browser_ && main_browser_->GetHost()) {
      main_browser_->GetHost()->CustomWebMediaPlayer(
          *custom_web_media_player_enabled_);
    }
  }
#endif  // ARKWEB_VIDEO_ASSISTANT
}

bool NWebHandlerDelegate::DoClose(CefRefPtr<CefBrowser> browser) {
  LOG(INFO) << "NWebHandlerDelegate::DoClose";
  CEF_REQUIRE_UI_THREAD();
  if (nweb_handler_) {
    nweb_handler_->OnWindowExitByJS();

    if (pip_status_ >= 0) {
        LOG(INFO) << "Pip exit " << " " << pip_delegate_id_
                  << " " << pip_child_id_ << " " << pip_frame_routing_id_;
        nweb_handler_->OnPip(1, pip_delegate_id_, pip_child_id_,
                             pip_frame_routing_id_, 0, 0);
    }
  }
  // Closing the main window requires special handling. See the DoClose()
  // documentation in the CEF header for a detailed destription of this
  // process.
  if (browser_list_.size() == 1) {
    // Set a flag to indicate that the window close should be allowed.
    is_closing_ = true;
  }

  // Allow the close. For windowed browsers this will result in the OS close
  // event being sent.
  return false;
}

void NWebHandlerDelegate::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
  LOG(INFO) << "NWebHandlerDelegate::OnBeforeClose";
  CEF_REQUIRE_UI_THREAD();

  // Destruct window here to ensure that the GPU thread has stopped
  // and will not use window again.
  if (is_enhance_surface_) {
    if (releaseSurfaceListener_ != nullptr) {
      LOG(INFO) << "NWebHandlerDelegate:: ReleaseSurface";
      releaseSurfaceListener_->ReleaseSurface();
    }
  } else {
    content::GpuProcessHost* host = content::GpuProcessHost::Get();
    if (host != nullptr && host->gpu_host() != nullptr && main_browser_ != nullptr) {
      host->gpu_host()->DestroyNativeWindow(main_browser_->GetHost()->GetAcceleratedWidget(false));
    }
    OHOS::NWeb::OhosAdapterHelperExt::GetWindowAdapterNdkInstance()
        .DestroyNativeWindow(window_);
    window_ = nullptr;
    OHOS::NWeb::OhosAdapterHelperExt::GetWindowAdapterNdkInstance()
        .DestroyNativeWindow(popup_window_);
    popup_window_ = nullptr;
  }

  // Remove from the list of existing browsers.
  BrowserList::iterator bit = browser_list_.begin();
  for (; bit != browser_list_.end(); ++bit) {
    if ((*bit)->IsSame(browser)) {
      browser_list_.erase(bit);
      break;
    }
  }
}

#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
void NWebHandlerDelegate::NotifyPopupWindowResult(bool result) {
  LOG(INFO) << "NWebHandlerDelegate::NotifyPopupWindowResult result: "
            << result;
  if (!popupWindowCallback_) {
    return;
  }
  if (result) {
    popupWindowCallback_->Continue();
  } else {
    popupWindowCallback_->Cancel();
  }
  popupWindowCallback_ = nullptr;
}
#endif  // BUILDFLAG(ARKWEB_MULTI_WINDOW)

#if BUILDFLAG(ARKWEB_ADBLOCK)
void NWebHandlerDelegate::SaveGlobalAdsBlock(bool enable) {
  is_global_adblock_enabled_ = enable;
}

bool NWebHandlerDelegate::TrigAdBlockEnabledForSiteFromUi(
    CefRefPtr<CefBrowser> browser,
    const CefString& url,
    int main_frame_tree_node_id) {
  LOG(DEBUG) << "[adblock] TrigAdBlockEnabledForSiteFromUi url = ***";

  if (web_app_client_extension_listener_ != nullptr &&
      web_app_client_extension_listener_->TrigAdBlockEnabledForSiteFromUi !=
          nullptr) {
    return web_app_client_extension_listener_->TrigAdBlockEnabledForSiteFromUi(
        url, main_frame_tree_node_id,
        web_app_client_extension_listener_->nweb_id);
  }

  return false;
}
#endif

void NWebHandlerDelegate::SavaArkJSFunctionForPopup(
    const std::string& object_name,
    const std::vector<std::string>& method_list,
    const std::vector<std::string>& async_method_list,
    const int32_t object_id,
    const std::string& permission) {
  if (method_list.empty() && async_method_list.empty()) {
    LOG(INFO) << "NWebHandlerDelegate::SavaArkJSFunctionForPopup method_list "
                 "is empty";
    return;
  }
  // sync method
  MethodPair object_pair;
  if (!method_list.empty()) {
    std::unordered_set<std::string> method_set;
    for (std::string method : method_list) {
      method_set.emplace(method);
    }
    object_pair.first = object_name;
    object_pair.second = method_set;
    javascript_sync_method_map_[object_id] = object_pair;
    javascript_sync_permission_map_[object_id] = permission;
  }

  // async method
  if (!async_method_list.empty()) {
    std::unordered_set<std::string> async_method_set;
    for (std::string method : async_method_list) {
      async_method_set.emplace(method);
    }
    object_pair.first = object_name;
    object_pair.second = async_method_set;
    javascript_async_method_map_[object_id] = object_pair;
    javascript_async_permission_map_[object_id] = permission;
  }
}

bool NWebHandlerDelegate::OnPreBeforePopup(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString& target_url,
    CefLifeSpanHandler::WindowOpenDisposition target_disposition,
    bool user_gesture,
    CefRefPtr<CefCallback> callback) {
  LOG(INFO) << "NWebHandlerDelegate::OnPreBeforePopup";
  CEF_REQUIRE_UI_THREAD();
  if (!preference_delegate_.get() ||
      !preference_delegate_->IsMultiWindowAccess()) {
    return true;
  }
  if (nweb_handler_ == nullptr) {
    return true;
  }
  switch (target_disposition) {
    case CEF_WOD_NEW_WINDOW:
    case CEF_WOD_NEW_POPUP: {
      popIndex_++;
      popupWindowCallback_ = callback;
      std::shared_ptr<NWebControllerHandler> handler =
          std::make_shared<NWebControllerHandlerImpl>(popIndex_, true);
      nweb_handler_->OnWindowNewByJS(target_url, true, user_gesture, handler);
      return false;
    }
    case CEF_WOD_NEW_BACKGROUND_TAB:
    case CEF_WOD_NEW_FOREGROUND_TAB: {
      popIndex_++;
      popupWindowCallback_ = callback;
      std::shared_ptr<NWebControllerHandler> handler =
          std::make_shared<NWebControllerHandlerImpl>(popIndex_, true);
      nweb_handler_->OnWindowNewByJS(target_url, false, user_gesture, handler);
      return false;
    }
    default:
      break;
  }
  return true;
}

bool NWebHandlerDelegate::OnBeforePopup(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    int popup_id,
    const CefString& target_url,
    const CefString& target_frame_name,
    CefLifeSpanHandler::WindowOpenDisposition target_disposition,
    bool user_gesture,
    const CefPopupFeatures& popup_features,
    CefWindowInfo& window_info,
    CefRefPtr<CefClient>& client,
    CefBrowserSettings& settings,
    CefRefPtr<CefDictionaryValue>& extra_info,
    bool* no_javascript_access) {
#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
  LOG(INFO) << "NWebHandlerDelegate::OnBeforePopup";
  CEF_REQUIRE_UI_THREAD();
  if (!preference_delegate_.get()) {
    LOG(ERROR)
        << "NWebHandlerDelegate::OnBeforePopup preference_delegate is null";
    return true;
  }
  if (preference_delegate_->IsMultiWindowAccess()) {
    switch (target_disposition) {
      case CEF_WOD_NEW_WINDOW:
      case CEF_WOD_NEW_POPUP:
      case CEF_WOD_NEW_BACKGROUND_TAB:
      case CEF_WOD_NEW_FOREGROUND_TAB: {
        if (nweb_handler_ == nullptr) {
          LOG(ERROR)
              << "NWebHandlerDelegate::OnBeforePopup nweb_handler is null";
          return true;
        }
        std::shared_ptr<NWebControllerHandler> handler =
            std::make_shared<NWebControllerHandlerImpl>(popIndex_, false);
        nweb_handler_->OnWindowNewByJS(target_url, true, user_gesture, handler);
        if (extra_info) {
          extra_info->SetInt("nweb_id", handler->GetNWebHandlerId());
        }
        NWebImpl* nweb = NWebImpl::FromID(handler->GetNWebHandlerId());
        if (!nweb) {
          LOG(ERROR) << "NWebHandlerDelegate::OnBeforePopup nweb is null";
          return true;
        }
        client = nweb->GetCefClient();
        if (!client) {
          LOG(ERROR) << "NWebHandlerDelegate::OnBeforePopup client is null";
          return true;
        }
        auto preference = nweb->GetPreference();
        if (preference) {
          CefRefPtr<CefCommandLine> command_line =
              CefCommandLine::GetGlobalCommandLine();
          if (command_line->HasSwitch(kOffScreenFrameRate)) {
            settings.windowless_frame_rate =
                atoi(command_line->GetSwitchValue(kOffScreenFrameRate)
                         .ToString()
                         .c_str());
          }
          static_cast<NWebPreferenceDelegate*>(preference.get())
              ->ComputeBrowserSettings(settings);
        } else {
          preference_delegate_->ComputeBrowserSettings(settings);
        }
        CefWindowHandle handle = kNullWindowHandle;
        window_info.SetAsWindowless(handle);
        return false;
      }
      default:
        break;
    }
  }
  if (main_browser_) {
    preference_delegate_->WebPreferencesChanged();
#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
    main_browser_->GetMainFrame()->AsArkWebFrame()->LoadURLWithUserGesture(
        target_url, user_gesture);
#else
    main_browser_->GetMainFrame()->LoadURL(target_url);
#endif
  }
#endif  // BUILDFLAG(ARKWEB_MULTI_WINDOW)
  return true;
}

/* CefLifeSpanHandler methods end */

/* CefLoadHandler methods begin */
void NWebHandlerDelegate::OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                               bool is_loading,
                                               bool can_go_back,
                                               bool can_go_forward) {}

void NWebHandlerDelegate::OnLoadStart(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame,
                                      const CefString& url,
                                      TransitionType transition_type) {
  LOG(INFO) << "NWebHandlerDelegate::OnLoadStart";
  if (frame == nullptr || !frame->IsMain()) {
    return;
  }
#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  if (!setWebPaintedTask_.IsCancelled()) {
    setWebPaintedTask_.Cancel();
  }
  isWebPaintedForSnapshot_ = false;
#endif

  if (nweb_handler_ != nullptr) {
    nweb_handler_->OnPageLoadBegin(url.ToString());
    browser->GetHost()->OnTextSelected(false);
  }

  if (onLoadStartCallback_) {
    onLoadStartCallback_();
  }
}

void NWebHandlerDelegate::OnLoadEnd(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    int http_status_code) {
  LOG(INFO) << "NWebHandlerDelegate::OnLoadEnd";
  if (frame == nullptr || !frame->IsMain()) {
    return;
  }
  LOG(INFO) << "NWebHandlerDelegate:: Mainframe OnLoadEnd";

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  setWebPaintedTask_.Reset(
      base::BindOnce(&NWebHandlerDelegate::SetWebPaintedForSnapshot,
                     weak_factory_.GetWeakPtr()));
  content::GetUIThreadTaskRunner({})->PostDelayedTask(
      FROM_HERE, setWebPaintedTask_.callback(),
      base::Milliseconds(WEB_CAN_SNAPSHOT_DELAY_TIME));
#endif

  if (nweb_handler_ != nullptr) {
    nweb_handler_->OnPageLoadEnd(http_status_code, frame->GetURL().ToString());
  }
  if (onLoadEndCallback_) {
    onLoadEndCallback_();
  }

#if defined(REPORT_SYS_EVENT)
  std::string error_type = "";
  std::string error_desc =
      "refer to "
      "https://www.iana.org/assignments/http-status-codes/"
      "http-status-codes.xml";
  if (http_status_code < 400) {
    access_success_count_++;
  } else if (http_status_code >= 400 && http_status_code < 500) {
    error_type = "http client error";
    access_fail_count_++;
    ReportPageLoadErrorInfo(nweb_id_, error_type, http_status_code, error_desc);
  } else {
    access_fail_count_++;
  }
  access_sum_count_ = access_success_count_ + access_fail_count_;
  ReportPageLoadStats(nweb_id_, access_sum_count_, access_success_count_,
                      access_fail_count_);
#endif
}

void NWebHandlerDelegate::OnPageVisible(CefRefPtr<CefBrowser> browser,
                                        const CefString& url,
                                        bool success) {
  LOG(INFO) << "NWebHandlerDelegate::OnPageVisible";
  if (nweb_handler_ != nullptr) {
    nweb_handler_->OnPageVisible(url);
  }
}

void NWebHandlerDelegate::OnFirstContentfulPaint(
    int64_t navigationStartTick,
    int64_t firstContentfulPaintMs) {
  LOG(INFO) << "NWebHandlerDelegate::OnFirstContentfulPaint";
  LOG(DEBUG) << "Web Load Performance FCP: " << firstContentfulPaintMs;
  if (nweb_handler_ != nullptr) {
    nweb_handler_->OnFirstContentfulPaint(navigationStartTick,
                                          firstContentfulPaintMs);
  }
}

void NWebHandlerDelegate::OnFirstMeaningfulPaint(
    CefRefPtr<CefFirstMeaningfulPaintDetails> details) {
  LOG(INFO) << "NWebHandlerDelegate::OnFirstMeaningfulPaint";
#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  isWebPaintedForSnapshot_ = true;
#endif
  if (nweb_handler_ != nullptr) {
    if (!details) {
      LOG(WARNING) << "NWebHandlerDelegate::OnFirstMeaningfulPaint failed "
                      "for details is null";
      return;
    }
    std::shared_ptr<NWebFirstMeaningfulPaintDetails> web_details =
        std::make_shared<NWebFirstMeaningfulPaintDetailsImpl>(
            details->GetNavigationStartTime(),
            details->GetFirstMeaningfulPaintTime());
    nweb_handler_->OnFirstMeaningfulPaint(web_details);
  }
}

void NWebHandlerDelegate::OnLargestContentfulPaint(
    CefRefPtr<CefLargestContentfulPaintDetails> details) {
  LOG(INFO) << "NWebHandlerDelegate::OnLargestContentfulPaint";
  if (nweb_handler_ != nullptr) {
    if (!details) {
      LOG(WARNING) << "NWebHandlerDelegate::OnLargestContentfulPaint failed "
                      "for details is null";
      return;
    }
    std::shared_ptr<NWebLargestContentfulPaintDetails> web_details =
        std::make_shared<NWebLargestContentfulPaintDetailsImpl>(
            details->GetNavigationStartTime(),
            details->GetLargestImagePaintTime(),
            details->GetLargestTextPaintTime(),
            details->GetLargestImageLoadStartTime(),
            details->GetLargestImageLoadEndTime(), details->GetImageBPP());
    nweb_handler_->OnLargestContentfulPaint(web_details);
  }
}

void NWebHandlerDelegate::OnSafeBrowsingCheckResult(int threat_type) {
  LOG(INFO) << "NWebHandlerDelegate::OnSafeBrowsingCheckResult";
  if (nweb_handler_ != nullptr) {
    nweb_handler_->OnSafeBrowsingCheckResult(threat_type);
  }
}

void NWebHandlerDelegate::OnDataResubmission(CefRefPtr<CefBrowser> browser,
                                             CefRefPtr<CefCallback> callback) {
  LOG(INFO) << "NWebHandlerDelegate::OnDataResubmission";
  if (nweb_handler_ != nullptr) {
    std::shared_ptr<NWebDataResubmissionCallback> handler =
        std::make_shared<NWebDataResubmissionCallbackImpl>(callback);
    nweb_handler_->OnDataResubmission(handler);
  }
}

void NWebHandlerDelegate::OnNavigationEntryCommitted(
    CefRefPtr<CefLoadCommittedDetails> details) {
  LOG(INFO) << "NWebHandlerDelegate::OnNavigationEntryCommitted";
#if BUILDFLAG(ARKWEB_NAVIGATION)
  if (nweb_handler_ != nullptr) {
    if (!details) {
      LOG(WARNING) << "NWebHandlerDelegate::OnNavigationEntryCommitted failed "
                      "for details is null";
      return;
    }
    auto type = static_cast<NWebLoadCommittedDetails::NavigationType>(
        details->GetNavigationType());
    std::shared_ptr<NWebLoadCommittedDetails> web_details =
        std::make_shared<NWebLoadCommittedDetailsImpl>(
            details->GetCurrentURL().ToString(), type, details->IsMainFrame(),
            details->IsSameDocument(), details->DidReplaceEntry());
    nweb_handler_->OnNavigationEntryCommitted(web_details);
    if (main_browser_ && main_browser_->GetHost()) {
      main_browser_->GetHost()->OnTextSelected(false);
    }
  }
#else
  return;
#endif
}

// Returns a data: URI with the specified contents.
std::string GetDataURI(const std::string& data, const std::string& mime_type) {
  return "data:" + mime_type + ";base64," +
         CefURIEncode(CefBase64Encode(data.data(), data.size()), false)
             .ToString();
}

void NWebHandlerDelegate::OnLoadError(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame,
                                      ErrorCode error_code,
                                      const CefString& error_text,
                                      const CefString& failed_url) {
  LOG(INFO) << "NWebHandlerDelegate::OnLoadError";
  CEF_REQUIRE_UI_THREAD();

  // Don't display an error for downloaded files.
  if (error_code == ERR_ABORTED) {
    return;
  }

  if (nweb_handler_ != nullptr) {
    nweb_handler_->OnPageLoadError(error_code, error_text.ToString(),
                                   failed_url.ToString());
  } else {
    // Display a load error message using a data: URI.
    std::stringstream ss;
    ss << "<html><body bgcolor=\"white\">"
          "<h2>Failed to load URL "
       << std::string(failed_url) << " with error " << std::string(error_text)
       << " (" << error_code << ").</h2></body></html>";

    frame->LoadURL(GetDataURI(ss.str(), "text/html"));
  }

#if defined(REPORT_SYS_EVENT)
  std::string error_type = "failded url";
  access_fail_count_++;
  access_sum_count_ = access_success_count_ + access_fail_count_;
  ReportPageLoadErrorInfo(nweb_id_, error_type, int(error_code),
                          std::string(error_text));
  ReportPageLoadStats(nweb_id_, access_sum_count_, access_success_count_,
                      access_fail_count_);
#endif
}

void NWebHandlerDelegate::OnLoadErrorWithRequest(CefRefPtr<CefRequest> request,
                                                 bool is_main_frame,
                                                 bool has_user_gesture,
                                                 int error_code,
                                                 const CefString& error_text) {
#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  if (error_code == ERR_ABORTED) {
    LOG(WARNING) << "ignoring the error";
    return;
  }
#endif
  CefRequest::HeaderMap cef_request_headers;
  request->GetHeaderMap(cef_request_headers);
  std::map<std::string, std::string> request_headers;
  ConvertMapToHeaderMap(cef_request_headers, request_headers);
  std::shared_ptr<NWebUrlResourceRequest> web_request =
      std::make_shared<NWebUrlResourceRequestImpl>(
          request->GetMethod().ToString(), request_headers,
          request->GetURL().ToString(), has_user_gesture, is_main_frame);
  std::shared_ptr<NWebUrlResourceError> error =
      std::make_shared<UrlResourceErrorImpl>(error_code, error_text.ToString());
  if (nweb_handler_ != nullptr) {
    nweb_handler_->OnResourceLoadError(web_request, error);
  }

#if defined(REPORT_SYS_EVENT)
  std::string error_type = "resource load error";
  access_fail_count_++;
  access_sum_count_ = access_success_count_ + access_fail_count_;
  ReportPageLoadErrorInfo(nweb_id_, error_type, error_code,
                          error_text.ToString());
  ReportPageLoadStats(nweb_id_, access_sum_count_, access_success_count_,
                      access_fail_count_);
#endif
}

void NWebHandlerDelegate::OnHttpError(CefRefPtr<CefRequest> request,
                                      bool is_main_frame,
                                      bool has_user_gesture,
                                      CefRefPtr<CefResponse> response) {
  if (nweb_handler_ != nullptr) {
    CefRequest::HeaderMap cef_request_headers;
    request->GetHeaderMap(cef_request_headers);
    std::map<std::string, std::string> request_headers;
    ConvertMapToHeaderMap(cef_request_headers, request_headers);
    std::shared_ptr<NWebUrlResourceRequest> web_request =
        std::make_shared<NWebUrlResourceRequestImpl>(
            request->GetMethod().ToString(), request_headers,
            request->GetURL().ToString(), has_user_gesture, is_main_frame);

    std::string data;
    CefResponse::HeaderMap cef_response_headers;
    response->GetHeaderMap(cef_response_headers);
    std::map<std::string, std::string> response_headers;
    ConvertMapToHeaderMap(cef_response_headers, response_headers);
    std::shared_ptr<NWebUrlResourceResponse> web_response =
        std::make_shared<NWebUrlResourceResponseImpl>(
            response->GetMimeType(), response->GetCharset(),
            response->GetStatus(), response->GetStatusText(), response_headers,
            data);
    nweb_handler_->OnHttpError(web_request, web_response);
  }

#if defined(REPORT_SYS_EVENT)
  std::string error_type = "http error";
  access_fail_count_++;
  access_sum_count_ = access_success_count_ + access_fail_count_;
  ReportPageLoadErrorInfo(nweb_id_, error_type, response->GetStatus(),
                          std::string(response->GetStatusText()));
  ReportPageLoadStats(nweb_id_, access_sum_count_, access_success_count_,
                      access_fail_count_);
#endif
}

void NWebHandlerDelegate::OnRefreshAccessedHistory(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString& url,
    bool isReload) {
  std::string url1 = url.ToString();
  auto pos = url1.find("?");
  url1 = url1.substr(0, pos);
  LOG(DEBUG) << "NWebHandlerDelegate::OnRefreshAccessedHistory, intercepted "
                "url: ***, isReload = "
             << isReload;
  if (nweb_handler_ == nullptr) {
    LOG(ERROR) << "nweb handler is null";
    return;
  }
  edited_forms_id_.clear();
  nweb_handler_->OnRefreshAccessedHistory(url.ToString(), isReload);
}

#if BUILDFLAG(ARKWEB_MEDIA_MUTE_AUDIO)
void NWebHandlerDelegate::OnAudioStateChanged(CefRefPtr<CefBrowser> browser,
                                              bool audible) {
  if (nweb_handler_ != nullptr) {
    nweb_handler_->OnAudioStateChanged(audible);
  }
}
#endif  // BUILDFLAG(ARKWEB_MEDIA_MUTE_AUDIO)

#if BUILDFLAG(ARKWEB_ACTIVITY_STATE)
void NWebHandlerDelegate::OnMediaStateChanged(CefRefPtr<CefBrowser> browser,
                                              MediaType type,
                                              MediaPlayingState state) {
  LOG(INFO) << "NWebHandlerDelegate::OnMediaStateChanged, MediaType: "
            << static_cast<int>(type)
            << " MediaPlayingState: " << static_cast<int>(state)
            << " nweb_id: " << nweb_id_;
  ActivityType mediaType = static_cast<ActivityType>(type);

  if (nweb_handler_ != nullptr) {
    nweb_handler_->OnActivityStateChanged(static_cast<int>(state), mediaType);
  }

#if BUILDFLAG(ARKWEB_NWEB_EX)
  if (web_app_client_extension_listener_ != nullptr &&
      web_app_client_extension_listener_->OnActivityStateChanged != nullptr) {
    web_app_client_extension_listener_->OnActivityStateChanged(
        static_cast<int>(state), static_cast<int>(mediaType),
        web_app_client_extension_listener_->nweb_id);
  }
#endif  // BUILDFLAG(ARKWEB_NWEB_EX)
  return;
}
#endif

/* CefLoadHandler methods end */

/* CefRequestHandler methods begin */
bool NWebHandlerDelegate::OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                                         CefRefPtr<CefFrame> frame,
                                         CefRefPtr<CefRequest> request,
                                         bool user_gesture,
                                         bool is_redirect) {
  LOG(INFO) << "NWebHandlerDelegate::OnBeforeBrowse";
  (void)(browser);

  if (!request) {
    LOG(ERROR) << "NWebHandlerDelegate::OnBeforeBrowse request is null";
    return false;
  }

  CefRequest::HeaderMap cef_request_headers;
  request->GetHeaderMap(cef_request_headers);
  std::map<std::string, std::string> request_headers;
  ConvertMapToHeaderMap(cef_request_headers, request_headers);
  std::shared_ptr<NWebUrlResourceRequest> nweb_request =
      std::make_shared<NWebUrlResourceRequestImpl>(
          request->GetMethod().ToString(), request_headers,
          request->GetURL().ToString(), user_gesture, frame->IsMain(),
          is_redirect);
  bool result = false;
  if (nweb_handler_ != nullptr) {
    result = nweb_handler_->OnHandleInterceptUrlLoading(nweb_request);
    LOG(DEBUG) << "NWebHandlerDelegate::OnBeforeBrowse "
                  "OnHandleInterceptUrlLoading result: "
               << result;
    return result;
  }
  LOG(DEBUG) << "NWebHandlerDelegate::OnBeforeBrowse result: " << result;
  return result;
}

bool NWebHandlerDelegate::OnCertificateError(CefRefPtr<CefBrowser> browser,
                                             cef_errorcode_t cert_error,
                                             const CefString& request_url,
                                             CefRefPtr<CefSSLInfo> ssl_info,
                                             CefRefPtr<CefCallback> callback) {
  LOG(INFO) << "NWebHandlerDelegate::OnCertificateError happened";
  SslError error = SslErrorConvert(cert_error);

  CEF_REQUIRE_IO_THREAD();

  std::shared_ptr<NWebJSSslErrorResult> js_result =
      std::make_shared<NWebJSSslErrorResultImpl>(callback);

  if (ssl_info == nullptr) {
    if (nweb_handler_ != nullptr) {
      return nweb_handler_->OnSslErrorRequestByJS(js_result, error);
    }
    return false;
  }

  std::vector<std::string> certChainData;
  CefRefPtr<CefX509Certificate> cert = ssl_info->GetX509Certificate();
  CefX509Certificate::IssuerChainBinaryList der_chain_list;
  if (cert == nullptr) {
    if (nweb_handler_ != nullptr) {
      return nweb_handler_->OnSslErrorRequestByJS(js_result, error);
    }
    return false;
  }

  cert->GetDEREncodedIssuerChain(der_chain_list);
  der_chain_list.insert(der_chain_list.begin(), cert->GetDEREncoded());

  for (size_t i = 0U; i < der_chain_list.size(); ++i) {
    if (!der_chain_list[i].get()) {
      LOG(ERROR) << "OnCertificateError Get CertChainData failed, der chain "
                    "data is null, index = "
                 << i;
      continue;
    }

    const size_t cert_data_size = der_chain_list[i]->GetSize();
    std::string cert_data_item;
    cert_data_item.resize(cert_data_size);
    der_chain_list[i]->GetData(const_cast<char*>(cert_data_item.data()),
                               cert_data_size, 0);
    certChainData.emplace_back(cert_data_item);
  }

  if (nweb_handler_ != nullptr) {
    bool flag =
        nweb_handler_->OnSslErrorRequestByJSV2(js_result, error, certChainData);
    if (ArkWebGetErrno() != ArkWebInterfaceResult::RESULT_OK) {
      flag = nweb_handler_->OnSslErrorRequestByJS(js_result, error);
    }
    return flag;
  }
  return false;
}

bool NWebHandlerDelegate::OnSelectClientCertificate(
    CefRefPtr<CefBrowser> browser,
    bool isProxy,
    const CefString& host,
    int port,
    const std::vector<CefString>& key_types,
    const std::vector<CefString>& principals,
    const CefRequestHandler::X509CertificateList& certificates,
    CefRefPtr<CefSelectClientCertificateCallback> callback) {
  LOG(INFO) << "NWebHandlerDelegate::OnSelectClientCertificate";
  CEF_REQUIRE_IO_THREAD();

  std::vector<std::string> key_types_str;
  for (std::string value : key_types) {
    key_types_str.push_back(value);
  }

  std::vector<std::string> principals_str;
  for (std::string value : principals) {
    principals_str.push_back(value);
  }

  std::shared_ptr<NWebJSSslSelectCertResultImpl> js_result =
      std::make_shared<NWebJSSslSelectCertResultImpl>(callback);
  if (nweb_handler_ != nullptr) {
    return nweb_handler_->OnSslSelectCertRequestByJS(
        js_result, host, port, key_types_str, principals_str);
  }
  return false;
}

CefRefPtr<CefResourceRequestHandler>
NWebHandlerDelegate::GetResourceRequestHandler(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    bool is_navigation,
    bool is_download,
    const CefString& request_initiator,
    bool& disable_default_handling) {
  return this;
}

void NWebHandlerDelegate::OnRenderProcessTerminated(
    CefRefPtr<CefBrowser> browser,
    TerminationStatus status,
    int error_code,
    const CefString& error_string) {
  if (nweb_handler_ == nullptr) {
    LOG(ERROR) << "invalid nweb handler (nullptr)";
    return;
  }

  RenderExitReason reason;
  std::string error_desc = "";
  switch (status) {
    case TS_ABNORMAL_TERMINATION:
      reason = RenderExitReason::PROCESS_ABNORMAL_TERMINATION;
      error_desc = "process abnormal termination";
      break;
    case TS_PROCESS_WAS_KILLED:
      reason = RenderExitReason::PROCESS_WAS_KILLED;
      error_desc = "process was killed";
      break;
    case TS_PROCESS_CRASHED:
      reason = RenderExitReason::PROCESS_CRASHED;
      error_desc = "process crashed";
      break;
    case TS_PROCESS_OOM:
      reason = RenderExitReason::PROCESS_OOM;
      error_desc = "process out of memory";
      break;
    default:
      reason = RenderExitReason::PROCESS_EXIT_UNKNOWN;
      error_desc = "process exit unknown";
      break;
  }

  LOG(INFO) << "NWebId: " << nweb_id_
            << " render process exit, reason = " << static_cast<int>(reason)
            << " reason info = " << error_desc;
  nweb_handler_->OnRenderExited(reason);

#if defined(REPORT_SYS_EVENT)
  std::string error_type = "render exitted";
  ReportPageLoadErrorInfo(nweb_id_, error_type, static_cast<int>(reason),
                          error_desc);
#endif

#if BUILDFLAG(ARKWEB_NWEB_EX) && BUILDFLAG(ARKWEB_CRASHPAD)
  OHOS::NWeb::ReportFeedbacklogsCrashDmpFiles(
      NWebImpl::GetDefaultFeedbacklogsCrashPath());
#endif
}

bool NWebHandlerDelegate::GetAuthCredentials(
    CefRefPtr<CefBrowser> browser,
    const CefString& origin_url,
    bool isProxy,
    const CefString& host,
    int port,
    const CefString& realm,
    const CefString& scheme,
    CefRefPtr<CefAuthCallback> callback) {
  CEF_REQUIRE_IO_THREAD();
  LOG(INFO) << "NWebHandlerDelegate: GetAuthCredentials";
  std::shared_ptr<NWebJSHttpAuthResult> js_result =
      std::make_shared<NWebJSHttpAuthResultImpl>(callback);
  if (nweb_handler_ != nullptr) {
    return nweb_handler_->OnHttpAuthRequestByJS(js_result, host, realm);
  }
  return false;
}

bool NWebHandlerDelegate::ShouldOverrideUrlLoading(
    CefRefPtr<CefBrowser> browser,
    const CefString& url,
    const CefString& method,
    bool user_gesture,
    bool is_redirect,
    bool is_outermost_main_frame) {
  LOG(INFO) << "NWebHandlerDelegate::ShouldOverrideUrlLoading";

  std::map<std::string, std::string> request_headers;
  std::shared_ptr<NWebUrlResourceRequest> nweb_request =
      std::make_shared<NWebUrlResourceRequestImpl>(
          method.ToString(), request_headers, url.ToString(), user_gesture,
          is_outermost_main_frame, is_redirect);
  bool result = false;
  if (nweb_handler_ != nullptr) {
    result = nweb_handler_->OnHandleOverrideUrlLoading(nweb_request);
    LOG(DEBUG) << "NWebHandlerDelegate::ShouldOverrideUrlLoading "
                  "OnHandleOverrideUrlLoading result: "
               << result;
    return result;
  }
  LOG(DEBUG) << "NWebHandlerDelegate::ShouldOverrideUrlLoading result: "
             << result;
  return result;
}

bool NWebHandlerDelegate::OnOpenAppLink(
    const CefString& url,
    CefRefPtr<CefOpenAppLinkCallback> callback) {
  std::shared_ptr<NWebAppLinkCallback> nweb_callback =
      std::make_shared<NWebAppLinkCallbackImpl>(callback);
  if (nweb_handler_ != nullptr) {
    return nweb_handler_->OnOpenAppLink(url.ToString(), nweb_callback);
  }
  return false;
}

void NWebHandlerDelegate::OnFullscreenModeChange(
    CefRefPtr<CefBrowser> browser,
    bool full_screen,
    const CefSize& video_natural_size) {
  LOG(DEBUG) << __func__ << " is_fullscreen? " << full_screen
             << ", video_natural_size: " << video_natural_size.width << "x"
             << video_natural_size.height;
  if (nweb_handler_ == nullptr) {
    LOG(ERROR) << "OnFullscreenModeChange nweb_handler_ is null";
    return;
  }
  if (full_screen) {
    std::shared_ptr<NWebFullScreenExitHandler> handler =
        std::make_shared<NWebFullScreenExitHandlerImpl>(browser);
    nweb_handler_->OnFullScreenEnterWithVideoSize(
        handler, video_natural_size.width, video_natural_size.height);
  } else {
    nweb_handler_->OnFullScreenExit();
  }
}

/* CefRequestHandler methods end */

/* CefDownloadHandler methods begin */
bool NWebHandlerDelegate::OnBeforeDownload(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefDownloadItem> download_item,
    const CefString& suggested_name,
    CefRefPtr<CefBeforeDownloadCallback> callback) {
  LOG(INFO) << "NWebHandlerDelegate::OnBeforeDownload";
  if (download_item->IsValid() == false) {
    LOG(ERROR) << "NWebHandlerDelegate::OnBeforeDownload error, not invalid";
    return false;
  }

  if (download_listener_ != nullptr) {
    download_listener_->OnDownloadStart(
        download_item->GetURL().ToString(),
        browser->GetHost()->DefaultUserAgent(),
        download_item->GetContentDisposition().ToString(),
        download_item->GetMimeType().ToString(),
        download_item->GetTotalBytes());
  }
  return true;
}

/* CefDownloadHandler methods end */

/* CefKeyboardHandler methods begin */
bool NWebHandlerDelegate::OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
                                        const CefKeyEvent& event,
                                        CefEventHandle os_event,
                                        bool* is_keyboard_shortcut) {
  LOG(DEBUG) << "NWebHandlerDelegate::OnPreKeyEvent type:" << event.type
             << ", win:" << event.windows_key_code;
  if (nweb_handler_ != nullptr) {
    int32_t action =
        NWebInputDelegate::CefConverter("ohoskeyaction", event.type);
    if (action == -1) {
      return false;
    }
    int32_t keyCode =
        NWebInputDelegate::CefConverter("ohoskeycode", event.windows_key_code);
    if (keyCode == -1) {
      return false;
    }
    std::shared_ptr<NWebKeyEvent> nwebEvent =
        std::make_shared<NWebKeyEventImpl>(action, keyCode);
    return nweb_handler_->OnPreKeyEvent(nwebEvent);
  }
  return false;
}

bool NWebHandlerDelegate::OnKeyEvent(CefRefPtr<CefBrowser> browser,
                                     const CefKeyEvent& event,
                                     CefEventHandle os_event) {
  LOG(DEBUG) << "NWebHandlerDelegate::OnKeyEvent type:" << event.type
             << ", win:" << event.windows_key_code;
  if (nweb_handler_ != nullptr) {
    int32_t action =
        NWebInputDelegate::CefConverter("ohoskeyaction", event.type);
    if (action == -1) {
      return false;
    }
    int32_t keyCode =
        NWebInputDelegate::CefConverter("ohoskeycode", event.windows_key_code);
    if (keyCode == -1) {
      return false;
    }
    std::shared_ptr<NWebKeyEvent> nwebEvent =
        std::make_shared<NWebKeyEventImpl>(action, keyCode);
    return nweb_handler_->OnUnProcessedKeyEvent(nwebEvent);
  }
  return false;
}

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void NWebHandlerDelegate::KeyboardReDispatch(const CefKeyEvent& event,
                                             bool isUsed) {
  LOG(INFO) << "NWebHandlerDelegate::KeyboardReDispatch type:" << event.type
            << ", win:" << event.windows_key_code << ", isUsed:" << isUsed;
  if (nweb_handler_ != nullptr) {
    int32_t action =
        NWebInputDelegate::CefConverter("ohoskeyaction", event.type);
    if (action == -1) {
      return;
    }
    int32_t keyCode =
        NWebInputDelegate::CefConverter("ohoskeycode", event.windows_key_code);
    if (keyCode == -1) {
      return;
    }
    std::shared_ptr<NWebKeyEvent> nwebEvent =
        std::make_shared<NWebKeyEventImpl>(action, keyCode);
    return nweb_handler_->KeyboardReDispatch(nwebEvent, isUsed);
  }
}

void NWebHandlerDelegate::OnTakeFocus(CefRefPtr<CefBrowser> browser,
                                      bool next) {
  // Focus is triggered by pressing the tab key on the last element.
  LOG(INFO) << "NWebHandlerDelegate::OnTakeFocus next:" << next;
  int32_t keyCode = NWebInputDelegate::CefConverter(
      "ohoskeycode", static_cast<int32_t>(ui::VKEY_TAB));
  std::shared_ptr<NWebKeyEvent> nwebEvent =
      std::make_shared<NWebKeyEventImpl>(0, keyCode);
  return nweb_handler_->KeyboardReDispatch(nwebEvent, false);
}

bool NWebHandlerDelegate::IsCurrentFocus() {
  return nweb_handler_ ? nweb_handler_->IsCurrentFocus() : false;
}
#endif
/* CefKeyboardHandler methods end */

/* CefResourceRequestHandler method begin */
CefResourceRequestHandler::ReturnValue
NWebHandlerDelegate::OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
                                          CefRefPtr<CefFrame> frame,
                                          CefRefPtr<CefRequest> request,
                                          CefRefPtr<CefCallback> callback) {
  if (nweb_handler_ != nullptr) {
    nweb_handler_->OnResource(request->GetURL().ToString());
  }
  return RV_CONTINUE;
}

CefRefPtr<CefResourceHandler> NWebHandlerDelegate::GetResourceHandler(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request) {
  if (!request) {
    LOG(ERROR) << "NWebHandlerDelegate::GetResourceHandler request is null";
    return nullptr;
  }

  CefRequest::HeaderMap cef_request_headers;
  request->GetHeaderMap(cef_request_headers);
  std::map<std::string, std::string> request_headers;
  ConvertMapToHeaderMap(cef_request_headers, request_headers);
  std::shared_ptr<NWebUrlResourceRequest> NWeb_request =
      std::make_shared<NWebUrlResourceRequestImpl>(
          request->GetMethod().ToString(), request_headers,
          request->GetURL().ToString(), false,
          request->AsArkWebRequestExt()->IsMainFrame());
  std::shared_ptr<NWebUrlResourceResponse> response =
      std::make_shared<NWebUrlResourceResponseImpl>();
  if (nweb_handler_->OnHandleInterceptRequest(NWeb_request, response)) {
    std::string str = "";
    return new NWebResourceHandler(response, str);
  } else {
    return nullptr;
  }
}

void NWebHandlerDelegate::GetResourceHandlerByIO(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    CefRefPtr<CefInterceptCallback> callback,
    CefRefPtr<CefSchemeHandlerFactory> scheme_factory,
    const CefString& scheme) {
  if (!content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    content::GetUIThreadTaskRunner({})->PostTask(
        FROM_HERE, base::BindOnce(&NWebHandlerDelegate::GetResourceHandlerByIO,
                                  this, browser, frame, request, callback,
                                  scheme_factory, scheme));
    return;
  }
  if (!request) {
    LOG(ERROR) << "NWebHandlerDelegate::GetResourceHandlerByIO request is null";
    return;
  }
  CefRequest::HeaderMap cef_request_headers;
  request->GetHeaderMap(cef_request_headers);
  std::map<std::string, std::string> request_headers;
  ConvertMapToHeaderMap(cef_request_headers, request_headers);
  std::shared_ptr<NWebUrlResourceRequest> nweb_request =
      std::make_shared<NWebUrlResourceRequestImpl>(
          request->GetMethod().ToString(), request_headers,
          request->GetURL().ToString(), false,
          request->AsArkWebRequestExt()->IsMainFrame());
  std::shared_ptr<NWebUrlResourceResponse> response =
      std::make_shared<NWebUrlResourceResponseImpl>();
  CefRefPtr<CefResourceHandler> resource_handler = nullptr;
  if (nweb_handler_->OnHandleInterceptRequest(nweb_request, response)) {
    std::string tag = "";
    resource_handler = new NWebResourceHandler(response, tag);
  } else if (scheme_factory) {
    // here to get ets schemeHandler
    resource_handler = scheme_factory->Create(browser, frame, scheme, request);
  }
  callback->ContinueLoad(resource_handler);
}
/* CefResourceRequestHandler method end */

/* CefPrintHandler method begin */
#if BUILDFLAG(ARKWEB_PRINT)
void NWebHandlerDelegate::OnPrintStart(CefRefPtr<CefBrowser> browser) {
  LOG(INFO) << "NWebHandlerDelegate::OnPrintStart";
}

void NWebHandlerDelegate::OnPrintSettings(CefRefPtr<CefBrowser> browser,
                                          CefRefPtr<CefPrintSettings> settings,
                                          bool get_defaults) {
  LOG(INFO) << "NWebHandlerDelegate::OnPrintSettings";
}

bool NWebHandlerDelegate::OnPrintDialog(
    CefRefPtr<CefBrowser> browser,
    bool has_selection,
    CefRefPtr<CefPrintDialogCallback> callback) {
  LOG(INFO) << "NWebHandlerDelegate::OnPrintDialog";
  return false;
}

bool NWebHandlerDelegate::OnPrintJob(CefRefPtr<CefBrowser> browser,
                                     const CefString& document_name,
                                     const CefString& pdf_file_path,
                                     CefRefPtr<CefPrintJobCallback> callback) {
  LOG(INFO) << "NWebHandlerDelegate::OnPrintJob";
  return false;
}

void NWebHandlerDelegate::OnPrintReset(CefRefPtr<CefBrowser> browser) {
  LOG(INFO) << "NWebHandlerDelegate::OnPrintReset";
}

CefSize NWebHandlerDelegate::GetPdfPaperSize(CefRefPtr<CefBrowser> browser,
                                             int device_units_per_inch) {
  LOG(INFO) << "NWebHandlerDelegate::GetPdfPaperSize";
  return CefSize();
}
#endif  // BUILDFLAG(ARKWEB_PRINT)
/* CefPrintHandler method end */

/* CefDisplayHandler method begin */
void NWebHandlerDelegate::OnTitleChange(CefRefPtr<CefBrowser> browser,
                                        const CefString& title) {
  if (nweb_handler_ != nullptr) {
    nweb_handler_->OnPageTitle(title.ToString());
  }
  return;
}

void NWebHandlerDelegate::OnLoadingProgressChange(CefRefPtr<CefBrowser> browser,
                                                  double progress) {
  constexpr int MAX_LOADING_PROGRESS = 100;
  int new_progress = static_cast<int>(progress * MAX_LOADING_PROGRESS);

  if (nweb_handler_ != nullptr) {
    nweb_handler_->OnLoadingProgress(new_progress);
  }

#if BUILDFLAG(ARKWEB_NWEB_EX)
  if (web_app_client_extension_listener_ != nullptr &&
      web_app_client_extension_listener_->OnLoadStarted != nullptr &&
      !on_load_start_notified_ && new_progress < MAX_LOADING_PROGRESS) {
    on_load_start_notified_ = true;
    web_app_client_extension_listener_->OnLoadStarted(
        (browser && browser->AsArkWebBrowser())
            ? browser->AsArkWebBrowser()->ShouldShowLoadingUI()
            : false,
        web_app_client_extension_listener_->nweb_id);
  }
  if (new_progress == MAX_LOADING_PROGRESS) {
    on_load_start_notified_ = false;
  }
#endif  // ARKWEB_NWEB_EX

  return;
}

bool NWebHandlerDelegate::OnOpenURLFromTab(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString& target_url,
    WindowOpenDisposition target_disposition,
    bool user_gesture) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  if (web_app_client_extension_listener_ != nullptr &&
      web_app_client_extension_listener_->OnOpenURLFromTab != nullptr) {
    web_app_client_extension_listener_->OnOpenURLFromTab(
        target_url.ToString(), static_cast<int>(target_disposition),
        user_gesture, web_app_client_extension_listener_->nweb_id);
    return true;
  }
#endif  // ARKWEB_NWEB_EX

  return false;
}

void NWebHandlerDelegate::ShowPasswordDialog(bool is_update,
                                             const CefString& url) {
#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
  if (web_app_client_extension_listener_ != nullptr &&
      web_app_client_extension_listener_->OnSaveOrUpdatePassword != nullptr) {
    web_app_client_extension_listener_->OnSaveOrUpdatePassword(
        is_update, url.ToString(), web_app_client_extension_listener_->nweb_id);
  }
#endif  // ARKWEB_EXT_PASSWORD
}

void NWebHandlerDelegate::OnShowAutofillPopup(
    CefRefPtr<CefBrowser> browser,
    const CefRect& bounds,
    bool right_aligned,
    const std::vector<CefAutofillPopupItem>& menu_items,
    bool is_password_popup_type) {
  float ratio = render_handler_->GetCefDeviceRatio();
  std::vector<std::string> label_list;
  std::vector<std::string> sublabel_list;
  for (auto& menu_item : menu_items) {
    label_list.push_back(CefString(&menu_item.label).ToString());
    sublabel_list.push_back(CefString(&menu_item.sublabel).ToString());
  }

#if BUILDFLAG(ARKWEB_DATALIST)
  if (!nweb_handler_) {
    return;
  }
  float scale = 1.0f;
  if (GetBrowser() && GetBrowser()->GetHost()) {
    scale = GetBrowser()->GetHost()->Scale();
  }
  if (!is_password_popup_type) {
    nweb_handler_->OnShowAutofillPopupV2(
        bounds.x * ratio, bounds.y * ratio + bounds.height * ratio,
        bounds.height * ratio, bounds.width, label_list);
    return;
  }
#endif

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
  if (!render_handler_) {
    return;
  }

  if (web_app_client_extension_listener_ != nullptr &&
      web_app_client_extension_listener_->OnShowPasswordAutofillPopup !=
          nullptr &&
      is_password_popup_type) {
    web_app_client_extension_listener_->OnShowPasswordAutofillPopup(
        bounds.x * ratio, bounds.y * ratio, bounds.width * ratio,
        bounds.height * ratio, right_aligned, label_list, sublabel_list,
        web_app_client_extension_listener_->nweb_id);
  }
#endif  // ARKWEB_EXT_PASSWORD
}

void NWebHandlerDelegate::OnHideAutofillPopup() {
#if BUILDFLAG(ARKWEB_DATALIST)
  if (!nweb_handler_) {
    return;
  }
  nweb_handler_->OnHideAutofillPopup();
#endif

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
  if (web_app_client_extension_listener_ != nullptr &&
      web_app_client_extension_listener_->OnHidePasswordAutofillPopup !=
          nullptr) {
    web_app_client_extension_listener_->OnHidePasswordAutofillPopup(
        web_app_client_extension_listener_->nweb_id);
  }
#endif  // ARKWEB_EXT_PASSWORD
}

#if BUILDFLAG(ARKWEB_ADBLOCK)
void NWebHandlerDelegate::OnAdsBlocked(
    CefRefPtr<CefBrowser> browser,
    const CefString& url,
    const std::map<CefString, CefString>& adsBlocked,
    bool is_site_first_report) {
  std::map<std::string, int32_t> adsBlocked_str;
  for (auto item : adsBlocked) {
    int32_t num = 0;
    base::StringToInt(item.second.ToString(), &num);
    adsBlocked_str.insert({item.first, num});
  }
  LOG(DEBUG) << "[adblock] OnAdsBlocked size: " << adsBlocked_str.size()
             << "  url = ***";

  if (web_app_client_extension_listener_ != nullptr &&
      web_app_client_extension_listener_->OnAdsBlocked != nullptr) {
    web_app_client_extension_listener_->OnAdsBlocked(
        url, adsBlocked_str, web_app_client_extension_listener_->nweb_id);
  }

  if (nweb_handler_ != nullptr) {
    std::vector<std::string> blocked;
    for (auto item : adsBlocked_str) {
      for (auto i = 0; i < item.second; i++) {
        blocked.push_back(item.first);
      }
    }
    nweb_handler_->OnAdsBlocked(url, blocked);
  }
}
#endif

// #if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
void NWebHandlerDelegate::OnTopControlsChanged(float top_controls_offset,
                                               float top_content_offset) {
#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
  if (web_app_client_extension_listener_ == nullptr ||
      web_app_client_extension_listener_->OnTopControlsChanged == nullptr) {
    return;
  }

  top_content_offset_ = top_content_offset;
  web_app_client_extension_listener_->OnTopControlsChanged(
      top_controls_offset, top_content_offset,
      web_app_client_extension_listener_->nweb_id);
#endif
}

NO_SANITIZE("cfi-icall") int NWebHandlerDelegate::OnGetTopControlsHeight() {
#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
  if (web_app_client_extension_listener_ == nullptr ||
      web_app_client_extension_listener_->OnGetTopControlsHeight == nullptr) {
    return 0;
  }

  return web_app_client_extension_listener_->OnGetTopControlsHeight(
      web_app_client_extension_listener_->nweb_id);
#else
  return 0;
#endif
}

bool NWebHandlerDelegate::DoBrowserControlsShrinkRendererSize() {
#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
  if (CefCommandLine::GetGlobalCommandLine()->HasSwitch(
          ::switches::kEnableNwebExTopControls) &&
      top_content_offset_ > 0) {
    return true;
  }
#endif

  return false;
}
// BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)

#if BUILDFLAG(ARKWEB_PULL_TO_REFRESH)
bool NWebHandlerDelegate::OnPullToRefreshAction(int action) {
  if (web_app_client_extension_listener_ == nullptr ||
      web_app_client_extension_listener_->OnPullToRefreshAction == nullptr) {
    return false;
  }

  return web_app_client_extension_listener_->OnPullToRefreshAction(
      action, web_app_client_extension_listener_->nweb_id);
}

void NWebHandlerDelegate::OnPullToRefreshPull(float offset_x, float offset_y) {
  if (web_app_client_extension_listener_ == nullptr ||
      web_app_client_extension_listener_->OnPullToRefreshPull == nullptr) {
    return;
  }

  web_app_client_extension_listener_->OnPullToRefreshPull(
      offset_x, offset_y, web_app_client_extension_listener_->nweb_id);
}
#endif

void NWebHandlerDelegate::OnReceivedIcon(const void* data,
                                         size_t width,
                                         size_t height,
                                         cef_color_type_t color_type,
                                         cef_alpha_type_t alpha_type) {
  if (!data) {
    LOG(ERROR) << "onReceivedIcon get error";
    return;
  }
  if (nweb_handler_ != nullptr) {
    nweb_handler_->OnPageIcon(data, width, height,
                              TransformColorType(color_type),
                              TransformAlphaType(alpha_type));
    SetFavicon(data, width, height, TransformColorType(color_type),
               TransformAlphaType(alpha_type));
  }
}

#if BUILDFLAG(ARKWEB_BFCACHE)
void NWebHandlerDelegate::UpdateFavicon(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();

  if (!browser || !browser->GetHost() ||
      !browser->GetHost()->GetVisibleNavigationEntry()) {
    return;
  }
  LOG(INFO) << "[Favicon] nweb_handler delegate start to update favicon.";

  void* data = nullptr;
  int color_type;
  int alpha_type;
  int width;
  int height;
  browser->GetHost()->GetVisibleNavigationEntry()->GetFavicon(
      &data, color_type, alpha_type, width, height);
  SetFavicon(data, width, height, ImageColorType(color_type),
             ImageAlphaType(alpha_type));
}
#endif  // BUILDFLAG(ARKWEB_BFCACHE)

void NWebHandlerDelegate::SetFavicon(const void* data,
                                     size_t width,
                                     size_t height,
                                     ImageColorType color_type,
                                     ImageAlphaType alpha_type) {
  base::AutoLock lock_scope(state_lock_);
  data_ = data;
  width_ = width;
  height_ = height;
  color_type_ = color_type;
  alpha_type_ = alpha_type;
}

bool NWebHandlerDelegate::GetFavicon(const void** data,
                                     size_t& width,
                                     size_t& height,
                                     ImageColorType& color_type,
                                     ImageAlphaType& alpha_type) {
  base::AutoLock lock_scope(state_lock_);
  if (data == nullptr) {
    LOG(ERROR) << "data is null";
    return false;
  }

  if (data_ == nullptr) {
    LOG(ERROR) << "data_ is null";
    return false;
  }
  *data = data_;
  width = width_;
  height = height_;
  color_type = color_type_;
  alpha_type = alpha_type_;
  return true;
}

void NWebHandlerDelegate::OnReceivedIconUrl(const CefString& image_url,
                                            const void* data,
                                            size_t width,
                                            size_t height,
                                            cef_color_type_t color_type,
                                            cef_alpha_type_t alpha_type) {
  if (!data) {
    LOG(ERROR) << "OnReceivedIconUrl get error";
    return;
  }

  if (web_app_client_extension_listener_ == nullptr ||
      web_app_client_extension_listener_->OnReceivedFaviconUrl == nullptr) {
    return;
  }

  char* c_image_url = CopyCefStringToChar(image_url);
  web_app_client_extension_listener_->OnReceivedFaviconUrl(
      c_image_url, width, height,
      TransformColorTypeToInt(TransformColorType(color_type)),
      TransformAlphaTypeToInt(TransformAlphaType(alpha_type)),
      web_app_client_extension_listener_->nweb_id);
  if (c_image_url) {
    delete[] c_image_url;
  }
}

#if BUILDFLAG(ARKWEB_NAVIGATION)
void NWebHandlerDelegate::OnTouchIconUrlWithSizesReceived(const CefString& image_url,
                                                          bool precomposed,
                                                          const std::vector<IconSize>& sizes)
{
  if (!web_app_client_extension_listener_ ||
      !web_app_client_extension_listener_->OnTouchIconUrlWithSizesReceived) {
    return;
  }

  char* c_image_url = CopyCefStringToChar(image_url);

  web_app_client_extension_listener_->OnTouchIconUrlWithSizesReceived(
      c_image_url, precomposed, sizes.data(), sizes.size(),
      web_app_client_extension_listener_->nweb_id);
  delete[] c_image_url;
}
#endif

void NWebHandlerDelegate::OnReceivedTouchIconUrl(CefRefPtr<CefBrowser> browser,
                                                 const CefString& icon_url,
                                                 bool precomposed) {
  if (nweb_handler_ != nullptr) {
    nweb_handler_->OnDesktopIconUrl(icon_url.ToString(), precomposed);
  }
  return;
}

bool NWebHandlerDelegate::OnConsoleMessage(CefRefPtr<CefBrowser> browser,
                                           cef_log_severity_t level,
                                           const CefString& message,
                                           const CefString& source,
                                           int line) {
  if (nweb_handler_ != nullptr) {
    NWebConsoleLog::NWebConsoleLogLevel message_level =
        ConvertConsoleMessageLevel(level);
    std::shared_ptr<NWebConsoleLog> console_log =
        std::make_shared<NWebConsoleLogImpl>(line, message.ToString(),
                                             message_level, source.ToString());
    return nweb_handler_->OnConsoleLog(console_log);
  }
  return false;
}

void NWebHandlerDelegate::OnScaleChanged(CefRefPtr<CefBrowser> browser,
                                         float old_page_scale_factor,
                                         float new_page_scale_factor) {
  if (!render_handler_) {
    LOG(ERROR) << "render handler is nullptr";
    return;
  }
  if (nweb_handler_ != nullptr) {
    LOG(INFO) << "OnScaleChanged new scale: " << new_page_scale_factor
              << " old scale: " << old_page_scale_factor;
    nweb_handler_->OnScaleChanged(old_page_scale_factor, new_page_scale_factor);
  }
#if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)
  scale_ = new_page_scale_factor;
#endif  // #if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)
}

#if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)
void NWebHandlerDelegate::OnScaleInited(CefRefPtr<CefBrowser> browser,
                                        float page_scale_factor) {
  if (!render_handler_) {
    LOG(ERROR) << "render handler is nullptr";
    return;
  }
  LOG(INFO) << "OnScaleInited scale: " << page_scale_factor;
  scale_ = page_scale_factor;
}
#endif  // BUILDFLAG(ARKWEB_PAGE_UP_DOWN)

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
bool NWebHandlerDelegate::OnCursorChange(
    CefRefPtr<CefBrowser> browser,
    CefCursorHandle cursor,
    cef_cursor_type_t type,
    const CefCursorInfo& custom_cursor_info) {
  LOG(DEBUG) << "OnCursorChange type: " << type;
  if (nweb_handler_ == nullptr) {
    LOG(ERROR) << "OnCursorChange nweb handler is nullptr";
    return false;
  }
  if (type < 0 || type >= static_cast<int32_t>(CursorType::CT_MAX_VALUE)) {
    LOG(ERROR) << "OnCursorChange type exception";
    return false;
  }

  CursorType cursorType(static_cast<CursorType>(type));
  if (type == CT_CUSTOM && custom_cursor_info.size.width > 0 &&
      custom_cursor_info.size.height > 0) {
    uint64_t len =
        custom_cursor_info.size.width * custom_cursor_info.size.height * 4;
    std::unique_ptr<uint8_t[]> buff = std::make_unique<uint8_t[]>(len);
    if (!buff) {
      LOG(ERROR) << "OnCursorChange make_unique failed";
      return false;
    }
    if (memcpy_s((char*)buff.get(), len, custom_cursor_info.buffer, len) !=
        EOK) {
      LOG(ERROR) << "OnCursorChange memcpy_s failed";
      return false;
    }
    std::shared_ptr<NWebCursorInfo> info = std::make_shared<NWebCursorInfoImpl>(
        custom_cursor_info.hotspot.x, custom_cursor_info.hotspot.y,
        custom_cursor_info.image_scale_factor, custom_cursor_info.size.width,
        custom_cursor_info.size.height, buff.get());
    return nweb_handler_->OnCursorChange(cursorType, info);
  }

  return nweb_handler_->OnCursorChange(cursorType,
                                       std::make_shared<NWebCursorInfoImpl>());
}

bool NWebHandlerDelegate::GetContinueNeedFocus() {
  return continueNeedFocus_;
}

void NWebHandlerDelegate::SetContinueNeedFocus(bool continueNeedFocus) {
  continueNeedFocus_ = continueNeedFocus;
}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

void NWebHandlerDelegate::OnContentsBrowserZoomChange(double zoom_factor,
                                                      bool can_show_bubble) {
#if BUILDFLAG(ARKWEB_EXT_GET_ZOOM_LEVEL)
  if (web_app_client_extension_listener_ != nullptr &&
      web_app_client_extension_listener_->ContentsBrowserZoomChange !=
          nullptr) {
    web_app_client_extension_listener_->ContentsBrowserZoomChange(
        zoom_factor, can_show_bubble,
        web_app_client_extension_listener_->nweb_id);
  }
#endif
}
/* CefDisplayHandler method end */

/* CefFocusHandler method begin */
bool NWebHandlerDelegate::OnSetFocus(CefRefPtr<CefBrowser> browser,
                                     FocusSource source) {
  if (nweb_handler_ != nullptr) {
#if BUILDFLAG(ARKWEB_FOCUS)
    if (!nweb_handler_->OnFocus(static_cast<NWebFocusSource>(source))) {
      return true;
    }
    LOG(INFO) << "nweb_handler request focus successful, source = " << source
              << ", nweb_id = " << nweb_id_;
    focusState_ = true;
#endif  // ARKWEB_FOCUS
  }
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  if (event_handler_ != nullptr) {
    event_handler_->SetIsFocus(true);
  }
  if (render_handler_ != nullptr) {
    render_handler_->SetFocusStatus(true);
  }
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
  return false;
}
/* CefFocusHandler method end */

/* CefFormHandler method begin */
#if BUILDFLAG(ARKWEB_ACTIVITY_STATE)
void NWebHandlerDelegate::OnFormEditingStateChanged(
    CefRefPtr<CefBrowser> browser,
    bool is_editing,
    uint64_t form_id) {
  bool form_editing_state_ = edited_forms_id_.size();
  std::vector<uint64_t>::iterator it =
      find(edited_forms_id_.begin(), edited_forms_id_.end(), form_id);

  if (it == edited_forms_id_.end() && is_editing) {
    edited_forms_id_.push_back(form_id);
  } else if (it != edited_forms_id_.end() && !is_editing) {
    edited_forms_id_.erase(it);
  } else {
    return;
  }

  bool current_is_editing_ = edited_forms_id_.size() != 0;
  if (current_is_editing_ != form_editing_state_) {
    LOG(INFO) << "NWebHandlerDelegate::OnFormEditingStateChanged, is_editing: "
              << is_editing << " nweb_id: " << nweb_id_;
  }

  if (nweb_handler_ != nullptr) {
    FormState state =
        is_editing ? FormState::kHadInteraction : FormState::kNoInteraction;
    nweb_handler_->OnActivityStateChanged(static_cast<int>(state),
                                          ActivityType::FORM);
  }

#if BUILDFLAG(ARKWEB_NWEB_EX)
  if (web_app_client_extension_listener_ != nullptr &&
      web_app_client_extension_listener_->OnActivityStateChanged != nullptr) {
    web_app_client_extension_listener_->OnActivityStateChanged(
        is_editing ? static_cast<int>(FormState::kHadInteraction)
                   : static_cast<int>(FormState::kNoInteraction),
        static_cast<int>(ActivityType::FORM),
        web_app_client_extension_listener_->nweb_id);
  }
#endif  // BUILDFLAG(ARKWEB_NWEB_EX)

  return;
}
#endif
/* CefFormHandler method end */

/* CefPermissionRequest method begin */
void NWebHandlerDelegate::OnGeolocationShow(const CefString& origin) {
  if (nweb_handler_ != nullptr) {
    // lifecycle wrapped by ace WebGeolocationOhos
    if (callback_ == nullptr) {
      callback_ = std::make_shared<NWebGeolocationCallback>(main_browser_);
    }
    nweb_handler_->OnGeolocationShow(origin, callback_);
  }
  return;
}

void NWebHandlerDelegate::OnGeolocationHide() {
  if (nweb_handler_ != nullptr) {
    nweb_handler_->OnGeolocationHide();
  }
  return;
}

void NWebHandlerDelegate::OnPermissionRequest(
    CefRefPtr<CefAccessRequest> request) {
#if BUILDFLAG(ARKWEB_EXT_PERMISSION)
  if ((*base::CommandLine::ForCurrentProcess())
          .HasSwitch(switches::kEnableNwebExPermission) &&
      web_app_client_extension_listener_ != nullptr &&
      web_app_client_extension_listener_->OnPermissionRequest != nullptr) {
    if (request->ResourceAcessId() ==
            NWebAccessRequest::Resources::CLIPBOARD_READ_WRITE ||
        request->ResourceAcessId() ==
            NWebAccessRequest::Resources::CLIPBOARD_SANITIZED_WRITE ||
        request->ResourceAcessId() ==
            NWebAccessRequest::Resources::PROTECTED_MEDIA_ID
#if BUILDFLAG(ARKWEB_NOTIFICATION)
        || request->ResourceAcessId() ==
            NWebAccessRequest::Resources::NOTIFICATION
#endif // ARKWEB_NOTIFICATION
        ) {
      std::shared_ptr<NWebAccessRequest> access_request =
          std::make_shared<NWebAccessRequestDelegate>(request);
      std::shared_ptr<NWebPermissionRequest> nweb_request =
          std::make_shared<NWebPermissionRequest>(
              web_app_client_extension_listener_->nweb_id, access_request);
      web_app_client_extension_listener_->OnPermissionRequest(
          web_app_client_extension_listener_->nweb_id, nweb_request);
      return;
    }
  }
#endif  // BUILDFLAG(ARKWEB_EXT_PERMISSION)

  if (nweb_handler_ != nullptr) {
    std::shared_ptr<NWebAccessRequest> access_request =
        std::make_shared<NWebAccessRequestDelegate>(request);
    nweb_handler_->OnPermissionRequest(access_request);
  }
  return;
}

void NWebHandlerDelegate::OnPermissionRequestCanceled(
    CefRefPtr<CefAccessRequest> request) {
  if (nweb_handler_ != nullptr) {
    std::shared_ptr<NWebAccessRequest> access_request =
        std::make_shared<NWebAccessRequestDelegate>(request);
    nweb_handler_->OnPermissionRequestCanceled(access_request);
  }
  return;
}

void NWebHandlerDelegate::OnScreenCaptureRequest(
    CefRefPtr<CefScreenCaptureAccessRequest> request) {
#if BUILDFLAG(ARKWEB_WEBRTC)
  LOG(INFO) << "NWebHandlerDelegate::OnScreenCaptureRequest origin: "
            << request->Origin().ToString();
  if (nweb_handler_ != nullptr) {
    std::shared_ptr<NWebScreenCaptureAccessRequest> access_request =
        std::make_shared<NWebScreenCaptureAccessRequestDelegate>(request);
    nweb_handler_->OnScreenCaptureRequest(access_request);
  }
#endif  // BUILDFLAG(ARKWEB_WEBRTC)
}
/* CefPermissionRequest method begin */

/* CefJSDialogHandler method begin */
bool NWebHandlerDelegate::OnJSDialog(CefRefPtr<CefBrowser> browser,
                                     const CefString& origin_url,
                                     JSDialogType dialog_type,
                                     const CefString& message_text,
                                     const CefString& default_prompt_text,
                                     CefRefPtr<CefJSDialogCallback> callback,
                                     bool& suppress_message) {
  if (nweb_handler_ == nullptr) {
    return false;
  }
  suppress_message = false;
  std::shared_ptr<NWebJSDialogResult> js_result =
      std::make_shared<NWebJSDialogResultImpl>(callback);
  switch (dialog_type) {
    case JSDIALOGTYPE_ALERT:
      return nweb_handler_->OnAlertDialogByJS(origin_url, message_text,
                                              js_result);
    case JSDIALOGTYPE_CONFIRM:
      return nweb_handler_->OnConfirmDialogByJS(origin_url, message_text,
                                                js_result);
    case JSDIALOGTYPE_PROMPT:
      return nweb_handler_->OnPromptDialogByJS(origin_url, message_text,
                                               default_prompt_text, js_result);
    default:
      break;
  }
  return false;
}

bool NWebHandlerDelegate::OnBeforeUnloadDialog(
    CefRefPtr<CefBrowser> browser,
    const CefString& message_text,
    bool is_reload,
    CefRefPtr<CefJSDialogCallback> callback) {
  if (nweb_handler_ == nullptr) {
    return false;
  }
  std::shared_ptr<NWebJSDialogResult> js_result =
      std::make_shared<NWebJSDialogResultImpl>(callback);
  std::string url = "";
  if (browser && browser->GetMainFrame()) {
    url = browser->GetMainFrame()->GetURL().ToString();
  }
  bool flag = nweb_handler_->OnBeforeUnloadByJSV2(url, message_text, is_reload, js_result);
  LOG(INFO) << "NWebHandlerDelegate::OnBeforeUnloadDialog nweb_handler_->OnBeforeUnloadByJSV2() called!";
  if (ArkWebGetErrno() != ArkWebInterfaceResult::RESULT_OK) {
    LOG(INFO) << "NWebHandlerDelegate::OnBeforeUnloadDialog V2() failed, OnBeforeUnloadByJS() called!";
    flag = nweb_handler_->OnBeforeUnloadByJS(url, message_text, js_result);
  }
  return flag;
}

/* CefJSDialogHandler method end */

/* CefDialogHandler method begin */
bool NWebHandlerDelegate::OnFileDialog(
    CefRefPtr<CefBrowser> browser,
    FileDialogMode mode,
    const CefString& title,
    const CefString& default_file_path,
    const std::vector<CefString>& accept_filters,
    const std::vector<CefString>& accept_extensions,
    const std::vector<CefString>& accept_descriptions,
    bool capture,
    const std::vector<CefString>& mime_filters,
    CefRefPtr<CefFileDialogCallback> callback) {
  if (nweb_handler_ == nullptr) {
    return false;
  }
  std::string file_selector_title = title.ToString();
  NWebFileSelectorParams::FileSelectorMode file_mode =
      ConvertFileSelectorMode(mode);
  if (title.ToString().size() == 0) {
    switch (file_mode) {
      case NWebFileSelectorParams::FileSelectorMode::FILE_OPEN_MODE:
        file_selector_title = "open file";
        break;
      case NWebFileSelectorParams::FileSelectorMode::FILE_OPEN_MULTIPLE_MODE:
        file_selector_title = "open files";
        break;
      case NWebFileSelectorParams::FileSelectorMode::FILE_OPEN_FOLDER_MODE:
        file_selector_title = "open file folder";
        break;
      case NWebFileSelectorParams::FileSelectorMode::FILE_SAVE_MODE:
        file_selector_title = "save as";
        break;
      default:
        break;
    }
  }
  std::shared_ptr<NWebFileSelectorParams> param =
      std::make_shared<FileSelectorParamsImpl>(
          file_mode, file_selector_title, accept_extensions,
          default_file_path.ToString(), capture, mime_filters);
  std::shared_ptr<NWebStringVectorValueCallback> file_path_callback =
      std::make_shared<FileSelectorCallbackImpl>(callback);
  return nweb_handler_->OnFileSelectorShow(file_path_callback, param);
}

#if BUILDFLAG(ARKWEB_HTML_SELECT)
void NWebHandlerDelegate::OnSelectPopupMenu(
    CefRefPtr<CefBrowser> browser,
    const CefRect& bounds,
    int item_height,
    double item_font_size,
    int selected_item,
    const std::vector<CefSelectPopupItem>& menu_items,
    bool right_aligned,
    bool allow_multiple_selection,
    CefRefPtr<CefSelectPopupCallback> callback) {
  if (!nweb_handler_ || !render_handler_) {
    return;
  }
  float ratio = render_handler_->GetCefDeviceRatio();
  std::shared_ptr<NWebSelectPopupMenuParamImpl> param =
      std::make_shared<NWebSelectPopupMenuParamImpl>();
  if (!param) {
    return;
  }
  std::shared_ptr<NWebSelectMenuBound> bound =
      std::make_shared<NWebSelectMenuBoundImpl>(
          bounds.x * ratio, bounds.y * ratio, bounds.width * ratio,
          bounds.height * ratio);
  param->SetSelectMenuBound(bound);
  param->SetItemHeight(item_height);
  param->SetSelectedItem(selected_item);
  param->SetIsRightAligned(right_aligned);
  param->SetItemFontSize(item_font_size * GetScale() / 100.0);
  param->SetIsAllowMultipleSelection(allow_multiple_selection);
  std::vector<std::shared_ptr<NWebSelectPopupMenuItem>> menu_list;
  for (auto& menu_item : menu_items) {
    std::string label = CefString(&menu_item.label);
    std::shared_ptr<NWebSelectPopupMenuItemImpl> item =
        std::make_shared<NWebSelectPopupMenuItemImpl>();
    item->SetAction(menu_item.action);
    item->SetIsEnabled(menu_item.enabled);
    item->SetIsChecked(menu_item.checked);
    item->SetLabel(CefString(&menu_item.label).ToString());
    item->SetToolTip(CefString(&menu_item.tool_tip).ToString());
    item->SetType(static_cast<SelectPopupMenuItemType>(menu_item.type));
    item->SetHasTextDirectionOverride(menu_item.has_text_direction_override);
    item->SetTextDirection(
        static_cast<TextDirection>(menu_item.text_direction));
    menu_list.push_back(std::move(item));
  }
  param->SetMenuItems(std::move(menu_list));

  std::shared_ptr<NWebSelectPopupMenuCallback> popup_callback =
      std::make_shared<NWebSelectPopupMenuCallbackImpl>(callback);
  nweb_handler_->OnSelectPopupMenu(param, popup_callback);
}
#endif  // #if BUILDFLAG(ARKWEB_HTML_SELECT)

#if BUILDFLAG(ARKWEB_CSS_INPUT_TIME)
void NWebHandlerDelegate::OnDateTimeChooserPopup(
    CefRefPtr<CefBrowser> browser,
    const CefDateTimeChooser& date_time_chooser,
    const std::vector<CefDateTimeSuggestion>& suggestion,
    CefRefPtr<CefDateTimeChooserCallback> callback) {
  if (!browser || !callback || !nweb_handler_) {
    return;
  }
  if (date_time_chooser.minimum >= date_time_chooser.maximum) {
    LOG(WARNING) << "date time chooser minimum > maxinum, is invald";
    callback->Continue(false, 0);
    return;
  }
  DateTimeChooserType type =
      ConvertDateTimeChooserType(date_time_chooser.dialog_type);
  DateTime selected =
      (type == DateTimeChooserType::DTC_MONTH)
          ? ConvertMonthToDateTime(date_time_chooser.dialog_value)
          : ConvertMsToDateTime(date_time_chooser.dialog_value);
  DateTime minimum = (type == DateTimeChooserType::DTC_MONTH)
                         ? ConvertMonthToDateTime(date_time_chooser.minimum)
                         : ConvertMsToDateTime(date_time_chooser.minimum);
  DateTime maximum = (type == DateTimeChooserType::DTC_MONTH)
                         ? ConvertMonthToDateTime(date_time_chooser.maximum)
                         : ConvertMsToDateTime(date_time_chooser.maximum);
  std::shared_ptr<NWebDateTimeChooserImpl> chooser =
      std::make_shared<NWebDateTimeChooserImpl>(date_time_chooser.step, minimum,
                                                maximum, selected, type);
  std::shared_ptr<NWebDateTimeChooserCallback> chooser_callback =
      std::make_shared<NWebDateTimeChooserCallbackImpl>(type, callback);
  if (!chooser_callback) {
    callback->Continue(false, 0);
    return;
  }
  chooser->SetHasSelected(!std::isnan(date_time_chooser.dialog_value));
  std::vector<std::shared_ptr<NWebDateTimeSuggestion>> suggestions;
  for (size_t index = 0; index < suggestion.size(); index++) {
    DateTime value = (type == DateTimeChooserType::DTC_MONTH)
                         ? ConvertMonthToDateTime(suggestion[index].value)
                         : ConvertMsToDateTime(suggestion[index].value);
    suggestions.push_back(std::make_shared<NWebDateTimeSuggestionImpl>(
        value, CefString(&suggestion[index].label).ToString(),
        CefString(&suggestion[index].localized_value).ToString()));
    if (date_time_chooser.dialog_value == suggestion[index].value) {
      chooser->SetSuggestionIndex(index);
    }
  }
  nweb_handler_->OnDateTimeChooserPopup(chooser, suggestions, chooser_callback);
}

void NWebHandlerDelegate::OnDateTimeChooserClose() {
  if (!nweb_handler_) {
    return;
  }

  nweb_handler_->OnDateTimeChooserClose();
}
#endif  // #if BUILDFLAG(ARKWEB_CSS_INPUT_TIME)
/* CefDialogHandler method end */

/* CefContextMenuHandler method begin */
void NWebHandlerDelegate::OnBeforeContextMenu(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefContextMenuParams> params,
    CefRefPtr<CefMenuModel> model) {
  LOG(INFO) << "NWebHandlerDelegate::OnBeforeContextMenu";
}

void NWebHandlerDelegate::CopyImageToClipboard(CefRefPtr<CefImage> image) {
#if BUILDFLAG(ARKWEB_CLIPBOARD)
  if (image != nullptr && image->GetWidth() > 0 && image->GetHeight() > 0) {
    int pixel_width = 0;
    int pixel_height = 0;
    CefRefPtr<CefBinaryValue> bitMap = image->GetAsBitmap(
        1, CEF_COLOR_TYPE_RGBA_8888, CEF_ALPHA_TYPE_PREMULTIPLIED, pixel_width,
        pixel_height);
    size_t bitMapSize = bitMap->GetSize();
    uint8_t* data = (uint8_t*)calloc((size_t)bitMapSize, sizeof(uint8_t));
    if (data == nullptr) {
      LOG(ERROR) << "calloc bitmap failed";
      return;
    }
    bitMap->GetData((void*)data, bitMapSize, 0);

    std::shared_ptr<ClipBoardImageDataAdapterImpl> imageInfo =
        std::make_shared<ClipBoardImageDataAdapterImpl>();
    if (!imageInfo) {
      LOG(ERROR) << "new ClipBoardImageDataAdapterImpl failed";
      return;
    }

    imageInfo->SetColorType(ClipBoardImageColorType::COLOR_TYPE_RGBA_8888);
    imageInfo->SetAlphaType(ClipBoardImageAlphaType::ALPHA_TYPE_PREMULTIPLIED);
    imageInfo->SetData((uint32_t*)data);
    imageInfo->SetDataSize(bitMapSize);
    imageInfo->SetWidth(pixel_width);
    imageInfo->SetHeight(pixel_height);

    std::shared_ptr<PasteDataRecordAdapter> imgRecord =
        PasteDataRecordAdapter::NewRecord("pixelMap");
    PasteRecordVector recordVector;
    if (imgRecord->SetImgData(imageInfo)) {
      recordVector.push_back(imgRecord);
      LOG(INFO) << "set img to record success";
    } else {
      LOG(ERROR) << "set img to record failed";
      free(data);
      return;
    }

    auto copy_option =
        static_cast<CopyOptionMode>(preference_delegate_->GetCopyOptionMode());
    base::ThreadPool::PostTask(
        FROM_HERE,
        base::BindOnce(
            [](const PasteRecordVector& record_vector,
               CopyOptionMode copy_option, uint8_t* data) {
              OhosAdapterHelper::GetInstance().GetPasteBoard().SetPasteData(
                  record_vector, copy_option);
              LOG(INFO) << "copy image to clipboard success";
              free(data);
            },
            recordVector, copy_option, data));
  }
#endif
}

void NWebHandlerDelegate::OnGetImageForContextNode(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefImage> image,
    int command_id) {
  LOG(INFO) << "NWebHandlerDelegate::OnGetImageForContextNode";
#if BUILDFLAG(ARKWEB_NWEB_EX)
  if (command_id == MENU_ID_IMAGE_SHARE &&
      base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableNwebEx)) {
    if (image != nullptr && image->GetWidth() > 0 && image->GetHeight() > 0) {
      OnGetImageData(image);
    } else {
      LOG(WARNING) << "onGetImageFromContextNode for share image is invalid";
      if (browser && browser->GetHost()) {
        browser->GetHost()->GetImageFromCacheEx(image_cache_src_url_,
                                                command_id);
      }
    }
    return;
  }
#endif

  if (image != nullptr && image->GetWidth() > 0 && image->GetHeight() > 0) {
    if (command_id == MENU_ID_IMAGE_COPY) {
      CopyImageToClipboard(image);
    }
  } else {
    LOG(WARNING) << "OnGetImageForContextNode image is invalid";
    if (browser && browser->GetHost()) {
      browser->GetHost()->GetImageFromCache(image_cache_src_url_, command_id);
    }
  }
}

void NWebHandlerDelegate::OnGetImageFromCache(CefRefPtr<CefImage> image,
                                              int command_id) {
  if (image != nullptr && image->GetWidth() > 0 && image->GetHeight() > 0) {
    if (command_id == MENU_ID_IMAGE_COPY) {
      CopyImageToClipboard(image);
    }
  } else {
    LOG(WARNING) << "OnGetImageFromCache image is invalid";
  }
}

#if BUILDFLAG(ARKWEB_NWEB_EX)
void NWebHandlerDelegate::OnGetImageFromCacheEx(int command_id,
                                                uint8_t* data,
                                                int buffer_size) {
  if (command_id == MENU_ID_IMAGE_SHARE) {
    OnGetImageForShare(data, buffer_size);
    return;
  }
  if (command_id == MENU_ID_FEED_SHARE) {
    if (web_extension_listener_ != nullptr &&
        web_extension_listener_->OnGetImageFromCache != nullptr) {
      web_extension_listener_->OnGetImageFromCache(
          data, buffer_size, web_extension_listener_->nweb_id);
    }
  }
}

void NWebHandlerDelegate::OnGetImageForShare(uint8_t* data, int buffer_size) {
  if (web_extension_listener_ != nullptr &&
      web_extension_listener_->OnGetImageFromContextNode != nullptr) {
    web_extension_listener_->OnGetImageFromContextNode(
        data, buffer_size, web_extension_listener_->nweb_id);
  }
}

void NWebHandlerDelegate::OnGetImageData(CefRefPtr<CefImage> image) {
  if (image != nullptr && image->GetWidth() > 0 && image->GetHeight() > 0) {
    int pixel_width = 0;
    int pixel_height = 0;
    CefRefPtr<CefBinaryValue> bitMap =
        image->GetAsJPEG(1, 100, pixel_width, pixel_height);  // quality set 100 percents
    size_t bitMapSize = bitMap->GetSize();
    void* data = calloc((size_t)bitMapSize, sizeof(uint8_t));
    if (data == nullptr) {
      LOG(ERROR) << "calloc bitmap failed";
      return;
    }
    bitMap->GetData(data, bitMapSize, 0);
    if (web_extension_listener_ != nullptr &&
        web_extension_listener_->OnGetImageFromContextNode != nullptr) {
      web_extension_listener_->OnGetImageFromContextNode(
          (uint8_t*)data, bitMapSize, web_extension_listener_->nweb_id);
    }
    free(data);
  } else {
    LOG(WARNING) << "OnGetImageData image is invalid";
    if (web_extension_listener_ != nullptr &&
        web_extension_listener_->OnGetImageFromContextNode != nullptr) {
      web_extension_listener_->OnGetImageFromContextNode(
          nullptr, 0, web_extension_listener_->nweb_id);
    }
  }
}
#endif

bool NWebHandlerDelegate::RunContextMenu(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefContextMenuParams> params,
    CefRefPtr<CefMenuModel> model,
    CefRefPtr<CefRunContextMenuCallback> callback) {
  if (!nweb_handler_ || !render_handler_) {
    return false;
  }
  int32_t view_port_height = 0;
#if defined(OHOS_EX_TOPCONTROLS)
  if (browser && browser->GetHost()) {
    view_port_height = browser->GetHost()->GetShrinkViewportHeight();
  }
#endif
  LOG(INFO) << "NWebHandlerDelegate RunContextMenu view_port_height:"
            << view_port_height;
  std::shared_ptr<NWebContextMenuParams> nweb_param =
      std::make_shared<NWebContextMenuParamsImpl>(
          params, render_handler_->GetVirtualPixelRatio(), view_port_height);
  std::shared_ptr<NWebContextMenuCallback> nweb_callback =
      std::make_shared<NWebContextMenuCallbackImpl>(callback);
  if (input_method_client_) {
    bool has_composition = input_method_client_->HasComposition();
    LOG(INFO) << "NWebHandlerDelegate has_composition " << has_composition;
    if (has_composition) {
      LOG(INFO) << "NWebHandlerDelegate input has composition";
      if (nweb_callback) {
        nweb_callback->Cancel();
      }
      return true;
    }
  }

  image_cache_src_url_ = params->GetSourceUrl();
  if (nweb_handler_->RunContextMenu(nweb_param, nweb_callback)) {
    return true;
  }
  if (nweb_callback) {
    nweb_callback->Cancel();
  }
  return false;
}

bool NWebHandlerDelegate::OnContextMenuCommand(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefContextMenuParams> params,
    int command_id,
    CefContextMenuHandler::EventFlags event_flags) {
  if ((command_id == MENU_ID_IMAGE_COPY) && (browser != nullptr) &&
      (browser->GetHost() != nullptr)) {
    image_cache_src_url_ = params->GetSourceUrl();
    browser->GetHost()->GetImageForContextNode(MENU_ID_IMAGE_COPY);
    return true;
  }
  return false;
}

void NWebHandlerDelegate::OnContextMenuDismissed(CefRefPtr<CefBrowser> browser,
                                                 CefRefPtr<CefFrame> frame) {
  if (nweb_handler_ != nullptr) {
    nweb_handler_->OnContextMenuDismissed();
  }
}

#if BUILDFLAG(ARKWEB_MENU)
bool NWebHandlerDelegate::UpdateClippedSelectionBounds(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefRect& select_bounds) {
  if (nweb_handler_ == nullptr || render_handler_ == nullptr) {
    return false;
  }
  nweb_handler_->UpdateClippedSelectionBounds(select_bounds.x, select_bounds.y,
                                              select_bounds.width,
                                              select_bounds.height);
  return true;
}
#endif

bool NWebHandlerDelegate::RunQuickMenu(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefPoint& location,
    const CefSize& size,
    const CefRect& select_bounds,
    CefContextMenuHandler::QuickMenuEditStateFlags edit_state_flags,
    CefRefPtr<CefRunQuickMenuCallback> callback,
    bool is_mouse_trigger,
    bool is_long_press_actived) {
  if (nweb_handler_ == nullptr || render_handler_ == nullptr) {
    return false;
  }

  LOG(INFO) << "NWebHandlerDelegate RunQuickMenu ";
  if (input_method_client_) {
    bool has_composition = input_method_client_->HasComposition();
    LOG(INFO) << "NWebHandlerDelegate has_composition " << has_composition;
    if (has_composition) {
      LOG(INFO) << "NWebHandlerDelegate input has composition";
      return false;
    }
  }

#if BUILDFLAG(ARKWEB_MENU)
  LOG(INFO) << "RunQuickMenu is_rich_text_:" << is_rich_text_
            << ", is_mouse_trigger:" << is_mouse_trigger;
#endif
  std::shared_ptr<NWebTouchHandleState> insert_touch_handle;
  std::shared_ptr<NWebTouchHandleState> begin_touch_handle;
  std::shared_ptr<NWebTouchHandleState> end_touch_handle;
  if (is_mouse_trigger) {
#if BUILDFLAG(ARKWEB_MENU)
    if (!is_rich_text_) {
      return false;
    }
#endif
    insert_touch_handle = nullptr;
    begin_touch_handle = render_handler_->GetDefalutTouchHandleState(
        NWebTouchHandleState::TouchHandleType::SELECTION_BEGIN_HANDLE);
    end_touch_handle = render_handler_->GetDefalutTouchHandleState(
        NWebTouchHandleState::TouchHandleType::SELECTION_END_HANDLE);
  } else {
    insert_touch_handle = render_handler_->GetTouchHandleState(
        NWebTouchHandleState::TouchHandleType::INSERT_HANDLE);
    begin_touch_handle = render_handler_->GetTouchHandleState(
        NWebTouchHandleState::TouchHandleType::SELECTION_BEGIN_HANDLE);
    end_touch_handle = render_handler_->GetTouchHandleState(
        NWebTouchHandleState::TouchHandleType::SELECTION_END_HANDLE);
  }
  std::shared_ptr<NWebQuickMenuCallback> nweb_callback =
      std::make_shared<NWebQuickMenuCallbackImpl>(callback);
  std::shared_ptr<NWebQuickMenuParamsImpl> nweb_param =
      std::make_shared<NWebQuickMenuParamsImpl>(
          location.x, location.y, size.width, size.height, edit_state_flags,
          select_bounds.x, select_bounds.y, select_bounds.width,
          select_bounds.height);
  nweb_param->SetTouchHandleState(
      insert_touch_handle,
      NWebTouchHandleState::TouchHandleType::INSERT_HANDLE);
  nweb_param->SetTouchHandleState(
      begin_touch_handle,
      NWebTouchHandleState::TouchHandleType::SELECTION_BEGIN_HANDLE);
  nweb_param->SetTouchHandleState(
      end_touch_handle,
      NWebTouchHandleState::TouchHandleType::SELECTION_END_HANDLE);
  nweb_param->SetIsMouseTrigger(is_mouse_trigger);
  nweb_param->SetIsLongPressActived(is_long_press_actived);
  return nweb_handler_->RunQuickMenu(nweb_param, nweb_callback);
}

bool NWebHandlerDelegate::OnQuickMenuCommand(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    int command_id,
    CefContextMenuHandler::EventFlags event_flags) {
  // TODO: Execute commands such as copy paste etc.
  return false;
}

void NWebHandlerDelegate::OnQuickMenuDismissed(CefRefPtr<CefBrowser> browser,
                                               CefRefPtr<CefFrame> frame,
                                               bool is_mouse_trigger) {
#if BUILDFLAG(ARKWEB_MENU)
  if (!is_mouse_trigger || is_rich_text_) {
    if (nweb_handler_ != nullptr) {
      nweb_handler_->OnQuickMenuDismissed();
    }
  }
#else
  if (nweb_handler_ != nullptr) {
    nweb_handler_->OnQuickMenuDismissed();
  }
#endif
}

void NWebHandlerDelegate::HideHandleAndQuickMenuIfNecessary(bool hide) {
  LOG(INFO) << "HideHandleAndQuickMenuIfNecessary hide:" << hide;
  if (nweb_handler_ != nullptr) {
    nweb_handler_->HideHandleAndQuickMenuIfNecessary(hide);
  }
}
#if BUILDFLAG(ARKWEB_CLIPBOARD)
void NWebHandlerDelegate::ChangeVisibilityOfQuickMenu() {
  if (nweb_handler_ != nullptr) {
    nweb_handler_->ChangeVisibilityOfQuickMenu();
  }
}
#endif

#if BUILDFLAG(ARKWEB_AI)
bool NWebHandlerDelegate::CloseImageOverlaySelection() {
  if (nweb_handler_ != nullptr) {
    return nweb_handler_->CloseImageOverlaySelection();
  }
  return false;
}
#endif
/* CefContextMenuHandler method end */

/* CefFindandler method begin */

void NWebHandlerDelegate::OnFindResult(CefRefPtr<CefBrowser> browser,
                                       int identifier,
                                       int count,
                                       const CefRect& selectionRect,
                                       int activeMatchOrdinal,
                                       bool finalUpdate) {
  if (find_delegate_ != nullptr) {
    find_delegate_->HandleFindReply(identifier, count, activeMatchOrdinal,
                                    finalUpdate);
  }
}

/* CefFindandler method end */

/* CefResourceRequestHandler methods begin */

bool NWebHandlerDelegate::CanSendCookie(CefRefPtr<CefBrowser> browser,
                                        CefRefPtr<CefFrame> frame,
                                        CefRefPtr<CefRequest> request,
                                        const CefCookie& cookie) {
  auto cookie_manager = NWebEngineImpl::GetInstance()->GetCookieManager();
  bool block_all_cookies = !cookie_manager->IsAcceptCookieAllowed();
  bool block_thirdparty_cookies = !cookie_manager->IsThirdPartyCookieAllowed();
  return AllowCookies(GURL(request->GetURL().ToString()),
                      net::SiteForCookies::FromUrl(
                          GURL(request->GetFirstPartyForCookies().ToString())),
                      block_all_cookies, block_thirdparty_cookies);
}
bool NWebHandlerDelegate::CanSaveCookie(CefRefPtr<CefBrowser> browser,
                                        CefRefPtr<CefFrame> frame,
                                        CefRefPtr<CefRequest> request,
                                        CefRefPtr<CefResponse> response,
                                        const CefCookie& cookie) {
  auto cookie_manager = NWebEngineImpl::GetInstance()->GetCookieManager();
  bool block_all_cookies = !cookie_manager->IsAcceptCookieAllowed();
  bool block_thirdparty_cookies = !cookie_manager->IsThirdPartyCookieAllowed();
  return AllowCookies(GURL(request->GetURL().ToString()),
                      net::SiteForCookies::FromUrl(
                          GURL(request->GetFirstPartyForCookies().ToString())),
                      block_all_cookies, block_thirdparty_cookies);
}
/* CefResourceRequestHandler methods end */

const std::vector<std::string> NWebHandlerDelegate::GetVisitedHistory() {
  if (nweb_handler_ != nullptr) {
    return nweb_handler_->VisitedUrlHistory();
  }
  return std::vector<std::string>();
}

void NWebHandlerDelegate::RegisterNativeLoadStartCallback(
    std::function<void(void)>&& callback) {
  onLoadStartCallback_ = std::move(callback);
}

void NWebHandlerDelegate::RegisterNativeLoadEndCallback(
    std::function<void(void)>&& callback) {
  onLoadEndCallback_ = std::move(callback);
}

void NWebHandlerDelegate::RegisterNativeJavaScriptCallBack(
    const char* objName,
    const std::vector<std::shared_ptr<NWebJsProxyCallback>>& callbacks) {
  std::unordered_map<std::string, std::function<char*(const char**, int32_t)>>
      map;
  for (auto callback : callbacks) {
    map[callback->GetMethodName()] = callback->GetMethodCallback();
  }
  objMap_[objName] = map;
}

void NWebHandlerDelegate::RegisterNativeJavaScriptCallBack(
    const std::string& objName,
    const std::vector<std::string>& methodName,
    std::vector<NativeJSProxyCallbackFunc>&& callback,
    bool isAsync,
    const std::string& permission) {
  size_t size = methodName.size();
  if (size == 0) {
    LOG(ERROR) << "NWebHandlerDelegate RegisterNativeJavaScriptCallBack error: "
                  "empty methods list";
    return;
  }
  std::unordered_map<std::string, NativeJSProxyCallbackFunc> map;
  for (size_t i = 0; i < size; i++) {
    map[methodName[i]] = callback[i];
  }
  if (isAsync) {
    if (auto async_it = asyncProxyObjWithResultMap_.find(objName);
        async_it != asyncProxyObjWithResultMap_.end()) {
      asyncProxyObjWithResultMap_.erase(objName);
    }
    asyncProxyObjMap_[objName] = map;
    asyncProxyPermissionMap_[objName] = permission;
  } else {
    if (auto async_it = syncProxyObjWithResultMap_.find(objName);
        async_it != syncProxyObjWithResultMap_.end()) {
      syncProxyObjWithResultMap_.erase(objName);
    }
    if (auto async_it = asyncThreadProxyObjWithResultMap_.find(objName);
      async_it != asyncThreadProxyObjWithResultMap_.end()) {
      asyncThreadProxyObjWithResultMap_.erase(objName);
    }
    syncProxyObjMap_[objName] = map;
    syncProxyPermissionMap_[objName] = permission;
  }
}

void NWebHandlerDelegate::RegisterNativeJavaScriptCallBackWithResult(
    const std::string& objName,
    const std::vector<std::string>& methodName,
    std::vector<NativeJSProxyCallbackFuncWithResult>&& callback,
    bool isAsync,
    const std::string& permission) {
  size_t size = methodName.size();
  if (size == 0) {
    LOG(ERROR) << "NWebHandlerDelegate RegisterNativeJavaScriptCallBack error: "
                  "empty methods list";
    return;
  }
  std::unordered_map<std::string, NativeJSProxyCallbackFuncWithResult> map;
  for (size_t i = 0; i < size; i++) {
    map[methodName[i]] = callback[i];
  }
  if (isAsync) {
    if (auto async_it = asyncProxyObjMap_.find(objName);
        async_it != asyncProxyObjMap_.end()) {
      asyncProxyObjMap_.erase(objName);
    }
    asyncProxyObjWithResultMap_[objName] = map;
    asyncProxyPermissionMap_[objName] = permission;
  } else {
    if (auto sync_it = syncProxyObjMap_.find(objName);
        sync_it != syncProxyObjMap_.end()) {
      syncProxyObjMap_.erase(objName);
    }
    if (auto sync_it = asyncThreadProxyObjWithResultMap_.find(objName);
        sync_it != asyncThreadProxyObjWithResultMap_.end()) {
      asyncThreadProxyObjWithResultMap_.erase(objName);
    }
    syncProxyObjWithResultMap_[objName] = map;
    syncProxyPermissionMap_[objName] = permission;
  }
}

void NWebHandlerDelegate::RegisterNativeAsyncThreadJavaScriptCallBackWithResult(
    const std::string& objName,
    const std::vector<std::string>& methodName,
    std::vector<NativeJSProxyCallbackFuncWithResult>&& callback,
    const std::string& permission) {
  size_t size = methodName.size();
  if (size == 0) {
    LOG(ERROR) << "NWebHandlerDelegate RegisterNativeAsyncThreadJavaScriptCallBackWithResult error: "
                  "empty methods list";
    return;
  }
  std::unordered_map<std::string, NativeJSProxyCallbackFuncWithResult> map;
  for (size_t i = 0; i < size; i++) {
    map[methodName[i]] = callback[i];
    asyncThreadProxyObjWithResultMap_[objName].insert(methodName[i]);
  }
  if (auto sync_it = syncProxyObjMap_.find(objName);
      sync_it != syncProxyObjMap_.end()) {
    syncProxyObjMap_.erase(objName);
  }
  syncProxyObjWithResultMap_[objName] = map;
  syncProxyPermissionMap_[objName] = permission;
}

int NWebHandlerDelegate::ProcessNativeProxyResultThread(
    CefRefPtr<CefListValue> args,
    const CefString& method,
    const CefString& object_name,
    CefRefPtr<CefListValue> result) {
  auto it = objMap_.find(object_name);
  if (it == objMap_.end()) {
    // object name not found
    return 1;
  }
  auto& methodMap = it->second;
  auto methodIt = methodMap.find(method);
  if (methodIt == methodMap.end()) {
    // method name not found
    return 1;
  }

  auto callback = methodMap[method];
  char** ptr = (char**)malloc(sizeof(char*) * args->GetSize());
  for (size_t i = 0; i < args->GetSize(); i++) {
    CefValueType type = args->GetType(i);
    CefRefPtr<CefValue> value = args->GetValue(i);
    if (type == VTYPE_STRING) {
      ptr[i] = strdup(value->GetString().ToString().c_str());
    } else {
      std::string jsonString =
          CefWriteJSON(value, JSON_WRITER_OMIT_BINARY_VALUES);
      ptr[i] = strdup(jsonString.c_str());
    }
  }
  char* callbackResult = callback((const char**)ptr, args->GetSize());
  if (callbackResult) {
    result->SetString(0, callbackResult);
  } else {
    LOG(INFO) << "native return nullptr, just set null string to result";
    result->SetString(0, "null");
  }

  for (size_t i = 0; i < args->GetSize(); i++) {
    if (ptr[i]) {
      free(ptr[i]);
      ptr[i] = nullptr;
    }
  }
  if (ptr) {
    free(ptr);
    ptr = nullptr;
  }
  return 0;
}

int NWebHandlerDelegate::ProcessNativeProxyResultNew(
    CefRefPtr<CefListValue> args,
    const CefString& method,
    const CefString& object_name,
    CefRefPtr<CefListValue> result) {
  if (!args) {
    LOG(ERROR) << "args is nullptr";
    return 1;
  }

  NativeJSProxyCallbackFunc callback = nullptr;
  auto it = asyncProxyObjMap_.find(object_name);
  if (it != asyncProxyObjMap_.end()) {
    auto& methodMap = it->second;
    auto methodIt = methodMap.find(method);
    if (methodIt != methodMap.end()) {
      LOG(DEBUG) << "Processing async native proxy result, "
                 << "method name: " << method.ToString();
      callback = methodMap[method];
    }
  }
  if (callback == nullptr) {
    it = syncProxyObjMap_.find(object_name);
    if (it != syncProxyObjMap_.end()) {
      auto& methodMap = it->second;
      auto methodIt = methodMap.find(method);
      if (methodIt != methodMap.end()) {
        LOG(DEBUG) << "Processing sync native proxy result, "
                   << "method name: " << method.ToString();
        callback = methodMap[method];
      }
    }
  }

  if (callback == nullptr) {
    LOG(DEBUG) << "Processing sync native proxy result failed, "
               << "method not found, name: " << method.ToString();
    return 1;
  }
  size_t argsSize = args->GetSize();
  std::vector<std::vector<uint8_t>> dataList(argsSize);
  std::vector<size_t> dataSize(argsSize);

  ParseNativeProxyArgs(args, dataList, dataSize);

  char* callbackResult = callback(dataList, dataSize);
  if (callbackResult) {
    result->SetString(0, callbackResult);
  } else {
    LOG(INFO) << "native return nullptr, just set null string to result";
    result->SetNull(0);
  }

  return 0;
}

int NWebHandlerDelegate::ProcessNativeProxyResultNewForReturnValue(
    CefRefPtr<CefListValue> args,
    const CefString& method,
    const CefString& object_name,
    CefRefPtr<CefListValue> result) {
  if (!args) {
    LOG(ERROR) << "args is nullptr";
    return 1;
  }

  NativeJSProxyCallbackFuncWithResult CallbackWithResult = nullptr;

  if (auto iter = asyncProxyObjWithResultMap_.find(object_name);
      iter != asyncProxyObjWithResultMap_.end()) {
    auto& methodMap = iter->second;
    auto methodIt = methodMap.find(method);
    if (methodIt != methodMap.end()) {
      LOG(DEBUG) << "Processing async native proxy result, "
                 << "method name: " << method.ToString();
      CallbackWithResult = methodMap[method];
    }
  }

  if (CallbackWithResult == nullptr) {
    auto iter = syncProxyObjWithResultMap_.find(object_name);
    if (iter != syncProxyObjWithResultMap_.end()) {
      auto& methodMap = iter->second;
      auto methodIt = methodMap.find(method);
      if (methodIt != methodMap.end()) {
        LOG(DEBUG) << "Processing sync native proxy result, "
                   << "method name: " << method.ToString();
        CallbackWithResult = methodMap[method];
      }
    }
  }

  if (CallbackWithResult == nullptr) {
    LOG(DEBUG) << "Processing native proxy result failed, "
               << "method not found, name: " << method.ToString();
    return 1;
  }
  size_t argsSize = args->GetSize();
  std::vector<std::vector<uint8_t>> dataList(argsSize);
  std::vector<size_t> dataSize(argsSize);
  ParseNativeProxyArgs(args, dataList, dataSize);

  std::shared_ptr<OHOS::NWeb::NWebValue> callbackResult =
      CallbackWithResult(dataList, dataSize);
  if (callbackResult) {
    ParseNWebValueToValue(callbackResult, result);
  } else {
    LOG(DEBUG) << "native return nullptr, just set null string to result";
    result->SetNull(0);
  }

  return 0;
}

int NWebHandlerDelegate::ProcessNativeProxyResult(
    CefRefPtr<CefListValue> args,
    const CefString& method,
    const CefString& object_name,
    CefRefPtr<CefListValue> result) {
  if (auto it = syncProxyObjWithResultMap_.find(object_name);
      it != syncProxyObjWithResultMap_.end()) {
    if (ProcessNativeProxyResultNew(args, method, object_name, result) == 1) {
      ProcessNativeProxyResultNewForReturnValue(args, method, object_name,
                                                result);
    }
    return 0;
  } else if (auto async_it = asyncProxyObjWithResultMap_.find(object_name);
             async_it != asyncProxyObjWithResultMap_.end()) {
    if (ProcessNativeProxyResultNew(args, method, object_name, result) == 1) {
      ProcessNativeProxyResultNewForReturnValue(args, method, object_name,
                                                result);
    }
    return 0;
  } else if (auto it_with_result = syncProxyObjMap_.find(object_name);
             it_with_result != syncProxyObjMap_.end()) {
    if (ProcessNativeProxyResultNew(args, method, object_name, result) == 1) {
      ProcessNativeProxyResultNewForReturnValue(args, method, object_name,
                                                result);
    }
    return 0;
  } else if (auto async_it_with_result = asyncProxyObjMap_.find(object_name);
             async_it_with_result != asyncProxyObjMap_.end()) {
    if (ProcessNativeProxyResultNew(args, method, object_name, result) == 1) {
      ProcessNativeProxyResultNewForReturnValue(args, method, object_name,
                                                result);
    }
    return 0;
  }

  if (auto it = objMap_.find(object_name); it != objMap_.end()) {
    ProcessNativeProxyResultThread(args, method, object_name, result);
    return 0;
  }

  LOG(DEBUG) << "native proxy object not found, name:"
             << object_name.ToString();
  return 1;
}

int NWebHandlerDelegate::NotifyJavaScriptResult(CefRefPtr<CefListValue> args,
                                                const CefString& method,
                                                const CefString& object_name,
                                                CefRefPtr<CefListValue> result,
                                                int32_t routing_id,
                                                int32_t object_id) {
  if (args.get() == nullptr || result.get() == nullptr) {
    return 0;
  }

  if (!ProcessNativeProxyResult(args, method, object_name, result)) {
    // native proxy object
    return 0;
  }  // ets proxy object

  if (!nweb_javascript_callback_) {
    return 1;
  }

  std::shared_ptr<NWebHapValue> hap_result =
      std::make_shared<NWebCoreValue>(NWebHapValue::Type::NONE);
  std::vector<std::shared_ptr<NWebHapValue>> hap_value_vector =
      ParseCefValueToHapValue(args, args->GetSize());
  nweb_javascript_callback_->GetJavaScriptResultV2(
      hap_value_vector, method, object_name, routing_id, object_id, hap_result);
  if (ArkWebGetErrno() != RESULT_OK) {
    std::vector<std::shared_ptr<NWebValue>> value_vector =
        ParseCefValueTONWebValue(args, args->GetSize());
    std::shared_ptr<NWebValue> ark_result =
        nweb_javascript_callback_->GetJavaScriptResult(
            value_vector, method, object_name, routing_id, object_id);
    if (!ark_result) {
      return 1;
    }
    ParseNWebValueToValue(ark_result, result);
    return 0;
  }

  ParseNWebValueToHapValue(hap_result, result);
  return 0;
}

// flowbuf sketch diagram
// +--------------------------------------------+
// | index1 | length1 | index2 | ... | length10 |
// +--------------------------------------------+
// | string1 | string2 |     ...     | string10 |
// +--------------------------------------------+
// iterate through flowbuf header, and count No. of string stored
int NWebHandlerDelegate::GetFlowbufCount(void* mem) {
  int* header = static_cast<int*>(
      mem);  // Cast the memory block to int* for easier access
  int count = 0;
  for (int i = 0; i < MAX_ENTRIES; i++) {
    if (*(header + (i * 2) + 1) != 0) {
      count++;
    } else {
      break;
    }
  }
  return count;
}

// flowbufIndex is index in flowbuf, argindex is string's original index in V8
// arglist
char* NWebHandlerDelegate::FlowbufStrAtIndex(void* mem,
                                             int flowbufIndex,
                                             int* argIndex,
                                             int* strLen) {
  int* header = static_cast<int*>(
      mem);  // Cast the memory block to int* for easier access
  int offset = 0;

  if (flowbufIndex >= MAX_ENTRIES) {
    *argIndex = -1;
    return nullptr;
  }

  int* entry = header + (flowbufIndex * INDEX_SIZE);
  if (*(entry + 1) == 0) {  // Check if length is 0, indicating unused entry
    *argIndex = -1;
    return nullptr;
  }

  int i = 0;
  for (i = 0; i < flowbufIndex; i++) {
    offset += *(header + (i * INDEX_SIZE) + 1);
  }

  *strLen = *(header + (i * INDEX_SIZE) + 1) - 1;
  *argIndex = *entry;

  char* dataSegment = static_cast<char*>(mem) + HEADER_SIZE;
  char* currentString = dataSegment + offset;
  return currentString;
}

int NWebHandlerDelegate::ProcessNativeProxyResultNewFlowbuf(
    CefRefPtr<CefListValue> args,
    const CefString& method,
    const CefString& object_name,
    int fd,
    CefRefPtr<CefListValue> result) {
  auto it = syncProxyObjMap_.find(object_name);
  if (it == syncProxyObjMap_.end()) {
    // object name not found
    return 1;
  }
  auto& methodMap = it->second;
  auto methodIt = methodMap.find(method);
  if (methodIt == methodMap.end()) {
    // method name not found
    return 1;
  }

  if (!args) {
    LOG(ERROR) << "args is nullptr";
    return 1;
  }

  auto flowbufferAdapter =
      OhosAdapterHelper::GetInstance().CreateFlowbufferAdapter();
  if (!flowbufferAdapter) {
    close(fd);
    return 1;
  }
  auto ashmem = flowbufferAdapter->CreateAshmemWithFd(
      fd, MAX_FLOWBUF_DATA_SIZE + HEADER_SIZE, PROT_READ);
  if (!ashmem) {
    close(fd);
    return 1;
  }

  size_t argsSize = args->GetSize();
  auto callback = methodMap[method];
  int flowbufSize = GetFlowbufCount(ashmem);
  std::vector<std::vector<uint8_t>> dataList(argsSize +
                                             static_cast<size_t>(flowbufSize));
  std::vector<size_t> dataSize(argsSize + static_cast<size_t>(flowbufSize));

  int argIndex = -1;
  int curIndex = 0;
  int flowbufIndex = 0;
  int strLen = 0;
  char* flowbufStr =
      FlowbufStrAtIndex(ashmem, flowbufIndex, &argIndex, &strLen);
  flowbufIndex++;
  while (argIndex == curIndex) {
    std::string flowbuf_stdstr(flowbufStr, strLen);
    dataList[curIndex] =
        std::vector<uint8_t>(flowbuf_stdstr.begin(), flowbuf_stdstr.end());
    dataSize[curIndex] = strLen;
    curIndex++;
    flowbufStr = FlowbufStrAtIndex(ashmem, flowbufIndex, &argIndex, &strLen);
    flowbufIndex++;
  }

  for (size_t i = 0; i < argsSize; i++) {
    while (argIndex == curIndex) {
      std::string flowbuf_stdstr(flowbufStr, strLen);
      dataList[curIndex] =
          std::vector<uint8_t>(flowbuf_stdstr.begin(), flowbuf_stdstr.end());
      dataSize[curIndex] = strLen;
      curIndex++;
      flowbufStr = FlowbufStrAtIndex(ashmem, flowbufIndex, &argIndex, &strLen);
      flowbufIndex++;
    }

    CefValueType type = args->GetType(i);
    CefRefPtr<CefValue> value = args->GetValue(i);
    if (!value) {
      LOG(ERROR) << "value is nullptr";
      curIndex++;
      continue;
    }

    if (type == VTYPE_STRING) {
      auto argString = value->GetString().ToString();
      size_t size = argString.size();

      dataList[curIndex] =
          std::vector<uint8_t>(argString.begin(), argString.end());
      dataSize[curIndex] = size;
    } else if (type == VTYPE_BINARY) {
      auto argBinary = value->GetBinary();
      size_t size = argBinary->GetSize();

      std::vector<uint8_t> data(size);
      argBinary->GetData(&data[0], size, 0);

      dataList[curIndex] = std::move(data);
      dataSize[curIndex] = size;
    } else {
      std::string jsonString =
          CefWriteJSON(value, JSON_WRITER_OMIT_BINARY_VALUES);
      dataList[curIndex] =
          std::vector<uint8_t>(jsonString.begin(), jsonString.end());
      dataSize[curIndex] = jsonString.size();
    }
    curIndex++;
  }

  while (argIndex == curIndex) {
    std::string flowbuf_stdstr(flowbufStr, strLen);
    dataList[curIndex] =
        std::vector<uint8_t>(flowbuf_stdstr.begin(), flowbuf_stdstr.end());
    dataSize[curIndex] = strLen;
    curIndex++;
    flowbufStr = FlowbufStrAtIndex(ashmem, flowbufIndex, &argIndex, &strLen);
    flowbufIndex++;
  }

  close(fd);

  char* callbackResult = callback(dataList, dataSize);
  if (callbackResult) {
    result->SetString(0, callbackResult);
  } else {
    LOG(INFO) << "native return nullptr, just set null string to result";
    result->SetNull(0);
  }

  return 0;
}

int NWebHandlerDelegate::ProcessNativeProxyResultFlowbuf(
    CefRefPtr<CefListValue> args,
    const CefString& method,
    const CefString& object_name,
    int fd,
    CefRefPtr<CefListValue> result) {
  if (auto it = syncProxyObjMap_.find(object_name);
      it != syncProxyObjMap_.end()) {
    ProcessNativeProxyResultNewFlowbuf(args, method, object_name, fd, result);
    return 0;
  }

  if (auto it = objMap_.find(object_name); it != objMap_.end()) {
    auto flowbufferAdapter =
        OhosAdapterHelper::GetInstance().CreateFlowbufferAdapter();
    if (!flowbufferAdapter) {
      close(fd);
      return 1;
    }

    auto ashmem = flowbufferAdapter->CreateAshmemWithFd(
        fd, MAX_FLOWBUF_DATA_SIZE + HEADER_SIZE, PROT_READ);
    if (!ashmem) {
      close(fd);
      return 1;
    }

    int argIndex = -1;
    int flowbufIndex = 0;
    int strLen = 0;
    do {
      char* flowbufStr =
          FlowbufStrAtIndex(ashmem, flowbufIndex, &argIndex, &strLen);
      if (argIndex == -1) {
        break;
      }
      flowbufIndex++;
      std::string str(flowbufStr);
      CefRefPtr<CefValue> value = CefValue::Create();
      value->SetString(str);
      args->SetValue(argIndex, value);
    } while (argIndex <= MAX_ENTRIES);
    close(fd);
    ProcessNativeProxyResultThread(args, method, object_name, result);
    return 0;
  }

  LOG(ERROR) << "native proxy object not found, name:"
             << object_name.ToString();
  return 1;
}

int NWebHandlerDelegate::NotifyJavaScriptResultFlowbuf(
    CefRefPtr<CefListValue> args,
    const CefString& method,
    const CefString& object_name,
    int fd,
    CefRefPtr<CefListValue> result,
    int32_t routing_id,
    int32_t object_id) {
  if (args.get() == nullptr || result.get() == nullptr) {
    return 0;
  }

  if (!ProcessNativeProxyResultFlowbuf(args, method, object_name, fd, result)) {
    // native proxy object
    return 0;
  }  // ets proxy object

  if (!nweb_javascript_callback_) {
    return 1;
  }

  std::shared_ptr<NWebHapValue> hap_result =
      std::make_shared<NWebCoreValue>(NWebHapValue::Type::NONE);
  std::vector<std::shared_ptr<NWebHapValue>> hap_value_vector =
      ParseCefValueToHapValue(args, args->GetSize());
  nweb_javascript_callback_->GetJavaScriptResultFlowbufV2(
      hap_value_vector, method, object_name, fd, routing_id, object_id,
      hap_result);
  if (ArkWebGetErrno() != RESULT_OK) {
    std::vector<std::shared_ptr<NWebValue>> value_vector =
        ParseCefValueTONWebValue(args, args->GetSize());
    std::shared_ptr<NWebValue> ark_result =
        nweb_javascript_callback_->GetJavaScriptResultFlowbuf(
            value_vector, method, object_name, fd, routing_id, object_id);
    if (!ark_result) {
      return 1;
    }
    ParseNWebValueToValue(ark_result, result);
    return 0;
  }
  ParseNWebValueToHapValue(hap_result, result);
  return 0;
}

bool NWebHandlerDelegate::HasJavaScriptObjectMethods(
    int32_t object_id,
    const CefString& method_name) {
  if (!nweb_javascript_callback_) {
    LOG(ERROR) << "NWebHandlerDelegate::HasJavaScriptObjectMethods "
                  "nweb_javascript_callback_ is null";
    return false;
  }
  return nweb_javascript_callback_->HasJavaScriptObjectMethods(object_id,
                                                               method_name);
}

bool NWebHandlerDelegate::HasNativeAsyncThreadJavaScriptMethods(
    std::string& object_name,
    const std::string& method_name) {
  auto iter1 = asyncThreadProxyObjWithResultMap_.find(object_name);
  if (iter1 == asyncThreadProxyObjWithResultMap_.end()) {
    return false;
  }

  auto iter2 = asyncThreadProxyObjWithResultMap_[object_name].find(method_name);
  if (iter2 == asyncThreadProxyObjWithResultMap_[object_name].end()) {
    return false;
  }

  return true;
}

void NWebHandlerDelegate::GetJavaScriptObjectMethods(
    int32_t object_id,
    CefRefPtr<CefValue> returned_method_names) {
  if (!nweb_javascript_callback_) {
    LOG(ERROR) << "NWebHandlerDelegate::GetJavaScriptObjectMethods "
                  "nweb_javascript_callback_ is null";
    return;
  }

  std::shared_ptr<NWebHapValue> hap_result =
      std::make_shared<NWebCoreValue>(NWebHapValue::Type::NONE);
  nweb_javascript_callback_->GetJavaScriptObjectMethodsV2(object_id,
                                                          hap_result);
  if (ArkWebGetErrno() != RESULT_OK) {
    std::shared_ptr<NWebValue> ark_result =
        nweb_javascript_callback_->GetJavaScriptObjectMethods(object_id);
    if (!ark_result) {
      LOG(ERROR)
          << "NWebHandlerDelegate::GetJavaScriptObjectMethods result is null";
      return;
    }
    returned_method_names = ParseNWebValueToValueHelper(ark_result);
  } else {
    returned_method_names = ParseHapValueToValueHelper(hap_result);
  }
}

void NWebHandlerDelegate::RemoveJavaScriptObjectHolder(int32_t holder,
                                                       int32_t object_id) {
  if (!nweb_javascript_callback_) {
    LOG(ERROR) << "NWebHandlerDelegate::RemoveJavaScriptObjectHolder "
                  "nweb_javascript_callback_ is null";
    return;
  }
  nweb_javascript_callback_->RemoveJavaScriptObjectHolder(holder, object_id);
}

void NWebHandlerDelegate::RemoveTransientJavaScriptObject() {
  if (!nweb_javascript_callback_) {
    LOG(ERROR) << "NWebHandlerDelegate::RemoveTransientJavaScriptObject "
                  "nweb_javascript_callback_ is null";
    return;
  }
  nweb_javascript_callback_->RemoveTransientJavaScriptObject();
}

bool NWebHandlerDelegate::OnTooltip(CefRefPtr<CefBrowser> browser,
                                    CefString& text) {
  if (nweb_handler_ != nullptr && !base::ohos::IsMobileDevice()) {
    nweb_handler_->OnTooltip(text.ToString());
    return true;
  }
  return false;
}

#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
void NWebHandlerDelegate::OnViewportFitChange(CefRefPtr<CefBrowser> browser,
                                              int viewportFit) {
  if (nweb_handler_ != nullptr) {
    ViewportFit viewport_fit_type = static_cast<ViewportFit>(viewportFit);
    nweb_handler_->OnViewportFitChange(viewport_fit_type);
  }
}
#endif

void NWebHandlerDelegate::SetNWebId(uint32_t nwebId) {
  nweb_id_ = nwebId;
}

uint32_t NWebHandlerDelegate::GetNWebId() {
  return nweb_id_;
}

#if BUILDFLAG(ARKWEB_FOCUS)
bool NWebHandlerDelegate::GetFocusState() {
  return focusState_;
}

void NWebHandlerDelegate::SetFocusState(bool focusState) {
  focusState_ = focusState;
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  if (event_handler_ != nullptr) {
    event_handler_->SetIsFocus(focusState);
  }
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
}
#endif  // #if BUILDFLAG(ARKWEB_FOCUS)

#if BUILDFLAG(ARKWEB_ITP)
void NWebHandlerDelegate::OnIntelligentTrackingPreventionResult(
    const CefString& website_host,
    const CefString& tracker_host) {
  LOG(INFO) << "NWebHandlerDelegate::OnIntelligentTrackingPreventionResult";
  if (web_app_client_extension_listener_ != nullptr &&
      web_app_client_extension_listener_
              ->OnIntelligentTrackingPreventionResult != nullptr) {
    web_app_client_extension_listener_->OnIntelligentTrackingPreventionResult(
        website_host, tracker_host,
        web_app_client_extension_listener_->nweb_id);
  }

  if (nweb_handler_ != nullptr) {
    nweb_handler_->OnIntelligentTrackingPreventionResult(website_host,
                                                         tracker_host);
  }
}
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
bool NWebHandlerDelegate::OnAllCertificateError(
    CefRefPtr<CefBrowser> browser,
    cef_errorcode_t cert_error,
    const CefString& request_url,
    const CefString& origin_url,
    const CefString& referrer,
    bool is_main_frame_request,
    bool is_fatal_error,
    CefRefPtr<CefSSLInfo> ssl_info,
    CefRefPtr<CefCallback> callback) {
  LOG(INFO) << "NWebHandlerDelegate::OnAllCertificateError happened";
  SslError error = SslErrorConvert(cert_error);

  CEF_REQUIRE_IO_THREAD();
  std::shared_ptr<NWebJSAllSslErrorResult> js_result =
      std::make_shared<NWebJSAllSslErrorResultImpl>(callback);
  if (nweb_handler_ != nullptr) {
    return nweb_handler_->OnAllSslErrorRequestByJS(
        js_result, error, request_url, origin_url, referrer, is_fatal_error,
        is_main_frame_request);
  }
  return false;
}

void NWebHandlerDelegate::OnLoadStarted(CefRefPtr<CefFrame> frame,
                                        const CefString& url) {
  LOG(INFO) << "NWebHandlerDelegate::OnLoadStarted";
  if (frame == nullptr || !frame->IsMain()) {
    return;
  }

  if (nweb_handler_ != nullptr) {
    nweb_handler_->OnLoadStarted(url.ToString());
  }
}

void NWebHandlerDelegate::OnLoadFinished(CefRefPtr<CefFrame> frame,
                                         const CefString& url) {
  LOG(INFO) << "NWebHandlerDelegate::OnLoadFinished";
  if (frame == nullptr || !frame->IsMain()) {
    return;
  }

  if (nweb_handler_ != nullptr) {
    nweb_handler_->OnLoadFinished(url.ToString());
  }
}
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
void NWebHandlerDelegate::RegisterOnCreateNativeMediaPlayerListener(
    std::shared_ptr<NWebCreateNativeMediaPlayerCallback> callback) {
  WVLOG_I("RegisterOnCreateNativeMediaPlayerListener(%{public}p)",
          callback.get());
  create_native_media_player_cb_ = std::move(callback);
}

CefOwnPtr<CefCustomMediaPlayerDelegate>
NWebHandlerDelegate::OnCreateCustomMediaPlayer(
    CefOwnPtr<CefMediaPlayerListener> listener,
    const CefCustomMediaInfo& media_info) {
  if (!create_native_media_player_cb_) {
    LOG(ERROR) << "OnCreateNativeMediaPlayer failed, callback is null";
    return nullptr;
  }

  std::shared_ptr<NWebNativeMediaPlayerHandler> handler(
      new NWebNativeMediaPlayerHandlerImpl(std::move(listener)));
  std::shared_ptr<NWebMediaInfo> nweb_media_info(
      new NWebMediaInfoImpl(media_info));
  std::shared_ptr<NWebNativeMediaPlayerBridge> bridge =
      create_native_media_player_cb_->OnCreate(std::move(handler),
                                               std::move(nweb_media_info));
  if (!bridge) {
    LOG(INFO) << "app creates no media player";
    return nullptr;
  }

  CefOwnPtr<CustomMediaPlayerImpl> player(
      new CustomMediaPlayerImpl(std::move(bridge)));
  return player;
}
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

void NWebHandlerDelegate::OnShowToast(double duration, const CefString& toast) {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (!web_app_client_extension_listener_) {
    LOG(WARNING) << "application extension listener is nullptr";
    return;
  }

  if (!web_app_client_extension_listener_->OnShowToast) {
    LOG(WARNING) << "show toast callback is nullptr";
    return;
  }

  web_app_client_extension_listener_->OnShowToast(
      web_app_client_extension_listener_->nweb_id, duration,
      toast.ToString().c_str());
#endif  // ARKWEB_VIDEO_ASSISTANT
}

void NWebHandlerDelegate::OnShowVideoAssistant(
    const CefString& videoAssistantItems) {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (!web_app_client_extension_listener_) {
    LOG(WARNING) << "application extension listener is nullptr";
    return;
  }

  if (!web_app_client_extension_listener_->OnShowVideoAssistant) {
    LOG(WARNING) << "show video assistant callback is nullptr";
    return;
  }

  web_app_client_extension_listener_->OnShowVideoAssistant(
      web_app_client_extension_listener_->nweb_id,
      videoAssistantItems.ToString().c_str());
#endif  // ARKWEB_VIDEO_ASSISTANT
}

void NWebHandlerDelegate::OnReportStatisticLog(const CefString& content) {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  NWebImpl::OnReportStatisticLog(content.ToString());
#endif  // ARKWEB_VIDEO_ASSISTANT
}

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
CefOwnPtr<CefMediaPlayerListenerForVAST>
NWebHandlerDelegate::OnFullScreenOverlayEnter(
    CefOwnPtr<CefMediaPlayerController> media_player_controller,
    const std::string& extra_info) {
  if (!web_app_client_extension_listener_) {
    LOG(WARNING) << "application extension listener is nullptr";
    return nullptr;
  }

  if (!web_app_client_extension_listener_->OnFullScreenOverlayEnter) {
    LOG(WARNING) << "OnFullScreenOverlayEnter is nullptr";
    return nullptr;
  }
  auto controller = std::make_unique<NWebMediaPlayerControllerImpl>(
      std::move(media_player_controller));

  auto listener = web_app_client_extension_listener_->OnFullScreenOverlayEnter(
      web_app_client_extension_listener_->nweb_id,
      controller.release(), extra_info.c_str());
  if (!listener) {
    return nullptr;
  }
  return std::make_unique<NWebMediaPlayerListenerForVAST>(
      std::unique_ptr<NWebMediaPlayerListener>(listener));
}
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_RENDERER_ANR_DUMP)
void NWebHandlerDelegate::OnRenderProcessNotResponding(
    CefRefPtr<CefBrowser> browser,
    const CefString& js_stack,
    int pid,
    int reason) {
  if (nweb_handler_ == nullptr) {
    LOG(ERROR) << "invalid nweb handler (nullptr)";
    return;
  }
  LOG(INFO) << "OnRenderProcessNotResponding:" << pid;
  RenderProcessNotRespondingReason anr_reason =
      static_cast<RenderProcessNotRespondingReason>(reason);
  nweb_handler_->OnRenderProcessNotResponding(js_stack, pid, anr_reason);
}

void NWebHandlerDelegate::OnRenderProcessResponding(
    CefRefPtr<CefBrowser> browser) {
  if (nweb_handler_ == nullptr) {
    LOG(ERROR) << "invalid nweb handler (nullptr)";
    return;
  }
  LOG(INFO) << "OnRenderProcessResponding";
  nweb_handler_->OnRenderProcessResponding();
}

void NWebHandlerDelegate::SetPopupSurface(void* popup_window) {
  if (main_browser_ && main_browser_->GetHost()) {
    if (!is_enhance_surface_) {
      if (popup_window_ != nullptr && popup_window_ != popup_window) {
        OHOS::NWeb::OhosAdapterHelperExt::GetWindowAdapterNdkInstance()
            .DestroyNativeWindow(popup_window_);
        popup_window_ = nullptr;
      }
      popup_window_ = popup_window;
      main_browser_->GetHost()->SetPopupWindow(popup_window_);
    }
  } else {
    popup_window_ = popup_window;
  }
}
#endif

#if BUILDFLAG(ARKWEB_NWEB_EX)
void NWebHandlerDelegate::OnUpdateTargetURL(CefRefPtr<CefBrowser> browser,
                                            const CefString& url) {
  if (web_app_client_extension_listener_ != nullptr &&
      web_app_client_extension_listener_->OnUpdateTargetURL != nullptr) {
    web_app_client_extension_listener_->OnUpdateTargetURL(
        url.ToString(), web_app_client_extension_listener_->nweb_id);
  }
}
#endif

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
// static
void NWebHandlerDelegate::RegisterWebExtensionApiListener(
    std::shared_ptr<NWebExtensionApiCallback> web_extension_api_listener) {
  LOG(INFO) << "RegisterWebExtensionApiListener";
  // TODO: Expected to be an instance of a profile
  g_extension_api_listener = web_extension_api_listener;
}

// static
void NWebHandlerDelegate::UnRegisterWebExtensionApiListener() {
  LOG(INFO) << "UnRegisterWebExtensionApiListener";
  g_extension_api_listener = nullptr;
}

void NWebHandlerDelegate::OnUpdateTab(
    int tab_id,
    const NWebExtensionTabUpdateProperties* update_properties) {
  if (!g_extension_api_listener) {
    LOG(ERROR) << "No web extension api listener";
    return;
  }

  if (g_extension_api_listener->OnUpdateTab) {
    LOG(INFO) << "OnUpdateTab tabId: " << tab_id;
    g_extension_api_listener->OnUpdateTab(tab_id, update_properties);
    return;
  }

  // Compatible with older versions
  if (g_extension_api_listener->OnUpdateTabUrl) {
    LOG(INFO) << "OnUpdateTabUrl tabId: " << tab_id;
    if (!update_properties->url) {
      LOG(ERROR) << "OnUpdateTabUrl not has url";
      return;
    }
    g_extension_api_listener->OnUpdateTabUrl(
          tab_id,
          update_properties->url.value().c_str());
    return;
  }

  LOG(ERROR) << "g_extension_api_listener OnUpdateTab is nullptr";
}

// static
bool NWebHandlerDelegate::OnCreateTab(const NWebTabCreateInfo& create_info,
                                      TabCreatedCallback callback) {
  static int request_id = 0;
  if (!g_extension_api_listener) {
    LOG(ERROR) << "No web extension api listener";
    return false;
  }

  if (!g_extension_api_listener->OnCreateTab) {
    LOG(ERROR) << "g_extension_api_listener OnCreateTab is nullptr";
    return false;
  }

  request_id++;
  LOG(INFO) << "OnCreateTab";
  g_tab_created_map_[request_id] = std::move(callback);
  g_extension_api_listener->OnCreateTab(create_info, request_id);
  return true;
}

void NWebHandlerDelegate::WebExtensionTabCreateCallback(int request_id,
                                                        const NWebExtensionTab* tab) {
  if (g_tab_created_map_.count(request_id)) {
    std::move(g_tab_created_map_[request_id]).Run(tab);
    g_tab_created_map_.erase(request_id);
  }
}

bool NWebHandlerDelegate::HasExtensionListener() {
  return !!g_extension_api_listener;
}
#endif  // BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)

void NWebHandlerDelegate::OnRequestOpenDevTools() {
  if (!web_app_client_extension_listener_) {
    LOG(WARNING) << "OnRequestOpenDevTools failed, no listener";
    return;
  }
  if (!web_app_client_extension_listener_->OnRequestOpenDevTools) {
    LOG(WARNING) << "OnRequestOpenDevTools failed, no function";
    return;
  }
  web_app_client_extension_listener_->OnRequestOpenDevTools(
      web_app_client_extension_listener_->nweb_id);
}

#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
void NWebHandlerDelegate::OnActivateContent() {
  if (nweb_handler_) {
    nweb_handler_->OnActivateContentByJS();
  }
  if (web_app_client_extension_listener_ == nullptr ||
      web_app_client_extension_listener_->OnActivateContent == nullptr) {
    LOG(ERROR)
        << "The web_app_client_extension_listener onActivateContent is null";
    return;
  }
  LOG(INFO) << "Handler delegate to extension listener onActivateContent";
  web_app_client_extension_listener_->OnActivateContent(
      web_app_client_extension_listener_->nweb_id);
}
#endif

#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
void NWebHandlerDelegate::SetTransformHint(uint32_t rotation) {
  content::GpuProcessHost* host = content::GpuProcessHost::Get();
  if (main_browser_ && host && host->gpu_host()) {
    // host->gpu_host()->SetTransformHint(rotation,
    // main_browser_->GetAcceleratedWidget(false));
  }
}
#endif

#if BUILDFLAG(ARKWEB_DISATCH_BEFORE_UNLOAD)
void NWebHandlerDelegate::OnBeforeUnloadFired(CefRefPtr<CefBrowser> browser,
                                              bool proceed) {
  if (web_app_client_extension_listener_ != nullptr &&
      web_app_client_extension_listener_->OnBeforeUnloadFired != nullptr) {
    web_app_client_extension_listener_->OnBeforeUnloadFired(
        proceed, web_app_client_extension_listener_->nweb_id);
  }
}
#endif  // ARKWEB_DISATCH_BEFORE_UNLOAD

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
void NWebHandlerDelegate::OnShareFile(const std::string& file_path,
                                      const std::string& utd_type_id) {
  if (web_app_client_extension_listener_ == nullptr ||
      web_app_client_extension_listener_->OnShareFile == nullptr) {
    LOG(ERROR)
        << "NWebHandlerDelegate::OnShareFile failed for nullptr, file_path "
        << file_path;
    return;
  }
  web_app_client_extension_listener_->OnShareFile(
      file_path, utd_type_id, web_app_client_extension_listener_->nweb_id);
}
#endif

void NWebHandlerDelegate::Discard()
{
  content::GpuProcessHost* host = content::GpuProcessHost::Get();
  if (host != nullptr && host->gpu_host() != nullptr && main_browser_ != nullptr
    && main_browser_->GetHost() != nullptr) {
    host->gpu_host()->Discard(main_browser_->GetHost()->GetAcceleratedWidget(false));
  }
}

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void NWebHandlerDelegate::EnableVideoAssistant(bool enable) {
  video_assistant_enabled_ = enable;
}
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void NWebHandlerDelegate::CustomWebMediaPlayer(bool enable) {
  LOG(INFO) << "NWebHandlerDelegate::CustomWebMediaPlayer enter. enable = " << enable;
  custom_web_media_player_enabled_ = enable;
}
#endif // ARKWEB_VIDEO_ASSISTANT

void NWebHandlerDelegate::HandleSafeBrowsingDetection(int detectMode,
                                                      int detectSwitch,
                                                      const CefString& url) {
  LOG(INFO) << "begin to handle safe browsing detection,"
            << "nweb id is " << nweb_id_ << ",detect mode is " << detectMode
            << ",detect switch is " << detectSwitch;
#if BUILDFLAG(ARKWEB_SAFEBROWSING)
  NWebSafeBrowsingDetectionHandler::GetInstance().HandleSafeBrowsingDetection(
      nweb_id_, detectMode, detectSwitch, url);
#else
  OnSafeBrowsingDetectionResult(-1, -1, "", url);
#endif
}

#if BUILDFLAG(ARKWEB_SAFEBROWSING)
void NWebHandlerDelegate::OnSafeBrowsingDetectionResult(
    int code,
    int policy,
    const std::string& mappingType,
    const std::string& url) {
  LOG(INFO) << "code is " << code << ",policy is " << policy
            << ",mapping type is " << mappingType << ",nweb id is " << nweb_id_;
  if (safe_browsing_detection_callback_ == nullptr) {
    return;
  }

  safe_browsing_detection_callback_->OnDetectionResult(code, policy,
                                                       mappingType, url);
}
#endif

void NWebHandlerDelegate::SetSafeBrowsingDetectionCallback(
    CefRefPtr<CefSafeBrowsingDetectionCallback> callback) {
#if BUILDFLAG(ARKWEB_SAFEBROWSING)
  safe_browsing_detection_callback_ = callback;
#endif
}

#if BUILDFLAG(ARKWEB_PIP)
bool NWebHandlerDelegate::OnPip(CefRefPtr<CefBrowser> browser,
                                int status,
                                int delegate_id,
                                int child_id,
                                int frame_routing_id,
                                int width,
                                int height) {
  if (nweb_handler_ != nullptr) {
    LOG(INFO) << __func__ << " status:" << status << " " << delegate_id
	          << " " << child_id << " " << frame_routing_id
			  << " " << width << " " << height;
    if (status == 1) {
      pip_status_ = -1;
    }
    pip_status_ = status;
    pip_delegate_id_ = delegate_id;
    pip_child_id_ = child_id;
    pip_frame_routing_id_ = frame_routing_id;
    nweb_handler_->OnPip(status, delegate_id, child_id,
                         frame_routing_id, width, height);
    return true;
  }
  return false;
}

void NWebHandlerDelegate::OnPipEvent(CefRefPtr<CefBrowser> browser,
                                     int event) {
  LOG(INFO) << __func__ << " event:" << event;

  if (web_app_client_extension_listener_ != nullptr &&
      web_app_client_extension_listener_->OnPipEvent != nullptr) {
    web_app_client_extension_listener_->OnPipEvent(
        event, web_app_client_extension_listener_->nweb_id);
  }
}
#endif

#if BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)
void NWebHandlerDelegate::RegisterScreenCaptureDelegateListener(
    CefRefPtr<CefScreenCaptureCallback> screen_capture_cb) {
  LOG(INFO) << "NWebHandlerDelegate RegisterScreenCaptureDelegateListener enter";
  screen_capture_cb_ = screen_capture_cb;
}
#endif // ARKWEB_EX_SCREEN_CAPTURE

}  // namespace OHOS::NWeb
