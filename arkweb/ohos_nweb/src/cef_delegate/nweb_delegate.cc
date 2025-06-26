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
#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
#include "nweb_accessibility_utils.h"
#endif
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "base/command_line.h"
#include "base/check.h"
#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "base/task/thread_pool.h"
#include "base/trace_event/trace_event.h"
#include "cef/include/base/cef_logging.h"
#include "cef/include/cef_app.h"
#include "cef/include/cef_base.h"
#include "cef/include/cef_request_context.h"
#include "cef/include/internal/cef_string_map.h"
#include "cef/ohos_cef_ext/include/arkweb_frame_ext.h"
#include "content/public/common/content_switches.h"
#include "securec.h"
#include "nweb_application.h"
#include "nweb_handler_delegate.h"
#include "nweb_hit_test_result_impl.h"
#include "nweb_render_handler.h"
#include "nweb_value_convert.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#if BUILDFLAG(ARKWEB_REPORT_SYS_EVENT)
#include "event_reporter.h"
#endif

#include "libcef/browser/thread_util.h"
#include "nweb_find_delegate.h"
#include "nweb_preference_delegate.h"
#include "url/gurl.h"

#if BUILDFLAG(ARKWEB_NAVIGATION)
#include "cef/ohos_cef_ext/libcef/browser/arkweb_navigation_state_serializer_ext.h"
#include "nweb_history_list_impl.h"
#endif  // BUILDFLAG(ARKWEB_NAVIGATION)

#if BUILDFLAG(ARKWEB_DRAG_DROP)
#include "nweb_drag_data_impl.h"
#endif  // #if BUILDFLAG(ARKWEB_DRAG_DROP)

#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
#include "base/strings/stringprintf.h"
#endif  // BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)

#include "base/strings/escape.h"
#include "cef/include/internal/cef_string_types.h"
#include "content/public/common/content_switches.h"
#include "libcef/common/net/url_util_ex.h"
#include "net/base/filename_util.h"
#include "nweb_download_handler_delegate.h"
#include "nweb_find_delegate.h"
#include "nweb_preference_delegate.h"
#include "ui/events/gesture_detection/gesture_configuration.h"

#if BUILDFLAG(ARKWEB_URL_TRUST_LIST)
#include "cef/ohos_cef_ext/libcef/browser/ohos_safe_browsing/ohos_url_trust_list_interface.h"
#endif

#if BUILDFLAG(ARKWEB_EXT_GET_ZOOM_LEVEL)
#include <cmath>
#endif

#if BUILDFLAG(ARKWEB_SECURITY_STATE)
#include "components/security_state/core/security_state.h"
#endif

#if BUILDFLAG(ARKWEB_I18N)
#include "arkweb/chromium_ext/base/ohos/locale_utils.h"
#endif

#include "ohos_nweb/src/capi/nweb_devtools_message_handler.h"
#include "ohos_nweb/src/cef_delegate/nweb_devtools_message_handler_impl.h"
#if BUILDFLAG(ARKWEB_DEVTOOLS)
#include "cef/include/cef_devtools_message_handler_delegate.h"
#endif // BUILDFLAG(ARKWEB_DEVTOOLS)

#if BUILDFLAG(ARKWEB_AI)
#include "ohos_resources/data_detector/grit/data_detector_resources.h"
#include "ui/base/clipboard/ohos/clip_board_image_data_adapter_impl.h"
#include "ui/base/resource/resource_bundle.h"
#endif // BUILDFLAG(ARKWEB_AI)

namespace {
static const float richtextDisplayRatio = 1.0;
}

namespace OHOS::NWeb {
using namespace ui;
#if BUILDFLAG(ARKWEB_EXT_GET_ZOOM_LEVEL)
static const double kZoomLevelToFactorRatio = 1.2;
#endif

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
const int NWebPlaybackState_NONE = 0;
#endif

static const int kDefaultWebNativeProxy = -2;
#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
static const int64_t kRootAccessibilityId = 1;
#endif

#if BUILDFLAG(ARKWEB_MSGPORT)
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
      std::shared_ptr<NWebMessageValueCallback> callback,
      uint32_t callbackId,
      std::shared_ptr<NWebDelegateInterface> delegate)
      : callback_(callback),
        callbackId_(callbackId),
        weakNWebDelegate_(std::weak_ptr<NWebDelegateInterface>(delegate)) {}
  ~JavaScriptResultCallbackImpl() {}
  void CallbackOnReceiveThread(std::shared_ptr<OHOS::NWeb::NWebMessage> data) {
    if (callback_) {
      callback_->OnReceiveValue(data);
    }
    if (weakNWebDelegate_.expired()) {
      LOG(INFO) << "weakNWebDelegate_ expired";
      return;
    }
    // post this instance to ui to destroy
    auto delegate = weakNWebDelegate_.lock();
    if (delegate) {
      CEF_POST_TASK(
          CEF_UIT,
          base::BindOnce(&NWebDelegateInterface::EraseJavaScriptCallbackImpl,
                         delegate, callbackId_));
    }
  }

  void OnJavaScriptExeResult(CefRefPtr<CefValue> result) override {
    if (callback_ != nullptr) {
      auto data=std::make_shared<OHOS::NWeb::NWebMessage>(NWebValue::Type::NONE);
      ConvertCefValueToNWebMessage(result, data);
      base::ThreadPool::PostTask(
          FROM_HERE, {base::MayBlock(), base::TaskPriority::HIGHEST},
          base::BindOnce(
              base::IgnoreResult(
                  &JavaScriptResultCallbackImpl::CallbackOnReceiveThread),
              base::Unretained(this), data));
    }
  }

 private:
  std::shared_ptr<NWebMessageValueCallback> callback_;
  uint32_t callbackId_;
  std::weak_ptr<NWebDelegateInterface> weakNWebDelegate_;

  IMPLEMENT_REFCOUNTING(JavaScriptResultCallbackImpl);
};

class CefWebMessageReceiverImpl : public CefWebMessageReceiver {
 public:
  CefWebMessageReceiverImpl(std::shared_ptr<NWebMessageValueCallback> callback)
      : callback_(callback) {}
  void OnMessage(CefRefPtr<CefValue> message) override {
    LOG(DEBUG) << "OnMessage in nweb delegate";
    if (callback_ != nullptr) {
        auto data = std::make_shared<OHOS::NWeb::NWebMessage>(NWebValue::Type::NONE);
        ConvertCefValueToNWebMessage(message, data);
        callback_->OnReceiveValue(data);
    }
  }

  bool OnMessageWithBoolResult(CefRefPtr<CefValue> message) override {
    LOG(DEBUG) << "OnMessageWithBoolResult in nweb delegate";
    if (callback_ != nullptr) {
        auto data = std::make_shared<OHOS::NWeb::NWebMessage>(NWebValue::Type::NONE);
        ConvertCefValueToNWebMessage(message, data);
        callback_->OnReceiveValue(data);
        return (data && data->IsBoolean()) ? data->GetBoolean() : false;
    }
    return false;
  }

 private:
  std::shared_ptr<NWebMessageValueCallback> callback_;

  IMPLEMENT_REFCOUNTING(CefWebMessageReceiverImpl);
};
#endif  // BUILDFLAG(ARKWEB_MSGPORT)

class StoreWebArchiveResultCallbackImpl
    : public CefStoreWebArchiveResultCallback {
 public:
  StoreWebArchiveResultCallbackImpl(
      std::shared_ptr<NWebStringValueCallback> callback)
      : callback_(callback) {}
  void OnStoreWebArchiveDone(const CefString& result) override {
    if (callback_ != nullptr) {
      callback_->OnReceiveValue(result.ToString());
    }
  }

 private:
  std::shared_ptr<NWebStringValueCallback> callback_;

  IMPLEMENT_REFCOUNTING(StoreWebArchiveResultCallbackImpl);
};

#if BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)
class ScreenCaptureCallbackImpl : public CefScreenCaptureCallback {
 public:
  ScreenCaptureCallbackImpl(
      std::shared_ptr<NWebScreenCaptureDelegateCallback> callback)
      : callback_(callback) {}
  NO_SANITIZE("cfi-icall")
  void OnStateChange(int32_t nweb_id,
                     const CefString& session_id,
                     int32_t code) override {
    if (callback_ != nullptr) {
      LOG(INFO) << "ScreenCapture onStateChange nweb_id:" << nweb_id << ", session_id:" << session_id.ToString()
                << ", stateCode:" << code;
      callback_->OnStateChange(nweb_id, session_id.ToString().c_str(), code);
    }
  }

 private:
  std::shared_ptr<NWebScreenCaptureDelegateCallback> callback_;

  IMPLEMENT_REFCOUNTING(ScreenCaptureCallbackImpl);
};
#endif  // defined(ARKWEB_EX_SCREEN_CAPTURE)

class GetImagesCallbackImpl : public CefGetImagesCallback {
 public:
  explicit GetImagesCallbackImpl(
      std::shared_ptr<NWebBoolValueCallback> callback)
      : callback_(callback) {}

  void GetImages(bool response) override {
    if (callback_ != nullptr) {
      callback_->OnReceiveValue(response);
    }
  }

 private:
  std::shared_ptr<NWebBoolValueCallback> callback_;

  IMPLEMENT_REFCOUNTING(GetImagesCallbackImpl);
};

class CefPrecompileCallbackImpl : public CefPrecompileCallback {
 public:
  explicit CefPrecompileCallbackImpl(
      std::shared_ptr<NWebMessageValueCallback> callback)
      : callback_(callback) {}

  void OnPrecompileFinished(int32_t result) override {
    if (callback_ != nullptr) {
        auto message = std::make_shared<OHOS::NWeb::NWebMessage>(NWebValue::Type::INTEGER);
        message->SetInt64(result);
        callback_->OnReceiveValue(message);
    }
  }

 private:
  std::shared_ptr<NWebMessageValueCallback> callback_;

  IMPLEMENT_REFCOUNTING(CefPrecompileCallbackImpl);
};

#if BUILDFLAG(ARKWEB_PDF)
class CefPdfValueCallbackImpl : public CefPdfValueCallback {
 public:
  CefPdfValueCallbackImpl(
      std::shared_ptr<NWebArrayBufferValueCallback> callback,
      uint32_t callback_id,
      std::shared_ptr<NWebDelegateInterface> delegate)
      : callback_(std::move(callback)),
        callback_id_(callback_id),
        weak_nweb_delegate_(
            std::weak_ptr<NWebDelegateInterface>(std::move(delegate))) {}

  ~CefPdfValueCallbackImpl() override = default;

  void CallbackOnReceiveThread(std::shared_ptr<std::string> pdf_data) {
    if (callback_) {
      // Ensure callback_ is still valid before using it
      callback_->OnReceiveValue(pdf_data->c_str(), pdf_data->size());
    }
    // Post this instance to UI to destroy
    auto delegate = weak_nweb_delegate_.lock();
    if (delegate) {
      CEF_POST_TASK(
          CEF_UIT,
          base::BindOnce(&NWebDelegateInterface::EraseCreatePDFCallbackImpl,
                         delegate, callback_id_));
    }
  }

  void OnReceiveValue(const char* value, const long size) override {
    if (callback_) {
      auto pdf_data = std::make_shared<std::string>(value, size);
      base::ThreadPool::PostTask(
          FROM_HERE, {base::MayBlock(), base::TaskPriority::HIGHEST},
          base::BindOnce(
              [](CefPdfValueCallbackImpl* self,
                 std::shared_ptr<std::string> data) {
                self->CallbackOnReceiveThread(std::move(data));
              },
              base::Unretained(this), std::move(pdf_data)));
    }
  }

 private:
  std::shared_ptr<NWebArrayBufferValueCallback> callback_;
  uint32_t callback_id_;
  std::weak_ptr<NWebDelegateInterface> weak_nweb_delegate_;

  IMPLEMENT_REFCOUNTING(CefPdfValueCallbackImpl);
};
#endif  // BUILDFLAG(ARKWEB_PDF)
class CefCacheOptionsImpl : public CefCacheOptions {
 public:
  explicit CefCacheOptionsImpl(
      const std::shared_ptr<CacheOptions>& cacheOptions)
      : responseHeaders_(cacheOptions->GetResponseHeaders()) {}

  cef_string_map_t GetResponseHeaders() override {
    cef_string_map_t cefHeaders = cef_string_map_alloc();
    for (const auto& pair : responseHeaders_) {
      cef_string_t key = {};
      cef_string_t value = {};
      cef_string_from_utf8(pair.first.c_str(), pair.first.size(), &key);
      cef_string_from_utf8(pair.second.c_str(), pair.second.size(), &value);
      cef_string_map_append(cefHeaders, &key, &value);
    }
    return cefHeaders;
  }

 private:
  std::map<std::string, std::string> responseHeaders_;

  IMPLEMENT_REFCOUNTING(CefCacheOptionsImpl);
};

#if BUILDFLAG(ARKWEB_NAVIGATION)
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

#endif  // BUILDFLAG(ARKWEB_NAVIGATION)

NWebDelegate::NWebDelegate(int argc, const char* argv[])
    : argc_(argc), argv_(argv) {}

NWebDelegate::~NWebDelegate() {
  if (display_listener_id_ >= 0 && display_listener_ != nullptr &&
      display_manager_adapter_ != nullptr) {
    display_manager_adapter_->UnregisterDisplayListener(display_listener_id_);
  }
}

#if BUILDFLAG(ARKWEB_DRAG_DROP)
void NWebDelegate::InitAppTempDir() {
  for (int i = 0; i < argc_; i++) {
    if (argv_[i] == nullptr) {
      continue;
    }

    if (!strncmp(argv_[i], "--ohos-temp-dir=", strlen("--ohos-temp-dir="))) {
      const char* value = argv_[i] + strlen("--ohos-temp-dir=");
      ohos_temp_dir_ = value;
      LOG(INFO) << "DrapDrop InitAppTempDir --ohos-temp-dir="
                << ohos_temp_dir_.c_str();
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

    if (!strncmp(argv_[i],
                 "--init-richtext-data=", strlen("--init-richtext-data="))) {
      const char* value = argv_[i] + strlen("--init-richtext-data=");
      richtext_data_str_ = value;
      LOG(INFO) << "richtext InitRichtextIdentifier data"
                << richtext_data_str_.c_str();
    }
  }
}

bool NWebDelegate::Init(bool is_enhance_surface,
                        void* window,
                        bool popup
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
                        ,
                        uint32_t nweb_id
#endif
#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
                        ,
                        bool incognito_mode
#endif
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
                        ,
                        const std::string& shared_render_process_token
#endif
) {
  preference_delegate_ = std::make_shared<NWebPreferenceDelegate>();
  int32_t backgroundColor;
  if (preference_delegate_ && HasBackgroundColorWithInit(backgroundColor)) {
    // background color should set when init in case of first white screen flash
    preference_delegate_->SetBackgroundColor(backgroundColor);
  }
#if BUILDFLAG(ARKWEB_DRAG_DROP)
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

  display_listener_id_ =
      display_manager_adapter_->RegisterDisplayListener(display_listener_);
  if (display_listener_id_ < 0) {
    LOG(ERROR) << "RegisterDisplayListener failed";
  }

  render_handler_ = NWebRenderHandler::Create();
  if (render_handler_ == nullptr) {
    display_manager_adapter_->UnregisterDisplayListener(display_listener_id_);
    return false;
  }

  event_handler_ = NWebEventHandler::Create();
  if (event_handler_ == nullptr) {
    display_manager_adapter_->UnregisterDisplayListener(display_listener_id_);
    return false;
  }

  std::string url_for_init = "";

  LOG(INFO) << "NWebDelegate::Init incognito_mode:" << incognito_mode
            << "[shared]" << shared_render_process_token;
  InitializeCef(url_for_init, is_enhance_surface_, window, popup
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
                ,
                nweb_id
#endif
#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
                ,
                incognito_mode
#endif
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
                ,
                shared_render_process_token
#endif
  );
  std::shared_ptr<DisplayAdapter> display =
      display_manager_adapter_->GetDefaultDisplay();
  if (display != nullptr) {
    NotifyScreenInfoChanged(display->GetRotation(),
                            display->GetDisplayOrientation());
    if (!richtext_data_str_.empty()) {
      // Created a richtext component
      SetVirtualPixelRatio(richtextDisplayRatio);
    } else {
      SetVirtualPixelRatio(display->GetVirtualPixelRatio());
    }
#if BUILDFLAG(ARKWEB_AI)
    if (display->IsFoldable()) {
      foldstatus_listener_ =
          std::make_shared<FoldStatusScreenListener>(shared_from_this());
      if (foldstatus_listener_ == nullptr) {
        LOG(ERROR) << "foldstatus_listener_ init error";
      }
      foldstatus_listener_id_ =
          display_manager_adapter_->RegisterFoldStatusListener(
              foldstatus_listener_);
      if (foldstatus_listener_ == nullptr) {
        LOG(ERROR) << "foldStatus_listener_id_ init error";
      }
      OnFoldStatusChanged(display->GetFoldStatus());
    }
#endif
  }
#if BUILDFLAG(ARKWEB_WEBRTC)
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "can not get browser ,can not set NWeb id";
    return true;
  }
  GetBrowser()->GetHost()->SetNWebId(GetBrowser()->GetNWebId());
#endif  // BUILDFLAG(ARKWEB_WEBRTC)
  return true;
}

