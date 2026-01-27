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
#include "ohos/adapter/cursor/cursor.h"

#include <window_manager/oh_window_comm.h>

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/common/native_api/oh_window.h"
#include "ohos/adapter/common/native_api/ui_input_event.h"
#include "ohos/adapter/common/trace.h"
#include "ohos/adapter/window/app_window_adapter.h"

namespace ohos::adapter {
constexpr int IMAGE_PIXEL_MAP = 4;

Cursor& Cursor::GetInstance() {
  static Cursor instance;
  return instance;
}

void Cursor::SetCursorVisible(const bool visible) {
  if (auto func =
          ohos::adapter::GetJSFunction("CursorAdapter.SetCursorVisible")) {
    func->InvokeAsync<void>(visible);
  }
}

void Cursor::SetCursor(int32_t id, const OhosCursorType cursor_type) {
  if (cursor_type == ohos::adapter::OhosCursorType::kNone) {
    SetCursorVisible(false);
  } else {
    if (auto func = ohos::adapter::GetJSFunction("CursorAdapter.SetCursor")) {
      SetCursorVisible(true);
      func->InvokeAsync<void>(id, cursor_type);
    }
  }
}

void Cursor::SetCustomCursor(CustomCusorInfo& cursor_info) {
  if (auto func = ohos::adapter::GetJSFunction("CursorAdapter.SetCustomCursor")) {
    SetCursorVisible(true);
    aki::ArrayBuffer arrayBuffer(
        reinterpret_cast<uint8_t*>(cursor_info.buff.get()),
        cursor_info.width * cursor_info.height * IMAGE_PIXEL_MAP);
    func->Invoke<void>(cursor_info.id, cursor_info.width, cursor_info.height,
                       cursor_info.hotspot_x, cursor_info.hotspot_y,
                       std::move(arrayBuffer));
  }
}

bool Cursor::LockCursor(int32_t widget_id) {
  std::vector<int32_t> window_ids =
      window::AppWindowAdapter::GetInstance().GetOriginWindowIds({widget_id});
  if (window_ids.empty()) {
    LOGE("[OhosCursorLock] GetOriginWindowIds failed");
    return false;
  }
  int32_t result = common::LockCursor(window_ids[0], false);
  if (result != OK) {
    LOGE(
        "[OhosCursorLock] "
        "OH_WindowManager_LockCursor error code: %{public}d", result);
    return false;
  }
  cursor_locked_ = true;
  return true;
}

bool Cursor::UnlockCursor(int32_t widget_id) {
  if (!cursor_locked_) {
    LOGE("[OhosCursorLock] Cursor is not locked");
    return false;
  }
  std::vector<int32_t> window_ids =
      window::AppWindowAdapter::GetInstance().GetOriginWindowIds({widget_id});
  if (window_ids.empty()) {
    LOGE("[OhosCursorLock] GetOriginWindowIds failed");
    return false;
  }
  int32_t result = common::UnlockCursor(window_ids[0]);
  // When the system function returns WINDOW_MANAGER_ERRORCODE_STATE_ABNORMAL,
  // system will exit the pointer lock state, in this case we should unlock to
  // match system behavior.
  if (result != OK &&
      result != WINDOW_MANAGER_ERRORCODE_STATE_ABNORMAL) {
    LOGE(
        "[OhosCursorLock] "
        "OH_WindowManager_UnlockCursor error code: %{public}d", result);
    return false;
  }
  cursor_locked_ = false;
  return true;
}

bool Cursor::SupportsCursorLock() {
  return common::SupportsLockCursorFunc() &&
         common::SupportsGetRawDeltaFunc();
}

bool Cursor::IsCursorLocked() {
  return cursor_locked_;
}

}  // namespace ohos::adapter
