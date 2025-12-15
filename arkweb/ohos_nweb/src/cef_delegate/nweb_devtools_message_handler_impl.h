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

#ifndef OHOS_NWEB_SRC_CEF_DELEGATE_NWEB_DEVTOOLS_MESSAGE_HANDLER_IMPL_H_
#define OHOS_NWEB_SRC_CEF_DELEGATE_NWEB_DEVTOOLS_MESSAGE_HANDLER_IMPL_H_

#include <memory>

#include "cef/include/cef_devtools_message_handler_delegate.h"

struct NWebDevtoolsMessageHandler;

namespace OHOS::NWeb {

class NWebDevToolsMessageHandlerImpl
    : public virtual CefDevToolsMessageHandlerDelegate {
 public:
  NWebDevToolsMessageHandlerImpl(
      std::unique_ptr<NWebDevtoolsMessageHandler> handler);
  NWebDevToolsMessageHandlerImpl(raw_ptr<NWebDevtoolsMessageHandler> handler);
  virtual ~NWebDevToolsMessageHandlerImpl();

  bool ShowFileChooser(FileDialogMode mode,
                       const CefString& title,
                       const CefString& default_file_path,
                       const std::vector<CefString>& accept_filters,
                       bool capture,
                       CefRefPtr<CefFileDialogCallback> callback) override;
  void ShowInfoBar(const CefString& message,
                   const CefString& cef_path,
                   CefRefPtr<CefInfoBarCallback> callback) override;
  bool BringToFront() override;
  bool CloseWindow() override;
  bool ActiveDevToolsWindow() override;
  void SetInspectedPageBounds(int left,
                              int top,
                              int width,
                              int height) override;
  void SetDockMode(int mode) override;

 private:
  void GetFileSelectorTitle(std::string& title, uint32_t mode);

 private:
  std::unique_ptr<NWebDevtoolsMessageHandler> handler_;
  raw_ptr<NWebDevtoolsMessageHandler> handlerNativeApi_;

  IMPLEMENT_REFCOUNTING(NWebDevToolsMessageHandlerImpl);
};

}  // namespace OHOS::NWeb

#endif  // OHOS_NWEB_SRC_CEF_DELEGATE_NWEB_DEVTOOLS_MESSAGE_HANDLER_IMPL_H_