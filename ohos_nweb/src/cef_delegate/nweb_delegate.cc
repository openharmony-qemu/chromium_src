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

#include "nweb_delegate.h"

#include <thread>
#include "nweb_accessibility_utils.h"
#include "nweb_application.h"
#include "nweb_handler_delegate.h"
#include "nweb_render_handler.h"
#include "nweb_value_convert.h"

#include "base/command_line.h"
#include "base/strings/utf_string_conversions.h"
#include "base/trace_event/trace_event.h"
#include "cef/include/base/cef_logging.h"
#include "cef/include/cef_app.h"
#include "cef/include/cef_base.h"
#include "cef/include/cef_request_context.h"
#include "content/public/common/content_switches.h"
#include "nweb_find_delegate.h"
#include "nweb_preference_delegate.h"
#include "url/gurl.h"
#ifdef OHOS_NAVIGATION
#include "cef/libcef/browser/navigation_state_serializer.h"
#include "nweb_history_list_impl.h"
#endif

#ifdef OHOS_DRAG_DROP
#include "nweb_drag_data_impl.h"
#endif  // #ifdef OHOS_DRAG_DROP

#if defined(OHOS_NO_STATE_PREFETCH)
#include "base/strings/stringprintf.h"
#endif  // defined(OHOS_NO_STATE_PREFETCH)

#include "cef/include/internal/cef_string_types.h"
#include "content/public/common/content_switches.h"
#include "nweb_download_handler_delegate.h"
#include "nweb_find_delegate.h"
#include "nweb_preference_delegate.h"
#include "url/gurl.h"
#include "ui/events/gesture_detection/gesture_configuration.h"

#ifdef OHOS_EX_GET_ZOOM_LEVEL
#include <cmath>
#endif

#ifdef OHOS_SECURITY_STATE
#include "components/security_state/core/security_state.h"
#endif

namespace {
static const float richtextDisplayRatio = 1.0;
}

namespace OHOS::NWeb {
#ifdef OHOS_EX_GET_ZOOM_LEVEL
static const double kZoomLevelToFactorRatio = 1.2;
#endif

#if BUILDFLAG(IS_OHOS)
namespace {

std::string FromProductDeviceType(OHOS::NWeb::ProductDeviceType deviceType) {
  switch (deviceType) {
    case OHOS::NWeb::ProductDeviceType::DEVICE_TYPE_TABLET:
      return ::switches::kOhosTabletDevice;
    case OHOS::NWeb::ProductDeviceType::DEVICE_TYPE_2IN1:
      return ::switches::kOhos2IN1Device;
    case OHOS::NWeb::ProductDeviceType::DEVICE_TYPE_MOBILE:
      return ::switches::kOhosMobileDevice;
    default:
      return ::switches::kOhosUnkownDevice;
  }
}
}
#endif

#if defined(OHOS_MSGPORT)
void ConvertCefValueToNWebMessage(CefRefPtr<CefValue> src,
                                  std::shared_ptr<NWebMessage> dst) {
  int type = src->GetType();
  LOG(DEBUG) << "OnMessage type:" << type;
  switch (type) {
    case VTYPE_STRING: {
      dst->SetType(NWebValue::Type::STRING);
      dst->SetString(src->GetString());
      break;
    }
    case VTYPE_BINARY: {
      CefRefPtr<CefBinaryValue> binValue = src->GetBinary();
      size_t len = binValue->GetSize();
      std::vector<uint8_t> arr(len);
      binValue->GetData(&arr[0], len, 0);
      dst->SetType(NWebValue::Type::BINARY);
      dst->SetBinary(arr);
      break;
    }
    case VTYPE_BOOL: {
      dst->SetType(NWebValue::Type::BOOLEAN);
      dst->SetBoolean(src->GetBool());
      break;
    }
    case VTYPE_DOUBLE: {
      dst->SetType(NWebValue::Type::DOUBLE);
      dst->SetDouble(src->GetDouble());
      break;
    }
    case VTYPE_INT: {
      dst->SetType(NWebValue::Type::INTEGER);
      dst->SetInt64(src->GetInt());
      break;
    }
    case VTYPE_DICTIONARY: {
      CefRefPtr<CefDictionaryValue> dict = src->GetDictionary();
      dst->SetType(NWebValue::Type::ERROR);
      dst->SetErrName(dict->GetString("Error.name"));
      dst->SetErrMsg(dict->GetString("Error.message"));
      break;
    }
    case VTYPE_LIST: {
      CefRefPtr<CefListValue> listValue = src->GetList();
      size_t len = listValue->GetSize();
      std::vector<std::string> string_arr;
      std::vector<bool> bool_arr;
      std::vector<double> double_arr;
      std::vector<int64_t> int64_arr;
      CefValueType elem_type;
      for (size_t i = 0; i < len; i++) {
        CefRefPtr<CefValue> elem = listValue->GetValue(i);
        if (elem->GetType() == VTYPE_STRING) {
          elem_type = VTYPE_STRING;
          string_arr.push_back(elem->GetString());
        } else if (elem->GetType() == VTYPE_BOOL) {
          elem_type = VTYPE_BOOL;
          bool_arr.push_back(elem->GetBool());
        } else if (elem->GetType() == VTYPE_DOUBLE) {
          elem_type = VTYPE_DOUBLE;
          double_arr.push_back(elem->GetDouble());
        } else if (elem->GetType() == VTYPE_INT) {
          elem_type = VTYPE_INT;
          int64_arr.push_back(elem->GetInt());
        }
      }
      if (elem_type == VTYPE_STRING) {
        dst->SetType(NWebValue::Type::STRINGARRAY);
        dst->SetStringArray(string_arr);
      } else if (elem_type == VTYPE_BOOL) {
        dst->SetType(NWebValue::Type::BOOLEANARRAY);
        dst->SetBooleanArray(bool_arr);
      } else if (elem_type == VTYPE_DOUBLE) {
        dst->SetType(NWebValue::Type::DOUBLEARRAY);
        dst->SetDoubleArray(double_arr);
      } else if (elem_type == VTYPE_INT) {
        dst->SetType(NWebValue::Type::INT64ARRAY);
        dst->SetInt64Array(int64_arr);
      }
      break;
    }
    default: {
      LOG(ERROR) << "OnMessage not support type";
      break;
    }
  }
}

class JavaScriptResultCallbackImpl : public CefJavaScriptResultCallback {
 public:
  JavaScriptResultCallbackImpl(
      std::shared_ptr<NWebValueCallback<std::shared_ptr<NWebMessage>>> callback)
      : callback_(callback) {}
  void OnJavaScriptExeResult(CefRefPtr<CefValue> result) override {
    if (callback_ != nullptr) {
      auto data =
          std::make_shared<OHOS::NWeb::NWebMessage>(NWebValue::Type::NONE);
      ConvertCefValueToNWebMessage(result, data);
      callback_->OnReceiveValue(data);
    }
  }

 private:
  std::shared_ptr<NWebValueCallback<std::shared_ptr<NWebMessage>>> callback_;

  IMPLEMENT_REFCOUNTING(JavaScriptResultCallbackImpl);
};

class CefWebMessageReceiverImpl : public CefWebMessageReceiver {
 public:
  CefWebMessageReceiverImpl(
      std::shared_ptr<NWebValueCallback<std::shared_ptr<NWebMessage>>> callback)
      : callback_(callback) {}
  void OnMessage(CefRefPtr<CefValue> message) override {
    LOG(INFO) << "OnMessage in nweb delegate";
    if (callback_ != nullptr) {
      auto data =
          std::make_shared<OHOS::NWeb::NWebMessage>(NWebValue::Type::NONE);
      ConvertCefValueToNWebMessage(message, data);
      callback_->OnReceiveValue(data);
    }
  }

 private:
  std::shared_ptr<NWebValueCallback<std::shared_ptr<NWebMessage>>> callback_;

  IMPLEMENT_REFCOUNTING(CefWebMessageReceiverImpl);
};
#endif  // defined(OHOS_MSGPORT)

class StoreWebArchiveResultCallbackImpl
    : public CefStoreWebArchiveResultCallback {
 public:
  StoreWebArchiveResultCallbackImpl(
      std::shared_ptr<NWebValueCallback<std::string>> callback)
      : callback_(callback) {}
  void OnStoreWebArchiveDone(const CefString& result) override {
    if (callback_ != nullptr) {
      callback_->OnReceiveValue(result.ToString());
    }
  }

 private:
  std::shared_ptr<NWebValueCallback<std::string>> callback_;

  IMPLEMENT_REFCOUNTING(StoreWebArchiveResultCallbackImpl);
};

class GetImagesCallbackImpl : public CefGetImagesCallback {
 public:
  explicit GetImagesCallbackImpl(
      std::shared_ptr<NWebValueCallback<bool>> callback)
      : callback_(callback) {}

  void GetImages(bool response) override {
    if (callback_ != nullptr) {
      callback_->OnReceiveValue(response);
    }
  }

 private:
  std::shared_ptr<NWebValueCallback<bool>> callback_;

  IMPLEMENT_REFCOUNTING(GetImagesCallbackImpl);
};

#ifdef OHOS_NAVIGATION
class NavigationEntryVisitorImpl : public CefNavigationEntryVisitor {
 public:
  NavigationEntryVisitorImpl()
      : history_list_(std::make_shared<NWebHistoryListImpl>()) {}

  bool Visit(CefRefPtr<CefNavigationEntry> entry,
             bool current,
             int index,
             int total) override {
    if (!history_list_) {
      return false;
    }
    if (current) {
      history_list_->SetCurrentIndex(index);
    }
    history_list_->AddHistoryItem(entry);
    return true;
  }

  std::shared_ptr<NWebHistoryListImpl> GetHistoryList() const {
    return history_list_;
  }

