// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/web_app/web_app_adapter.h"

#include "ohos/adapter/aki_hook/aki_hook.h"

namespace ohos::adapter {

WebAppAdapter& WebAppAdapter::GetInstance() {
  static WebAppAdapter instance;
  return instance;
}

bool WebAppAdapter::CreateDesktopShortcut(const DesktopShortcut& shortcut) {
  auto func =
      ohos::adapter::GetJSFunction("WebAppAdapter.CreateDesktopShortcut");
  if (func) {
    std::promise<bool> create_promise;
    std::function<void(bool)> callback = [&create_promise](bool ret) {
      create_promise.set_value(ret);
    };
    func->Invoke<void>(shortcut, callback);
    return create_promise.get_future().get();
  }
  LOGE("[WebAppAdapter] CreateDesktopShortcut function not found");
  return false;
}

JSBIND_CLASS(DesktopShortcut) {
  JSBIND_PROPERTY(shortcutId);
  JSBIND_PROPERTY(label);
  JSBIND_PROPERTY(foregroundIconPath);
  JSBIND_PROPERTY(backgroundIconPath);
  JSBIND_PROPERTY(openAsWindow);
}

}  // namespace ohos::adapter
