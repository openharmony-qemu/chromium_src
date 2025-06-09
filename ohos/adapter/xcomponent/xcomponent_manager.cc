/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ohos/adapter/xcomponent/xcomponent_manager.h"

#include <chrono>

#include "ohos/adapter/accessibility/accessibility_adapter.h"
#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/common/trace.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"

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
  newParam.use_floating_window = param.use_floating_window;
  newParam.use_dark_mode = param.use_dark_mode;
  newParam.is_stateless = param.is_stateless;
  newParam.caption_button_visible = param.caption_button_visible;
  return newParam;
}
}

XComponentManager* XComponentManager::manager_ = nullptr;
XComponentManager* XComponentManager::GetInstance() {
  if (manager_ == nullptr) {
    manager_ = new XComponentManager();
  }
  return manager_;
}

XComponentManager::~XComponentManager() {
  render_map_.clear();
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

std::shared_ptr<XComponentImpl>
XComponentManager::GetXComponent(const std::string& id) {
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
  std::lock_guard<std::mutex> lock_protect(mutex_);
  for (const auto& [xcomponentId, render] : render_map_) {
    if (render != nullptr &&
        WindowAdapter::GetInstance().GetWidgetId(
            render->GetId()) == widget_id) {
      render->RegisterInputEventCallBack(callback);
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
  std::string create_id = param.window_id;
  // no available windows, need to create
  if (reuse_window_.empty()) {
    creating_window_ = param.window_id;
    window_status_.insert({param.window_id, std::promise<bool>()});
    if (param.use_floating_window) {
      SystemFloatingWindowAdapter::GetInstance().Create(param);
    } else {
      AppWindowAdapter::GetInstance().Create(param);
    }
    auto future = window_status_[param.window_id].get_future();
    auto status = future.wait_for(std::chrono::seconds(3));
    if (status == std::future_status::timeout) {
      LOGE("XComponentManager::CreateWindow %{public}s timeout", param.window_id.c_str());
      creating_window_.clear();
      create_id.clear();
    }
  } else {
    // If arkui has already created a window, use it directly
    create_id = reuse_window_.front();
    reuse_window_.pop();
  }
  return create_id;
}

std::string XComponentManager::CreateSubWindow(const NewWindowParam& param) {
  return SubWindowAdapter::GetInstance().Create(param);
}

void XComponentManager::RequestLayout(const std::string& id) {
  auto render = GetXComponent(id);
  if (!render) {
    LOGE("not found render id:%{public}s", id.c_str());
    return;
  }
  render->RequestLayout();
}

void XComponentManager::OnActivationChanged(const std::string& id, bool active) {
  std::lock_guard<std::mutex> lock_protect(mutex_);
  auto render = GetXComponent(id);
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
  std::lock_guard<std::mutex> lock_protect(mutex_);
  auto render = GetXComponent(id);
  if (!render || render->GetType() == XComponentType::kSubWindow) {
    LOGI("filter sub window render: %{public}s", id.c_str());
    return;
  }
  // window created by the browser ui
  if (window_status_.contains(id)) {
    window_status_[id].set_value(true);
  } else {
    // window created by the ark ui, need to reuse it
    reuse_window_.push(id);
  }
  creating_window_.clear();

  // lost surface focus event in create xcomponent on OH platform, trigger manually
  SetActivateWindow(id);
}

void XComponentManager::OnWidgetDestroyed(const std::string& id) {
  std::lock_guard<std::mutex> lock_protect(mutex_);
  RemoveXComponent(id);
}

void XComponentManager::SetActivateWindow(const std::string& id) {
  activate_window_ = id;

  auto jsFunc = ohos::adapter::GetJSFunction("ContextAdapter.SetActiveWindow");
  if (jsFunc) {
    jsFunc->Invoke<void>(id);
  }
}

}  // namespace ohos::adapter::xcomponent
