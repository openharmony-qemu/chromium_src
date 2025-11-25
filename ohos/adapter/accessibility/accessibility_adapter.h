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

#ifndef OHOS_ADAPTER_ACCESSIBILITY_ACCESSIBILITY_ADAPTER_H_
#define OHOS_ADAPTER_ACCESSIBILITY_ACCESSIBILITY_ADAPTER_H_

#include <ace/xcomponent/native_interface_xcomponent.h>
#include <arkui/native_interface_accessibility.h>

#include <string>

#include "ohos/adapter/common/shared_library.h"
#include "ohos/adapter/export.h"

namespace ohos::adapter::accessibility {

using NativeElementList = ArkUI_AccessibilityElementInfoList*;
using NativeElement = ArkUI_AccessibilityElementInfo*;

class NativeElementWrapper {};

class ADAPTER_EXPORT_API AccessibilityAdapter {
 public:
  class Delegate {
   public:
    Delegate() = default;
    virtual ~Delegate() = default;

    virtual bool FindAccessibilityNodeInfosById(
        int32_t element_id,
        ArkUI_AccessibilitySearchMode mode,
        NativeElementList element_list) = 0;
  };

  AccessibilityAdapter(const AccessibilityAdapter&) = delete;
  AccessibilityAdapter& operator=(const AccessibilityAdapter&) = delete;
  static AccessibilityAdapter& GetInstance();

  void Speech(const std::string& text, const std::string& id);
  void ShutDown();

  void Initialize(OH_NativeXComponent* native_xcomponent, std::string& id);
  void Initialize(ArkUI_NodeHandle node, std::string& id);
  void RegisterCallback(std::string& id);
  static int32_t FindAccessibilityNodeInfosById(
      const char* instance_id,
      int64_t element_id,
      ArkUI_AccessibilitySearchMode mode,
      int32_t request_id,
      NativeElementList element_list);
  static int32_t FindAccessibilityNodeInfosByText(
      const char* instance_id,
      int64_t element_id,
      const char* text,
      int32_t request_id,
      NativeElementList element_list);
  static int32_t FindFocusedAccessibilityNode(
      const char* instance_id,
      int64_t element_id,
      ArkUI_AccessibilityFocusType focusType,
      int32_t request_id,
      NativeElement element_info);
  static int32_t FindNextFocusAccessibilityNode(
      const char* instance_id,
      int64_t element_id,
      ArkUI_AccessibilityFocusMoveDirection direction,
      int32_t request_id,
      NativeElement element_info);
  static int32_t ExecuteAccessibilityAction(
      const char* instance_id,
      int64_t element_id,
      ArkUI_Accessibility_ActionType action,
      ArkUI_AccessibilityActionArguments* action_arguments,
      int32_t request_id);
  static int32_t ClearFocusedFocusAccessibilityNode(const char* instance_id);
  static int32_t GetAccessibilityNodeCursorPosition(const char* instance_id,
                                                    int64_t element_id,
                                                    int32_t request_id,
                                                    int32_t* index);

 private:
  AccessibilityAdapter();
  ~AccessibilityAdapter() = default;

  ArkUI_AccessibilityProvider* provider_ = nullptr;
  ArkUI_AccessibilityProviderCallbacksWithInstance
      accessibility_provider_callbacks_;
  common::SharedLibrary native_accessibility_lib_;
  using NativeAccessibilityForProviderFunc =
      int32_t(const char*,
              ArkUI_AccessibilityProvider*,
              ArkUI_AccessibilityProviderCallbacksWithInstance*);
  NativeAccessibilityForProviderFunc*
      accessibility_provider_register_callback_fn_ = nullptr;
};

}  // namespace ohos::adapter::accessibility
#endif  // OHOS_ADAPTER_ACCESSIBILITY_ACCESSIBILITY_ADAPTER_H_
