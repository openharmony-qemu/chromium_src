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

#include "ohos_nweb/src/cef_delegate/nweb_devtools_message_handler_impl.h"

#include "cef/include/base/cef_logging.h"
#include "ohos_nweb/include/nweb_file_selector_params.h"
#include "ohos_nweb/src/capi/common/common.h"
#include "ohos_nweb/src/capi/nweb_devtools_message_handler.h"
#include "ohos_nweb/src/cef_delegate/nweb_file_selector_params_impl.h"
#include "securec.h"

extern "C" {
void* __real_malloc(size_t);
}  // extern "C"

namespace OHOS::NWeb {

namespace {

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

class InfoBarCallbackImpl : public NWebBoolValueCallback {
 public:
  InfoBarCallbackImpl(CefRefPtr<CefInfoBarCallback> cef_callback)
      : callback_(cef_callback) {}
  ~InfoBarCallbackImpl() override {
    if (callback_) {
      callback_->Allow(default_value_);
    }
  }

  void OnReceiveValue(bool value) override {
    if (callback_) {
      callback_->Allow(value);
    }
  }

 private:
  bool default_value_ = true;
  CefRefPtr<CefInfoBarCallback> callback_;
};

class InfoBarCallbackImplNativeApi : public NWebBoolValueCallbackNativeApi {
 public:
  InfoBarCallbackImplNativeApi(CefRefPtr<CefInfoBarCallback> cef_callback)
      : callback_(cef_callback) {}
  ~InfoBarCallbackImplNativeApi() override {
    if (callback_) {
      callback_->Allow(default_value_);
    }
  }

  void OnReceiveValue(bool value) override {
    if (callback_) {
      callback_->Allow(value);
    }
  }