 private:
  std::shared_ptr<NWebHistoryListImpl> history_list_;
  IMPLEMENT_REFCOUNTING(NavigationEntryVisitorImpl);
};

#endif

#if defined(OHOS_INPUT_EVENTS)
std::set<uint32_t> NWebDelegate::focus_nweb_id_{};
#endif  // defined(OHOS_INPUT_EVENTS)

NWebDelegate::NWebDelegate(int argc, const char* argv[])
    : argc_(argc), argv_(argv) {}

NWebDelegate::~NWebDelegate() {
  if (display_listener_ != nullptr && display_manager_adapter_ != nullptr) {
    display_manager_adapter_->UnregisterDisplayListener(display_listener_);
  }
}

#ifdef OHOS_DRAG_DROP
void NWebDelegate::InitAppTempDir() {
  for (int i = 0; i < argc_; i++) {
    if (argv_[i] == nullptr) {
      continue;
    }

    if (!strncmp(argv_[i], "--ohos-temp-dir=", strlen("--ohos-temp-dir="))) {
      const char* value = argv_[i] + strlen("--ohos-temp-dir=");
      ohos_temp_dir_ = value;
      LOG(INFO) << "DrapDrop InitAppTempDir --ohos-temp-dir=" << ohos_temp_dir_.c_str();
      return;
    }
  }

  // default temp dir
  ohos_temp_dir_ = "/data/storage/el2/base/haps/entry/temp";
}
#endif

bool NWebDelegate::HasBackgroundColorWithInit(int32_t& backgroundColor) {
  for (int i = 0; i < argc_; i++) {
    if (argv_[i] == nullptr) {
      continue;
    }

    if (!strncmp(argv_[i], "--init-background-color=",
                 strlen("--init-background-color="))) {
      const char* value = argv_[i] + strlen("--init-background-color=");
      backgroundColor = atoi(value);
      LOG(INFO) << "HasBackgroundColorWithInit, background color = "
                << backgroundColor;
      return true;
    }
  }

  return false;
}

void NWebDelegate::InitRichtextIdentifier() {
  for (int i = 0; i < argc_; i++) {
    if (argv_[i] == nullptr) {
      continue;
    }
 
    if (!strncmp(argv_[i], "--init-richtext-data=", strlen("--init-richtext-data="))) {
      const char* value = argv_[i] + strlen("--init-richtext-data=");
      richtext_data_str_ = value;
      LOG(INFO) << "richtext InitRichtextIdentifier data" << richtext_data_str_.c_str();
    }
  }
}


bool NWebDelegate::Init(bool is_enhance_surface,
                        void* window,
                        bool popup
#if defined(OHOS_EX_DOWNLOAD)
                        , uint32_t nweb_id
#endif
#if defined(OHOS_INCOGNITO_MODE)
                        , bool incognito_mode
#endif
                        ) {
  preference_delegate_ = std::make_shared<NWebPreferenceDelegate>();
  int32_t backgroundColor;
  if (preference_delegate_ && HasBackgroundColorWithInit(backgroundColor)) {
    // background color should set when init in case of first white screen flash
    preference_delegate_->SetBackgroundColor(backgroundColor);
  }
#ifdef OHOS_DRAG_DROP
  InitAppTempDir();
#endif
InitRichtextIdentifier();

  find_delegate_ = std::make_shared<NWebFindDelegate>();
  is_enhance_surface_ = is_enhance_surface;
  display_manager_adapter_ =
      OhosAdapterHelper::GetInstance().CreateDisplayMgrAdapter();
  if (display_manager_adapter_ == nullptr) {
    return false;
  }

  display_listener_ =
      std::make_shared<DisplayScreenListener>(shared_from_this());
  if (display_listener_ == nullptr) {
    return false;
  }

  if (!display_manager_adapter_->RegisterDisplayListener(display_listener_)) {
    LOG(ERROR) << "RegisterDisplayListener failed";
  }

  render_handler_ = NWebRenderHandler::Create();
  if (render_handler_ == nullptr) {
    display_manager_adapter_->UnregisterDisplayListener(display_listener_);
    return false;
  }

  event_handler_ = NWebEventHandler::Create();
  if (event_handler_ == nullptr) {
    display_manager_adapter_->UnregisterDisplayListener(display_listener_);
    return false;
  }

  std::string url_for_init = "";

  LOG(INFO) << "NWebDelegate::Init incognito_mode:" << incognito_mode;
  InitializeCef(url_for_init, is_enhance_surface_, window, popup
#if defined(OHOS_EX_DOWNLOAD)
      , nweb_id
#endif
#if defined(OHOS_INCOGNITO_MODE)
      , incognito_mode
#endif
      );
  std::shared_ptr<DisplayAdapter> display =
      display_manager_adapter_->GetDefaultDisplay();
  if (display != nullptr) {
    NotifyScreenInfoChanged(display->GetRotation(), display->GetOrientation());
    if (!richtext_data_str_.empty()) {
      // Created a richtext component
      SetVirtualPixelRatio(richtextDisplayRatio);
    } else {
      SetVirtualPixelRatio(display->GetVirtualPixelRatio());
    }
  }

  return true;
}

void NWebDelegate::OnDestroy(bool is_close_all) {
  if (display_listener_ != nullptr && display_manager_adapter_ != nullptr) {
    display_manager_adapter_->UnregisterDisplayListener(display_listener_);
  }
  if (handler_delegate_ != nullptr) {
    handler_delegate_->OnDestroy();
  }
  if (preference_delegate_ != nullptr) {
    preference_delegate_->OnDestroy();
  }
  if (!GetBrowser().get()) {
    return;
  }
  GetBrowser()->GetHost()->DestroyAllWebMessagePorts();
}

void NWebDelegate::RegisterDownLoadListener(
    std::shared_ptr<NWebDownloadCallback> download_listener) {
  if (handler_delegate_ == nullptr) {
    LOG(ERROR) << "fail to register download listener, NWEB handler is nullptr";
    return;
  }
  handler_delegate_->RegisterDownLoadListener(download_listener);
}

void NWebDelegate::RegisterReleaseSurfaceListener(
    std::shared_ptr<NWebReleaseSurfaceCallback> releaseSurfaceListener) {
  if (handler_delegate_ == nullptr) {
    LOG(ERROR) << "fail to register release surface, NWEB handler is nullptr";
    return;
  }
  handler_delegate_->RegisterReleaseSurfaceListener(releaseSurfaceListener);
}

void NWebDelegate::RegisterFindListener(
    std::shared_ptr<NWebFindCallback> find_listener) {
  if (find_delegate_ == nullptr) {
    LOG(ERROR) << "fail to register FindListener, find_delegate_ is nullptr";
    return;
  }
  find_delegate_->SetListener(find_listener);
}

void NWebDelegate::RegisterWebDownloadDelegateListener(
    std::shared_ptr<NWebDownloadDelegateCallback> downloadDelegateListener) {
  if (handler_delegate_ == nullptr) {
    LOG(ERROR) << "fail to register download delegate listener, NWEB handler "
                  "is nullptr";
    return;
  }
  CefRefPtr<NWebDownloadHandlerDelegate> delegate =
      new NWebDownloadHandlerDelegate(preference_delegate_);
  delegate->RegisterWebDownloadDelegateListener(downloadDelegateListener);
  CefSetDownloadHandler(delegate);
}

void NWebDelegate::StartDownload(const char* url) {
  if (handler_delegate_ == nullptr) {
    LOG(ERROR) << "fail to start download, NWEB handler is nullptr";
    return;
  }
  auto browser = GetBrowser();
  if (browser != nullptr && browser->GetHost() != nullptr) {
    browser->GetHost()->StartDownload(url);
  }
}

void NWebDelegate::ResumeDownload(
    std::shared_ptr<NWebDownloadItem> web_download) {
  LOG(DEBUG) << "NWebDelegate::ResumeDownload";
  auto browser = GetBrowser();

  if (web_download == nullptr) {
    LOG(ERROR) << "fail to resume download, NWEB DownloadItem is nullptr";
    return;
  }
  if (browser != nullptr && browser->GetHost() != nullptr) {
    browser->GetHost()->ResumeDownload(
        web_download->url, web_download->full_path,
        web_download->received_bytes, web_download->total_bytes,
        web_download->etag, web_download->mime_type,
        web_download->last_modified, web_download->received_slices);
  }
}

void NWebDelegate::FindAllAsync(const std::string& search_string) const {
  if (find_delegate_ == nullptr) {
    LOG(ERROR) << "fail to FindAllAsync, find_delegate_ is nullptr";
    return;
  }
  find_delegate_->FindAllAsync(GetBrowser().get(),
                               base::UTF8ToUTF16(search_string));
}

void NWebDelegate::ClearMatches() const {
  if (find_delegate_ == nullptr) {
    LOG(ERROR) << "fail to ClearMatches, find_delegate_ is nullptr";
    return;
  }

  find_delegate_->ClearMatches(GetBrowser().get());
}

void NWebDelegate::FindNext(const bool forward) const {
  if (find_delegate_ == nullptr) {
    LOG(ERROR) << "fail to FindNext, find_delegate_ is nullptr";
    return;
  }
  find_delegate_->FindNext(GetBrowser().get(), forward);
}

void NWebDelegate::RegisterWebAppClientExtensionListener(
    std::shared_ptr<NWebAppClientExtensionCallback>
        web_app_client_extension_listener) {
  if (handler_delegate_ == nullptr) {
    LOG(ERROR) << "fail to register web app client extension listener, nweb "
                  "handler delegate is nullptr";
    return;
  }
  handler_delegate_->RegisterWebAppClientExtensionListener(
      web_app_client_extension_listener);
}

#if defined(OHOS_NWEB_EX)
bool NWebDelegate::CanStoreWebArchive() {
  if (!GetBrowser().get()) {
    return false;
  }

  return GetBrowser()->CanStoreWebArchive();
}

void NWebDelegate::UnRegisterWebAppClientExtensionListener() {
  if (handler_delegate_ == nullptr) {
    LOG(ERROR) << "fail to unregister web app client extension listener, nweb "
                  "handler delegate is nullptr";
    return;
  }
  handler_delegate_->UnRegisterWebAppClientExtensionListener();
}
#endif  // defined(OHOS_NWEB_EX)

void NWebDelegate::RegisterNWebHandler(std::shared_ptr<NWebHandler> handler) {
  if (handler_delegate_ == nullptr) {
    LOG(ERROR)
        << "fail to register nweb handler, nweb handler delegate is nullptr";
    return;
  }
  handler_delegate_->RegisterNWebHandler(handler);
}

void NWebDelegate::SetInputMethodClient(
    CefRefPtr<NWebInputMethodClient> client) {
  if (render_handler_ == nullptr) {
    LOG(ERROR)
        << "fail to register inputmethod client, render handler is nullptr";
    return;
  }
  render_handler_->SetInputMethodClient(client);
}

void NWebDelegate::RegisterRenderCb(
    std::function<void(const char*)> render_update_cb) {
  if (render_handler_ != nullptr) {
    render_handler_->RegisterRenderCb(render_update_cb);
  }
}

void NWebDelegate::Resize(uint32_t width, uint32_t height, bool isKeyboard) {
#if defined(OHOS_COMPOSITE_RENDER)
  if (width == width_ && height == height_) {
    render_handler_->OnResizeNotWork();
    return;
  }

  TRACE_EVENT2("base", "NWebDelegate::Resize", "width", width, "height",
               height);
  width_ = width;
  height_ = height;
#endif  // defined(OHOS_COMPOSITE_RENDER)

  if (render_handler_ != nullptr) {
    render_handler_->Resize(width, height);
  }

  auto browser = GetBrowser();
  if (browser != nullptr && browser->GetHost() != nullptr) {
    if (isKeyboard) {
      browser->GetHost()->WasKeyboardResized();
    } else {
      browser->GetHost()->WasResized();
    }
  }
}

void NWebDelegate::OnTouchPress(int32_t id,
                                double x,
                                double y,
                                bool from_overlay) {
  if (event_handler_ != nullptr) {
    auto browser = GetBrowser();
    if (browser != nullptr && browser->GetHost() != nullptr) {
      browser->GetHost()->SetFocus(true);
    }
    event_handler_->OnTouchPress(id, x / default_virtual_pixel_ratio_,
                                 y / default_virtual_pixel_ratio_,
                                 from_overlay);
  }
#ifdef OHOS_DRAG_DROP
  if (render_handler_ != nullptr) {
    render_handler_->SetIrregularDragBackground(true);
  }
#endif  // #ifdef OHOS_DRAG_DROP
}

void NWebDelegate::OnTouchRelease(int32_t id,
                                  double x,
                                  double y,
                                  bool from_overlay) {
  if (event_handler_ != nullptr) {
    event_handler_->OnTouchRelease(id, x / default_virtual_pixel_ratio_,
                                   y / default_virtual_pixel_ratio_,
                                   from_overlay);
  }
}

void NWebDelegate::OnTouchMove(int32_t id,
                               double x,
                               double y,
                               bool from_overlay) {
  if (event_handler_ != nullptr) {
    event_handler_->OnTouchMove(id, x / default_virtual_pixel_ratio_,
                                y / default_virtual_pixel_ratio_, from_overlay);
  }
}

void NWebDelegate::OnTouchCancel() {
  if (event_handler_ != nullptr) {
    event_handler_->OnTouchCancel();
  }
}

bool NWebDelegate::SendKeyEvent(int32_t keyCode, int32_t keyAction) {
  bool retVal = false;
  if (event_handler_ != nullptr) {
    retVal = event_handler_->SendKeyEventFromAce(keyCode, keyAction);
  }
  return retVal;
}

void NWebDelegate::SendMouseWheelEvent(double x,
                                       double y,
                                       double deltaX,
                                       double deltaY) {
  if (event_handler_ != nullptr) {
    event_handler_->SendMouseWheelEvent(x / default_virtual_pixel_ratio_,
                                        y / default_virtual_pixel_ratio_,
                                        deltaX / default_virtual_pixel_ratio_,
                                        deltaY / default_virtual_pixel_ratio_);
  }
}

void NWebDelegate::SendMouseEvent(int x,
                                  int y,
                                  int button,
                                  int action,
                                  int count) {
  if (event_handler_ != nullptr) {
    event_handler_->SendMouseEvent(x / default_virtual_pixel_ratio_,
                                   y / default_virtual_pixel_ratio_, button,
                                   action, count);
  }
#ifdef OHOS_DRAG_DROP
  if (render_handler_ != nullptr) {
    render_handler_->SetIrregularDragBackground(true);
  }
#endif  // #ifdef OHOS_DRAG_DROP
  if (action == MouseAction::MOVE) {
    auto* accessibilityManager = GetAccessibilityManager();
    if (accessibilityManager != nullptr) {
      gfx::PointF point(x / default_virtual_pixel_ratio_,
                        y / default_virtual_pixel_ratio_);
      accessibilityManager->OnHoverEvent(point);
    }
  }
}

void NWebDelegate::NotifyScreenInfoChanged(RotationType rotation,
                                           OrientationType orientation) {
  if (render_handler_ != nullptr) {
    if (display_manager_adapter_ == nullptr) {
      LOG(ERROR) << "Get display_manager_adapter_ failed";
      return;
    }
    std::shared_ptr<DisplayAdapter> display =
        display_manager_adapter_->GetDefaultDisplay();
    if (display == nullptr) {
      LOG(ERROR) << "Get display failed";
      return;
    }
    double display_ratio = 0.0;
    if (!richtext_data_str_.empty()) {
      // Created a richtext component
      display_ratio = richtextDisplayRatio;
    } else {
      display_ratio = display->GetVirtualPixelRatio();
    }
    if (display_ratio <= 0) {
      LOG(ERROR) << "Invalid display_ratio, display_ratio = " << display_ratio;
      return;
    }
    int width = display->GetWidth() / display_ratio;
    int height = display->GetHeight() / display_ratio;
#ifdef OHOS_SCREEN_ROTATION
    bool default_portrait = display_manager_adapter_->IsDefaultPortrait();
    render_handler_->SetScreenInfo({rotation, orientation, width, height,
                                    display_ratio, default_portrait});
#endif  // #ifdef OHOS_SCREEN_ROTATION
    auto browser = GetBrowser();
    if (browser != nullptr && browser->GetHost() != nullptr) {
      browser->GetHost()->NotifyScreenInfoChanged();
    }
  }
}

void NWebDelegate::SetVirtualPixelRatio(float ratio) {
  if (ratio <= 0 || ratio == default_virtual_pixel_ratio_) {
    return;
  }
  default_virtual_pixel_ratio_ = ratio;
  auto browser = GetBrowser();
  if (browser != nullptr && browser->GetHost() != nullptr) {
    browser->GetHost()->SetVirtualPixelRatio(ratio);
  }
  ui::GestureConfiguration::GetInstance()->set_virtual_pixel_ratio(default_virtual_pixel_ratio_);
}

std::shared_ptr<NWebPreference> NWebDelegate::GetPreference() const {
  return preference_delegate_;
}

int NWebDelegate::Load(const std::string& url) {
  GURL gurl = GURL(url);
  if (gurl.is_empty() || !gurl.is_valid()) {
    GURL gurlWithHttp = GURL("https://" + url);
    if (!gurlWithHttp.is_valid()) {
      return NWEB_INVALID_URL;
    }
  }
  LOG(DEBUG) << "NWebDelegate::Load url=" << url;
  auto browser = GetBrowser();
  if (browser == nullptr) {
    LOG(ERROR) << "NWebDelegate::Load browser is nullptr";
    return NWEB_ERR;
  }
  browser->GetMainFrame()->LoadURL(CefString(url));
  RequestVisitedHistory();
  return NWEB_OK;
}

#ifdef OHOS_POST_URL
int NWebDelegate::PostUrl(const std::string& url, std::vector<char>& postData) {
  GURL gurl = GURL(url);
  if (gurl.is_empty() || !gurl.is_valid()) {
    GURL gurlWithHttp = GURL("https://" + url);
    if (!gurlWithHttp.is_valid()) {
      return NWEB_INVALID_URL;
    }
  }
  LOG(DEBUG) << "NWebDelegate::PostUrl url=" << url;
  auto browser = GetBrowser();
  if (browser == nullptr) {
    LOG(ERROR) << "NWebDelegate::PostUrl browser is nullptr";
    return NWEB_ERR;
  }
  browser->GetMainFrame()->PostURL(CefString(url), postData);
  RequestVisitedHistory();
  return NWEB_OK;
}
#endif // defined(OHOS_POST_URL)


bool NWebDelegate::IsNavigatebackwardAllowed() const {
  LOG(DEBUG) << "NWebDelegate::IsNavigatebackwardAllowed";
  if (GetBrowser().get()) {
    return GetBrowser()->CanGoBack();
  }
  return false;
}

bool NWebDelegate::IsNavigateForwardAllowed() const {
  LOG(DEBUG) << "NWebDelegate::IsNavigateForwardAllowed";
  if (GetBrowser().get()) {
    return GetBrowser()->CanGoForward();
  }
  return false;
}

bool NWebDelegate::CanNavigateBackOrForward(int num_steps) const {
  LOG(DEBUG) << "NWebDelegate::CanNavigateBackOrForward";
  if (GetBrowser().get()) {
    return GetBrowser()->CanGoBackOrForward(num_steps);
  }
  return false;
}

void NWebDelegate::NavigateBack() const {
  LOG(DEBUG) << "NWebDelegate::NavigateBack";
  if (GetBrowser().get()) {
    GetBrowser()->GoBack();
  }
}

void NWebDelegate::NavigateForward() const {
  LOG(DEBUG) << "NWebDelegate::NavigateForward";
  if (GetBrowser().get()) {
    GetBrowser()->GoForward();
  }
}

void NWebDelegate::NavigateBackOrForward(int step) const {
  LOG(DEBUG) << "NWebDelegate::NavigateBackOrForward";
  if (GetBrowser().get()) {
    GetBrowser()->GoBackOrForward(step);
  }
}

void NWebDelegate::DeleteNavigateHistory() {
  LOG(DEBUG) << "NWebDelegate::DeleteNavigateHistory";
  if (GetBrowser().get()) {
    GetBrowser()->DeleteHistory();
  }
}

void NWebDelegate::ClearSslCache() {
  LOG(DEBUG) << "NWebDelegate::ClearSslCache";
  CefRefPtr<CefRequestContext> context = CefRequestContext::GetGlobalContext();
  if (context != nullptr) {
    context->ClearCertificateExceptions(nullptr);
  }
}

void NWebDelegate::ClearClientAuthenticationCache() {
  LOG(DEBUG) << "NWebDelegate::ClearClientAuthenticationCache";
  CefRefPtr<CefRequestContext> context = CefRequestContext::GetGlobalContext();
  if (context != nullptr) {
    context->ClearClientAuthenticationCache(nullptr);
  }
}

void NWebDelegate::Reload() const {
  LOG(DEBUG) << "NWebDelegate::Reload";
  if (GetBrowser().get()) {
    GetBrowser()->Reload();
  }
}

void NWebDelegate::ReloadOriginalUrl() const {
  LOG(DEBUG) << "NWebDelegate::ReloadOriginalUrl";
  if (GetBrowser().get()) {
    GetBrowser()->ReloadOriginalUrl();
  }
}

const std::string NWebDelegate::GetOriginalUrl() {
  LOG(DEBUG) << "NWebDelegate::GetOriginalUrl";
  if (GetBrowser().get()) {
    return GetBrowser()->GetHost()->GetOriginalUrl();
  }
  return std::string();
}

bool NWebDelegate::GetFavicon(const void** data,
                              size_t& width,
                              size_t& height,
                              ImageColorType& colorType,
                              ImageAlphaType& alphaType) {
  LOG(DEBUG) << "NWebDelegate::getFavicon";
  if (handler_delegate_) {
    return handler_delegate_->GetFavicon(data, width, height, colorType,
                                         alphaType);
  } else {
    LOG(ERROR) << "handler_delegate_ is null";
    return false;
  }
}

void NWebDelegate::PutNetworkAvailable(bool avaiable) {
  LOG(DEBUG) << "NWebDelegate::PutNetworkAvailable";
  if (GetBrowser().get()) {
    GetBrowser()->GetHost()->PutNetworkAvailable(avaiable);
  }
}

void NWebDelegate::SetBrowserUserAgentString(const std::string& user_agent) {
  LOG(DEBUG) << "NWebDelegate::SetBrowserUserAgentString";
  if (GetBrowser().get()) {
    GetBrowser()->SetBrowserUserAgentString(user_agent);
  }
}

void NWebDelegate::SetWindowId(uint32_t window_id) {
  if (handler_delegate_) {
    handler_delegate_->SetWindowId(window_id);
  }

  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "SetWindowId failed, can not get browser";
    return;
  }

