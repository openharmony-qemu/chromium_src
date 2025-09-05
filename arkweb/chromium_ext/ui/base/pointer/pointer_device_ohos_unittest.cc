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

#include "ui/base/pointer/pointer_device.h"

#include <gtest/gtest.h>

namespace ui {

class PointerDeviceOhosTest {};

TEST(PointerDeviceOhosTest, MaxTouchPoints) {
  EXPECT_EQ(MaxTouchPoints(), 0);
}

TEST(PointerDeviceOhosTest, GetPrimaryPointerType) {
  EXPECT_EQ(GetPrimaryPointerType(POINTER_TYPE_FINE), POINTER_TYPE_FINE);
  EXPECT_EQ(GetPrimaryPointerType(POINTER_TYPE_COARSE), POINTER_TYPE_COARSE);
  EXPECT_EQ(GetPrimaryPointerType(POINTER_TYPE_NONE), POINTER_TYPE_NONE);
}

TEST(PointerDeviceOhosTest, GetPrimaryHoverType) {
  EXPECT_EQ(GetPrimaryHoverType(HOVER_TYPE_HOVER), HOVER_TYPE_HOVER);
  EXPECT_EQ(GetPrimaryHoverType(HOVER_TYPE_NONE), HOVER_TYPE_NONE);
}

}  // namespace ui