void NWebDelegate::OnDestroy(bool is_close_all) {
  if (display_listener_id_ >= 0 && display_listener_ != nullptr &&
      display_manager_adapter_ != nullptr) {
    display_manager_adapter_->UnregisterDisplayListener(display_listener_id_);
  }
#if BUILDFLAG(ARKWEB_AI)
  if (foldstatus_listener_id_ >= 0 && foldstatus_listener_ != nullptr &&
      display_manager_adapter_ != nullptr) {
    display_manager_adapter_->UnregisterFoldStatusListener(
        foldstatus_listener_id_);
  }
#endif
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
#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
  LOG(INFO) << "NWebDelegate::StartDownload";
#endif  // BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
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
#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
  LOG(INFO) << "NWebDelegate::ResumeDownload";
#else
  LOG(DEBUG) << "NWebDelegate::ResumeDownload";
#endif  // BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
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

#if BUILDFLAG(ARKWEB_EXT_DOWNLOAD)
NWebDownloadItemState NWebDelegate::GetDownloadItemState(long item_id) {
  auto browser = GetBrowser();
  if (browser == nullptr || browser->GetHost() == nullptr) {
    LOG(ERROR) << "GetDownloadItemState failed, for browser or browser->host "
                  "is nullptr";
    return NWebDownloadItemState::MAX_DOWNLOAD_STATE;
  }
  CefRefPtr<CefDownloadItem> download_item =
      browser->GetHost()->GetDownloadItem(item_id);
  if (!download_item) {
    LOG(ERROR) << "GetDownloadItemState failed, for download_item is nullptr";
    return NWebDownloadItemState::MAX_DOWNLOAD_STATE;
  }

  return NWebDownloadItem::GetNWebState(download_item);
}
#endif

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

#if BUILDFLAG(ARKWEB_NWEB_EX)
bool NWebDelegate::CanStoreWebArchive() const {
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

void NWebDelegate::GetImageFromContextNode() {
  auto browser = GetBrowser();
  if (browser != nullptr && browser->GetHost() != nullptr) {
    browser->GetHost()->GetImageForContextNode(MENU_ID_IMAGE_SHARE);
  }
}

void NWebDelegate::GetImageFromCacheEx(const std::string& url) {
  auto browser = GetBrowser();
  if (browser != nullptr && browser->GetHost() != nullptr) {
    browser->GetHost()->GetImageFromCacheEx(CefString(url), MENU_ID_FEED_SHARE);
  }
}

void NWebDelegate::RegisterWebExtensionListener(
    std::shared_ptr<NWebExtensionCallback> web_extension_listener) {
  if (handler_delegate_ == nullptr) {
    LOG(ERROR) << "fail to register web app client extension listener, nweb "
                  "handler delegate is nullptr";
    return;
  }
  handler_delegate_->RegisterWebExtensionListener(web_extension_listener);
}

void NWebDelegate::UnRegisterWebExtensionListener() {
  if (handler_delegate_ == nullptr) {
    LOG(ERROR) << "fail to unregister web app client extension listener, nweb "
                  "handler delegate is nullptr";
    return;
  }
  handler_delegate_->UnRegisterWebExtensionListener();
}

#endif  // BUILDFLAG(ARKWEB_NWEB_EX)

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

  if (handler_delegate_ == nullptr) {
    LOG(ERROR)
        << "fail to register inputmethod client, delegate handler is nullptr";
    return;
  }
  handler_delegate_->SetInputMethodClient(client);
}

void NWebDelegate::RegisterRenderCb(
    std::function<void(const char*)> render_update_cb) {
  if (render_handler_ != nullptr) {
    render_handler_->RegisterRenderCb(render_update_cb);
  }
}

#if BUILDFLAG(ARKWEB_SCREEN_OFFSET)
void NWebDelegate::SetScreenOffset(double x, double y) {
  if (display_ratio_ == 0) {
    return;
  }
  if (render_handler_ == nullptr) {
    return;
  }

  double origin_x = 0;
  double origin_y = 0;
  render_handler_->GetScreenOffset(nullptr, origin_x, origin_y);
  double new_x = std::round(x / display_ratio_);
  double new_y = std::round(y / display_ratio_);
  if (origin_x == new_x && origin_y == new_y) {
    return;
  }
  render_handler_->SetScreenOffset(new_x, new_y);
  auto browser = GetBrowser();
  if (browser != nullptr && browser->GetHost() != nullptr) {
    browser->GetHost()->NotifyScreenInfoChangedV2();
  }
}
#endif  // BUILDFLAG(ARKWEB_SCREEN_OFFSET)

void NWebDelegate::Resize(uint32_t width, uint32_t height, bool isKeyboard) {
#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
  if (width == width_ && height == height_) {
    render_handler_->OnResizeNotWork();
    return;
  }

  TRACE_EVENT2("base", "NWebDelegate::Resize", "width", width, "height",
               height);
  width_ = width;
  height_ = height;
#endif  // BUILDFLAG(ARKWEB_COMPOSITE_RENDER)

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
    browser->GetHost()->OnTextSelected(false);
  }
#if BUILDFLAG(ARKWEB_VIEWPORT_AVOID)
  if (avoid_height_ != 0) {
    AvoidVisibleViewportBottom(avoid_height_);
  }
#endif
}

#if BUILDFLAG(ARKWEB_INPUT_EVENTS) || BUILDFLAG(ARKWEB_VIEWPORT_AVOID)
void NWebDelegate::ResizeVisibleViewport(uint32_t width,
                                         uint32_t height,
                                         bool isKeyboard) {
  if (width == visible_width_ && height == visible_height_) {
    return;
  }

  TRACE_EVENT2("base", "NWebDelegate::ResizeVisibleViewport", "width", width,
               "height", height);
  visible_width_ = width;
  visible_height_ = height;

  if (render_handler_ != nullptr) {
    render_handler_->ResizeVisibleViewport(width, height);
  }

  auto browser = GetBrowser();
  if (browser != nullptr && browser->GetHost() != nullptr) {
    if (isKeyboard) {
      browser->GetHost()->WasKeyboardResized();
    } else {
      browser->GetHost()->WasResized();
    }
    browser->GetHost()->OnTextSelected(false);
  }
}
#endif

void NWebDelegate::OnTouchPress(int32_t id,
                                double x,
                                double y,
                                bool from_overlay) {
  if (event_handler_ != nullptr) {
    if (pressing_num_ < 0) {
      pressing_num_ = 0;
    }
    ++pressing_num_;
    event_handler_->OnTouchPress(id, x / default_virtual_pixel_ratio_,
                                 y / default_virtual_pixel_ratio_,
                                 from_overlay);
  }
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  if (render_handler_ != nullptr) {
    render_handler_->SetIrregularDragBackground(true);
  }
#endif  // #if BUILDFLAG(ARKWEB_DRAG_DROP)
}

void NWebDelegate::OnTouchRelease(int32_t id,
                                  double x,
                                  double y,
                                  bool from_overlay) {
  if (event_handler_ != nullptr) {
    --pressing_num_;
    event_handler_->OnTouchRelease(id, x / default_virtual_pixel_ratio_,
                                   y / default_virtual_pixel_ratio_,
                                   from_overlay);
  }
}

void NWebDelegate::OnTouchMove(
    const std::vector<std::shared_ptr<NWebTouchPointInfo>>& touch_point_infos,
    bool from_overlay) {
  if (event_handler_ == nullptr) {
    return;
  }

  event_handler_->OnTouchMove(touch_point_infos, from_overlay,
                              default_virtual_pixel_ratio_);
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
    --pressing_num_;
    event_handler_->OnTouchCancel();
  }
}

void NWebDelegate::OnTouchCancelById(int32_t id,
                                     double x,
                                     double y,
                                     bool from_overlay) {
  if (event_handler_ != nullptr) {
    --pressing_num_;
    event_handler_->OnTouchCancelById(id, x / default_virtual_pixel_ratio_,
                                      y / default_virtual_pixel_ratio_,
                                      from_overlay);
  }
}

bool NWebDelegate::SendKeyEvent(int32_t keyCode, int32_t keyAction) {
  bool retVal = false;
  if (event_handler_ != nullptr) {
    retVal = event_handler_->SendKeyEventFromAce(keyCode, keyAction);
  }
  return retVal;
}

void NWebDelegate::SendTouchpadFlingEvent(double x,
                                          double y,
                                          double vx,
                                          double vy) {
  if (event_handler_ != nullptr) {
    event_handler_->SendTouchpadFlingEvent(
        x / default_virtual_pixel_ratio_, y / default_virtual_pixel_ratio_,
        vx / default_virtual_pixel_ratio_, vy / default_virtual_pixel_ratio_);
  }
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
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  if (event_handler_ != nullptr &&
      (!handler_delegate_ || !handler_delegate_->IsDragEnter())) {
#else
  if (event_handler_ != nullptr) {
#endif  // #if BUILDFLAG(ARKWEB_DRAG_DROP)
    event_handler_->SendMouseEvent(x / default_virtual_pixel_ratio_,
                                   y / default_virtual_pixel_ratio_, button,
                                   action, count);
  }
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  if (render_handler_ != nullptr) {
    render_handler_->SetIrregularDragBackground(false);
  }
#endif  // #if BUILDFLAG(ARKWEB_DRAG_DROP)
}

void NWebDelegate::NotifyScreenInfoChanged(RotationType rotation,
                                           DisplayOrientation orientation) {
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
      if (display_ratio_ == 0.0) {
        display_ratio = display->GetVirtualPixelRatio();
      } else {
        display_ratio = display_ratio_;
      }
    }
    if (display_ratio <= 0) {
      LOG(ERROR) << "Invalid display_ratio, display_ratio = " << display_ratio;
      return;
    }
    int width = std::ceil(display->GetWidth() / display_ratio);
    int height = std::ceil(display->GetHeight() / display_ratio);
#if BUILDFLAG(ARKWEB_SCREEN_ROTATION)
    bool default_portrait = display_manager_adapter_->IsDefaultPortrait();
    render_handler_->SetScreenInfo({rotation, orientation, width, height,
                                    display_ratio, default_portrait});
#endif  // #if BUILDFLAG(ARKWEB_SCREEN_ROTATION)
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

  ui::GestureConfiguration::GetInstance()->set_virtual_pixel_ratio(
      default_virtual_pixel_ratio_);
  if (preference_delegate_) {
    preference_delegate_->SetVirtualPixelRatio(ratio);
  }
}

std::shared_ptr<NWebPreference> NWebDelegate::GetPreference() const {
  return preference_delegate_;
}

bool NWebDelegate::IsFileProtocol(const GURL& gurl) {
  if (gurl.is_empty() || !gurl.is_valid()) {
    return false;
  }
  std::string fileProtocolName = "file://";
  if (gurl.spec().substr(0, fileProtocolName.size()) == fileProtocolName) {
    return true;
  }
  return false;
}

bool NWebDelegate::IsUrlFileExist(const GURL& gurl, const std::string& url) {
  if (url == "file://") {
    return false;
  }

  base::FilePath filePath;
  std::string unscaped_url_str = base::UnescapeURLComponent(
      gurl.spec(),
      base::UnescapeRule::URL_SPECIAL_CHARS_EXCEPT_PATH_SEPARATORS);
  GURL::Replacements repl;
  repl.ClearRef();
  GURL gurl_no_ref = GURL(unscaped_url_str).ReplaceComponents(repl);
  if (net::FileURLToFilePath(gurl_no_ref, &filePath) && base::PathExists(filePath)) {
    return true;
  }

  // if url start with file:// and end whih *#*.html UnescapeURLComponent function will delete char after #
  // PathExists will return false but file is actually exist. check again with no unscaped will solve.
  base::FilePath originFilePath;
  GURL origin_gurl_no_ref = GURL(url).ReplaceComponents(repl);
  if (net::FileURLToFilePath(origin_gurl_no_ref, &originFilePath) && base::PathExists(originFilePath)) {
    return true;
  }

  LOG(ERROR) << "IsUrlFileExist failed, file does not exist";
  return false;
}

int NWebDelegate::Load(const std::string& url) {
  GURL gurl = GURL(url);
  if (gurl.is_empty() || !gurl.is_valid()) {
    GURL gurlWithHttp = GURL("https://" + url);
    if (!gurlWithHttp.is_valid()) {
      return NWEB_INVALID_URL;
    }
  }

  GURL file_gurl = url_util::FixupGURL(url);
  if (IsFileProtocol(file_gurl) && !IsUrlFileExist(file_gurl, url)) {
    return NWEB_INVALID_RESOURCE;
  }
  LOG(DEBUG) << "NWebDelegate::Load url: ***";
  auto browser = GetBrowser();
  if (browser == nullptr) {
    LOG(ERROR) << "NWebDelegate::Load browser is nullptr";
    return NWEB_ERR;
  }
  browser->GetMainFrame()->LoadURL(CefString(url));
  RequestVisitedHistory();
  return NWEB_OK;
}

#if BUILDFLAG(ARKWEB_POST_URL)
int NWebDelegate::PostUrl(const std::string& url,
                          const std::vector<char>& postData) {
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
  browser->GetMainFrame()->AsArkWebFrame()->PostURL(CefString(url), postData);
  RequestVisitedHistory();
  return NWEB_OK;
}
#endif  // BUILDFLAG(ARKWEB_POST_URL)

bool NWebDelegate::IsNavigatebackwardAllowed() const {
  if (GetBrowser().get()) {
    return GetBrowser()->CanGoBack();
  }
  return false;
}

bool NWebDelegate::IsNavigateForwardAllowed() const {
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

bool NWebDelegate::TerminateRenderProcess() {
  LOG(DEBUG) << "NWebDelegate::TerminateRenderProcess";
  if (GetBrowser().get()) {
    return GetBrowser()->GetHost()->TerminateRenderProcess();
  }
  return false;
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
    std::shared_ptr<NWebStringValueCallback> callback) const {
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

void NWebDelegate::ExecuteJavaScriptExt(
    const int fd,
    const size_t scriptLength,
    std::shared_ptr<NWebMessageValueCallback> callback,
    bool extention) {
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(
        CEF_UIT,
        base::BindOnce((void(NWebDelegate::*)(
                           const int fd, const size_t scriptLength,
                           std::shared_ptr<NWebMessageValueCallback>, bool)) &
                           NWebDelegate::ExecuteJavaScriptExt,
                       this, fd, scriptLength, callback, extention));
    return;
  }

  if (GetBrowser().get()) {
    runJSCallbackId_++;
    CefRefPtr<JavaScriptResultCallbackImpl> JsResultCb =
        new JavaScriptResultCallbackImpl(callback, runJSCallbackId_,
                                         shared_from_this());
    runJSCallbackMap_[runJSCallbackId_] = JsResultCb;
    GetBrowser()->GetHost()->ExecuteJavaScriptExt(
        fd, static_cast<uint64_t>(scriptLength), JsResultCb, extention);
  }
}

#if BUILDFLAG(ARKWEB_PDF)
void NWebDelegate::EraseCreatePDFCallbackImpl(uint32_t id) {
  if (create_pdf_value_callback_map_.count(id)) {
    create_pdf_value_callback_map_.erase(id);
  }
}

void NWebDelegate::ExecuteCreatePDFExt(
    std::shared_ptr<NWebPDFConfigArgs> pdf_config,
    std::shared_ptr<NWebArrayBufferValueCallback> callback) {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "ExecuteCreatePDFExt can not get browser";
    return;
  }
  CefPdfPrintSettings settings;
  settings.margin_left = pdf_config->GetMarginLeft();
  settings.margin_right = pdf_config->GetMarginRight();
  settings.margin_top = pdf_config->GetMarginTop();
  settings.margin_bottom = pdf_config->GetMarginBottom();
  settings.paper_width = pdf_config->GetWidth();
  settings.paper_height = pdf_config->GetHeight();
  settings.scale = pdf_config->GetScale();
  if (pdf_config->GetShouldPrintBackground()) {
    settings.print_background = 1;
  } else {
    settings.print_background = 0;
  }
  settings.landscape = 0;
  settings.margin_type = PDF_PRINT_MARGIN_CUSTOM;
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(
        CEF_UIT,
        base::BindOnce(
            (void(NWebDelegate::*)(
                std::shared_ptr<NWebPDFConfigArgs> pdf_config,
                std::shared_ptr<NWebArrayBufferValueCallback> callback)) &
                NWebDelegate::ExecuteCreatePDFExt,
            this, pdf_config, callback));
    return;
  }

  if (GetBrowser().get()) {
    create_pdf_value_callback_id_++;
    CefRefPtr<CefPdfValueCallbackImpl> js_result_callback =
        new CefPdfValueCallbackImpl(callback, create_pdf_value_callback_id_,
                                    shared_from_this());
    create_pdf_value_callback_map_[create_pdf_value_callback_id_] =
        js_result_callback;
    GetBrowser()->GetHost()->CreateToPDF(settings, js_result_callback);
  }
}
#endif  // BUILDFLAG(ARKWEB_PDF)

#if BUILDFLAG(ARKWEB_MSGPORT)
void NWebDelegate::EraseJavaScriptCallbackImpl(uint32_t id) {
  if (runJSCallbackMap_.count(id)) {
    runJSCallbackMap_.erase(id);
  }
}

void NWebDelegate::ExecuteJavaScript(
    const std::string& code,
    std::shared_ptr<NWebMessageValueCallback> callback,
    bool extention) {
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(
        CEF_UIT,
        base::BindOnce((void(NWebDelegate::*)(
                           const std::string&,
                           std::shared_ptr<NWebMessageValueCallback>, bool)) &
                           NWebDelegate::ExecuteJavaScript,
                       this, code, callback, extention));
    return;
  }

  if (GetBrowser().get()) {
    runJSCallbackId_++;
    CefRefPtr<JavaScriptResultCallbackImpl> JsResultCb =
        new JavaScriptResultCallbackImpl(callback, runJSCallbackId_,
                                         shared_from_this());
    runJSCallbackMap_[runJSCallbackId_] = JsResultCb;
    GetBrowser()->GetHost()->ExecuteJavaScript(code, JsResultCb, extention);
  }
}
#endif  // BUILDFLAG(ARKWEB_MSGPORT)

