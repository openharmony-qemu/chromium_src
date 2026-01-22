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

#include "ohos/adapter/browser/browser_adapter.h"

#include <js_native_api.h>
#include <js_native_api_types.h>

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"

namespace ohos::adapter {

BrowserAdapter& BrowserAdapter::GetInstance() {
  static BrowserAdapter helper;
  return helper;
}

void BrowserAdapter::RegisterBrowserCallback(BrowserCallback callback) {
  if (callback != nullptr) {
    callback_ = callback;
  }
}

CommandResult BrowserAdapter::ExecuteCommand(const int opt_type,
                                             const aki::Value opt_value) {
  std::lock_guard<std::mutex> lock(mutex_);

  CommandParameter param;
  param.type = static_cast<CommandType>(opt_type);
  param.url = opt_value["url"].As<std::string>();
  param.user_data = opt_value["user_data"].As<std::string>();
  param.is_sync = opt_value["is_sync"].As<bool>();
  param.is_webapp = opt_value["is_webapp"].As<bool>();
  LOGI("[ohoswindow] BrowserAdapter::ExecuteCommand param: %{public}s",
       param.ToString().c_str());

  auto result = std::make_shared<CommandResult>();
  if (param.type == CommandType::kGetWidget) {
    result->ret_code = 0;
    result->widget_Id =
        xcomponent::WindowAdapter::GetInstance().NextWindowWidgetId();
    LOGI("[ohoswindow] BrowserAdapter::ExecuteCommand GetWidget %{public}d return.", result->widget_Id);
    return *result;
  }

  if (param.type == CommandType::kGetNextWidgetId) {
    result->ret_code = 0;
    result->widget_Id =
        xcomponent::WindowAdapter::GetInstance().PeekNextWindowWidgetId();
    return *result;
  }

  auto callback = GetBrowserCallback();
  if (!callback) {
    LOGW("[ohoswindow] BrowserAdapter::ExecuteCommand no callback register.");
    return *result;
  }

  // asynchronous call
  if (!param.is_sync) {
    callback(param, result);
    // synchronous call
  } else {
    auto promise = std::make_shared<std::promise<bool>>();
    auto future = promise->get_future();
    result->async_callback = [promise]() { promise->set_value(true); };
    callback(param, result);
    auto status = future.wait_for(std::chrono::seconds(3));
    if (status == std::future_status::timeout) {
      LOGE("[ohoswindow] BrowserAdapter::ExecuteCommand Wait timeout");
      return CommandResult{.ret_code = -1};
    }
    future.get();
  }
  LOGI("[ohoswindow] BrowserAdapter::ExecuteCommand GetWindow return.");
  return *result;
}

CommandResult ExecuteCommand(const int opt_type, const aki::Value opt_value) {
  return BrowserAdapter::GetInstance().ExecuteCommand(opt_type,
                                                      std::move(opt_value));
}

BrowserCloseResponse BrowserAdapter::GetAppCloseResponse() {
  return app_close_response_;
}

BrowserCloseResponse BrowserAdapter::GetBrowserCloseResponse(int32_t id) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (close_responses_.contains(id)) {
    return close_responses_[id];
  } else {
    return BrowserCloseResponse::kUndetermined;
  }
}

BrowserCloseResponse GetBrowserCloseResponse(int32_t id) {
  return BrowserAdapter::GetInstance().GetBrowserCloseResponse(id);
}

BrowserCloseResponse GetAppCloseResponse() {
  return BrowserAdapter::GetInstance().GetAppCloseResponse();
}

void BrowserAdapter::SetBrowserCloseResponse(int32_t id,
                                             BrowserCloseResponse response) {
  std::lock_guard<std::mutex> lock(mutex_);
  close_responses_[id] = response;
}

void BrowserAdapter::SetAppCloseResponse(BrowserCloseResponse response) {
  std::lock_guard<std::mutex> lock(mutex_);
  app_close_response_ = response;
}

void BrowserAdapter::ResetCloseResponse() {
  close_responses_.clear();
  app_close_response_ = BrowserCloseResponse::kUndetermined;
}

JSBIND_CLASS(CommandResult) {
  JSBIND_PROPERTY(ret_code);
  JSBIND_PROPERTY(widget_Id);
  JSBIND_PROPERTY(last_widget_Id);
}

JSBIND_GLOBAL() {
  JSBIND_FUNCTION(ExecuteCommand);
  JSBIND_FUNCTION(GetBrowserCloseResponse);
  JSBIND_FUNCTION(GetAppCloseResponse);
}

}  // namespace ohos::adapter
