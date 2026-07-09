/*
 * Copyright (c) 2023-2026 Huawei Device Co., Ltd.
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

#include "ohos/adapter/xcomponent/adapter/native_window_wrapper.h"

#include <native_window/external_window.h>

namespace ohos::adapter::xcomponent {

NativeWindowWrapper::NativeWindowWrapper(OHNativeWindow* window)
    : window_(window) {
  if (window_) {
    OH_NativeWindow_NativeObjectReference(window_);
  }
}

NativeWindowWrapper::~NativeWindowWrapper() {
  if (window_) {
    OH_NativeWindow_NativeObjectUnreference(window_);
  }
}

NativeWindowWrapper::NativeWindowWrapper(NativeWindowWrapper&& other) noexcept {
  window_ = std::move(other.window_);
}

NativeWindowWrapper& NativeWindowWrapper::operator=(NativeWindowWrapper&& other) noexcept {
  if (this != &other) {
    if (window_) {
      OH_NativeWindow_NativeObjectUnreference(window_);
    }
    window_ = std::move(other.window_);
  }
  return *this;
}

OHNativeWindow* NativeWindowWrapper::Get() const {
  return window_;
}

bool NativeWindowWrapper::IsValid() const {
  return window_ != nullptr;
}

}  // namespace ohos::adapter::xcomponent