  GetBrowser()->GetHost()->SetWindowId(window_id, nweb_id_);
}

void NWebDelegate::StoreWebArchive(
    const std::string& base_name,
    bool auto_name,
    std::shared_ptr<NWebValueCallback<std::string>> callback) const {
  if (GetBrowser().get()) {
    CefRefPtr<StoreWebArchiveResultCallbackImpl> save_webarchive_callback =
        new StoreWebArchiveResultCallbackImpl(callback);
    GetBrowser()->GetHost()->StoreWebArchive(base_name, auto_name,
                                             save_webarchive_callback);
  }
}

int NWebDelegate::Zoom(float zoomFactor) const {
  LOG(DEBUG) << "NWebDelegate::Zoom";
  if (!preference_delegate_) {
    LOG(ERROR) << "preference_delegate_ get fail";
    return NWEB_ERR;
  }
  if (!preference_delegate_->ZoomingfunctionEnabled()) {
    return NWEB_FUNCTION_NOT_ENABLE;
  }
  if (!GetBrowser().get()) {
    LOG(ERROR) << "JSAPI Zoom can not get browser";
    return NWEB_ERR;
  }
  GetBrowser()->GetHost()->ZoomBy(zoomFactor,
                                  width_ / default_virtual_pixel_ratio_,
                                  height_ / default_virtual_pixel_ratio_);
  return NWEB_OK;
}

int NWebDelegate::ZoomIn() const {
  LOG(DEBUG) << "NWebDelegate::ZoomIn";
  if (!preference_delegate_) {
    return NWEB_ERR;
  }
  if (!preference_delegate_->ZoomingfunctionEnabled()) {
    return NWEB_FUNCTION_NOT_ENABLE;
  }
  if (!GetBrowser().get()) {
    LOG(ERROR) << "JSAPI ZoomIn can not get browser";
    return NWEB_ERR;
  }
  GetBrowser()->GetHost()->ZoomBy(zoom_in_factor_,
                                  width_ / default_virtual_pixel_ratio_,
                                  height_ / default_virtual_pixel_ratio_);
  return NWEB_OK;
}

int NWebDelegate::ZoomOut() const {
  LOG(DEBUG) << "NWebDelegate::ZoomOut";
  if (!preference_delegate_) {
    return NWEB_ERR;
  }
  if (!preference_delegate_->ZoomingfunctionEnabled()) {
    return NWEB_FUNCTION_NOT_ENABLE;
  }
  if (!GetBrowser().get()) {
    LOG(ERROR) << "JSAPI ZoomOut can not get browser";
    return NWEB_ERR;
  }
  GetBrowser()->GetHost()->ZoomBy(zoom_out_factor_,
                                  width_ / default_virtual_pixel_ratio_,
                                  height_ / default_virtual_pixel_ratio_);
  return NWEB_OK;
}

bool NWebDelegate::SetZoomInFactor(float factor) {
  LOG(DEBUG) << "NWebDelegate::SetZoomInFactor";
  if (factor <= 0) {
    return false;
  }
  zoom_in_factor_ = factor;
  return true;
}

bool NWebDelegate::SetZoomOutFactor(float factor) {
  LOG(DEBUG) << "NWebDelegate::SetZoomOutFactor";
  if (factor >= 0) {
    return false;
  }
  zoom_out_factor_ = factor;
  return true;
}

void NWebDelegate::Stop() const {
  LOG(DEBUG) << "NWebDelegate::Stop";
  if (GetBrowser().get()) {
    GetBrowser()->StopLoad();
  }
}

void NWebDelegate::ExecuteJavaScript(const std::string& code) const {
  LOG(DEBUG) << "NWebDelegate::ExecuteJavaScript";
  if (GetBrowser().get()) {
    GetBrowser()->GetMainFrame()->ExecuteJavaScript(
        code, GetBrowser()->GetMainFrame()->GetURL(), 0);
  }
}

