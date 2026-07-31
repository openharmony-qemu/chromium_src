// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu_native_ipc_proxy.h"

#include <native_window/external_window.h>

#include "gpu_native_ipc_common.h"
#include "native_window_parcel.h"
#include "ohos/adapter/common/logging.h"

namespace ohos::adapter::multiprocess {

OH_IPC_ErrorCode GpuNativeIpcProxy::GetPid(int32_t* child_pid) {
  if (child_pid == nullptr) {
    LOGE("[GpuNativeIpcProxy] %{public}s: child_pid parameter is null", __func__);
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
    LOGE("[GpuNativeIpcProxy] %{public}s: window parameter is null, window_id=%{public}s",
         __func__, window_id.c_str());
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
    LOGE("[GpuNativeIpcProxy] %{public}s: window parameter is null, window_id=%{public}s",
         __func__, window_id.c_str());
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
    LOGE("[GpuNativeIpcProxy] %{public}s: call OH_IPCRemoteProxy_SendRequest GET_PID failed, error=%{public}d",
         __func__, ret);
    return static_cast<OH_IPC_ErrorCode>(ret);
  }

  int32_t child_pid;
  ret = OH_IPCParcel_ReadInt32(reply_parcel_, &child_pid);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("[GpuNativeIpcProxy] %{public}s: call OH_IPCParcel_ReadInt32 for child_pid failed, error=%{public}d",
         __func__, ret);
    return static_cast<OH_IPC_ErrorCode>(ret);
  }

  *child_pid_ = child_pid;
  return OH_IPC_SUCCESS;
}

OH_IPC_ErrorCode RequestInitializeWindowAdapter::Run(
    const OHIPCRemoteProxy* remote_proxy) {
  int ret = OH_IPCParcel_WriteInt32(data_parcel_, windows_.size());
  if (ret != OH_IPC_SUCCESS) {
    LOGE("[GpuNativeIpcProxy] %{public}s: call OH_IPCParcel_WriteInt32 for window count failed, error=%{public}d",
         __func__, ret);
    return static_cast<OH_IPC_ErrorCode>(ret);
  }

  for (auto& [window_id, window] : windows_) {
    ret = OH_IPCParcel_WriteString(data_parcel_, window_id.c_str());
    if (ret != OH_IPC_SUCCESS) {
      LOGE("[GpuNativeIpcProxy] %{public}s: call OH_IPCParcel_WriteString for window_id failed, error=%{public}d",
           __func__, ret);
      return static_cast<OH_IPC_ErrorCode>(ret);
    }

    ret = NativeWindowWriteToParcel(window, data_parcel_);
    if (ret != 0) {
      LOGE("[GpuNativeIpcProxy] %{public}s: NativeWindowWriteToParcel failed for window_id=%{public}s",
           __func__, window_id.c_str());
      return OH_IPC_PARCEL_WRITE_ERROR;
    }
  }

  ret = OH_IPCRemoteProxy_SendRequest(
      remote_proxy,
      static_cast<uint32_t>(GpuNativeIpcOpCode::INITIALIZE_WINDOW_ADAPTER),
      data_parcel_, reply_parcel_, nullptr);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("[GpuNativeIpcProxy] %{public}s:"
         "call OH_IPCRemoteProxy_SendRequest INITIALIZE_WINDOW_ADAPTER failed, error=%{public}d",
         __func__, ret);
    return static_cast<OH_IPC_ErrorCode>(ret);
  }

  return OH_IPC_SUCCESS;
}

OH_IPC_ErrorCode RequestAddWindow::Run(const OHIPCRemoteProxy* remote_proxy) {
  int ret = OH_IPCParcel_WriteString(data_parcel_, window_id_.c_str());
  if (ret != OH_IPC_SUCCESS) {
    LOGE("[GpuNativeIpcProxy] %{public}s: call OH_IPCParcel_WriteString for window_id failed, error=%{public}d",
         __func__, ret);
    return static_cast<OH_IPC_ErrorCode>(ret);
  }
  ret = NativeWindowWriteToParcel(window_, data_parcel_);
  if (ret != 0) {
    LOGE("[GpuNativeIpcProxy] %{public}s: NativeWindowWriteToParcel failed, window_id=%{public}s, error=%{public}d",
         __func__, window_id_.c_str(), ret);
    return OH_IPC_PARCEL_WRITE_ERROR;
  }

  ret = OH_IPCRemoteProxy_SendRequest(
      remote_proxy, static_cast<uint32_t>(GpuNativeIpcOpCode::ADD_WINDOW),
      data_parcel_, reply_parcel_, nullptr);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("[GpuNativeIpcProxy] %{public}s: call OH_IPCRemoteProxy_SendRequest ADD_WINDOW failed, error=%{public}d",
         __func__, ret);
    return static_cast<OH_IPC_ErrorCode>(ret);
  }

  return OH_IPC_SUCCESS;
}