 private:
  bool default_value_ = true;
  CefRefPtr<CefInfoBarCallback> callback_;
};

}  // namespace

NWebDevToolsMessageHandlerImpl::NWebDevToolsMessageHandlerImpl(
    std::unique_ptr<NWebDevtoolsMessageHandler> handler)
    : handler_(std::move(handler)) {}

NWebDevToolsMessageHandlerImpl::NWebDevToolsMessageHandlerImpl(
    raw_ptr<NWebDevtoolsMessageHandler> handler)
    : handlerNativeApi_(handler) {}

NO_SANITIZE("cfi")
NWebDevToolsMessageHandlerImpl::~NWebDevToolsMessageHandlerImpl() {
  auto* handler = handler_.release();
  if (handler) {
    delete handler;
  }
}

const char** ConvertToCharPtrArray(const std::vector<CefString>& filters) {
  const char** result = new const char *[filters.size() + 1];
  for (size_t i = 0; i < filters.size(); ++i) {
    std::string str = filters[i].ToString();
    char* tmp = new char[str.size() + 1];
    if (strcpy_s(tmp, str.size() + 1, str.c_str()) != EOK) {
      LOG(ERROR) << "strcpy_s failed when ConvertToCharPtrArray";
    }
    result[i] = tmp;
  }
  result[filters.size()] = nullptr;
  return result;
}

void NWebDevToolsMessageHandlerImpl::GetFileSelectorTitle(std::string& title,
                                                          uint32_t mode) {
  if (title.empty()) {
    switch (mode) {
      case NWebFileSelectorParams::FileSelectorMode::FILE_OPEN_MODE:
        title = "open file";
        break;
      case NWebFileSelectorParams::FileSelectorMode::FILE_OPEN_MULTIPLE_MODE:
        title = "open files";
        break;
      case NWebFileSelectorParams::FileSelectorMode::FILE_OPEN_FOLDER_MODE:
        title = "open file folder";
        break;
      case NWebFileSelectorParams::FileSelectorMode::FILE_SAVE_MODE:
        title = "save as";
        break;
      default:
        break;
    }
  }
}

bool NWebDevToolsMessageHandlerImpl::ShowFileChooser(
    FileDialogMode mode,
    const CefString& title,
    const CefString& default_file_path,
    const std::vector<CefString>& accept_filters,
    bool capture,
    CefRefPtr<CefFileDialogCallback> callback) {
  std::string file_selector_title = title.ToString();
  NWebFileSelectorParams::FileSelectorMode file_mode =
      ConvertFileSelectorMode(mode);
  GetFileSelectorTitle(file_selector_title, file_mode);
  if (handler_) {
#if defined(ADDRESS_SANITIZER) || defined(HWADDRESS_SANITIZER)
    FileSelectorParamsImpl* tmp = new FileSelectorParamsImpl(
        file_mode, file_selector_title, accept_filters,
        default_file_path.ToString(), capture);
#else
    void* addr = __real_malloc(sizeof(FileSelectorParamsImpl));
    FileSelectorParamsImpl* tmp = new (addr)
        FileSelectorParamsImpl(file_mode, file_selector_title, accept_filters,
                               default_file_path.ToString(), capture);
#endif
    std::shared_ptr<NWebFileSelectorParams> param(tmp);

    std::shared_ptr<NWebStringVectorValueCallback> file_path_callback =
        std::make_shared<FileSelectorCallbackImpl>(callback);

    if (!CheckValid(handler_.get(), &handler_->show_file_chooser)) {
      LOG(ERROR) << "ShowFileChooser failed, method is invalid";
      return false;
    }
    return (handler_.get()->*(handler_->show_file_chooser))(param,
                                                            file_path_callback);
  }

  if (handlerNativeApi_) {
    std::string default_file = default_file_path.ToString();
    const char** accept_filters_array = ConvertToCharPtrArray(accept_filters);
    std::unique_ptr<NWebFileSelectorParamsNativeApi> param =
        std::make_unique<NWebFileSelectorParamsNativeApi>(
            file_mode, file_selector_title.c_str(), accept_filters_array,
            accept_filters.size(), default_file.c_str(), capture);
    std::unique_ptr<NWebStringVectorValueCallbackNativeApi> file_path_callback =
        std::make_unique<FileSelectorCallbackImplNativeApi>(callback);
    if (!CheckValid(handlerNativeApi_.get(),
                    &handlerNativeApi_->show_file_chooser_native_api)) {
      LOG(ERROR) << "ShowFileChooser failed, method is invalid";
      for (uint32_t i = 0; i < accept_filters.size(); ++i) {
        delete[] accept_filters_array[i];
      }
      delete[] accept_filters_array;
      return false;
    }
    bool result = (handlerNativeApi_.get()
                       ->*(handlerNativeApi_->show_file_chooser_native_api))(
        param.release(), file_path_callback.release());
    for (uint32_t i = 0; i < accept_filters.size(); ++i) {
      delete[] accept_filters_array[i];
    }
    delete[] accept_filters_array;
    return result;
  }

  LOG(INFO) << "ShowFileChooser failed, handler is null";
  return false;
}

void NWebDevToolsMessageHandlerImpl::ShowInfoBar(
    const CefString& cef_message,
    const CefString& cef_path,
    CefRefPtr<CefInfoBarCallback> cef_callback) {
  if (handler_) {
    std::shared_ptr<NWebBoolValueCallback> callback =
        std::make_shared<InfoBarCallbackImpl>(cef_callback);
    std::string message = cef_message.ToString();
    std::string path = cef_path.ToString();

    if (!CheckValid(handler_.get(), &handler_->show_info_bar)) {
      LOG(ERROR) << "ShowInfoBar failed, method is invalid";
      return;
    }
    return (handler_.get()->*(handler_->show_info_bar))(message, path,
                                                        callback);
  }

  if (handlerNativeApi_) {
    std::unique_ptr<NWebBoolValueCallbackNativeApi> callback =
        std::make_unique<InfoBarCallbackImplNativeApi>(cef_callback);
    std::string message = cef_message.ToString().c_str();
    std::string path = cef_path.ToString().c_str();

    if (!CheckValid(handlerNativeApi_.get(),
                    &handlerNativeApi_->show_info_bar_native_api)) {
      LOG(ERROR) << "ShowInfoBar failed, method is invalid";
      return;
    }
    (handlerNativeApi_.get()->*(handlerNativeApi_->show_info_bar_native_api))(
        message.c_str(), path.c_str(), callback.release());
    return;
  }

  LOG(INFO) << "ShowInfoBar failed, handler is null";
  return;
}

bool NWebDevToolsMessageHandlerImpl::BringToFront() {
  if (handler_) {
    if (!CheckValid(handler_.get(), &handler_->bring_to_front)) {
      LOG(ERROR) << "BringToFront failed, method is invalid";
      return false;
    }
    return (handler_.get()->*(handler_->bring_to_front))();
  }
  if (handlerNativeApi_) {
    if (!CheckValid(handlerNativeApi_.get(),
                    &handlerNativeApi_->bring_to_front)) {
      LOG(ERROR) << "BringToFront failed, method is invalid";
      return false;
    }
    return (handlerNativeApi_.get()->*(handlerNativeApi_->bring_to_front))();
  }

  LOG(INFO) << "BringToFront failed, handler is null";
  return false;
}

bool NWebDevToolsMessageHandlerImpl::CloseWindow() {
  if (handler_) {
    if (!CheckValid(handler_.get(), &handler_->close_window)) {
      LOG(ERROR) << "CloseWindow failed, method is invalid";
      return false;
    }
    return (handler_.get()->*(handler_->close_window))();
  }

  if (handlerNativeApi_) {
    if (!CheckValid(handlerNativeApi_.get(),
                    &handlerNativeApi_->close_window)) {
      LOG(ERROR) << "CloseWindow failed, method is invalid";
      return false;
    }
    return (handlerNativeApi_.get()->*(handlerNativeApi_->close_window))();
  }

  LOG(INFO) << "CloseWindow failed, handler is null";
  return false;
}

bool NWebDevToolsMessageHandlerImpl::ActiveDevToolsWindow() {
  if (handler_) {
    if (!CheckValid(handler_.get(), &handler_->active_devtools_window)) {
      LOG(ERROR) << "ActiveDevToolsWindow failed, method is invalid";
      return false;
    }
    return (handler_.get()->*(handler_->active_devtools_window))();
  }

  if (handlerNativeApi_) {
    if (!CheckValid(handlerNativeApi_.get(),
                    &handlerNativeApi_->active_devtools_window)) {
      LOG(ERROR) << "ActiveDevToolsWindow failed, method is invalid";
      return false;
    }
    return (handlerNativeApi_.get()
                ->*(handlerNativeApi_->active_devtools_window))();
  }

  LOG(INFO) << "ActiveDevToolsWindow failed, handler is null";
  return false;
}

bool NWebDevToolsMessageHandlerImpl::SetInspectedPageBounds(int left,
                                                            int top,
                                                            int width,
                                                            int height) {
  LOG(DEBUG) << "NWebDevToolsMessageHandlerImpl::SetInspectedPageBounds"
            << ", left: " << left
            << ", top: " << top
            << ", width: " << width
            << ", height: " << height;
  if (handler_) {
    if (!CheckValid(handler_.get(), &handler_->set_inspected_page_bounds)) {
      LOG(ERROR) << "SetInspectedPageBounds failed, method is invalid";
      return false;
    }
    return (handler_.get()->*(handler_->set_inspected_page_bounds))(left, top, width, height);
  }
 
  if (handlerNativeApi_) {
    if (!CheckValid(handlerNativeApi_.get(),
                    &handlerNativeApi_->set_inspected_page_bounds)) {
      LOG(ERROR) << "SetInspectedPageBounds failed, method is invalid";
      return false;
    }
    return (handlerNativeApi_.get()
                ->*(handlerNativeApi_->set_inspected_page_bounds))(left, top, width, height);
  }
 
  LOG(INFO) << "SetInspectedPageBounds failed, handler is null";
  return false;
}
 
bool NWebDevToolsMessageHandlerImpl::SetDockMode(int mode) {
  LOG(DEBUG) << "NWebDevToolsMessageHandlerImpl::SetDockMode mode: " << mode;
  if (handler_) {
    if (!CheckValid(handler_.get(), &handler_->set_dock_mode)) {
      LOG(ERROR) << "SetDockMode failed, method is invalid";
      return false;
    }
    return (handler_.get()->*(handler_->set_dock_mode))(mode);
  }
 
  if (handlerNativeApi_) {
    if (!CheckValid(handlerNativeApi_.get(),
                    &handlerNativeApi_->set_dock_mode)) {
      LOG(ERROR) << "SetDockMode failed, method is invalid";
      return false;
    }
    return (handlerNativeApi_.get()
                ->*(handlerNativeApi_->set_dock_mode))(mode);
  }
 
  LOG(INFO) << "SetDockMode failed, handler is null";
  return false;
}

}  // namespace OHOS::NWeb
