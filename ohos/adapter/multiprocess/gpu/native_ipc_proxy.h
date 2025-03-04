// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_MULTIPROCESS_NATIVE_IPC_PROXY_H
#define OHOS_ADAPTER_MULTIPROCESS_NATIVE_IPC_PROXY_H

#include <IPCKit/ipc_kit.h>

#include <memory>
#include <string>

#include "ohos/adapter/common/logging.h"

namespace ohos::adapter::multiprocess {
class NativeIpcRequest {
 public:
  virtual OH_IPC_ErrorCode Run(const OHIPCRemoteProxy* remote_proxy) = 0;

  virtual std::string ToString() const = 0;

  OH_IPC_ErrorCode Prepare();

  NativeIpcRequest() = default;

  virtual ~NativeIpcRequest();

 protected:
  OHIPCParcel* data_parcel_{nullptr};
  OHIPCParcel* reply_parcel_{nullptr};
};

class NativeIpcProxy {
 public:
  explicit NativeIpcProxy(OHIPCRemoteProxy* remote_proxy)
      : remote_proxy_(remote_proxy) {}

  virtual ~NativeIpcProxy();

 protected:
  OHIPCRemoteProxy* remote_proxy_;

  OH_IPC_ErrorCode PerformRequest(std::unique_ptr<NativeIpcRequest> request);
};
}  // namespace ohos::adapter::multiprocess
#endif  // OHOS_ADAPTER_MULTIPROCESS_NATIVE_IPC_PROXY_H
