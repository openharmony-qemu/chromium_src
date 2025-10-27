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

#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "base/command_line.h"
#include "third_party/blink/renderer/core/layout/layout_theme_mobile.h"
#include "third_party/blink/renderer/core/style/computed_style.h"
#define private public
#include "third_party/blink/renderer/core/layout/layout_theme_ohos.h"
#undef private

namespace blink {
namespace {

class MockSysInfoUtils {
 public:
  static bool IsPcDevice() { return is_pc_device_; }
  static void SetIsPcDevice(bool is_pc) { is_pc_device_ = is_pc; }

 private:
  static bool is_pc_device_;
};
bool MockSysInfoUtils::is_pc_device_ = false;

}  // namespace
}  // namespace blink

namespace base {
namespace {
bool IsPcDevice() { return blink::MockSysInfoUtils::IsPcDevice(); }
}  // namespace
}  // namespace base

namespace blink {

class LayoutThemeOhosTest : public testing::Test {
 protected:
  void SetUp() override {
    MockSysInfoUtils::SetIsPcDevice(false);
    initial_style_ = ComputedStyle::GetInitialStyleSingleton();
  }

  ComputedStyleBuilder CreateComputedStyleBuilder() {
    return ComputedStyleBuilder(*initial_style_);
  }

  LayoutThemeOhos* theme() {
    static LayoutThemeOhos* instance = []() {
      return static_cast<LayoutThemeOhos*>(LayoutThemeOhos::Create().get());
    }();
    return instance;
  }
  Persistent<const ComputedStyle> initial_style_;
};

TEST_F(LayoutThemeOhosTest, PlatformActiveSelectionBackgroundColor) {
  Color dark_color = theme()->PlatformActiveSelectionBackgroundColor(
      mojom::blink::ColorScheme::kDark);
  EXPECT_EQ(dark_color, LayoutThemeOhos::kDefaultDrakSchemeActiveSelectionBackgroundColor);

  Color light_color = theme()->PlatformActiveSelectionBackgroundColor(
      mojom::blink::ColorScheme::kLight);
  EXPECT_EQ(light_color, LayoutThemeOhos::kDefaultActiveSelectionBackgroundColor);
}

TEST_F(LayoutThemeOhosTest, PlatformActiveSelectionForegroundColor) {
  Color dark_color = theme()->PlatformActiveSelectionForegroundColor(
      mojom::blink::ColorScheme::kDark);
  EXPECT_EQ(dark_color, Color::FromRGBA32(0xFF3B3B3B));

  Color light_color = theme()->PlatformActiveSelectionForegroundColor(
      mojom::blink::ColorScheme::kLight);
  EXPECT_NE(light_color, dark_color);
}

TEST_F(LayoutThemeOhosTest, AdjustInnerSpinButtonStyle) {
  ComputedStyleBuilder builder = CreateComputedStyleBuilder();

  MockSysInfoUtils::SetIsPcDevice(false);
  theme()->AdjustInnerSpinButtonStyle(builder);

  MockSysInfoUtils::SetIsPcDevice(true);
  theme()->AdjustInnerSpinButtonStyle(builder);
}

TEST_F(LayoutThemeOhosTest, PlatformTapHighlightColor) {
  MockSysInfoUtils::SetIsPcDevice(false);
  Color mobile_color = theme()->PlatformTapHighlightColor();
  EXPECT_NE(mobile_color, Color());

  MockSysInfoUtils::SetIsPcDevice(true);
  Color pc_color = theme()->PlatformTapHighlightColor();
  EXPECT_NE(pc_color, Color());
}

TEST_F(LayoutThemeOhosTest, ExtraDefaultStyleSheet) {
  const base::CommandLine::CharType* argv[] = {switches::kDisableMobileStyleSheet};
  
  base::CommandLine::Reset();
  theme()->ExtraDefaultStyleSheet();
  EXPECT_EQ(base::CommandLine::ForCurrentProcess(), nullptr);

  base::CommandLine::Init(std::size(argv), argv);
  theme()->ExtraDefaultStyleSheet();
  EXPECT_NE(base::CommandLine::ForCurrentProcess(), nullptr);
}

}  // namespace blink