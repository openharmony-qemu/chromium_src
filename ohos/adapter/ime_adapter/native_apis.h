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

#ifndef IME_ADAPTER_NATIVE_API_H_
#define IME_ADAPTER_NATIVE_API_H_

#include <deviceinfo.h>
#include <inputmethod/inputmethod_attach_options_capi.h>
#include <inputmethod/inputmethod_inputmethod_proxy_capi.h>
#include <inputmethod/inputmethod_types_capi.h>

#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/device_info/device_info.h"

namespace {
extern "C" {
InputMethod_ErrorCode OH_InputMethodProxy_ShowTextInput(
    InputMethod_InputMethodProxy* inputMethodProxy,
    InputMethod_AttachOptions* options) __attribute__((weak));
InputMethod_ErrorCode OH_AttachOptions_GetRequestKeyboardReason(
    InputMethod_AttachOptions* options,
    int* requestKeyboardReason) __attribute__((weak));
InputMethod_AttachOptions* OH_AttachOptions_CreateWithRequestKeyboardReason(
    bool showKeyboard,
    InputMethod_RequestKeyboardReason requestKeyboardReason)
    __attribute__((weak));
}
}  // namespace

namespace ohos::adapter {
InputMethod_ErrorCode OhInputMethodProxyShowTextInput(
    InputMethod_InputMethodProxy* inputMethodProxy,
    InputMethod_AttachOptions* options) {
  if (OH_InputMethodProxy_ShowTextInput) {
    return OH_InputMethodProxy_ShowTextInput(inputMethodProxy, options);
  }
  LOGE("OH_InputMethodProxy_ShowTextInput is nullptr");
  return IME_ERR_UNDEFINED;
}

InputMethod_ErrorCode OhAttachOptionsGetRequestKeyboardReason(
    InputMethod_AttachOptions* options,
    int* requestKeyboardReason) {
  if (OH_AttachOptions_GetRequestKeyboardReason) {
    return OH_AttachOptions_GetRequestKeyboardReason(options,
                                                     requestKeyboardReason);
  }
  LOGE("OH_AttachOptions_GetRequestKeyboardReason is nullptr");
  return IME_ERR_UNDEFINED;
}

InputMethod_AttachOptions* OhAttachOptionsCreateWithRequestKeyboardReason(
    bool showKeyboard,
    InputMethod_RequestKeyboardReason requestKeyboardReason) {
  if (OH_AttachOptions_CreateWithRequestKeyboardReason) {
    return OH_AttachOptions_CreateWithRequestKeyboardReason(
        showKeyboard, requestKeyboardReason);
  }
  LOGE("OH_AttachOptions_CreateWithRequestKeyboardReason is nullptr");
  return nullptr;
}

bool IsNativeAPIsSufficientForInputMethod() {
  static const bool is_native_apis_sufficient =
      OH_GetSdkApiVersion() > ohos::adapter::device_info::SDK_VERSION_14 &&
      OH_AttachOptions_CreateWithRequestKeyboardReason &&
      OH_InputMethodProxy_ShowTextInput &&
      OH_AttachOptions_GetRequestKeyboardReason;
  return is_native_apis_sufficient;
}
}  // namespace ohos::adapter

#endif
