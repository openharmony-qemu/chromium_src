/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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

#include "ohos/adapter/window/pip_window_adapter.h"

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/window/app_window_adapter.h"

namespace ohos::adapter::window {

PipWindowAdapter& PipWindowAdapter::GetInstance() {
  static PipWindowAdapter instance;
  return instance;
}

bool PipWindowAdapter::IsSupportNativePipWindow() const {
  return is_support_native_pip_window_;
}

PipWindowAdapter::PipWindowAdapter()
    : native_pip_lib_("native_window_manager") {
  if (is_support_native_pip_window_) {
    LOGE("Native pip window is already supported");
    return;
  }

  if (!native_pip_lib_.IsLoaded()) {
    LOGE("PipWindowAdapter load libnative_window_manager.so failed");
    return;
  }

  if (!LoadAllPipWindowFunctions()) {
    LOGE("PipWindowAdapter load functions failed");
    return;
  }

  LOGI("PipWindowAdapter Initialize success, support native pip window");
  is_support_native_pip_window_ = true;
}

PipWindowAdapter::~PipWindowAdapter() {
  pip_delete_pip_func_ = nullptr;
  pip_start_pip_func_ = nullptr;
  pip_stop_pip_func_ = nullptr;
  pip_update_content_size_func_ = nullptr;
  pip_unregister_pip_start_callbacks_ = nullptr;
  pip_unregister_lifecycle_callbacks_ = nullptr;
  pip_unregister_control_event_callbacks_ = nullptr;
  pip_unregister_resize_listener_callbacks_ = nullptr;
  pip_create_pip_config_func_ = nullptr;
  pip_destroy_pip_config_func_ = nullptr;
  pip_set_pip_main_window_id_func_ = nullptr;
  pip_set_pip_tempplate_type_func_ = nullptr;
  pip_set_pip_rect_func_ = nullptr;
  pip_set_pip_control_group_func_ = nullptr;
  pip_set_pip_napi_env_func_ = nullptr;
  pip_register_start_pip_callback_func_ = nullptr;
  pip_register_lifecycle_listener_func_ = nullptr;
  pip_register_control_event_listener_func_ = nullptr;
  pip_register_resize_listener_func_ = nullptr;
  pip_update_pip_control_status_func_ = nullptr;
  pip_set_pip_initial_surface_rect_func_ = nullptr;
  pip_set_pip_control_enable_func_ = nullptr;
}

bool PipWindowAdapter::LoadAllPipWindowFunctions() {
  return native_pip_lib_.LoadFunction(&pip_create_pip_func_,
                                      "OH_PictureInPicture_CreatePip") &&
         native_pip_lib_.LoadFunction(&pip_delete_pip_func_,
                                      "OH_PictureInPicture_DeletePip") &&
         native_pip_lib_.LoadFunction(&pip_start_pip_func_,
                                      "OH_PictureInPicture_StartPip") &&
         native_pip_lib_.LoadFunction(&pip_stop_pip_func_,
                                      "OH_PictureInPicture_StopPip") &&
         native_pip_lib_.LoadFunction(
             &pip_update_content_size_func_,
             "OH_PictureInPicture_UpdatePipContentSize") &&
         native_pip_lib_.LoadFunction(
             &pip_unregister_pip_start_callbacks_,
             "OH_PictureInPicture_UnregisterAllStartPipCallbacks") &&
         native_pip_lib_.LoadFunction(
             &pip_unregister_lifecycle_callbacks_,
             "OH_PictureInPicture_UnregisterAllLifecycleListeners") &&
         native_pip_lib_.LoadFunction(
             &pip_unregister_control_event_callbacks_,
             "OH_PictureInPicture_UnregisterAllControlEventListeners") &&
         native_pip_lib_.LoadFunction(
             &pip_unregister_resize_listener_callbacks_,
             "OH_PictureInPicture_UnregisterAllResizeListeners") &&
         native_pip_lib_.LoadFunction(&pip_create_pip_config_func_,
                                      "OH_PictureInPicture_CreatePipConfig") &&
         native_pip_lib_.LoadFunction(&pip_destroy_pip_config_func_,
                                      "OH_PictureInPicture_DestroyPipConfig") &&
         native_pip_lib_.LoadFunction(
             &pip_set_pip_main_window_id_func_,
             "OH_PictureInPicture_SetPipMainWindowId") &&
         native_pip_lib_.LoadFunction(
             &pip_set_pip_tempplate_type_func_,
             "OH_PictureInPicture_SetPipTemplateType") &&
         native_pip_lib_.LoadFunction(&pip_set_pip_rect_func_,
                                      "OH_PictureInPicture_SetPipRect") &&
         native_pip_lib_.LoadFunction(
             &pip_set_pip_control_group_func_,
             "OH_PictureInPicture_SetPipControlGroup") &&
         native_pip_lib_.LoadFunction(&pip_set_pip_napi_env_func_,
                                      "OH_PictureInPicture_SetPipNapiEnv") &&
         native_pip_lib_.LoadFunction(
             &pip_register_start_pip_callback_func_,
             "OH_PictureInPicture_RegisterStartPipCallback") &&
         native_pip_lib_.LoadFunction(
             &pip_register_lifecycle_listener_func_,
             "OH_PictureInPicture_RegisterLifecycleListener") &&
         native_pip_lib_.LoadFunction(
             &pip_register_control_event_listener_func_,
             "OH_PictureInPicture_RegisterControlEventListener") &&
         native_pip_lib_.LoadFunction(
             &pip_register_resize_listener_func_,
             "OH_PictureInPicture_RegisterResizeListener") &&
         native_pip_lib_.LoadFunction(
             &pip_update_pip_control_status_func_,
             "OH_PictureInPicture_UpdatePipControlStatus") &&
         native_pip_lib_.LoadFunction(
             &pip_set_pip_initial_surface_rect_func_,
             "OH_PictureInPicture_SetPipInitialSurfaceRect") &&
         native_pip_lib_.LoadFunction(
             &pip_set_pip_control_enable_func_,
             "OH_PictureInPicture_SetPipControlEnabled");
}

napi_env PipWindowAdapter::GetNApiEnv() {
  return aki::JSBind::GetScopedEnv();
}

uint32_t PipWindowAdapter::GetOriginWindowId(int32_t widget) {
  std::vector<int32_t> ids =
      AppWindowAdapter::GetInstance().GetOriginWindowIds({widget});
  if (!ids.empty()) {
    return static_cast<uint32_t>(ids[0]);
  }
  return 0;
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t PipWindowAdapter::CreatePip(PictureInPicture_PipConfig pip_config,
                                    uint32_t* controller_id) {
  if (pip_create_pip_func_ == nullptr) {
    LOGE("PipWindowAdapter::CreatePip func null");
    return -1;
  }
  return pip_create_pip_func_(pip_config, controller_id);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t PipWindowAdapter::DeletePip(uint32_t controller_id) {
  if (pip_delete_pip_func_ == nullptr) {
    LOGE("PipWindowAdapter::DeletePip func null");
    return -1;
  }
  return pip_delete_pip_func_(controller_id);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t PipWindowAdapter::StartPip(uint32_t controller_id) {
  if (pip_start_pip_func_ == nullptr) {
    LOGE("PipWindowAdapter::StartPip func null");
    return -1;
  }
  return pip_start_pip_func_(controller_id);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t PipWindowAdapter::StopPip(uint32_t controller_id) {
  if (pip_stop_pip_func_ == nullptr) {
    LOGE("PipWindowAdapter::StopPip func null");
    return -1;
  }
  return pip_stop_pip_func_(controller_id);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t PipWindowAdapter::UpdatePipContentSize(uint32_t controller_id,
                                               uint32_t width,
                                               uint32_t height) {
  if (pip_update_content_size_func_ == nullptr) {
    LOGE("PipWindowAdapter::UpdatePipContentSize func null");
    return -1;
  }
  return pip_update_content_size_func_(controller_id, width, height);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t PipWindowAdapter::UnregisterAllStartPipCallbacks(
    uint32_t controller_id) {
  if (pip_unregister_pip_start_callbacks_ == nullptr) {
    LOGE("PipWindowAdapter::UnregisterAllStartPipCallbacks func null");
    return -1;
  }
  return pip_unregister_pip_start_callbacks_(controller_id);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t PipWindowAdapter::UnregisterAllLifecycleListeners(
    uint32_t controller_id) {
  if (pip_unregister_lifecycle_callbacks_ == nullptr) {
    LOGE("PipWindowAdapter::UnregisterAllLifecycleListeners func null");
    return -1;
  }
  return pip_unregister_lifecycle_callbacks_(controller_id);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t PipWindowAdapter::UnregisterAllControlEventListeners(
    uint32_t controller_id) {
  if (pip_unregister_control_event_callbacks_ == nullptr) {
    LOGE("PipWindowAdapter::UnregisterAllControlEventListeners func null");
    return -1;
  }
  return pip_unregister_control_event_callbacks_(controller_id);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t PipWindowAdapter::UnregisterAllResizeListeners(uint32_t controller_id) {
  if (pip_unregister_resize_listener_callbacks_ == nullptr) {
    LOGE("PipWindowAdapter::UnregisterAllResizeListeners func null");
    return -1;
  }
  return pip_unregister_resize_listener_callbacks_(controller_id);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t PipWindowAdapter::CreatePipConfig(
    PictureInPicture_PipConfig* pip_config) {
  if (pip_create_pip_config_func_ == nullptr) {
    LOGE("PipWindowAdapter::CreatePipConfig func null");
    return -1;
  }
  return pip_create_pip_config_func_(pip_config);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t PipWindowAdapter::DestroyPipConfig(
    PictureInPicture_PipConfig* pip_config) {
  if (pip_destroy_pip_config_func_ == nullptr) {
    LOGE("PipWindowAdapter::DestroyPipConfig func null");
    return -1;
  }
  return pip_destroy_pip_config_func_(pip_config);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t PipWindowAdapter::SetPipMainWindowId(
    PictureInPicture_PipConfig pip_config, uint32_t main_window_id) {
  if (pip_set_pip_main_window_id_func_ == nullptr) {
    LOGE("PipWindowAdapter::SetPipMainWindowId func null");
    return -1;
  }
  return pip_set_pip_main_window_id_func_(pip_config, main_window_id);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t PipWindowAdapter::SetPipTemplateType(
    PictureInPicture_PipConfig pip_config,
    PictureInPicture_PipTemplateType pip_template_type) {
  if (pip_set_pip_tempplate_type_func_ == nullptr) {
    LOGE("PipWindowAdapter::SetPipTemplateType func null");
    return -1;
  }
  return pip_set_pip_tempplate_type_func_(pip_config, pip_template_type);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t PipWindowAdapter::SetPipRect(PictureInPicture_PipConfig pip_config,
                                     uint32_t width,
                                     uint32_t height) {
  if (pip_set_pip_rect_func_ == nullptr) {
    LOGE("PipWindowAdapter::SetPipRect func null");
    return -1;
  }
  return pip_set_pip_rect_func_(pip_config, width, height);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t PipWindowAdapter::SetPipControlGroup(
    PictureInPicture_PipConfig pip_config,
    PictureInPicture_PipControlGroup* control_group,
    uint8_t control_group_length) {
  if (pip_set_pip_control_group_func_ == nullptr) {
    LOGE("PipWindowAdapter::SetPipControlGroup func null");
    return -1;
  }
  return pip_set_pip_control_group_func_(pip_config, control_group,
                                         control_group_length);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t PipWindowAdapter::SetPipNapiEnv(PictureInPicture_PipConfig pip_config,
                                        napi_env env) {
  if (pip_set_pip_napi_env_func_ == nullptr) {
    LOGE("PipWindowAdapter::SetPipNapiEnv func null");
    return -1;
  }
  return pip_set_pip_napi_env_func_(pip_config, env);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t PipWindowAdapter::RegisterStartPipCallback(
    uint32_t controller_id, WebPipStartPipCallback callback) {
  if (pip_register_start_pip_callback_func_ == nullptr) {
    LOGE("PipWindowAdapter::RegisterStartPipCallback func null");
    return -1;
  }
  return pip_register_start_pip_callback_func_(controller_id, callback);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t PipWindowAdapter::RegisterLifecycleListener(
    uint32_t controller_id, WebPipLifecycleCallback callback) {
  if (pip_register_lifecycle_listener_func_ == nullptr) {
    LOGE("PipWindowAdapter::RegisterLifecycleListener func null");
    return -1;
  }
  return pip_register_lifecycle_listener_func_(controller_id, callback);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t PipWindowAdapter::RegisterControlEventListener(
    uint32_t controller_id, WebPipControlEventCallback callback) {
  if (pip_register_control_event_listener_func_ == nullptr) {
    LOGE("PipWindowAdapter::RegisterControlEventListener func null");
    return -1;
  }
  return pip_register_control_event_listener_func_(controller_id, callback);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t PipWindowAdapter::RegisterResizeListener(
    uint32_t controller_id, WebPipResizeCallback callback) {
  if (pip_register_resize_listener_func_ == nullptr) {
    LOGE("PipWindowAdapter::RegisterResizeListener func null");
    return -1;
  }
  return pip_register_resize_listener_func_(controller_id, callback);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t PipWindowAdapter::UpdatePipControlStatus(
    uint32_t controller_id,
    PictureInPicture_PipControlType control_type,
    PictureInPicture_PipControlStatus status) {
  if (pip_update_pip_control_status_func_ == nullptr) {
    LOGE("PipWindowAdapter::UpdatePipControlStatus func null");
    return -1;
  }
  return pip_update_pip_control_status_func_(controller_id, control_type,
                                             status);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t PipWindowAdapter::SetPipInitialSurfaceRect(uint32_t controller_id,
                                                   int32_t position_x,
                                                   int32_t position_y,
                                                   uint32_t width,
                                                   uint32_t height) {
  if (pip_set_pip_initial_surface_rect_func_ == nullptr) {
    LOGE("PipWindowAdapter::SetPipInitialSurfaceRect func null");
    return -1;
  }
  return pip_set_pip_initial_surface_rect_func_(controller_id, position_x,
                                                position_y, width, height);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
int32_t PipWindowAdapter::SetPipControlEnabled(
    uint32_t controller_id,
    PictureInPicture_PipControlType control_type,
    bool enabled) {
  if (pip_set_pip_control_enable_func_ == nullptr) {
    LOGE("PipWindowAdapter::SetPipControlEnabled func null");
    return -1;
  }
  return pip_set_pip_control_enable_func_(controller_id, control_type, enabled);
}
}  // namespace ohos::adapter::window
