// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