void NWebDelegate::PutBackgroundColor(int color) const {
  LOG(DEBUG) << "NWebDelegate::PutBackgroundColor color: " << (uint32_t)color;
  if (GetBrowser().get()) {
    GetBrowser()->GetHost()->SetBackgroundColor(color);
  }

#if BUILDFLAG(ARKWEB_BACKGROUND_COLOR)
  if (preference_delegate_) {
    preference_delegate_->SetBackgroundColor(color);
  }
#endif  // BUILDFLAG(ARKWEB_BACKGROUND_COLOR)
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

void NWebDelegate::PutOptimizeParserBudgetEnabled(bool enable) const {
  LOG(DEBUG) << "NWebDelegate::PutOptimizeParserBudgetEnabled";
  if (GetBrowser().get()) {
    GetBrowser()->GetHost()->SetOptimizeParserBudgetEnabled(enable);
  }
}

void NWebDelegate::OnPause() {
  LOG(DEBUG) << "NWebDelegate::OnPause, nweb_id = " << nweb_id_;
  if (!GetBrowser().get()) {
    return;
  }

  // Remove focus from the browser.
  GetBrowser()->GetHost()->SetFocus(false);

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  if (handler_delegate_) {
    handler_delegate_->SetFocusState(false);
  }
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

  if (!hidden_) {
    // Set the browser as hidden.
    LOG(DEBUG) << "NWebDelegate::OnPause set hidden, nweb_id = " << nweb_id_;
    GetBrowser()->GetHost()->WasHidden(true);
    hidden_ = true;
  }

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  is_onPause_ = true;
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
}

void NWebDelegate::OnWindowShow() {
  TRACE_EVENT1("base", "NWebDelegate::OnWindowShow", "nweb id = ", nweb_id_);
  LOG(INFO) << "NWebDelegate::OnWindowShow, nweb id = " << nweb_id_;
  if (!GetBrowser().get()) {
    return;
  }
  GetBrowser()->GetHost()->OnWindowShow();
}

void NWebDelegate::OnWindowHide() {
  TRACE_EVENT1("base", "NWebDelegate::OnWindowHide", "nweb id = ", nweb_id_);
  LOG(INFO) << "NWebDelegate::OnWindowHide, nweb id = " << nweb_id_;
  if (!GetBrowser().get()) {
    return;
  }
  GetBrowser()->GetHost()->OnWindowHide();
}

void NWebDelegate::OnOnlineRenderToForeground() {
  TRACE_EVENT1("base", "NWebDelegate::OnOnlineRenderToForeground",
               "nweb id = ", nweb_id_);
  LOG(INFO) << "NWebDelegate::OnOnlineRenderToForeground, nweb id = "
            << nweb_id_;
  if (!GetBrowser().get()) {
    return;
  }
  GetBrowser()->GetHost()->OnOnlineRenderToForeground();
}

void NWebDelegate::NotifyForNextTouchEvent() {
  TRACE_EVENT1("base", "NWebDelegate::NotifyForNextTouchEvent",
               "nweb id = ", nweb_id_);

  if (event_handler_ != nullptr) {
    event_handler_->NotifyForNextTouchEvent();
  }
}

#if BUILDFLAG(ARKWEB_ACTIVE_POLICY)
void NWebDelegate::SetDelayDurationForBackgroundTabFreezing(int64_t delay) {
  if (preference_delegate_) {
    preference_delegate_->SetDelayDurationForBackgroundTabFreezing(delay);
  }
}
#endif

void NWebDelegate::SetAutofillCallback(
    std::shared_ptr<NWebMessageValueCallback> callback) {
  CefRefPtr<CefWebMessageReceiver> JsResultCb =
      new CefWebMessageReceiverImpl(callback);

  if (GetBrowser() && GetBrowser()->GetHost()) {
    GetBrowser()->GetHost()->SetAutofillCallback(JsResultCb);
  } else if (preference_delegate_) {
    preference_delegate_->SetAutofillCallback(JsResultCb);
  }
}

void NWebDelegate::FillAutofillData(std::shared_ptr<NWebMessage> data) {
  if (!GetBrowser().get()) {
    return;
  }

  CefRefPtr<CefValue> message = CefValue::Create();
  ConvertNWebMsgToCefValue(data, message);
  GetBrowser()->GetHost()->FillAutofillData(message);
}

void NWebDelegate::OnContinue() {
  LOG(DEBUG) << "NWebDelegate::OnContinue, nweb_id = " << nweb_id_;
  if (!GetBrowser().get()) {
    return;
  }

  if (occluded_) {
    LOG(DEBUG) << "NWebDelegate::OnContinue set occluded, nweb_id = "
               << nweb_id_;
    hidden_ = false;
    GetBrowser()->GetHost()->WasOccluded(true);
    return;
  }

  if (hidden_) {
    // Set the browser as visible.
    LOG(DEBUG) << "NWebDelegate::OnContinue set unhidden, nweb_id = "
               << nweb_id_;
    GetBrowser()->GetHost()->WasHidden(false);

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
    if (GetBrowser()->GetHost()->NeedsReload()) {
      LOG(INFO) << "NWebDelegate::OnContinue restore.";
      GetBrowser()->GetHost()->Restore();
      GetBrowser()->GetHost()->SetNeedsReload(false);
    }
#endif
    hidden_ = false;
  }

  // Give focus to the browser.
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  if (handler_delegate_ && handler_delegate_->GetContinueNeedFocus()) {
    if (is_onPause_) {
      GetBrowser()->GetHost()->SetFocus(true);
      handler_delegate_->SetContinueNeedFocus(false);
    }
  } else if (handler_delegate_ && !handler_delegate_->GetFocusState() &&
             handler_delegate_->IsCurrentFocus()) {
    handler_delegate_->SetFocusState(true);
    GetBrowser()->GetHost()->SetFocus(true);
  }
  is_onPause_ = false;
#endif
}

void NWebDelegate::WebComponentsBlur() {
  LOG(INFO) << "NWebDelegate::WebComponentsBlur, nweb_id = " << nweb_id_;
  if (!GetBrowser().get()) {
    return;
  }
  GetBrowser()->GetHost()->SetFocusOnWeb();
}

void NWebDelegate::OnOccluded() {
  LOG(INFO) << "NWebDelegate::OnOccluded, nweb_id = " << nweb_id_;
  TRACE_EVENT1("base", "NWebDelegate::OnOccluded", "nweb id = ", nweb_id_);
  if (!GetBrowser().get()) {
    return;
  }

  if (!hidden_ && !occluded_) {
    // Set the browser as occluded.
    LOG(INFO) << "NWebDelegate::OnOccluded set occluded, nweb_id = "
              << nweb_id_;
    TRACE_EVENT1("base", "NWebDelegate::OnOccluded set occluded",
                 "nweb id = ", nweb_id_);
    GetBrowser()->GetHost()->WasOccluded(true);
  }
  occluded_ = true;
}

void NWebDelegate::OnUnoccluded() {
  LOG(INFO) << "NWebDelegate::OnUnoccluded, nweb_id = " << nweb_id_;
  TRACE_EVENT1("base", "NWebDelegate::OnUnoccluded", "nweb id = ", nweb_id_);
  if (!GetBrowser().get()) {
    return;
  }

  if (!hidden_ && occluded_) {
    // Set the browser as visible.
    LOG(INFO) << "NWebDelegate::OnUnoccluded set unoccluded, nweb_id = "
              << nweb_id_;
    TRACE_EVENT1("base", "NWebDelegate::OnUnoccluded set unoccluded",
                 "nweb id = ", nweb_id_);
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

void NWebDelegate::SetEnableHalfFrameRate(bool enabled) {
  LOG(DEBUG) << "NWebDelegate::SetEnableHalfFrameRate, nweb_id = " << nweb_id_;
  if (!GetBrowser().get()) {
    return;
  }

  GetBrowser()->GetHost()->SetEnableHalfFrameRate(enabled);
}

void NWebDelegate::InitializeCef(std::string url,
                                 bool is_enhance_surface,
                                 void* window,
                                 bool popup
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
                                 ,
                                 uint32_t nweb_id
#endif
#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
                                 ,
                                 bool incognito_mode
#endif
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
                                 ,
                                 const std::string& shared_render_process_token
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

#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
  handler_delegate_->SetNWebId(nweb_id);
#endif

#if BUILDFLAG(ARKWEB_HAP_DECOMPRESSED)
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

  bool excludable_devices =
      base::ohos::IsTabletDevice() || base::ohos::IsPcDevice();
  settings.persist_session_cookies = !excludable_devices;

#if !defined(CEF_USE_SANDBOX)
  settings.no_sandbox = true;
#endif

#if BUILDFLAG(ARKWEB_API_INIT_WEB_ENGINE)
#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
  settings.incognito_mode = incognito_mode;
#endif
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
  settings.shared_render_process_token = shared_render_process_token;
#endif
  bool is_initialized = NWebApplication::GetDefault()->HasInitializedCef();
  LOG(ERROR) << "NWebDelegate::InitializeCef, is_initialized:"
             << is_initialized;
  if (is_initialized) {
    NWebApplication::GetDefault()->CreateBrowser(preference_delegate_, url,
                                                 handler_delegate_, window
#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
                                                 ,
                                                 incognito_mode
#endif
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
                                                 ,
                                                 shared_render_process_token
#endif
    );
  } else {
    // Create browser when context initialized.
    NWebApplication::GetDefault()->RunAfterContextInitialized(base::BindOnce(
        &NWebDelegate::OnContextInitializeComplete, base::Unretained(this), url,
        window
#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
        ,
        incognito_mode
#endif
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
        ,
        shared_render_process_token
#endif

        ));
    NWebApplication::GetDefault()->InitializeCef(mainargs, settings);
  }
#endif  // BUILDFLAG(ARKWEB_API_INIT_WEB_ENGINE)
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

#if BUILDFLAG(ARKWEB_MSGPORT)
std::vector<std::string> NWebDelegate::CreateWebMessagePorts() {
  std::vector<std::string> ports;
  if (!GetBrowser().get()) {
    LOG(ERROR) << "JSAPI CreateWebMessagePorts can not get browser";
    return ports;
  }
  std::vector<CefString> cefPorts;
  GetBrowser()->GetHost()->CreateWebMessagePorts(cefPorts);

  for (CefString port : cefPorts) {
    ports.push_back(port.ToString());
  }
  return ports;
}

void NWebDelegate::PostWebMessage(const std::string& message,
                                  const std::vector<std::string>& ports,
                                  const std::string& targetUri) {
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

void NWebDelegate::ClosePort(const std::string& portHandle) {
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

void NWebDelegate::PostPortMessage(const std::string& portHandle,
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
    const std::string& portHandle,
    std::shared_ptr<NWebMessageValueCallback> callback) {
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
#endif  // BUILDFLAG(ARKWEB_MSGPORT)

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

std::shared_ptr<HitTestResult> NWebDelegate::GetHitTestResult() const {
  std::shared_ptr<HitTestResultImpl> data =
      std::make_shared<HitTestResultImpl>();
  if (!GetBrowser().get()) {
    return data;
  }
  int type;
  CefString extra_data;
  GetBrowser()->GetHost()->GetHitData(type, extra_data);
  data->SetType(type);
  data->SetExtra(extra_data.ToString());
  return data;
}

std::shared_ptr<HitTestResult> NWebDelegate::GetLastHitTestResult() const {
  std::shared_ptr<HitTestResultImpl> data =
      std::make_shared<HitTestResultImpl>();
  if (!GetBrowser().get()) {
    return data;
  }
  int type;
  CefString extra_data;
  GetBrowser()->GetHost()->GetLastHitData(type, extra_data);
  data->SetType(type);
  data->SetExtra(extra_data.ToString());
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
    const std::string& url,
    const std::map<std::string, std::string>& additionalHttpHeaders) {
  GURL gurl = GURL(url);
  if (gurl.is_empty() || !gurl.is_valid()) {
    GURL gurlWithHttp = GURL("https://" + url);
    if (!gurlWithHttp.is_valid()) {
      return NWEB_INVALID_URL;
    }
  }
  GURL file_gurl = url_util::FixupGURL(url);
  if (IsFileProtocol(file_gurl) && !IsUrlFileExist(file_gurl, url)) {
    return NWEB_INVALID_RESOURCE;
  }
  std::string extra = "";
  for (auto iter = additionalHttpHeaders.begin();
       iter != additionalHttpHeaders.end(); iter++) {
    const std::string& key = iter->first;
    const std::string& value = iter->second;
    if (!key.empty()) {
      extra += "\r\n" + std::string(key) + ": " + std::string(value);
    }
  }
  auto browser = GetBrowser();
  if (browser == nullptr) {
    return NWEB_ERR;
  }
  browser->GetMainFrame()->AsArkWebFrame()->LoadHeaderUrl(CefString(url),
                                                          CefString(extra));
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

const CefRefPtr<ArkWebBrowserExt> NWebDelegate::GetBrowser() const {
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

#if BUILDFLAG(ARKWEB_ADBLOCK)
void NWebDelegate::UpdateAdblockEasyListRules(long adBlockEasyListVersion) {
  if (GetBrowser() && GetBrowser()->GetHost()) {
    GetBrowser()->GetHost()->UpdateAdblockEasyListRules(adBlockEasyListVersion);
  }
}
#endif

void NWebDelegate::RegisterArkJSfunction(
    const std::string& object_name,
    const std::vector<std::string>& method_list,
    const std::vector<std::string>& async_method_list,
    const int32_t object_id,
    const std::string& permission) const {
  LOG(INFO) << "RegisterArkJSfunction name : " << object_name.c_str();
  std::vector<CefString> method_vector;
  for (std::string method : method_list) {
    method_vector.push_back(method);
  }
  std::vector<CefString> async_method_vector;
  for (std::string method : async_method_list) {
    async_method_vector.push_back(method);
  }

  if (is_popup_ready_) {
    if (handler_delegate_) {
      LOG(INFO) << "NWebDelegate::RegisterArkJSfunction popup case, the "
                   "object_name is "
                << object_name.c_str();
      handler_delegate_->SavaArkJSFunctionForPopup(
          object_name, method_list, async_method_list, object_id, permission);
    }
  }
  if (!GetBrowser()) {
    LOG(ERROR) << "NWebDelegate::RegisterArkJSfunction fail due to "
                  "GetBrowser() return null, the object_name is "
               << object_name.c_str();
    return;
  } else {
    GetBrowser()->GetHost()->RegisterArkJSfunction(
        object_name, method_vector, async_method_vector, object_id, permission);
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
    const std::vector<std::shared_ptr<NWebJsProxyCallback>>& callbacks) {
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(CEF_UIT,
                  base::BindOnce(&NWebDelegate::RegisterNativeArkJSFunction,
                                 this, objName, callbacks));
    return;
  }

  handler_delegate_->RegisterNativeJavaScriptCallBack(objName, callbacks);
  std::vector<CefString> method_vector;
  for (auto callback : callbacks) {
    method_vector.push_back(callback->GetMethodName());
  }
  if (GetBrowser() && GetBrowser()->GetHost()) {
    GetBrowser()->GetHost()->RegisterArkJSfunction(objName, method_vector,
                                                   std::vector<CefString>(),
                                                   kDefaultWebNativeProxy, "");
  } else {
    LOG(ERROR) << "browser or host is null";
  }
}

void NWebDelegate::RegisterNativeJSProxy(
    const std::string& objName,
    const std::vector<std::string>& methodName,
    std::vector<std::function<char*(std::vector<std::vector<uint8_t>>&,
                                    std::vector<size_t>&)>>&& callback,
    bool isAsync,
    const std::string& permission) {
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(
        CEF_UIT,
        base::BindOnce(&NWebDelegate::RegisterNativeJSProxy, this, objName,
                       methodName, std::move(callback), isAsync, permission));
    return;
  }

  if (handler_delegate_ == nullptr) {
    LOG(ERROR) << "handler_delegate_ is nullptr";
    return;
  }

  handler_delegate_->RegisterNativeJavaScriptCallBack(
      objName, methodName, std::move(callback), isAsync, permission);

  size_t size = methodName.size();
  std::vector<CefString> method_vector;
  for (size_t i = 0; i < size; i++) {
    method_vector.push_back(methodName[i]);
  }
  if (GetBrowser() && GetBrowser()->GetHost()) {
    GetBrowser()->GetHost()->RegisterNativeJSProxy(
        objName, method_vector, kDefaultWebNativeProxy, isAsync, permission);
  } else {
    LOG(ERROR) << "browser or host is null";
  }
}

void NWebDelegate::RegisterNativeJSProxyWithResult(
    const std::string& objName,
    const std::vector<std::string>& methodName,
    std::vector<std::function<std::shared_ptr<OHOS::NWeb::NWebValue>(
        std::vector<std::vector<uint8_t>>&,
        std::vector<size_t>&)>>&& callback,
    bool isAsync,
    const std::string& permission) {
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(CEF_UIT,
                  base::BindOnce(&NWebDelegate::RegisterNativeJSProxyWithResult,
                                 this, objName, methodName, std::move(callback),
                                 isAsync, permission));
    return;
  }

  if (handler_delegate_ == nullptr) {
    LOG(ERROR) << "handler_delegate_ is nullptr";
    return;
  }

  handler_delegate_->RegisterNativeJavaScriptCallBackWithResult(
      objName, methodName, std::move(callback), isAsync, permission);

  size_t size = methodName.size();
  std::vector<CefString> method_vector;
  for (size_t i = 0; i < size; i++) {
    method_vector.push_back(methodName[i]);
  }
  if (GetBrowser() && GetBrowser()->GetHost()) {
    GetBrowser()->GetHost()->RegisterNativeJSProxy(
        objName, method_vector, kDefaultWebNativeProxy, isAsync, permission);
  } else {
    LOG(ERROR) << "browser or host is null";
  }
}

void NWebDelegate::RegisterNativeAsyncThreadJSProxyWithResult(
    const std::string& objName,
    const std::vector<std::string>& methodName,
    std::vector<std::function<std::shared_ptr<OHOS::NWeb::NWebValue>(
        std::vector<std::vector<uint8_t>>&,
        std::vector<size_t>&)>>&& callback,
    const std::string& permission) {
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(CEF_UIT,
                  base::BindOnce(&NWebDelegate::RegisterNativeAsyncThreadJSProxyWithResult,
                                 this, objName, methodName, std::move(callback),
                                 permission));
    return;
  }

  if (handler_delegate_ == nullptr) {
    LOG(ERROR) << "handler_delegate_ is nullptr";
    return;
  }

  handler_delegate_->RegisterNativeAsyncThreadJavaScriptCallBackWithResult(
      objName, methodName, std::move(callback), permission);

  size_t size = methodName.size();
  std::vector<CefString> method_vector;
  for (size_t i = 0; i < size; i++) {
    method_vector.push_back(methodName[i]);
  }
  if (GetBrowser() && GetBrowser()->GetHost()) {
    GetBrowser()->GetHost()->RegisterNativeJSProxy(
        objName, method_vector, kDefaultWebNativeProxy, false, permission);
  } else {
    LOG(ERROR) << "browser or host is null";
  }
}

void NWebDelegate::UnRegisterNativeArkJSFunction(const char* objName) {
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(CEF_UIT,
                  base::BindOnce(&NWebDelegate::UnRegisterNativeArkJSFunction,
                                 this, objName));
    return;
  }

  std::vector<CefString> method_vector;
  if (GetBrowser() && GetBrowser()->GetHost()) {
    GetBrowser()->GetHost()->UnregisterArkJSfunction(objName, method_vector);
  } else {
    LOG(ERROR) << "browser or host is null";
  }
}

void NWebDelegate::RegisterNativeLoadStartCallback(
    std::function<void(void)>&& callback) {
  if (handler_delegate_ != nullptr) {
    handler_delegate_->RegisterNativeLoadStartCallback(std::move(callback));
  } else {
    LOG(ERROR) << "handler_delegate_ is nullptr";
  }
}

void NWebDelegate::RegisterNativeLoadEndCallback(
    std::function<void(void)>&& callback) {
  if (handler_delegate_ != nullptr) {
    handler_delegate_->RegisterNativeLoadEndCallback(std::move(callback));
  } else {
    LOG(ERROR) << "handler_delegate_ is nullptr";
  }
}

void NWebDelegate::RegisterNativeScrollCallback(
    std::function<void(double, double)>&& callback) {
  if (render_handler_ != nullptr) {
    render_handler_->RegisterNativeScrollCallback(std::move(callback));
  } else {
    LOG(ERROR) << "render_handler_ is nullptr";
  }
}

#if BUILDFLAG(ARKWEB_JSPROXY)
void NWebDelegate::JavaScriptOnDocumentStart(const ScriptItems& scriptItems) {
  if (GetBrowser() != nullptr && GetBrowser()->GetHost() != nullptr) {
    int count = 0;
    if (scriptItems.size() == 0) {
      GetBrowser()->GetHost()->JavaScriptOnDocumentStart("", std::vector<CefString>(),
                                                         true);
    }
    for (auto item : scriptItems) {
      count++;
      CefString script = item.first;
      std::vector<CefString> scriptRules;
      for (std::string rule : item.second) {
        CefString cefRule;
        cefRule.FromString(rule);
        scriptRules.push_back(cefRule);
      }
      GetBrowser()->GetHost()->JavaScriptOnDocumentStart(script, scriptRules,
                                                         count == scriptItems.size());
    }
  } else if (preference_delegate_) {
    preference_delegate_->PutJavaScriptOnDocumentStart(scriptItems);
  } else {
    LOG(ERROR) << "JavaScriptOnDocumentStart has failed";
  }
}

void NWebDelegate::JavaScriptOnDocumentStartByOrder(
    const ScriptItems& scriptItems,
    const ScriptItemsByOrder& scriptItemsByOrder) {
  if (GetBrowser() != nullptr && GetBrowser()->GetHost() != nullptr) {
    if (scriptItems.size() == 0) {
      GetBrowser()->GetHost()->JavaScriptOnDocumentStart("", std::vector<CefString>(),
                                                         true);
    }
    int count = 0;
    for (const auto& item : scriptItemsByOrder) {
      if (scriptItems.find(item) == scriptItems.end()) {
        continue;
      }
      CefString script = item;
      std::vector<CefString> scriptRules;
      count++;
      for (const std::string& rule : scriptItems.at(item)) {
        CefString cefRule;
        cefRule.FromString(rule);
        scriptRules.push_back(cefRule);
      }
      GetBrowser()->GetHost()->JavaScriptOnDocumentStart(script, scriptRules,
                                                         count == scriptItems.size());
    }
  } else if (preference_delegate_) {
    preference_delegate_->PutJavaScriptOnDocumentStartByOrder(
        scriptItems, scriptItemsByOrder);
  } else {
    LOG(ERROR) << "JavaScriptOnDocumentStartByOrder has failed";
  }
}

void NWebDelegate::JavaScriptOnDocumentEndByOrder(
    const ScriptItems& scriptItems,
    const ScriptItemsByOrder& scriptItemsByOrder) {
  if (GetBrowser() != nullptr && GetBrowser()->GetHost() != nullptr) {
    if (scriptItems.size() == 0) {
      GetBrowser()->GetHost()->JavaScriptOnDocumentEnd("", std::vector<CefString>(),
                                                       true);
    }
    int count = 0;
    for (const auto& item : scriptItemsByOrder) {
      if (scriptItems.find(item) == scriptItems.end()) {
        continue;
      }
      CefString script = item;
      std::vector<CefString> scriptRules;
      count++;
      for (const std::string& rule : scriptItems.at(item)) {
        CefString cefRule;
        cefRule.FromString(rule);
        scriptRules.push_back(cefRule);
      }
      GetBrowser()->GetHost()->JavaScriptOnDocumentEnd(script, scriptRules,
                                                       count == scriptItems.size());
    }
  } else if (preference_delegate_) {
    preference_delegate_->PutJavaScriptOnDocumentEndByOrder(scriptItems,
                                                            scriptItemsByOrder);
  } else {
    LOG(ERROR) << "JavaScriptOnDocumentEndByOrder has failed";
  }
}

void NWebDelegate::JavaScriptOnHeadReadyByOrder(
    const ScriptItems& scriptItems,
    const ScriptItemsByOrder& scriptItemsByOrder) {
  if (GetBrowser() != nullptr && GetBrowser()->GetHost() != nullptr) {
    if (scriptItems.size() == 0) {
      GetBrowser()->GetHost()->JavaScriptOnHeadReady("", std::vector<CefString>(),
                                                     true);
    }
    int count = 0;
    for (const auto& item : scriptItemsByOrder) {
      if (scriptItems.find(item) == scriptItems.end()) {
        continue;
      }

      CefString script = item;
      std::vector<CefString> scriptRules;
      count++;
      for (const std::string& rule : scriptItems.at(item)) {
        CefString cefRule;
        cefRule.FromString(rule);
        scriptRules.push_back(cefRule);
      }
      GetBrowser()->GetHost()->JavaScriptOnHeadReady(script, scriptRules,
                                                     count == scriptItems.size());
    }
  } else if (preference_delegate_) {
    preference_delegate_->PutJavaScriptOnHeadReadyByOrder(scriptItems,
                                                          scriptItemsByOrder);
  } else {
    LOG(ERROR) << "JavaScriptOnHeadReadyByOrder has failed";
  }
}
#endif

void NWebDelegate::CallH5Function(
    int32_t routing_id,
    int32_t h5_object_id,
    const std::string& h5_method_name,
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
  if (GetBrowser() != nullptr && GetBrowser()->GetHost() != nullptr) {
    if (scriptItems.size() == 0) {
      GetBrowser()->GetHost()->JavaScriptOnDocumentEnd("", std::vector<CefString>(),
                                                       true);
    }
    int count = 0;
    for (auto item : scriptItems) {
      count++;
      CefString script = item.first;
      std::vector<CefString> scriptRules;
      for (std::string rule : item.second) {
        CefString cefRule;
        cefRule.FromString(rule);
        scriptRules.push_back(cefRule);
      }
      GetBrowser()->GetHost()->JavaScriptOnDocumentEnd(script, scriptRules,
                                                       count == scriptItems.size());
    }
  } else if (preference_delegate_) {
    preference_delegate_->PutJavaScriptOnDocumentEnd(scriptItems);
  } else {
    LOG(ERROR) << "JavaScriptOnDocumentEnd has failed";
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
  LOG(DEBUG) << "NWebDelegate::OnFocus, nweb_id = " << nweb_id_;
  if (!GetBrowser().get()) {
    LOG(ERROR) << "NWebDelegate::OnFocus GetBrowser().get() fail";
    return false;
  }

#if BUILDFLAG(ARKWEB_FOCUS)
  if (handler_delegate_ && !handler_delegate_->GetFocusState()) {
    LOG(INFO) << "NWebDelegate::OnFocus set web focus, nweb_id = " << nweb_id_;
    GetBrowser()->GetHost()->SetFocus(true);
  }
#endif  // #if BUILDFLAG(ARKWEB_FOCUS)

  return true;
}

void NWebDelegate::OnBlur() const {
  LOG(DEBUG) << "NWebDelegate::OnBlur, nweb_id = " << nweb_id_;
  if (!GetBrowser().get()) {
    LOG(ERROR) << "NWebDelegate::OnBlur GetBrowser().get() fail";
    return;
  }

#if BUILDFLAG(ARKWEB_FOCUS)
  if (handler_delegate_ && handler_delegate_->GetFocusState()) {
    LOG(INFO) << "NWebDelegate::OnBlur set web blur, nweb_id = " << nweb_id_;
    handler_delegate_->SetFocusState(false);
    GetBrowser()->GetHost()->SetFocus(false);
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
    if (is_onPause_) {
      handler_delegate_->SetContinueNeedFocus(true);
    }
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
  }
#endif  // #if BUILDFLAG(ARKWEB_FOCUS)
}

#if BUILDFLAG(ARKWEB_I18N)
void NWebDelegate::UpdateLocale(const std::string& language,
                                const std::string& region) {
  if (!GetBrowser().get()) {
    return;
  }
  bool setSuccess =
      OhosAdapterHelper::GetInstance().GetAudioSystemManager().SetLanguage(
          language);
  if (!setSuccess) {
    LOG(ERROR) << "UpdateLocale SetLanguage error,language=" << language;
  }
  std::string locale = base::ohos::ComputeLanguageByRegion(region);
  if (locale == "") {
    // Current system only support zh, bo, ug, en.
    return;
  }

  GetBrowser()->GetHost()->UpdateLocale(CefString(locale));
}
#endif  // #if BUILDFLAG(ARKWEB_I18N)

void NWebDelegate::SetNWebId(uint32_t nwebId) {
  nweb_id_ = nwebId;
  if (nweb_id_ != 0) {
    handler_delegate_->SetNWebId(nweb_id_);
  }
}

#if BUILDFLAG(ARKWEB_DRAG_DROP)
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

bool NWebDelegate::DarkModeEnabled() {
  if (!preference_delegate_) {
    LOG(ERROR) << "DarkModeEnabled preference_delegate_ get failed";
    return false;
  }

  LOG(DEBUG) << "DarkModeEnabled DarkSchemeEnabled: "
             << preference_delegate_->DarkSchemeEnabled()
             << ", ForceDarkModeEnabled: "
             << preference_delegate_->ForceDarkModeEnabled();
  if (preference_delegate_->DarkSchemeEnabled() &&
      preference_delegate_->ForceDarkModeEnabled()) {
    return true;
  }
  return false;
}
#endif  // ARKWEB_DRAG_DROP

void NWebDelegate::SendDragEvent(const DelegateDragEvent& dragEvent) const {
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(
        CEF_UIT, base::BindOnce(&NWebDelegate::SendDragEvent, this, dragEvent));
    return;
  }
#endif  // ARKWEB_DRAG_DROP

  if (!GetBrowser().get() || !render_handler_) {
    LOG(ERROR) << "browser or render_handler is nullptr";
    return;
  }
  CefMouseEvent event;
  float ratio = render_handler_->GetVirtualPixelRatio();
  event.x = dragEvent.x / ratio;
#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
  event.y =
      dragEvent.y / ratio - GetBrowser()->GetHost()->GetShrinkViewportHeight();
#else
  event.y = dragEvent.y / ratio;
#endif
  event.modifiers = EVENTFLAG_LEFT_MOUSE_BUTTON;
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  switch (dragEvent.action) {
    case DelegateDragAction::DRAG_START:
      LOG(DEBUG) << "DragDrop event SendDragEvent start webId:"
                 << GetBrowser()->GetNWebId();
      break;
    case DelegateDragAction::DRAG_ENTER:
      if (render_handler_) {
#if BUILDFLAG(ARKWEB_REPORT_SYS_EVENT)
        ReportDragDropStatus("DRAG_ENTER", GetBrowser()->GetNWebId());
#endif
        LOG(INFO) << "DragDrop event DRAG_ENTER SendDragEvent enter, send "
                     "dragdata to chromium webId:"
                  << GetBrowser()->GetNWebId();
        handler_delegate_->SetDragEnter(true);
        auto drag_data = render_handler_->GetDragData();
        GetBrowser()->GetHost()->DragTargetDragEnter(drag_data, event,
                                                     DRAG_OPERATION_EVERY);
      } else {
        LOG(ERROR) << "DragDrop drag data render_handler_ nullptr";
      }
      break;
    case DelegateDragAction::DRAG_LEAVE:
      LOG(INFO) << "DragDrop event SendDragEvent leave webId:"
                << GetBrowser()->GetNWebId();
#if BUILDFLAG(ARKWEB_REPORT_SYS_EVENT)
      ReportDragDropStatus("DRAG_LEAVE", GetBrowser()->GetNWebId());
#endif
      handler_delegate_->SetDragEnter(false);
      GetBrowser()->GetHost()->DragTargetDragLeave();
      break;
    case DelegateDragAction::DRAG_OVER:
      LOG(DEBUG) << "DragDrop event SendDragEvent over webId:"
                 << GetBrowser()->GetNWebId();
      GetBrowser()->GetHost()->DragTargetDragOver(event, DRAG_OPERATION_EVERY);
      break;
    case DelegateDragAction::DRAG_DROP:
      event.modifiers = EVENTFLAG_NONE;
      handler_delegate_->SetDragEnter(false);
      LOG(INFO) << "DragDrop event SendDragEvent drop webId:"
                << GetBrowser()->GetNWebId();
      if (render_handler_) {
        auto drag_data1 = render_handler_->GetDragData();
        auto fragment1 = drag_data1->GetFragmentText();
        LOG(DEBUG) << "DragDrop drag data GetFragmentText:"
                   << fragment1.ToString();
        auto link_url1 = drag_data1->GetLinkURL();
        LOG(DEBUG) << "DragDrop drag data GetLinkURL:" << link_url1.ToString();
        auto link_html1 = drag_data1->GetFragmentHtml();
        LOG(DEBUG) << "DragDrop drag data GetFragmentHtml:"
                   << link_html1.ToString();
#if BUILDFLAG(ARKWEB_REPORT_SYS_EVENT)
        ReportDragDropInfo("DRAG_DROP", GetBrowser()->GetNWebId(), fragment1,
                           link_url1, link_html1);
#endif
      } else {
        LOG(ERROR) << "DragDrop drag data render_handler_ nullptr";
      }

      GetBrowser()->GetHost()->DragTargetDrop(event);
      break;
    case DelegateDragAction::DRAG_END:
#if BUILDFLAG(ARKWEB_REPORT_SYS_EVENT)
      ReportDragDropStatus("DRAG_END", GetBrowser()->GetNWebId());
#endif
      handler_delegate_->SetDragEnter(false);
      ClearDragData();
      LOG(INFO) << "DragDrop event SendDragEvent end webId:"
                << GetBrowser()->GetNWebId();
      GetBrowser()->GetHost()->DragSourceEndedAt(event.x, event.y,
                                                 DRAG_OPERATION_COPY);
      GetBrowser()->GetHost()->DragSourceSystemDragEnded();
      break;
    case DelegateDragAction::DRAG_CANCEL:
      handler_delegate_->SetDragEnter(false);
      ClearDragData();
      LOG(INFO) << "DragDrop event SendDragEvent cancel webId:"
                << GetBrowser()->GetNWebId();
      GetBrowser()->GetHost()->DragSourceSystemDragEnded();
      break;
    default:
      LOG(INFO) << "invalid drag action";
      break;
  }
#endif  // ARKWEB_DRAG_DROP
}

void NWebDelegate::GetImages(std::shared_ptr<NWebBoolValueCallback> callback) {
  if (!GetBrowser().get()) {
    LOG(ERROR) << "JSAPI GetImages can not get browser";
    return;
  }

  CefRefPtr<GetImagesCallbackImpl> GetImagesCb =
      new GetImagesCallbackImpl(callback);
  GetBrowser()->GetMainFrame()->AsArkWebFrame()->GetImages(GetImagesCb);
}

void NWebDelegate::RemoveCache(bool include_disk_files) {
  if (!GetBrowser().get()) {
    LOG(ERROR) << "JSAPI RemoveCache can not get browser";
    return;
  }

  GetBrowser()->GetHost()->RemoveCache(include_disk_files);
}

#if BUILDFLAG(ARKWEB_NAVIGATION)
std::shared_ptr<NWebHistoryList> NWebDelegate::GetHistoryList() {
  if (!GetBrowser().get()) {
    return nullptr;
  }

  CefRefPtr<NavigationEntryVisitorImpl> visitor =
      new NavigationEntryVisitorImpl();
  GetBrowser()->GetHost()->GetNavigationEntries(visitor, false);
  return visitor->GetHistoryList();
}

std::vector<uint8_t> NWebDelegate::SerializeWebState() {
  std::vector<uint8_t> state;
  if (!GetBrowser() || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "SerializeWebState get browser is nullptr";
    return state;
  }
  CefRefPtr<CefBinaryValue> state_value =
      GetBrowser()->GetHost()->GetWebState();
  if (!state_value || !GetBrowser().get()) {
    return state;
  }
  size_t state_size = state_value->GetSize();
  if (state_size == 0) {
    return state;
  }

  state.resize(state_size);
  size_t read_size = state_value->GetData(state.data(), state_size, 0);
  if (read_size != state_size) {
    LOG(ERROR) << "SerializeWebState failed";
    return state;
  }
  return state;
}

bool NWebDelegate::RestoreWebState(const std::vector<uint8_t>& state) {
  if (!GetBrowser().get() || state.size() == 0) {
    return false;
  }
  auto web_state = CefBinaryValue::Create(state.data(), state.size());
  return GetBrowser()->GetHost()->RestoreWebState(web_state);
}
#endif  // BUILDFLAG(ARKWEB_NAVIGATION)

#if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)
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

#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
void NWebDelegate::GetScrollOffset(float* offset_x, float* offset_y) {
  if (!GetBrowser().get()) {
    LOG(ERROR) << "JSAPI GetScrollOffset can not get browser";
    return;
  }

  if (!render_handler_) {
    LOG(ERROR) << "JSAPI GetScrollOffset render_handler_ is nullptr.";
    return;
  }

  float offsetX = 0;
  float offsetY = 0;
  render_handler_->GetScrollOffset(offsetX, offsetY);

  if ((nullptr == offset_x) || (nullptr == offset_y)) {
    LOG(ERROR) << "offset_x or offset_y is nullptr";
    return;
  }

  *offset_x = offsetX;
  *offset_y = offsetY;
}
#endif
#endif  // #if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
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

  GetBrowser()->GetHost()->ScrollTo(std::round(x * ratio),
                                    std::round(y * ratio));
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

  GetBrowser()->GetHost()->ScrollBy(std::round(delta_x * ratio),
                                    std::round(delta_y * ratio));
}

void NWebDelegate::ScrollByRefScreen(float delta_x,
                                     float delta_y,
                                     float vx,
                                     float vy) {
  if (!GetBrowser().get()) {
    LOG(ERROR) << "ScrollByRefScreen can not get browser";
    return;
  }
  float scale = Scale();
  if (scale > 0 && (delta_x != 0 || delta_y != 0)) {
    // delta_x and delta_y here should be sure to be a value in physical pixels.
    GetBrowser()->GetHost()->ScrollBy(std::round(delta_x) / scale,
                                      std::round(delta_y) / scale);
  }
}

void NWebDelegate::SlideScroll(float vx, float vy) {
  if (!GetBrowser().get()) {
    LOG(ERROR) << "JSAPI SlideScroll can not get browser";
    return;
  }

  GetBrowser()->GetHost()->SlideScroll(vx, vy);
}

bool NWebDelegate::WebSendKeyEvent(int32_t keyCode,
                                   int32_t keyAction,
                                   const std::vector<int32_t>& pressedCodes) {
  bool retVal = false;
  if (event_handler_ != nullptr) {
    retVal = event_handler_->WebSendKeyEventFromAce(keyCode, keyAction,
                                                    pressedCodes);
  } else {
    LOG(ERROR) << "WebSendKeyEvent event_handler_ is nullptr";
  }
  return retVal;
}

void NWebDelegate::WebSendMouseWheelEvent(
    double x,
    double y,
    double deltaX,
    double deltaY,
    const std::vector<int32_t>& pressedCodes) {
  if (event_handler_ != nullptr) {
    event_handler_->WebSendMouseWheelEvent(
        x / default_virtual_pixel_ratio_, y / default_virtual_pixel_ratio_,
        deltaX / default_virtual_pixel_ratio_,
        deltaY / default_virtual_pixel_ratio_, pressedCodes);
  } else {
    LOG(ERROR) << "WebSendMouseWheelEvent event_handler_ is nullptr";
  }
}
void NWebDelegate::WebSendMouseWheelEventV2(
    double x,
    double y,
    double deltaX,
    double deltaY,
    const std::vector<int32_t>& pressedCodes,
    int32_t source) {
  if (event_handler_ != nullptr) {
    event_handler_->WebSendMouseWheelEventV2(
        x / default_virtual_pixel_ratio_,
        y / default_virtual_pixel_ratio_,
        deltaX / default_virtual_pixel_ratio_,
        deltaY / default_virtual_pixel_ratio_,
        pressedCodes,
        source);
  } else {
    LOG(ERROR) << "WebSendMouseWheelEvent event_handler_ is nullptr";
  }
}

void NWebDelegate::WebSendTouchpadFlingEvent(
    double x,
    double y,
    double vx,
    double vy,
    const std::vector<int32_t>& pressedCodes) {
  if (event_handler_ != nullptr) {
    event_handler_->WebSendTouchpadFlingEvent(
        x / default_virtual_pixel_ratio_, y / default_virtual_pixel_ratio_,
        vx / default_virtual_pixel_ratio_, vy / default_virtual_pixel_ratio_,
        pressedCodes);
  } else {
    LOG(ERROR) << "WebSendTouchpadFlingEvent event_handler_ is nullptr";
  }
}

bool NWebDelegate::SendKeyboardEvent(
    const std::shared_ptr<OHOS::NWeb::NWebKeyboardEvent>& keyboardEvent) {
  bool retVal = false;
  if (event_handler_ != nullptr) {
    retVal = event_handler_->SendKeyboardEvent(keyboardEvent);
  }
  return retVal;
}

bool NWebDelegate::ScrollByWithResult(float delta_x, float delta_y) {
  if (handler_delegate_ == nullptr) {
    LOG(ERROR) << "handler_delegate_ is nullptr , ScrollByWithResult fail";
    return false;
  }
  LOG(DEBUG) << "The pressing_num_ in ScrollByWithResult is" << (pressing_num_);
  if (render_handler_ == nullptr) {
    LOG(ERROR) << "fail to register NWebDelegateInterface client, render "
                  "handler is nullptr";
    return false;
  }
  bool isDontScroll = pressing_num_ > 0;
  bool isIgnoreDown = render_handler_->GetGestureEventResult();
  if (isIgnoreDown) {
    LOG(DEBUG) << "Web is touched down but on arkui area ,so continue scroll";
    isDontScroll = false;
  }
  if (isDontScroll) {
    LOG(DEBUG) << "Web is touched down, return false";
    return false;
  }
  ScrollBy(delta_x, delta_y);
  return true;
}

void NWebDelegate::WebSendMouseEvent(
    const std::shared_ptr<OHOS::NWeb::NWebMouseEvent>& mouseEvent) {
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  if (event_handler_ != nullptr &&
      (!handler_delegate_ || !handler_delegate_->IsDragEnter())) {
#else
  if (event_handler_ != nullptr) {
#endif  // #if BUILDFLAG(ARKWEB_DRAG_DROP)
    event_handler_->WebSendMouseEvent(mouseEvent, default_virtual_pixel_ratio_);
  } else {
#if BUILDFLAG(ARKWEB_DRAG_DROP)
    LOG(INFO) << "Mouse Event dropped! event_handler is " << !!event_handler_
              << " handler_delegate is " << !handler_delegate_;
#else
    LOG(INFO) << "Mouse Event dropped! event_handler is " << !!event_handler_;
#endif  // BUILDFLAG(ARKWEB_DRAG_DROP)
  }
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  if (render_handler_ != nullptr) {
    render_handler_->SetIrregularDragBackground(false);
  }
#endif  // #if BUILDFLAG(ARKWEB_DRAG_DROP)
}

void NWebDelegate::ScrollToWithAnime(float x, float y, int32_t duration) {
  if (!GetBrowser().get()) {
    LOG(ERROR) << "JSAPI ScrollToWithAnime can not get browser";
    return;
  }
  if (render_handler_ == nullptr) {
    LOG(ERROR) << "fail to  ScrollToWithAnime, render "
                  "handler is nullptr";
    return;
  }
  float ratio = render_handler_->GetVirtualPixelRatio();
  if (ratio <= 0) {
    LOG(ERROR) << "get ratio invalid: " << ratio;
    return;
  }

  GetBrowser()->GetHost()->ScrollToWithAnime(std::round(x * ratio),
                                             std::round(y * ratio), duration);
}

void NWebDelegate::ScrollByWithAnime(float delta_x,
                                     float delta_y,
                                     int32_t duration) {
  if (!GetBrowser().get()) {
    LOG(ERROR) << "JSAPI ScrollByWithAnime can not get browser";
    return;
  }
  if (render_handler_ == nullptr) {
    LOG(ERROR) << "fail to  ScrollByWithAnime, render "
                  "handler is nullptr";
    return;
  }
  float ratio = render_handler_->GetVirtualPixelRatio();
  if (ratio <= 0) {
    LOG(ERROR) << "get ratio invalid: " << ratio;
    return;
  }

  GetBrowser()->GetHost()->ScrollByWithAnime(
      std::round(delta_x * ratio), std::round(delta_y * ratio), duration);
}

#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
void NWebDelegate::GetOverScrollOffset(float* offset_x, float* offset_y) {
  if (!GetBrowser().get()) {
    LOG(ERROR) << "JSAPI GetOverScrollOffset can not get browser";
    return;
  }
  if (!render_handler_) {
    LOG(ERROR) << "JSAPI GetOverScrollOffset render_handler_ is nullptr.";
    return;
  }
  if (render_handler_->HasOverscroll()) {
    GetBrowser()->GetHost()->GetOverScrollOffset(offset_x, offset_y);
  } else {
    *offset_x = 0.0f;
    *offset_y = 0.0f;
  }
}
#endif
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_API_INIT_WEB_ENGINE)
void NWebDelegate::OnContextInitializeComplete(
    const std::string& url,
    void* window
#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
    ,
    bool incognito_mode
#endif
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
    ,
    const std::string& shared_render_process_token

#endif
) {
  LOG(INFO) << "NWebDelegate::OnContextInitializeComplete";
  // Create browser after context initialzed complete.
  NWebApplication::GetDefault()->CreateBrowser(preference_delegate_, url,
                                               handler_delegate_, window
#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
                                               ,
                                               incognito_mode
#endif
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
                                               ,
                                               shared_render_process_token
#endif
  );
}
#endif  // BUILDFLAG(ARKWEB_API_INIT_WEB_ENGINE)

#if BUILDFLAG(ARKWEB_MEDIA_MUTE_AUDIO)
void NWebDelegate::SetAudioMuted(bool muted) {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "SetAudioMuted can not get browser";
    return;
  }

  GetBrowser()->GetHost()->SetAudioMuted(muted);
}
#endif  // BUILDFLAG(ARKWEB_MEDIA_MUTE_AUDIO)

#if BUILDFLAG(ARKWEB_WEBRTC)
void NWebDelegate::StartCamera() {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "StartCamera can not get browser";
    return;
  }

  GetBrowser()->GetHost()->StartCamera();
}

void NWebDelegate::StopCamera() {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "StopCamera can not get browser";
    return;
  }

  GetBrowser()->GetHost()->StopCamera();
}

void NWebDelegate::CloseCamera() {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "CloseCamera can not get browser";
    return;
  }

  GetBrowser()->GetHost()->CloseCamera();
}
#endif  // BUILDFLAG(ARKWEB_WEBRTC)

