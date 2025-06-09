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

#ifndef OHOS_ADAPTER_MULTIPROCESS_NATIVE_IPC_PROXY_H
#define OHOS_ADAPTER_MULTIPROCESS_NATIVE_IPC_PROXY_H

#include <memory>
#include <string>

#include <IPCKit/ipc_kit.h>

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
