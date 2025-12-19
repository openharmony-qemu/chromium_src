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

#ifndef OHOS_ADAPTER_APP_WINDOW_APP_WINDOW_ADAPTER_H_
#define OHOS_ADAPTER_APP_WINDOW_APP_WINDOW_ADAPTER_H_

#include <cstdint>
#include <functional>
#include <string>

#include "ohos/adapter/export.h"
#include "ohos/adapter/window/window_common.h"

namespace ohos::adapter::window {

class ADAPTER_EXPORT_API AppWindowAdapter {
 public:
  using ChangeSizeCallback = std::function<void()>;
  static AppWindowAdapter& GetInstance();

  void Create(const NewWindowParam& param);
  void Close(int32_t id);
  void Show(int32_t id);
  void Hide(int32_t id);
  void Activate(int32_t id);
  void SetFullscreen(int32_t id);
  void SetBounds(int32_t id, const WindowRect& rect);
  void SetEnabled(bool enabled, int32_t id);
  void Maximize(int32_t id);
  void UnMaximize(int32_t id);
  void Minimize(int32_t id);
  void SetWindowLimits(int32_t min_width,
                       int32_t min_height,
                       int32_t max_width,
                       int32_t max_height,
                       int32_t id);
  void SetTitle(const std::string& title, int32_t id);
  bool Relaunch();
  void StartWindowMoving(int32_t id);
  std::vector<std::string> GetWindowsByCoordinate(
      const PointCoordinate& coordinate);
  void Restore(int32_t id);
  std::vector<int32_t> GetOriginWindowIds(std::vector<int32_t> window_ids);
  bool ShiftWindowMouseEvent(const int32_t source_window_id,
                             const int32_t target_window_id);
  void StartWindowMovingWithOffset(const int32_t id,
                                   const float offset_x,
                                   const float offset_y);
  bool ShiftWindowTouchEvent(const int32_t source_id,
                             const int32_t target_id,
                             const int32_t finger_id);
  bool Bind(const std::string& id);
  bool UnBind(const std::string& id);

 private:
  AppWindowAdapter() = default;
  ~AppWindowAdapter() = default;
};

}  // namespace ohos::adapter::window
#endif  // OHOS_ADAPTER_APP_WINDOW_APP_WINDOW_ADAPTER_H_
