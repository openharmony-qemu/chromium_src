/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "window_manager_adapter_impl.h"

#include <dlfcn.h>
#include <window_manager/oh_window.h>
#include "arkweb/ohos_nweb/src/nweb_hilog.h"

namespace OHOS::NWeb {
using OHWindowManagerLockCursor = int32_t (*)(int32_t windowId, bool isCursorFollowMovement);
using OHWindowManagerUnLockCursor = int32_t (*)(int32_t windowId);

WindowManagerAdapterImpl& WindowManagerAdapterImpl::GetInstance()
{
    WVLOG_D("Window manager adapter impl get instance.");
    static WindowManagerAdapterImpl instance;
    return instance;
}

bool WindowManagerAdapterImpl::GetWindowPrivacyMode(uint32_t windowId)
{
    WindowManager_WindowProperties windowProperties;
    int32_t ret = OH_WindowManager_GetWindowProperties(windowId, &windowProperties);
    if (ret != 0) {
        WVLOG_E("Window manager get window properties failed ret = %{public}d.", ret);
        return false;
    }
    return windowProperties.isPrivacyMode;
}

int32_t WindowManagerAdapterImpl::LockPointer(int32_t windowId) {
#if defined(__aarch64__)
  void* dl = dlopen("/system/lib64/ndk/libnative_window_manager.so", RTLD_LAZY);
#else
  void* dl = dlopen("/system/lib/ndk/libnative_window_manager.so", RTLD_LAZY);
#endif

  if (dl == nullptr) {
    WVLOG_W("cannot find the Window's path");
    return WindowManager_ErrorCode::OK;
  }
  OHWindowManagerLockCursor set_lock_curosr = nullptr;
  set_lock_curosr = reinterpret_cast<OHWindowManagerLockCursor>(
      dlsym(dl, "OH_WindowManager_LockCursor"));
  if (set_lock_curosr == nullptr) {
    WVLOG_W("cannot find the function called OH_WindowManager_LockCursor");
    return WindowManager_ErrorCode::OK;
  }
  int32_t res = set_lock_curosr(windowId, false);
  WVLOG_I("LockPointer result code is %{public}d.", res);
  return res;
}

int32_t WindowManagerAdapterImpl::UnlockPointer(int32_t windowId) {
#if defined(__aarch64__)
  void* dl = dlopen("/system/lib64/ndk/libnative_window_manager.so", RTLD_LAZY);
#else
  void* dl = dlopen("/system/lib/ndk/libnative_window_manager.so", RTLD_LAZY);
#endif

  OHWindowManagerUnLockCursor set_unlock_curosr = nullptr;
  if (dl == nullptr) {
    WVLOG_W("cannot find the Window's path");
    return WindowManager_ErrorCode::OK;
  }

  set_unlock_curosr = reinterpret_cast<OHWindowManagerUnLockCursor>(
      dlsym(dl, "OH_WindowManager_UnlockCursor"));
  if (set_unlock_curosr == nullptr) {
    WVLOG_W("cannot find the function called OH_WindowManager_UnlockCursor");
    return WindowManager_ErrorCode::OK;
  }
  int32_t res = set_unlock_curosr(windowId);
  WVLOG_I("UnLockPointer result code is %{public}d.", res);
  return res;
}
} // namespace OHOS::NWeb