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

#ifndef OHOS_ADAPTER_CURSOR_CURSOR_H_
#define OHOS_ADAPTER_CURSOR_CURSOR_H_

#include <string>

#include "ohos/adapter/export.h"

namespace ohos::adapter {

enum class OhosCursorType {
  kDefault,
  kEast,
  kWest,
  kSouth,
  kNorth,
  kWestEast,
  kNorthSouth,
  kNorthEast,
  kNorthWest,
  kSouthEast,
  kSouthWest,
  kNorthEastSouthWest,
  kNorthWestSouthEast,
  kCross,
  kCursorCopy,
  kCursorForbid,
  kColorSucker,
  kHandGrabbing,
  kHandOpen,
  kHandPointing,
  kHelp,
  kMove,
  kResizeLeftRight,
  kResizeUpDown,
  kScreenshotChoose,
  kScreenshotCursor,
  kTextCursor,
  kZoomIn,
  kZoomOut,
  kMiddleBtnEast,
  kMiddleBtnWest,
  kMiddleBtnSouth,
  kMiddleBtnNorth,
  kMiddleBtnNorthSouth,
  kMiddleBtnNorthEast,
  kMiddleBtnNorthWest,
  kMiddleBtnSouthEast,
  kMiddleBtnSouthWest,
  kMiddleBtnNorthSouthWestEast,
  kHorizontalTextCursor,
  kCursorCross,
  kCursorCircle,
  kLoading,
  kRunning,
  kNone,
  kMinValue = kDefault,
  kMaxValue = kNone,
};

struct CustomCusorInfo {
  std::string id;
  int width;
  int height;
  int hotspot_x;
  int hotspot_y;
  std::shared_ptr<char[]> buff;
};

class ADAPTER_EXPORT_API Cursor {
 public:
  static Cursor& GetInstance();
  Cursor(const Cursor&) = delete;
  Cursor(Cursor&) = delete;
  Cursor& operator=(const Cursor&) = delete;

  void SetCursorVisible(const bool visible);
  void SetCursor(int32_t id, const OhosCursorType cursor_type);
  void SetCustomCursor(CustomCusorInfo& cursor_info);
  bool IsCursorLocked();
  bool SupportsCursorLock();
  bool LockCursor(int32_t widget_id);
  bool UnlockCursor(int32_t widget_id);

 private:
  Cursor() = default;
  ~Cursor() = default;
  bool cursor_locked_ = false;
};
}  // namespace ohos::adapter
#endif  // OHOS_ADAPTER_CURSOR_CURSOR_H_