#if defined(OHOS_MSGPORT)
void NWebDelegate::ExecuteJavaScript(
    const std::string& code,
    std::shared_ptr<NWebValueCallback<std::shared_ptr<NWebMessage>>> callback,
    bool extention) const {
  LOG(DEBUG) << "NWebDelegate::ExecuteJavaScript with callback";

  if (GetBrowser().get()) {
    CefRefPtr<JavaScriptResultCallbackImpl> JsResultCb =
        new JavaScriptResultCallbackImpl(callback);
    GetBrowser()->GetHost()->ExecuteJavaScript(code, JsResultCb, extention);
  }
}
#endif  // defined(OHOS_MSGPORT)

void NWebDelegate::PutBackgroundColor(int color) const {
  LOG(DEBUG) << "NWebDelegate::PutBackgroundColor color: " << (uint32_t)color;
  if (GetBrowser().get()) {
    GetBrowser()->GetHost()->SetBackgroundColor(color);
  }

#if defined(OHOS_BACKGROUND_COLOR)
  if (preference_delegate_) {
    preference_delegate_->SetBackgroundColor(color);
  }
#endif  // defined(OHOS_BACKGROUND_COLOR)
}

void NWebDelegate::InitialScale(float scale) const {
  LOG(DEBUG) << "NWebDelegate::InitialScale";
  if (scale == intial_scale_ || !render_handler_) {
    return;
  }
  float ratio = render_handler_->GetVirtualPixelRatio();
  if (GetBrowser().get()) {
    GetBrowser()->GetHost()->SetInitialScale(scale / ratio);
  }
}

void NWebDelegate::OnPause() {
  LOG(DEBUG) << "NWebDelegate::OnPause, nweb_id = " << nweb_id_;
  if (!GetBrowser().get()) {
    return;
  }

  // Remove focus from the browser.
  GetBrowser()->GetHost()->SetFocus(false);

#if defined(OHOS_INPUT_EVENTS)
  if (handler_delegate_) {
    handler_delegate_->SetFocusState(false);
  }
  if (focus_nweb_id_.find(nweb_id_) != focus_nweb_id_.end()) {
    focus_nweb_id_.erase(nweb_id_);
  }
#endif  // defined(OHOS_INPUT_EVENTS)

  if (!hidden_) {
    // Set the browser as hidden.
    LOG(DEBUG) << "NWebDelegate::OnPause set hidden, nweb_id = " << nweb_id_;
    GetBrowser()->GetHost()->WasHidden(true);
    hidden_ = true;
  }

#if defined(OHOS_INPUT_EVENTS)
  is_onPause_ = true;
#endif  // defined(OHOS_INPUT_EVENTS)
}

void NWebDelegate::OnContinue() {
  LOG(DEBUG) << "NWebDelegate::OnContinue, nweb_id = " << nweb_id_;
  if (!GetBrowser().get()) {
    return;
  }

  if (occluded_) {
    LOG(DEBUG) << "NWebDelegate::OnContinue set occluded, nweb_id = " << nweb_id_;
    hidden_ = false;
    GetBrowser()->GetHost()->WasOccluded(true);
    return;
  }

  if (hidden_) {
    // Set the browser as visible.
    LOG(DEBUG) << "NWebDelegate::OnContinue set unhidden, nweb_id = " << nweb_id_;
    GetBrowser()->GetHost()->WasHidden(false);
    hidden_ = false;
  }

  // Give focus to the browser.
  if (handler_delegate_ && handler_delegate_->GetContinueNeedFocus()) {
    if (is_onPause_) {
      GetBrowser()->GetHost()->SetFocus(true);
      focus_nweb_id_.insert(nweb_id_);
      handler_delegate_->SetContinueNeedFocus(false);
    }
  }
  is_onPause_ = false;
}

void NWebDelegate::OnOccluded() {
  LOG(DEBUG) << "NWebDelegate::OnOccluded, nweb_id = " << nweb_id_;
  if (!GetBrowser().get()) {
    return;
  }

  if (!hidden_ && !occluded_) {
    // Set the browser as occluded.
    LOG(DEBUG) << "NWebDelegate::OnOccluded set occluded, nweb_id = " << nweb_id_;
    GetBrowser()->GetHost()->WasOccluded(true);
  }
  occluded_ = true;
}

void NWebDelegate::OnUnoccluded() {
  LOG(DEBUG) << "NWebDelegate::OnUnoccluded, nweb_id = " << nweb_id_;
  if (!GetBrowser().get()) {
    return;
  }

  if (!hidden_ && occluded_) {
    // Set the browser as visible.
    LOG(DEBUG) << "NWebDelegate::OnUnoccluded set unoccluded, nweb_id = " << nweb_id_;
    GetBrowser()->GetHost()->WasOccluded(false);
  }
  occluded_ = false;
}

void NWebDelegate::SetEnableLowerFrameRate(bool enabled) {
  LOG(DEBUG) << "NWebDelegate::SetEnableLowerFrameRate, nweb_id = " << nweb_id_;
  if (!GetBrowser().get()) {
    return;
  }

  GetBrowser()->GetHost()->SetEnableLowerFrameRate(enabled);
}


void NWebDelegate::InitializeCef(std::string url,
                                 bool is_enhance_surface,
                                 void* window,
                                 bool popup
#if defined(OHOS_EX_DOWNLOAD)
                                 , uint32_t nweb_id
#endif
#if defined(OHOS_INCOGNITO_MODE)
                                 , bool incognito_mode
#endif
                                ) {
  if (popup) {
    LOG(DEBUG) << "pop windows";
    handler_delegate_ = NWebHandlerDelegate::Create(
        preference_delegate_, render_handler_, event_handler_, find_delegate_,
        is_enhance_surface, window);
    is_popup_ready_ = true;
    return;
  }
  handler_delegate_ = NWebHandlerDelegate::Create(
      preference_delegate_, render_handler_, event_handler_, find_delegate_,
      is_enhance_surface, window);

#if defined(OHOS_EX_DOWNLOAD)
  handler_delegate_->SetNWebId(nweb_id);
#endif

#ifdef OHOS_HAP_DECOMPRESSED
  CefMainArgs mainargs(argc_, const_cast<char**>(argv_));
  base::CommandLine::StringVector argv;
  for (int i = 0; i < argc_; i++) {
    argv.push_back(argv_[i]);
  }
  if (base::CommandLine::ForCurrentProcess()) {
    base::CommandLine cl(argv);
    base::CommandLine::ForCurrentProcess()->AppendArguments(cl, false);
  }
#endif

  CefSettings settings;
  settings.windowless_rendering_enabled = true;
  settings.log_severity = LOGSEVERITY_INFO;
  settings.multi_threaded_message_loop = false;
  auto& system_properties_adapter =
        OHOS::NWeb::OhosAdapterHelper::GetInstance()
            .GetSystemPropertiesInstance();
  OHOS::NWeb::ProductDeviceType deviceType =
      system_properties_adapter.GetProductDeviceType();
  bool is_pc_device =
      deviceType == OHOS::NWeb::ProductDeviceType::DEVICE_TYPE_TABLET ||
      deviceType == OHOS::NWeb::ProductDeviceType::DEVICE_TYPE_2IN1;
  settings.persist_session_cookies = !is_pc_device;

#if BUILDFLAG(IS_OHOS)
  if (base::CommandLine::ForCurrentProcess()) {
    base::CommandLine::ForCurrentProcess()->AppendSwitchASCII(
        ::switches::kOhosDeviceType, FromProductDeviceType(deviceType));
  }
#endif

#if !defined(CEF_USE_SANDBOX)
  settings.no_sandbox = true;
#endif

#if defined(OHOS_API_INIT_WEB_ENGINE)
#if defined(OHOS_INCOGNITO_MODE)
  settings.incognito_mode = incognito_mode;
#endif

  bool is_initialized = NWebApplication::GetDefault()->HasInitializedCef();
  if (is_initialized) {
    NWebApplication::GetDefault()->CreateBrowser(preference_delegate_, url,
                                                 handler_delegate_, window
#if defined(OHOS_INCOGNITO_MODE)
                                                 , incognito_mode
#endif
                                                 );
  } else {
    // Create browser when context initialized.
    NWebApplication::GetDefault()->RunAfterContextInitialized(
        base::BindOnce(&NWebDelegate::OnContextInitializeComplete,
                       base::Unretained(this), url, window
#if defined(OHOS_INCOGNITO_MODE)
                       , incognito_mode
#endif
                       ));
    NWebApplication::GetDefault()->InitializeCef(mainargs, settings);
  }
#endif  // defined(OHOS_API_INIT_WEB_ENGINE)
}

void NWebDelegate::RunMessageLoop() {
  // Run the CEF message loop.
  // This will block until CefQuitMessageLoop() is called.
  CefRunMessageLoop();
}

std::string NWebDelegate::Title() {
  if (!GetBrowser().get()) {
    return "";
  }
  return GetBrowser()->GetHost()->Title();
}

#if defined(OHOS_MSGPORT)
void NWebDelegate::CreateWebMessagePorts(std::vector<std::string>& ports) {
  if (!GetBrowser().get()) {
    LOG(ERROR) << "JSAPI CreateWebMessagePorts can not get browser";
    return;
  }
  std::vector<CefString> cefPorts;
  GetBrowser()->GetHost()->CreateWebMessagePorts(cefPorts);

  for (CefString port : cefPorts) {
    ports.push_back(port.ToString());
  }
}

void NWebDelegate::PostWebMessage(std::string& message,
                                  std::vector<std::string>& ports,
                                  std::string& targetUri) {
  if (!GetBrowser().get()) {
    LOG(ERROR) << "JSAPI PostWebMessage can not get browser";
    return;
  }

  std::vector<CefString> cefPorts;
  for (std::string port : ports) {
    CefString portCef;
    portCef.FromString(port);
    cefPorts.push_back(portCef);
  }

  CefString msgCef;
  msgCef.FromString(message);
  CefString uri;
  uri.FromString(targetUri);

  GetBrowser()->GetHost()->PostWebMessage(msgCef, cefPorts, uri);
}

void NWebDelegate::ClosePort(std::string& portHandle) {
  if (!GetBrowser().get()) {
    LOG(ERROR) << "JSAPI ClosePort can not get browser";
    return;
  }
  CefString handleCef;
  handleCef.FromString(portHandle);

  GetBrowser()->GetHost()->ClosePort(handleCef);
}

void NWebDelegate::ConvertNWebMsgToCefValue(std::shared_ptr<NWebMessage> data,
                                            CefRefPtr<CefValue> message) {
  switch (data->GetType()) {
    case NWebValue::Type::STRING: {
      message->SetString(data->GetString());
      break;
    }
    case NWebValue::Type::BINARY: {
      std::vector<uint8_t> vecBinary = data->GetBinary();
      CefRefPtr<CefBinaryValue> value =
          CefBinaryValue::Create(vecBinary.data(), vecBinary.size());
      message->SetBinary(value);
      break;
    }
    case NWebValue::Type::BOOLEAN: {
      message->SetBool(data->GetBoolean());
      break;
    }

    case NWebValue::Type::DOUBLE: {
      message->SetDouble(data->GetDouble());
      break;
    }

    case NWebValue::Type::INTEGER: {
      message->SetInt(data->GetInt64());
      break;
    }

    case NWebValue::Type::STRINGARRAY: {
      CefRefPtr<CefListValue> value = CefListValue::Create();
      for (size_t i = 0; i < data->GetStringArray().size(); i++) {
        CefString msgCef;
        msgCef.FromString(data->GetStringArray()[i]);
        value->SetString(i, msgCef);
      }
      message->SetList(value);
      break;
    }

    case NWebValue::Type::BOOLEANARRAY: {
      CefRefPtr<CefListValue> value = CefListValue::Create();
      for (size_t i = 0; i < data->GetBooleanArray().size(); i++) {
        value->SetBool(i, data->GetBooleanArray()[i]);
      }
      message->SetList(value);
      break;
    }
    case NWebValue::Type::DOUBLEARRAY: {
      CefRefPtr<CefListValue> value = CefListValue::Create();
      for (size_t i = 0; i < data->GetDoubleArray().size(); i++) {
        value->SetDouble(i, data->GetDoubleArray()[i]);
      }
      message->SetList(value);
      break;
    }

    case NWebValue::Type::INT64ARRAY: {
      CefRefPtr<CefListValue> value = CefListValue::Create();
      for (size_t i = 0; i < data->GetInt64Array().size(); i++) {
        value->SetInt(i, data->GetInt64Array()[i]);
      }
      message->SetList(value);
      break;
    }

    case NWebValue::Type::ERROR: {
      CefRefPtr<CefDictionaryValue> dict = CefDictionaryValue::Create();
      dict->SetString("Error.name", data->GetErrName());
      dict->SetString("Error.message", data->GetErrMsg());
      message->SetDictionary(dict);
      break;
    }

    default: {
      LOG(ERROR) << "PostPortMessage not support type" << (int)data->GetType();
      break;
    }
  }
}

