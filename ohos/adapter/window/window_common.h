// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_WINDOW_WINDOW_COMMON_H_
#define OHOS_ADAPTER_WINDOW_WINDOW_COMMON_H_

#include <cstdint>
#include <string>

#include "ohos/adapter/export.h"

namespace ohos::adapter::window {

class WindowRect {
 public:
  int32_t left;
  int32_t top;
  int32_t width;
  int32_t height;
};

struct ADAPTER_EXPORT_API NewWindowParam {
  std::string id;
  int left;
  int top;
  int width;
  int height;
  bool hide_title_bar = true;
  bool using_system_floating_window = false;

  NewWindowParam(const std::string& id,
                 const int left,
                 const int top,
                 const int width,
                 const int height,
                 const bool hide_title_bar = true,
                 const bool using_system_floating_window = false)
      : id(id),
        left(left),
        top(top),
        width(width),
        height(height),
        hide_title_bar(hide_title_bar),
        using_system_floating_window(using_system_floating_window) {}
};

}  // namespace ohos::adapter::window
#endif  // OHOS_ADAPTER_WINDOW_WINDOW_COMMON_H_
