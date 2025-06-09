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
#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/trace.h"

namespace ohos::adapter {

constexpr int IMAGE_PIXEL_MAP = 4;

void SetCursorVisible(const bool visiable) {
  if (auto func =
          ohos::adapter::GetJSFunction("CursorAdapter.SetCursorVisible")) {
    func->InvokeAsync<void>(visiable);
  }
}

void SetCursor(int32_t id, const OhosCursorType cursor_type) {
  if (cursor_type == ohos::adapter::OhosCursorType::kNone) {
    SetCursorVisible(false);
  } else {
    if (auto func = ohos::adapter::GetJSFunction("CursorAdapter.SetCursor")) {
      SetCursorVisible(true);
      func->InvokeAsync<void>(id, cursor_type);
    }
  }
}

void SetCustomCursor(CustomCusorInfo& cursor_info) {
  if (auto func = ohos::adapter::GetJSFunction("CursorAdapter.SetCustomCursor")) {
    aki::ArrayBuffer arrayBuffer(
        reinterpret_cast<uint8_t*>(cursor_info.buff.get()),
        cursor_info.width * cursor_info.height * IMAGE_PIXEL_MAP);
    func->Invoke<void>(cursor_info.id, cursor_info.width, cursor_info.height,
                       cursor_info.hotspot_x, cursor_info.hotspot_y,
                       std::move(arrayBuffer));
  }
}

}  // namespace ohos::adapter
