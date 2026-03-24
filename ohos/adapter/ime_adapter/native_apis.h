#ifndef IME_ADAPTER_NATIVE_API_H_
#define IME_ADAPTER_NATIVE_API_H_

#include <deviceinfo.h>
#include <inputmethod/inputmethod_attach_options_capi.h>
#include <inputmethod/inputmethod_inputmethod_proxy_capi.h>
#include <inputmethod/inputmethod_types_capi.h>

#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/device_info/device_info.h"

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
namespace ohos {
namespace adapter {
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
}  // namespace adapter
}  // namespace ohos

#endif