#if BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)
void NWebDelegate::StopScreenCapture(int32_t nweb_id, const char* session_id) {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "StopScreenCapture can not get browser";
    return;
  }
  LOG(INFO) << "StopScreenCapture nweb_id:" << nweb_id;
  GetBrowser()->GetHost()->StopScreenCapture(nweb_id, CefString(session_id));
}

void NWebDelegate::SetScreenCapturePickerShow() {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "SetScreenCapturePickerShow can not get browser";
    return;
  }

  GetBrowser()->GetHost()->SetScreenCapturePickerShow();
}

void NWebDelegate::DisableSessionReuse() {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "DisableSessionReuse can not get browser";
    return;
  }

  GetBrowser()->GetHost()->DisableSessionReuse();
}

void NWebDelegate::RegisterScreenCaptureDelegateListener(
    std::shared_ptr<NWebScreenCaptureDelegateCallback> listener) {
  CefRefPtr<ScreenCaptureCallbackImpl> screenCaptureCb =
      new ScreenCaptureCallbackImpl(listener);
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    if (handler_delegate_ == nullptr) {
      LOG(ERROR) << "Fail to register screen capture callback, handler_delegate is nullptr.";
      return;
    }
    handler_delegate_->SetNWebId(nweb_id_);
    handler_delegate_->RegisterScreenCaptureDelegateListener(screenCaptureCb);
    LOG(INFO) << "handler_delegate register screen capture callback";
    return;
  }
  GetBrowser()->GetHost()->RegisterScreenCaptureDelegateListener(screenCaptureCb);
}
#endif  // defined(ARKWEB_EX_SCREEN_CAPTURE)

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
void NWebDelegate::SetShouldFrameSubmissionBeforeDraw(bool should) {
  if (GetBrowser().get()) {
    GetBrowser()->GetHost()->SetShouldFrameSubmissionBeforeDraw(should);
  }
}

