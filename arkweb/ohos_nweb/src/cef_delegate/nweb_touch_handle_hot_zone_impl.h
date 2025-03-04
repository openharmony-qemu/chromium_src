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

#ifndef NWEB_TOUCH_HANDLE_HOT_ZONE_IMPL_H
#define NWEB_TOUCH_HANDLE_HOT_ZONE_IMPL_H

#include "nweb_handler.h"

namespace OHOS::NWeb {

class NWebTouchHandleHotZoneImpl : public NWebTouchHandleHotZone {
 public:
  NWebTouchHandleHotZoneImpl() = default;
  ~NWebTouchHandleHotZoneImpl() = default;

  double GetWidth();

  void SetWidth(double width) override;

  double GetHeight();

  void SetHeight(double height) override;

 private:
  double width_ = 0.0;
  double height_ = 0.0;
};

}  // namespace OHOS::NWeb

#endif  // NWEB_TOUCH_HANDLE_HOT_ZONE_IMPL_H
