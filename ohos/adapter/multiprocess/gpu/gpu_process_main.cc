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

#include <cstdint>

#include <IPCKit/ipc_kit.h>

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
