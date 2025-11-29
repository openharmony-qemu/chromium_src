// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_MULTIPROCESS_NATIVE_IPC_STUB_H
#define OHOS_ADAPTER_MULTIPROCESS_NATIVE_IPC_STUB_H

#include <IPCKit/ipc_kit.h>

#include "gpu_native_ipc_common.h"

namespace ohos::adapter::multiprocess {
class NativeIpcStub {
 public:
  virtual OH_IPC_ErrorCode HandleRemoteRequest(GpuNativeIpcOpCode code,
                                               const OHIPCParcel* data,
                                               OHIPCParcel* reply,
                                               void* user_data) = 0;

  void Initialize(OHIPCRemoteStub* ipc_remote_stub);

  virtual ~NativeIpcStub();

 protected:
  OHIPCRemoteStub* ipc_remote_stub_;
};
}  // namespace ohos::adapter::multiprocess

#endif  // OHOS_ADAPTER_MULTIPROCESS_NATIVE_IPC_STUB_H
