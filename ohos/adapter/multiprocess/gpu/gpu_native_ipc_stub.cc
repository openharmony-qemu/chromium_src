// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu_native_ipc_stub.h"

#include <native_window/external_window.h>

#include <functional>

#include "gpu_native_ipc_common.h"
#include "gpu_native_process.h"
#include "native_window_parcel.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"

using namespace ohos::adapter::xcomponent;

namespace ohos::adapter::multiprocess {

GpuNativeIpcStub& GpuNativeIpcStub::GetInstance() {
  static GpuNativeIpcStub instance;
  return instance;
}

OH_IPC_ErrorCode GpuNativeIpcStub::HandleInvalidCode(GpuNativeIpcOpCode code,
                                                     const OHIPCParcel* data,
                                                     OHIPCParcel* reply,
                                                     void* user_data) {
  static_cast<void>(data);
  static_cast<void>(reply);
  static_cast<void>(user_data);
  LOGE("Invalid operation code: %{public}d", static_cast<int>(code));
  return OH_IPC_CHECK_PARAM_ERROR;
}

OH_IPC_ErrorCode GpuNativeIpcStub::HandleGetPid(GpuNativeIpcOpCode code,
                                                const OHIPCParcel* data,
                                                OHIPCParcel* reply,
                                                void* user_data) {
  static_cast<void>(data);
  static_cast<void>(user_data);

  if ((code) != (GpuNativeIpcOpCode::GET_PID)) {
    LOGE("Unexpected operation coed, actual: %{public}d, expected: %{public}d",
         static_cast<int>(code), static_cast<int>(GpuNativeIpcOpCode::GET_PID));
    return OH_IPC_CHECK_PARAM_ERROR;
  }

  int pid = GpuNativeProcess::GetInstance().GetPid();
  int ret = OH_IPCParcel_WriteInt32(reply, pid);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("Write pid into reply parcel error: %{public}d", ret);
    return static_cast<OH_IPC_ErrorCode>(ret);
  }

  return OH_IPC_SUCCESS;
}

OH_IPC_ErrorCode GpuNativeIpcStub::AddWindowInner(const OHIPCParcel* data) {
  const char* window_id = OH_IPCParcel_ReadString(data);
  if (window_id == nullptr) {
    LOGE("Read window id from data parcel error.");
    return OH_IPC_PARCEL_READ_ERROR;
  }

  void* window = nullptr;
  int ret = NativeWindowReadFromParcel(data, &window);
  if (ret != 0 || window == nullptr) {
    LOGE("Read native window from parcel error: %{public}d", ret);
    return OH_IPC_PARCEL_READ_ERROR;
  }

  GpuNativeProcess::GetInstance().AddWindow(window_id, window);

  return OH_IPC_SUCCESS;
}

OH_IPC_ErrorCode GpuNativeIpcStub::HandleInitializeWindowAdapterInner(
    GpuNativeIpcOpCode code,
    const OHIPCParcel* data) {
  if ((code) != (GpuNativeIpcOpCode::INITIALIZE_WINDOW_ADAPTER)) {
    LOGE("Unexpected operation coed, actual: %{public}d, expected: %{public}d",
         static_cast<int>(code),
         static_cast<int>(GpuNativeIpcOpCode::INITIALIZE_WINDOW_ADAPTER));
    return OH_IPC_CHECK_PARAM_ERROR;
  }

  int num_elements;
  int ret = OH_IPCParcel_ReadInt32(data, &num_elements);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("Read # of elements error: %{public}d", ret);
    return static_cast<OH_IPC_ErrorCode>(ret);
  }

  for (int i = 0; i < num_elements; i++) {
    LOGD("Begin to read #%{public}d element.", i);
    ret = AddWindowInner(data);
    if (ret != OH_IPC_SUCCESS) {
      return static_cast<OH_IPC_ErrorCode>(ret);
    }
  }

  return OH_IPC_SUCCESS;
}

