// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_BROWSER_BROWSER_ADAPTER_H_
#define OHOS_ADAPTER_BROWSER_BROWSER_ADAPTER_H_

#include <functional>

#include "ohos/adapter/export.h"

namespace ohos::adapter {
class ADAPTER_EXPORT_API BrowserAdapter {
 public:
  static BrowserAdapter& GetInstance();
  using NewWindowCallback = std::function<void(const std::string&)>;
  void StartNewWindow(const std::string& url);
  NewWindowCallback GetNewWindowCallback() { return new_window_callback_; }
  void RegisterNewWindowCallback(NewWindowCallback callback);
  using GetLastActiveCallback = std::function<uint32_t()>;
  void RegisterGetLastActiveCallback(GetLastActiveCallback callback);
  void UnRegisterGetLastActiveCallback();
  uint32_t GetLastActiveAcceleratedWidget();

 private:
  BrowserAdapter() = default;
  ~BrowserAdapter() = default;
  NewWindowCallback new_window_callback_;
  GetLastActiveCallback get_last_active_callback_;
};
}  // namespace ohos::adapter

#endif  // OHOS_ADAPTER_BROWSER_BROWSER_ADAPTER_H_