void NWebDelegate::PostPortMessage(std::string& portHandle,
                                   std::shared_ptr<NWebMessage> data) {
  if (!GetBrowser().get()) {
    LOG(ERROR) << "JSAPI PostPortMessage can not get browser";
    return;
  }
  CefString handleCef;
  handleCef.FromString(portHandle);

  LOG(DEBUG) << "JSAPI PostPortMessage in nweb delegate";
  CefRefPtr<CefValue> message = CefValue::Create();
  ConvertNWebMsgToCefValue(data, message);

  GetBrowser()->GetHost()->PostPortMessage(handleCef, message);
}

void NWebDelegate::SetPortMessageCallback(
    std::string& portHandle,
    std::shared_ptr<NWebValueCallback<std::shared_ptr<NWebMessage>>> callback) {
  if (!GetBrowser().get()) {
    LOG(ERROR) << "JSAPI SetPortMessageCallback can not get browser";
    return;
  }
  CefRefPtr<CefWebMessageReceiver> JsResultCb =
      new CefWebMessageReceiverImpl(callback);
  CefString handleCef;
  handleCef.FromString(portHandle);
  GetBrowser()->GetHost()->SetPortMessageCallback(handleCef, JsResultCb);
}
#endif  // defined(OHOS_MSGPORT)

std::string NWebDelegate::GetUrl() const {
  LOG(DEBUG) << "NWebDelegate::get url";
  if (GetBrowser().get()) {
    auto entry = GetBrowser()->GetHost()->GetVisibleNavigationEntry();
    if (entry) {
      return entry->GetDisplayURL().ToString();
    }
  }
  return "";
}

HitTestResult NWebDelegate::GetHitTestResult() const {
  HitTestResult data;
  if (!GetBrowser().get()) {
    return data;
  }
  int type;
  CefString extra_data;
  GetBrowser()->GetHost()->GetHitData(type, extra_data);
  data.SetType(type);
  data.SetExtra(extra_data.ToString());
  return data;
}

int NWebDelegate::PageLoadProgress() {
  if (!GetBrowser().get()) {
    return 0;
  }
  return GetBrowser()->GetHost()->PageLoadProgress();
}

float NWebDelegate::Scale() {
  if (!GetBrowser().get()) {
    return 0;
  }
  return GetBrowser()->GetHost()->Scale();
}

int NWebDelegate::Load(
    std::string& url,
    std::map<std::string, std::string> additionalHttpHeaders) {
  GURL gurl = GURL(url);
  if (gurl.is_empty() || !gurl.is_valid()) {
    GURL gurlWithHttp = GURL("https://" + url);
    if (!gurlWithHttp.is_valid()) {
      return NWEB_INVALID_URL;
    }
  }
  std::map<std::string, std::string>::iterator iter;
  std::string extra = "";
  for (iter = additionalHttpHeaders.begin();
       iter != additionalHttpHeaders.end(); iter++) {
    const std::string& key = iter->first;
    const std::string& value = iter->second;
    if (!key.empty()) {
      // Delimit with "\r\n".
      if (!value.empty()) {
        extra += "\r\n";
      }
      extra += std::string(key) + ": " + std::string(value);
    }
  }
  auto browser = GetBrowser();
  if (browser == nullptr) {
    return NWEB_ERR;
  }
  browser->GetMainFrame()->LoadHeaderUrl(CefString(url), CefString(extra));
  RequestVisitedHistory();
  return NWEB_OK;
}

int NWebDelegate::LoadWithDataAndBaseUrl(const std::string& baseUrl,
                                         const std::string& data,
                                         const std::string& mimeType,
                                         const std::string& encoding,
                                         const std::string& historyUrl) {
  LOG(DEBUG) << "NWebDelegate::LoadWithDataAndBaseUrl";
  if (!GetBrowser().get()) {
    return NWEB_ERR;
  }
  GetBrowser()->GetHost()->LoadWithDataAndBaseUrl(baseUrl, data, mimeType,
                                                  encoding, historyUrl);
  RequestVisitedHistory();
  return NWEB_OK;
}

int NWebDelegate::LoadWithData(const std::string& data,
                               const std::string& mimeType,
                               const std::string& encoding) {
  LOG(DEBUG) << "NWebDelegate::LoadWithData";
  if (!GetBrowser().get()) {
    return NWEB_ERR;
  }
  GetBrowser()->GetHost()->LoadWithData(data, mimeType, encoding);
  RequestVisitedHistory();
  return NWEB_OK;
}

const CefRefPtr<CefBrowser> NWebDelegate::GetBrowser() const {
  if (handler_delegate_) {
    return handler_delegate_->GetBrowser();
  }
  return nullptr;
}

bool NWebDelegate::IsReady() {
  return is_popup_ready_ || GetBrowser() != nullptr;
}

void NWebDelegate::RequestVisitedHistory() {
  if (!GetBrowser().get()) {
    return;
  }
  if (!has_requested_visited_history) {
    has_requested_visited_history = true;
    if (handler_delegate_) {
      std::vector<std::string> outUrls = handler_delegate_->GetVisitedHistory();
      std::vector<CefString> urls = std::vector<CefString>();
      for (auto url : outUrls) {
        urls.push_back(url);
      }
      GetBrowser()->GetHost()->AddVisitedLinks(urls);
    }
  }
}

int NWebDelegate::ContentHeight() {
  float ratio = render_handler_->GetVirtualPixelRatio();
  if (ratio > 0 && render_handler_ != nullptr) {
    return render_handler_->ContentHeight() / ratio;
  }
  return 0;
}

void NWebDelegate::RegisterArkJSfunction(
    const std::string& object_name,
    const std::vector<std::string>& method_list,
    const int32_t object_id) const {
  LOG(INFO) << "RegisterArkJSfunction name : " << object_name.c_str();
  std::vector<CefString> method_vector;
  for (std::string method : method_list) {
    method_vector.push_back(method);
  }

  if (is_popup_ready_) {
    if (handler_delegate_) {
      LOG(INFO) << "NWebDelegate::RegisterArkJSfunction popup case, the "
                   "object_name is "
                << object_name.c_str();
      handler_delegate_->SavaArkJSFunctionForPopup(object_name, method_list,
                                                   object_id);
    }
    return;
  } else if (!GetBrowser()) {
    LOG(ERROR) << "NWebDelegate::RegisterArkJSfunction fail due to "
                  "GetBrowser() return null, the object_name is "
               << object_name.c_str();
    return;
  } else {
    GetBrowser()->GetHost()->RegisterArkJSfunction(object_name, method_vector,
                                                   object_id);
  }
}

void NWebDelegate::UnregisterArkJSfunction(
    const std::string& object_name,
    const std::vector<std::string>& method_list) const {
  LOG(DEBUG) << "UnregisterArkJSfunction name : " << object_name.c_str();
  std::vector<CefString> method_vector;
  for (std::string method : method_list) {
    method_vector.push_back(method);
  }

  if (!GetBrowser()) {
    LOG(ERROR) << "NWebDelegate::UnregisterArkJSfunction fail due to "
                  "GetBrowser() return null, the object_name is "
               << object_name.c_str();
    return;
  }

  GetBrowser()->GetHost()->UnregisterArkJSfunction(object_name, method_vector);
}

void NWebDelegate::RegisterNativeArkJSFunction(
    const char* objName,
    const char** methodName,
    std::vector<std::function<char*(const char** argv, int32_t argc)>> callback,
    int32_t size) {
  handler_delegate_->RegisterNativeJavaScriptCallBack(objName, methodName, callback, size);
  std::vector<CefString> method_vector;
  for (int i = 0; i < size; i++) {
    method_vector.push_back(methodName[i]);
  }
  if (GetBrowser() && GetBrowser()->GetHost()) {
    GetBrowser()->GetHost()->RegisterArkJSfunction(objName, method_vector, -1);
  } else {
    LOG(ERROR) << "browser or host is null";
  }
}

void NWebDelegate::UnRegisterNativeArkJSFunction(const char* objName) {
  std::vector<CefString> method_vector;
  if (GetBrowser() && GetBrowser()->GetHost()) {
    GetBrowser()->GetHost()->UnregisterArkJSfunction(objName, method_vector);
  } else {
    LOG(ERROR) << "browser or host is null";
  }
}

void NWebDelegate::JavaScriptOnDocumentStart(const ScriptItems& scriptItems) {
  GetBrowser()->GetHost()->RemoveJavaScriptOnDocumentStart();
  for (auto item: scriptItems) {
    CefString script = item.first;
    std::vector<CefString> scriptRules;
    for (std::string rule : item.second) {
      CefString cefRule;
      cefRule.FromString(rule);
      scriptRules.push_back(cefRule);
    }
    GetBrowser()->GetHost()->JavaScriptOnDocumentStart(script, scriptRules);
  }
}

void NWebDelegate::CallH5Function(
    int32_t routing_id,
    int32_t h5_object_id,
    const std::string h5_method_name,
    const std::vector<std::shared_ptr<NWebValue>>& args) const {
  if (!GetBrowser()) {
    LOG(ERROR) << "NWebDelegate::CallH5Function fail due to "
                  "GetBrowser() return null";
    return;
  }

  std::vector<CefRefPtr<CefValue>> cef_args;
  for (auto& item : args) {
    cef_args.push_back(ParseNWebValueToValueHelper(item));
  }

  CefString name(h5_method_name);
  if (GetBrowser()->GetHost()) {
    GetBrowser()->GetHost()->CallH5Function(routing_id, h5_object_id, name,
                                            cef_args);
  }
}

void NWebDelegate::JavaScriptOnDocumentEnd(const ScriptItems& scriptItems) {
  GetBrowser()->GetHost()->RemoveJavaScriptOnDocumentEnd();
  for (auto item: scriptItems) {
    CefString script = item.first;
    std::vector<CefString> scriptRules;
    for (std::string rule : item.second) {
      CefString cefRule;
      cefRule.FromString(rule);
      scriptRules.push_back(cefRule);
    }
    GetBrowser()->GetHost()->JavaScriptOnDocumentEnd(script, scriptRules);
  }
}

void NWebDelegate::RegisterNWebJavaScriptCallBack(
    std::shared_ptr<NWebJavaScriptResultCallBack> callback) {
  if (handler_delegate_ == nullptr) {
    LOG(ERROR) << "fail to register NWEB client, NWEB handler is nullptr";
    return;
  }
  handler_delegate_->RegisterNWebJavaScriptCallBack(callback);
}

bool NWebDelegate::OnFocus(const FocusReason& focusReason) const {
  if (!GetBrowser().get()) {
    LOG(ERROR) << "NWebDelegate::OnFocus GetBrowser().get() fail";
    return false;
  }

#if defined(OHOS_INPUT_EVENTS)
  if (focusReason == FocusReason::FOCUS_DEFAULT && !focus_nweb_id_.empty() &&
      focus_nweb_id_.find(nweb_id_) == focus_nweb_id_.end()) {
    LOG(DEBUG) << "There is already web capture, and there is no need for "
                  "capture when loading this web page.";
    return false;
  }
#endif  // defined(OHOS_INPUT_EVENTS)

#ifdef OHOS_FOCUS
  if (handler_delegate_ && !handler_delegate_->GetFocusState()) {
    GetBrowser()->GetHost()->SetFocus(true);
#if defined(OHOS_INPUT_EVENTS)
    focus_nweb_id_.insert(nweb_id_);
#endif  // defined(OHOS_INPUT_EVENTS)
  }
#endif  // #ifdef OHOS_FOCUS

  return true;
}