void NWebDelegate::SetDrawRect(int32_t x,
                               int32_t y,
                               int32_t width,
                               int32_t height) {
  if (GetBrowser().get()) {
    GetBrowser()->GetHost()->SetDrawRect(x, y, width, height);
    GetBrowser()->GetHost()->UpdateDrawRect();
  }
}

void NWebDelegate::SetDrawMode(int32_t mode) {
  if (GetBrowser().get()) {
    GetBrowser()->GetHost()->SetDrawMode(mode);
  }
  if (preference_delegate_) {
    preference_delegate_->SetDrawMode(mode);
  }
}

bool NWebDelegate::GetPendingSizeStatus() {
  if (GetBrowser().get()) {
    return GetBrowser()->GetHost()->GetPendingSizeStatus();
  }
  return false;
}

void NWebDelegate::SetFitContentMode(int32_t mode) {
  if (GetBrowser().get()) {
    GetBrowser()->GetHost()->SetFitContentMode(mode);
  }
  if (preference_delegate_) {
    preference_delegate_->SetFitContent(mode);
  }
}

std::string NWebDelegate::GetCurrentLanguage() {
  if (!GetBrowser().get()) {
    LOG(ERROR) << "NWebDelegate::GetCurrentLanguage GetBrowser().get() fail";
    return "";
  }
  return GetBrowser()->GetHost()->GetCurrentLanguage();
}
#endif  // BUILDFLAG(ARKWEB_COMPOSITE_RENDER)

#if BUILDFLAG(ARKWEB_SAME_LAYER)
void NWebDelegate::SetNativeInnerWeb(bool isInnerWeb) {
  if (GetBrowser().get()) {
    GetBrowser()->GetHost()->SetNativeInnerWeb(isInnerWeb);
  }
}
#endif

#if BUILDFLAG(ARKWEB_EXT_FORCE_ZOOM)
void NWebDelegate::SetForceEnableZoom(bool forceEnableZoom) {
  LOG(INFO) << "NWebDelegate::SetForceEnableZoom " << forceEnableZoom;
#if BUILDFLAG(ARKWEB_REPORT_SYS_EVENT)
  ReportForceZoomEnable();
#endif
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

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void NWebDelegate::SetVirtualKeyBoardArg(int32_t width,
                                         int32_t height,
                                         double keyboard) {
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

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
void NWebDelegate::SetAudioResumeInterval(int32_t resumeInterval) {
  if (GetBrowser() != nullptr && GetBrowser()->GetHost() != nullptr) {
    GetBrowser()->GetHost()->SetAudioResumeInterval(resumeInterval);
  }
  if (preference_delegate_) {
    preference_delegate_->PutAudioResumeInterval(resumeInterval);
  }
}

void NWebDelegate::SetAudioExclusive(bool audioExclusive) {
  if (GetBrowser() != nullptr && GetBrowser()->GetHost() != nullptr) {
    GetBrowser()->GetHost()->SetAudioExclusive(audioExclusive);
  }
  if (preference_delegate_) {
    preference_delegate_->PutAudioExclusive(audioExclusive);
  }
}

void NWebDelegate::CloseAllMediaPresentations() {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "CloseAllMediaPresentations can not get browser";
    return;
  }

  GetBrowser()->GetHost()->CloseMedia();
}

void NWebDelegate::StopAllMedia() {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "StopAllMedia can not get browser";
    return;
  }

  GetBrowser()->GetHost()->StopMedia();
}

void NWebDelegate::ResumeAllMedia() {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "ResumeAllMedia can not get browser";
    return;
  }

  GetBrowser()->GetHost()->ResumeMedia();
}

void NWebDelegate::PauseAllMedia() {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "PauseAllMedia can not get browser";
    return;
  }

  GetBrowser()->GetHost()->PauseMedia();
}

int NWebDelegate::GetMediaPlaybackState() {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "GetMediaPlaybackState can not get browser";
    return NWebPlaybackState_NONE;
  }

  return GetBrowser()->GetHost()->GetMediaPlaybackState();
}

#endif  // BUILDFLAG(ARKWEB_MEDIA_POLICY)

void NWebDelegate::PrefetchPage(
    const std::string& url,
    const std::map<std::string, std::string>& additionalHttpHeaders) {
#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
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
#endif  // BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
}

#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
void NWebDelegate::NotifyPopupWindowResult(bool result) {
  if (handler_delegate_) {
    handler_delegate_->NotifyPopupWindowResult(result);
  }
}
#endif  // BUILDFLAG(ARKWEB_MULTI_WINDOW)

#if BUILDFLAG(ARKWEB_CA)
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

#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
void NWebDelegate::UpdateBrowserControlsState(int constraints,
                                              int current,
                                              bool animate) const {
  if (GetBrowser().get()) {
    GetBrowser()->AsArkWebBrowser()->UpdateBrowserControlsState(
        constraints, current, animate);
  }
}

void NWebDelegate::UpdateBrowserControlsHeight(int height, bool animate) {
  if (GetBrowser().get()) {
    GetBrowser()->AsArkWebBrowser()->UpdateBrowserControlsHeight(height,
                                                                 animate);
  }
}

#endif

#if BUILDFLAG(ARKWEB_PRINT)
void NWebDelegate::SetToken(void* token) {
  if (GetBrowser() && GetBrowser()->GetHost()) {
    GetBrowser()->GetHost()->SetToken(token);
  }

  if (preference_delegate_) {
    preference_delegate_->PutPrintToken(token);
  }
}

void* NWebDelegate::CreateWebPrintDocumentAdapter(const std::string& jobName) {
  LOG(DEBUG) << "Create Web print document adapter jobName = " << jobName;
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "CreateWebPrintDocumentAdapter can not get browser";
    return nullptr;
  }

  void* webPrintDocumentAdapter = nullptr;
  GetBrowser()->GetHost()->CreateWebPrintDocumentAdapter(
      CefString(jobName), &webPrintDocumentAdapter);
  return webPrintDocumentAdapter;
}

void NWebDelegate::SetPrintBackground(bool enable) {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "SetPrintBackground can not get browser";
    return;
  }

  GetBrowser()->GetHost()->SetPrintBackground(enable);
}

bool NWebDelegate::GetPrintBackground() {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "GetPrintBackground can not get browser";
    return false;
  }

  return GetBrowser()->GetHost()->GetPrintBackground();
}
#endif  // BUILDFLAG(ARKWEB_PRINT)

#if BUILDFLAG(ARKWEB_ADBLOCK)
void NWebDelegate::EnableAdsBlock(bool enable) {
  LOG(INFO) << "NWebDelegate::EnableAdsBlock " << enable;
  if (is_popup_ready_) {
    if (handler_delegate_) {
      LOG(INFO) << "[ADBLOCK] NWebDelegate::EnableAdsBlock popup case, the "
                   "adblock switch is "
                << enable;
      handler_delegate_->SaveGlobalAdsBlock(enable);
    }
    return;
  } else if (GetBrowser().get()) {
    GetBrowser()->EnableAdsBlock(enable);
  }
}

bool NWebDelegate::IsAdsBlockEnabled() {
  if (GetBrowser().get()) {
    return GetBrowser()->IsAdsBlockEnabled();
  }
  return false;
}

bool NWebDelegate::IsAdsBlockEnabledForCurPage() {
  if (GetBrowser().get()) {
    return GetBrowser()->IsAdsBlockEnabledForCurPage();
  }
  return false;
}

void NWebDelegate::SetAdBlockEnabledForSite(bool is_adblock_enabled,
                                            int main_frame_tree_node_id) {
  LOG(INFO) << "NWebDelegate::SetAdBlockEnabledForSite " << is_adblock_enabled;
  if (GetBrowser().get()) {
    GetBrowser()->SetAdBlockEnabledForSite(is_adblock_enabled,
                                           main_frame_tree_node_id);
  }
}
#endif

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
void NWebDelegate::ProcessAutofillCancel(const std::string& fillContent) {
  if (GetBrowser() && GetBrowser()->GetHost()) {
    GetBrowser()->GetHost()->ProcessAutofillCancel(fillContent);
  }
}
#endif

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
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
#endif  // ARKWEB_EXT_PASSWORD

#if BUILDFLAG(ARKWEB_EXT_PASSWORD) || BUILDFLAG(ARKWEB_DATALIST)
void NWebDelegate::PasswordSuggestionSelected(int list_index) const {
  LOG(INFO) << "NWebDelegate::PasswordSuggestionSelected";
  if (GetBrowser().get()) {
    GetBrowser()->PasswordSuggestionSelected(list_index);
  }
}
#endif

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
std::string NWebDelegate::GetSelectedTextFromContextParam() {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    return std::string();
  }
  return GetBrowser()->GetHost()->GetSelectedTextFromContextParam();
}

void NWebDelegate::ShowFreeCopyMenu() {
  if (GetBrowser().get()) {
    GetBrowser()->ShowFreeCopyMenu();
  }
}

bool NWebDelegate::ShouldShowFreeCopyMenu() {
  if (GetBrowser().get()) {
    return GetBrowser()->ShouldShowFreeCopyMenu();
  }
  return false;
}
#endif  // BUILDFLAG(ARKWEB_EXT_FREE_COPY)

#if BUILDFLAG(ARKWEB_EXT_GET_ZOOM_LEVEL)
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

#if BUILDFLAG(ARKWEB_SAFEBROWSING)
bool NWebDelegate::IsSafeBrowsingEnabled() {
  if (GetBrowser() == nullptr) {
    LOG(ERROR) << "NWebDelegate::IsSafeBrowsingEnabled failed.";
    return false;
  }

  return GetBrowser()->IsSafeBrowsingEnabled();
}

void NWebDelegate::EnableSafeBrowsing(bool enable) {
  if (GetBrowser() == nullptr) {
    LOG(ERROR) << "NWebDelegate::EnableSafeBrowsing failed.";
    return;
  }

  GetBrowser()->EnableSafeBrowsing(enable);
}

void NWebDelegate::EnableSafeBrowsingDetection(bool enable, bool strictMode) {
  LOG(INFO) << "NWebDelegate::EnableSafeBrowsingDetection. enable is " << enable
            << ", strictMode is " << strictMode;
  if (GetBrowser() == nullptr) {
    LOG(ERROR) << "NWebDelegate::EnableSafeBrowsingDetection failed.";
    return;
  }

  GetBrowser()->EnableSafeBrowsingDetection(enable, strictMode);
}

void NWebDelegate::OnSafeBrowsingDetectionResult(int code,
                                                 int policy,
                                                 const std::string& mappingType,
                                                 const std::string& url) {
  if (handler_delegate_ == nullptr) {
    return;
  }

  handler_delegate_->OnSafeBrowsingDetectionResult(code, policy, mappingType,
                                                   url);
}
#endif  // BUILDFLAG(ARKWEB_SAFEBROWSING)

