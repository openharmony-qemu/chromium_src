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

#include "nweb_touch_handle_state_impl.h"

namespace OHOS::NWeb {
NWebTouchHandleStateImpl::NWebTouchHandleStateImpl(const CefTouchHandleState& state)
  : state_(state) {}

int32_t NWebTouchHandleStateImpl::GetTouchHandleId() const {
  return state_.touch_handle_id;
}

int32_t NWebTouchHandleStateImpl::GetX() const {
  return state_.origin.x;
}

int32_t NWebTouchHandleStateImpl::GetY() const {
  return state_.origin.y;
}

NWebTouchHandleState::TouchHandleType
NWebTouchHandleStateImpl::GetTouchHandleType() const {
  switch (state_.orientation) {
    case CEF_HORIZONTAL_ALIGNMENT_CENTER:
      return INSERT_HANDLE;
    case CEF_HORIZONTAL_ALIGNMENT_LEFT:
      return SELECTION_BEGIN_HANDLE;
    case CEF_HORIZONTAL_ALIGNMENT_RIGHT:
      return SELECTION_END_HANDLE;
    default:
      return INVALID_HANDLE;
  }
}

bool NWebTouchHandleStateImpl::IsEnable() const {
  return state_.enabled;
}

float NWebTouchHandleStateImpl::GetAlpha() const {
  return state_.alpha;
}

float NWebTouchHandleStateImpl::GetEdgeHeight() const {
  return state_.edge_height;
}
}