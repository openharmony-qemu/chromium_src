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

#include <string>

#include "gtest/gtest.h"
#define private public
#include "color_picker_adapter_impl.h"
#undef private
#include "arkweb/ohos_nweb/src/nweb_hilog.h"

using namespace testing;

using namespace OHOS::NWeb;

class ColorPickerAdapterImplTest : public ::testing::Test {
 protected:
  void SetUp() override {}
  void TearDown() override {}
};

/**
 * @tc.name: StartColorPicker_WhenLibNullptr
 * @tc.number: ColorPickerAdapterImplTest_001
 * @tc.desc: Test StartColorPicker method of ColorPickerAdapterImpl class
 */
TEST_F(ColorPickerAdapterImplTest, StartColorPicker_WhenLibNullptr) {
  ColorPickerAdapterImpl impl;
  impl.library_handle_ = nullptr;
  ColorPickerCallback color_picker_callback = [](bool success, uint32_t color) {
    WVLOG_I("OnEyeDropperResult, success == %{public}d, color == %{public}d",
            success, color);
    EXPECT_EQ(success, false);
    EXPECT_EQ(color, 0);
  };
  impl.StartColorPicker(0, 0, color_picker_callback);
  EXPECT_EQ(impl.callback_index_, 0);
}

/**
 * @tc.name: StartColorPicker_WhenNormal
 * @tc.number: ColorPickerAdapterImplTest_002
 * @tc.desc: Test StartColorPicker method of ColorPickerAdapterImpl class
 */
TEST_F(ColorPickerAdapterImplTest, StartColorPicker_WhenNormal) {
  ColorPickerAdapterImpl impl;
  ColorPickerCallback color_picker_callback = [](bool success, uint32_t color) {
    WVLOG_I("OnEyeDropperResult, success == %{public}d, color == %{public}d",
            success, color);
    EXPECT_EQ(success, true);
  };
  impl.StartColorPicker(0, 0, color_picker_callback);
  EXPECT_EQ(impl.callback_index_, 1);
}

/**
 * @tc.name: StartColorPickerWithColorValue_WhenNormal
 * @tc.number: ColorPickerAdapterImplTest_003
 * @tc.desc: Test StartColorPickerWithColorValue method of
 * ColorPickerAdapterImpl class
 */
TEST_F(ColorPickerAdapterImplTest, StartColorPickerWithColorValue_WhenNormal) {
  ColorPickerAdapterImpl impl;
  ColorPickerCallback color_picker_callback = [](bool success, uint32_t color) {
    WVLOG_I("OnEyeDropperResult, success == %{public}d, color == %{public}d",
            success, color);
    EXPECT_EQ(success, true);
  };
  impl.StartColorPickerWithColorValue(0, 0, color_picker_callback);
  EXPECT_EQ(impl.callback_wrapper_.Size(), 0);
}