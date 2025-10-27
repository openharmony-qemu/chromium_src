/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "nweb_imf_text_config_adapter_impl.h"

namespace OHOS::NWeb {

std::shared_ptr<IMFInputAttributeAdapter>
NWebIMFTextConfigAdapterImpl::GetInputAttribute() {
  return input_attribute_;
}

std::shared_ptr<IMFCursorInfoAdapter>
NWebIMFTextConfigAdapterImpl::GetCursorInfo() {
  return cursor_info_;
}

std::shared_ptr<IMFSelectionRangeAdapter>
NWebIMFTextConfigAdapterImpl::GetSelectionRange() {
  return range_;
}

uint32_t NWebIMFTextConfigAdapterImpl::GetWindowId() {
  return window_id_;
}

double NWebIMFTextConfigAdapterImpl::GetPositionY() {
  return position_y_;
}

double NWebIMFTextConfigAdapterImpl::GetHeight() {
  return height_;
}

void NWebIMFTextConfigAdapterImpl::SetInputAttribute(
    std::shared_ptr<IMFInputAttributeAdapter> attribute) {
  input_attribute_ = attribute;
}

void NWebIMFTextConfigAdapterImpl::SetCursorInfo(
    std::shared_ptr<IMFCursorInfoAdapter> info) {
  cursor_info_ = info;
}

void NWebIMFTextConfigAdapterImpl::SetSelectionRange(
    std::shared_ptr<IMFSelectionRangeAdapter> range) {
  range_ = range;
}

void NWebIMFTextConfigAdapterImpl::SetWindowId(uint32_t id) {
  window_id_ = id;
}

void NWebIMFTextConfigAdapterImpl::SetPositionY(double positionY) {
  position_y_ = positionY;
}

void NWebIMFTextConfigAdapterImpl::SetHeight(double height) {
  height_ = height;
}

}  // namespace OHOS::NWeb
