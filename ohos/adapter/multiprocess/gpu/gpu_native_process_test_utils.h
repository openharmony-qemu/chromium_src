// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_MUILTIPROCESS_GPU_GPU_NATIVE_NATIVE_PROCESS_TEST_UTILS_H
#define OHOS_ADAPTER_MUILTIPROCESS_GPU_GPU_NATIVE_NATIVE_PROCESS_TEST_UTILS_H

#include <AbilityKit/native_child_process.h>

#include <functional>
#include <unordered_map>
#include <vector>

#include "gmock/gmock.h"
#include "native_window_parcel.h"

#define GTEST_COUT std::cout << "[          ] [ INFO ] "

struct NativeWindow {
  int width;
  int height;
  std::vector<int> attrs;
  char* app_framework;
};

struct OHIPCRemoteProxy {};

namespace ohos::adapter::multiprocess {

// Attributes need to write into parcel.
// Needs to change when nativeWindowGetInt32Ops or
// nativeWindowSetInt32Ops changed,
// and the order should be the same.
enum WindowAttrType : int {
  FORMAT,
  USAGE,
  STRIDE,
  SWAP_INTERVAL,
  TIMEOUT,
  COLOR_GAMUT,
  TRANSFORM,
  SOURCE_TYPE,
  MAX_TYPE_COUNT,
};

class NativeApi {
 public:
  virtual int GetBufferGeometry(NativeWindow* window,
                                int op_code,
                                int* width,
                                int* height) = 0;
  virtual int SetBufferGeometry(NativeWindow* window,
                                int op_code,
                                int width,
                                int height) = 0;
  virtual int GetInt32Attr(NativeWindow* window, int type, int* val) = 0;
  virtual int SetInt32Attr(NativeWindow* window, int type, int val) = 0;
  virtual int GetStringAttr(NativeWindow* window, int type, char* str) = 0;
  virtual int SetStringAttr(NativeWindow* window, int type, char* str) = 0;
  virtual int MockNativeWindowWriteToParcel(NativeWindow* window,
                                            OHIPCParcel* data_parcel) = 0;
  virtual int MockNativeWindowReadFromParcel(OHIPCParcel* data_parcel,
                                             NativeWindow** window) = 0;
  virtual int IpcRemoteProxySendRequest(const OHIPCRemoteProxy* proxy,
                                        uint32_t code,
                                        const OHIPCParcel* data,
                                        OHIPCParcel* reply,
                                        const OH_IPC_MessageOption* option) = 0;
  virtual void IpcRemoteProxyDestroy(OHIPCRemoteProxy* remote_proxy) = 0;
  virtual int CreateNativeChildProcess(
      const char* lib_name,
      OH_Ability_OnNativeChildProcessStarted onProcessStarted) = 0;
};

class NativeMockApi : public NativeApi {
 public:
  MOCK_METHOD4(GetBufferGeometry, int(NativeWindow*, int, int*, int*));
  MOCK_METHOD4(SetBufferGeometry, int(NativeWindow*, int, int, int));
  MOCK_METHOD3(GetInt32Attr, int(NativeWindow*, int, int*));
  MOCK_METHOD3(SetInt32Attr, int(NativeWindow*, int, int));
  MOCK_METHOD3(GetStringAttr, int(NativeWindow*, int, char*));
  MOCK_METHOD3(SetStringAttr, int(NativeWindow*, int, char*));
  MOCK_METHOD2(MockNativeWindowWriteToParcel, int(NativeWindow*, OHIPCParcel*));
  MOCK_METHOD2(MockNativeWindowReadFromParcel,
               int(OHIPCParcel*, NativeWindow**));
  MOCK_METHOD5(IpcRemoteProxySendRequest,
               int(const OHIPCRemoteProxy*,
                   uint32_t,
                   const OHIPCParcel*,
                   OHIPCParcel*,
                   const OH_IPC_MessageOption*));
  MOCK_METHOD1(IpcRemoteProxyDestroy, void(OHIPCRemoteProxy*));
  MOCK_METHOD2(CreateNativeChildProcess,
               int(const char*, OH_Ability_OnNativeChildProcessStarted));
};

class NativeApiMockManager {
 public:
  static NativeApiMockManager& GetInstance();
  NativeApi* GetApiImpl();
  void SetApiImpl(NativeApi* impl);

 private:
  NativeApi* impl_{nullptr};
};

OH_IPC_ErrorCode FillUpParcel(OHIPCParcel* parcel, int count);
OH_IPC_ErrorCode CheckParcel(const OHIPCParcel* parcel, int count);
void WriteWindowIntoParcel(OHIPCParcel* parcel, int id);
void WriteWindowsIntoParcel(OHIPCParcel* parcel, int count);
}  // namespace ohos::adapter::multiprocess
#endif  // OHOS_ADAPTER_MUILTIPROCESS_GPU_GPU_NATIVE_NATIVE_PROCESS_TEST_UTILS_H
