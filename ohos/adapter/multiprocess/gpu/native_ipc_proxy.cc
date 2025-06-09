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

#include "native_ipc_proxy.h"

#include "ohos/adapter/common/logging.h"

namespace ohos::adapter::multiprocess {
NativeIpcRequest::~NativeIpcRequest() {
  if (data_parcel_ != nullptr) {
    OH_IPCParcel_Destroy(data_parcel_);
    data_parcel_ = nullptr;
  }
  if (reply_parcel_ != nullptr) {
    OH_IPCParcel_Destroy(reply_parcel_);
    reply_parcel_ = nullptr;
  }
}

OH_IPC_ErrorCode NativeIpcRequest::Prepare() {
  OHIPCParcel* data_parcel = OH_IPCParcel_Create();
  if (data_parcel == nullptr) {
    LOGE("Create data parcel failed.");
    return OH_IPC_MEM_ALLOCATOR_ERROR;
  }

  OHIPCParcel* reply_parcel = OH_IPCParcel_Create();
  if (reply_parcel == nullptr) {
    LOGE("Create data parcel failed.");
    OH_IPCParcel_Destroy(data_parcel);
    data_parcel = nullptr;
    return OH_IPC_MEM_ALLOCATOR_ERROR;
  }

  data_parcel_ = data_parcel;
  reply_parcel_ = reply_parcel;

  return OH_IPC_SUCCESS;
}

NativeIpcProxy::~NativeIpcProxy() {
  if (remote_proxy_ != nullptr) {
    OH_IPCRemoteProxy_Destroy(remote_proxy_);
    remote_proxy_ = nullptr;
  }
}

OH_IPC_ErrorCode NativeIpcProxy::PerformRequest(
    std::unique_ptr<NativeIpcRequest> request) {
  if (remote_proxy_ == nullptr) {
    LOGE("OHIPCRemoteProxy is null.");
    return OH_IPC_INNER_ERROR;
  }
  if (request == nullptr) {
    LOGE("Request is null.");
    return OH_IPC_CHECK_PARAM_ERROR;
  }

  OH_IPC_ErrorCode error = request->Prepare();
  if (error != OH_IPC_SUCCESS) {
    LOGE("Prepare native IPC request error: %{public}d", error);
    return error;
  }

  return request->Run(remote_proxy_);
}
}  // namespace ohos::adapter::multiprocess
