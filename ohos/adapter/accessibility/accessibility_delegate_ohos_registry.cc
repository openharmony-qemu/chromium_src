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

#include "ohos/adapter/accessibility/accessibility_delegate_ohos_registry.h"

#include "ohos/adapter/common/logging.h"

namespace ohos::adapter::accessibility {

// static
AccessibilityDelegateOhosRegistry&
AccessibilityDelegateOhosRegistry::GetInstance() {
  static AccessibilityDelegateOhosRegistry instance;
  return instance;
}

std::shared_ptr<AccessibilityAdapter::Delegate>
AccessibilityDelegateOhosRegistry::GetAccessibilityDelegate(
    const std::string& window_id) {
  auto it = window_to_delegate_.find(window_id);
  if (it == window_to_delegate_.end()) {
    LOGE(
        "AccessibilityDelegateOhosRegistry::GetAccessibilityDelegate the "
        "delegate of window_id: %{public}s can't not found",
        window_id.c_str());
    return nullptr;
  }

  return it->second;
}

void AccessibilityDelegateOhosRegistry::RegisterAccessibilityDelegate(
    const std::string& window_id,
    std::shared_ptr<AccessibilityAdapter::Delegate> accessibility_delegate) {
  window_to_delegate_[window_id] = std::move(accessibility_delegate);
}

void AccessibilityDelegateOhosRegistry::UnregisterAccessibilityDelegate(
    const std::string& window_id) {
  auto it = window_to_delegate_.find(window_id);
  if (it != window_to_delegate_.end()) {
    window_to_delegate_.erase(it);
  }
}

}  // namespace ohos::adapter::accessibility
