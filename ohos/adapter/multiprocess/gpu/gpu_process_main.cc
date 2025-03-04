// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <IPCKit/ipc_kit.h>

#include <cstdint>

#include "gpu_native_ipc_common.h"
#include "gpu_native_ipc_stub.h"
#include "gpu_native_process.h"
#include "ohos/adapter/common/constants.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/export.h"
#include "ohos/adapter/web_entry/web_entry.h"

namespace ohos::adapter::multiprocess {

int OnRemoteRequest(uint32_t code,
                    const OHIPCParcel* data,
                    OHIPCParcel* reply,
                    void* user_data) {
  if (code <= static_cast<int>(GpuNativeIpcOpCode::INVALID_OPERATION_CODE) ||
      code >= static_cast<int>(GpuNativeIpcOpCode::MAX_OPERATION_CODE)) {
    LOGE("Receive a native IPC remote request with invalid code: %{public}d",
         code);
    return OH_IPC_CHECK_PARAM_ERROR;
  }
  return GpuNativeIpcStub::GetInstance().HandleRemoteRequest(
      static_cast<GpuNativeIpcOpCode>(code), data, reply, user_data);
}

#ifdef __cplusplus
extern "C" {
#endif

OHIPCRemoteStub* ADAPTER_EXPORT_API NativeChildProcess_OnConnect() {
  OHIPCRemoteStub* stub = OH_IPCRemoteStub_Create(
      GPU_IPC_DESCRIPTOR, &OnRemoteRequest, nullptr, nullptr);
  if (stub == nullptr) {
    LOGE("NativeChildProcess_OnConnect: Create stub failed.");
    return nullptr;
  }
  GpuNativeIpcStub::GetInstance().Initialize(stub);
  return stub;
}

// Main function when start gpu process by calling
// OH_Ability_CreateNativeChildProcess()
// 1. waiting for OHNativeWindow sent by parent process,
// 2. call RunOtherProcessType()
void ADAPTER_EXPORT_API NativeChildProcess_MainProc() {
  GpuNativeProcess& process = GpuNativeProcess::GetInstance();

  // Waiting for WindowAdapter initialization result.
  bool can_run_process = process.GetInitializeResult();
  if (!can_run_process) {
    LOGE(
        "NativeChildProcess_MainProc: receive message from parent process "
        "failed, exit.");
    return;
  }

  ohos::adapter::web_entry::RunOtherProcessType(ProcessType::kGpuProcess);
}

#ifdef __cplusplus
}  // extern "C"
#endif

}  // namespace ohos::adapter::multiprocess
