// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/core/layout/layout_theme_ohos.h"

#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "third_party/blink/renderer/platform/wtf/std_lib_extras.h"
#include "ui/base/ui_base_features.h"

namespace blink {

scoped_refptr<LayoutTheme> LayoutThemeOhos::Create() {
  return base::AdoptRef(new LayoutThemeOhos());
}

LayoutTheme& LayoutTheme::NativeTheme() {
  DEFINE_STATIC_REF(LayoutTheme, layout_theme, (LayoutThemeOhos::Create()));
  return *layout_theme;
}

LayoutThemeOhos::~LayoutThemeOhos() {}

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

}  // namespace blink
