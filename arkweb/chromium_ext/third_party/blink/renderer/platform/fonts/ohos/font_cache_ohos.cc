// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "skia/ext/font_utils.h"
#include "third_party/blink/renderer/platform/fonts/font_cache.h"

namespace blink {

static AtomicString& MutableSystemFontFamily() {
  DEFINE_THREAD_SAFE_STATIC_LOCAL(AtomicString, system_font_family, ());
  return system_font_family;
}

// static
const AtomicString& FontCache::SystemFontFamily() {
  return MutableSystemFontFamily();
}

// FIXME: Specific functions will be adapted later.
// Lack of emoji adaptation, Reference Android
const SimpleFontData* FontCache::PlatformFallbackFontForCharacter(
    const FontDescription& font_description,
    UChar32 c,
    const SimpleFontData*,
    FontFallbackPriority fallback_priority) {
  sk_sp<SkFontMgr> fm(skia::DefaultFontMgr());

  const char* generic_family_name = nullptr;
  if (font_description.GenericFamily() == FontDescription::kSerifFamily) {
    generic_family_name = "serif";
  }

  AtomicString family_name = GetFamilyNameForCharacter(
      fm.get(), c, font_description, generic_family_name, fallback_priority);
  if (family_name.empty()) {
    return GetLastResortFallbackFont(font_description);
  }

  return FontDataFromFontPlatformData(GetFontPlatformData(
      font_description, FontFaceCreationParams(family_name)));
}
}  // namespace blink
