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

#include "ohos/adapter/window/sub_window_adapter.h"

#include <chrono>
#include <cstdint>
#include <future>

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/common/trace.h"

namespace ohos::adapter::window {

SubWindowAdapter& SubWindowAdapter::GetInstance() {
  static SubWindowAdapter helper;
  return helper;
}

std::string SubWindowAdapter::ReuseSubWindow(const NewWindowParam& param) {
  if (auto func = ohos::adapter::GetJSFunction("SubWindow.ReuseSubWindow")) {
    return func->Invoke<std::string>(param);
  }
  return std::string();
}
 
void SubWindowAdapter::Create(const NewWindowParam& param) {
  TRACE_EVENT_0("SubWindowAdapter::Create");
  if (auto func = ohos::adapter::GetJSFunction("SubWindow.Create")) {
    func->Invoke<void>(param);
    return;
  }
  LOGE("[ohoswindow] SubWindowAdapter::Create %{public}s error.", param.window_id.c_str());
}

void SubWindowAdapter::Cancel(const std::string& id) {
  if (auto func = ohos::adapter::GetJSFunction("SubWindow.Cancel")) {
    func->Invoke<void>(id);
  }
}

void SubWindowAdapter::Show(const std::string& id) {
  if (auto func = ohos::adapter::GetJSFunction("SubWindow.Show")) {
    func->Invoke<void>(id);
    return;
  }
  LOGW("[ohoswindow] SubWindowAdapter::Show %{public}s error.", id.c_str());
}

void SubWindowAdapter::Hide(const std::string& id) {
  if (auto func = ohos::adapter::GetJSFunction("SubWindow.Hide")) {
    func->Invoke<void>(id);
  }
}

void SubWindowAdapter::SetBounds(const std::string& id, const WindowRect& rect) {
  if (auto func = ohos::adapter::GetJSFunction("SubWindow.SetBounds")) {
    func->Invoke<void>(id, rect);
  }
}
}  // namespace ohos::adapter::window
