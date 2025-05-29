// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/accessibility/accessibility_adapter.h"

#include "aki/jsbind.h"
#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/logging.h"

namespace ohos::adapter::accessibility {

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

}  // namespace ohos::adapter::accessibility
