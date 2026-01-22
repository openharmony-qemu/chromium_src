/*
 * Copyright (c) 2023-2026 Huawei Device Co., Ltd.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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

#include "ohos/adapter/native_messaging/native_messaging_adapter.h"

#include <future>
#include <string>
#include <vector>

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/device_info/device_info.h"

namespace ohos::adapter::nativemessaging {

const int kInvalidPid = -1;
const int kWaitTime = 3;

NativeMessagingAdapter& NativeMessagingAdapter::GetInstance() {
  static NativeMessagingAdapter instance;
  return instance;
}

int NativeMessagingAdapter::ConnectNative(const std::vector<std::string>& commands,
                                          const std::vector<std::pair<int, int>>& fds) {
  if (device_info::DeviceInfo::SdkApi() < device_info::SDK_VERSION_21) {
    LOGE("[NativeMessaging] ConnectNative unsupported SDK version: %{public}d. "
         "Minimum required: %{public}d",
         device_info::DeviceInfo::SdkApi(),
         device_info::SDK_VERSION_21);
    return kInvalidPid;
  }
  auto connect_native_func = ohos::adapter::GetJSFunction(
      "NativeMessagingAdapter.ConnectNative");
  if (!connect_native_func) {
    LOGE("NativeMessagingAdapter.ConnectNative not defined");
    return kInvalidPid;
  }
  std::vector<int> share_fds;
  for (auto& [rawfd, remap_fd] : fds) {
    LOGD("HandleRequest fd %{public}d => %{public}d", rawfd, remap_fd);
    share_fds.push_back(rawfd);
  }
  std::function<void(StartCallbackFunc)> start_func =
    [connect_native_func, commands, share_fds](StartCallbackFunc callback) {
      connect_native_func->Invoke<void>(commands, share_fds, callback);
    };

  std::promise<int32_t> connect_pid;
  StartCallbackFunc set_connect_pid = [&connect_pid](int32_t pid) -> void {
    connect_pid.set_value(pid);
  };
  start_func(set_connect_pid);

  auto future = connect_pid.get_future();
  auto status = future.wait_for(std::chrono::seconds(kWaitTime));
  if (status == std::future_status::timeout) {
    LOGE("NativeMessagingAdapter.ConnectNative Wait timeout");
    return kInvalidPid;
  }
  return future.get();
}

void NativeMessagingAdapter::DisconnectNative(int pid) {
  if (device_info::DeviceInfo::SdkApi() < device_info::SDK_VERSION_21) {
    LOGE("[NativeMessaging] DisconnectNative unsupported SDK version: %{public}d. "
         "Minimum required: %{public}d",
         device_info::DeviceInfo::SdkApi(),
         device_info::SDK_VERSION_21);
    return;
  }
  if (auto disconnect_native_func =
        ohos::adapter::GetJSFunction("NativeMessagingAdapter.DisconnectNative")) {
    disconnect_native_func->Invoke<void>(pid);
  }
}

std::string NativeMessagingAdapter::GetManifestConfig(const std::string& name) {
  if (device_info::DeviceInfo::SdkApi() < device_info::SDK_VERSION_20) {
    LOGE("[NativeMessaging] ShareConfig unsupported SDK version: %{public}d. "
         "Minimum required: %{public}d",
         device_info::DeviceInfo::SdkApi(),
         device_info::SDK_VERSION_20);
    return "";
  }
  if (auto manifest_config_func =
        ohos::adapter::GetJSFunction("NativeMessagingAdapter.GetManifestConfig")) {
    std::promise<std::string> ret;
    ConfigCallbackFunc set_ret = [&ret](std::string data) -> void {
      ret.set_value(data);
    };
    std::function<void(ConfigCallbackFunc)> config_func =
      [manifest_config_func, name](ConfigCallbackFunc callback) {
        manifest_config_func->Invoke<void>(name, callback);
      };
    config_func(set_ret);

    auto future = ret.get_future();
    auto status = future.wait_for(std::chrono::seconds(kWaitTime));
    if (status == std::future_status::timeout) {
      LOGE("NativeMessagingAdapter.GetManifestConfig Wait timeout");
      return "";
    }
    return future.get();
  }
  return "";
}

}  // namespace ohos::adapter::nativemessaging