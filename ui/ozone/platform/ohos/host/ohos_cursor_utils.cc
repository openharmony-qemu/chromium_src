// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/ozone/platform/ohos/host/ohos_cursor_utils.h"

#include "ohos/adapter/cursor/cursor.h"

namespace ui {
using ohos::adapter::OhosCursorType;
using ui::mojom::CursorType;

#define CURSOR_CODE(mojo_type, ohos_type) ohos_type

constexpr OhosCursorType
    kLookupTable[static_cast<int32_t>(CursorType::kMaxValue) + 1] = {
        CURSOR_CODE(CursorType::kPointer, OhosCursorType::kDefault),
        CURSOR_CODE(CursorType::kCross, OhosCursorType::kCross),
        CURSOR_CODE(CursorType::kHand, OhosCursorType::kHandPointing),
        CURSOR_CODE(CursorType::kIBeam, OhosCursorType::kTextCursor),
        CURSOR_CODE(CursorType::kWait, OhosCursorType::kLoading),
        CURSOR_CODE(CursorType::kHelp, OhosCursorType::kHelp),
        CURSOR_CODE(CursorType::kEastResize, OhosCursorType::kEast),
        CURSOR_CODE(CursorType::kNorthResize, OhosCursorType::kNorth),
        CURSOR_CODE(CursorType::kNorthEastResize, OhosCursorType::kNorthEast),
        CURSOR_CODE(CursorType::kNorthWestResize, OhosCursorType::kNorthWest),
        CURSOR_CODE(CursorType::kSouthResize, OhosCursorType::kSouth),
        CURSOR_CODE(CursorType::kSouthEastResize, OhosCursorType::kSouthEast),
        CURSOR_CODE(CursorType::kSouthWestResize, OhosCursorType::kSouthWest),
        CURSOR_CODE(CursorType::kWestResize, OhosCursorType::kWest),
        CURSOR_CODE(CursorType::kNorthSouthResize, OhosCursorType::kNorthSouth),
        CURSOR_CODE(CursorType::kEastWestResize, OhosCursorType::kWestEast),
        CURSOR_CODE(CursorType::kNorthEastSouthWestResize,
                    OhosCursorType::kNorthEastSouthWest),
        CURSOR_CODE(CursorType::kNorthWestSouthEastResize,
                    OhosCursorType::kNorthWestSouthEast),
        CURSOR_CODE(CursorType::kColumnResize,
                    OhosCursorType::kResizeLeftRight),
        CURSOR_CODE(CursorType::kRowResize, OhosCursorType::kResizeUpDown),
        CURSOR_CODE(CursorType::kMiddlePanning,
                    OhosCursorType::kMiddleBtnNorthSouthWestEast),
        CURSOR_CODE(CursorType::kEastPanning, OhosCursorType::kMiddleBtnEast),
        CURSOR_CODE(CursorType::kNorthPanning, OhosCursorType::kMiddleBtnNorth),
        CURSOR_CODE(CursorType::kNorthEastPanning,
                    OhosCursorType::kMiddleBtnNorthEast),
        CURSOR_CODE(CursorType::kNorthWestPanning,
                    OhosCursorType::kMiddleBtnNorthWest),
        CURSOR_CODE(CursorType::kSouthPanning, OhosCursorType::kMiddleBtnSouth),
        CURSOR_CODE(CursorType::kSouthEastPanning,
                    OhosCursorType::kMiddleBtnSouthEast),
        CURSOR_CODE(CursorType::kSouthWestPanning,
                    OhosCursorType::kMiddleBtnSouthWest),
        CURSOR_CODE(CursorType::kWestPanning, OhosCursorType::kMiddleBtnWest),
        CURSOR_CODE(CursorType::kMove, OhosCursorType::kMove),
        CURSOR_CODE(CursorType::kVerticalText,
                    OhosCursorType::kHorizontalTextCursor),
        CURSOR_CODE(CursorType::kCell, OhosCursorType::kCursorCross),
        CURSOR_CODE(CursorType::kContextMenu, OhosCursorType::kDefault),
        CURSOR_CODE(CursorType::kAlias, OhosCursorType::kDefault),
        CURSOR_CODE(CursorType::kProgress, OhosCursorType::kRunning),
        CURSOR_CODE(CursorType::kNoDrop, OhosCursorType::kCursorForbid),
        CURSOR_CODE(CursorType::kCopy, OhosCursorType::kCursorCopy),
        CURSOR_CODE(CursorType::kNone, OhosCursorType::kNone),
        CURSOR_CODE(CursorType::kNotAllowed, OhosCursorType::kCursorForbid),
        CURSOR_CODE(CursorType::kZoomIn, OhosCursorType::kZoomIn),
        CURSOR_CODE(CursorType::kZoomOut, OhosCursorType::kZoomOut),
        CURSOR_CODE(CursorType::kGrab, OhosCursorType::kHandOpen),
        CURSOR_CODE(CursorType::kGrabbing, OhosCursorType::kHandGrabbing),
        CURSOR_CODE(CursorType::kMiddlePanningVertical,
                    OhosCursorType::kMiddleBtnNorthSouth),
        CURSOR_CODE(CursorType::kMiddlePanningHorizontal,
                    OhosCursorType::kDefault),
        CURSOR_CODE(CursorType::kCustom, OhosCursorType::kDefault),
        CURSOR_CODE(CursorType::kDndNone, OhosCursorType::kDefault),
        CURSOR_CODE(CursorType::kDndMove, OhosCursorType::kDefault),
        CURSOR_CODE(CursorType::kDndCopy, OhosCursorType::kDefault),
        CURSOR_CODE(CursorType::kDndLink, OhosCursorType::kDefault),
        CURSOR_CODE(CursorType::kEastWestNoResize, OhosCursorType::kDefault),
        CURSOR_CODE(CursorType::kNorthSouthNoResize, OhosCursorType::kDefault),
        CURSOR_CODE(CursorType::kNorthEastSouthWestNoResize,
                    OhosCursorType::kDefault),
        CURSOR_CODE(CursorType::kNorthWestSouthEastNoResize,
                    OhosCursorType::kDefault),
};
#undef CURSOR_CODE

ohos::adapter::OhosCursorType ConvertToOhosCursorType(
    const mojom::CursorType type) {
  OhosCursorType oh_cursor_type = OhosCursorType::kDefault;
  if (type >= CursorType::kPointer && type <= CursorType::kMaxValue) {
    oh_cursor_type = kLookupTable[static_cast<int32_t>(type)];
  }
  return oh_cursor_type;
}
}  // namespace ui
