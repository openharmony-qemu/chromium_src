// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_WEB_APP_WEB_APP_ADAPTER_H_
#define OHOS_ADAPTER_WEB_APP_WEB_APP_ADAPTER_H_

#include <string>

#include "ohos/adapter/export.h"

namespace ohos::adapter {

struct DesktopShortcut {
  std::string shortcutId = "";
  std::string label = "";
  std::string foregroundIconPath = "";
  std::string backgroundIconPath = "";
  bool openAsWindow = true;
};

class ADAPTER_EXPORT_API WebAppAdapter {
 public:
  static WebAppAdapter& GetInstance();

  WebAppAdapter(const WebAppAdapter&) = delete;
  WebAppAdapter& operator=(const WebAppAdapter&) = delete;

  ~WebAppAdapter() = default;

  bool CreateDesktopShortcut(const DesktopShortcut& shortcut);

 private:
  WebAppAdapter() = default;
};

}  // namespace ohos::adapter

#endif  // OHOS_ADAPTER_WEB_APP_WEB_APP_ADAPTER_H_
