// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/xcomponent/xcomponent_manager.h"

#include <chrono>

#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/common/trace.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"

using namespace std::chrono_literals;
using namespace ohos::adapter::window;

namespace ohos::adapter::xcomponent {

XComponentManager* XComponentManager::manager_ = nullptr;
XComponentManager* XComponentManager::GetInstance() {
  if (manager_ == nullptr) {
    manager_ = new XComponentManager();
  }
  return manager_;
}

XComponentManager::~XComponentManager() {
  for (auto iter = render_map_.begin(); iter != render_map_.end(); ++iter) {
    if (iter->second != nullptr) {
      delete iter->second;
      iter->second = nullptr;
    }
  }
  render_map_.clear();
}

void XComponentManager::Initialize(napi_env env, napi_value exports) {
  napi_value export_instance = nullptr;
  if (napi_get_named_property(env, exports, OH_NATIVE_XCOMPONENT_OBJ,
                              &export_instance) != napi_ok) {
    LOGE("XComponentManager::Initialize napi_get_named_property fail");
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
  if (OH_NativeXComponent_GetXComponentId(native_xcomponent, id_str,
                                          &id_size) !=
      OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
    LOGE(
        "XComponentManager::Initialize OH_NativeXComponent_GetXComponentId "
        "fail");
    return;
  }

  std::string id(id_str);
  LOGI("XComponentManager::Initialize idStr: %{public}s", id_str);
  auto render = GetOrCreateXComponent(id);
  render->Initialize(native_xcomponent, this);
}

XComponentImpl* XComponentManager::GetOrCreateXComponent(
    const std::string& id) {
  if (render_map_.find(id) == render_map_.end()) {
    XComponentImpl* instance = new XComponentImpl(id);
    render_map_[id] = instance;
    return instance;
  }
  return render_map_[id];
}

XComponentImpl* XComponentManager::GetXComponent(const std::string& id) {
  if (render_map_.find(id) != render_map_.end()) {
    return render_map_[id];
  }

  return nullptr;
}

void XComponentManager::RemoveXComponent(const std::string& id) {
  render_map_.erase(id);
  window_status_.erase(id);
}

void XComponentManager::RegisterInputEventCallBack(
    int32_t widget_id,
    std::shared_ptr<InputEventCallBack> callback) {
  for (const auto& [xcomponentId, render] : render_map_) {
    if (render != nullptr && WindowAdapter::GetInstance().GetWidgetId(
                                 render->GetId()) == widget_id) {
      render->RegisterInputEventCallBack(callback);
    }
  }
}

// TODO: remove this method.
bool XComponentManager::StartNewWindow(const NewWindowParam& param) {
  TRACE_EVENT_1("XComponentManager::StartNewWindow", "xcomponentid", param.id);
  creating_window_ = param.id;
  window_status_.insert({param.id, std::promise<bool>()});
  if (param.using_system_floating_window) {
    SystemFloatingWindowAdapter::GetInstance().Create(param);
  } else {
    AppWindowAdapter::GetInstance().Create(param);
  }
  auto future = window_status_[param.id].get_future();
  auto status = future.wait_for(5s);
  if (status == std::future_status::timeout) {
    LOGE("XComponentManager::StartNewWindow timeout");
    creating_window_.clear();
    return false;
  }
  return true;
}

bool XComponentManager::WindowIsCreating(int32_t id) {
  if (!creating_window_.empty()) {
    std::string window_id = WindowAdapter::GetInstance().GetWindowId(id);
    if (creating_window_ != window_id) {
      return true;
    }
  }
  return false;
}

void XComponentManager::RequestLayout(const std::string& id) {
  auto render = GetXComponent(id);
  if (!render) {
    LOGE("not found render id:%{public}s", id.c_str());
    return;
  }
  render->RequestLayout();
}

void XComponentManager::OnActivationChanged(const std::string& id,
                                            bool active) {
  if (active) {
    activate_window_ = id;
  }
}

void XComponentManager::OnWidgetAvailable(const std::string& id) {
  if (window_status_.contains(id)) {
    window_status_[id].set_value(true);
  }
  creating_window_.clear();
}

void XComponentManager::OnWidgetDestroyed(const std::string& id) {
  RemoveXComponent(id);
}

}  // namespace ohos::adapter::xcomponent
