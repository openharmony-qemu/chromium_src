/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef OHOS_ADAPTER_BROWSER_BROWSER_ADAPTER_H_
#define OHOS_ADAPTER_BROWSER_BROWSER_ADAPTER_H_

#include <functional>
#include <mutex>
#include <string>
 
#include "aki/jsbind.h"
#include "ohos/adapter/export.h"

namespace ohos::adapter {

enum class CommandType {
  kNewWindow,
  kNewWebContent,
  kNewWidget,
  kGetWidget,
  kGetNextWidgetId,
  kGetLastActiveWidget,
  kOpenURL,
  kAppExit
};

enum class BrowserCloseResponse {
  kUndetermined,
  kClosingContinue,
  kClosingInterrupt,
  kClosed,
  kCloseCancelled,
  kClosedAnyway,
};
 
class ADAPTER_EXPORT_API CommandResult {
public:
  std::function<void()> async_callback = [](){};
  // Returns the result of command execution
  // 0 if success or errcode.
  int32_t ret_code = -1;
  // Returns the widget id.
  int32_t widget_Id = 0;
  // Returns the last widget id.
  int32_t last_widget_Id = 0;
};

class ADAPTER_EXPORT_API CommandParameter {
public:
  // Tells desired Operator type
  CommandType type = CommandType::kNewWindow;
  // Tells the browser load Specify urls.
  std::string url;
  // Tells the browser whether to create new window.
  bool force_open;
  // user custom data.
  std::string user_data;
  // Specifies that it is a synchronous call
  bool is_sync = false;
  // Tells the browser load is webapp
  bool is_webapp = false;

  std::string ToString() const {
    std::stringstream stream;
    stream << "type = " << (int)type << ", url_len = " << url.length()
           << ", force_open = " << force_open << ", is_sync = " << is_sync;
    return stream.str();
  }
};
 
using BrowserCallback = std::function<void(CommandParameter&, std::shared_ptr<CommandResult> result)>;

class ADAPTER_EXPORT_API BrowserAdapter {
 public:
  static BrowserAdapter& GetInstance();

  void RegisterBrowserCallback(BrowserCallback callback);
  BrowserCallback GetBrowserCallback() { return callback_; }
 
  CommandResult ExecuteCommand(const int opt_type,
                               const aki::Value opt_value);

  BrowserCloseResponse GetBrowserCloseResponse(int32_t id);
  BrowserCloseResponse GetAppCloseResponse();
  void SetBrowserCloseResponse(int32_t id, BrowserCloseResponse response);
  void SetAppCloseResponse(BrowserCloseResponse response);
  void ResetCloseResponse();

 private:
  BrowserAdapter() = default;
  ~BrowserAdapter() = default;
 
  BrowserCallback callback_ = nullptr;
  std::mutex mutex_;

  std::unordered_map<int32_t, BrowserCloseResponse> close_responses_;
  BrowserCloseResponse app_close_response_;
};
}

#endif // OHOS_ADAPTER_BROWSER_BROWSER_ADAPTER_H_