#if BUILDFLAG(IS_OHOS)
void NWebDelegate::PrecompileJavaScript(
    const std::string& url,
    const std::string& script,
    std::shared_ptr<CacheOptions>& cacheOptions,
    std::shared_ptr<NWebMessageValueCallback> callback) {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "NWebDelegate::PrecompileJavaScript failed. browser host has "
                  "not initialized";
    return;
  }

  CefRefPtr<CefPrecompileCallbackImpl> precompileCallback =
      new CefPrecompileCallbackImpl(callback);
  CefRefPtr<CefCacheOptionsImpl> cefOptions =
      new CefCacheOptionsImpl(cacheOptions);
  GetBrowser()->GetHost()->PrecompileJavaScript(url, script, cefOptions,
                                                precompileCallback);
}

bool NWebDelegate::HitNativeArea(double x, double y) {
  auto iter = embedDataInfo_.begin();
  for (; iter != embedDataInfo_.end(); iter++) {
    std::shared_ptr<NWebNativeEmbedDataInfo> dataInfo = iter->second;
    if (dataInfo == nullptr) {
      continue;
    }
    auto embedInfo = dataInfo->GetNativeEmbedInfo();

    double width = embedInfo->GetWidth();
    double height = embedInfo->GetHeight();
    double embedX = embedInfo->GetX();
    double embedY = embedInfo->GetY();
    if (embedX <= x && x <= (embedX + width) && embedY <= y &&
        y <= (embedY + height)) {
      return true;
    }
  }
  return false;
}

void NWebDelegate::UpdateNativeEmbedInfo(
    std::shared_ptr<NWebNativeEmbedDataInfo> info) {
  std::string embedId = info->GetEmbedId();
  auto status = info->GetStatus();
  if (status == NativeEmbedStatus::CREATE ||
      status == NativeEmbedStatus::UPDATE) {
    embedDataInfo_.insert_or_assign(embedId, info);
  } else if (status == NativeEmbedStatus::DESTROY) {
    auto iter = embedDataInfo_.find(embedId);
    if (iter != embedDataInfo_.end()) {
      embedDataInfo_.erase(iter);
    }
  }
}
#endif

#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
void NWebDelegate::SetTransformHint(uint32_t rotation) {
  if (handler_delegate_) {
    handler_delegate_->SetTransformHint(rotation);
  }
}
#endif

#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_CORNER)
void NWebDelegate::SetBorderRadiusFromWeb(double borderRadiusTopLeft,
                                          double borderRadiusTopRight,
                                          double borderRadiusBottomLeft,
                                          double borderRadiusBottomRight) {
  if (!preference_delegate_) {
    return;
  }
  preference_delegate_->SetBorderRadiusFromWeb(
      borderRadiusTopLeft, borderRadiusTopRight, borderRadiusBottomLeft,
      borderRadiusBottomRight);
}
#endif  // ARKWEB_SCROLLBAR_AVOID_CORNER

#if BUILDFLAG(ARKWEB_SECURITY_STATE)
int NWebDelegate::GetSecurityLevel() {
  if (GetBrowser() == nullptr) {
    LOG(ERROR) << "NWebDelegate::GetSecurityLevel failed.";
    return static_cast<int>(security_state::SecurityLevel::NONE);
  }

  return GetBrowser()->GetSecurityLevel();
}
#endif

#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
void NWebDelegate::SetAccessibilityState(cef_state_t accessibilityState) {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "SetAccessibilityState can not get browser";
    return;
  }
  if (accessibility_state_ != (accessibilityState == STATE_ENABLED)) {
    accessibility_state_ = (accessibilityState == STATE_ENABLED);
    GetBrowser()->GetHost()->SetAccessibilityState(accessibilityState);
  }
}

bool NWebDelegate::ExecuteAction(
    int64_t accessibilityId,
    uint32_t action,
    const std::map<std::string, std::string>& actionArguments) {
  auto* accessibilityManager = GetAccessibilityManager();
  if (accessibilityManager == nullptr) {
    return false;
  }
  BrowserAccessibilityOHOS* node =
      BrowserAccessibilityOHOS::GetFromAccessibilityId(
          GetRealAccessibilityId(accessibilityId));
  if (node == nullptr) {
    LOG(ERROR) << "ExecuteAction node is not found";
    return false;
  }
  AceAction aceAction = static_cast<AceAction>(action);
  LOG(INFO) << "ExecuteAction accessibilityId is " << accessibilityId
            << ", action is " << action;

  switch (aceAction) {
    case AceAction::ACTION_CLICK:
      accessibilityManager->DoDefaultAction(*node);
        break;
    case AceAction::ACTION_ACCESSIBILITY_FOCUS:
        if (accessibilityManager->MoveAccessibilityFocusToId(accessibilityId) &&
          !is_hovering_) {
        accessibilityManager->ScrollToMakeNodeVisible(accessibilityId);
        }
        break;
    case AceAction::ACTION_CLEAR_ACCESSIBILITY_FOCUS:
      accessibilityManager->SendAccessibilityEvent(
          accessibilityId,
          AccessibilityEventType::ACCESSIBILITY_FOCUS_CLEARED);
      if (accessibilityManager->GetAccessibilityFocusId() ==
          accessibilityId) {
        accessibilityManager->MoveAccessibilityFocus(
            accessibilityManager->GetAccessibilityFocusId(), -1);
        accessibilityManager->SetAccessibilityFocusId(-1);
      }
      break;
    case AceAction::ACTION_FOCUS:
      accessibilityManager->SetFocus(*node);
      break;
    case AceAction::ACTION_CLEAR_FOCUS:
      accessibilityManager->SetFocus(
          *accessibilityManager->GetBrowserAccessibilityRoot());
      break;
    case AceAction::ACTION_SCROLL_FORWARD:
    case AceAction::ACTION_SCROLL_BACKWARD: {
      int32_t argument = GetArgumentByKey(actionArguments, "scrolltype");
      AccessibilityScrollType scrollType;
      switch (argument) {
        case 0:
          scrollType = AccessibilityScrollType::SCROLL_HALF;
          break;
        case 1:
          scrollType = AccessibilityScrollType::SCROLL_FULL;
          break;
        default:
          scrollType = AccessibilityScrollType::SCROLL_DEFAULT;
          break;
      }
      node->Scroll(aceAction == AceAction::ACTION_SCROLL_FORWARD
                       ? ScrollDirection::FORWARD
                       : ScrollDirection::BACKWARD,
                   scrollType != AccessibilityScrollType::SCROLL_HALF);
      node->manager()->LoadInlineTextBoxes(*node);
      break;
    }
    case AceAction::ACTION_COPY:
      accessibilityManager->Copy();
      break;
    case AceAction::ACTION_PASTE:
      accessibilityManager->Paste();
      break;
    case AceAction::ACTION_CUT:
      accessibilityManager->Cut();
      break;
    case AceAction::ACTION_SET_SELECTION: {
      if (!node->IsTextField() || actionArguments.empty()) {
        return false;
      }

      int start = 0;
      int end = 0;
      auto iter = actionArguments.find("selectTextBegin");

      if (iter != actionArguments.end()) {
        std::stringstream str_start;
        str_start << iter->second;
        str_start >> start;
      }

      iter = actionArguments.find("selectTextEnd");

      if (iter != actionArguments.end()) {
        std::stringstream str_end;
        str_end << iter->second;
        str_end >> end;
      }

      accessibilityManager->SetSelection(BrowserAccessibility::AXRange(
          node->CreatePositionForSelectionAt(start),
          node->CreatePositionForSelectionAt(end)));
      break;
    }
    case AceAction::ACTION_SET_TEXT: {
      if (!node->IsTextField()) {
        return false;
      }
      if (actionArguments.empty()) {
        return false;
      }
      std::string newText = "";
      auto iter = actionArguments.find("setText");
      if (iter != actionArguments.end()) {
        newText = iter->second;
      }
      if (newText.empty()) {
        return false;
      }
      accessibilityManager->SetValue(*node, newText);
      accessibilityManager->SetSelection(BrowserAccessibility::AXRange(
          node->CreatePositionForSelectionAt(newText.length()),
          node->CreatePositionForSelectionAt(newText.length())));
      break;
    }

    case AceAction::ACTION_SET_CURSOR_POSITION: {
      if (!node->IsTextField() || actionArguments.empty()) {
        return false;
      }

      int offset = 0;
      auto iter = actionArguments.find("offset");

      if (iter != actionArguments.end()) {
        std::stringstream str_offset;
        str_offset << iter->second;
        str_offset >> offset;
      }
      LOG(INFO) << "ExecuteAction setCursorPosition offset is " << offset;
      accessibilityManager->SetSelection(BrowserAccessibility::AXRange(
          node->CreatePositionForSelectionAt(offset),
          node->CreatePositionForSelectionAt(offset)));
      break;
    }
    case AceAction::ACTION_NEXT_HTML_ITEM: {
      std::string elementType = "";
      auto iter = actionArguments.find("htmlItem");
      if (iter != actionArguments.end()) {
        elementType = iter->second;
      }
      return accessibilityManager->JumpToElementType(
          accessibilityId, elementType, true, false);
    }
    case AceAction::ACTION_PREVIOUS_HTML_ITEM: {
      std::string elementType = "";
      auto iter = actionArguments.find("htmlItem");
      if (iter != actionArguments.end()) {
        elementType = iter->second;
      }
      return accessibilityManager->JumpToElementType(
          accessibilityId, elementType, false,
          accessibilityId ==
              accessibilityManager->GetRootAccessibilityId());
    }
    default:
      LOG(INFO) << "ExecuteAction unsupported action";
      break;
  }
  return true;
}

int64_t NWebDelegate::GetRealAccessibilityId(int64_t accessibilityId) const {
  if (accessibilityId < 0 || accessibilityId == kRootAccessibilityId) {
    auto* accessibilityManager = GetAccessibilityManager();
    if (accessibilityManager == nullptr) {
        return -1;
    }
    return accessibilityManager->GetRootAccessibilityId();
  }
  return accessibilityId;
}

bool NWebDelegate::GetAccessibilityNodeRectById(int64_t accessibilityId,
                                                int32_t* width,
                                                int32_t* height,
                                                int32_t* offsetX,
                                                int32_t* offsetY) {
  if (width == nullptr || height == nullptr || offsetX == nullptr ||
      offsetY == nullptr) {
    return false;
  }
  BrowserAccessibilityOHOS* node =
      BrowserAccessibilityOHOS::GetFromAccessibilityId(
          GetRealAccessibilityId(accessibilityId));
  if (node == nullptr) {
    LOG(ERROR) << "GetAccessibilityNodeRectById node is not found";
    return false;
  }

  ui::AXOffscreenResult offscreen_result = ui::AXOffscreenResult::kOnscreen;
  gfx::Rect rect = node->GetClippedRootFrameBoundsRect(&offscreen_result);

  *width = rect.width();
  *height = rect.height();
  *offsetX = rect.x();
  *offsetY = rect.y() + GetViewPointHeight();
  return true;
}

void NWebDelegate::SendAccessibilityHoverEvent(int x, int y, bool isHoverEnter) {
  if (accessibility_state_) {
    if (!isHoverEnter) {
      is_hovering_ = false;
      return;
    }
    is_hovering_ = true;
    auto* accessibilityManager = GetAccessibilityManager();
     if (accessibilityManager != nullptr && handler_delegate_ != nullptr) {
        auto scale = handler_delegate_->GetScale() / 100.0;
        gfx::Point point(x / scale, (y - GetViewPointHeight()) / scale);
        accessibilityManager->HitTest(point, 0);
    }
  }
}

BrowserAccessibilityManagerOHOS* NWebDelegate::GetAccessibilityManager() const {
  if (!accessibility_state_ || GetBrowser() == nullptr ||
      GetBrowser()->GetHost() == nullptr) {
    return nullptr;
  }
  void* manager = nullptr;
  GetBrowser()->GetHost()->GetRootBrowserAccessibilityManager(&manager);
  return static_cast<BrowserAccessibilityManagerOHOS*>(manager);
}

std::shared_ptr<NWebAccessibilityNodeInfo>
NWebDelegate::GetFocusedAccessibilityNodeInfo(int64_t accessibilityId,
                                              bool isAccessibilityFocus) {
  auto* accessibilityManager = GetAccessibilityManager();
  if (accessibilityManager == nullptr) {
    return nullptr;
  }
  auto rootNode = accessibilityManager->GetBrowserAccessibilityRoot();
  if (rootNode == nullptr) {
    LOG(ERROR) << "GetFocusedAccessibilityNodeInfo rootNode is not found";
    return nullptr;
  }
  BrowserAccessibilityOHOS* resultNode = nullptr;
  if (isAccessibilityFocus) {
    auto accessibilityFocusId = accessibilityManager->GetAccessibilityFocusId();
    if (accessibilityFocusId > 0) {
        resultNode = BrowserAccessibilityOHOS::GetFromAccessibilityId(
            accessibilityFocusId);
    }
  } else {
    resultNode = static_cast<BrowserAccessibilityOHOS*>(
        accessibilityManager->GetFocus());
    if (resultNode == rootNode) {
        return nullptr;
    }
  }
  if (resultNode == nullptr) {
    LOG(ERROR) << "GetFocusedAccessibilityNodeInfo resultNode is not found";
    return nullptr;
  }
  BrowserAccessibilityOHOS* node =
      BrowserAccessibilityOHOS::GetFromAccessibilityId(
          GetRealAccessibilityId(accessibilityId));
  if (node == nullptr || !resultNode->IsDescendantOf(node)) {
    LOG(ERROR) << "GetFocusedAccessibilityNodeInfo resultNode is not "
                  "descendant of node";
    return nullptr;
  }
  return PopulateAccessibilityNodeInfo(resultNode);
}

std::shared_ptr<NWebAccessibilityNodeInfo>
NWebDelegate::GetAccessibilityNodeInfoById(int64_t accessibilityId) {
  BrowserAccessibilityOHOS* node =
      BrowserAccessibilityOHOS::GetFromAccessibilityId(
          GetRealAccessibilityId(accessibilityId));
  if (node == nullptr) {
    LOG(ERROR) << "GetAccessibilityNodeInfoById node is not found";
    return nullptr;
  }
  return PopulateAccessibilityNodeInfo(node);
}

bool NWebDelegate::GetAccessibilityVisible(int64_t accessibilityId) {
  BrowserAccessibilityOHOS* node =
      BrowserAccessibilityOHOS::GetFromAccessibilityId(
          GetRealAccessibilityId(accessibilityId));
  if (node == nullptr) {
    LOG(ERROR) << "GetAccessibilityNodeInfoById node is not found";
    return true;
  }
  ui::AXOffscreenResult offscreen_result = ui::AXOffscreenResult::kOnscreen;
  node->GetUnclippedRootFrameBoundsRect(&offscreen_result);
  return offscreen_result == ui::AXOffscreenResult::kOnscreen;
}

std::shared_ptr<NWebAccessibilityNodeInfo>
NWebDelegate::GetAccessibilityNodeInfoByFocusMove(int64_t accessibilityId,
                                                  int32_t direction) {
  LOG(INFO) << "GetAccessibilityNodeInfoByFocusMove accessibilityId is "
            << accessibilityId << ", direction is " << direction;
  BrowserAccessibilityOHOS* node =
      BrowserAccessibilityOHOS::GetFromAccessibilityId(
          GetRealAccessibilityId(accessibilityId));
  if (node == nullptr) {
    LOG(ERROR) << "GetAccessibilityNodeInfoByFocusMove node is not found";
    return nullptr;
  }
  auto resultNode = node->GetAccessibilityNodeByFocusMove(direction);
  if (resultNode == nullptr) {
    LOG(ERROR) << "GetAccessibilityNodeInfoByFocusMove resultNode is not found";
    return nullptr;
  }
  return PopulateAccessibilityNodeInfo(resultNode);
}

std::shared_ptr<NWebAccessibilityNodeInfo>
NWebDelegate::PopulateAccessibilityNodeInfo(BrowserAccessibilityOHOS* node) {
  auto* accessibilityManager = GetAccessibilityManager();
  if (accessibilityManager == nullptr) {
    return nullptr;
  }

  std::shared_ptr<NWebAccessibilityNodeInfoImpl> nodeInfo =
      std::make_shared<NWebAccessibilityNodeInfoImpl>();
  if (nodeInfo == nullptr || node == nullptr) {
    LOG(ERROR) << "PopulateAccessibilityNodeInfo nodeInfo or node is null";
    return nullptr;
  }
  nodeInfo->SetAccessibilityId(kRootAccessibilityId);
  nodeInfo->SetParentId(-1);
  bool isRoot = !node->PlatformGetParent();
  if (!isRoot) {
    nodeInfo->SetAccessibilityId(node->GetAccessibilityId());
    auto* parentNode =
        static_cast<BrowserAccessibilityOHOS*>(node->PlatformGetParent());
    if (parentNode) {
        if (parentNode->PlatformGetParent()) {
        nodeInfo->SetParentId(parentNode->GetAccessibilityId());
        } else {
        nodeInfo->SetParentId(kRootAccessibilityId);
        }
    }
  }

  std::vector<int64_t> childIds;
  for (const auto& childNode : node->PlatformChildren()) {
    const BrowserAccessibilityOHOS& childNodeOHOS =
        static_cast<const BrowserAccessibilityOHOS&>(childNode);
    childIds.emplace_back(childNodeOHOS.GetAccessibilityId());
  }
  nodeInfo->SetChildIds(childIds);
  nodeInfo->SetIsAccessibilityFocus(
      (accessibilityManager->GetAccessibilityFocusId() ==
               node->GetAccessibilityId()
           ? true
           : false));

  AddAccessibilityNodeInfoAttributes(nodeInfo, node);
  AddAccessibilityNodeInfoRect(nodeInfo, node);
  AddAccessibilityNodeInfoCollection(nodeInfo, node);
  AddAccessibilityNodeInfoActions(nodeInfo, node);

  return nodeInfo;
}

