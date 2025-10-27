/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef UI_OHOS_OVERSCROLL_REFRESH_HANDLER_H_
#define UI_OHOS_OVERSCROLL_REFRESH_HANDLER_H_

#include "ui/ohos/overscroll_refresh.h"
#include "ui_ohos_export.h"

namespace ui {

enum PullToRefreshAction {
  UNKNOWN = 0,
  PULL_START,
  PULL_REFRESH,
  PULL_CANCEL,
  PULL_RELEASE,
  PULL_RESET
};

class UI_OHOS_EXPORT OverscrollRefreshHandler {
 public:
  virtual ~OverscrollRefreshHandler() {}

  virtual bool PullToRefreshAction(PullToRefreshAction action) = 0;
  // Signals a pull update, where |x_delta| and |y_delta| are in device pixels.
  virtual void PullToRefreshUpdate(float x_delta, float y_delta) = 0;
};

}  // namespace ui

#endif  // UI_OHOS_OVERSCROLL_REFRESH_HANDLER_H_
