// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_ACCESSIBILITY_PLATFORM_INSPECT_AX_TREE_FORMATTER_OHOS_H_
#define UI_ACCESSIBILITY_PLATFORM_INSPECT_AX_TREE_FORMATTER_OHOS_H_

#include "base/component_export.h"
#include "ui/accessibility/platform/inspect/ax_tree_formatter_base.h"

namespace ui {
class COMPONENT_EXPORT(AX_PLATFORM) AXTreeFormatterOhos
    : public AXTreeFormatterBase {
 public:
  AXTreeFormatterOhos() = default;
  ~AXTreeFormatterOhos() override = default;

  base::Value::Dict BuildTreeForSelector(
      const AXTreeSelector& selector) const override;

  std::string ProcessTreeForOutput(
      const base::Value::Dict& node) const override;

  base::Value::Dict BuildTree(AXPlatformNodeDelegate* root) const override;
};
}  // namespace ui

#endif  // UI_ACCESSIBILITY_PLATFORM_INSPECT_AX_TREE_FORMATTER_OHOS_H_