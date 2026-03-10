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

#ifndef OHOS_NWEB_SRC_CAPI_NWEB_DEVTOOLS_MESSAGE_HANDLER_H_
#define OHOS_NWEB_SRC_CAPI_NWEB_DEVTOOLS_MESSAGE_HANDLER_H_

#include <memory>
#include <string>

namespace OHOS::NWeb {
class NWebBoolValueCallback;
class NWebFileSelectorParams;
class NWebStringVectorValueCallback;
}  // namespace OHOS::NWeb

struct NWebFileSelectorParamsNativeApi {
 public:
  int mode;
  const char* title;
  const char** accept_file_types;
  uint32_t accept_file_types_size;
  const char* default_file_name;
  bool is_capture;

 public:
  NWebFileSelectorParamsNativeApi(int mode_in,
                                  const char* title_in,
                                  const char** accept_file_types_in,
                                  uint32_t accept_file_types_size_in,
                                  const char* default_file_name_in,
                                  bool isCapture_in)
      : mode(mode_in),
        title(title_in),
        accept_file_types(accept_file_types_in),
        accept_file_types_size(accept_file_types_size_in),
        default_file_name(default_file_name_in),
        is_capture(isCapture_in) {}
};

struct NWebStringVectorValueCallbackNativeApi {
 public:
  virtual ~NWebStringVectorValueCallbackNativeApi() = default;
  virtual void OnReceiveValue(const char** value) { return; }
};

struct NWebBoolValueCallbackNativeApi {
 public:
  virtual ~NWebBoolValueCallbackNativeApi() = default;
  virtual void OnReceiveValue(bool value) { return; }
};

template <typename T>
struct NWebDevtoolsMessageHandlerBase {
  const size_t struct_size = sizeof(NWebDevtoolsMessageHandlerBase);
  bool (T::*show_file_chooser)(
      std::shared_ptr<OHOS::NWeb::NWebFileSelectorParams> param,
      std::shared_ptr<OHOS::NWeb::NWebStringVectorValueCallback>
          file_path_callback) = nullptr;
  void (T::*show_info_bar)(
      const std::string& message,
      const std::string& path,
      std::shared_ptr<OHOS::NWeb::NWebBoolValueCallback> callback) = nullptr;
  bool (T::*bring_to_front)() = nullptr;
  bool (T::*close_window)() = nullptr;
  bool (T::*active_devtools_window)() = nullptr;
  bool (T::*show_file_chooser_native_api)(
      NWebFileSelectorParamsNativeApi* param,
      NWebStringVectorValueCallbackNativeApi* file_path_callback) = nullptr;
  void (T::*show_info_bar_native_api)(
      const char* message,
      const char* path,
      NWebBoolValueCallbackNativeApi* callback) = nullptr;
  bool (T::*set_inspected_page_bounds)(int left,
                                       int top,
                                       int width,
                                       int height) = nullptr;
  bool (T::*set_dock_mode)(int mode) = nullptr;

  NWebDevtoolsMessageHandlerBase() = default;
  NWebDevtoolsMessageHandlerBase(const NWebDevtoolsMessageHandlerBase& other) =
      delete;
  NWebDevtoolsMessageHandlerBase(NWebDevtoolsMessageHandlerBase&& other) =
      delete;
  NWebDevtoolsMessageHandlerBase& operator=(
      const NWebDevtoolsMessageHandlerBase& other) = delete;
  NWebDevtoolsMessageHandlerBase& operator=(
      NWebDevtoolsMessageHandlerBase&& other) = delete;
};

