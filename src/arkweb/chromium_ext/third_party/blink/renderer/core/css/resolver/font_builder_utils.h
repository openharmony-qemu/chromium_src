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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CSS_RESOLVER_FONT_BUILDER_UTILS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CSS_RESOLVER_FONT_BUILDER_UTILS_H_

#include "base/memory/raw_ptr.h"
#include "third_party/blink/renderer/platform/fonts/font_description.h"

namespace blink {
class FontBuilder;

class FontBuilderUtils {
 public:
  FontBuilderUtils(FontBuilder* font_builder);

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
  void UpdateFixedFontSize(FontDescription&);
#endif
#if BUILDFLAG(ARKWEB_CSS_FONT)
  void UpdateWeightScale(FontDescription&);
#endif
  void UpdateFontDescription(FontDescription& description, bool& modified);

 private:
  raw_ptr<FontBuilder> font_builder_;
};
}  // namespace blink
#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_CSS_RESOLVER_FONT_BUILDER_UTILS_H_