void NWebDelegate::OnBlur() const {
  if (!GetBrowser().get()) {
    LOG(ERROR) << "NWebDelegate::OnBlur GetBrowser().get() fail";
    return;
  }

#ifdef OHOS_FOCUS
  if (handler_delegate_ && handler_delegate_->GetFocusState()) {
    handler_delegate_->SetFocusState(false);
    GetBrowser()->GetHost()->SetFocus(false);
#if defined(OHOS_INPUT_EVENTS)
    if (focus_nweb_id_.find(nweb_id_) != focus_nweb_id_.end()) {
      focus_nweb_id_.erase(nweb_id_);
    }
    if (is_onPause_) {
      handler_delegate_->SetContinueNeedFocus(true);
    }
#endif  // defined(OHOS_INPUT_EVENTS)
  }
#endif  // #ifdef OHOS_FOCUS
}

#if defined(OHOS_I18N)
void NWebDelegate::UpdateLocale(const std::string& language,
                                const std::string& region) {
  if (!GetBrowser().get()) {
    return;
  }

  CefString locale = "";
  if (language == "en" && region == "US") {
    locale = "en-US";
  } else if (language == "zh") {
    locale = "zh-CN";
  } else {
    // Now only support zh and en.
    return;
  }

  GetBrowser()->GetHost()->UpdateLocale(locale);
}
#endif  // #ifdef OHOS_I18N

#if defined(REPORT_SYS_EVENT)
void NWebDelegate::SetNWebId(uint32_t nwebId) {
  nweb_id_ = nwebId;
  if (nweb_id_ != 0) {
    handler_delegate_->SetNWebId(nweb_id_);
  }
}
#endif

#ifdef OHOS_DRAG_DROP
std::shared_ptr<NWebDragData> NWebDelegate::GetOrCreateDragData() {
  if (!render_handler_) {
    LOG(ERROR) << "render_handler is nullptr";
    return nullptr;
  }
  return std::make_shared<NWebDragDataImpl>(render_handler_->GetDragData());
}

void NWebDelegate::ClearDragData() const {
  if (!render_handler_) {
    return;
  }
  render_handler_->FreePixlMapData();
  auto drag_data = render_handler_->GetDragData();
  if (drag_data) {
    drag_data->SetFragmentText("");
    drag_data->SetLinkURL("");
    drag_data->SetFragmentHtml("");
  }
}
#endif  // OHOS_DRAG_DROP

void NWebDelegate::SendDragEvent(const DelegateDragEvent& dragEvent) const {
  if (!GetBrowser().get() || !render_handler_) {
    LOG(ERROR) << "browser or render_handler is nullptr";
    return;
  }
  CefMouseEvent event;
  float ratio = render_handler_->GetVirtualPixelRatio();
  event.x = dragEvent.x / ratio;
  event.y = dragEvent.y / ratio;
  event.modifiers = EVENTFLAG_LEFT_MOUSE_BUTTON;
#ifdef OHOS_DRAG_DROP
  switch (dragEvent.action) {
    case DelegateDragAction::DRAG_START:
      LOG(DEBUG) << "DragDrop event SendDragEvent start webId:" << GetBrowser()->GetNWebId();
      break;
    case DelegateDragAction::DRAG_ENTER:
      if (render_handler_) {
        LOG(DEBUG) << "DragDrop event DRAG_ENTER SendDragEvent enter, send dragdata to chromium webId:"
                  << GetBrowser()->GetNWebId();
        ClearDragData();
        auto drag_data = render_handler_->GetDragData();
        GetBrowser()->GetHost()->DragTargetDragEnter(drag_data, event,
                                                     DRAG_OPERATION_EVERY);
      } else {
        LOG(ERROR) << "DragDrop drag data render_handler_ nullptr";
      }
      break;
    case DelegateDragAction::DRAG_LEAVE:
      LOG(DEBUG) << "DragDrop event SendDragEvent leave webId:" << GetBrowser()->GetNWebId();
      GetBrowser()->GetHost()->DragTargetDragLeave();
      break;
    case DelegateDragAction::DRAG_OVER:
      LOG(DEBUG) << "DragDrop event SendDragEvent over webId:" << GetBrowser()->GetNWebId();
      GetBrowser()->GetHost()->DragTargetDragOver(event, DRAG_OPERATION_EVERY);
      break;
    case DelegateDragAction::DRAG_DROP:
      event.modifiers = EVENTFLAG_NONE;
      LOG(DEBUG) << "DragDrop event SendDragEvent drop webId:" << GetBrowser()->GetNWebId();
      if (render_handler_) {
        auto drag_data1 = render_handler_->GetDragData();
        auto fragment1 = drag_data1->GetFragmentText();
        LOG(DEBUG) << "DragDrop drag data GetFragmentText:" << fragment1.ToString();
        auto link_url1 = drag_data1->GetLinkURL();
        LOG(DEBUG) << "DragDrop drag data GetLinkURL:" << link_url1.ToString();
        auto link_html1 = drag_data1->GetFragmentHtml();
        LOG(DEBUG) << "DragDrop drag data GetFragmentHtml:" << link_html1.ToString();
      } else {
        LOG(ERROR) << "DragDrop drag data render_handler_ nullptr";
      }

      GetBrowser()->GetHost()->DragTargetDrop(event);
      break;
    case DelegateDragAction::DRAG_END:
      ClearDragData();
      LOG(DEBUG) << "DragDrop event SendDragEvent end webId:" << GetBrowser()->GetNWebId();
      GetBrowser()->GetHost()->DragSourceEndedAt(event.x, event.y,
                                                 DRAG_OPERATION_COPY);
      GetBrowser()->GetHost()->DragSourceSystemDragEnded();
      break;
    case DelegateDragAction::DRAG_CANCEL:
      ClearDragData();
      LOG(DEBUG) << "DragDrop event SendDragEvent cancel webId:" << GetBrowser()->GetNWebId();
      GetBrowser()->GetHost()->DragSourceSystemDragEnded();
      break;
    default:
      LOG(DEBUG) << "invalid drag action";
      break;
  }
#endif  // OHOS_DRAG_DROP
}

void NWebDelegate::GetImages(
    std::shared_ptr<NWebValueCallback<bool>> callback) {
  if (!GetBrowser().get()) {
    LOG(ERROR) << "JSAPI GetImages can not get browser";
    return;
  }

  CefRefPtr<GetImagesCallbackImpl> GetImagesCb =
      new GetImagesCallbackImpl(callback);
  GetBrowser()->GetMainFrame()->GetImages(GetImagesCb);
}

void NWebDelegate::RemoveCache(bool include_disk_files) {
  if (!GetBrowser().get()) {
    LOG(ERROR) << "JSAPI RemoveCache can not get browser";
    return;
  }

  GetBrowser()->GetHost()->RemoveCache(include_disk_files);
}

#ifdef OHOS_NAVIGATION
std::shared_ptr<NWebHistoryList> NWebDelegate::GetHistoryList() {
  if (!GetBrowser().get()) {
    return nullptr;
  }

  CefRefPtr<NavigationEntryVisitorImpl> visitor =
      new NavigationEntryVisitorImpl();
  GetBrowser()->GetHost()->GetNavigationEntries(visitor, false);
  return visitor->GetHistoryList();
}

WebState NWebDelegate::SerializeWebState() {
  CefRefPtr<CefBinaryValue> state_value =
      GetBrowser()->GetHost()->GetWebState();
  if (!state_value || !GetBrowser().get()) {
    return nullptr;
  }
  size_t state_size = state_value->GetSize();
  if (state_size == 0) {
    return nullptr;
  }
  WebState state = std::make_shared<std::vector<uint8_t>>(state_size);
  size_t read_size = state_value->GetData(state->data(), state_size, 0);
  if (read_size != state_size) {
    LOG(ERROR) << "SerializeWebState failed";
    return nullptr;
  }
  return state;
}

bool NWebDelegate::RestoreWebState(WebState state) {
  if (!GetBrowser().get() || !state || state->size() == 0) {
    return false;
  }
  auto web_state = CefBinaryValue::Create(state->data(), state->size());
  return GetBrowser()->GetHost()->RestoreWebState(web_state);
}
#endif

#ifdef OHOS_PAGE_UP_DOWN
void NWebDelegate::PageUp(bool top) {
  if (!GetBrowser().get() || !render_handler_ || !handler_delegate_) {
    return;
  }
  float scale = handler_delegate_->GetScale() / 100.0;
  if (scale <= 0) {
    LOG(ERROR) << "get scale invalid: " << scale;
    return;
  }
  GetBrowser()->GetHost()->ScrollPageUpDown(true, !top, height_ / scale);
}

void NWebDelegate::PageDown(bool bottom) {
  if (!GetBrowser().get() || !render_handler_ || !handler_delegate_) {
    return;
  }
  float scale = handler_delegate_->GetScale() / 100.0;
  if (scale <= 0) {
    LOG(ERROR) << "get scale invalid: " << scale;
    return;
  }
  GetBrowser()->GetHost()->ScrollPageUpDown(false, !bottom, height_ / scale);
}
#endif  // #ifdef OHOS_PAGE_UP_DOWN

#if defined(OHOS_INPUT_EVENTS)
void NWebDelegate::SetNWebDelegateInterface(
    std::shared_ptr<NWebDelegateInterface> client) {
  if (render_handler_ == nullptr) {
    LOG(ERROR) << "fail to register NWebDelegateInterface client, render "
                  "handler is nullptr";
    return;
  }
  render_handler_->SetNWebDelegateInterface(client);
}

void NWebDelegate::ScrollTo(float x, float y) {
  if (!GetBrowser().get()) {
    LOG(ERROR) << "JSAPI ScrollTo can not get browser";
    return;
  }
  float ratio = render_handler_->GetVirtualPixelRatio();
  if (ratio <= 0) {
    LOG(ERROR) << "get ratio invalid: " << ratio;
    return;
  }

  GetBrowser()->GetHost()->ScrollTo(x * ratio, y * ratio);
}

void NWebDelegate::ScrollBy(float delta_x, float delta_y) {
  if (!GetBrowser().get()) {
    LOG(ERROR) << "JSAPI ScrollBy can not get browser";
    return;
  }
  float ratio = render_handler_->GetVirtualPixelRatio();
  if (ratio <= 0) {
    LOG(ERROR) << "get ratio invalid: " << ratio;
    return;
  }

  GetBrowser()->GetHost()->ScrollBy(delta_x * ratio, delta_y * ratio);
}

void NWebDelegate::SlideScroll(float vx, float vy) {
  if (!GetBrowser().get()) {
    LOG(ERROR) << "JSAPI SlideScroll can not get browser";
    return;
  }

  GetBrowser()->GetHost()->SlideScroll(vx, vy);
}
#endif  // defined(OHOS_INPUT_EVENTS)

#if defined(OHOS_API_INIT_WEB_ENGINE)
void NWebDelegate::OnContextInitializeComplete(const std::string& url,
                                               void* window
#if defined(OHOS_INCOGNITO_MODE)
                                              , bool incognito_mode
#endif
                                               ) {
  // Create browser after context initialzed complete.
  NWebApplication::GetDefault()->CreateBrowser(preference_delegate_, url,
                                               handler_delegate_, window
#if defined(OHOS_INCOGNITO_MODE)
                                               , incognito_mode
#endif
                                               );
}
#endif  // defined(OHOS_API_INIT_WEB_ENGINE)

#if defined(OHOS_MEDIA_MUTE_AUDIO)
void NWebDelegate::SetAudioMuted(bool muted) {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "SetAudioMuted can not get browser";
    return;
  }

  GetBrowser()->GetHost()->SetAudioMuted(muted);
}
#endif  // defined(OHOS_MEDIA_MUTE_AUDIO)

#if defined(OHOS_COMPOSITE_RENDER)
void NWebDelegate::SetShouldFrameSubmissionBeforeDraw(bool should) {
  if (GetBrowser().get()) {
    GetBrowser()->GetHost()->SetShouldFrameSubmissionBeforeDraw(should);
  }
}

void NWebDelegate::SetDrawRect(int32_t x, int32_t y, int32_t width, int32_t height) {
  if (GetBrowser().get()) {
    GetBrowser()->GetHost()->SetDrawRect(x, y, width, height);
  }
}

void NWebDelegate::SetDrawMode(int32_t mode) {
  if (GetBrowser().get()) {
    GetBrowser()->GetHost()->SetDrawMode(mode);
  }
}
#endif  // defined(OHOS_COMPOSITE_RENDER)

