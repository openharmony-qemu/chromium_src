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

#include <unistd.h>
#include <sstream>

#include "include/cef_app.h"
#include "nweb_export.h"
#include "nweb_hilog.h"
#include "base/process/process_handle.h"
#include <AbilityKit/native_child_process.h>
#include <IPCKit/ipc_kit.h>
#include <native_window/external_window.h>

#include "base/posix/global_descriptors.h"
#include "content/public/common/content_descriptors.h"

namespace {
const uint32_t IPC_B2G_CODE_INIT_DATA = 10001;
const uint32_t IPC_G2B_CODE_QUERY_WINDOW = 10002;
const uint32_t IPC_G2B_CODE_DESTROY_WINDOW = 10003;
const uint32_t IPC_G2B_CODE_PASS_SURFACE = 10004;
const uint32_t IPC_G2B_CODE_QUERY_BOOL = 10005;
const char *GPU_IPC_DESCRIPTOR = "chromium.native.gpu";
const int START_TIMEOUT_SEC = 3;

int g_ipc_fd = 0, g_shared_fd = 0, g_crash_fd = 0;
std::string g_param;
std::mutex mtx;
std::condition_variable cv;

OHIPCRemoteProxy* g_ipc_remote_proxy = nullptr;
} // namespace

extern bool SetGlobalDescriptors(int ipcFd, int sharedFd, int crashFd);

void NWebRenderMainForGPU(const char* args) {
  WVLOG_I("NWebRenderMain for GPU start, sandbox pid=%{public}d", getpid());
  std::string args_str = args;

  std::stringstream args_ss(args_str);
  const char separator = '#';
  std::vector<std::string> argv_str;
  std::string arg_str;
  while (std::getline(args_ss, arg_str, separator)) {
    argv_str.push_back(arg_str);
  }
  std::vector<char*> argv_cstr;
  int argc = argv_str.size();
  argv_cstr.reserve(argc + 1);
  for (const auto& arg : argv_str) {
    argv_cstr.push_back(const_cast<char*>(arg.c_str()));
  }
  argv_cstr.push_back(nullptr);

  CefMainArgs main_args(argc, const_cast<char**>(argv_cstr.data()));

  if (!SetGlobalDescriptors(g_ipc_fd, g_shared_fd, g_crash_fd)) {
    WVLOG_E("failed to set global fd");
    return;
  }

  (void)CefExecuteProcess(main_args, nullptr, nullptr);
  WVLOG_I("NWebRenderMain end, sandbox pid=%{public}d", getpid());
}

int OnRemoteRequest(uint32_t code, const OHIPCParcel* data, OHIPCParcel* reply, void* user_data) {
  WVLOG_I("Receive a native IPC remote request from browser process, code=%{public}d", code);

  if (code == IPC_B2G_CODE_INIT_DATA) {
    std::unique_lock<std::mutex> lock(mtx);
    g_ipc_remote_proxy = OH_IPCParcel_ReadRemoteProxy(data);
    g_param = std::string(OH_IPCParcel_ReadString(data));
    OH_IPCParcel_ReadFileDescriptor(data, &g_ipc_fd);
    OH_IPCParcel_ReadFileDescriptor(data, &g_shared_fd);
    OH_IPCParcel_ReadFileDescriptor(data, &g_crash_fd);
    int pid = getpid();
    OH_IPCParcel_WriteInt32(reply, pid);
    cv.notify_all();
    return 0;
  } else {
    WVLOG_E("unknow code from browser process, code = %{public}d", code);
    return -1;
  }
}

void* QueryRenderWindowFromBrowserProcess(int32_t window_id) {
  if (g_ipc_remote_proxy == nullptr) {
    WVLOG_E("ipc remote proxy is null");
    return nullptr;
  }
  OHIPCParcel* data = OH_IPCParcel_Create();
  OHIPCParcel* reply = OH_IPCParcel_Create();
  OH_IPCParcel_WriteInt32(data, window_id);
  if (OH_IPCRemoteProxy_SendRequest(g_ipc_remote_proxy, IPC_G2B_CODE_QUERY_WINDOW, data, reply, nullptr) != 0) {
    WVLOG_E("failed to send QueryRenderWindow id = %{public}d", window_id);
    OH_IPCParcel_Destroy(data);
    OH_IPCParcel_Destroy(reply);
    return nullptr;
  }

  OHNativeWindow *window;
  if (OH_NativeWindow_ReadFromParcel(reply, &window) != 0) {
    WVLOG_E("failed to read window for id = %{public}d", window_id);
    OH_IPCParcel_Destroy(data);
    OH_IPCParcel_Destroy(reply);
    return nullptr;
  }

  OH_IPCParcel_Destroy(data);
  OH_IPCParcel_Destroy(reply);
  return (void*)window;
}

