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

#include <string>
#include <mutex>
#include "base/logging.h"

#include "nweb_extension_connect_native.h"
#include "nweb_runtime_connect_info_impl.h"
 
namespace OHOS::NWeb {
 
NWebConnectNativeManager* NWebConnectNativeManager::instance = nullptr;
 
NWebConnectNativeManager* NWebConnectNativeManager::GetInstance() {
  if (instance == nullptr) {
    instance = new NWebConnectNativeManager();
  }
  return instance;
}
 
void NWebConnectNativeManager::RegisterNWebHandler(int nweb_id, std::shared_ptr<NWebHandler> handler) {
  nweb_handler_ = std::weak_ptr<NWebHandler>(handler);
  auto it = nweb_handler_map_.find(nweb_id);
  if (it == nweb_handler_map_.end()) {
    nweb_handler_map_[nweb_id] = handler;
  }
}
 
void NWebConnectNativeManager::UnRegisterNWebHandler(int nweb_id) {
  auto it = nweb_handler_map_.find(nweb_id);
  if (it != nweb_handler_map_.end()) {
    nweb_handler_map_.erase(it);
  }
}

base::Process NWebConnectNativeManager::LaunchConnectNative(const std::string& native_host_name, 
                                                  const std::string& origin, 
                                                  int read_file, 
                                                  int write_file) {
  ConnectNativeInfo info;
  info.connectionId = -1;
  info.bundleName = native_host_name;
  info.extensionOrigin = origin;
  info.extensionPid = -1;
  info.messageReadPipeId = read_file;
  info.messageWritePipeId = write_file;
  LOG(INFO) << "NWebConnectNativeManager::LaunchConnectNative bundleName: " << info.bundleName
            << " extensionOrigin: " << info.extensionOrigin
            << " read_file: " << info.messageReadPipeId
            << " write_file: " << info.messageWritePipeId;

  OHOS::NWeb::ConnectNativeOptions options;

  options.SetOnConnect([](const int connectionId) {
    LOG(INFO) << "Connection Success, PID is " << connectionId;
  });
 
  options.SetOnDisconnect([](const int connectionId) {
    LOG(INFO) << "DisConnection Sucess, PID is" << connectionId;
  });
 
  options.SetOnFail([](int error_code) {
    LOG(ERROR) << "Connection Fail, ERROR CODE is " << error_code;
  });

  std::string connection_result = "";
  std::shared_ptr<NWebNativeMessageCallback> nweb_callback =
      std::make_shared<NWebNativeMessageCallbackImpl>(options);
  std::shared_ptr<NWebRuntimeConnectInfo> nwebRuntimeConnectInfo =
      std::make_shared<NWebRuntimeConnectInfoImpl>(info.bundleName, info.extensionOrigin, 
      info.messageReadPipeId, info.messageWritePipeId);

  if (!nweb_handler_map_.empty()) {
    auto weak_handler = nweb_handler_map_.begin()->second;
    if (auto handler = weak_handler.lock()) {
      connection_result = handler->OnWebNativeMessage(nwebRuntimeConnectInfo, nweb_callback);
      LOG(INFO) << "Connect appservice create process connection_result: " << connection_result;
    }
  } else {
    LOG(ERROR) << "LaunchConnectNative cannot find web handler";
    return base::Process();
  }

  int connectionId = -1;
  size_t pos = connection_result.find(':');
  std::string status = connection_result.substr(0, pos);
  if (status == "success"){
    connectionId = std::stoi(connection_result.substr(pos + 1));
  } else {
    LOG(ERROR) << "Connect appservice process failed";
    return base::Process();
  }

  LOG(INFO) << "Connect appservice create process PID: " << connectionId;
  base::Process local_process = base::Process(connectionId);

  auto it = connectid_process_map_.find(connectionId);
  if (it == connectid_process_map_.end()) {
    base::Process duplicated_process  = local_process.Duplicate();
    connectid_process_map_[connectionId] = std::move(duplicated_process);
  }
  return local_process;
}
 
void NWebConnectNativeManager::DisconnectNative(int connectionId) {
  auto it = connectid_process_map_.find(connectionId);
  if (it != connectid_process_map_.end()) {
    if (!nweb_handler_map_.empty()) {
      auto weak_handler = nweb_handler_map_.begin()->second;
      if (auto handler = weak_handler.lock()) {      
        LOG(INFO) << "DisconnectNative send to appservice PID: " << connectionId;
        handler->OnExtensionDisconnect(connectionId);
      }
    } else {
      LOG(ERROR) << "DisconnectNative cannot find web handler";
      return;
    }
    connectid_process_map_.erase(it);
  } else {
    LOG(ERROR) << "DisconnectNative Fail, Cannot found PID:" << connectionId;
  }
}
}  // namespace OHOS::NWeb

