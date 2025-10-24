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
#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>
#include <memory>

#include "base/containers/flat_map.h"
#define private public
#define protected public
#include "arkweb/ohos_adapter_ndk/mock_ndk_api/include/mock_sys_info_util_ext.h"
#include "arkweb/chromium_ext/ui/native_theme/native_theme_aura_utils.h"
#include "cc/paint/record_paint_canvas.h"
#include "ui/native_theme/native_theme.h"
#include "ui/native_theme/native_theme_aura.h"

using namespace testing;

namespace ui {
using testing::Return;
class NativeThemeAuraUtilsTest : public testing::Test {
 public:
  void SetUp() override;
  void TearDown() override;

  std::shared_ptr<NativeThemeAuraUtils> native_theme_aura_utils_;
  std::shared_ptr<NativeThemeAura> native_theme_aura_;
};

void NativeThemeAuraUtilsTest::SetUp() {
  native_theme_aura_ = std::make_shared<NativeThemeAura>(
      /*use_overlay_scrollbars=*/false,
      /*should_only_use_dark_colors=*/false,
      /*system_theme=*/ui::SystemTheme::kDefault,
      /*configure_web_instance=*/true);
  ASSERT_NE(native_theme_aura_, nullptr);
  native_theme_aura_utils_ =
      std::make_shared<NativeThemeAuraUtils>(native_theme_aura_.get());
  ASSERT_NE(native_theme_aura_utils_, nullptr);
}

void NativeThemeAuraUtilsTest::TearDown() {
  native_theme_aura_ = nullptr;
  native_theme_aura_utils_ = nullptr;
}

TEST_F(NativeThemeAuraUtilsTest, PaintOverlayScrollbarThumb_001) {
  cc::RecordPaintCanvas canvas;
  const gfx::Rect rect;
  SkColor scrollbar_color = SK_ColorLTGRAY;
  NativeTheme::Part part = NativeTheme::kScrollbarHorizontalThumb;
  SkColor thumb_color;
  cc::PaintFlags fill_flags;

  native_theme_aura_utils_->PaintOverlayScrollbarThumb(
      &canvas, rect, scrollbar_color, part, thumb_color, fill_flags);

  part = NativeTheme::kScrollbarVerticalThumb;
  native_theme_aura_utils_->PaintOverlayScrollbarThumb(
      &canvas, rect, scrollbar_color, part, thumb_color, fill_flags);
}

TEST_F(NativeThemeAuraUtilsTest, PaintScrollbarThumbWithColor_001) {
  cc::RecordPaintCanvas canvas;
  const gfx::Rect rect;
  SkColor scrollbar_color = SK_ColorLTGRAY;
  NativeTheme::Part part = NativeTheme::kScrollbarVerticalThumb;
  NativeTheme::State state = NativeTheme::kNormal;
  NativeTheme::ScrollbarThumbExtraParams extra_params;
  NativeTheme::ColorScheme color_scheme = NativeTheme::ColorScheme::kDefault;
  native_theme_aura_utils_->PaintScrollbarThumbWithColor(
      &canvas, rect, scrollbar_color, color_scheme, part, state, extra_params);

  color_scheme = NativeTheme::ColorScheme::kDark;
  extra_params.thumb_color = SK_ColorLTGRAY;
  part = NativeTheme::kScrollbarHorizontalThumb;
  native_theme_aura_utils_->PaintScrollbarThumbWithColor(
      &canvas, rect, scrollbar_color, color_scheme, part, state, extra_params);

  state = NativeTheme::kHovered;
  part = NativeTheme::kScrollbarVerticalThumb;
  native_theme_aura_utils_->PaintScrollbarThumbWithColor(
      &canvas, rect, scrollbar_color, color_scheme, part, state, extra_params);

  part = NativeTheme::kScrollbarHorizontalThumb;
  native_theme_aura_utils_->PaintScrollbarThumbWithColor(
      &canvas, rect, scrollbar_color, color_scheme, part, state, extra_params);

  state = NativeTheme::kPressed;
  part = NativeTheme::kScrollbarVerticalThumb;
  native_theme_aura_utils_->PaintScrollbarThumbWithColor(
      &canvas, rect, scrollbar_color, color_scheme, part, state, extra_params);

  part = NativeTheme::kScrollbarHorizontalThumb;
  native_theme_aura_utils_->PaintScrollbarThumbWithColor(
      &canvas, rect, scrollbar_color, color_scheme, part, state, extra_params);
}

TEST_F(NativeThemeAuraUtilsTest, GetPartSize_001) {
  NativeTheme::Part part = NativeTheme::kScrollbarHorizontalThumb;
  NativeTheme::State state = NativeTheme::kNormal;
  NativeTheme::ScrollbarThumbExtraParams extra_params;
  native_theme_aura_utils_->native_theme_aura_->set_use_overlay_scrollbar(true);
  native_theme_aura_utils_->GetPartSize(part, state, extra_params);

  part = NativeTheme::kScrollbarVerticalThumb;
  native_theme_aura_utils_->GetPartSize(part, state, extra_params);

  part = NativeTheme::kScrollbarHorizontalTrack;
  native_theme_aura_utils_->GetPartSize(part, state, extra_params);

  native_theme_aura_utils_->native_theme_aura_->set_use_overlay_scrollbar(
      false);
  part = NativeTheme::kScrollbarDownArrow;
  native_theme_aura_utils_->GetPartSize(part, state, extra_params);

  part = NativeTheme::kScrollbarUpArrow;
  native_theme_aura_utils_->GetPartSize(part, state, extra_params);

  part = NativeTheme::kScrollbarLeftArrow;
  native_theme_aura_utils_->GetPartSize(part, state, extra_params);

  part = NativeTheme::kScrollbarRightArrow;
  native_theme_aura_utils_->GetPartSize(part, state, extra_params);

  part = NativeTheme::kScrollbarHorizontalThumb;
  native_theme_aura_utils_->GetPartSize(part, state, extra_params);
}

TEST_F(NativeThemeAuraUtilsTest, GetNinePatchAperture_001) {
  NativeTheme::Part part = NativeTheme::kScrollbarHorizontalThumb;
  native_theme_aura_utils_->GetNinePatchAperture(part);

  part = NativeTheme::kScrollbarVerticalThumb;
  native_theme_aura_utils_->GetNinePatchAperture(part);
}

TEST_F(NativeThemeAuraUtilsTest, PCDeviceTest_001) {
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = true;
  auto& system_properties_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(system_properties_mock, IsPcDevice())
      .WillOnce(Return(true))
      .WillRepeatedly(Return(true));

  native_theme_aura_utils_->SetScrollbarThumbWidth();
  EXPECT_EQ(native_theme_aura_->scrollbar_width_,
            kOverlayScrollbarThumbWidthPressedPc);

  cc::RecordPaintCanvas canvas;
  const gfx::Rect rect;
  SkColor scrollbar_color = SK_ColorLTGRAY;
  NativeTheme::Part part = NativeTheme::kScrollbarHorizontalThumb;
  SkColor thumb_color;
  cc::PaintFlags fill_flags;

  native_theme_aura_utils_->PaintOverlayScrollbarThumb(
      &canvas, rect, scrollbar_color, part, thumb_color, fill_flags);

  part = NativeTheme::kScrollbarVerticalThumb;
  native_theme_aura_utils_->PaintOverlayScrollbarThumb(
      &canvas, rect, scrollbar_color, part, thumb_color, fill_flags);
  base::ohos::SysInfoUtilsMock::mockIsPcDevice = false;      
}
}  // namespace ui
