/*
 * Copyright (c) 2006, 2007, 2008, 2009 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <unicode/locid.h>

#include <memory>
#include <utility>

#include "arkweb/build/features/features.h"
#include "base/check_op.h"
#include "base/metrics/histogram_functions.h"
#include "base/notreached.h"
#include "build/build_config.h"
#include "skia/ext/font_utils.h"
#include "third_party/blink/public/platform/linux/web_sandbox_support.h"
#include "third_party/blink/public/platform/platform.h"
#include "third_party/blink/renderer/platform/font_family_names.h"
#include "third_party/blink/renderer/platform/fonts/alternate_font_family.h"
#include "third_party/blink/renderer/platform/fonts/bitmap_glyphs_block_list.h"
#include "third_party/blink/renderer/platform/fonts/font_cache.h"
#include "third_party/blink/renderer/platform/fonts/font_description.h"
#include "third_party/blink/renderer/platform/fonts/font_face_creation_params.h"
#include "third_party/blink/renderer/platform/fonts/font_fallback_priority.h"
#include "third_party/blink/renderer/platform/fonts/font_global_context.h"
#include "third_party/blink/renderer/platform/fonts/simple_font_data.h"
#include "third_party/blink/renderer/platform/fonts/skia/sktypeface_factory.h"
#include "third_party/blink/renderer/platform/language.h"
#include "third_party/blink/renderer/platform/runtime_enabled_features.h"
#include "third_party/blink/renderer/platform/wtf/text/atomic_string.h"
#include "third_party/skia/include/core/SkFontMgr.h"
#include "third_party/skia/include/core/SkStream.h"
#include "third_party/skia/include/core/SkTypeface.h"

#if BUILDFLAG(IS_MAC)
#error This file should not be used by MacOS.
#endif

#if BUILDFLAG(ARKWEB_THEME_FONT)
constexpr SkFourByteTag kWghtTag = SkSetFourByteTag('w', 'g', 'h', 't');
#endif

namespace blink {
AtomicString ToAtomicString(const SkString& str) {
  return AtomicString::FromUTF8(std::string_view(str.begin(), str.end()));
}

namespace {
const FontPlatformData* CreateFontPlatformDataForTypeface(
    sk_sp<SkTypeface> typeface,
    const FontDescription& font_description) {
  if (!typeface) {
    return nullptr;
  }

  SkString skia_family_name;
  typeface->getFamilyName(&skia_family_name);

  bool synthetic_bold = font_description.Weight() >= kBoldThreshold &&
                        !typeface->isBold() &&
                        font_description.SyntheticBoldAllowed();
  bool synthetic_italic = font_description.Style() > kNormalSlopeValue &&
                          !typeface->isItalic() &&
                          font_description.SyntheticItalicAllowed();

  return MakeGarbageCollected<FontPlatformData>(
      std::move(typeface), skia_family_name.c_str(),
      font_description.EffectiveFontSize(), synthetic_bold, synthetic_italic,
      font_description.TextRendering(), ResolvedFontFeatures(),
      font_description.Orientation());
}
}  // namespace

#if BUILDFLAG(ARKWEB_CSS_FONT)
base::Lock& GetCacheLock() {
  DEFINE_STATIC_LOCAL(base::Lock, lock,());
  return lock;
}
using FontVariantKey = std::pair<SkTypefaceID, int>;
using TypefaceCache = HashMap<FontVariantKey, sk_sp<SkTypeface>>;
TypefaceCache& GetTypefaceCache() {
  DEFINE_STATIC_LOCAL(TypefaceCache, cache, ());
  return cache;
}

sk_sp<SkTypeface> ApplyWeightToVariableTypeface(sk_sp<SkTypeface> typeface,
    const FontDescription& font_description) {
  if (!typeface) {
    return nullptr;
  }

  int existing_axes = typeface->getVariationDesignPosition({});
  if (existing_axes <= 0) {
      return typeface;
  }

  Vector<SkFontArguments::VariationPosition::Coordinate> coordinates_to_set;
  coordinates_to_set.resize(existing_axes);

  if (typeface->getVariationDesignPosition(coordinates_to_set) != existing_axes) {
      return typeface;
  }
  bool hasChanged = false;
  for (auto& coordinate : coordinates_to_set) {
    if (coordinate.axis == kWghtTag) {
      coordinate.value =
          SkFloatToScalar(font_description.SkiaFontStyle().weight());
      hasChanged = true;
    }
  }
  if (!hasChanged) {
    SkFontArguments::VariationPosition::Coordinate coordinate;
    coordinate.axis = kWghtTag;
    coordinate.value =
        SkFloatToScalar(font_description.SkiaFontStyle().weight());
    coordinates_to_set.push_back(coordinate);
  }
  SkFontArguments::VariationPosition variation_design_position{
      coordinates_to_set.data(), static_cast<int>(coordinates_to_set.size())};
  SkFontArguments fontArgs;
  fontArgs.setVariationDesignPosition(variation_design_position);
  return typeface->makeClone(fontArgs);
}

void FontCache::TypefaceCacheClear() {
  TypefaceCache tmp_map;
  {
    base::AutoLock auto_lock(GetCacheLock());
    tmp_map.swap(GetTypefaceCache());
  }
}
#endif

#if BUILDFLAG(IS_ANDROID) || BUILDFLAG(IS_WIN) || BUILDFLAG(IS_OHOS)
// static
const FontPlatformData* FontCache::CreateFontPlatformDataForCharacter(
    SkFontMgr* fm,
    UChar32 c,
    const FontDescription& font_description,
    const char* family_name,
    FontFallbackPriority fallback_priority) {
  DCHECK(fm);

  Bcp47Vector locales =
      GetBcp47LocaleForRequest(font_description, fallback_priority);
  sk_sp<SkTypeface> typeface(fm->matchFamilyStyleCharacter(
      family_name, font_description.SkiaFontStyle(), locales.data(),
      locales.size(), c));

#if BUILDFLAG(ARKWEB_CSS_FONT)
  if (!typeface) {
    return nullptr;
  }
  
  FontVariantKey key = {typeface->uniqueID(), font_description.SkiaFontStyle().weight()};
  sk_sp<SkTypeface> result;
  {
    base::AutoLock auto_lock(GetCacheLock());
    TypefaceCache& typefaceCache = GetTypefaceCache();
    auto it = typefaceCache.find(key);
    if (it != typefaceCache.end()) {
      result = it->value;
    }
  }
  if (!result) {
    result = ApplyWeightToVariableTypeface(typeface, font_description);
    base::AutoLock auto_lock(GetCacheLock());
    GetTypefaceCache().insert(key, result);
  }
  typeface = result;
#endif

  return CreateFontPlatformDataForTypeface(std::move(typeface),
                                           font_description);
}
#endif

void FontCache::PlatformInit() {}

const SimpleFontData* FontCache::FallbackOnStandardFontStyle(
    const FontDescription& font_description,
    UChar32 character) {
  FontDescription substitute_description(font_description);
  substitute_description.SetStyle(kNormalSlopeValue);
  substitute_description.SetWeight(kNormalWeightValue);

  FontFaceCreationParams creation_params(
      substitute_description.Family().FamilyName());
  const FontPlatformData* substitute_platform_data =
      GetFontPlatformData(substitute_description, creation_params);
  if (substitute_platform_data &&
      substitute_platform_data->FontContainsCharacter(character)) {
    FontPlatformData* platform_data =
        MakeGarbageCollected<FontPlatformData>(*substitute_platform_data);
    platform_data->SetSyntheticBold(font_description.Weight() >=
                                        kBoldThreshold &&
                                    font_description.SyntheticBoldAllowed());
    platform_data->SetSyntheticItalic(
        font_description.Style() == kItalicSlopeValue &&
        font_description.SyntheticItalicAllowed());
    return FontDataFromFontPlatformData(platform_data);
  }

  return nullptr;
}

const SimpleFontData* FontCache::GetLastResortFallbackFont(
    const FontDescription& description) {
  const FontFaceCreationParams fallback_creation_params(
      GetFallbackFontFamily(description));
  const FontPlatformData* font_platform_data = GetFontPlatformData(
      description, fallback_creation_params, AlternateFontName::kLastResort);

  int last_resort_fallback_attempt = 0;

  // We should at least have Sans or Arial which is the last resort fallback of
  // SkFontHost ports.
  if (!font_platform_data) {
    DEFINE_THREAD_SAFE_STATIC_LOCAL(const FontFaceCreationParams,
                                    sans_creation_params,
                                    (font_family_names::kSans));
    font_platform_data = GetFontPlatformData(description, sans_creation_params,
                                             AlternateFontName::kLastResort);
    ++last_resort_fallback_attempt;
  }
  if (!font_platform_data) {
    DEFINE_THREAD_SAFE_STATIC_LOCAL(const FontFaceCreationParams,
                                    arial_creation_params,
                                    (font_family_names::kArial));
    font_platform_data = GetFontPlatformData(description, arial_creation_params,
                                             AlternateFontName::kLastResort);
    ++last_resort_fallback_attempt;
  }
#if BUILDFLAG(IS_WIN)
  // Try some more Windows-specific fallbacks.
  if (!font_platform_data) {
    DEFINE_THREAD_SAFE_STATIC_LOCAL(const FontFaceCreationParams,
                                    msuigothic_creation_params,
                                    (font_family_names::kMSUIGothic));
    font_platform_data =
        GetFontPlatformData(description, msuigothic_creation_params,
                            AlternateFontName::kLastResort);
    ++last_resort_fallback_attempt;
  }
  if (!font_platform_data) {
    DEFINE_THREAD_SAFE_STATIC_LOCAL(const FontFaceCreationParams,
                                    mssansserif_creation_params,
                                    (font_family_names::kMicrosoftSansSerif));
    font_platform_data =
        GetFontPlatformData(description, mssansserif_creation_params,
                            AlternateFontName::kLastResort);
    ++last_resort_fallback_attempt;
  }
  if (!font_platform_data) {
    DEFINE_THREAD_SAFE_STATIC_LOCAL(const FontFaceCreationParams,
                                    segoeui_creation_params,
                                    (font_family_names::kSegoeUI));
    font_platform_data = GetFontPlatformData(
        description, segoeui_creation_params, AlternateFontName::kLastResort);
    ++last_resort_fallback_attempt;
  }
  if (!font_platform_data) {
    DEFINE_THREAD_SAFE_STATIC_LOCAL(const FontFaceCreationParams,
                                    calibri_creation_params,
                                    (font_family_names::kCalibri));
    font_platform_data = GetFontPlatformData(
        description, calibri_creation_params, AlternateFontName::kLastResort);
    ++last_resort_fallback_attempt;
  }
  if (!font_platform_data) {
    DEFINE_THREAD_SAFE_STATIC_LOCAL(const FontFaceCreationParams,
                                    timesnewroman_creation_params,
                                    (font_family_names::kTimesNewRoman));
    font_platform_data =
        GetFontPlatformData(description, timesnewroman_creation_params,
                            AlternateFontName::kLastResort);
    ++last_resort_fallback_attempt;
  }
  if (!font_platform_data) {
    DEFINE_THREAD_SAFE_STATIC_LOCAL(const FontFaceCreationParams,
                                    couriernew_creation_params,
                                    (font_family_names::kCourierNew));
    font_platform_data =
        GetFontPlatformData(description, couriernew_creation_params,
                            AlternateFontName::kLastResort);
    ++last_resort_fallback_attempt;
  }
#endif

#if BUILDFLAG(IS_ARKWEB)
  if (!font_platform_data) {
    DEFINE_THREAD_SAFE_STATIC_LOCAL(const FontFaceCreationParams,
                                    ohos_sans_creation_params,
                                    (font_family_names::kHarmonyOSSans));
    font_platform_data = GetFontPlatformData(
        description, ohos_sans_creation_params, AlternateFontName::kLastResort);
  }
#endif

#if BUILDFLAG(IS_ANDROID) || BUILDFLAG(IS_WIN)
  if (!font_platform_data) {
    // At least try to match locale.
    font_platform_data = FontCache::CreateFontPlatformDataForCharacter(
        skia::DefaultFontMgr().get(), ' ', description, nullptr,
        FontFallbackPriority::kText);
    ++last_resort_fallback_attempt;
  }
#endif

  if (!font_platform_data) {
    // Match anything.
    font_platform_data = CreateFontPlatformDataForTypeface(
        skia::DefaultFontMgr()->legacyMakeTypeface(nullptr,
                                                   description.SkiaFontStyle()),
        description);
    ++last_resort_fallback_attempt;
  }

  // 0 <= last_resort_fallback_attempt <= 10 (9 on linux), so set the max to 11
  // and put failed attempts in that bucket.
  static const int kMaxAttempts = 11;
  if (!font_platform_data) {
    last_resort_fallback_attempt = kMaxAttempts;
  }
  base::UmaHistogramExactLinear(
      "Blink.Fonts.LastResortAttemptsUntilStaticMatch2",
      last_resort_fallback_attempt, kMaxAttempts);
  base::UmaHistogramBoolean("Blink.Fonts.LastResortFallbackFound",
                            font_platform_data != nullptr);
  DCHECK(font_platform_data);
  return FontDataFromFontPlatformData(font_platform_data);
}

sk_sp<SkTypeface> FontCache::CreateTypeface(
    const FontDescription& font_description,
    const FontFaceCreationParams& creation_params,
    std::string& name) {
#if !BUILDFLAG(IS_WIN) && !BUILDFLAG(IS_ANDROID) && !BUILDFLAG(IS_FUCHSIA)
  // TODO(fuchsia): Revisit this and other font code for Fuchsia.

  if (creation_params.CreationType() == kCreateFontByFciIdAndTtcIndex) {
    if (Platform::Current()->GetSandboxSupport()) {
      return SkTypeface_Factory::FromFontConfigInterfaceIdAndTtcIndex(
          creation_params.FontconfigInterfaceId(), creation_params.TtcIndex());
    }
    return SkTypeface_Factory::FromFilenameAndTtcIndex(
        creation_params.Filename().data(), creation_params.TtcIndex());
  }
#endif

  const AtomicString& family = creation_params.Family();
  DCHECK_NE(family, font_family_names::kSystemUi);
  // convert the name to utf8
  name = family.Utf8();

#if BUILDFLAG(IS_ANDROID)
  // If this is a locale-specific family, try looking up locale-specific
  // typeface first.
  if (const char* locale_family = GetLocaleSpecificFamilyName(family)) {
    if (sk_sp<SkTypeface> typeface =
            CreateLocaleSpecificTypeface(font_description, locale_family))
      return typeface;
  }
#endif  // BUILDFLAG(IS_ANDROID)
#if BUILDFLAG(IS_OHOS)
  auto typeface = sk_sp<SkTypeface>(skia::DefaultFontMgr()->matchFamilyStyle(
      name.empty() ? nullptr : name.c_str(), font_description.SkiaFontStyle()));
  return ApplyWeightToVariableTypeface(typeface, font_description);
#else
  return sk_sp<SkTypeface>(skia::DefaultFontMgr()->matchFamilyStyle(
      name.empty() ? nullptr : name.c_str(), font_description.SkiaFontStyle()));
#endif      
}

#if !BUILDFLAG(IS_WIN)
const FontPlatformData* FontCache::CreateFontPlatformData(
    const FontDescription& font_description,
    const FontFaceCreationParams& creation_params,
    float font_size,
    AlternateFontName alternate_name) {
  std::string name;

  sk_sp<SkTypeface> typeface;
#if BUILDFLAG(IS_ANDROID) || BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_CHROMEOS) || BUILDFLAG(ARKWEB_WPT) 
  bool noto_color_emoji_from_gmscore = false;
#if BUILDFLAG(IS_ANDROID)
  // Use the unique local matching pathway for fetching Noto Color Emoji Compat
  // from GMS core if this family is requested, see font_cache_android.cc. Noto
  // Color Emoji Compat is an up-to-date emoji font shipped with GMSCore which
  // provides better emoji coverage and emoji sequence support than the firmware
  // Noto Color Emoji font.
  noto_color_emoji_from_gmscore =
      (creation_params.CreationType() ==
           FontFaceCreationType::kCreateFontByFamily &&
       creation_params.Family() == kNotoColorEmojiCompat);
#endif
  if (RuntimeEnabledFeatures::FontSrcLocalMatchingEnabled() &&
      (alternate_name == AlternateFontName::kLocalUniqueFace ||
       noto_color_emoji_from_gmscore)) {
    typeface = CreateTypefaceFromUniqueName(creation_params);
  } else {
    typeface = CreateTypeface(font_description, creation_params, name);
  }
#else
  typeface = CreateTypeface(font_description, creation_params, name);
#endif

  if (!typeface)
    return nullptr;

  bool synthetic_bold =
      (font_description.Weight() >
           FontSelectionValue(200) +
               FontSelectionValue(typeface->fontStyle().weight()) ||
       font_description.IsSyntheticBold()) &&
      font_description.GetFontSynthesisWeight() ==
          FontDescription::kAutoFontSynthesisWeight;

  bool synthetic_italic = (((font_description.Style() == kItalicSlopeValue) &&
                            !typeface->isItalic()) ||
                           font_description.IsSyntheticItalic()) &&
                          font_description.GetFontSynthesisStyle() ==
                              FontDescription::kAutoFontSynthesisStyle;

  FontPlatformData* font_platform_data = MakeGarbageCollected<FontPlatformData>(
      typeface, name, font_size, synthetic_bold, synthetic_italic,
      font_description.TextRendering(), font_description.ResolveFontFeatures(),
      font_description.Orientation());

  font_platform_data->SetAvoidEmbeddedBitmaps(
      BitmapGlyphsBlockList::ShouldAvoidEmbeddedBitmapsForTypeface(*typeface));

  return font_platform_data;
}
#endif  // !BUILDFLAG(IS_WIN)

}  // namespace blink
