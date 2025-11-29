// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "native_ipc_stub.h"

namespace ohos::adapter::multiprocess {
void NativeIpcStub::Initialize(OHIPCRemoteStub* ipc_remote_stub) {
  ipc_remote_stub_ = ipc_remote_stub;
}

NativeIpcStub::~NativeIpcStub() {
  if (ipc_remote_stub_ != nullptr) {
    OH_IPCRemoteStub_Destroy(ipc_remote_stub_);
    ipc_remote_stub_ = nullptr;
  }
}
}  // namespace ohos::adapter::multiprocess