void NWebDelegate::AddAccessibilityNodeInfoAttributes(
    std::shared_ptr<NWebAccessibilityNodeInfoImpl> nodeInfo,
    const BrowserAccessibilityOHOS* node) const {
  nodeInfo->SetComponentType(node->GetRoleString());
  nodeInfo->SetIsFocused(node->IsFocused());
  nodeInfo->SetIsVisible(!node->IsInvisibleOrIgnored());
  nodeInfo->SetIsEnabled(node->IsEnabled());
  nodeInfo->SetIsFocusable(node->IsFocusable());
  if (node->IsPasswordField()) {
    nodeInfo->SetContent("*");
    nodeInfo->SetDescriptionInfo("");
  } else {
    nodeInfo->SetContent(base::UTF16ToUTF8(node->GetTextContentUTF16()));
    nodeInfo->SetDescriptionInfo("");
  }

  nodeInfo->SetHint(node->GetHint());
  nodeInfo->SetIsHinting(node->IsHint());
  nodeInfo->SetIsChecked(node->IsChecked());
  nodeInfo->SetIsSelected(node->IsSelected());
  nodeInfo->SetIsPassword(node->IsPasswordField());
  nodeInfo->SetIsCheckable(node->IsCheckable());
  nodeInfo->SetIsScrollable(node->IsScrollable());
  nodeInfo->SetIsEditable(node->IsTextField());
  nodeInfo->SetIsPluralLineSupported(node->IsMultiLine());
  nodeInfo->SetIsPopupSupported(node->CanOpenPopup());
  nodeInfo->SetError(node->GetContentInvalidErrorMessage());
  nodeInfo->SetIsContentInvalid(node->IsContentInvalid());
  nodeInfo->SetIsDeletable(false);
  nodeInfo->SetInputType(node->OHOSInputType());
  nodeInfo->SetLiveRegion(node->OHOSLiveRegionType());
  if (node->IsTextField()) {
    nodeInfo->SetSelectionStart(node->GetSelectionStart());
    nodeInfo->SetSelectionEnd(node->GetSelectionEnd());
  } else {
    nodeInfo->SetSelectionStart(0);
    nodeInfo->SetSelectionEnd(0);
  }
  nodeInfo->SetItemCounts(node->GetItemCount());
  nodeInfo->SetIsClickable(node->IsClickable());
  if (node->IsRangeControlWithoutAriaValueText()) {
    nodeInfo->SetRangeInfoMin(node->RangeMin());
    nodeInfo->SetRangeInfoMax(node->RangeMax());
    nodeInfo->SetRangeInfoCurrent(node->RangeCurrentValue());
  } else {
    nodeInfo->SetRangeInfoMin(0.0f);
    nodeInfo->SetRangeInfoMax(0.0f);
    nodeInfo->SetRangeInfoCurrent(0.0f);
  }
  nodeInfo->SetIsAccessibilityGroup(node->IsAccessibilityGroup());
}

void NWebDelegate::AddAccessibilityNodeInfoRect(
    std::shared_ptr<NWebAccessibilityNodeInfoImpl> nodeInfo,
    const BrowserAccessibilityOHOS* node) const {
  ui::AXOffscreenResult offscreen_result = ui::AXOffscreenResult::kOnscreen;
  gfx::Rect absolute_rect =
      node->GetUnclippedRootFrameBoundsRect(&offscreen_result);

  nodeInfo->SetRectX(absolute_rect.x());
  nodeInfo->SetRectY(absolute_rect.y() + GetViewPointHeight());
  nodeInfo->SetRectWidth(absolute_rect.width());
  nodeInfo->SetRectHeight(absolute_rect.height());
}

float NWebDelegate::GetViewPointHeight() const {
  float height = 0.0f;
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "GetViewPointHeight can not get browser";
    return height;
  }
  auto viewPointHeight = GetBrowser()->GetHost()->GetShrinkViewportHeight();
  if (viewPointHeight != 0 && render_handler_ != nullptr) {
    CefScreenInfo screen_info;
    render_handler_->GetScreenInfo(GetBrowser(), screen_info);
    height = viewPointHeight * screen_info.device_scale_factor;
  }
  return height;
}

void NWebDelegate::AddAccessibilityNodeInfoCollection(
    std::shared_ptr<NWebAccessibilityNodeInfoImpl> nodeInfo,
    const BrowserAccessibilityOHOS* node) const {
  nodeInfo->SetGridRows(-1);
  nodeInfo->SetGridColumns(-1);
  nodeInfo->SetGridSelectedMode(-1);
  if (node->IsCollection()) {
    nodeInfo->SetGridRows(node->RowCount());
    nodeInfo->SetGridColumns(node->ColumnCount());
    nodeInfo->SetGridSelectedMode(node->IsHierarchical());
  }
  nodeInfo->SetGridItemRow(-1);
  nodeInfo->SetGridItemRowSpan(-1);
  nodeInfo->SetGridItemColumn(-1);
  nodeInfo->SetGridItemColumnSpan(-1);
  nodeInfo->SetIsHeading(false);
  if (node->IsCollectionItem() || node->IsTableHeader()) {
    nodeInfo->SetGridItemRow(node->RowIndex());
    nodeInfo->SetGridItemRowSpan(node->RowSpan());
    nodeInfo->SetGridItemColumn(node->ColumnIndex());
    nodeInfo->SetGridItemColumnSpan(node->ColumnSpan());
    nodeInfo->SetIsHeading(node->IsTableHeader());
  }
}

void NWebDelegate::AddAccessibilityNodeInfoActions(
    std::shared_ptr<NWebAccessibilityNodeInfoImpl> nodeInfo,
    const BrowserAccessibilityOHOS* node) const {
  std::vector<uint32_t> actions = nodeInfo->GetActions();
  actions.clear();
  if (nodeInfo->GetIsClickable()) {
    actions.emplace_back(static_cast<uint32_t>(AceAction::ACTION_CLICK));
  }
  if (nodeInfo->GetIsFocusable()) {
    if (nodeInfo->GetIsFocused()) {
        actions.emplace_back(
            static_cast<uint32_t>(AceAction::ACTION_CLEAR_FOCUS));
    } else {
        actions.emplace_back(static_cast<uint32_t>(AceAction::ACTION_FOCUS));
    }
  }

  if (nodeInfo->GetIsAccessibilityFocus()) {
    actions.emplace_back(
        static_cast<uint32_t>(AceAction::ACTION_CLEAR_ACCESSIBILITY_FOCUS));
  } else {
    actions.emplace_back(
        static_cast<uint32_t>(AceAction::ACTION_ACCESSIBILITY_FOCUS));
  }
  if (node != nullptr) {
    if (node->CanScrollForward()) {
        actions.emplace_back(
            static_cast<uint32_t>(AceAction::ACTION_SCROLL_FORWARD));
    }
    if (node->CanScrollBackward()) {
        actions.emplace_back(
            static_cast<uint32_t>(AceAction::ACTION_SCROLL_BACKWARD));
    }
    if (nodeInfo->GetIsEditable() && nodeInfo->GetIsEnabled()) {
        actions.emplace_back(static_cast<uint32_t>(AceAction::ACTION_PASTE));
        if (node->HasNonEmptyValue()) {
        actions.emplace_back(
            static_cast<uint32_t>(AceAction::ACTION_SET_SELECTION));
        actions.emplace_back(static_cast<uint32_t>(AceAction::ACTION_CUT));
        actions.emplace_back(static_cast<uint32_t>(AceAction::ACTION_COPY));
        }
    }
  }
  actions.emplace_back(static_cast<uint32_t>(AceAction::ACTION_NEXT_HTML_ITEM));
  actions.emplace_back(
      static_cast<uint32_t>(AceAction::ACTION_PREVIOUS_HTML_ITEM));
  nodeInfo->SetActions(actions);
}
#endif  // BUILDFLAG(ARKWEB_ACCESSIBILITY)

void NWebDelegate::SuggestionSelected(int index) const {
  LOG(INFO) << "NWebDelegate::SuggestionSelected";
  if (GetBrowser().get()) {
    GetBrowser()->PasswordSuggestionSelected(index);
  }
}

#if BUILDFLAG(ARKWEB_DISCARD)
bool NWebDelegate::Discard() {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "NWebDelegate::Discard failed, browser is nullptr";
    return false;
  }

  if (is_discarded_) {
    LOG(ERROR) << "NWebDelegate::Discard failed, the webview window was "
                  "discarded before";
    return false;
  }

  is_discarded_ = GetBrowser()->GetHost()->Discard();
  LOG(DEBUG) << "NWebDelegate::Discard is_discarded_: " << is_discarded_;

  if (handler_delegate_) {
    handler_delegate_->Discard();
  }
  return is_discarded_;
}

bool NWebDelegate::Restore() {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "NWebDelegate::Restore failed, browser is nullptr";
    return false;
  }

  if (!is_discarded_) {
    LOG(ERROR) << "NWebDelegate::Restore failed, the webview window was not "
                  "discarded before";
    return false;
  }

  bool is_restored = GetBrowser()->GetHost()->Restore();
  is_discarded_ = !is_restored;
  LOG(DEBUG) << "NWebDelegate::Restore is_restored: " << is_restored;
  return is_restored;
}
#endif

#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
int NWebDelegate::InsertBackForwardEntry(int offset, const std::string& url) {
  if (GetBrowser().get()) {
    return GetBrowser()->InsertBackForwardEntry(offset, url);
  }
  return NWebNavigationEntryUpdateResult::ERR_OTHER;
}

int NWebDelegate::UpdateNavigationEntryUrl(int index, const std::string& url) {
  if (GetBrowser().get()) {
    return GetBrowser()->UpdateNavigationEntryUrl(index, url);
  }
  return NWebNavigationEntryUpdateResult::ERR_OTHER;
}

void NWebDelegate::ClearForwardList() {
  if (GetBrowser().get()) {
    GetBrowser()->ClearForwardList();
  }
}
#endif  // BUILDFLAG(ARKWEB_EXT_NAVIGATION)

#if BUILDFLAG(ARKWEB_ITP)
void NWebDelegate::EnableIntelligentTrackingPrevention(bool enable) {
  if (GetBrowser() == nullptr) {
    LOG(ERROR) << "NWebDelegate::EnableIntelligentTrackingPrevention failed "
                  "for browser is nullptr";
    return;
  }
  GetBrowser()->EnableIntelligentTrackingPrevention(enable);
}

bool NWebDelegate::IsIntelligentTrackingPreventionEnabled() const {
  if (GetBrowser() == nullptr) {
    LOG(ERROR) << "NWebDelegate::IsIntelligentTrackingPreventionEnabled "
                  "failed for browser is nullptr";
    return false;
  }
  return GetBrowser()->IsIntelligentTrackingPreventionEnabled();
}
#endif

#if BUILDFLAG(ARKWEB_SCREEN_LOCK)
void NWebDelegate::SetWakeLockCallback(
    int32_t windowId,
    const std::shared_ptr<NWebScreenLockCallback>& callback) {
  if (handler_delegate_) {
    handler_delegate_->SetWakeLockCallback(windowId, callback);
  }
}
#endif

#if BUILDFLAG(ARKWEB_SECURE_JAVASCRIPT_PROXY)
std::string NWebDelegate::GetLastJavascriptProxyCallingFrameUrl() {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "GetLastJavascriptProxyCallingFrameUrl can not get browser";
    return "";
  }

  return GetBrowser()->GetHost()->GetLastJavascriptProxyCallingFrameUrl();
}
#endif

int NWebDelegate::ScaleGestureChange(double scale,
                                     double centerX,
                                     double centerY) const {
  return NWEB_OK;
}

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
void NWebDelegate::RegisterOnCreateNativeMediaPlayerListener(
    std::shared_ptr<NWebCreateNativeMediaPlayerCallback> callback) {
  if (handler_delegate_ == nullptr) {
    LOG(ERROR) << "fail to set create native media player callback, NWEB "
                  "handler is nullptr";
    return;
  }
  handler_delegate_->RegisterOnCreateNativeMediaPlayerListener(
      std::move(callback));
}
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void NWebDelegate::EnableVideoAssistant(bool enable) {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    if (!handler_delegate_) {
      LOG(ERROR)
          << "failed to enable video assistant, handler delegate is null";
      return;
    }
    handler_delegate_->EnableVideoAssistant(enable);
    return;
  }

  GetBrowser()->GetHost()->EnableVideoAssistant(enable);
}

void NWebDelegate::ExecuteVideoAssistantFunction(const std::string& cmd_id) {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "failed to get host when execute video assistant function";
    return;
  }

  GetBrowser()->GetHost()->ExecuteVideoAssistantFunction(cmd_id);
}

void NWebDelegate::CustomWebMediaPlayer(bool enable) {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    if (!handler_delegate_) {
      LOG(ERROR) << "failed to enable custom web media player, handler delegate is null";
      return;
    }
    handler_delegate_->CustomWebMediaPlayer(enable);
    return;
  }

  GetBrowser()->GetHost()->CustomWebMediaPlayer(enable);
}
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_MENU)
void NWebDelegate::SetIsRichText(bool is_rich_text) {
  if (!handler_delegate_) {
    LOG(ERROR) << "fail to set is_rich_text, NWEB handler is nullptr";
    return;
  }
  handler_delegate_->SetIsRichText(is_rich_text);
}
#endif

std::string NWebDelegate::GetSelectInfo() {
  return std::string();
}

#if BUILDFLAG(ARKWEB_AI_WRITE)
int NWebDelegate::GetSelectStartIndex()
{
  return 0;
}
int NWebDelegate::GetSelectEndIndex()
{
  return 0;
}
std::string NWebDelegate::GetAllTextInfo()
{
  return std::string();
}
#endif // ARKWEB_AI_WRITE
#if BUILDFLAG(ARKWEB_AI)
void NWebDelegate::OnTextSelected() {
  LOG(INFO) << "NWebDelegate::OnTextSelected";
  if (!GetBrowser().get()) {
    return;
  }
  GetBrowser()->GetHost()->OnTextSelected(true);
}

void NWebDelegate::OnDestroyImageAnalyzerOverlay() {
  LOG(INFO) << "NWebDelegate::OnDestroyImageAnalyzerOverlay";
  if (!GetBrowser().get()) {
    return;
  }
  GetBrowser()->GetHost()->OnDestroyImageAnalyzerOverlay();
}

void NWebDelegate::OnFoldStatusChanged(FoldStatus foldstatus) {
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(CEF_UIT, base::BindOnce(&NWebDelegate::OnFoldStatusChanged, this, foldstatus));
    return;
  }
  LOG(INFO) << "NWebDelegate::OnFoldStatusChanged " << static_cast<uint32_t>(foldstatus);
  if (!GetBrowser().get()) {
    return;
  }
  GetBrowser()->GetHost()->OnFoldStatusChanged(
      static_cast<uint32_t>(foldstatus));
}

void NWebDelegate::RunDataDetectorJS() {
  if (!data_detector_enable_) {
    return;
  }
  LOG(INFO) << "NWebDelegate::RunDataDetectorJS";
  const auto& rb = ui::ResourceBundle::GetSharedInstance();
  std::string_view script_data = rb.GetRawDataResource(IDR_DATA_DETECTOR_JS);

  LOG(DEBUG) << "NWebDelegate::GetAndRunDataDetectorJS script_data size:" << script_data.size();

  std::string code(script_data.data(), script_data.size());

  CEF_POST_TASK(
      CEF_UIT,
      base::BindOnce((void(NWebDelegate::*)(
                         const std::string&,
                         std::shared_ptr<NWebMessageValueCallback>, bool)) &
                         NWebDelegate::ExecuteJavaScript,
                     this, code, nullptr, false));
}

void NWebDelegate::SetDataDetectorEnable(bool enable) {
  LOG(INFO) << "NWebDelegate::SetDataDetectorEnable: " << enable;
  data_detector_enable_ = enable;
}

bool NWebDelegate::GetDataDetectorEnable() {
  return data_detector_enable_;
}

std::string NWebDelegate::GetDataDetectorSelectText() {
  if (!data_detector_enable_) {
    return std::string();
  }
  if (GetBrowser().get()) {
    return GetBrowser()->GetHost()->GetDataDetectorSelectText();
  }
  return std::string();
}

void NWebDelegate::OnDataDetectorSelectText() {
  if (!data_detector_enable_) {
    return;
  }
  LOG(INFO) << "NWebDelegate::OnDataDetectorSelectText";
  if (GetBrowser().get()) {
    GetBrowser()->GetHost()->OnDataDetectorSelectText();
  }
}

void NWebDelegate::OnDataDetectorCopy(const std::vector<std::string>& recordMix) {
  if (!data_detector_enable_) {
    return;
  }
  LOG(INFO) << "NWebDelegate::OnDataDetectorCopy";
  if (!preference_delegate_ || recordMix.size() < 2) {
    return;
  }
  auto copy_option =
      static_cast<CopyOptionMode>(preference_delegate_->GetCopyOptionMode());

  std::shared_ptr<PasteDataRecordAdapter> record =
      PasteDataRecordAdapter::NewRecord("text/html");
  PasteRecordVector recordVector;

  std::shared_ptr<std::string> text =
      std::make_shared<std::string>(recordMix[0]);
  std::shared_ptr<std::string> html =
      std::make_shared<std::string>(recordMix[1]);
  if (record->SetPlainText(text) && record->SetHtmlText(html)) {
    recordVector.emplace_back(record);
    LOG(INFO) << "set text to record success";
  } else {
    LOG(ERROR) << "set text to record failed";
  }
  base::ThreadPool::PostTask(
      FROM_HERE,
      base::BindOnce(
          [](const PasteRecordVector& record_vector, CopyOptionMode copy_option) {
            OhosAdapterHelper::GetInstance().GetPasteBoard().SetPasteData(
                record_vector, copy_option);
            LOG(INFO) << "copy data detector result to clipboard success";
          },
          recordVector, copy_option));
}
#endif

#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
void NWebDelegate::OnSafeInsetsChange(int left,
                                      int top,
                                      int right,
                                      int bottom) {
  if (!GetBrowser().get()) {
    return;
  }
  GetBrowser()->GetHost()->OnSafeInsetsChange(left, top, right, bottom);
}
#endif

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
void NWebDelegate::SetWholePageDrawing() {
  if (preference_delegate_) {
    preference_delegate_->SetWholePageDrawing();
  }
}

bool NWebDelegate::WebPageSnapshot(const char* id,
                                   PixelUnit type,
                                   int width,
                                   int height,
                                   const WebSnapshotCallback callback) {
  if (!GetBrowser().get()) {
    LOG(ERROR) << "NWebDelegate::WebPageSnapshot can not get browser";
    return false;
  }

  if (!handler_delegate_) {
    LOG(ERROR) << "NWebDelegate::WebPageSnapshot can not get handler";
    return false;
  }

  if (!handler_delegate_->IsWebPaintedForSnapshot()) {
    LOG(ERROR) << "Web is not painted ready for snapshot";
    return false;
  }

  float ratio = render_handler_->GetVirtualPixelRatio();

  switch (type) {
    case PixelUnit::VP:
      width = width * ratio;
      height = height * ratio;
      break;
    case PixelUnit::PERCENTAGE:
      width = -width;
      height = -height;
      break;
    default:
      LOG(INFO) << "NWebDelegate should not trans data";
      break;
  }

  return GetBrowser()->GetHost()->WebPageSnapshot(
      id, width, height,
      base::BindOnce(
          [](WebSnapshotCallback napiCallback, float ratio, const char* id,
             bool state, void* data, int width, int height) {
            napiCallback(id, state, ratio, data, width, height);
          },
          std::move(callback), ratio));
}
#endif

