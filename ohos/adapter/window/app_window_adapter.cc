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

#include "ohos/adapter/window/app_window_adapter.h"

#include "ohos/adapter/aki_hook/aki_hook.h"

namespace ohos::adapter::window {

AppWindowAdapter& AppWindowAdapter::GetInstance() {
  static AppWindowAdapter instance;
  return instance;
}

void AppWindowAdapter::Create(const NewWindowParam& param) {
  auto jsFunc = ohos::adapter::GetJSFunction("AppWindow.CreateWindow");
  if (jsFunc) {
    jsFunc->Invoke<void>(param);
  }
}

void AppWindowAdapter::Close(int32_t id) {
  auto jsFunc = ohos::adapter::GetJSFunction("AppWindow.CloseWindow");
  if (jsFunc) {
    jsFunc->Invoke<void>(id);
  }
}

void AppWindowAdapter::Show(int32_t id) {
  auto jsFunc = ohos::adapter::GetJSFunction("AppWindow.ShowWindow");
  if (jsFunc) {
    jsFunc->Invoke<void>(id);
  }
}

void AppWindowAdapter::Hide(int32_t id) {}

void AppWindowAdapter::Activate(int32_t id) {
  auto jsFunc = ohos::adapter::GetJSFunction("AppWindow.ActivateWindow");
  if (jsFunc) {
    jsFunc->Invoke<void>(id);
  }
}

void AppWindowAdapter::SetFullscreen(int32_t id) {
  auto jsFunc = ohos::adapter::GetJSFunction("AppWindow.SetFullscreen");
  if (jsFunc) {
    jsFunc->Invoke<void>(id);
  }
}

void AppWindowAdapter::SetBounds(int32_t id,
                                 const WindowRect& rect,
                                 ChangeSizeCallback callback) {
  auto jsFunc = ohos::adapter::GetJSFunction("AppWindow.SetBounds");
  if (jsFunc) {
    jsFunc->Invoke<void>(id, rect, callback);
  }
}

void AppWindowAdapter::SetEnabled(bool enable, int32_t id) {
  auto jsFunc = ohos::adapter::GetJSFunction("AppWindow.SetEnabled");
  if (jsFunc) {
    jsFunc->Invoke<void>(enable, id);
  }
}

void AppWindowAdapter::Maximize(int32_t id) {
  auto jsFunc = ohos::adapter::GetJSFunction("AppWindow.Maximize");
  if (jsFunc) {
    jsFunc->Invoke<void>(id);
  }
}

void AppWindowAdapter::UnMaximize(int32_t id) {
  auto jsFunc = ohos::adapter::GetJSFunction("AppWindow.UnMaximize");
  if (jsFunc) {
    jsFunc->Invoke<void>(id);
  }
}

void AppWindowAdapter::Minimize(int32_t id) {
  auto jsFunc = ohos::adapter::GetJSFunction("AppWindow.Minimize");
  if (jsFunc) {
    jsFunc->Invoke<void>(id);
  }
}

void AppWindowAdapter::SetWindowLimits(int32_t min_width,
                                       int32_t min_height,
                                       int32_t max_width,
                                       int32_t max_height,
                                       int32_t id) {
  auto jsFunc = ohos::adapter::GetJSFunction("AppWindow.SetWindowLimits");
  if (jsFunc) {
    jsFunc->Invoke<void>(min_width, min_height, max_width, max_height, id);
  }
}

void AppWindowAdapter::SetTitle(const std::string& title, int32_t id) {
  auto jsFunc = ohos::adapter::GetJSFunction("AppWindow.SetTitle");
  if (jsFunc) {
    jsFunc->Invoke<void>(title, id);
  }
}

bool AppWindowAdapter::Relaunch() {
  auto jsFunc = ohos::adapter::GetJSFunction("AppWindow.Relaunch");
  if (jsFunc) {
    return jsFunc->Invoke<bool>();
  }
  return false;
}

void AppWindowAdapter::StartWindowMoving(int32_t id) {
  auto jsFunc = ohos::adapter::GetJSFunction("AppWindow.StartWindowMoving");
  if (jsFunc) {
    jsFunc->Invoke<void>(id);
  }
}

std::vector<std::string> AppWindowAdapter::GetWindowsByCoordinate(
    const PointCoordinate& coordinate) {
  std::promise<std::vector<std::string>> promise;
  std::function<void(std::vector<std::string>)> callback =
      [&](std::vector<std::string> window_ids) -> void {
    promise.set_value(window_ids);
  };
  auto func = ohos::adapter::GetJSFunction("AppWindow.GetWindowsByCoordinate");
  if (func) {
    func->Invoke<void>(coordinate, callback);
  }
  std::vector<std::string> result = promise.get_future().get();
  return result;
}

void AppWindowAdapter::Restore(int32_t id) {
  auto jsFunc = ohos::adapter::GetJSFunction("AppWindow.Restore");
  if (jsFunc) {
    jsFunc->Invoke<void>(id);
  }
}

void AppWindowAdapter::StartWindowMovingWithOffset(const int32_t id,
                                                   const float offset_x,
                                                   const float offset_y) {
  auto jsFunc =
      ohos::adapter::GetJSFunction("AppWindow.StartWindowMovingWithOffset");
  if (jsFunc) {
    jsFunc->Invoke<void>(id, offset_x, offset_y);
  }
}

std::vector<int32_t> AppWindowAdapter::GetOriginWindowIds(
    std::vector<int32_t> window_ids) {
  auto jsFunc = ohos::adapter::GetJSFunction("AppWindow.GetOriginWindowIds");
  if (jsFunc) {
    return jsFunc->Invoke<std::vector<int32_t>>(window_ids);
  }
  return std::vector<int32_t>();
}

bool AppWindowAdapter::ShiftWindowEvent(const int32_t source_id,
                                        const int32_t target_id) {
  auto jsFunc = ohos::adapter::GetJSFunction("AppWindow.ShiftWindowEvent");
  if (jsFunc) {
    auto promise = std::make_shared<std::promise<bool>>();
    auto future = promise->get_future();
    std::function<void(bool)> callback = [promise](bool result) -> void {
      promise->set_value(result);
    };
    jsFunc->Invoke<void>(source_id, target_id, callback);
    auto status = future.wait_for(std::chrono::seconds(3));
    if (status == std::future_status::timeout) {
      LOGE("AppWindowAdapter::ShiftWindowEvent Wait timeout");
      return false;
    }
    bool result = future.get();
    LOGI("[OhosTabDrag] AppWindowAdapter::ShiftWindowEvent future result:%{public}d", result);
    return result;
  }
  return false;
}

}  // namespace ohos::adapter::window
