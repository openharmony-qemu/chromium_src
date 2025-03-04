// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_MULTIPROCESS_GPU_NATIVE_IPC_STUB_H
#define OHOS_ADAPTER_MULTIPROCESS_GPU_NATIVE_IPC_STUB_H

#include <IPCKit/ipc_kit.h>

#include <cstdint>
#include <functional>

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
