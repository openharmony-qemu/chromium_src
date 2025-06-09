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

#include "ohos/adapter/accessibility/accessibility_adapter.h"

#include <dlfcn.h>
#include <cstring>

#include "aki/jsbind.h"
#include "ohos/adapter/accessibility/accessibility_delegate_ohos_registry.h"
#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/common/shared_library.h"
#include "ohos/adapter/device_info/device_info.h"

namespace ohos::adapter::accessibility {

common::SharedLibrary native_accessibility_lib("ace_ndk.z");

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t AccessibilityProviderRegisterCallback(
    const char* instance_id,
    ArkUI_AccessibilityProvider* provider,
    AccessibilityProviderCallbacks* callbacks) {
  using NativeAccessibilityForProviderFunc =
      int32_t(const char*, ArkUI_AccessibilityProvider*,
              AccessibilityProviderCallbacks*);

  if (!native_accessibility_lib.IsLoaded()) {
    LOGE(
        "AccessibilityProviderRegisterCallback native_accessibility_lib Load "
        "fail");
    return -1;
  }
  auto fn =
      native_accessibility_lib.GetFunction<NativeAccessibilityForProviderFunc>(
          "OH_ArkUI_AccessibilityProviderRegisterCallbackWithInstance");
  if (fn == nullptr) {
    LOGE(
        "AccessibilityProviderRegisterCallback get "
        "OH_ArkUI_AccessibilityProviderRegisterCallbackWithInstance fail");
    return -1;
  }
  return fn(instance_id, provider, callbacks);
}

AccessibilityAdapter& AccessibilityAdapter::GetInstance() {
  static AccessibilityAdapter instance;
  return instance;
}

void AccessibilityAdapter::Speech(const std::string& text,
                                  const std::string& id) {
  if (auto func = GetJSFunction("AccessibilityAdapter.Speech")) {
    func->Invoke<void>(text, id);
  } else {
    LOGW("[AccessibilityAdapter] Speech ets function not found");
  }
}

void AccessibilityAdapter::ShutDown() {
  if (auto func = GetJSFunction("AccessibilityAdapter.ShutDown")) {
    func->Invoke<void>();
  } else {
    LOGW("[AccessibilityAdapter] ShutDown ets function not found");
  }
}

void AccessibilityAdapter::Initialize(OH_NativeXComponent* native_xcomponent,
                                      std::string& id) {
  int32_t ret = OH_NativeXComponent_GetNativeAccessibilityProvider(
      native_xcomponent, &provider_);
  if (provider_ == nullptr) {
    LOGE(
        "AccessibilityAdapter::Initialize "
        "OH_NativeXComponent_GetNativeAccessibilityProvider get provider "
        "fail");
    return;
  }

  accessibility_provider_callbacks_ = {
      FindAccessibilityNodeInfosById,    FindAccessibilityNodeInfosByText,
      FindFocusedAccessibilityNode,      FindNextFocusAccessibilityNode,
      ExecuteAccessibilityAction,        ClearFocusedFocusAccessibilityNode,
      GetAccessibilityNodeCursorPosition};

  ret = AccessibilityProviderRegisterCallback(
      id.c_str(), provider_, &accessibility_provider_callbacks_);
  if (ret != 0) {
    LOGE(
        "AccessibilityAdapter::Initialize "
        "AccessibilityProviderRegisterCallback run fail, instance_id: "
        "%{public}s, ret: %{public}d",
        id.c_str(), ret);
    return;
  }
}

// static
int32_t AccessibilityAdapter::FindAccessibilityNodeInfosById(
    const char* instance_id,
    int64_t element_id,
    ArkUI_AccessibilitySearchMode mode,
    int32_t request_id,
    NativeElementList element_list) {
  if (!element_list || !instance_id) {
    LOGE(
        "AccessibilityAdapter::FindAccessibilityNodeInfosById element_list "
        "or "
        "instance_id is null, request_id: %{public}d",
        request_id);
    return OH_NATIVEXCOMPONENT_RESULT_FAILED;
  }

  std::string xcomponent_id(instance_id);
  auto accessibility_delegate =
      AccessibilityDelegateOhosRegistry::GetInstance().GetAccessibilityDelegate(
          xcomponent_id);
  if (!accessibility_delegate) {
    LOGE(
        "AccessibilityAdapter::FindAccessibilityNodeInfosById "
        "accessibility_delegate of xcomponent_id: %{public}s can't not "
        "found, "
        "request_id: %{public}d",
        xcomponent_id.c_str(), request_id);
    return OH_NATIVEXCOMPONENT_RESULT_FAILED;
  }

  int32_t accessibility_id = static_cast<int32_t>(element_id);
  bool ret = accessibility_delegate->FindAccessibilityNodeInfosById(
      accessibility_id, mode, element_list);
  if (!ret) {
    LOGE(
        "AccessibilityAdapter::FindAccessibilityNodeInfosById failed, "
        "xcomponent_id: %{public}s, accessibility_id: %{public}d, "
        "request_id: "
        "%{public}d",
        xcomponent_id.c_str(), accessibility_id, request_id);
    return OH_NATIVEXCOMPONENT_RESULT_FAILED;
  }
  return OH_NATIVEXCOMPONENT_RESULT_SUCCESS;
}

// static
int32_t AccessibilityAdapter::FindAccessibilityNodeInfosByText(
    const char* instance_id,
    int64_t element_id,
    const char* text,
    int32_t request_id,
    NativeElementList element_list) {
  LOGW(
      "AccessibilityAdapter::FindAccessibilityNodeInfosByText is not "
      "support, "
      "request_id: %{public}d",
      request_id);
  return OH_NATIVEXCOMPONENT_RESULT_FAILED;
}

// static
int32_t AccessibilityAdapter::FindFocusedAccessibilityNode(
    const char* instance_id,
    int64_t element_id,
    ArkUI_AccessibilityFocusType focusType,
    int32_t request_id,
    NativeElement element_info) {
  LOGW(
      "AccessibilityAdapter::FindFocusedAccessibilityNode is not support, "
      "request_id: %{public}d",
      request_id);
  return OH_NATIVEXCOMPONENT_RESULT_FAILED;
}

// static
int32_t AccessibilityAdapter::FindNextFocusAccessibilityNode(
    const char* instance_id,
    int64_t element_id,
    ArkUI_AccessibilityFocusMoveDirection direction,
    int32_t request_id,
    NativeElement element_info) {
  LOGW(
      "AccessibilityAdapter::FindNextFocusAccessibilityNode is not "
      "support, "
      "request_id: %{public}d",
      request_id);
  return OH_NATIVEXCOMPONENT_RESULT_FAILED;
}

// static
int32_t AccessibilityAdapter::ExecuteAccessibilityAction(
    const char* instance_id,
    int64_t element_id,
    ArkUI_Accessibility_ActionType action,
    ArkUI_AccessibilityActionArguments* action_arguments,
    int32_t request_id) {
  LOGW(
      "AccessibilityAdapter::ExecuteAccessibilityAction is not support, "
      "request_id: %{public}d",
      request_id);
  return OH_NATIVEXCOMPONENT_RESULT_FAILED;
}

// static
int32_t AccessibilityAdapter::ClearFocusedFocusAccessibilityNode(
    const char* instance_id) {
  LOGW(
      "AccessibilityAdapter::ClearFocusedFocusAccessibilityNode is not "
      "support");
  return OH_NATIVEXCOMPONENT_RESULT_FAILED;
}

// static
int32_t AccessibilityAdapter::GetAccessibilityNodeCursorPosition(
    const char* instance_id,
    int64_t element_id,
    int32_t request_id,
    int32_t* index) {
  LOGW(
      "AccessibilityAdapter::GetAccessibilityNodeCursorPosition is not "
      "support, request_id: %{public}d",
      request_id);
  return OH_NATIVEXCOMPONENT_RESULT_FAILED;
}

}  // namespace ohos::adapter::accessibility