struct NWebDevtoolsMessageHandler
    : public NWebDevtoolsMessageHandlerBase<NWebDevtoolsMessageHandler> {
 public:
  using Base = NWebDevtoolsMessageHandlerBase<NWebDevtoolsMessageHandler>;
  NWebDevtoolsMessageHandler()
      : NWebDevtoolsMessageHandlerBase<NWebDevtoolsMessageHandler>() {
    this->show_file_chooser = &NWebDevtoolsMessageHandler::ShowFileChooser;
    this->show_info_bar = &NWebDevtoolsMessageHandler::ShowInfoBar;
    this->bring_to_front = &NWebDevtoolsMessageHandler::BringToFront;
    this->close_window = &NWebDevtoolsMessageHandler::CloseWindow;
    this->active_devtools_window =
        &NWebDevtoolsMessageHandler::ActiveDevToolsWindow;
    this->show_file_chooser_native_api =
        &NWebDevtoolsMessageHandler::ShowFileChooser;
    this->show_info_bar_native_api = &NWebDevtoolsMessageHandler::ShowInfoBar;
    this->set_inspected_page_bounds =
        &NWebDevtoolsMessageHandler::SetInspectedPageBounds;
    this->set_dock_mode = &NWebDevtoolsMessageHandler::SetDockMode;
  }
  virtual ~NWebDevtoolsMessageHandler() = default;

 protected:
  virtual bool ShowFileChooser(
      std::shared_ptr<OHOS::NWeb::NWebFileSelectorParams> param,
      std::shared_ptr<OHOS::NWeb::NWebStringVectorValueCallback>
          file_path_callback) {
    return false;
  }
  virtual void ShowInfoBar(
      const std::string& message,
      const std::string& path,
      std::shared_ptr<OHOS::NWeb::NWebBoolValueCallback> callback) {}
  virtual bool BringToFront() { return false; }
  virtual bool CloseWindow() { return false; }
  virtual bool ActiveDevToolsWindow() { return false; }
  virtual bool ShowFileChooser(
      NWebFileSelectorParamsNativeApi* param,
      NWebStringVectorValueCallbackNativeApi* file_path_callback) {
    return false;
  }
  virtual void ShowInfoBar(const char* message,
                           const char* path,
                           NWebBoolValueCallbackNativeApi* callback) {}
  virtual bool SetInspectedPageBounds(int left,
                                      int top,
                                      int width,
                                      int height) { return false; }
  virtual bool SetDockMode(int mode) { return false; }

 private:
  static_assert(offsetof(Base, struct_size) == 0,
                "Must NOT break the order of Base members.");
  static_assert((offsetof(Base, show_file_chooser) -
                 offsetof(Base, struct_size)) == sizeof(Base::struct_size),
                "Must NOT break the order of Base members.");
  static_assert((offsetof(Base, show_info_bar) -
                 offsetof(Base, show_file_chooser)) ==
                    sizeof(Base::show_file_chooser),
                "Must NOT break the order of Base members.");
  static_assert((offsetof(Base, bring_to_front) -
                 offsetof(Base, show_info_bar)) == sizeof(Base::show_info_bar),
                "Must NOT break the order of Base members.");
  static_assert((offsetof(Base, close_window) -
                 offsetof(Base, bring_to_front)) ==
                    sizeof(Base::bring_to_front),
                "Must NOT break the order of Base members.");
  static_assert((offsetof(Base, active_devtools_window) -
                 offsetof(Base, close_window)) == sizeof(Base::close_window),
                "Must NOT break the order of Base members.");
  static_assert((offsetof(Base, show_file_chooser_native_api) -
                 offsetof(Base, active_devtools_window)) ==
                    sizeof(Base::active_devtools_window),
                "Must NOT break the order of Base members.");
  static_assert((offsetof(Base, show_info_bar_native_api) -
                 offsetof(Base, show_file_chooser_native_api)) ==
                    sizeof(Base::show_file_chooser_native_api),
                "Must NOT break the order of Base members.");
  static_assert((offsetof(Base, set_inspected_page_bounds) -
                 offsetof(Base, show_info_bar_native_api)) ==
                    sizeof(Base::show_info_bar_native_api),
                "Must NOT break the order of Base members.");
  static_assert((offsetof(Base, set_dock_mode) -
                 offsetof(Base, set_inspected_page_bounds)) ==
                    sizeof(Base::set_inspected_page_bounds),
                "Must NOT break the order of Base members.");
};

struct InspectPoint {
  int x = 0;
  int y = 0;
};

struct OpenDevToolsParam final {
  int nweb_id = -1;
  std::unique_ptr<NWebDevtoolsMessageHandler> handler;
  InspectPoint point;
  NWebDevtoolsMessageHandler* handlerNativeApi = nullptr;
};

struct OpenDevToolsExtOpt final {
    bool canDock = false;
    bool showToolbox = false;
};

struct RequestOpenDevToolsParams final {
    std::string type;
    std::string source_id;
    std::string target_id;
    std::string extension_id;
};

#endif  // OHOS_NWEB_SRC_CAPI_NWEB_DEVTOOLS_MESSAGE_HANDLER_H_
