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

#ifndef OHOS_ADAPTER_MULTIPROCESS_GPU_NATIVE_IPC_STUB_H
#define OHOS_ADAPTER_MULTIPROCESS_GPU_NATIVE_IPC_STUB_H

#include <cstdint>
#include <functional>

#include <IPCKit/ipc_kit.h>

#include "gpu_native_ipc_common.h"
#include "native_ipc_stub.h"

namespace ohos::adapter::multiprocess {

class GpuNativeIpcStub : public NativeIpcStub {
 public:
  OH_IPC_ErrorCode HandleRemoteRequest(GpuNativeIpcOpCode code,
                                       const OHIPCParcel* data,
                                       OHIPCParcel* reply,
                                       void* user_data) override;

  static GpuNativeIpcStub& GetInstance();

 private:
  OH_IPC_ErrorCode HandleInvalidCode(GpuNativeIpcOpCode code,
                                     const OHIPCParcel* data,
                                     OHIPCParcel* reply,
                                     void* user_data);

  OH_IPC_ErrorCode HandleGetPid(GpuNativeIpcOpCode code,
                                const OHIPCParcel* data,
                                OHIPCParcel* reply,
                                void* user_data);

  OH_IPC_ErrorCode HandleInitializeWindowAdapter(GpuNativeIpcOpCode code,
                                                 const OHIPCParcel* data,
                                                 OHIPCParcel* reply,
                                                 void* user_data);

  OH_IPC_ErrorCode HandleAddWindow(GpuNativeIpcOpCode code,
                                   const OHIPCParcel* data,
                                   OHIPCParcel* reply,
                                   void* user_data);

  OH_IPC_ErrorCode HandleRemoveWindow(GpuNativeIpcOpCode code,
                                      const OHIPCParcel* data,
                                      OHIPCParcel* reply,
                                      void* user_data);

  OH_IPC_ErrorCode HandleSetWindowWidget(GpuNativeIpcOpCode code,
                                         const OHIPCParcel* data,
                                         OHIPCParcel* reply,
                                         void* user_data);

  OH_IPC_ErrorCode HandleNotifyWindowChange(GpuNativeIpcOpCode code,
                                            const OHIPCParcel* data,
                                            OHIPCParcel* reply,
                                            void* user_data);

  OH_IPC_ErrorCode AddWindowInner(const OHIPCParcel* data);

  OH_IPC_ErrorCode HandleInitializeWindowAdapterInner(GpuNativeIpcOpCode code,
                                                      const OHIPCParcel* data);

  void* GetWindowFromAdapter(const OHIPCParcel* data);

  GpuNativeIpcStub() = default;

  ~GpuNativeIpcStub() override = default;

  GpuNativeIpcStub(const GpuNativeIpcStub&);

  GpuNativeIpcStub& operator=(const GpuNativeIpcStub&);
};

}  // namespace ohos::adapter::multiprocess

#endif  // CHROMIUM_GPU_NATIVE_IPC_STUB_H
