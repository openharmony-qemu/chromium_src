// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_ACCESSIBILITY_ACCESSIBILITY_ADAPTER_H_
#define OHOS_ADAPTER_ACCESSIBILITY_ACCESSIBILITY_ADAPTER_H_

#include <string>

#include "ohos/adapter/export.h"

namespace ohos::adapter::accessibility {

class ADAPTER_EXPORT_API AccessibilityAdapter {
 public:
  AccessibilityAdapter(const AccessibilityAdapter&) = delete;
  AccessibilityAdapter& operator=(const AccessibilityAdapter&) = delete;
  static AccessibilityAdapter& GetInstance();

  void Speech(const std::string& text, const std::string& id);
  void ShutDown();

 private:
  AccessibilityAdapter() = default;
  ~AccessibilityAdapter() = default;
};

}  // namespace ohos::adapter::accessibility
#endif  // OHOS_ADAPTER_ACCESSIBILITY_ACCESSIBILITY_ADAPTER_H_
