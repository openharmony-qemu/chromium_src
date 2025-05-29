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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/css/resolver/font_builder_utils.h"

#include "base/logging.h"
#include "third_party/blink/renderer/core/css/resolver/font_builder.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/frame/settings.h"
#include "third_party/blink/renderer/platform/fonts/font_description.h"

#if BUILDFLAG(ARKWEB_CSS_FONT)
constexpr float epsilon = 0.001f;
#endif

namespace blink {
FontBuilderUtils::FontBuilderUtils(FontBuilder* font_builder)
    : font_builder_(font_builder) {}

#if BUILDFLAG(ARKWEB_CSS_FONT)
void FontBuilderUtils::UpdateWeightScale(FontDescription& font_description) {
  float weightScale = 1.0f;
  if (font_builder_->document_) {
    Settings* settings = font_builder_->document_->GetSettings();
    if (settings) {
      weightScale = settings->GetFontWeightScale();
    }
  }
  if (weightScale < epsilon) {
    LOG(ERROR) << "FollowSystemFontWeight error: weightScale <= 0";
    return;
  }
  float newWeight = std::clamp((float)font_description.Weight() * weightScale,
                               (float)kMinWeightValue, (float)kMaxWeightValue);
  font_description.SetWeight(FontSelectionValue(newWeight));
}
#endif

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
void FontBuilderUtils::UpdateFixedFontSize(FontDescription& font_description) {
  DCHECK(font_builder_->document_);
  Settings* settings = font_builder_->document_->GetSettings();
  if (settings) {
    font_description.SetFixedFontSize(settings->GetDefaultFixedFontSize());
  }
}
#endif

void FontBuilderUtils::UpdateFontDescription(FontDescription& description,
                                             bool& modified) {
  FontSelectionValue oldWeight = description.Weight();
  description.SetWeight(font_builder_->font_description_.Weight());
  UpdateWeightScale(description);
  if (description.Weight() != oldWeight) {
    modified = true;
  }
}
}  // namespace blink