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

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_FONTS_FONT_PLATFORM_DATA_UTILS_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_FONTS_FONT_PLATFORM_DATA_UTILS_H_


#include "arkweb/build/features/features.h"
#include "third_party/blink/renderer/platform/fonts/font_platform_data.h"

namespace blink {

class FontPlatformData;

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
#define FONT_SCALE 0.79

struct ScaleParam {
  SkScalar fontScale;
};
const ScaleParam DEFAULT_SCALE_PARAM = ScaleParam{.fontScale = 0};
#endif

class FontPlatformDataUtils final
  : public GarbageCollected<FontPlatformDataUtils> {
public:

FontPlatformDataUtils(FontPlatformData* data);
~FontPlatformDataUtils();
void Trace(Visitor*) const;

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
  const ScaleParam& FindCompressionConfigWithFont(
      const std::string& familyName) const;
#endif

private:
Member<FontPlatformData> font_data_;
#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
const std::unordered_map<std::string, ScaleParam>
    FONT_FAMILY_COMPRESSION_CONFIG = {
        {"Noto Serif Tibetan", ScaleParam{.fontScale = FONT_SCALE}},
        {"Noto Sans Tibetan", ScaleParam{.fontScale = FONT_SCALE}},
    };
#endif
};

}
#endif  //THIRD_PARTY_BLINK_RENDERER_PLATFORM_FONTS_FONT_PLATFORM_DATA_UTILS_H_