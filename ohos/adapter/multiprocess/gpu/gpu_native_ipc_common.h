// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_MULTIPROCESS_GPU_NATIVE_IPC_COMMON_H
#define OHOS_ADAPTER_MULTIPROCESS_GPU_NATIVE_IPC_COMMON_H

#include <cstdint>

namespace ohos::adapter::multiprocess {

#define GPU_IPC_DESCRIPTOR "chromium.native.gpu"

/**
 * GPU native IPC operation codes,
 * used for indexing in a function array,
 * must be declared consecutively.
 */
enum class GpuNativeIpcOpCode : uint32_t {
  INVALID_OPERATION_CODE,
  /** Start operation code. */
  GET_PID,
  INITIALIZE_WINDOW_ADAPTER,
  ADD_WINDOW,
  REMOVE_WINDOW,
  SET_WINDOW_WIDGET,
  NOTIFY_WINDOW_CHANGE,
  /** Maximum operation code. */
  MAX_OPERATION_CODE
};
}  // namespace ohos::adapter::multiprocess

#endif  // OHOS_ADAPTER_MULTIPROCESS_GPU_NATIVE_IPC_COMMON_H
