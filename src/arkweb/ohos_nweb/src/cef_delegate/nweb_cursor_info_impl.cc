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

#include "nweb_cursor_info_impl.h"

namespace OHOS::NWeb {

NWebCursorInfoImpl::NWebCursorInfoImpl(int32_t x,
                                       int32_t y,
                                       float scale,
                                       int32_t width,
                                       int32_t height,
                                       uint8_t* buff)
    : x_(x),
      y_(y),
      scale_(scale),
      width_(width),
      height_(height),
      buff_(buff) {}

int32_t NWebCursorInfoImpl::GetX() {
  return x_;
}

int32_t NWebCursorInfoImpl::GetY() {
  return y_;
}

float NWebCursorInfoImpl::GetScale() {
  return scale_;
}

uint8_t* NWebCursorInfoImpl::GetBuff() {
  return buff_;
}

int32_t NWebCursorInfoImpl::GetWidth() {
  return width_;
}

int32_t NWebCursorInfoImpl::GetHeight() {
  return height_;
}

}  // namespace OHOS::NWeb
