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

#include "gpu_native_ipc_proxy.h"

#include <native_window/external_window.h>

#include "gpu_native_ipc_common.h"
#include "native_window_parcel.h"
#include "ohos/adapter/common/logging.h"

namespace ohos::adapter::multiprocess {

OH_IPC_ErrorCode GpuNativeIpcProxy::GetPid(int32_t* child_pid) {
  if (child_pid == nullptr) {
    return OH_IPC_CHECK_PARAM_ERROR;
  }
  return PerformRequest(std::make_unique<RequestGetPid>(child_pid));
}

OH_IPC_ErrorCode GpuNativeIpcProxy::InitializeWindowAdapter(
    const std::vector<std::pair<std::string, void*>>& windows) {
  return PerformRequest(
      std::make_unique<RequestInitializeWindowAdapter>(windows));
}

OH_IPC_ErrorCode GpuNativeIpcProxy::AddWindow(const std::string& window_id,
                                              void* window) {
  if (window == nullptr) {
    return OH_IPC_CHECK_PARAM_ERROR;
  }
  return PerformRequest(std::make_unique<RequestAddWindow>(window_id, window));
}

OH_IPC_ErrorCode GpuNativeIpcProxy::RemoveWindow(const std::string& window_id) {
  return PerformRequest(std::make_unique<RequestRemoveWindow>(window_id));
}

OH_IPC_ErrorCode GpuNativeIpcProxy::SetWindowWidget(
    const std::string& window_id,
    int32_t widget_id) {
  return PerformRequest(
      std::make_unique<RequestSetWindowWidget>(window_id, widget_id));
}

OH_IPC_ErrorCode GpuNativeIpcProxy::NotifyWindowChange(
    const std::string& window_id,
    void* window) {
  if (window == nullptr) {
    return OH_IPC_CHECK_PARAM_ERROR;
  }
  return PerformRequest(
      std::make_unique<RequestNotifyWindowChange>(window_id, window));
}

OH_IPC_ErrorCode RequestGetPid::Run(const OHIPCRemoteProxy* remote_proxy) {
  int ret = OH_IPCRemoteProxy_SendRequest(
      remote_proxy, static_cast<uint32_t>(GpuNativeIpcOpCode::GET_PID),
      data_parcel_, reply_parcel_, nullptr);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("Request native IPC request error: %{public}d", ret);
    return static_cast<OH_IPC_ErrorCode>(ret);
  }

  int32_t child_pid;
  ret = OH_IPCParcel_ReadInt32(reply_parcel_, &child_pid);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("Read GPU process pid from reply parcel error: %{public}d", ret);
    return static_cast<OH_IPC_ErrorCode>(ret);
  }

  *child_pid_ = child_pid;
  return OH_IPC_SUCCESS;
}

OH_IPC_ErrorCode RequestInitializeWindowAdapter::Run(
    const OHIPCRemoteProxy* remote_proxy) {
  int ret = OH_IPCParcel_WriteInt32(data_parcel_, windows_.size());
  if (ret != OH_IPC_SUCCESS) {
    LOGE("Write # of elements into data parcel error: %{public}d", ret);
    return static_cast<OH_IPC_ErrorCode>(ret);
  }

  for (auto& [window_id, window] : windows_) {
    ret = OH_IPCParcel_WriteString(data_parcel_, window_id.c_str());
    if (ret != OH_IPC_SUCCESS) {
      LOGE("Write window id into data parcel error: %{public}d", ret);
      return static_cast<OH_IPC_ErrorCode>(ret);
    }

    ret = NativeWindowWriteToParcel(window, data_parcel_);
    if (ret != 0) {
      LOGE("Write native window and attr into parcel failed.");
      return OH_IPC_PARCEL_WRITE_ERROR;
    }
  }

  ret = OH_IPCRemoteProxy_SendRequest(
      remote_proxy,
      static_cast<uint32_t>(GpuNativeIpcOpCode::INITIALIZE_WINDOW_ADAPTER),
      data_parcel_, reply_parcel_, nullptr);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("Request native IPC request error: %{public}d", ret);
    return static_cast<OH_IPC_ErrorCode>(ret);
  }

  return OH_IPC_SUCCESS;
}

