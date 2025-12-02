// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/xcomponent/xcomponent_manager.h"

#include <chrono>

#include <arkui/native_interface.h>
#include <arkui/native_node.h>
#include <arkui/native_node_napi.h>

#include "ohos/adapter/accessibility/accessibility_adapter.h"
#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/common/trace.h"
#include "ohos/adapter/task_runner/main_thread_task_runner.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"
#include "ohos/adapter/xcomponent/node_handle_util.h"

namespace ohos::adapter::xcomponent {

namespace {
NewWindowParam ConvertWindowInitParamsToNewParams(
    const WindowInitParameter& param) {
  NewWindowParam newParam;
  newParam.parent_id = param.parent_id;
  newParam.window_id = param.window_id;
  newParam.bounds = param.bounds;
  newParam.init_color_argb = param.background_color;
  newParam.hide_title_bar = param.hide_title_bar;
  newParam.use_dark_mode = param.use_dark_mode;
  newParam.caption_button_visible = param.caption_button_visible;
  newParam.ability_type = param.ability_type;
  newParam.app_id = std::string(param.app_id);
  newParam.status = param.status;
  newParam.window_limit = param.window_limit;
  if (param.use_floating_window) {
    newParam.adapter_type = AdapterType::kSystemFloatingWindow;
  } else if (param.type == WindowInitType::kPopup) {
    newParam.adapter_type = AdapterType::kSubWindow;
  }
  return newParam;
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
bool SetXComponentAttribute(
    XComponentAttribute attribute,
    ArkUI_NodeHandle node_handle,
    const std::string& xcomponent_id,
    ArkUI_NativeNodeAPI_1* node_api) {
  ArkUI_NumberValue color_value[] = {{.u32 = attribute.background_color}};
  ArkUI_AttributeItem color_item = {color_value, 1};
  auto result = node_api->setAttribute(node_handle, NODE_BACKGROUND_COLOR, &color_item);
  if (result != ARKUI_ERROR_CODE_NO_ERROR) {
    LOGE("SetXComponentAttribute %{public}s set BACKGROUND_COLOR failed, res %{public}d",
        xcomponent_id.c_str(), result);
    return false;
  }
 
  ArkUI_NumberValue focusable_value[] = {{.i32 = attribute.focusable}};
  ArkUI_AttributeItem focusable_item = {focusable_value, 1};
  result = node_api->setAttribute(node_handle, NODE_FOCUSABLE, &focusable_item);
  if (result != ARKUI_ERROR_CODE_NO_ERROR) {
    LOGE("SetXComponentAttribute %{public}s set FOCUSABLE failed, res %{public}d",
        xcomponent_id.c_str(), result);
    return false;
  }
 
  ArkUI_NumberValue default_focus_value[] = {{.i32 = attribute.default_focus}};
  ArkUI_AttributeItem default_focus_item = {default_focus_value, 1};
  result = node_api->setAttribute(node_handle, NODE_DEFAULT_FOCUS, &default_focus_item);
  if (result != ARKUI_ERROR_CODE_NO_ERROR) {
    LOGE("SetXComponentAttribute %{public}s set DEFAULT_FOCUS failed, res %{public}d",
        xcomponent_id.c_str(), result);
    return false;
  }
 
  ArkUI_NumberValue render_fit_value[] = {{.i32 = attribute.render_fit}};
  ArkUI_AttributeItem render_fit_item = {render_fit_value, 1};
  result = node_api->setAttribute(node_handle, NODE_RENDER_FIT, &render_fit_item);
  if (result != ARKUI_ERROR_CODE_NO_ERROR) {
    LOGE("SetXComponentAttribute %{public}s set RENDER_FIT failed, res %{public}d",
        xcomponent_id.c_str(), result);
    return false;
  }
 
  ArkUI_AttributeItem xcomponent_id_item = {.string = xcomponent_id.c_str()};
  result = node_api->setAttribute(node_handle, NODE_ID, &xcomponent_id_item);
  if (result != ARKUI_ERROR_CODE_NO_ERROR) {
    LOGE("SetXComponentAttribute %{public}s set NODE_ID failed, res %{public}d",
        xcomponent_id.c_str(), result);
    return false;
  }
  return true;
}
 
__attribute__((no_sanitize("cfi", "cfi-icall")))
bool CreateAndBindNodeHandle(
    std::shared_ptr<NodeHandleXComponentImpl> render,
    ArkUI_NativeNodeAPI_1* node_api,
    const std::string& xcomponent_id) {
  ArkUI_NodeHandle node_handle = node_api->createNode(ARKUI_NODE_XCOMPONENT);
  if (node_handle == nullptr) {
    LOGE("CreateAndBindNodeHandle %{public}s create node handle failed",
        xcomponent_id.c_str());
    return false;
  }
  render->SetNodeHandle(node_handle);
  return true;
}

bool CreateAndBindSurfaceHolder(
    std::shared_ptr<NodeHandleXComponentImpl> render,
    const std::string& xcomponent_id) {
  OH_ArkUI_SurfaceHolder* surface_holder =
      NodeHandleImpl::GetInstance().SurfaceHolderCreate(
          render->GetNodeHandle());
  if (surface_holder == nullptr) {
    LOGE(
        "[XComponentManager] CreateAndBindSurfaceHolder %{public}s create "
        "surface holder failed",
        xcomponent_id.c_str());
    return false;
  }
  render->SetSurfaceHolder(surface_holder);

  auto result = NodeHandleImpl::GetInstance().SetUserData(
      surface_holder, render->GetNodeHandle());
  if (result != ARKUI_ERROR_CODE_NO_ERROR) {
    LOGE(
        "[XComponentManager] CreateAndBindSurfaceHolder %{public}s set user "
        "data failed, res %{public}d",
        xcomponent_id.c_str(), result);
    return false;
  }
  return true;
}

bool CreateAndBindSurfaceCallback(
    std::shared_ptr<NodeHandleXComponentImpl> render,
    const std::string& xcomponent_id) {
  OH_ArkUI_SurfaceCallback* surface_callback =
      NodeHandleImpl::GetInstance().SurfaceCallbackCreate();
  if (surface_callback == nullptr) {
    LOGE(
        "[XComponentManager] CreateAndBindSurfaceCallback %{public}s create "
        "surface callback failed",
        xcomponent_id.c_str());
    return false;
  }
  render->SetSurfaceCallback(surface_callback);
  return true;
}

bool InitializeNodeHandleXComponent(
    std::shared_ptr<NodeHandleXComponentImpl> render,
    const std::string& xcomponent_id) {
  render->InitializeWithNodeHandle(XComponentManager::GetInstance());
  // Initialize xcomponent node and trigger onSurfaceCreated callback
  auto result = NodeHandleImpl::GetInstance().XComponentInitialize(
      render->GetNodeHandle());
  if (result != ARKUI_ERROR_CODE_NO_ERROR) {
    LOGE(
        "[XComponentManager] InitializeNodeHandleXComponent %{public}s "
        "initialize failed, res %{public}d",
        xcomponent_id.c_str(), result);
    return false;
  }
  return true;
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
void FindOrCreateNodeHandleXComponent(XComponentAttribute attribute) {
  std::string xcomponent_id = attribute.xcomponent_id;

  if (XComponentManager::GetInstance()->GetNodeHandleXComponent(
          xcomponent_id)) {
    XComponentManager::GetInstance()->OnXcomponentAvailable(xcomponent_id);
    return;
  }

  // create XComponentImpl
  std::shared_ptr<NodeHandleXComponentImpl> render =
      XComponentManager::GetInstance()->CreateNodeHandleXComponent(
          xcomponent_id, attribute.xcomponent_type);
  if (render == nullptr) {
    LOGE(
        "[XComponentManager] FindOrCreateNodeHandleXComponent initialize "
        "xcomponent impl failed, xcomponent_id:%{public}s",
        xcomponent_id.c_str());
    return;
  }

  ArkUI_NativeNodeAPI_1* node_api = GetNativeNodeAPI();
  if (node_api == nullptr) {
    LOGE(
        "[XComponentManager] FindOrCreateNodeHandleXComponent %{public}s get "
        "nodeAPI failed",
        xcomponent_id.c_str());
    return;
  }

  if (!CreateAndBindNodeHandle(render, node_api, xcomponent_id)) {
    return;
  }

  if (!CreateAndBindSurfaceHolder(render, xcomponent_id)) {
    return;
  }

  if (!CreateAndBindSurfaceCallback(render, xcomponent_id)) {
    return;
  }

  if (!SetXComponentAttribute(std::move(attribute), render->GetNodeHandle(),
                              xcomponent_id, node_api)) {
    return;
  }

  InitializeNodeHandleXComponent(render, xcomponent_id);
  ohos::adapter::accessibility::AccessibilityAdapter::GetInstance().Initialize(
      render->GetNodeHandle(), xcomponent_id);
}

}  // namespace

XComponentManager* XComponentManager::GetInstance() {
  static XComponentManager instance;
  return &instance;
}

XComponentManager::~XComponentManager() {
  render_map_.clear();
  node_handle_render_map_.clear();
}

void XComponentManager::Initialize(napi_env env, napi_value exports) {
  napi_value export_instance = nullptr;
  if (napi_get_named_property(env, exports, OH_NATIVE_XCOMPONENT_OBJ,
                              &export_instance) != napi_ok) {
    LOGE("XComponentManager::Initialize napi_get_named_property fail");
    return;
  }

  if (export_instance == nullptr) {
    LOGE("XComponentManager::Initialize export instance null");
    return;
  }

  OH_NativeXComponent* native_xcomponent = nullptr;
  if (napi_unwrap(env, export_instance,
                  reinterpret_cast<void**>(&native_xcomponent)) != napi_ok) {
    LOGE("XComponentManager::Initialize napi_unwrap fail");
    return;
  }

  if (native_xcomponent == nullptr) {
    LOGE("XComponentManager::Initialize native xcomponent null");
    return;
  }

  char id_str[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
  uint64_t id_size = OH_XCOMPONENT_ID_LEN_MAX + 1;
  if (OH_NativeXComponent_GetXComponentId(native_xcomponent,
                                          id_str,
                                          &id_size) !=
      OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
    LOGE("XComponentManager::Initialize OH_NativeXComponent_GetXComponentId fail");
    return;
  }

  std::string xcomponent_id(id_str);
  LOGI("XComponentManager::Initialize idStr: %{public}s", id_str);
  std::size_t pos = xcomponent_id.find(":");
  if (pos == std::string::npos) {
    LOGE("XComponentManager invalid id: %{public}s", id_str);
    return;
  }

  auto id = xcomponent_id.substr(0, pos);
  auto type = xcomponent_id.substr(pos + 1);
  auto render = GetOrCreateXComponent(id, type);
  render->Initialize(native_xcomponent, this);

  ohos::adapter::accessibility::AccessibilityAdapter::GetInstance().Initialize(
      native_xcomponent, id);
}

std::shared_ptr<XComponentImpl> XComponentManager::GetOrCreateXComponent(
    const std::string& id, const std::string& type) {
  if (render_map_.find(id) == render_map_.end()) {
    render_map_[id] = std::make_shared<XComponentImpl>(id, type);
  }
  return render_map_[id];
}

std::shared_ptr<NodeHandleXComponentImpl>
XComponentManager::CreateNodeHandleXComponent(const std::string& id,
                                              XComponentType type) {
  auto result = node_handle_render_map_.emplace(
      id, std::make_shared<NodeHandleXComponentImpl>(id, type));
  return result.first->second;
}

std::shared_ptr<XComponentImpl>
XComponentManager::GetXComponent(const std::string& id) {
  if (render_map_.find(id) != render_map_.end()) {
    return render_map_[id];
  }

  return nullptr;
}

std::shared_ptr<NodeHandleXComponentImpl>
XComponentManager::GetNodeHandleXComponent(const std::string& id) {
  auto it = node_handle_render_map_.find(id);
  if (it != node_handle_render_map_.end()) {
    return it->second;
  }

  return nullptr;
}

std::shared_ptr<XComponentBase> XComponentManager::GetXComponentBase(
    const std::string& id) {
  if (nodeHandle::NodeHandleImpl::GetInstance().IsSupportNodeHandle()) {
    return GetNodeHandleXComponent(id);
  } else {
    return GetXComponent(id);
  }
}

void XComponentManager::RemoveXComponent(const std::string& id) {
  if (nodeHandle::NodeHandleImpl::GetInstance().IsSupportNodeHandle()) {
    node_handle_render_map_.erase(id);
    {
      std::lock_guard<std::mutex> lock_protect(ability_status_mutex_);
      ability_status_.erase(id);
    }
  } else {
    render_map_.erase(id);
  }
 
  {
    std::lock_guard<std::mutex> lock_protect(window_status_mutex_);
    window_status_.erase(id);
  }
}

void XComponentManager::RemoveNodeHandleXComponent(const std::string& id) {
  node_handle_render_map_.erase(id);
  {
    std::lock_guard<std::mutex> lock_protect(window_status_mutex_);
    window_status_.erase(id);
  }
}

void XComponentManager::RegisterInputEventCallBack(
    int32_t widget_id,
    std::shared_ptr<InputEventCallBack> callback) {
  for (const auto& [xcomponentId, render] : render_map_) {
    if (render != nullptr &&
        WindowAdapter::GetInstance().GetWidgetId(
            render->GetId()) == widget_id) {
      render->RegisterInputEventCallBack(callback);
    }
  }
}

void XComponentManager::RegisterNodeHandleInputEventCallBack(
    int32_t widget_id,
    std::shared_ptr<NodeHandleInputEventCallBack> callback) {
  for (const auto& [xcomponentId, render] : node_handle_render_map_) {
    if (render != nullptr &&
        WindowAdapter::GetInstance().GetWidgetId(render->GetId()) == widget_id) {
      render->RegisterNodeHandleInputEventCallBack(callback);
    }
  }
}

std::string XComponentManager::CreateWindow(const WindowInitParameter& param) {
  TRACE_EVENT_1("XComponentManager::CreateWindow", "xcomponentid", param.window_id);

  std::string create_id;
  NewWindowParam newParam =
      ConvertWindowInitParamsToNewParams(param);
  switch (param.type) {
    case WindowInitType::kWindow:
      create_id = CreateMainWindow(std::move(newParam));
      break;
    case WindowInitType::kPopup:
      create_id = CreateSubWindow(std::move(newParam));
      break;
    default:
      LOGE("unsupported window type:%{public}d", (int)param.type);
      break;
  }
  return create_id;
}

std::string XComponentManager::CreateMainWindow(const NewWindowParam& param) {
  if (nodeHandle::NodeHandleImpl::GetInstance().IsSupportNodeHandle()) {
    std::string create_id = param.window_id;
    creating_window_ = create_id;
    CreateXComponentViaNodeHandle(create_id, XComponentType::kWindow);
    WaitForXComponentCreated(create_id);
    creating_window_.clear();
    return create_id;
  } else {
    {
      std::lock_guard<std::mutex> lock_protect(window_status_mutex_);
      if (!reuse_window_.empty()) {
        std::string create_id = reuse_window_.front();
        reuse_window_.pop();
        return create_id;
      }
    }
    return CreateWindowViaDeclarative(param);
  }
}

void XComponentManager::CreateXComponentViaNodeHandle(
    const std::string& create_id,
    XComponentType type) {
  {
    std::lock_guard<std::mutex> lock_protect(window_status_mutex_);
    window_status_[create_id] = std::promise<bool>();
  }
 
  // Create XComponent via NodeHandle
  XComponentAttribute attribute = XComponentAttribute{
      create_id, type, true, true, 0x00FFFFFF, ARKUI_RENDER_FIT_TOP_LEFT};
  taskRunner::MainThreadTaskRunner::GetInstance().PostTask(
      std::bind(FindOrCreateNodeHandleXComponent, std::move(attribute)));
}
 
void XComponentManager::CreateAbilityViaNodeHandle(const NewWindowParam& param,
                                                   std::string& create_id) {
  {
    std::lock_guard<std::mutex> lock_protect(ability_status_mutex_);
    ability_status_.insert({create_id, std::promise<bool>()});
  }
  CreateWindowViaAdapter(param);
}

std::string XComponentManager::GetCreatedAbility() {
  std::lock_guard<std::mutex> lock_protect(ability_status_mutex_);
  std::string ability_id;
  if (!reuse_ability_.empty()) {
    ability_id = reuse_ability_.front();
    reuse_ability_.pop();
  }
  return ability_id;
}

void XComponentManager::CreateAndShowAbility(const WindowInitParameter& param,
                                             std::string create_id) {
  NewWindowParam newParam = ConvertWindowInitParamsToNewParams(param);
  CreateAbilityViaNodeHandle(newParam, create_id);
  WaitForAbilityCreated(create_id);
}

std::string XComponentManager::CreateSubWindow(const NewWindowParam& param) {
  std::string reuse_window_id =
      SubWindowAdapter::GetInstance().ReuseSubWindow(param);
  if (!reuse_window_id.empty()) {
    LOGI("XComponentManager::CreateSubWindow reuse sub window, id: %{public}s",
         reuse_window_id.c_str());

    if (nodeHandle::NodeHandleImpl::GetInstance().IsSupportNodeHandle()) {
      CreateXComponentViaNodeHandle(reuse_window_id,
                                    XComponentType::kSubWindow);
      WaitForXComponentCreated(reuse_window_id);
    }
    return reuse_window_id;
  }
 
  if (nodeHandle::NodeHandleImpl::GetInstance().IsSupportNodeHandle()) {
    return CreateSubWindowViaNodeHandle(std::move(param));
  } else {
    return CreateWindowViaDeclarative(std::move(param));
  }
}

std::string XComponentManager::CreateWindowViaDeclarative(
    const NewWindowParam& param) {
  std::string create_id = param.window_id;
  creating_window_ = param.window_id;
  {
    std::lock_guard<std::mutex> lock_protect(window_status_mutex_);
    window_status_.insert({create_id, std::promise<bool>()});
  }
  CreateWindowViaAdapter(param);
  WaitForXComponentCreated(create_id);
  creating_window_.clear();
  return create_id;
}
 
std::string XComponentManager::CreateSubWindowViaNodeHandle(
    const NewWindowParam& param) {
  std::string create_id = param.window_id;
  CreateXComponentViaNodeHandle(create_id, XComponentType::kSubWindow);
  CreateAbilityViaNodeHandle(param, create_id);
  WaitForAbilityCreated(create_id);
  WaitForXComponentCreated(create_id);
  return create_id;
}
 
void XComponentManager::CreateWindowViaAdapter(const NewWindowParam& param) {
  switch (param.adapter_type) {
    case AdapterType::kAppWindow:
      AppWindowAdapter::GetInstance().Create(param);
      break;
    case AdapterType::kSubWindow:
      SubWindowAdapter::GetInstance().Create(param);
      break;
    case AdapterType::kSystemFloatingWindow:
      SystemFloatingWindowAdapter::GetInstance().Create(param);
      break;
    default:
      LOGE("Unexpected window adapter type");
  }
}

void XComponentManager::OnActivationChanged(const std::string& id, bool active) {
  auto render = GetXComponentBase(id);
  if (!render || render->GetType() == XComponentType::kSubWindow) {
    LOGI("filter sub window render: %{public}s", id.c_str());
    return;
  }
  // update focus window exclude sub window
  if (active) {
    SetActivateWindow(id);
  }
}

void XComponentManager::OnWidgetAvailable(const std::string& id) {
  auto render = GetXComponentBase(id);
  if (!render) {
    LOGI("filter sub window render: %{public}s", id.c_str());
    return;
  }

  OnXcomponentAvailable(id);
 
  if (render->GetType() != XComponentType::kSubWindow) {
    // lost surface focus event in create xcomponent on OH platform, trigger
    // manually
    SetActivateWindow(id);
  }
}

void XComponentManager::OnWidgetDestroyed(const std::string& id) {
  RemoveXComponent(id);
}

void XComponentManager::SetActivateWindow(const std::string& id) {
  activate_window_ = id;

  auto jsFunc = ohos::adapter::GetJSFunction("ContextAdapter.SetActiveWindow");
  if (jsFunc) {
    jsFunc->Invoke<void>(id);
  }
}

bool XComponentManager::BindNativeXComponentNode(
    const std::string& id,
    ArkUI_NodeContentHandle node_content_handle) {
  std::shared_ptr<NodeHandleXComponentImpl> render =
      GetNodeHandleXComponent(id);
  if (render == nullptr) {
    LOGE(
        "XComponentManager::BindNativeXComponentNode not found render "
        "id: %{public}s",
        id.c_str());
    return false;
  }

  return render->BindNativeXComponentNode(node_content_handle);
}

bool XComponentManager::UnBindNativeXComponentNode(
    const std::string& id,
    ArkUI_NodeContentHandle node_content_handle) {
  std::shared_ptr<NodeHandleXComponentImpl> render =
      GetNodeHandleXComponent(id);
  if (render == nullptr) {
    LOGE(
        "XComponentManager::UnBindNativeXComponentNode not found render "
        "id: %{public}s",
        id.c_str());
    return false;
  }

  return render->UnBindNativeXComponentNode(node_content_handle);
}

void XComponentManager::OnAbilityAvailable(const std::string& id) {
  std::lock_guard<std::mutex> lock_protect(ability_status_mutex_);
  auto iter = ability_status_.find(id);
  if (iter != ability_status_.end()) {
    iter->second.set_value(true);
  } else {
    reuse_ability_.push(id);
  }
}

void XComponentManager::OnXcomponentAvailable(const std::string& id) {
  std::lock_guard<std::mutex> lock_protect(window_status_mutex_);
  auto iter = window_status_.find(id);
  if (iter != window_status_.end()) {
    iter->second.set_value(true);
  } else {
    reuse_window_.push(id);
  }
}

void XComponentManager::WaitForAbilityCreated(std::string& create_id) {
  if (create_id.empty()) {
    return;
  }
 
  std::future<bool> future;
  {
    std::lock_guard<std::mutex> lock_protect(ability_status_mutex_);
    future = ability_status_[create_id].get_future();
  }
  auto status = future.wait_for(std::chrono::seconds(3));
  if (status == std::future_status::timeout) {
    LOGE(
        "XComponentManager::CreateWindow %{public}s wait for ability create "
        "timeout",
        create_id.c_str());
    create_id.clear();
  }
}
 
void XComponentManager::WaitForXComponentCreated(std::string& create_id) {
  if (create_id.empty()) {
    return;
  }
  
  std::future<bool> future;
  {
    std::lock_guard<std::mutex> lock_protect(window_status_mutex_);
    future = window_status_[create_id].get_future();
  }
  auto status = future.wait_for(std::chrono::seconds(3));
  if (status == std::future_status::timeout) {
    LOGE(
        "XComponentManager::CreateWindow %{public}s wait for xcomponent create "
        "timeout",
        create_id.c_str());
    create_id.clear();
  }
}
 
void OnAbilityStartedCB(const std::string& id) {
  XComponentManager::GetInstance()->OnAbilityAvailable(id);
}
 
JSBIND_GLOBAL() {
  JSBIND_FUNCTION(OnAbilityStartedCB);
}

}  // namespace ohos::adapter::xcomponent
