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

#ifndef CEF_LIBCEF_BROWSER_DEVTOOLS_ARKWEB_DEVTOOLS_MESSAGE_HANDLER_H_
#define CEF_LIBCEF_BROWSER_DEVTOOLS_ARKWEB_DEVTOOLS_MESSAGE_HANDLER_H_

#include "base/values.h"
#include "base/functional/bind.h"

#include "chrome/browser/devtools/devtools_settings.h"
#include "libcef/browser/file_dialog_manager.h"
#include "include/cef_devtools_message_handler_delegate.h"

namespace blink::mojom {
class FileChooserParams;
}

class CefDevToolsMessageHandler final {
 public:
  explicit CefDevToolsMessageHandler(
      CefRefPtr<CefDevToolsMessageHandlerDelegate> delegate, Profile* profile);
  CefDevToolsMessageHandler(const CefDevToolsMessageHandler&) = delete;
  CefDevToolsMessageHandler(CefDevToolsMessageHandler&&) = delete;
  ~CefDevToolsMessageHandler();

  CefFileDialogManager::RunFileChooserCallback ShowFileChooser(
      const blink::mojom::FileChooserParams& params,
      CefFileDialogManager::RunFileChooserCallback callback);

  using InfoBarCallback = base::OnceCallback<void(bool)>;
  void ShowInfoBar(
      const std::string& message,
      const std::string& path,
      InfoBarCallback callback);

  struct Result final {
    bool handled = false;
    base::Value result;
  };
  Result HandleMessage(int request_id, const std::string& method,
      const base::Value::List& params);

 private:
  Result HandleProtocolMessage(const base::Value::List&);
  Result BringToFront(const base::Value::List&);
  Result CloseWindow(const base::Value::List&);
  Result InspectedURLChanged(const base::Value::List&);
  Result PageBringToFront(const base::Value::List&);

  Result RegisterPreference(const base::Value::List& params);
  Result GetPreferences(const base::Value::List& params);
  Result GetPreference(const base::Value::List& params);
  Result SetPreference(const base::Value::List& params);
  Result RemovePreference(const base::Value::List& params);
  Result ClearPreferences(const base::Value::List& params);

  CefRefPtr<CefDevToolsMessageHandlerDelegate> delegate_;

  using Handler = base::RepeatingCallback<Result(const base::Value::List&)>;
  std::map<std::string, Handler> method_handlers_;
  std::map<std::string, Handler> protocol_message_handlers_;

  DevToolsSettings settings_;
};
#endif // CEF_LIBCEF_BROWSER_DEVTOOLS_ARKWEB_DEVTOOLS_MESSAGE_HANDLER_H_