OH_IPC_ErrorCode RequestAddWindow::Run(const OHIPCRemoteProxy* remote_proxy) {
  int ret = OH_IPCParcel_WriteString(data_parcel_, window_id_.c_str());
  if (ret != OH_IPC_SUCCESS) {
    LOGE("Write window id into data parcel error: %{public}d", ret);
    return static_cast<OH_IPC_ErrorCode>(ret);
  }
  ret = NativeWindowWriteToParcel(window_, data_parcel_);
  if (ret != 0) {
    LOGE("Write native window into data parcel error: %{public}d", ret);
    return OH_IPC_PARCEL_WRITE_ERROR;
  }

  ret = OH_IPCRemoteProxy_SendRequest(
      remote_proxy, static_cast<uint32_t>(GpuNativeIpcOpCode::ADD_WINDOW),
      data_parcel_, reply_parcel_, nullptr);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("Request native IPC request error: %{public}d", ret);
    return static_cast<OH_IPC_ErrorCode>(ret);
  }

  return OH_IPC_SUCCESS;
}

OH_IPC_ErrorCode RequestRemoveWindow::Run(
    const OHIPCRemoteProxy* remote_proxy) {
  int ret = OH_IPCParcel_WriteString(data_parcel_, window_id_.c_str());
  if (ret != OH_IPC_SUCCESS) {
    LOGE("Write window id into data parcel error: %{public}d", ret);
    return static_cast<OH_IPC_ErrorCode>(ret);
  }

  ret = OH_IPCRemoteProxy_SendRequest(
      remote_proxy, static_cast<uint32_t>(GpuNativeIpcOpCode::REMOVE_WINDOW),
      data_parcel_, reply_parcel_, nullptr);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("Request native IPC request error: %{public}d", ret);
    return static_cast<OH_IPC_ErrorCode>(ret);
  }

  return OH_IPC_SUCCESS;
}

OH_IPC_ErrorCode RequestSetWindowWidget::Run(
    const OHIPCRemoteProxy* remote_proxy) {
  int ret = OH_IPCParcel_WriteString(data_parcel_, window_id_.c_str());
  if (ret != OH_IPC_SUCCESS) {
    LOGE("Write window id into data parcel error: %{public}d", ret);
    return static_cast<OH_IPC_ErrorCode>(ret);
  }
  ret = OH_IPCParcel_WriteInt32(data_parcel_, widget_id_);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("Write widget id into data parcel error: %{public}d", ret);
    return static_cast<OH_IPC_ErrorCode>(ret);
  }

  ret = OH_IPCRemoteProxy_SendRequest(
      remote_proxy,
      static_cast<uint32_t>(GpuNativeIpcOpCode::SET_WINDOW_WIDGET),
      data_parcel_, reply_parcel_, nullptr);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("Request native IPC request error: %{public}d", ret);
    return static_cast<OH_IPC_ErrorCode>(ret);
  }

  return OH_IPC_SUCCESS;
}

OH_IPC_ErrorCode RequestNotifyWindowChange::Run(
    const OHIPCRemoteProxy* remote_proxy) {
  int ret = OH_IPCParcel_WriteString(data_parcel_, window_id_.c_str());
  if (ret != OH_IPC_SUCCESS) {
    LOGE("Write window id into data parcel error: %{public}d", ret);
    return static_cast<OH_IPC_ErrorCode>(ret);
  }
  ret = NativeWindowAttrToParcel(window_, data_parcel_);
  if (ret != 0) {
    LOGE("Write native window into data parcel error: %{public}d", ret);
    return OH_IPC_PARCEL_WRITE_ERROR;
  }

  ret = OH_IPCRemoteProxy_SendRequest(
      remote_proxy,
      static_cast<uint32_t>(GpuNativeIpcOpCode::NOTIFY_WINDOW_CHANGE),
      data_parcel_, reply_parcel_, nullptr);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("Request native IPC request error: %{public}d", ret);
    return static_cast<OH_IPC_ErrorCode>(ret);
  }

  return OH_IPC_SUCCESS;
}
}  // namespace ohos::adapter::multiprocess
