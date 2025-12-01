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

#ifndef OHOS_ADAPTER_WINDOW_PIP_WINDOW_ADAPTER_H_
#define OHOS_ADAPTER_WINDOW_PIP_WINDOW_ADAPTER_H_

#include <napi/native_api.h>

#include "ohos/adapter/common/shared_library.h"
#include "ohos/adapter/export.h"

namespace ohos::adapter::window {

using PictureInPicture_PipConfig = void*;

enum class PictureInPicture_PipTemplateType {
  VIDEO_PLAY = 0,
  VIDEO_CALL = 1,
  VIDEO_MEETING = 2,
  VIDEO_LIVE = 3
};

enum class PictureInPicture_PipControlGroup {
  VIDEO_PLAY_VIDEO_PREVIOUS_NEXT = 101,
  VIDEO_PLAY_FAST_FORWARD_BACKWARD = 102,
};

enum class PictureInPicture_PipControlType {
  VIDEO_PLAY_PAUSE = 0,
  VIDEO_PREVIOUS = 1,
  VIDEO_NEXT = 2,
};

enum class PictureInPicture_PipState {
  ABOUT_TO_START = 1,
  STARTED = 2,
  ABOUT_TO_STOP = 3,
  STOPPED = 4,
  ABOUT_TO_RESTORE = 5,
  ERROR = 6,
};

enum class PictureInPicture_PipControlStatus {
  PLAY = 1,
  PAUSE = 0,
};

using WebPipStartPipCallback = void (*)(uint32_t, uint8_t, uint64_t);
using WebPipLifecycleCallback = void (*)(uint32_t,
                                         PictureInPicture_PipState,
                                         int32_t);
using WebPipControlEventCallback = void (*)(uint32_t,
                                            PictureInPicture_PipControlType,
                                            PictureInPicture_PipControlStatus);
using WebPipResizeCallback = void (*)(uint32_t, uint32_t, uint32_t, double);

using PictureInPictureAction = int32_t(uint32_t);
using PictureInPictureUpdatePipContentSize = int32_t(uint32_t,
                                                     uint32_t,
                                                     uint32_t);
using PictureInPictureCreatePip = int32_t(void*, uint32_t*);
using PictureInPicturePipConfigAction = int32_t(void**);
using PictureInPictureSetPipMainWindowId = int32_t(void*, int32_t);
using PictureInPictureSetPipTemplateType =
    int32_t(void*, PictureInPicture_PipTemplateType);
using PictureInPictureSetPipRect = int32_t(void*, uint32_t, uint32_t);
using PictureInPictureSetPipControlGroup =
    int32_t(void*, PictureInPicture_PipControlGroup*, uint8_t);
using PictureInPictureSetPipNapiEnv = int32_t(void*, void*);

using PictureInPictureRegisterStartPipCallback =
    int32_t(uint32_t, WebPipStartPipCallback);
using PictureInPictureRegisterLifecycleListener =
    int32_t(uint32_t, WebPipLifecycleCallback);
using PictureInPictureRegisterControlEventListener =
    int32_t(uint32_t, WebPipControlEventCallback);
using PictureInPictureRegisterResizeListener = int32_t(uint32_t,
                                                       WebPipResizeCallback);

using PictureInPictureUpdatePipControlStatus =
    int32_t(uint32_t,
            PictureInPicture_PipControlType,
            PictureInPicture_PipControlStatus);
using PictureInPictureSetPipInitialSurfaceRect =
    int32_t(uint32_t, uint32_t, int32_t, int32_t, int32_t);
using PictureInPictureSetPipControlEnabled =
    int32_t(uint32_t, PictureInPicture_PipControlType, bool);

class ADAPTER_EXPORT_API PipWindowAdapter {
 public:
  static PipWindowAdapter& GetInstance();
  PipWindowAdapter(const PipWindowAdapter&) = delete;
  PipWindowAdapter& operator=(const PipWindowAdapter&) = delete;
  uint32_t GetOriginWindowId(int32_t widget);
  napi_env GetNApiEnv();
  bool IsSupportNativePipWindow() const;

