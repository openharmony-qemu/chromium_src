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

#include "native_window_parcel.h"

#include <cstdint>

namespace ohos::adapter::multiprocess {

static NativeWindowOperation nativeWindowGetInt32Ops[] = {
    GET_FORMAT,
    GET_USAGE,
    GET_STRIDE,
    GET_SWAP_INTERVAL,
    GET_TIMEOUT,
    GET_COLOR_GAMUT,
    GET_TRANSFORM,
    GET_SOURCE_TYPE
};

static NativeWindowOperation nativeWindowSetInt32Ops[] = {
    SET_FORMAT,
    SET_USAGE,
    SET_STRIDE,
    SET_SWAP_INTERVAL,
    SET_TIMEOUT,
    SET_COLOR_GAMUT,
    SET_TRANSFORM,
    SET_SOURCE_TYPE
};

static int WriteInt32AttrToParcel(NativeWindowOperation op,
                                  void* window,
                                  OHIPCParcel* data_parcel) {
  int32_t val;
  int ret =
      OH_NativeWindow_NativeWindowHandleOpt((OHNativeWindow*)window, op, &val);
  if (ret != 0) {
    LOGE("Native window %{public}d error: %{public}d", op, ret);
    return OH_IPC_PARCEL_WRITE_ERROR;
  }
  ret = OH_IPCParcel_WriteInt32(data_parcel, val);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("Native window write %{public}d into parcel error: %{public}d", op,
         ret);
    return ret;
  }

  return 0;
}

static int ReadInt32AttrFromParcel(NativeWindowOperation op,
                                   void* window,
                                   const OHIPCParcel* data_parcel) {
  int32_t val;
  int ret = OH_IPCParcel_ReadInt32(data_parcel, &val);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("Native window read %{public}d from parcel error: %{public}d", op,
         ret);
    return OH_IPC_PARCEL_READ_ERROR;
  }

  ret = OH_NativeWindow_NativeWindowHandleOpt((OHNativeWindow*)window, op, val);
  if (ret != 0) {
    LOGE("Native window %{public}d error: %{public}d", op, ret);
    return ret;
  }

  return 0;
}

int NativeWindowAttrToParcel(void* window, OHIPCParcel* data_parcel) {
  int32_t height;
  int32_t width;
  int ret = OH_NativeWindow_NativeWindowHandleOpt(
      (OHNativeWindow*)window, GET_BUFFER_GEOMETRY, &height, &width);
  if (ret != 0) {
    LOGE("Native window GET_BUFFER_GEOMETRY error: %{public}d", ret);
    return OH_IPC_PARCEL_WRITE_ERROR;
  }

  ret = OH_IPCParcel_WriteInt32(data_parcel, height);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("Native window write height into parcel error: %{public}d", ret);
    return ret;
  }
  ret = OH_IPCParcel_WriteInt32(data_parcel, width);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("Native window write width into parcel error: %{public}d", ret);
    return ret;
  }

  for (NativeWindowOperation op : nativeWindowGetInt32Ops) {
    if (WriteInt32AttrToParcel(op, window, data_parcel) != 0) {
      return OH_IPC_PARCEL_WRITE_ERROR;
    }
  }

  char app_framework_type[64] = {'\0'};
  ret = OH_NativeWindow_NativeWindowHandleOpt(
      (OHNativeWindow*)window, GET_APP_FRAMEWORK_TYPE, app_framework_type);
  if (ret != 0) {
    LOGE("Native window %{public}d error: %{public}d", GET_APP_FRAMEWORK_TYPE,
         ret);
    return OH_IPC_PARCEL_WRITE_ERROR;
  }
  ret = OH_IPCParcel_WriteString(data_parcel, app_framework_type);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("Native window write %{public}d into parcel error: %{public}d",
         GET_APP_FRAMEWORK_TYPE, ret);
    return ret;
  }

  return OH_IPC_SUCCESS;
}

int NativeWindowAttrFromParcel(const OHIPCParcel* data_parcel, void* window) {
  int32_t height;
  int32_t width;
  int ret = OH_IPCParcel_ReadInt32(data_parcel, &height);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("Native window read height from parcel error: %{public}d", ret);
    return static_cast<OH_IPC_ErrorCode>(ret);
  }

  ret = OH_IPCParcel_ReadInt32(data_parcel, &width);
  if (ret != OH_IPC_SUCCESS) {
    LOGE("Native window read width from parcel error: %{public}d", ret);
    return static_cast<OH_IPC_ErrorCode>(ret);
  }

  ret = OH_NativeWindow_NativeWindowHandleOpt(
      (OHNativeWindow*)window, SET_BUFFER_GEOMETRY, width, height);
  if (ret != 0) {
    LOGE("Native window set window size error: %{public}d", ret);
    return OH_IPC_PARCEL_READ_ERROR;
  }

  for (NativeWindowOperation op : nativeWindowSetInt32Ops) {
    if (ReadInt32AttrFromParcel(op, window, data_parcel) != 0) {
      return OH_IPC_PARCEL_READ_ERROR;
    }
  }

  const char* app_framework_type = OH_IPCParcel_ReadString(data_parcel);
  if (app_framework_type == nullptr) {
    LOGE("Native window read appFrameworkType from parcel failed.");
    return OH_IPC_PARCEL_READ_ERROR;
  }
  ret = OH_NativeWindow_NativeWindowHandleOpt(
      (OHNativeWindow*)window, SET_APP_FRAMEWORK_TYPE, app_framework_type);
  if (ret != 0) {
    LOGE("Native window %{public}d error: %{public}d", SET_APP_FRAMEWORK_TYPE,
         ret);
    return OH_IPC_PARCEL_READ_ERROR;
  }

  return OH_IPC_SUCCESS;
}

int NativeWindowWriteToParcel(void* window, OHIPCParcel* data_parcel) {
  int ret = OH_NativeWindow_WriteToParcel((OHNativeWindow*)window, data_parcel);
  if (ret != 0) {
    LOGE("Write native window into data parcel error: %{public}d", ret);
    return OH_IPC_PARCEL_WRITE_ERROR;
  }

  if (NativeWindowAttrToParcel(window, data_parcel) != OH_IPC_SUCCESS) {
    LOGE("Native window write attr to parcel error.");
    return OH_IPC_PARCEL_WRITE_ERROR;
  }

  return OH_IPC_SUCCESS;
}

int NativeWindowReadFromParcel(const OHIPCParcel* data_parcel, void** window) {
  OHNativeWindow* window_tmp;
  int32_t ret = OH_NativeWindow_ReadFromParcel(
      const_cast<OHIPCParcel*>(data_parcel), &window_tmp);
  if (ret != 0 || window_tmp == nullptr) {
    LOGE(
        "Native window read from parcel error: %{public}d, "
        "pointer is null? %{public}d",
        ret, window_tmp == nullptr);
    return OH_IPC_PARCEL_READ_ERROR;
  }

  if (NativeWindowAttrFromParcel(data_parcel, window_tmp) != OH_IPC_SUCCESS) {
    LOGE("Native window setup attr from parcel error.");
    return OH_IPC_PARCEL_READ_ERROR;
  }

  *window = window_tmp;

  return OH_IPC_SUCCESS;
}
}  // namespace ohos::adapter::multiprocess
