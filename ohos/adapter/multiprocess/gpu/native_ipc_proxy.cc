// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