  int32_t DeletePip(uint32_t controller_id);
  int32_t StartPip(uint32_t controller_id);
  int32_t StopPip(uint32_t controller_id);
  int32_t CreatePip(PictureInPicture_PipConfig pip_config,
                    uint32_t* controller_id);
  int32_t UpdatePipContentSize(uint32_t controllerId,
                               uint32_t width,
                               uint32_t height);
  int32_t UnregisterAllStartPipCallbacks(uint32_t controller_id);
  int32_t UnregisterAllLifecycleListeners(uint32_t controller_id);
  int32_t UnregisterAllControlEventListeners(uint32_t controller_id);
  int32_t UnregisterAllResizeListeners(uint32_t controller_id);
  int32_t CreatePipConfig(PictureInPicture_PipConfig* pip_config);
  int32_t DestroyPipConfig(PictureInPicture_PipConfig* pip_config);
  int32_t SetPipMainWindowId(PictureInPicture_PipConfig pip_config,
                             uint32_t main_window_id);
  int32_t SetPipTemplateType(
      PictureInPicture_PipConfig pip_config,
      PictureInPicture_PipTemplateType pip_template_type);
  int32_t SetPipRect(PictureInPicture_PipConfig pip_config,
                     uint32_t width,
                     uint32_t height);
  int32_t SetPipControlGroup(PictureInPicture_PipConfig pip_config,
                             PictureInPicture_PipControlGroup* control_group,
                             uint8_t control_group_length);
  int32_t SetPipNapiEnv(PictureInPicture_PipConfig pip_config, napi_env env);
  int32_t RegisterStartPipCallback(uint32_t controller_id,
                                   WebPipStartPipCallback callback);
  int32_t RegisterLifecycleListener(uint32_t controller_id,
                                    WebPipLifecycleCallback callback);
  int32_t RegisterControlEventListener(uint32_t controller_id,
                                       WebPipControlEventCallback callback);
  int32_t RegisterResizeListener(uint32_t controller_id,
                                 WebPipResizeCallback callback);
  int32_t UpdatePipControlStatus(uint32_t controller_id,
                                 PictureInPicture_PipControlType control_type,
                                 PictureInPicture_PipControlStatus status);
  int32_t SetPipInitialSurfaceRect(uint32_t controller_id,
                                   int32_t position_x,
                                   int32_t position_y,
                                   uint32_t width,
                                   uint32_t height);
  int32_t SetPipControlEnabled(uint32_t controller_id,
                               PictureInPicture_PipControlType control_type,
                               bool enabled);

 private:
  bool LoadAllPipWindowFunctions();
  bool is_support_native_pip_window_ = false;
  common::SharedLibrary native_pip_lib_;
  PipWindowAdapter();
  ~PipWindowAdapter();

  PictureInPictureCreatePip* pip_create_pip_func_ = nullptr;
  PictureInPictureAction* pip_delete_pip_func_ = nullptr;
  PictureInPictureAction* pip_start_pip_func_ = nullptr;
  PictureInPictureAction* pip_stop_pip_func_ = nullptr;
  PictureInPictureUpdatePipContentSize* pip_update_content_size_func_ = nullptr;

  PictureInPictureAction* pip_unregister_pip_start_callbacks_ = nullptr;
  PictureInPictureAction* pip_unregister_lifecycle_callbacks_ = nullptr;
  PictureInPictureAction* pip_unregister_control_event_callbacks_ = nullptr;
  PictureInPictureAction* pip_unregister_resize_listener_callbacks_ = nullptr;

  PictureInPicturePipConfigAction* pip_create_pip_config_func_ = nullptr;
  PictureInPicturePipConfigAction* pip_destroy_pip_config_func_ = nullptr;
  PictureInPictureSetPipMainWindowId* pip_set_pip_main_window_id_func_ =
      nullptr;
  PictureInPictureSetPipTemplateType* pip_set_pip_tempplate_type_func_ =
      nullptr;
  PictureInPictureSetPipRect* pip_set_pip_rect_func_ = nullptr;
  PictureInPictureSetPipControlGroup* pip_set_pip_control_group_func_ = nullptr;
  PictureInPictureSetPipNapiEnv* pip_set_pip_napi_env_func_ = nullptr;

  PictureInPictureRegisterStartPipCallback*
      pip_register_start_pip_callback_func_ = nullptr;
  PictureInPictureRegisterLifecycleListener*
      pip_register_lifecycle_listener_func_ = nullptr;
  PictureInPictureRegisterControlEventListener*
      pip_register_control_event_listener_func_ = nullptr;
  PictureInPictureRegisterResizeListener* pip_register_resize_listener_func_ =
      nullptr;

  PictureInPictureUpdatePipControlStatus* pip_update_pip_control_status_func_ =
      nullptr;
  PictureInPictureSetPipInitialSurfaceRect*
      pip_set_pip_initial_surface_rect_func_ = nullptr;
  PictureInPictureSetPipControlEnabled* pip_set_pip_control_enable_func_ =
      nullptr;
};

}  // namespace ohos::adapter::window
#endif  // OHOS_ADAPTER_APP_WINDOW_APP_WINDOW_ADAPTER_H_