OH_IPC_ErrorCode RequestRemoveWindow::Run(
    const OHIPCRemoteProxy* remote_proxy) {
  int ret = OH_IPCParcel_WriteString(data_parcel_, window_id_.c_str());
  if (ret != OH_IPC_SUCCESS) {
    LOGE("[GpuNativeIpcProxy] %{public}s: call OH_IPCParcel_WriteString for window_id failed, error=%{public}d",
         __func__, ret);
    return static_cast<OH_IPC_ErrorCode>(ret);
  }

  ret = OH_IPCRemoteProxy_SendRequest(
      remote_proxy, static_cast<uint32_t>(GpuNativeIpcOpCode::REMOVE_WINDOW),
      data_parcel_, reply_parcel_, nullptr);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("[GpuNativeIpcProxy] %{public}s: call OH_IPCRemoteProxy_SendRequest REMOVE_WINDOW failed, error=%{public}d",
         __func__, ret);
    return static_cast<OH_IPC_ErrorCode>(ret);
  }

  return OH_IPC_SUCCESS;
}

OH_IPC_ErrorCode RequestSetWindowWidget::Run(
    const OHIPCRemoteProxy* remote_proxy) {
  int ret = OH_IPCParcel_WriteString(data_parcel_, window_id_.c_str());
  if (ret != OH_IPC_SUCCESS) {
    LOGE("[GpuNativeIpcProxy] %{public}s: call OH_IPCParcel_WriteString for window_id failed, error=%{public}d",
         __func__, ret);
    return static_cast<OH_IPC_ErrorCode>(ret);
  }
  ret = OH_IPCParcel_WriteInt32(data_parcel_, widget_id_);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("[GpuNativeIpcProxy] %{public}s: call OH_IPCParcel_WriteInt32 for widget_id failed, error=%{public}d",
         __func__, ret);
    return static_cast<OH_IPC_ErrorCode>(ret);
  }

  ret = OH_IPCRemoteProxy_SendRequest(
      remote_proxy,
      static_cast<uint32_t>(GpuNativeIpcOpCode::SET_WINDOW_WIDGET),
      data_parcel_, reply_parcel_, nullptr);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("[GpuNativeIpcProxy] %{public}s:"
         "call OH_IPCRemoteProxy_SendRequest SET_WINDOW_WIDGET failed, error=%{public}d",
         __func__, ret);
    return static_cast<OH_IPC_ErrorCode>(ret);
  }

  return OH_IPC_SUCCESS;
}

OH_IPC_ErrorCode RequestNotifyWindowChange::Run(
    const OHIPCRemoteProxy* remote_proxy) {
  int ret = OH_IPCParcel_WriteString(data_parcel_, window_id_.c_str());
  if (ret != OH_IPC_SUCCESS) {
    LOGE("[GpuNativeIpcProxy] %{public}s:"
         "call OH_IPCParcel_WriteString for window_id failed, error=%{public}d",
         __func__, ret);
    return static_cast<OH_IPC_ErrorCode>(ret);
  }
  ret = NativeWindowAttrToParcel(window_, data_parcel_);
  if (ret != 0) {
    LOGE("[GpuNativeIpcProxy] %{public}s:"
         "NativeWindowAttrToParcel failed, window_id=%{public}s, error=%{public}d",
         __func__, window_id_.c_str(), ret);
    return OH_IPC_PARCEL_WRITE_ERROR;
  }

  ret = OH_IPCRemoteProxy_SendRequest(
      remote_proxy,
      static_cast<uint32_t>(GpuNativeIpcOpCode::NOTIFY_WINDOW_CHANGE),
      data_parcel_, reply_parcel_, nullptr);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("[GpuNativeIpcProxy] %{public}s:"
         "call OH_IPCRemoteProxy_SendRequest NOTIFY_WINDOW_CHANGE failed, error=%{public}d",
         __func__, ret);
    return static_cast<OH_IPC_ErrorCode>(ret);
  }

  return OH_IPC_SUCCESS;
}
}  // namespace ohos::adapter::multiprocess
