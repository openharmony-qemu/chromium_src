// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ax_tree_formatter_ohos.h"

namespace ui {

base::Value::Dict AXTreeFormatterOhos::BuildTreeForSelector(const AXTreeSelector& selector) const {
  return base::Value::Dict();
}

std::string AXTreeFormatterOhos::ProcessTreeForOutput(const base::Value::Dict& node) const {
  return "";
}

base::Value::Dict AXTreeFormatterOhos::BuildTree(AXPlatformNodeDelegate* root) const {
  return base::Value::Dict();
}

}  // namespace ui