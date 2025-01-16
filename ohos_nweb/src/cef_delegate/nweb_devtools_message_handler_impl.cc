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
  InfoBarCallbackImpl(
      CefRefPtr<CefInfoBarCallback> cef_callback)
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

} // namespace

NWebDevToolsMessageHandlerImpl::NWebDevToolsMessageHandlerImpl(
    std::unique_ptr<NWebDevtoolsMessageHandler> handler)
  : handler_(std::move(handler)) {}

NWebDevToolsMessageHandlerImpl::~NWebDevToolsMessageHandlerImpl() = default;

bool NWebDevToolsMessageHandlerImpl::ShowFileChooser(
    FileDialogMode mode,
    const CefString& title,
    const CefString& default_file_path,
    const std::vector<CefString>& accept_filters,
    bool capture,
    CefRefPtr<CefFileDialogCallback> callback) {
  if (!handler_) {
    LOG(INFO) << "ShowFileChooser failed, handler_ is null";
    return false;
  }

  std::string file_selector_title = title.ToString();
  NWebFileSelectorParams::FileSelectorMode file_mode =
      ConvertFileSelectorMode(mode);
  if (file_selector_title.empty()) {
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
          file_mode, file_selector_title, accept_filters,
          default_file_path.ToString(), capture);
  std::shared_ptr<NWebStringVectorValueCallback> file_path_callback =
      std::make_shared<FileSelectorCallbackImpl>(callback);
  if (!CheckValid(handler_.get(), &handler_->show_file_chooser)) {
    LOG(ERROR) << "ShowFileChooser failed, method is invalid";
    return false;
  }
  return (handler_.get()->*(handler_->show_file_chooser))(
      param, file_path_callback);
}

void NWebDevToolsMessageHandlerImpl::ShowInfoBar(
    const CefString& cef_message,
    const CefString& cef_path,
    CefRefPtr<CefInfoBarCallback> cef_callback) {
  if (!handler_) {
    LOG(INFO) << "ShowInfoBar failed, handler_ is null";
    return;
  }
  std::shared_ptr<NWebBoolValueCallback> callback =
    std::make_shared<InfoBarCallbackImpl>(cef_callback);
  std::string message = cef_message.ToString();
  std::string path = cef_path.ToString();

  if (!CheckValid(handler_.get(), &handler_->show_info_bar)) {
    LOG(ERROR) << "ShowInfoBar failed, method is invalid";
    return ;
  }
  return (handler_.get()->*(handler_->show_info_bar))(message, path, callback);
}

bool NWebDevToolsMessageHandlerImpl::BringToFront() {
  if (!handler_) {
    LOG(INFO) << "BringToFront failed, handler_ is null";
    return false;
  }
  if (!CheckValid(handler_.get(), &handler_->bring_to_front)) {
    LOG(ERROR) << "BringToFront failed, method is invalid";
    return false;
  }
  return (handler_.get()->*(handler_->bring_to_front))();
}

bool NWebDevToolsMessageHandlerImpl::CloseWindow() {
  if (!handler_) {
    LOG(INFO) << "CloseWindow failed, handler_ is null";
    return false;
  }
  if (!CheckValid(handler_.get(), &handler_->close_window)) {
    LOG(ERROR) << "CloseWindow failed, method is invalid";
    return false;
  }
  return (handler_.get()->*(handler_->close_window))();
}

bool NWebDevToolsMessageHandlerImpl::ActiveDevToolsWindow() {
  if (!handler_) {
    LOG(INFO) << "ActiveDevToolsWindow failed, handler_ is null";
    return false;
  }

  if (!CheckValid(handler_.get(), &handler_->active_devtools_window)) {
    LOG(ERROR) << "ActiveDevToolsWindow failed, method is invalid";
    return false;
  }
  return (handler_.get()->*(handler_->active_devtools_window))();
}

} // namespace
