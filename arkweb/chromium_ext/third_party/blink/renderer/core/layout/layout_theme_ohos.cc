// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/core/layout/layout_theme_ohos.h"

#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "base/command_line.h"
#include "third_party/blink/renderer/platform/wtf/std_lib_extras.h"
#include "ui/base/ui_base_features.h"

namespace blink {
// LCOV_EXCL_START
scoped_refptr<LayoutTheme> LayoutThemeOhos::Create() {
  return base::AdoptRef(new LayoutThemeOhos());
}

LayoutTheme& LayoutTheme::NativeTheme() {
  DEFINE_STATIC_REF(LayoutTheme, layout_theme, (LayoutThemeOhos::Create()));
  return *layout_theme;
}

LayoutThemeOhos::~LayoutThemeOhos() {}

String LayoutThemeOhos::ExtraDefaultStyleSheet() {
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  bool cmd_value = false;
  if (command_line) {
    cmd_value = command_line->HasSwitch(switches::kDisableMobileStyleSheet);
  }
  if (cmd_value) {
    return LayoutThemeDefault::ExtraDefaultStyleSheet();
  }
  return LayoutThemeMobile::ExtraDefaultStyleSheet();
}

Color LayoutThemeOhos::PlatformActiveSelectionBackgroundColor(
    mojom::blink::ColorScheme color_scheme) const {
  return color_scheme == mojom::blink::ColorScheme::kDark
             ? LayoutThemeOhos::kDefaultDrakSchemeActiveSelectionBackgroundColor
             : LayoutThemeOhos::kDefaultActiveSelectionBackgroundColor;
}

Color LayoutThemeOhos::PlatformActiveSelectionForegroundColor(
    mojom::blink::ColorScheme color_scheme) const {
  return color_scheme == mojom::blink::ColorScheme::kDark
             ? Color::FromRGBA32(0xFF3B3B3B)
             : LayoutThemeMobile::PlatformActiveSelectionForegroundColor(
                   color_scheme);
}

void LayoutThemeOhos::AdjustInnerSpinButtonStyle(
    ComputedStyleBuilder& builder) const {
  if (!base::ohos::IsPcDevice()) {
    // For other devices, InnerSpinButton should not be displayed.
    // This Matches LayoutThemeMobile::AdjustInnerSpinButtonStyle.
    return;
  }
  LayoutThemeDefault::AdjustInnerSpinButtonStyle(builder);
}

Color LayoutThemeOhos::PlatformTapHighlightColor() const {
  if (base::ohos::IsPcDevice()) {
    return LayoutThemeDefault::PlatformTapHighlightColor();
  }
  return LayoutThemeMobile::PlatformTapHighlightColor();
}
// LCOV_EXCL_STOP

}  // namespace blink
