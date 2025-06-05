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

#include "gpu_native_process.h"

#include <unistd.h>

#include <native_window/external_window.h>
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"

using namespace ohos::adapter::xcomponent;

namespace ohos::adapter::multiprocess {

GpuNativeProcess& GpuNativeProcess::GetInstance() {
  static GpuNativeProcess instance;
  return instance;
}

int GpuNativeProcess::GetPid() {
  return getpid();
}

void GpuNativeProcess::AddWindow(const std::string& window_id, void* window) {
  WindowAdapter::GetInstance().AddWindow(window_id, window);
}

void GpuNativeProcess::RemoveWindow(const std::string& window_id) {
  auto window = WindowAdapter::GetInstance().GetWindow(window_id);
  WindowAdapter::GetInstance().RemoveWindow(window_id);
  if (window) {
    // The native window added by the gpu process needs to be released.
    OH_NativeWindow_DestroyNativeWindow((OHNativeWindow*)window);
  }
}

void GpuNativeProcess::SetWindowWidget(void* window, int widget_id) {
  WindowAdapter::GetInstance().SetWindowWidget(window, widget_id);
}

void GpuNativeProcess::SetInitializeResult(bool result) {
  initialize_success_.set_value(result);
}

bool GpuNativeProcess::GetInitializeResult() {
  return initialize_success_.get_future().get();
}
}  // namespace ohos::adapter::multiprocess
