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

#include "font_platform_data_utils.h"

namespace blink {

FontPlatformDataUtils::FontPlatformDataUtils(FontPlatformData* data) : font_data_(data) {}
FontPlatformDataUtils::~FontPlatformDataUtils() {}

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
const ScaleParam& FontPlatformDataUtils::FindCompressionConfigWithFont(
    const std::string& familyName) const {
  auto iter = FONT_FAMILY_COMPRESSION_CONFIG.find(familyName);
  if (iter == FONT_FAMILY_COMPRESSION_CONFIG.end()) {
    return DEFAULT_SCALE_PARAM;
  }
  return iter->second;
}
#endif
}