OH_IPC_ErrorCode GpuNativeIpcStub::HandleInitializeWindowAdapter(
    GpuNativeIpcOpCode code,
    const OHIPCParcel* data,
    OHIPCParcel* reply,
    void* user_data) {
  static_cast<void>(reply);
  static_cast<void>(user_data);

  OH_IPC_ErrorCode ret = HandleInitializeWindowAdapterInner(code, data);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("InitializeWindowAdapter failed, error: %{public}d", ret);
    GpuNativeProcess::GetInstance().SetInitializeResult(false);
    return ret;
  }

  GpuNativeProcess::GetInstance().SetInitializeResult(true);
  return OH_IPC_SUCCESS;
}

OH_IPC_ErrorCode GpuNativeIpcStub::HandleAddWindow(GpuNativeIpcOpCode code,
                                                   const OHIPCParcel* data,
                                                   OHIPCParcel* reply,
                                                   void* user_data) {
  static_cast<void>(reply);
  static_cast<void>(user_data);

  if ((code) != (GpuNativeIpcOpCode::ADD_WINDOW)) {
    LOGE("Unexpected operation coed, actual: %{public}d, expected: %{public}d",
         static_cast<int>(code),
         static_cast<int>(GpuNativeIpcOpCode::ADD_WINDOW));
    return OH_IPC_CHECK_PARAM_ERROR;
  }

  return AddWindowInner(data);
}

OH_IPC_ErrorCode GpuNativeIpcStub::HandleRemoveWindow(GpuNativeIpcOpCode code,
                                                      const OHIPCParcel* data,
                                                      OHIPCParcel* reply,
                                                      void* user_data) {
  static_cast<void>(reply);
  static_cast<void>(user_data);

  if ((code) != (GpuNativeIpcOpCode::REMOVE_WINDOW)) {
    LOGE("Unexpected operation coed, actual: %{public}d, expected: %{public}d",
         static_cast<int>(code),
         static_cast<int>(GpuNativeIpcOpCode::REMOVE_WINDOW));
    return OH_IPC_CHECK_PARAM_ERROR;
  }

  const char* window_id = OH_IPCParcel_ReadString(data);
  if (window_id == nullptr) {
    LOGE("Read window id from data parcel error.");
    return OH_IPC_PARCEL_READ_ERROR;
  }

  GpuNativeProcess::GetInstance().RemoveWindow(window_id);
  return OH_IPC_SUCCESS;
}

OH_IPC_ErrorCode GpuNativeIpcStub::HandleSetWindowWidget(
    GpuNativeIpcOpCode code,
    const OHIPCParcel* data,
    OHIPCParcel* reply,
    void* user_data) {
  static_cast<void>(reply);
  static_cast<void>(user_data);

  if ((code) != (GpuNativeIpcOpCode::SET_WINDOW_WIDGET)) {
    LOGE("Unexpected operation coed, actual: %{public}d, expected: %{public}d",
         static_cast<int>(code),
         static_cast<int>(GpuNativeIpcOpCode::SET_WINDOW_WIDGET));
    return OH_IPC_CHECK_PARAM_ERROR;
  }
  void* window = GetWindowFromAdapter(data);
  if (window == nullptr) {
    return OH_IPC_INNER_ERROR;
  }

  int32_t widget_id;
  int ret = OH_IPCParcel_ReadInt32(data, &widget_id);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("Read widget id from data parcel error: %{public}d", ret);
    return static_cast<OH_IPC_ErrorCode>(ret);
  }

  GpuNativeProcess::GetInstance().SetWindowWidget(window, widget_id);

  return OH_IPC_SUCCESS;
}

