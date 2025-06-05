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

#include "ohos/adapter/native_theme/native_theme_adapter.h"

#include "ohos/adapter/aki_hook/aki_hook.h"

namespace ohos::adapter::native_theme {

NativeThemeAdapter& NativeThemeAdapter::GetInstance() {
  static NativeThemeAdapter instance;
  return instance;
}

OhosColorMode NativeThemeAdapter::GetSystemThemeSource() {
  if (auto jsFuncSystemNativeThemeList = ohos::adapter::GetJSFunction(
          "NativeThemeAdapter.GetSystemNativeTheme")) {
    return jsFuncSystemNativeThemeList->Invoke<OhosColorMode>();
  }
  return OhosColorMode::COLOR_MODE_NOT_SET;
}

void NativeThemeAdapter::SetAppColorMode(OhosColorMode color_mode) {
  auto jsFunc =
      ohos::adapter::GetJSFunction("NativeThemeAdapter.SetAppColorMode");
  if (jsFunc) {
    jsFunc->Invoke<void>(color_mode);
  }
}

void SetThemeSource(const OhosColorMode theme_source) {
  if (theme_source != OhosColorMode::COLOR_MODE_NOT_SET) {
    NativeThemeAdapter::GetInstance().NotifyThemeSourceEvent(theme_source);
  }
}

void NativeThemeAdapter::RegisterThemeSourceEvent(
    std::shared_ptr<ThemeSourceEventCallback> theme_source_event_callback) {
  theme_source_event_callback_ = theme_source_event_callback;
}

void NativeThemeAdapter::NotifyThemeSourceEvent(
    const OhosColorMode theme_source) {
  if (theme_source_event_callback_ != nullptr) {
    theme_source_event_callback_->OnThemeSourceChanged(theme_source);
  }
}

JSBIND_GLOBAL() {
  JSBIND_FUNCTION(SetThemeSource);
}

}  // namespace ohos::adapter::native_theme
