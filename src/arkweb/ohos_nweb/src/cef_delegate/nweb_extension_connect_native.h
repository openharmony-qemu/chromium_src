/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef NWEB_EXTENSION_CONNECT_NATIVE_H_
#define NWEB_EXTENSION_CONNECT_NATIVE_H_
 
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include "base/process/process.h"
#include "base/process/launch.h"
#include "base/process/kill.h"

#include "arkweb/ohos_nweb/include/nweb_handler.h"

namespace OHOS::NWeb {
 
struct ConnectNativeInfo {
  int connectionId;
  int extensionPid;
  std::string bundleName;
  std::string extensionOrigin;
  int messageReadPipeId;
  int messageWritePipeId;
};
 
class OHOS_NWEB_EXPORT ConnectNativeOptions {
 public:
  using ConnectCallback = std::function<void(const int connection)>;
  using DisconnectCallback = std::function<void(const int connection)>;
  using FailCallback = std::function<void(int error_code)>;
 
  ConnectNativeOptions() = default;
  ~ConnectNativeOptions() = default;

  void SetOnConnect(ConnectCallback callback) {
    on_connect_callback_ = callback;
  }
 
  void SetOnDisconnect(DisconnectCallback callback) {
    on_disconnect_callback_ = callback;
  }
 
  void SetOnFail(FailCallback callback) {
    on_fail_callback_ = callback;
  }

  void TriggerConnect(const int connection) {
    if (on_connect_callback_) {
      on_connect_callback_(connection);
    }
  }
 
  void TriggerDisconnect(const int connection) {
    if (on_disconnect_callback_) {
      on_disconnect_callback_(connection);
    }
  }
 
  void TriggerFail(int error_code) {
    if (on_fail_callback_) {
      on_fail_callback_(error_code);
    }
  }
 
 private:
  ConnectCallback on_connect_callback_;
  DisconnectCallback on_disconnect_callback_;
  FailCallback on_fail_callback_;
};
 
 
class NWebConnectNativeManager {
 public:
 
  static NWebConnectNativeManager* GetInstance();
 
  void RegisterNWebHandler(int nweb_id, std::shared_ptr<NWebHandler> handler);

  void UnRegisterNWebHandler(int nweb_id);
 
  base::Process LaunchConnectNative(const std::string& native_host_name, 
                          const std::string& origin, 
                          int read_file, 
                          int write_file);
 
 
  void DisconnectNative(int connectionId);
 
  
 
  NWebConnectNativeManager() = default;
 
  NWebConnectNativeManager(const NWebConnectNativeManager&) = delete;
 
  NWebConnectNativeManager& operator=(const NWebConnectNativeManager&) = delete;
 
 private:
 
  static NWebConnectNativeManager* instance;
 
  std::weak_ptr<NWebHandler> nweb_handler_;

  std::unordered_map<int, std::weak_ptr<NWebHandler>> nweb_handler_map_;
 
  std::unordered_map<int, base::Process> connectid_process_map_;
};

class NWebNativeMessageCallbackImpl : public NWebNativeMessageCallback {
 public:
  explicit NWebNativeMessageCallbackImpl(OHOS::NWeb::ConnectNativeOptions callback)
      : callback_(callback) {}

  void OnConnect(int pid) override {
    callback_.TriggerConnect(pid);
  }

  void OnDisconnect(int nativeInfo) override {
    callback_.TriggerDisconnect(nativeInfo);
  }
  void OnFailed(int code) override {
    callback_.TriggerFail(code);
  }

 private:
  OHOS::NWeb::ConnectNativeOptions callback_;
};

}  // namespace OHOS::NWeb
 
#endif  // NWEB_EXTENSION_CONNECT_NATIVE_H_