void DestoryRenderWindowFromBrowserProcess(int32_t window_id) {
  if (g_ipc_remote_proxy == nullptr) {
    WVLOG_E("ipc remote proxy is null");
    return;
  }
  OHIPCParcel* data = OH_IPCParcel_Create();
  OHIPCParcel* reply = OH_IPCParcel_Create();
  OH_IPCParcel_WriteInt32(data, window_id);
  if (OH_IPCRemoteProxy_SendRequest(g_ipc_remote_proxy, IPC_G2B_CODE_DESTROY_WINDOW, data, reply, nullptr) != 0) {
    WVLOG_E("failed to send DestoryRenderWindow id = %{public}d", window_id);
    OH_IPCParcel_Destroy(data);
    OH_IPCParcel_Destroy(reply);
    return;
  }

  OH_IPCParcel_Destroy(data);
  OH_IPCParcel_Destroy(reply);
}

void PassWindow(int64_t window_id) {
  if (g_ipc_remote_proxy == nullptr) {
    WVLOG_E("ipc remote proxy is null");
    return;
  }
  OHNativeWindow* window;
  if (OH_NativeWindow_CreateNativeWindowFromSurfaceId(window_id, &window) != 0) {
    WVLOG_E("PassWindow get window failed, id = %{public}d", window_id);
    return;
  }
  OHIPCParcel* data = OH_IPCParcel_Create();
  OHIPCParcel* reply = OH_IPCParcel_Create();
  if (OH_NativeWindow_WriteToParcel(window, data) != 0) {
    WVLOG_E("PassWindow write to window failed, id = %{public}d", window_id);
    OH_IPCParcel_Destroy(data);
    OH_IPCParcel_Destroy(reply);
    return;
  }
  if(OH_IPCRemoteProxy_SendRequest(g_ipc_remote_proxy, IPC_G2B_CODE_PASS_SURFACE, data, reply, nullptr) != 0) {
    WVLOG_E("PassWindow send request failed, id = %{public}d", window_id);
    OH_IPCParcel_Destroy(data);
    OH_IPCParcel_Destroy(reply);
    return;
  }
  OH_IPCParcel_Destroy(data);
  OH_IPCParcel_Destroy(reply);
}

bool QueryBoolFromBrowserProcess(const std::string& key, bool defaultValue) {
  if (g_ipc_remote_proxy == nullptr) {
    WVLOG_I("ipc remote proxy is null");
    return OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .GetSystemPropertiesInstance().GetBoolParameter(key, defaultValue);
  }

  OHIPCParcel* data = OH_IPCParcel_Create();
  OHIPCParcel* reply = OH_IPCParcel_Create();

  if (OH_IPCParcel_WriteString(data, key.c_str()) != 0) {
    WVLOG_E("failed to write key");
    return defaultValue;
  }

  if (OH_IPCParcel_WriteInt32(data, defaultValue) != 0) {
    WVLOG_E("failed to write default value");
    return defaultValue;
  }

  if (OH_IPCRemoteProxy_SendRequest(g_ipc_remote_proxy, IPC_G2B_CODE_QUERY_BOOL, data, reply, nullptr) != 0) {
    WVLOG_E("failed to send query bool key = %{public}s", key.c_str());
    OH_IPCParcel_Destroy(data);
    OH_IPCParcel_Destroy(reply);
    return defaultValue;
  }

  int32_t ret = defaultValue;
  if (OH_IPCParcel_ReadInt32(reply, &ret) != 0) {
    WVLOG_E("failed to read value");
    return -1;
  }

  OH_IPCParcel_Destroy(data);
  OH_IPCParcel_Destroy(reply);
  return !!ret;
}

extern "C" OHOS_NWEB_EXPORT OHIPCRemoteStub* NativeChildProcess_OnConnect() {
  WVLOG_I("NativeChildProcess_OnConnect, create IPC remote stub.");
  OHIPCRemoteStub* stub = OH_IPCRemoteStub_Create(GPU_IPC_DESCRIPTOR, &OnRemoteRequest, nullptr, nullptr);
  if (stub == nullptr) {
    WVLOG_E("NativeChildProcess_OnConnect: create stub failed.");
    return nullptr;
  }
  return stub;
}

extern "C" OHOS_NWEB_EXPORT void NativeChildProcess_MainProc() {
  WVLOG_I("NativeChildProcess_MainProc");

  std::unique_lock<std::mutex> lock(mtx);
  if (!cv.wait_for(lock, std::chrono::seconds(START_TIMEOUT_SEC), [] {
      return !g_param.empty() && (g_ipc_fd != 0) && (g_shared_fd != 0) \
        && (g_ipc_remote_proxy != nullptr);
    })) {
    WVLOG_E("timeout for get start param");
    return;
  }

  NWebRenderMainForGPU(g_param.c_str());
}