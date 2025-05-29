// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu_native_process_test_utils.h"

using ohos::adapter::multiprocess::NativeApi;
using ohos::adapter::multiprocess::NativeApiMockManager;
using ohos::adapter::multiprocess::WindowAttrType;

namespace ohos::adapter::multiprocess {

NativeApiMockManager& NativeApiMockManager::GetInstance() {
  static NativeApiMockManager instance;
  return instance;
}

NativeApi* NativeApiMockManager::GetApiImpl() {
  return impl_;
}

void NativeApiMockManager::SetApiImpl(NativeApi* impl) {
  impl_ = impl;
}

OH_IPC_ErrorCode FillUpParcel(OHIPCParcel* parcel, int count) {
  for (int i = 0; i < count; i++) {
    if (OH_IPCParcel_WriteInt32(parcel, i) != OH_IPC_SUCCESS) {
      return OH_IPC_PARCEL_WRITE_ERROR;
    }
  }
  return OH_IPC_SUCCESS;
}

OH_IPC_ErrorCode CheckParcel(const OHIPCParcel* parcel, int count) {
  int val;
  for (int i = 0; i < count; i++) {
    OH_IPCParcel_ReadInt32(parcel, &val);
    GTEST_COUT << "i = " << i << ", val = " << val << std::endl;
    if (val != i) {
      return OH_IPC_PARCEL_READ_ERROR;
    }
  }
  return OH_IPC_SUCCESS;
}

void WriteWindowIntoParcel(OHIPCParcel* parcel, int id) {
  std::string window_id = "browser" + std::to_string(id);
  EXPECT_EQ(OH_IPC_SUCCESS,
            OH_IPCParcel_WriteString(parcel, window_id.c_str()));
  EXPECT_EQ(OH_IPC_SUCCESS, OH_IPCParcel_WriteInt32(parcel, 1440));
  EXPECT_EQ(OH_IPC_SUCCESS, OH_IPCParcel_WriteInt32(parcel, 2560));
  EXPECT_EQ(OH_IPC_SUCCESS,
            FillUpParcel(parcel, WindowAttrType::MAX_TYPE_COUNT));
  EXPECT_EQ(OH_IPC_SUCCESS, OH_IPCParcel_WriteString(parcel, "AppFramework1"));
}

void WriteWindowsIntoParcel(OHIPCParcel* parcel, int count) {
  EXPECT_EQ(OH_IPC_SUCCESS, OH_IPCParcel_WriteInt32(parcel, count));
  for (int i = 0; i < count; i++) {
    WriteWindowIntoParcel(parcel, i);
  }
}
}  // namespace ohos::adapter::multiprocess

// Native API mock start

int NativeWindowActionToAttr(int op_code) {
  switch (op_code) {
    case GET_FORMAT:
    case SET_FORMAT:
      return WindowAttrType::FORMAT;
    case GET_USAGE:
    case SET_USAGE:
      return WindowAttrType::USAGE;
    case GET_STRIDE:
    case SET_STRIDE:
      return WindowAttrType::STRIDE;
    case GET_SWAP_INTERVAL:
    case SET_SWAP_INTERVAL:
      return WindowAttrType::SWAP_INTERVAL;
    case GET_TIMEOUT:
    case SET_TIMEOUT:
      return WindowAttrType::TIMEOUT;
    case GET_COLOR_GAMUT:
    case SET_COLOR_GAMUT:
      return WindowAttrType::COLOR_GAMUT;
    case GET_TRANSFORM:
    case SET_TRANSFORM:
      return WindowAttrType::TRANSFORM;
    case GET_SOURCE_TYPE:
    case SET_SOURCE_TYPE:
      return WindowAttrType::SOURCE_TYPE;
    default:
      // Just testing a sub-set of operations is enough.
      GTEST_COUT << "Unknown op code: " << op_code;
      return WindowAttrType::MAX_TYPE_COUNT;
  }
}

int OH_NativeWindow_NativeWindowHandleOpt(NativeWindow* window,
                                          int op_code,
                                          ...) {
  NativeApi* api = NativeApiMockManager::GetInstance().GetApiImpl();
  va_list args;
  va_start(args, op_code);
  int ret;
  switch (op_code) {
    case GET_BUFFER_GEOMETRY:
      ret = api->GetBufferGeometry(window, op_code, va_arg(args, int*),
                                   va_arg(args, int*));
      break;
    case SET_BUFFER_GEOMETRY:
      ret = api->SetBufferGeometry(window, op_code, va_arg(args, int),
                                   va_arg(args, int));
      break;
    case GET_FORMAT:
    case GET_USAGE:
    case GET_STRIDE:
    case GET_SWAP_INTERVAL:
    case GET_TIMEOUT:
    case GET_COLOR_GAMUT:
    case GET_TRANSFORM:
    case GET_SOURCE_TYPE:
      ret = api->GetInt32Attr(window, NativeWindowActionToAttr(op_code),
                              va_arg(args, int*));
      break;
    case SET_FORMAT:
    case SET_USAGE:
    case SET_STRIDE:
    case SET_SWAP_INTERVAL:
    case SET_TIMEOUT:
    case SET_COLOR_GAMUT:
    case SET_TRANSFORM:
    case SET_SOURCE_TYPE:
      ret = api->SetInt32Attr(window, NativeWindowActionToAttr(op_code),
                              va_arg(args, int));
      break;
    case GET_APP_FRAMEWORK_TYPE:
      ret = api->GetStringAttr(window, op_code, va_arg(args, char*));
      break;
    case SET_APP_FRAMEWORK_TYPE:
      ret = api->SetStringAttr(window, op_code, va_arg(args, char*));
      break;
    default:
      // Just testing a sub-set of operations is enough.
      GTEST_COUT << "Operation not covered: " << op_code;
      return 0;
  }
  va_end(args);
  return ret;
}

int OH_NativeWindow_WriteToParcel(NativeWindow* window, OHIPCParcel* parcel) {
  NativeApi* api = NativeApiMockManager::GetInstance().GetApiImpl();
  EXPECT_NE(nullptr, api);
  return api->MockNativeWindowWriteToParcel(window, parcel);
}

int OH_NativeWindow_ReadFromParcel(OHIPCParcel* parcel, NativeWindow** window) {
  NativeApi* api = NativeApiMockManager::GetInstance().GetApiImpl();
  EXPECT_NE(nullptr, api);
  return api->MockNativeWindowReadFromParcel(parcel, window);
}

int OH_IPCRemoteProxy_SendRequest(const OHIPCRemoteProxy* proxy,
                                  uint32_t code,
                                  const OHIPCParcel* data,
                                  OHIPCParcel* reply,
                                  const OH_IPC_MessageOption* option) {
  NativeApi* api = NativeApiMockManager::GetInstance().GetApiImpl();
  EXPECT_NE(nullptr, api);
  return api->IpcRemoteProxySendRequest(proxy, code, data, reply, option);
}

void OH_IPCRemoteProxy_Destroy(OHIPCRemoteProxy* remote_proxy) {
  NativeApi* api = NativeApiMockManager::GetInstance().GetApiImpl();
  EXPECT_NE(nullptr, api);
  api->IpcRemoteProxyDestroy(remote_proxy);
}

int OH_Ability_CreateNativeChildProcess(
    const char* lib_name,
    OH_Ability_OnNativeChildProcessStarted on_process_started) {
  NativeApi* api = NativeApiMockManager::GetInstance().GetApiImpl();
  EXPECT_NE(nullptr, api);
  return api->CreateNativeChildProcess(lib_name, on_process_started);
}
