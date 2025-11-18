/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NWEB_TOUCH_HANDLE_STATE_IMPL_H
#define NWEB_TOUCH_HANDLE_STATE_IMPL_H

#include "cef/include/internal/cef_types_wrappers.h"
#include "nweb_touch_handle_state.h"

namespace OHOS::NWeb {
class NWebTouchHandleStateImpl : public NWebTouchHandleState {
 public:
  explicit NWebTouchHandleStateImpl(const CefTouchHandleState& state);
  int32_t GetTouchHandleId() override;
  int32_t GetX() override;
  int32_t GetY() override;
  int32_t GetViewPortX() override;
  int32_t GetViewPortY() override;
  TouchHandleType GetTouchHandleType() override;
  bool IsEnable() override;
  float GetAlpha() override;
  float GetEdgeHeight() override;
  bool IsDragging() override;

 private:
  CefTouchHandleState state_;
};
}  // namespace OHOS::NWeb
#endif