#if BUILDFLAG(ARKWEB_URL_TRUST_LIST)
int NWebDelegate::SetUrlTrustListWithErrMsg(const std::string& urlTrustList,
                                            std::string& detailErrMsg) {
  LOG(INFO) << "NWebDelegate::SetUrlTrustListErrMsg";
  if (!GetBrowser().get()) {
    LOG(ERROR)
        << "NWebDelegate::SetUrlTrustListWithErrMsg failed, get browser failed";
    return static_cast<int>(ohos_safe_browsing::UrlListSetResult::INIT_ERROR);
  }
  CefString detailErrMsgCef;
  int res =
      GetBrowser()->SetUrlTrustListWithErrMsg(urlTrustList, detailErrMsgCef);
  detailErrMsg = detailErrMsgCef.ToString();
  return res;
}
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
void NWebDelegate::SetPathAllowingUniversalAccess(
    const std::vector<std::string>& pathList) {
  if (!GetBrowser().get() || !GetBrowser()->GetHost() ||
      !preference_delegate_) {
    LOG(ERROR) << "NWebDelegate::SetPathAllowingUniversalAccess failed, get "
                  "browser failed";
    return;
  }
  preference_delegate_->PutEnableUniversalAccessFromFileURLs(pathList.size() !=
                                                             0);
  std::vector<CefString> cef_path_list;
  std::for_each(pathList.begin(), pathList.end(),
                [&cef_path_list](const std::string& path) {
                  cef_path_list.emplace_back(CefString(path));
                });
  GetBrowser()->GetHost()->SetGrantFileAccessDirs(cef_path_list);
}
#endif

#if BUILDFLAG(ARKWEB_EXT_FILE_ACCESS)
void NWebDelegate::DisallowSandboxFileAccessFromFileUrl(bool disallow) {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "NWebDelegate::DisallowSandboxFileAccessFromFileUrl failed,"
                  " get browser failed";
    return;
  }
  GetBrowser()->GetHost()->SetDisallowSandboxFileAccessFromFileUrl(disallow);
}
#endif  // BUILDFLAG(ARKWEB_EXT_FILE_ACCESS)

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
void NWebDelegate::WebExtensionTabCreated(int tab_id) {
  LOG(INFO) << "WebExtensionTabCreated:" << tab_id;
  if (!GetBrowser().get() || !GetBrowser()->GetHost()) {
    LOG(ERROR) << "WebExtensionTabCreated failed, get browser failed";
    return;
  }

  return GetBrowser()->ExtensionSetTabId(tab_id);
}

void NWebDelegate::WebExtensionTabRemoved(int tab_id) {
  LOG(INFO) << "WebExtensionTabRemoved:" << tab_id;
  if (!GetBrowser().get() || !GetBrowser()->GetHost()) {
    LOG(ERROR) << "WebExtensionTabRemoved failed, get browser failed";
    return;
  }

  return GetBrowser()->ExtensionSetTabId(tab_id);
}

void NWebDelegate::WebExtensionTabUpdated(
    int tab_id,
    const std::vector<std::string>& changed_property_names,
    const std::string& url) {
  LOG(INFO) << "WebExtensionTabUpdated:" << tab_id;
  if (!GetBrowser().get() || !GetBrowser()->GetHost()) {
    LOG(ERROR) << "WebExtensionTabUpdated failed, get browser failed";
    return;
  }

  GetBrowser()->ExtensionSetTabId(tab_id);

  std::vector<CefString> changed_properties;
  std::for_each(changed_property_names.begin(), changed_property_names.end(),
                [&changed_properties](const std::string& name) {
                  changed_properties.emplace_back(CefString(name));
                });
  return GetBrowser()->GetHost()->WebExtensionTabUpdated(
      tab_id, changed_properties, url);
}

void NWebDelegate::WebExtensionTabUpdated(int tab_id,
    const std::vector<std::string>& changed_property_names,
    std::unique_ptr<NWebExtensionTabChangeInfo> changeInfo) {
  LOG(INFO) << "WebExtensionTabUpdated:" << tab_id;
  if (!GetBrowser().get() || !GetBrowser()->GetHost()) {
    LOG(ERROR) << "WebExtensionTabUpdated failed, get browser failed";
    return;
  }
  GetBrowser()->ExtensionSetTabId(tab_id);
 
  std::vector<CefString> changed_properties;
  std::for_each(changed_property_names.begin(), changed_property_names.end(),
      [&changed_properties] (const std::string& name) {
    changed_properties.emplace_back(CefString(name));
  });
  return GetBrowser()->GetHost()->WebExtensionTabUpdated(
      tab_id, changed_properties, std::move(changeInfo));
}
 
void NWebDelegate::WebExtensionTabActivated(
    std::unique_ptr<NWebExtensionTabActiveInfo> activeInfo) {
  LOG(INFO) << "WebExtensionTabActivated, tab_id: "
            << activeInfo->tabId << " windowId: " << activeInfo->windowId;
  if (!GetBrowser().get() || !GetBrowser()->GetHost()) {
    LOG(ERROR) << "WebExtensionTabActivated failed, get browser failed";
    return;
  }
  return GetBrowser()->GetHost()->WebExtensionTabActivated(activeInfo->tabId, activeInfo->windowId);
}
 
void NWebDelegate::WebExtensionTabAttached(
    std::unique_ptr<NWebExtensionTabAttachInfo> attachInfo) {
  LOG(INFO) << "WebExtensionTabAttached, newPosition: "
            << attachInfo->newPosition
            << " newWindowId: " << attachInfo->newWindowId;
  if (!GetBrowser().get() || !GetBrowser()->GetHost()) {
    LOG(ERROR) << "WebExtensionTabAttached failed, get browser failed";
    return;
  }
}
 
void NWebDelegate::WebExtensionTabDetached(
    std::unique_ptr<NWebExtensionTabDetachInfo> detachInfo) {
  LOG(INFO) << "WebExtensionTabDetached, oldPosition: "
            << detachInfo->oldPosition
            << " oldWindowId: " << detachInfo->oldWindowId;
  if (!GetBrowser().get() || !GetBrowser()->GetHost()) {
    LOG(ERROR) << "WebExtensionTabDetached failed, get browser failed";
    return;
  }
}
 
void NWebDelegate::WebExtensionTabHighlighted(int32_t tab_id,
                                              int32_t window_id) {
  LOG(INFO) << "WebExtensionTabHighlighted, tab_id: " << tab_id
            << " windowId: " << window_id;
  if (!GetBrowser().get() || !GetBrowser()->GetHost()) {
    LOG(ERROR) << "WebExtensionTabHighlighted failed, get browser failed";
    return;
  }
}
 
void NWebDelegate::WebExtensionTabMoved(
    int32_t tab_id,
    std::unique_ptr<NWebExtensionTabMoveInfo> moveInfo) {
  LOG(INFO) << "WebExtensionTabMoved, tab_id: " << tab_id
            << " from: " << moveInfo->fromIndex << " to: " << moveInfo->toIndex
            << "window id: " << moveInfo->windowId;
  if (!GetBrowser().get() || !GetBrowser()->GetHost()) {
    LOG(ERROR) << "WebExtensionTabMoved failed, get browser failed";
    return;
  }
}
 
void NWebDelegate::WebExtensionTabReplaced(int32_t addedTabId,
                                           int32_t removedTabId) {
  LOG(INFO) << "WebExtensionTabReplaced, addedTabId: " << addedTabId
            << " removedTabId: " << removedTabId;
  if (!GetBrowser().get() || !GetBrowser()->GetHost()) {
    LOG(ERROR) << "WebExtensionTabReplaced failed, get browser failed";
    return;
  }
}
 
void NWebDelegate::WebExtensionTabZoomChange(
    std::unique_ptr<NWebExtensionTabZoomChangeInfo> tabZoomChangeInfo) {
  LOG(INFO) << "WebExtensionTabZoomChange, tab_id: "
            << tabZoomChangeInfo->tabId
            << " newZoomFactor: " << tabZoomChangeInfo->newZoomFactor
            << " oldZoomFactor: " << tabZoomChangeInfo->oldZoomFactor;
  if (!GetBrowser().get() || !GetBrowser()->GetHost()) {
    LOG(ERROR) << "WebExtensionTabZoomChange failed, get browser failed";
    return;
  }
}
 
void NWebDelegate::WebExtensionActionClicked(std::string extensionId,
                                             const NWebExtensionTab* tab) {
  LOG(DEBUG) << "NWebDelegate WebExtensionActionClicked";
  if (!GetBrowser().get() || !GetBrowser()->GetHost()) {
    LOG(ERROR) << "ActionClicked failed, get browser failed";
    return;
  }
 
  GetBrowser()->GetHost()->WebExtensionActionClicked(extensionId, tab);
}
#endif

#if BUILDFLAG(ARKWEB_BFCACHE)
void NWebDelegate::SetBackForwardCacheOptions(int32_t size,
                                              int32_t timeToLive) {
  if (!GetBrowser()) {
    if (preference_delegate_) {
      preference_delegate_->PutBackForwardCacheOptions(size, timeToLive);
    }
    return;
  }

  GetBrowser()->SetBackForwardCacheOptions(size, timeToLive);
}
#endif

#if BUILDFLAG(ARKWEB_MIXED_CONTENT)
void NWebDelegate::EnableMixedContentAutoUpgrades(bool enable) {
  LOG(DEBUG) << "NWebDelegate::EnableMixedContentAutoUpgrades " << enable;
  if (preference_delegate_) {
    preference_delegate_->EnableMixedContentAutoUpgrades(enable);
  } else {
    LOG(ERROR) << "NWebDelegate::EnableMixedContentAutoUpgrades"
                  "get preference_delegate failed ";
  }
}

bool NWebDelegate::IsMixedContentAutoUpgradesEnabled() {
  if (preference_delegate_) {
    return preference_delegate_->IsMixedContentAutoUpgradesEnabled();
  } else {
    LOG(ERROR) << "NWebDelegate::IsMixedContentAutoUpgradesEnabled"
                  "get preference_delegate failed ";
  }
  return false;
}
#endif

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
void NWebDelegate::SetUsageScenario(int32_t usage_scenario) {
  if (preference_delegate_) {
    preference_delegate_->SetUsageScenario(usage_scenario);
  } else {
    LOG(ERROR) << "NWebDelegate::SetUsageScenario"
                  "get preference_delegate failed ";
  }
}

int32_t NWebDelegate::GetUsageScenario() {
  if (preference_delegate_) {
    return preference_delegate_->GetUsageScenario();
  } else {
    LOG(ERROR) << "NWebDelegate::GetUsageScenario"
                  "get preference_delegate failed ";
  }
  return 0;
}
#endif

#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
void NWebDelegate::RefreshAccessibilityManagerClickEvent() {
  if (accessibility_state_) {
    if (!GetAccessibilityManager()) {
      LOG(ERROR) << "GetAccessibilityManager is null";
    }
  }
}
#endif
#if BUILDFLAG(ARKWEB_MEDIA_NETWORK_TRAFFIC_PROMPT)
void NWebDelegate::EnableMediaNetworkTrafficPrompt(bool enable) {
  if (!preference_delegate_) {
    LOG(INFO)
        << "EnableMediaNetworkTrafficPrompt failed, no preference_delegate_"
        << ", nweb_id_[" << nweb_id_ << "]";
    return;
  }
  LOG(INFO) << "EnableMediaNetworkTrafficPrompt, preference_delegate_["
            << preference_delegate_.get() << "], nweb_id_[" << nweb_id_ << "]";
  preference_delegate_->EnableMediaNetworkTrafficPrompt(enable);
}
#endif  // ARKWEB_MEDIA_NETWORK_TRAFFIC_PROMPT

void NWebDelegate::SetSurfaceDensity(const double& density) {
  display_ratio_ = density;
  SetVirtualPixelRatio(density);
  std::shared_ptr<DisplayAdapter> display =
      display_manager_adapter_->GetDefaultDisplay();
  LOG(INFO) << "SetSurfaceDensity: " << density;
  if (display != nullptr) {
    NotifyScreenInfoChanged(display->GetRotation(),
                            display->GetDisplayOrientation());
  }
}

int NWebDelegate::ScaleGestureChangeV2(int type,
                                       double scale,
                                       double originScale,
                                       double centerX,
                                       double centerY) const {
  LOG(DEBUG) << "NWebDelegate::ScaleGestureChangeV2";
  if (!preference_delegate_) {
    LOG(ERROR) << "preference_delegate_ get fail";
    return NWEB_ERR;
  }
  if (!preference_delegate_->ZoomingfunctionEnabled()) {
    return NWEB_FUNCTION_NOT_ENABLE;
  }
  if (!GetBrowser().get()) {
    LOG(ERROR) << "NWebDelegate::ScaleGestureChangeV2 can not get browser";
    return NWEB_ERR;
  }

  GetBrowser()->GetHost()->ScaleGestureChangeV2(
      type, scale, originScale, centerX / default_virtual_pixel_ratio_,
      centerY / default_virtual_pixel_ratio_);
  return NWEB_OK;
}

int32_t NWebDelegate::GetArgumentByKey(
    const std::map<std::string, std::string>& actionArguments,
    const std::string& checkKey) const {
  auto iter = actionArguments.find(checkKey);
  int32_t argument = -1;  // -1:default value
  if (iter != actionArguments.end()) {
    std::stringstream strArguments;
    strArguments << iter->second;
    strArguments >> argument;
  }
  return argument;
}

void NWebDelegate::OpenDevtoolsWith(
      std::shared_ptr<NWebDelegateInterface> nweb_delegate,
      std::unique_ptr<OpenDevToolsParam> param) {
  LOG(INFO) << "NWebDelegate::OpenDevtoolsWith";
  if (!GetBrowser() || !GetBrowser()->GetHost()) {
    LOG(INFO) << "OpenDevtoolsWith failed, no browser host";
    return;
  }
  NWebDelegate* devtools_delegate =
      static_cast<NWebDelegate*>(nweb_delegate.get());

#if BUILDFLAG(ARKWEB_DEVTOOLS)
  auto devtools_message_handler = CefRefPtr<NWebDevToolsMessageHandlerImpl>(
      new NWebDevToolsMessageHandlerImpl(std::move(param->handler)));

  CefPoint inspect_element_at(param->point.x, param->point.y);
  GetBrowser()->GetHost()->ShowDevToolsWith(
      devtools_delegate->GetBrowser()->GetHost(),
      devtools_message_handler, inspect_element_at);
#endif // #if BUILDFLAG(ARKWEB_DEVTOOLS)
}

void NWebDelegate::CloseDevtools() {
  LOG(INFO) << "NWebDelegate::CloseDevtools";
  if (!GetBrowser() || !GetBrowser()->GetHost()) {
    LOG(INFO) << "CloseDevtools failed, no browser host";
    return;
  }
  GetBrowser()->GetHost()->CloseDevTools();
}

#if BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
void NWebDelegate::SetPopupSurface(void* popupSurface) {
  if (handler_delegate_ == nullptr) {
    LOG(ERROR) << "fail to set popup surface handle_delegate_ don't exist.";
    return;
  }
  handler_delegate_->SetPopupSurface(popupSurface);
}
#endif

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
void NWebDelegate::MaximizeResize() {
  if (GetBrowser().get()) {
    GetBrowser()->GetHost()->MaximizeResize();
  }
}
#endif  // ARKWEB_MAXIMIZE_RESIZE

#if BUILDFLAG(ARKWEB_DISATCH_BEFORE_UNLOAD)
bool NWebDelegate::NeedToFireBeforeUnloadOrUnloadEvents() {
  if (GetBrowser().get()) {
    return GetBrowser()->NeedToFireBeforeUnloadOrUnloadEvents();
  }
  return false;
}

void NWebDelegate::DispatchBeforeUnload() {
  LOG(INFO) << "NWebDelegate::DispatchBeforeUnload";
  if (GetBrowser().get()) {
    GetBrowser()->DispatchBeforeUnload();
  }
}
#endif  // ARKWEB_DISATCH_BEFORE_UNLOAD

#if BUILDFLAG(ARKWEB_EX_REFRESH_IFRAME)
bool NWebDelegate::WebExtensionContextMenuIsIframe() {
  if (!GetBrowser().get() || !GetBrowser()->GetHost()) {
    LOG(ERROR) << "get browser failed or get host failed";
    return false;
  }
  return GetBrowser()->GetHost()->IsIframe();
}

void NWebDelegate::WebExtensionContextMenuReloadFocusedFrame() {
  if (!GetBrowser().get() || !GetBrowser()->GetHost()) {
    LOG(ERROR) << "get browser failed or get host failed";
    return;
  }
  return GetBrowser()->GetHost()->ReloadFocusedFrame();
}
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
bool NWebDelegate::SetFocusByPosition(float x, float y)
{
  if (!GetBrowser().get()) {
    return false;
  }
  return GetBrowser()->GetHost()->SetFocusByPosition(x, y);
}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_PIP)
void NWebDelegate::SetPipNativeWindow(int delegate_id,
                                      int child_id,
                                      int frame_routing_id,
                                      void* window) {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "PIC SetPipNativeWindow can not get browser";
    return;
  }
  GetBrowser()->GetHost()->SetPipNativeWindow(delegate_id, child_id,
                                              frame_routing_id, window);
}

void NWebDelegate::SendPipEvent(int delegate_id,
                                int child_id,
                                int frame_routing_id,
                                int event) {
  if (GetBrowser() == nullptr || GetBrowser()->GetHost() == nullptr) {
    LOG(ERROR) << "PIC SendPipEvent can not get browser";
    return;
  }
  GetBrowser()->GetHost()->SendPipEvent(delegate_id, child_id,
                                        frame_routing_id, event);
}
#endif

#if BUILDFLAG(ARKWEB_VIEWPORT_AVOID)
void NWebDelegate::AvoidVisibleViewportBottom(int32_t avoidHeight) {
  LOG(INFO) << "NWebDelegate::AvoidVisibleViewportBottom: " << avoidHeight << " viewportHeight: " << height_;
  if (avoidHeight < 0) {
    avoidHeight = 0;
  }
  avoid_height_ = avoidHeight;
  if (render_handler_ == nullptr) {
    LOG(ERROR) << "fail to AvoidVisibleViewportBottom, render handler is nullptr";
    return;
  }
  float ratio = render_handler_->GetVirtualPixelRatio();
  uint32_t heightChange = static_cast<uint32_t>(std::floor(avoidHeight * ratio));
  render_handler_->SetViewportAvoidHeight(heightChange);
  if (heightChange > height_) {
    heightChange = height_;
  }
  if (avoidHeight == 0) {
    ResizeVisibleViewport(0, 0, false);
  } else {
    // when avoidHeight is greater than 0, it needs to scroll.
    ResizeVisibleViewport(width_, height_ - heightChange, true);
  }
}

int32_t NWebDelegate::GetVisibleViewportAvoidHeight() {
  return avoid_height_;
}
#endif

#if BUILDFLAG(ARKWEB_ERROR_PAGE)
void NWebDelegate::SetErrorPageEnabled(bool enable) {
  if (!preference_delegate_) {
    LOG(ERROR)
      << "SetErrorPageEnabled failed, no preference_delegate_"
      << ", nweb_id_[" << nweb_id_ << "]";
    return;
  }
  preference_delegate_->PutErrorPageEnabled(enable);
}

bool NWebDelegate::GetErrorPageEnabled() {
  if (!preference_delegate_) {
    LOG(ERROR)
      << "GetErrorPageEnabled failed, no preference_delegate_"
      << ", nweb_id_[" << nweb_id_ << "]";
    return false;
  }
  return preference_delegate_->ErrorPageEnabled();
}
#endif

}  // namespace OHOS::NWeb
