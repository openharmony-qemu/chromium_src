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

#include "nweb_select_menu_bound_impl.h"

namespace OHOS::NWeb {

NWebSelectMenuBoundImpl::NWebSelectMenuBoundImpl(int x, int y, int width,
                                                 int height)
    : x_(x), y_(y), width_(width), height_(height) {
}

int NWebSelectMenuBoundImpl::GetX() {
  return x_;
}

int NWebSelectMenuBoundImpl::GetY() {
  return y_;
}

int NWebSelectMenuBoundImpl::GetWidth() {
  return width_;
}

int NWebSelectMenuBoundImpl::GetHeight() {
  return height_;
}

} // namespace OHOS::NWeb
