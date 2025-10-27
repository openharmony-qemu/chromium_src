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

#include "nweb_imf_cursor_info_adapter_impl.h"

namespace OHOS::NWeb {

double NWebIMFCursorInfoAdapterImpl::GetLeft() {
  return left_;
}

double NWebIMFCursorInfoAdapterImpl::GetTop() {
  return top_;
}

double NWebIMFCursorInfoAdapterImpl::GetWidth() {
  return width_;
}

double NWebIMFCursorInfoAdapterImpl::GetHeight() {
  return height_;
}

void NWebIMFCursorInfoAdapterImpl::SetLeft(double left) {
  left_ = left;
}

void NWebIMFCursorInfoAdapterImpl::SetTop(double top) {
  top_ = top;
}

void NWebIMFCursorInfoAdapterImpl::SetWidth(double width) {
  width_ = width;
}

void NWebIMFCursorInfoAdapterImpl::SetHeight(double height) {
  height_ = height;
}

}  // namespace OHOS::NWeb
