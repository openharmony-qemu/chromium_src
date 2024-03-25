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

#ifndef NWEB_SELECT_MENU_BOUND_H
#define NWEB_SELECT_MENU_BOUND_H

#include "nweb_select_popup_menu.h"

namespace OHOS::NWeb {

class NWebSelectMenuBoundImpl : public NWebSelectMenuBound {
public:
  NWebSelectMenuBoundImpl(int x, int y, int width, int height);
  ~NWebSelectMenuBoundImpl() = default;

  int GetX() override;
  int GetY() override;
  int GetWidth() override;
  int GetHeight() override;

private:
  int x_ = -1;
  int y_ = -1;
  int width_ = -1;
  int height_ = -1;
};

} // namespace OHOS::NWeb

#endif // NWEB_SELECT_MENU_BOUND_H
