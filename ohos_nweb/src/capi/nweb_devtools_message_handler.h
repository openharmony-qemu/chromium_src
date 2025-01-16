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

namespace OHOS::NWeb {
class NWebBoolValueCallback;
class NWebFileSelectorParams;
class NWebStringVectorValueCallback;
}

class NWebDevtoolsMessageHandler;

struct NWebDevtoolsMessageHandlerBase {
  const size_t struct_size = sizeof(NWebDevtoolsMessageHandlerBase);
  bool (NWebDevtoolsMessageHandler::*show_file_chooser)(
      std::shared_ptr<OHOS::NWeb::NWebFileSelectorParams> param,
      std::shared_ptr<OHOS::NWeb::NWebStringVectorValueCallback>
          file_path_callback) = nullptr;
  void (NWebDevtoolsMessageHandler::*show_info_bar)(
      const std::string& message,
      const std::string& path,
      std::shared_ptr<OHOS::NWeb::NWebBoolValueCallback> callback) = nullptr;
  bool (NWebDevtoolsMessageHandler::*bring_to_front)() = nullptr;
  bool (NWebDevtoolsMessageHandler::*close_window)() = nullptr;
  bool (NWebDevtoolsMessageHandler::*active_devtools_window)() = nullptr;

  NWebDevtoolsMessageHandlerBase() = default;
  NWebDevtoolsMessageHandlerBase(
      const NWebDevtoolsMessageHandlerBase& other) = delete;
  NWebDevtoolsMessageHandlerBase(
      NWebDevtoolsMessageHandlerBase&& other) = delete;
  NWebDevtoolsMessageHandlerBase& operator=(
      const NWebDevtoolsMessageHandlerBase& other) = delete;
  NWebDevtoolsMessageHandlerBase& operator=(
      NWebDevtoolsMessageHandlerBase&& other) = delete;
};

class NWebDevtoolsMessageHandler : public NWebDevtoolsMessageHandlerBase {
 public:
  using Base = NWebDevtoolsMessageHandlerBase;
  NWebDevtoolsMessageHandler() : NWebDevtoolsMessageHandlerBase() {
    this->show_file_chooser =
        &NWebDevtoolsMessageHandler::ShowFileChooser;
    this->show_info_bar =
        &NWebDevtoolsMessageHandler::ShowInfoBar;
    this->bring_to_front =
        &NWebDevtoolsMessageHandler::BringToFront;
    this->close_window =
        &NWebDevtoolsMessageHandler::CloseWindow;
    this->active_devtools_window =
        &NWebDevtoolsMessageHandler::ActiveDevToolsWindow;
  }
  virtual ~NWebDevtoolsMessageHandler() = default;

 protected:
  virtual bool ShowFileChooser(
      std::shared_ptr<OHOS::NWeb::NWebFileSelectorParams> param,
      std::shared_ptr<OHOS::NWeb::NWebStringVectorValueCallback>
          file_path_callback) { return false; }
  virtual void ShowInfoBar(
      const std::string& message,
      const std::string& path,
      std::shared_ptr<OHOS::NWeb::NWebBoolValueCallback> callback) {}
  virtual bool BringToFront() { return false; }
  virtual bool CloseWindow() { return false; }
  virtual bool ActiveDevToolsWindow() { return false; }

 private:
  static_assert(offsetof(Base, struct_size) == 0,
      "Must NOT break the order of Base members.");
  static_assert(
      (offsetof(Base, show_file_chooser) - offsetof(Base, struct_size))
          == sizeof(Base::struct_size),
      "Must NOT break the order of Base members.");
  static_assert(
      (offsetof(Base, show_info_bar) - offsetof(Base, show_file_chooser))
          == sizeof(Base::show_file_chooser),
      "Must NOT break the order of Base members.");
  static_assert(
      (offsetof(Base, bring_to_front) - offsetof(Base, show_info_bar))
          == sizeof(Base::show_info_bar),
      "Must NOT break the order of Base members.");
  static_assert(
      (offsetof(Base, close_window) - offsetof(Base, bring_to_front))
          == sizeof(Base::bring_to_front),
      "Must NOT break the order of Base members.");
  static_assert(
      (offsetof(Base, active_devtools_window) - offsetof(Base, close_window))
          == sizeof(Base::close_window),
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
};

#endif  // OHOS_NWEB_SRC_CAPI_NWEB_DEVTOOLS_MESSAGE_HANDLER_H_
