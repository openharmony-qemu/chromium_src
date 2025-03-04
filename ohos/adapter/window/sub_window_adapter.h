// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_SUBWINDOW_SUBWINDOW_ADAPTER_H_
#define OHOS_ADAPTER_SUBWINDOW_SUBWINDOW_ADAPTER_H_

#include <cstdint>
#include <functional>
#include <string>

#include "ohos/adapter/export.h"
#include "ohos/adapter/window/window_common.h"

namespace ohos::adapter::window {

class ADAPTER_EXPORT_API SubWindowAdapter {
 public:
  static SubWindowAdapter& GetInstance();

  bool Create(const std::string& parent_id,
              const std::string& id,
              const WindowRect& rect,
              uint32_t color_argb,
              std::string& create_id);
  void Cancel(const std::string& id);

  void Show(const std::string& id);
  void Hide(const std::string& id);

  void SetBounds(const std::string& id, const WindowRect& rect);

 private:
  SubWindowAdapter() = default;
  ~SubWindowAdapter() = default;
};

}  // namespace ohos::adapter::window
#endif  // OHOS_ADAPTER_SUBWINDOW_SUBWINDOW_ADAPTER_H_
