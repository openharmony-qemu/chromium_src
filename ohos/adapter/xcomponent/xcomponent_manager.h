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
#include <mutex>
#include <queue>
#include <string>
#include <unordered_map>

#include "ohos/adapter/export.h"
#include "ohos/adapter/window/app_window_adapter.h"
#include "ohos/adapter/window/sub_window_adapter.h"
#include "ohos/adapter/window/system_floating_window_adapter.h"
#include "ohos/adapter/xcomponent/renderer/node_handle_xcomponent_impl.h"
#include "ohos/adapter/xcomponent/renderer/xcomponent_impl.h"
#include "ohos/adapter/xcomponent/xcomponent_delegate.h"

namespace ohos::adapter::xcomponent {

struct XComponentAttribute {
  std::string xcomponent_id;
  XComponentType xcomponent_type;
  bool focusable;
  bool default_focus;
  uint32_t background_color;
  ArkUI_RenderFit render_fit;
};

using namespace ohos::adapter::window;
using namespace ohos::adapter::nodeHandle;

class ADAPTER_EXPORT_API XComponentManager final : public XComponentDelegate {
 public:
  ~XComponentManager();
  XComponentManager(XComponentManager& other) = delete;
  void operator=(const XComponentManager&) = delete;
  static XComponentManager* GetInstance();

  void Initialize(napi_env env, napi_value exports);

  std::shared_ptr<XComponentImpl>
  GetOrCreateXComponent(const std::string& id,
                        const std::string& type);
  std::shared_ptr<XComponentImpl> GetXComponent(const std::string& id);
  std::shared_ptr<XComponentBase> GetXComponentBase(const std::string& id);
  void RemoveXComponent(const std::string& id);
  void RemoveNodeHandleXComponent(const std::string& id);
  void RegisterInputEventCallBack(int32_t widget_id,
                                  std::shared_ptr<InputEventCallBack> callback);
  std::string CreateWindow(const WindowInitParameter& param);
  std::string GetCreatingWindow() { return creating_window_; }
  std::string GetActivateWindow() const { return activate_window_; }

  // XComponentDelegate
  void OnActivationChanged(const std::string& id, bool active) override;
  void OnWidgetAvailable(const std::string& id) override;
  void OnWidgetDestroyed(const std::string& id) override;

  // XComponent For NodeHandle
  std::shared_ptr<NodeHandleXComponentImpl> CreateNodeHandleXComponent(
                                                      const std::string& id,
                                                      XComponentType type);
  std::shared_ptr<NodeHandleXComponentImpl> GetNodeHandleXComponent(const std::string& id);
  void RegisterNodeHandleInputEventCallBack(int32_t widget_id,
                                            std::shared_ptr<NodeHandleInputEventCallBack> callback);
  bool BindNativeXComponentNode(const std::string& id, ArkUI_NodeContentHandle node_content_handle);
  bool UnBindNativeXComponentNode(const std::string& id, ArkUI_NodeContentHandle node_content_handle);
  void OnAbilityAvailable(const std::string& id);
  void OnXcomponentAvailable(const std::string& id);
  std::string GetCreatedAbility();
  void CreateAndShowAbility(const WindowInitParameter& param,
                            std::string create_id);
  void AddCreatingAbility(const std::string& create_id);

 private:
  XComponentManager() = default;

  std::string CreateMainWindow(const NewWindowParam& param);
  std::string CreateSubWindow(const NewWindowParam& param);
  void SetActivateWindow(const std::string& id);
  std::string CreateWindowViaDeclarative(const NewWindowParam& param);
  // XComponent For NodeHandle
  std::string CreateMainWindowViaNodeHandle(const NewWindowParam& param);
  std::string CreateSubWindowViaNodeHandle(const NewWindowParam& param);
  void CreateXComponentViaNodeHandle(const std::string& create_id,
                                     XComponentType type);
  void CreateAbility(const NewWindowParam& param);
  void WaitForXComponentCreated(std::string& create_id);
  void WaitForAbilityCreated(std::string& create_id);

  std::unordered_map<std::string, std::shared_ptr<XComponentImpl>> render_map_;
  std::unordered_map<std::string, std::promise<bool>> window_status_;
  std::queue<std::string> reuse_window_;
  std::string creating_window_;
  std::string activate_window_;
  // lock the resource related to window (window_status_&&reuse_window_)
  std::mutex window_status_mutex_;
 
  // XComponent For NodeHandle
  // node_handle_render_map_ MUST be accessed in ArkUI Main Thread
  std::unordered_map<std::string, std::shared_ptr<NodeHandleXComponentImpl>> node_handle_render_map_;
  std::unordered_map<std::string, std::promise<bool>> ability_status_;
  std::queue<std::string> reuse_ability_;
  // lock the resource related to ability (ability_status_&&reuse_ability_)
  std::mutex ability_status_mutex_;
};

}  // namespace ohos::adapter::xcomponent

#endif  // OHOS_ADAPTER_XCOMPONENT_XCOMPONENT_MANAGER_H_
