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

#include "ui/ozone/platform/ohos/host/ohos_pip_window.h"

#include <napi/native_api.h>
#include <native_window/external_window.h>

#include <string>

#include "base/ohos/task_scheduler/task_runner_ohos.h"
#include "base/strings/utf_string_conversions.h"
#include "base/threading/platform_thread.h"
#include "ohos/adapter/task_runner/main_thread_task_runner.h"
#include "ohos/adapter/window/pip_window_adapter.h"
#include "ui/aura/window.h"
#include "ui/aura/window_tree_host.h"
#include "ui/base/hit_test.h"
#include "ui/base/ui_base_features.h"
#include "ui/display/screen.h"
#include "ui/display/types/display_constants.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/ozone/platform/ohos/common/ohos_util.h"
#include "ui/ozone/platform/ohos/host/ohos_event_source.h"
#include "ui/ozone/platform/ohos/host/ohos_pip_window_manager.h"
#include "ui/ozone/platform/ohos/host/ohos_window_manager.h"

namespace ui {

constexpr std::chrono::duration kWindowCreationTimeout =
    std::chrono::seconds(3);

namespace {

void PipStartPipCallback(uint32_t controller_id,
                         uint8_t request_id,
                         uint64_t surface_id) {
  base::WeakPtr<OhosPipWindow> pip_window =
      OhosPipWindowManager::GetInstance().GetPipWindowById(controller_id);
  if (!pip_window) {
    LOG(ERROR) << "There is no corresponding instance,controller_id:"
               << controller_id;
    return;
  }
  pip_window->CreateAndRegisterNativeWindow(surface_id);
}

void PipLifecycleCallback(uint32_t controller_id,
                          PictureInPicture_PipState state,
                          int32_t err_code) {
  base::WeakPtr<OhosPipWindow> pip_window =
      OhosPipWindowManager::GetInstance().GetPipWindowById(controller_id);
  if (!pip_window) {
    LOG(ERROR) << "There is no corresponding instance,controller_id:"
               << controller_id;
    return;
  }

  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(
      FROM_HERE, base::BindOnce(&OhosPipWindow::OnPipLifecycleCallback,
                                pip_window, state, err_code));
}

void PipControlEventCallback(uint32_t controller_id,
                             PictureInPicture_PipControlType control_type,
                             PictureInPicture_PipControlStatus status) {
  base::WeakPtr<OhosPipWindow> pip_window =
      OhosPipWindowManager::GetInstance().GetPipWindowById(controller_id);
  if (!pip_window) {
    LOG(ERROR) << "There is no corresponding instance,controller_id:"
               << controller_id;
    return;
  }

  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(
      FROM_HERE, base::BindOnce(&OhosPipWindow::OnPipControlEventCallback,
                                pip_window, control_type, status));
}

void PipResizeCallback(uint32_t controller_id,
                       uint32_t width,
                       uint32_t height,
                       double scale) {
  base::WeakPtr<OhosPipWindow> pip_window =
      OhosPipWindowManager::GetInstance().GetPipWindowById(controller_id);
  if (!pip_window) {
    LOG(ERROR) << "There is no corresponding instance,controller_id:"
               << controller_id;
    return;
  }

  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(
      FROM_HERE, base::BindOnce(&OhosPipWindow::OnPipResizeCallback, pip_window,
                                width, height, scale));
}
}  // namespace

OhosPipWindow::OhosPipWindow(PlatformWindowDelegate* delegate,
                             OhosWindowManager* manager)
    : OhosWindow(delegate, manager), weak_ohos_pip_window_factory_(this) {
  state_ = PlatformWindowState::kNormal;
}

OhosPipWindow::~OhosPipWindow() {
  PipWindowAdapter::GetInstance().UnregisterAllStartPipCallbacks(
      controller_id_);
  PipWindowAdapter::GetInstance().UnregisterAllLifecycleListeners(
      controller_id_);
  PipWindowAdapter::GetInstance().UnregisterAllControlEventListeners(
      controller_id_);
  PipWindowAdapter::GetInstance().UnregisterAllResizeListeners(controller_id_);
  PipWindowAdapter::GetInstance().DeletePip(controller_id_);
  OhosPipWindowManager::GetInstance().RemovePipWindow(controller_id_);
}

void OhosPipWindow::OnPipLifecycleCallback(PictureInPicture_PipState state,
                                           int32_t err_code) {
  SetPipState(state);
  switch (state) {
    case PictureInPicture_PipState::ABOUT_TO_STOP:
      OnPipControlClose();
      break;
    case PictureInPicture_PipState::ERROR:
      LOG(ERROR) << "pip state error,err_code: " << err_code
                 << ", controller_id: " << controller_id_;
      OnPipControlClose();
      break;
    default:
      break;
  }
}

void OhosPipWindow::OnPipResizeCallback(uint32_t width,
                                        uint32_t height,
                                        double scale) {
  if (scale_ == scale) {
    return;
  }

  scale_ = scale;
  previous_bounds_in_pixels_ = bounds_in_pixels_;
  bounds_in_pixels_ =
      gfx::Rect{previous_bounds_in_pixels_.x(), previous_bounds_in_pixels_.y(),
                width * scale, height * scale};

  Applied(previous_bounds_in_pixels_, bounds_in_pixels_);
}

void OhosPipWindow::OnPipControlEventCallback(
    PictureInPicture_PipControlType control_type,
    PictureInPicture_PipControlStatus status) {
  switch (control_type) {
    case PictureInPicture_PipControlType::VIDEO_PLAY_PAUSE:
      if (status == PictureInPicture_PipControlStatus::PLAY) {
        pip_controller_->Play();
      } else {
        pip_controller_->Pause();
      }
      break;
    case PictureInPicture_PipControlType::VIDEO_PREVIOUS:
      pip_controller_->PreviousTrack();
      break;
    case PictureInPicture_PipControlType::VIDEO_NEXT:
      pip_controller_->NextTrack();
      break;
    default:
      break;
  }
}

PictureInPicture_PipConfig OhosPipWindow::ConfigurePipWindow() {
  PictureInPicture_PipConfig pip_config = nullptr;
  int32_t main_window_id =
      PipWindowAdapter::GetInstance().GetOriginWindowId(parent_widget_);
  uint32_t control_size = 1;
  PictureInPicture_PipControlGroup control_group[control_size];
  control_group[0] =
      PictureInPicture_PipControlGroup::VIDEO_PLAY_VIDEO_PREVIOUS_NEXT;

  int32_t result = PipWindowAdapter::GetInstance().CreatePipConfig(&pip_config);
  if (result != 0) {
    LOG(ERROR) << "CreatePipConfig failed, result: " << result;
  }
  result = PipWindowAdapter::GetInstance().SetPipMainWindowId(pip_config,
                                                              main_window_id);
  if (result != 0) {
    LOG(ERROR) << "SetPipMainWindowId failed, result: " << result;
  }
  result = PipWindowAdapter::GetInstance().SetPipTemplateType(
      pip_config, PictureInPicture_PipTemplateType::VIDEO_PLAY);
  if (result != 0) {
    LOG(ERROR) << "SetPipTemplateType failed, result: " << result;
  }
  result = PipWindowAdapter::GetInstance().SetPipRect(
      pip_config, bounds_in_pixels_.width(), bounds_in_pixels_.height());
  if (result != 0) {
    LOG(ERROR) << "SetPipRect failed, result: " << result;
  }
  result = PipWindowAdapter::GetInstance().SetPipControlGroup(
      pip_config, control_group, control_size);
  if (result != 0) {
    LOG(ERROR) << "SetPipControlGroup failed, result: " << result;
  }
  result = PipWindowAdapter::GetInstance().SetPipNapiEnv(
      pip_config, PipWindowAdapter::GetInstance().GetNApiEnv());
  if (result != 0) {
    LOG(ERROR) << "SetPipNapiEnv failed, result: " << result;
  }
  return pip_config;
}

void OhosPipWindow::RegisterCallbacks() {
  int32_t result = PipWindowAdapter::GetInstance().RegisterStartPipCallback(
      controller_id_, PipStartPipCallback);
  if (result != 0) {
    LOG(ERROR) << "RegisterStartPipCallback failed, result: " << result;
  }

  result = PipWindowAdapter::GetInstance().RegisterLifecycleListener(
      controller_id_, PipLifecycleCallback);
  if (result != 0) {
    LOG(ERROR) << "RegisterLifecycleListener failed, result: " << result;
  }

  result = PipWindowAdapter::GetInstance().RegisterControlEventListener(
      controller_id_, PipControlEventCallback);
  if (result != 0) {
    LOG(ERROR) << "RegisterControlEventListener failed, result: " << result;
  }

  result = PipWindowAdapter::GetInstance().RegisterResizeListener(
      controller_id_, PipResizeCallback);
  if (result != 0) {
    LOG(ERROR) << "RegisterResizeListener failed, result: " << result;
  }
}

void OhosPipWindow::CreateAndRegisterNativeWindow(uint64_t surface_id) {
  RAW_PTR_EXCLUSION OHNativeWindow* native_window;
  int32_t result = OH_NativeWindow_CreateNativeWindowFromSurfaceId(
      surface_id, &native_window);
  if (result != 0) {
    LOG(ERROR)
        << "OH_NativeWindow_CreateNativeWindowFromSurfaceId failed, result: "
        << result;
    return;
  }

  std::string window_id = GetWindowUniqueId();
  WindowAdapter::GetInstance().AddWindow(window_id, native_window);
  NotifyPipWindowCreated();
}

void OhosPipWindow::CreatePipWindow() {
  PictureInPicture_PipConfig pip_config = ConfigurePipWindow();
  if (pip_config == nullptr) {
    LOG(ERROR) << "CreatePip failed, pip_config is nullptr";
    return;
  }
  int32_t result =
      PipWindowAdapter::GetInstance().CreatePip(pip_config, &controller_id_);
  PipWindowAdapter::GetInstance().DestroyPipConfig(&pip_config);
  if (result != 0) {
    LOG(ERROR) << "CreatePip failed, result: " << result;
    return;
  }
  LOG(INFO) << "CreatePip success, controller_id: " << controller_id_;
  OhosPipWindowManager::GetInstance().AddPipWindow(controller_id_,
                                                   GetWeakPtr());

  result = PipWindowAdapter::GetInstance().UpdatePipControlStatus(
      controller_id_, PictureInPicture_PipControlType::VIDEO_PLAY_PAUSE,
      pip_controller_->IsPlayerActive()
          ? PictureInPicture_PipControlStatus::PLAY
          : PictureInPicture_PipControlStatus::PAUSE);
  if (result != 0) {
    LOG(ERROR) << "UpdatePipControlStatus failed, result:" << result;
  }

  result = PipWindowAdapter::GetInstance().SetPipInitialSurfaceRect(
      controller_id_, bounds_in_pixels_.x(), bounds_in_pixels_.y(),
      bounds_in_pixels_.width(), bounds_in_pixels_.height());
  if (result != 0) {
    LOG(ERROR) << "SetPipInitialSurfaceRect failed, result: " << result;
  }

  RegisterCallbacks();
  result = PipWindowAdapter::GetInstance().StartPip(controller_id_);
  if (result != 0) {
    LOG(ERROR) << "StartPip failed, result: " << result;
  }
  RegisterControlCallbacks();
}

void OhosPipWindow::RegisterControlCallbacks() {
  if (!pip_controller_) {
    LOG(ERROR) << "Register control callbacks failed, controller_id: "
               << controller_id_;
    return;
  }

  auto playback_state_handler =
      base::BindRepeating([](uint32_t controller_id, bool is_play) {
        PipWindowAdapter::GetInstance().UpdatePipControlStatus(
            controller_id, PictureInPicture_PipControlType::VIDEO_PLAY_PAUSE,
            is_play ? PictureInPicture_PipControlStatus::PLAY
                    : PictureInPicture_PipControlStatus::PAUSE);
      });

  auto video_previous_andler =
      base::BindRepeating([](uint32_t controller_id, bool is_enable) {
        PipWindowAdapter::GetInstance().SetPipControlEnabled(
            controller_id, PictureInPicture_PipControlType::VIDEO_PREVIOUS,
            is_enable);
      });

  auto video_next_handler = base::BindRepeating([](uint32_t controller_id,
                                                   bool is_enable) {
    PipWindowAdapter::GetInstance().SetPipControlEnabled(
        controller_id, PictureInPicture_PipControlType::VIDEO_NEXT, is_enable);
  });

  content::UpdateControlCallbacks callbacks(
      playback_state_handler, video_previous_andler, video_next_handler);
  pip_controller_->SetUpdateControllCallbacks(controller_id_, callbacks);
}

void OhosPipWindow::Hide() {
  LOG(INFO) << "OhosPipWindow::" << __func__
            << " controller_id: " << controller_id_;
  OhosWindow::Hide();
  StopPipController();
}

void OhosPipWindow::Close() {
  LOG(INFO) << "OhosPipWindow::" << __func__
            << " controller_id: " << controller_id_;
  if (!StopPipController()) {
    LOG(ERROR) << "PipController is not stopped and cannot be deleted.";
    return;
  }

  int32_t result = PipWindowAdapter::GetInstance().DeletePip(controller_id_);
  if (result != 0) {
    LOG(ERROR) << "DeletePip failed, result: " << result;
    return;
  }
  CloseInternal();
}

bool OhosPipWindow::IsVisible() const {
  return is_visible_ && PictureInPicture_PipState::STARTED == pip_state_;
}

void OhosPipWindow::SetBoundsInPixels(const gfx::Rect& bounds) {
  int32_t result = PipWindowAdapter::GetInstance().UpdatePipContentSize(
      controller_id_, bounds.width(), bounds.height());
  if (result != 0) {
    LOG(ERROR) << "OH_UpdatePipContentSize failed, result: " << result;
  }
}

void OhosPipWindow::OnInitialize(PlatformWindowInitProperties properties) {
  pip_controller_ = properties.pip_controller;
  parent_widget_ = properties.pip_parent_widget;
  OhosWindow* parent_window = window_manager()->GetWindow(parent_widget_);
  if (parent_window) {
    SetCurrentDisplayId(parent_window->GetCurrentDisplayId());
  }
}

void OhosPipWindow::NotifyPipWindowCreated() {
  pip_window_created_promise_.set_value(true);
}

bool OhosPipWindow::OnCreateWindow(WindowInitParameter param) {
  auto create_task = std::bind(&OhosPipWindow::CreatePipWindow, this);
  ohos::adapter::taskRunner::MainThreadTaskRunner::GetInstance().PostTask(
      create_task);

  std::future<bool> window_created_future =
      pip_window_created_promise_.get_future();
  if (window_created_future.wait_for(kWindowCreationTimeout) ==
      std::future_status::timeout) {
    LOG(ERROR) << "create pip window timeout, window id: " << param.window_id;
    return false;
  }
  return true;
}

WindowInitParameter OhosPipWindow::BuildWindowInitParameter() {
  WindowInitParameter parameter;

  WindowRect rect{bounds_in_pixels_.x(), bounds_in_pixels_.y(),
                  bounds_in_pixels_.width(), bounds_in_pixels_.height()};
  // Calculate initial bounds.
  parameter.bounds = rect;
  parameter.window_id = GetWindowUniqueId();
  return parameter;
}

bool OhosPipWindow::StopPipController() {
  if (pip_state_ == PictureInPicture_PipState::STOPPED) {
    return true;
  }
  int32_t result = PipWindowAdapter::GetInstance().StopPip(controller_id_);
  if (result != 0) {
    LOG(ERROR) << "StopPip failed, result: " << result;
    return false;
  }
  return true;
}

void OhosPipWindow::CloseInternal() {
  std::string window_id = GetWindowUniqueId();
  WindowAdapter::GetInstance().RemoveWindow(window_id);
}

void OhosPipWindow::SetPipState(PictureInPicture_PipState pip_state) {
  this->pip_state_ = pip_state;
}

void OhosPipWindow::OnPipControlClose() {
  if (pip_controller_) {
    pip_controller_->OnWindowDestroyed(false);
  }
}

base::WeakPtr<OhosPipWindow> OhosPipWindow::GetWeakPtr() {
  return weak_ohos_pip_window_factory_.GetWeakPtr();
}

display::Display OhosPipWindow::GetCurrentDisplay() {
  int64_t display_id = GetCurrentDisplayId();
  display::Display current_display;
  display::Screen* screen = display::Screen::GetScreen();
  screen->GetDisplayWithDisplayId(display_id, &current_display);
  return current_display;
}

}  // namespace ui
