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

#include "ohos/adapter/window/system_floating_window_adapter.h"

#include <chrono>
#include <future>

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/logging.h"

namespace ohos::adapter::window {

SystemFloatingWindowAdapter& SystemFloatingWindowAdapter::GetInstance() {
  static SystemFloatingWindowAdapter instance;
  return instance;
}

void SystemFloatingWindowAdapter::Create(const NewWindowParam& param) {
  auto jsFunc =
      ohos::adapter::GetJSFunction("SystemFloatingWindow.CreateWindow");
  if (jsFunc) {
    jsFunc->Invoke<void>(param);
  }
}

void SystemFloatingWindowAdapter::Close(int32_t id) {
  auto jsFunc =
      ohos::adapter::GetJSFunction("SystemFloatingWindow.CloseWindow");
  if (jsFunc) {
    jsFunc->Invoke<void>(id);
  }
}

void SystemFloatingWindowAdapter::SetBounds(int32_t id,
                                            const WindowRect& rect) {
  std::promise<bool> promise;
  std::function<void(bool)> on_completed = [&promise](bool successful) -> void {
    promise.set_value(successful);
  };
  auto jsFunc = ohos::adapter::GetJSFunction("SystemFloatingWindow.SetBounds");
  if (jsFunc) {
    jsFunc->Invoke<void>(id, rect, on_completed);
    auto future = promise.get_future();
    auto status = future.wait_for(std::chrono::seconds(3));
    if (status == std::future_status::timeout) {
      LOGE("SystemFloatingWindow.SetBounds timeout");
      return;
    }
    bool successful = future.get();
    if (!successful) {
      LOGE("SystemFloatingWindow.SetBounds failed");
    }
  }
}

void SystemFloatingWindowAdapter::SetWindowLimits(int32_t min_width,
                                                  int32_t min_height,
                                                  int32_t max_width,
                                                  int32_t max_height,
                                                  int32_t id) {
  auto jsFunc =
      ohos::adapter::GetJSFunction("SystemFloatingWindow.SetWindowLimits");
  if (jsFunc) {
    jsFunc->Invoke<void>(min_width, min_height, max_width, max_height, id);
  }
}

void SystemFloatingWindowAdapter::StartWindowMoving(int32_t id) {
  auto jsFunc = ohos::adapter::GetJSFunction("SystemFloatingWindow.StartWindowMoving");
  if (jsFunc) {
    jsFunc->Invoke<void>(id);
  }
}

}  // namespace ohos::adapter::window
