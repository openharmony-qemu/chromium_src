/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "arkweb/chromium_ext/ui/touch_selection/longpress_drag_selector_util.h"
#include "arkweb/build/features/features.h"
#include "ui/touch_selection/touch_selection_controller.h"

namespace ui {

LongPressDragSelectorUtils::LongPressDragSelectorUtils(
    LongPressDragSelector* controller)
    : controller_(controller) {}

LongPressDragSelectorUtils::~LongPressDragSelectorUtils() {}

bool LongPressDragSelectorUtils::PositionInSelection(gfx::PointF position,
                                                     gfx::PointF start,
                                                     gfx::PointF end) {
  if (!controller_) {
    return false;
  }
  if (position.y() < end.y() && position.y() > start.y() &&
      position.x() > start.x() && position.x() < end.x()) {
    return true;
  }
  return false;
}
}  // namespace ui
