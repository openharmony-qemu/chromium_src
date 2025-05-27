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

#ifndef UI_NATIVE_THEME_NATIVE_THEME_AURA_UTILS_H_
#define UI_NATIVE_THEME_NATIVE_THEME_AURA_UTILS_H_

#include "arkweb/build/features/features.h"
#include "ui/native_theme/native_theme.h"
#include "ui/native_theme/native_theme_aura.h"
#include "ui/native_theme/overlay_scrollbar_constants_aura.h"

namespace ui {
class NativeTheme;
class NativeThemeAura;

class NativeThemeAuraUtils {
  friend class NativeThemeAura;

 public:
  NativeThemeAuraUtils(NativeThemeAura* native_theme_aura);

  void SetScrollbarThumbWidth();
  void PaintOverlayScrollbarThumb(cc::PaintCanvas* canvas,
                                  const gfx::Rect& rect,
                                  SkColor scrollbar_color,
                                  NativeTheme::Part part,
                                  SkColor& thumb_color,
                                  cc::PaintFlags& fill_flags);
  void PaintScrollbarThumbWithColor(
      cc::PaintCanvas* canvas,
      const gfx::Rect& rect,
      SkColor scrollbar_color,
      NativeTheme::ColorScheme& color_scheme,
      NativeTheme::Part part,
      NativeTheme::State state,
      const NativeTheme::ScrollbarThumbExtraParams& extra_params);
  gfx::Size GetPartSize(NativeTheme::Part part,
                        NativeTheme::State state,
                        const NativeTheme::ExtraParams& extra) const;
  gfx::Size GetNinePatchCanvasSize(NativeTheme::Part part) const;
  gfx::Rect GetNinePatchAperture(NativeTheme::Part part) const;

  NativeThemeAura* native_theme_aura_;
};
}  // namespace ui
#endif  // UI_NATIVE_THEME_NATIVE_THEME_AURA_UTILS_H_