OH_IPC_ErrorCode GpuNativeIpcStub::HandleNotifyWindowChange(
    ohos::adapter::multiprocess::GpuNativeIpcOpCode code,
    const OHIPCParcel* data,
    OHIPCParcel* reply,
    void* user_data) {
  static_cast<void>(reply);
  static_cast<void>(user_data);

  if ((code) != (GpuNativeIpcOpCode::NOTIFY_WINDOW_CHANGE)) {
    LOGE("Unexpected operation coed, actual: %{public}d, expected: %{public}d",
         static_cast<int>(code),
         static_cast<int>(GpuNativeIpcOpCode::NOTIFY_WINDOW_CHANGE));
    return OH_IPC_CHECK_PARAM_ERROR;
  }

  void* window = GetWindowFromAdapter(data);
  if (window == nullptr) {
    return OH_IPC_INNER_ERROR;
  }

  if (NativeWindowAttrFromParcel(data, window) != OH_IPC_SUCCESS) {
    LOGE("Native window read and setup from parcel error.");
    return OH_IPC_INNER_ERROR;
  }

  return OH_IPC_SUCCESS;
}

void* GpuNativeIpcStub::GetWindowFromAdapter(const OHIPCParcel* data) {
  const char* window_id = OH_IPCParcel_ReadString(data);
  if (window_id == nullptr) {
    LOGE("Read window id from data parcel error.");
    return nullptr;
  }

  void* window = WindowAdapter::GetInstance().GetWindow(window_id);
  if (window == nullptr) {
    LOGE("NOTIFY_WINDOW_CHANGE Native window not found, window_id: %{public}s",
         window_id);
  }
  return window;
}

OH_IPC_ErrorCode GpuNativeIpcStub::HandleRemoteRequest(GpuNativeIpcOpCode code,
                                                       const OHIPCParcel* data,
                                                       OHIPCParcel* reply,
                                                       void* user_data) {
  if (data == nullptr || reply == nullptr) {
    LOGE("GpuNativeIpcStub::HandleRemoteRequest: data or reply is null.");
    return OH_IPC_INNER_ERROR;
  }
  if (code >= GpuNativeIpcOpCode::MAX_OPERATION_CODE) {
    return HandleInvalidCode(code, data, reply, user_data);
  }

  /**
   * GPU native IPC requests handlers,
   * must be declared in the same order
   * as in GpuNativeIpcOpCode.
   */
  static std::function<OH_IPC_ErrorCode(
      GpuNativeIpcOpCode code, const OHIPCParcel* data, OHIPCParcel* reply)>
      request_handlers[] = {
          [this, user_data](GpuNativeIpcOpCode code, const OHIPCParcel* data,
                            OHIPCParcel* reply) -> OH_IPC_ErrorCode {
            return this->GetInstance().HandleInvalidCode(code, data, reply,
                                                         user_data);
          },
          [this, user_data](GpuNativeIpcOpCode code, const OHIPCParcel* data,
                            OHIPCParcel* reply) -> OH_IPC_ErrorCode {
            return this->GetInstance().HandleGetPid(code, data, reply,
                                                    user_data);
          },
          [this, user_data](GpuNativeIpcOpCode code, const OHIPCParcel* data,
                            OHIPCParcel* reply) -> OH_IPC_ErrorCode {
            return this->GetInstance().HandleInitializeWindowAdapter(
                code, data, reply, user_data);
          },
          [this, user_data](GpuNativeIpcOpCode code, const OHIPCParcel* data,
                            OHIPCParcel* reply) -> OH_IPC_ErrorCode {
            return this->GetInstance().HandleAddWindow(code, data, reply,
                                                       user_data);
          },
          [this, user_data](GpuNativeIpcOpCode code, const OHIPCParcel* data,
                            OHIPCParcel* reply) -> OH_IPC_ErrorCode {
            return this->GetInstance().HandleRemoveWindow(code, data, reply,
                                                          user_data);
          },
          [this, user_data](GpuNativeIpcOpCode code, const OHIPCParcel* data,
                            OHIPCParcel* reply) -> OH_IPC_ErrorCode {
            return this->GetInstance().HandleSetWindowWidget(code, data, reply,
                                                             user_data);
          },
          [this, user_data](GpuNativeIpcOpCode code, const OHIPCParcel* data,
                            OHIPCParcel* reply) -> OH_IPC_ErrorCode {
            return this->GetInstance().HandleNotifyWindowChange(
                code, data, reply, user_data);
          },
      };

  return request_handlers[(int)code](code, data, reply);
}

}  // namespace ohos::adapter::multiprocess
