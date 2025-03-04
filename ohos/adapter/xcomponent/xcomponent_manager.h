// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_XCOMPONENT_XCOMPONENT_MANAGER_H_
#define OHOS_ADAPTER_XCOMPONENT_XCOMPONENT_MANAGER_H_

#include <ace/xcomponent/native_interface_xcomponent.h>
#include <js_native_api.h>
#include <js_native_api_types.h>
#include <napi/native_api.h>

#include <future>
#include <memory>
#include <string>
#include <unordered_map>

#include "ohos/adapter/export.h"
#include "ohos/adapter/window/app_window_adapter.h"
#include "ohos/adapter/window/system_floating_window_adapter.h"
#include "ohos/adapter/xcomponent/renderer/xcomponent_impl.h"
#include "ohos/adapter/xcomponent/xcomponent_delegate.h"

namespace ohos::adapter::xcomponent {

class ADAPTER_EXPORT_API XComponentManager final : public XComponentDelegate {
 public:
  ~XComponentManager();
  XComponentManager(XComponentManager& other) = delete;
  void operator=(const XComponentManager&) = delete;
  static XComponentManager* GetInstance();

  void Initialize(napi_env env, napi_value exports);

  XComponentImpl* GetOrCreateXComponent(const std::string& id);
  XComponentImpl* GetXComponent(const std::string& id);
  void RemoveXComponent(const std::string& id);
  void RegisterInputEventCallBack(int32_t widget_id,
                                  std::shared_ptr<InputEventCallBack> callback);
  bool StartNewWindow(const ohos::adapter::window::NewWindowParam& param);
  std::string GetCreatingWindow() { return creating_window_; }
  bool WindowIsCreating(int32_t id);
  void RequestLayout(const std::string& id);
  std::string GetActivateWindow() const { return activate_window_; }

  // XComponentDelegate
  void OnActivationChanged(const std::string& id, bool active) override;
  void OnWidgetAvailable(const std::string& id) override;
  void OnWidgetDestroyed(const std::string& id) override;

 private:
  XComponentManager() = default;
  static XComponentManager* manager_;
  std::unordered_map<std::string, XComponentImpl*> render_map_;
  std::unordered_map<std::string, std::promise<bool>> window_status_;
  std::string creating_window_;
  std::string activate_window_;
};

}  // namespace ohos::adapter::xcomponent

#endif  // OHOS_ADAPTER_XCOMPONENT_XCOMPONENT_MANAGER_H_
