// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_XCOMPONENT_XCOMPONENT_DELEGATE_H_
#define OHOS_ADAPTER_XCOMPONENT_XCOMPONENT_DELEGATE_H_

#include <string>

namespace ohos::adapter::xcomponent {

enum class XComponentState {
  kUnknown,
  kStarting,
  kStarted,
  kDestroyed,
};

class XComponentDelegate {
 public:
  XComponentDelegate() = default;
  virtual ~XComponentDelegate() = default;

  virtual void OnCloseRequest(const std::string& id) {}
  virtual void OnClosed(const std::string& id) {}
  virtual void OnWindowStateChanged(const std::string& id,
                                    XComponentState old_state,
                                    XComponentState new_state) {}
  virtual void OnWidgetAvailable(const std::string& id) = 0;
  virtual void OnWidgetDestroyed(const std::string& id) = 0;
  virtual void OnActivationChanged(const std::string& id, bool active) = 0;
};

}  // namespace ohos::adapter::xcomponent

#endif  // OHOS_ADAPTER_XCOMPONENT_XCOMPONENT_DELEGATE_H_
