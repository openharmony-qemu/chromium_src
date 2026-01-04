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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/hilight/arkweb_highlight_style_utils.h"

#include "components/shared_highlighting/core/common/fragment_directives_constants.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/frame/settings.h"
#include "third_party/blink/renderer/core/page/page.h"
#include "third_party/blink/renderer/core/page/page_utils.h"

namespace blink {
#if BUILDFLAG(ARKWEB_DRAG_DROP)
bool ArkWebHighlightStyleUtils::InSelectionDragging(const Document& document) {
  // Select range and dragging it
  return document.GetPage() && document.GetPage()->page_utils()->IsInTextDraging() &&
         !document.Printing();
}
#endif  // BUILDFLAG(ARKWEB_DRAG_DROP)

#if BUILDFLAG(ARKWEB_AI)
bool ArkWebHighlightStyleUtils::ShouldUseAIColors(PseudoId pseudo,
                                                  const CSSProperty& property,
                                                  const Document& document) {
  if (pseudo == PseudoId::kPseudoIdTargetText) {
    switch (property.PropertyID()) {
      case CSSPropertyID::kColor:
      case CSSPropertyID::kBackgroundColor: {
        if (document.GetSettings() &&
            document.GetSettings()->GetArkwebAgentEnabled()) {
          return true;
        }
        break;
      }
      default:
        break;
    }
  }

  return false;
}

Color ArkWebHighlightStyleUtils::GetTargetTextForegroundColor(
    const Document& document,
    mojom::blink::ColorScheme color_scheme) {
  if (document.GetSettings() &&
      document.GetSettings()->GetArkwebAgentEnabled()) {
    return color_scheme == mojom::blink::ColorScheme::kDark
               ? Color::kWhite
               : Color::kBlack;
  }
  return LayoutTheme::GetTheme().PlatformTextSearchColor(
      false /* active match */, document.InForcedColorsMode(), color_scheme,
      document.GetColorProviderForPainting(color_scheme),
      document.IsInWebAppScope());
}

Color ArkWebHighlightStyleUtils::GetTargetTextBackgroundColor(
    const Document& document,
    mojom::blink::ColorScheme color_scheme) {
  if (document.GetSettings() &&
      document.GetSettings()->GetArkwebAgentEnabled()) {
    return color_scheme == mojom::blink::ColorScheme::kDark
               ? Color::FromRGBA32(kTargetTextBackgroundColorDark)
               : Color::FromRGBA32(kTargetTextBackgroundColor);
  }
  return Color::FromRGBA32(
      shared_highlighting::kFragmentTextBackgroundColorARGB);
}
#endif
}  // namespace blink