#if defined(OHOS_EX_FORCE_ZOOM)
void NWebDelegate::SetForceEnableZoom(bool forceEnableZoom) {
  LOG(INFO) << "NWebDelegate::SetForceEnableZoom " << forceEnableZoom;
  if (GetBrowser().get()) {
    GetBrowser()->SetForceEnableZoom(forceEnableZoom);
  }
}

bool NWebDelegate::GetForceEnableZoom() {
  if (GetBrowser().get()) {
    return GetBrowser()->GetForceEnableZoom();
  }
  return false;
}
#endif

#if defined(OHOS_INPUT_EVENTS)
void NWebDelegate::SetVirtualKeyBoardArg(int32_t width, int32_t height, double keyboard) {
  if (GetBrowser().get()) {
    GetBrowser()->GetHost()->SetVirtualKeyBoardArg(width, height, keyboard);
  }
}
 
bool NWebDelegate::ShouldVirtualKeyboardOverlay() {
  if (GetBrowser().get()) {
    return GetBrowser()->GetHost()->ShouldVirtualKeyboardOverlay();
  }
  return false;
}
#endif

#if defined(OHOS_MEDIA_POLICY)
void NWebDelegate::SetAudioResumeInterval(int32_t resumeInterval) {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "SetAudioResumeInterval can not get browser";
    return;
  }

  GetBrowser()->GetHost()->SetAudioResumeInterval(resumeInterval);
}

void NWebDelegate::SetAudioExclusive(bool audioExclusive) {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "SetAudioExclusive can not get browser";
    return;
  }

  GetBrowser()->GetHost()->SetAudioExclusive(audioExclusive);
}
#endif  // defined(OHOS_MEDIA_POLICY)

#if defined(OHOS_NO_STATE_PREFETCH)
void NWebDelegate::PrefetchPage(
    std::string& url,
    std::map<std::string, std::string> additionalHttpHeaders) {
  CefString urlCef;
  urlCef.FromString(url);
  std::string output;
  for (auto& header : additionalHttpHeaders) {
    base::StringAppendF(&output, "%s: %s\r\n", header.first.c_str(),
                        header.second.c_str());
  }
  output.append("\r\n");
  CefString additionalHttpHeadersCef;
  additionalHttpHeadersCef.FromString(output);
  if (GetBrowser().get()) {
    GetBrowser()->PrefetchPage(urlCef, additionalHttpHeadersCef);
  }
}
#endif  // defined(OHOS_NO_STATE_PREFETCH)

#if defined(OHOS_MULTI_WINDOW)
void NWebDelegate::NotifyPopupWindowResult(bool result) {
  if (handler_delegate_) {
    handler_delegate_->NotifyPopupWindowResult(result);
  }
}
#endif  // defined(OHOS_MULTI_WINDOW)
#ifdef OHOS_CA
bool NWebDelegate::GetCertChainDerData(std::vector<std::string>& certChainData,
                                       bool isSingleCert) {
  if (!GetBrowser().get()) {
    LOG(ERROR) << "GetCertChainDerData failed, browser is null";
    return false;
  }

  CefRefPtr<CefNavigationEntry> navigation =
      GetBrowser()->GetHost()->GetVisibleNavigationEntry();
  if (!navigation) {
    LOG(ERROR)
        << "GetCertChainDerData failed, visible navigation entry is null";
    return false;
  }

  CefRefPtr<CefSSLStatus> ssl = navigation->GetSSLStatus();
  if (!ssl) {
    LOG(ERROR) << "GetCertChainDerData failed, ssl status is null";
    return false;
  }

  CefRefPtr<CefX509Certificate> cert = ssl->GetX509Certificate();
  if (!cert) {
    LOG(ERROR) << "GetCertChainDerData failed, cef x509cert is null";
    return false;
  }

  return GetCertChainDerDataInner(cert, certChainData, isSingleCert);
}

bool NWebDelegate::GetCertChainDerDataInner(
    CefRefPtr<CefX509Certificate> cert,
    std::vector<std::string>& certChainData,
    bool isSingleCert) {
  CefX509Certificate::IssuerChainBinaryList der_chain_list;
  cert->GetDEREncodedIssuerChain(der_chain_list);
  der_chain_list.insert(der_chain_list.begin(), cert->GetDEREncoded());

  LOG(DEBUG) << "GetCertChainDerData der_chain_list size = "
             << der_chain_list.size();
  for (size_t i = 0U; i < der_chain_list.size(); ++i) {
    if (!der_chain_list[i].get()) {
      LOG(ERROR)
          << "GetCertChainDerDataInner failed, der chain data is null, index = "
          << i;
      continue;
    }

    const size_t cert_data_size = der_chain_list[i]->GetSize();
    std::string cert_data_item;
    cert_data_item.resize(cert_data_size);
    der_chain_list[i]->GetData(const_cast<char*>(cert_data_item.data()),
                               cert_data_size, 0);
    certChainData.emplace_back(cert_data_item);
    if (isSingleCert) {
      LOG(DEBUG) << "get only one certificate of the current website";
      break;
    }
  }

  if (certChainData.size() == 0) {
    LOG(DEBUG) << "GetCertChainDerData, no certificate data";
    return false;
  }

  return true;
}
#endif

#ifdef OHOS_EX_TOPCONTROLS
void NWebDelegate::UpdateBrowserControlsState(int constraints,
                                              int current,
                                              bool animate) const {
  if (GetBrowser().get()) {
    GetBrowser()->UpdateBrowserControlsState(constraints, current, animate);
  }
}

void NWebDelegate::UpdateBrowserControlsHeight(int height, bool animate) {
  if (GetBrowser().get()) {
    GetBrowser()->UpdateBrowserControlsHeight(height, animate);
  }
}

#endif

#if defined(OHOS_PRINT)
void NWebDelegate::SetToken(void* token) {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "SetToken can not get browser";
    return;
  }

  GetBrowser()->GetHost()->SetToken(token);
}

void* NWebDelegate::CreateWebPrintDocumentAdapter(const std::string& jobName) {
  LOG(DEBUG) << "Create Web print document adapter jobName = " << jobName;
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "CreateWebPrintDocumentAdapter can not get browser";
    return nullptr;
  }

  void* webPrintDocumentAdapter = nullptr;
  GetBrowser()->GetHost()->CreateWebPrintDocumentAdapter(CefString(jobName), &webPrintDocumentAdapter);
  return webPrintDocumentAdapter;
}
#endif // defined(OHOS_PRINT)

#if defined(OHOS_EX_PASSWORD)
void NWebDelegate::SaveOrUpdatePassword(bool is_update) {
  if (GetBrowser().get()) {
    GetBrowser()->SaveOrUpdatePassword(is_update);
  }
}
void NWebDelegate::SetSavePasswordAutomatically(bool enable) {
  LOG(INFO) << "NWebDelegate::SetSavePasswordAutomatically " << enable;
  if (GetBrowser().get()) {
    GetBrowser()->SetSavePasswordAutomatically(enable);
  }
}

bool NWebDelegate::GetSavePasswordAutomatically() {
  if (GetBrowser().get()) {
    return GetBrowser()->GetSavePasswordAutomatically();
  }
  return false;
}

void NWebDelegate::SetSavePassword(bool enable) {
  LOG(INFO) << "NWebDelegate::SetSavePassword " << enable;
  if (GetBrowser().get()) {
    GetBrowser()->SetSavePassword(enable);
  }
}

bool NWebDelegate::GetSavePassword() {
  if (GetBrowser().get()) {
    return GetBrowser()->GetSavePassword();
  }
  return false;
}

void NWebDelegate::PasswordSuggestionSelected(int list_index) const {
  LOG(INFO) << "NWebDelegate::PasswordSuggestionSelected";
  if (GetBrowser().get()) {
    GetBrowser()->PasswordSuggestionSelected(list_index);
  }
}
#endif  // OHOS_EX_PASSWORD

#if defined(OHOS_EX_FREE_COPY)
void NWebDelegate::SelectAndCopy() {
  if (GetBrowser().get()) {
    GetBrowser()->SelectAndCopy();
  }
}

bool NWebDelegate::ShouldShowFreeCopy() {
  if (GetBrowser().get()) {
    return GetBrowser()->ShouldShowFreeCopy();
  }
  return false;
}
#endif  // #if defined(OHOS_EX_FREE_COPY)

#ifdef OHOS_EX_BLANK_TARGET_POPUP_INTERCEPT
void NWebDelegate::SetEnableBlankTargetPopupIntercept(
    bool enableBlankTargetPopup) {
  LOG(INFO) << "NWebDelegate::SetEnableBlankTargetPopupIntercept "
            << enableBlankTargetPopup;
  if (GetBrowser().get()) {
    GetBrowser()->SetEnableBlankTargetPopupIntercept(enableBlankTargetPopup);
  }
  if (preference_delegate_) {
    // When GetBrowser() may return nullptr, we cannot set
    // enableBlankTargetPopup only. Set enableBlankTargetPopup in
    // NWebPreferenceDelegate first, and then set it in
    // NWebHandlerDelegate::OnAfterCreated when browser is created.
    LOG(DEBUG) << "NWebDelegate::SetEnableBlankTargetPopupIntercept to "
                 "preference_delegate_ "<< enableBlankTargetPopup;
    preference_delegate_->SetEnableBlankTargetPopupIntercept(
        enableBlankTargetPopup);
  }
}
#endif

bool NWebDelegate::Discard() {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "NWebDelegate::Discard failed, browser is nullptr";
    return false;
  }

  if (is_discarded_) {
    LOG(ERROR) << "NWebDelegate::Discard failed, the webview window was discarded before";
    return false;
  }

  is_discarded_ = GetBrowser()->GetHost()->Discard();
  LOG(DEBUG) << "NWebDelegate::Discard is_discarded_: " << is_discarded_;
  return is_discarded_;
}

bool NWebDelegate::Restore() {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "NWebDelegate::Restore failed, browser is nullptr";
    return false;
  }

  if (!is_discarded_) {
    LOG(ERROR) << "NWebDelegate::Restore failed, the webview window was not discarded before";
    return false;
  }

  bool is_restored = GetBrowser()->GetHost()->Restore();
  is_discarded_ = !is_restored;
  LOG(DEBUG) << "NWebDelegate::Restore is_restored: " << is_restored;
  return is_restored;
}

#ifdef OHOS_EX_GET_ZOOM_LEVEL
void NWebDelegate::SetBrowserZoomLevel(double zoom_factor) {
  LOG(DEBUG) << "NWebDelegate::SetBrowserZoomLevel: " << zoom_factor;
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "SetBrowserZoomLevel can not get browser";
    return;
  }

  GetBrowser()->GetHost()->SetBrowserZoomLevel(zoom_factor);
}

double NWebDelegate::GetBrowserZoomLevel() {
  LOG(DEBUG) << "NWebDelegate::GetBrowserZoomLevel.";
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "GetBrowserZoomLevel can not get browser";
    return 1.0;
  }

  return std::pow(kZoomLevelToFactorRatio,
                  GetBrowser()->GetHost()->GetZoomLevel());
}
#endif

#if defined(OHOS_SECURITY_STATE)
int NWebDelegate::GetSecurityLevel() {
  if (GetBrowser() == nullptr) {
    LOG(ERROR) << "NWebDelegate::GetSecurityLevel failed.";
    return static_cast<int>(security_state::SecurityLevel::NONE);
  }

  return GetBrowser()->GetSecurityLevel();
}
#endif
 
void NWebDelegate::RegisterAccessibilityEventListener(
    std::shared_ptr<NWebAccessibilityEventCallback>
        accessibility_event_listener) {
  auto* accessibilityManager = GetAccessibilityManager();
  if (accessibilityManager == nullptr) {
    LOG(ERROR) << "RegisterAccessibilityEventListener can not get "
                  "accessibilityManager";
    return;
  }
  accessibilityManager->RegisterAccessibilityEventListener(
      accessibility_event_listener);
}

void NWebDelegate::RegisterAccessibilityIdGenerator(
    std::function<int32_t()> accessibilityIdGenerator) const {
  content::BrowserAccessibilityManagerOHOS::RegisterAccessibilityIdGenerator(
      accessibilityIdGenerator);
};

void NWebDelegate::SetAccessibilityState(cef_state_t accessibilityState) {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "SetAccessibilityState can not get browser";
    return;
  }
  accessibility_state_ = (accessibilityState == STATE_ENABLED);
  GetBrowser()->GetHost()->SetAccessibilityState(accessibilityState);
}

