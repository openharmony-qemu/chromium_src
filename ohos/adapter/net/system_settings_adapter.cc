// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/net/system_settings_adapter.h"

namespace ohos::adapter::net {

SystemSettingsAdapter& SystemSettingsAdapter::GetInstance() {
  static SystemSettingsAdapter instance;
  return instance;
}

void SystemSettingsAdapter::ShowSystemsSettings(const std::string& uri,
                                                const std::string& sub_uri) {
  std::promise<bool> promise;
  std::function<void(bool)> callback = [&promise](bool successful) -> void {
    promise.set_value(successful);
  };
  if (auto func =
          ohos::adapter::GetJSFunction("NetConnection.ShowSystemSettings")) {
    if (sub_uri.empty()) {
      func->Invoke<void>(uri, callback);
    } else {
      func->Invoke<void>(uri, callback, sub_uri);
    }
    promise.get_future().get();
  }
}

}  // namespace ohos::adapter::net
