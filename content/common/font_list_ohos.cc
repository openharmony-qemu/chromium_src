// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>
#include <string>
#include <utility>

#include "base/values.h"
#include "content/common/font_list.h"
#include "ohos/adapter/font/font_adapter.h"

namespace content {

base::Value::List GetFontList_SlowBlocking() {
  base::Value::List font_list;
  std::vector<std::string> sorted_families =
      ohos::adapter::font::FontAdapter::GetInstance().GetConfigFontList();

  std::sort(sorted_families.begin(), sorted_families.end());
  for (const auto& family : sorted_families) {
    base::Value::List font_item;
    font_item.Append(family);
    font_item.Append(family);  // localized name.
    font_list.Append(std::move(font_item));
  }

  return font_list;
}
}  // namespace content