void NWebDelegate::ExecuteAction(int32_t accessibilityId,
                                 uint32_t action) const {
  auto* accessibilityManager = GetAccessibilityManager();
  if (accessibilityManager == nullptr) {
    LOG(ERROR) << "ExecuteAction can not get accessibilityManager";
    return;
  }
  auto* node = content::BrowserAccessibilityOHOS::GetFromAccessibilityId(
      accessibilityId);
  if (node == nullptr) {
    LOG(ERROR) << "ExecuteAction can not get node";
    return;
  }
  AceAction aceAction = static_cast<AceAction>(action);

  switch (aceAction) {
    case AceAction::ACTION_CLICK:
      accessibilityManager->DoDefaultAction(*node);
      break;
    case AceAction::ACTION_ACCESSIBILITY_FOCUS:
      accessibilityManager->MoveAccessibilityFocusToId(accessibilityId);
      break;
    case AceAction::ACTION_CLEAR_ACCESSIBILITY_FOCUS:
      accessibilityManager->SendAccessibilityEvent(
          accessibilityId, AccessibilityEventType::ACCESSIBILITY_FOCUS_CLEARED);
      if (accessibilityManager->GetAccessibilityFocusId() == accessibilityId) {
        accessibilityManager->MoveAccessibilityFocus(
            accessibilityManager->GetAccessibilityFocusId(), 0);
        accessibilityManager->SetAccessibilityFocusId(0);
      }
      if (accessibilityManager->GetLastHoverId() == accessibilityId) {
        accessibilityManager->SendAccessibilityEvent(
            accessibilityManager->GetLastHoverId(),
            AccessibilityEventType::HOVER_EXIT_EVENT);
        accessibilityManager->SetLastHoverId(0);
      }
      break;
    case AceAction::ACTION_FOCUS:
      accessibilityManager->SetFocus(*node);
      break;
    case AceAction::ACTION_CLEAR_FOCUS:
      accessibilityManager->SetFocus(*accessibilityManager->GetBrowserAccessibilityRoot());
      break;
    default:
      LOG(INFO) << "ExecuteAction unsupported action";
      break;
  }
}

content::BrowserAccessibilityManagerOHOS*
NWebDelegate::GetAccessibilityManager() const {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "GetAccessibilityManager can not get browser";
    return nullptr;
  }
  void* manager = nullptr;
  GetBrowser()->GetHost()->GetOrCreateRootBrowserAccessibilityManager(&manager);
  return static_cast<content::BrowserAccessibilityManagerOHOS*>(manager);
}

bool NWebDelegate::GetFocusedAccessibilityNodeInfo(
    int32_t accessibilityId,
    bool isAccessibilityFocus,
    OHOS::NWeb::NWebAccessibilityNodeInfo& nodeInfo) const {
  auto* accessibilityManager = GetAccessibilityManager();
  if (accessibilityManager == nullptr) {
    LOG(ERROR)
        << "GetFocusedAccessibilityNodeInfo can not get accessibilityManager";
    return false;
  }
  content::BrowserAccessibilityOHOS* resultNode = nullptr;
  if (isAccessibilityFocus) {
    auto accessibilityFocusId = accessibilityManager->GetAccessibilityFocusId();
    if (accessibilityFocusId > 0) {
      resultNode = content::BrowserAccessibilityOHOS::GetFromAccessibilityId(
          accessibilityFocusId);
    }
  } else {
    resultNode = static_cast<content::BrowserAccessibilityOHOS*>(
        accessibilityManager->GetFocus());
  }
  if (resultNode == nullptr) {
    LOG(ERROR) << "GetFocusedAccessibilityNodeInfo can not get node";
    return false;
  }
  if (accessibilityId != -1) {
    auto* node = content::BrowserAccessibilityOHOS::GetFromAccessibilityId(
        accessibilityId);
    if (node == nullptr) {
      LOG(ERROR)
          << "GetFocusedAccessibilityNodeInfo can not get accessibilityId";
      return false;
    }
    if (!resultNode->IsDescendantOf(node)) {
      return false;
    }
  }
  return PopulateAccessibilityNodeInfo(resultNode, nodeInfo);
}

bool NWebDelegate::GetAccessibilityNodeInfoById(
    int32_t accessibilityId,
    OHOS::NWeb::NWebAccessibilityNodeInfo& nodeInfo) const {
  auto* accessibilityManager = GetAccessibilityManager();
  if (accessibilityManager == nullptr) {
    LOG(ERROR)
        << "GetAccessibilityNodeInfoById can not get accessibilityManager";
    return false;
  }
  content::BrowserAccessibilityOHOS* node = nullptr;
  if (accessibilityId < 0) {
    node = static_cast<content::BrowserAccessibilityOHOS*>(
        accessibilityManager->GetBrowserAccessibilityRoot());
  } else {
    node = content::BrowserAccessibilityOHOS::GetFromAccessibilityId(
        accessibilityId);
  }
  if (node == nullptr) {
    LOG(ERROR) << "GetAccessibilityNodeInfoById can not get node";
    return false;
  }
  return PopulateAccessibilityNodeInfo(node, nodeInfo);
}

bool NWebDelegate::GetAccessibilityNodeInfoByFocusMove(
    int32_t accessibilityId,
    int32_t direction,
    OHOS::NWeb::NWebAccessibilityNodeInfo& nodeInfo) const {
  auto* accessibilityManager = GetAccessibilityManager();
  if (accessibilityManager == nullptr) {
    LOG(ERROR) << "GetAccessibilityNodeInfoByFocusMove can not get "
                  "accessibilityManager";
    return false;
  }
  content::BrowserAccessibilityOHOS* node = nullptr;
  if (accessibilityId < 0) {
    node = static_cast<content::BrowserAccessibilityOHOS*>(
        accessibilityManager->GetBrowserAccessibilityRoot());
  } else {
    node = content::BrowserAccessibilityOHOS::GetFromAccessibilityId(
        accessibilityId);
  }
  auto resultNode = node->GetAccessibilityNodeByFocusMove(direction);
  if (resultNode == nullptr) {
    LOG(ERROR) << "GetAccessibilityNodeInfoByFocusMove can not get node";
    return false;
  }
  return PopulateAccessibilityNodeInfo(resultNode, nodeInfo);
}

bool NWebDelegate::PopulateAccessibilityNodeInfo(
    const content::BrowserAccessibilityOHOS* node,
    NWebAccessibilityNodeInfo& nodeInfo) const {
  auto* accessibilityManager = GetAccessibilityManager();
  if (accessibilityManager == nullptr) {
    LOG(ERROR)
        << "GetFocusedAccessibilityNodeInfo can not get accessibilityManager";
    return false;
  }
  nodeInfo.accessibilityId = node->GetAccessibilityId();
  bool isRoot = !node->PlatformGetParent();
  if (!isRoot) {
    auto* parentNode = static_cast<content::BrowserAccessibilityOHOS*>(
        node->PlatformGetParent());
    nodeInfo.parentId = parentNode->GetAccessibilityId();

    if (!parentNode->PlatformGetParent()) {
      nodeInfo.parentId = -1;
    }
  }

  nodeInfo.childIds.clear();

  for (const auto& childNode : node->PlatformChildren()) {
    const content::BrowserAccessibilityOHOS& childNodeOHOS =
        static_cast<const content::BrowserAccessibilityOHOS&>(childNode);
    nodeInfo.childIds.push_back(childNodeOHOS.GetAccessibilityId());
  }

  nodeInfo.componentType = node->GetRoleString();
  nodeInfo.focused = node->IsFocused();
  nodeInfo.visible = !node->IsInvisibleOrIgnored();
  nodeInfo.enabled = node->IsEnabled();
  nodeInfo.accessibilityFocus =
      (accessibilityManager->GetAccessibilityFocusId() ==
               node->GetAccessibilityId()
           ? true
           : false);

  nodeInfo.focusable = node->IsFocusable();
  nodeInfo.descriptionInfo = "";
  nodeInfo.content = "";
  if (node->IsLink()) {
    nodeInfo.descriptionInfo = node->GetTargetUrl();
  } else if (node->IsPasswordField()) {
    nodeInfo.content = "*";
  } else {
    nodeInfo.content = base::UTF16ToUTF8(node->GetTextContentUTF16());
  }
  nodeInfo.hint = node->GetHint();
  nodeInfo.hinting = node->IsHint();
  nodeInfo.checked = node->IsChecked();
  nodeInfo.selected = node->IsSelected();
  nodeInfo.password = node->IsPasswordField();
  nodeInfo.checkable = node->IsCheckable();
  nodeInfo.scrollable = node->IsScrollable();
  nodeInfo.editable = node->IsTextField();
  nodeInfo.pluralLineSupported = node->IsMultiLine();
  nodeInfo.popupSupported = node->CanOpenPopup();
  nodeInfo.error = node->GetContentInvalidErrorMessage();
  nodeInfo.contentInvalid = node->IsContentInvalid();
  nodeInfo.deletable = false;
  nodeInfo.inputType = node->OHOSInputType();
  nodeInfo.liveRegion = node->OHOSLiveRegionType();
  nodeInfo.selectionStart = node->GetSelectionStart();
  nodeInfo.selectionEnd = node->GetSelectionEnd();
  nodeInfo.itemCounts = node->GetItemCount();
  nodeInfo.clickable = node->IsClickable();
  nodeInfo.rangeInfoMin = node->RangeMin();
  nodeInfo.rangeInfoMax = node->RangeMax();
  nodeInfo.rangeInfoCurrent = node->RangeCurrentValue();

  AddAccessibilityNodeInfoRect(nodeInfo, node);
  AddAccessibilityNodeInfoCollection(nodeInfo, node);
  AddAccessibilityNodeInfoActions(nodeInfo);

  return true;
}

void NWebDelegate::AddAccessibilityNodeInfoRect(
    NWebAccessibilityNodeInfo& nodeInfo,
    const content::BrowserAccessibilityOHOS* node) const {
  ui::AXOffscreenResult offscreen_result = ui::AXOffscreenResult::kOnscreen;
  gfx::Rect absolute_rect = node->GetUnclippedRootFrameBoundsRect(&offscreen_result);

  nodeInfo.rectX = absolute_rect.x();
  nodeInfo.rectY = absolute_rect.y();
  nodeInfo.rectWidth = absolute_rect.width();
  nodeInfo.rectHeight = absolute_rect.height();
}

void NWebDelegate::AddAccessibilityNodeInfoCollection(
    NWebAccessibilityNodeInfo& nodeInfo,
    const content::BrowserAccessibilityOHOS* node) const {
  if (node->IsCollection()) {
    nodeInfo.gridRows = node->RowCount();
    nodeInfo.gridColumns = node->ColumnCount();
    nodeInfo.gridSelectedMode = node->IsHierarchical();
  }
  if (node->IsCollectionItem()) {
    nodeInfo.gridItemRow = node->RowIndex();
    nodeInfo.gridItemRowSpan = node->RowSpan();
    nodeInfo.gridItemColumn = node->ColumnIndex();
    nodeInfo.gridItemColumnSpan = node->ColumnSpan();
    nodeInfo.heading = node->IsHeading();
  }
}

void NWebDelegate::AddAccessibilityNodeInfoActions(
    NWebAccessibilityNodeInfo& nodeInfo) const {
  if (nodeInfo.clickable) {
    nodeInfo.actions.emplace_back(
        static_cast<uint32_t>(AceAction::ACTION_CLICK));
  }
  if (nodeInfo.focusable) {
    if (nodeInfo.focused) {
      nodeInfo.actions.emplace_back(
          static_cast<uint32_t>(AceAction::ACTION_CLEAR_FOCUS));
    } else {
      nodeInfo.actions.emplace_back(
          static_cast<uint32_t>(AceAction::ACTION_FOCUS));
    }
  }

  if (nodeInfo.accessibilityFocus) {
    nodeInfo.actions.emplace_back(
        static_cast<uint32_t>(AceAction::ACTION_CLEAR_ACCESSIBILITY_FOCUS));
  } else {
    nodeInfo.actions.emplace_back(
        static_cast<uint32_t>(AceAction::ACTION_ACCESSIBILITY_FOCUS));
  }
}
}  // namespace OHOS::NWeb
