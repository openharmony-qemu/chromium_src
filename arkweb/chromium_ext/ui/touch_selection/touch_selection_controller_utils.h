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

#ifndef UI_TOUCH_SELECTION_TOUCH_SELECTION_CONTROLLER_UTILS_H_
#define UI_TOUCH_SELECTION_TOUCH_SELECTION_CONTROLLER_UTILS_H_

#include "arkweb/build/features/features.h"

namespace ui {
class TouchSelectionController;

class TouchSelectionControllerUtils {
 public:
  TouchSelectionControllerUtils(TouchSelectionController* controller);

  ~TouchSelectionControllerUtils();

#if BUILDFLAG(ARKWEB_MENU)
  void SetResetSelectionTemporarily(bool reset_selection_temporarily) {
    reset_selection_temporarily_ = reset_selection_temporarily;
  }
  bool GetResetSelectionTemporarily() { return reset_selection_temporarily_; }
#endif  // BUILDFLAG(ARKWEB_MENU)

  TouchSelectionController* controller_;
#if BUILDFLAG(ARKWEB_MENU)
  bool reset_selection_temporarily_ = false;
#endif  // BUILDFLAG(ARKWEB_MENU)
};
}  // namespace ui
#endif  // UI_TOUCH_SELECTION_TOUCH_SELECTION_CONTROLLER_UTILS_H_
