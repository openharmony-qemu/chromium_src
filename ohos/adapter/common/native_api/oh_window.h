/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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

#ifndef OHOS_ADAPTER_COMMON_NATIVE_API_OH_WINDOW_H_
#define OHOS_ADAPTER_COMMON_NATIVE_API_OH_WINDOW_H_

#include <window_manager/oh_window_comm.h>

#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/device_info/device_info.h"

namespace ohos::adapter::common {
extern "C" {
  int32_t OH_WindowManager_LockCursor(
      int32_t window_id,
      bool is_cursor_follow_movement) __attribute__((weak));
  int32_t OH_WindowManager_UnlockCursor(
      int32_t window_id) __attribute__((weak));
}

bool SupportsLockCursorFunc() {
  if (device_info::DeviceInfo::SdkApi() < device_info::SDK_VERSION_22) {
    LOGE(
        "[OhosNativeApi] Unsupported SDK version: %{public}d. "
        "Minimum required: %{public}d",
        device_info::DeviceInfo::SdkApi(),
        device_info::SDK_VERSION_22);
    return false;
  }
  return OH_WindowManager_LockCursor &&
         OH_WindowManager_UnlockCursor;
}

int32_t LockCursor(int32_t window_id, bool is_cursor_follow_movement) {
  if (OH_WindowManager_LockCursor) {
    return OH_WindowManager_LockCursor(window_id, is_cursor_follow_movement);
  }
  LOGE("[OhosNativeApi] OH_WindowManager_LockCursor not loaded");
  return WINDOW_MANAGER_ERRORCODE_DEVICE_NOT_SUPPORTED;
}

int32_t UnlockCursor(int32_t window_id) {
  if (OH_WindowManager_UnlockCursor) {
    return OH_WindowManager_UnlockCursor(window_id);
  }
  LOGE("[OhosNativeApi] OH_WindowManager_UnlockCursor not loaded");
  return WINDOW_MANAGER_ERRORCODE_DEVICE_NOT_SUPPORTED;
}
}  // namespace ohos::adapter::common
#endif  // OHOS_ADAPTER_COMMON_NATIVE_API_OH_WINDOW_H_

