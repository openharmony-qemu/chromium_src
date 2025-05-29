// Copyright (c) 2021 Huawei Device Co., Ltd. All rights reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "FontConfig_ohos.h"

#include <dirent.h>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "SkFontMgr_ohos.h"
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "base/logging.h"
#include "include/core/SkFontStyle.h"
#include "include/core/SkString.h"
#include "src/ports/SkFontScanner_FreeType_priv.h"

using namespace ErrorCode;

SkString OHOS_FONT_INSTALL_DIR("/data/service/el1/public/for-all-app/fonts/");

/*! Constructor
 * \param fontScanner the scanner to get the font information from a font file
 * \param fname the full name of system font configuration document.
 *     \n The default value is '/system/etc/fontconfig.json', if fname is given
 * null
 */
FontConfig_OHOS::FontConfig_OHOS(const SkFontScanner& fontScanner,
                                 const char* fname) {
  buildNameToFamilyMap();
  buildStyleNameToFamilyMap(OHOS::NWeb::ArkWeb_Drawing_SystemFontType::STYLISH);
  for (const auto& dir : fontDirSet) {
    scanFonts(fontScanner, dir, false);
  }
  if (base::ohos::IsTabletDevice() || base::ohos::IsPcDevice()) {
    scanFonts(fontScanner, OHOS_FONT_INSTALL_DIR, true);
  }
  resetGenericValue();
  resetFallbackValue();
}

/*! To build name to genericFamilySet and fallbackSet
 */
void FontConfig_OHOS::buildNameToFamilyMap() {
  OHOS::NWeb::ArkWeb_Drawing_FontConfigInfo* fontconfig = nullptr;
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .GetOhosDrawingTextTypographyAdapter()
      .GetSystemFontConfigInfo(nullptr, reinterpret_cast<void**>(&fontconfig));
  if (fontconfig == nullptr) {
    LOG(ERROR) << "the font configuration class was empty.";
    return;
  }
  for (int i = 0; i < fontconfig->fontDirSize; ++i) {
    fontDirSet.push_back(SkString(fontconfig->fontDirSet[i]));
  }
  for (int i = 0; i < fontconfig->fontGenericInfoSize; i++) {
    auto familyName = fontconfig->fontGenericInfoSet[i].familyName;
    std::vector<AliasInfo> aliasSet;
    for (int j = 0; j < fontconfig->fontGenericInfoSet[i].aliasInfoSize; j++) {
      auto weight = fontconfig->fontGenericInfoSet[i].aliasInfoSet[j].weight;
      auto innerFamilyName =
          fontconfig->fontGenericInfoSet[i].aliasInfoSet[j].familyName;
      std::unique_ptr<GenericFamily> genericFamily =
          std::make_unique<GenericFamily>();
      genericFamily->familyName = SkString(innerFamilyName);
      if (aliasSet.size() == 0 || weight > 0) {
        genericFamily->typefaceSet = std::make_shared<TypefaceSet>();
      } else {
        int index = aliasSet[0].pos;
        genericFamily->typefaceSet = genericFamilySet[index]->typefaceSet;
      }
      genericNames.set(SkString(genericFamily->familyName),
                       genericFamilySet.size());
      AliasInfo info = {static_cast<int>(genericFamilySet.size()), weight};
      aliasSet.emplace_back(std::move(info));
      genericFamilySet.emplace_back(std::move(genericFamily));
    }

    if (aliasSet.size()) {
      aliasMap.set(SkString(familyName), aliasSet);
    }

    std::vector<AdjustInfo> adjustSet;
    for (int j = 0; j < fontconfig->fontGenericInfoSet[i].adjustInfoSize; j++) {
      auto weight = fontconfig->fontGenericInfoSet[i].adjustInfoSet[j].weight;
      auto to = fontconfig->fontGenericInfoSet[i].adjustInfoSet[j].to;
      AdjustInfo info = {weight, to};
      adjustSet.push_back(info);
    }

    if (adjustSet.size()) {
      adjustMap.set(SkString(familyName), adjustSet);
    }
  }

  for (int i = 0; i < fontconfig->fallbackGroupSize; i++) {
    unsigned int startPos = fallbackSet.size();
    SkString fallbackFor("");
    for (int j = 0; j < fontconfig->fallbackGroupSet[i].fallbackInfoSize; j++) {
      auto langs = fontconfig->fallbackGroupSet[i].fallbackInfoSet[j].language;
      auto familyName =
          fontconfig->fallbackGroupSet[i].fallbackInfoSet[j].familyName;
      std::unique_ptr<FallbackInfo> fallback = std::make_unique<FallbackInfo>();
      fallback->langs = SkString(langs);
      fallback->familyName = SkString(familyName);
      fallback->typefaceSet = std::make_shared<TypefaceSet>();
      fallbackNames.set(SkString(familyName), fallbackSet.size());
      fallbackSet.emplace_back(std::move(fallback));
    }
    FallbackSetPos setPos = {startPos,
                             (unsigned int)(fallbackSet.size() - startPos)};
    fallbackForMap.set(fallbackFor, setPos);
  }
}

/*! To build stylish and installed name to fallbackSet
 */
void FontConfig_OHOS::buildStyleNameToFamilyMap(
    OHOS::NWeb::ArkWeb_Drawing_SystemFontType fontType) {
  void* fontList = nullptr;
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .GetOhosDrawingTextFontAdapter()
      .GetSystemFontFullNamesByType(fontType,
                                    reinterpret_cast<void**>(&fontList));
  int32_t size = 0;
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .GetOhosDrawingTextTypographyAdapter()
      .GetDrawingArraySize(fontList, size);
  SkString fallbackFor("");
  unsigned int startPos = 0;
  for (size_t index = 0; index < size; ++index) {
    const void* fontFullName = nullptr;
    OHOS::NWeb::OhosAdapterHelper::GetInstance()
        .GetOhosDrawingTextFontAdapter()
        .GetSystemFontFullNameByIndex(fontList, index, &fontFullName);
    OHOS::NWeb::ArkWeb_Drawing_FontDescriptor* description = nullptr;
    const OHOS::NWeb::ArkWeb_Drawing_String* drawingString =
        static_cast<const OHOS::NWeb::ArkWeb_Drawing_String*>(fontFullName);
    OHOS::NWeb::ArkWeb_Drawing_String* nonConstDrawingString =
        const_cast<OHOS::NWeb::ArkWeb_Drawing_String*>(drawingString);
    OHOS::NWeb::OhosAdapterHelper::GetInstance()
        .GetOhosDrawingTextFontAdapter()
        .GetFontDescriptorByFullName(nonConstDrawingString, fontType,
                                     reinterpret_cast<void**>(&description));
    if (description == nullptr) {
      LOG(ERROR) << "one font description was empty at index:" << index;
      continue;
    }
    std::unique_ptr<FallbackInfo> fallback = std::make_unique<FallbackInfo>();
    fallback->familyName = SkString(description->fontFamily);
    fallback->typefaceSet = std::make_shared<TypefaceSet>();
    fallbackNames.set(SkString(description->fontFamily), fallbackSet.size());
    fallbackSet.emplace_back(std::move(fallback));
    pathToFamily.set(SkString(description->path),
                     SkString(description->fontFamily));
    OHOS::NWeb::OhosAdapterHelper::GetInstance()
        .GetOhosDrawingTextFontAdapter()
        .DestroyFontDescriptor(description);
    description = nullptr;
  }

  FallbackSetPos setPos = {startPos,
                           (unsigned int)(fallbackSet.size() - startPos)};
  fallbackForMap.set(fallbackFor, setPos);
}

/*! To get the fallbackForMap
 *  \return The reference of fallbackForMap
 */
const FallbackForMap& FontConfig_OHOS::getFallbackForMap() const {
  return fallbackForMap;
}

/*! To get the fallback set
 *  \return The reference of fallbackSet
 */
const FallbackSet& FontConfig_OHOS::getFallbackSet() const {
  return fallbackSet;
}

/*! To get the count of font style sets supported in the system
 *  \return The count of font style sets in generic family
 */
int FontConfig_OHOS::getFamilyCount() const {
  return genericFamilySet.size();
}

/*! To get the family name of the default font style set
 *  \param[out] familyName a pointer of SkString object, to which the family
 * value will be set. \return The count of typeface in this font style set \n
 * Return -1, if there is no any font style set in the system.
 */
int FontConfig_OHOS::getDefaultFamily(SkString* familyName) const {
  return getFamilyName(0, familyName);
}

/*! To get the family name of a font style set
 * \param index the index of a font style set in generic family
 * \param[out] familyName a pointer of SkString object, to which the family
 * value will be set \return The count of typeface in the font style set \n
 * Return -1, if the 'index' is out of range
 */
int FontConfig_OHOS::getFamilyName(int index, SkString* familyName) const {
  if (index < 0 || index >= this->getFamilyCount()) {
    if (familyName) {
      familyName->reset();
    }
    return -1;
  }
  if (familyName) {
    *familyName = genericFamilySet[index]->familyName;
  }
  return genericFamilySet[index]->typefaceSet->size();
}

/*! To get the count of a font style set
 * \param styleIndex the index of a font style set
 * \param isFallback to indicate the font style set is from generic family or
 * fallback family \n                 false , the font style set is from generic
 * family list \n                 true, the font style set is from fallback
 * family list \return The count of typeface in the font style set
 */
int FontConfig_OHOS::getTypefaceCount(int styleIndex, bool isFallback) const {
  if (styleIndex < 0) {
    return -1;
  }
  if (isFallback) {
    if ((unsigned int)styleIndex < fallbackSet.size()) {
      return fallbackSet[styleIndex]->typefaceSet->size();
    }
  } else {
    if ((unsigned int)styleIndex < genericFamilySet.size()) {
      return genericFamilySet[styleIndex]->typefaceSet->size();
    }
  }
  return -1;
}

/*! To get a typeface
 * \param styleIndex the index of a font style set
 * \param index the index of a typeface in its style set
 * \param isFallback false, the font style set is generic
 * \n          true, the font style set is fallback
 * \return The pointer of a typeface
 * \n       Return null, if 'styleIndex' or 'index' is out of range
 */
SkTypeface_OHOS* FontConfig_OHOS::getTypeface(int styleIndex,
                                              int index,
                                              bool isFallback) const {
  if (styleIndex < 0 || index < 0 ||
      (isFallback && (unsigned int)styleIndex >= fallbackSet.size()) ||
      (!isFallback && (unsigned int)styleIndex >= genericFamilySet.size())) {
    return nullptr;
  }
  if (isFallback) {
    const TypefaceSet& tpSet = *(fallbackSet[styleIndex]->typefaceSet.get());
    if ((unsigned int)index < tpSet.size()) {
      return tpSet[index].get();
    }
  } else {
    const TypefaceSet& tpSet =
        *(genericFamilySet[styleIndex]->typefaceSet.get());
    if ((unsigned int)index < tpSet.size()) {
      return tpSet[index].get();
    }
  }
  return nullptr;
}

/*! To get a typeface
 * \param styleIndex the index a font style set
 * \param style the font style to be matching
 * \param isFallback false, the font style set is generic
 * \n                true, the font style set is fallback
 * \return An object of typeface whose font style is the closest matching to
 * 'style' \n      Return null, if 'styleIndex' is out of range
 */
SkTypeface_OHOS* FontConfig_OHOS::getTypeface(int styleIndex,
                                              const SkFontStyle& style,
                                              bool isFallback) const {
  if (styleIndex < 0 ||
      (isFallback && (unsigned int)styleIndex >= fallbackSet.size()) ||
      (!isFallback && (unsigned int)styleIndex >= genericFamilySet.size())) {
    return nullptr;
  }
  const TypefaceSet* pSet = nullptr;
  if (isFallback) {
    pSet = fallbackSet[styleIndex]->typefaceSet.get();
  } else {
    pSet = genericFamilySet[styleIndex]->typefaceSet.get();
  }
  sk_sp<SkTypeface_OHOS> tp = matchFontStyle(*pSet, style);
  if (tp.get() != nullptr) {
    return tp.get();
  }
  return nullptr;
}

/*! To get the index of a font style set
 *  \param familyName the family name of the font style set
 *  \n     get the index of default font style set, if 'familyName' is null
 *  \param[out] isFallback to tell if the family is from generic or fallback to
 * the caller. \n          isFallback is false, if the font style is from
 * generic family list \n          isFallback is true, if the font style is from
 * fallback family list \return The index of the font style set \n      Return
 * -1, if 'familyName' is not found in the system
 */
int FontConfig_OHOS::getStyleIndex(const char* familyName,
                                   bool& isFallback) const {
  if (familyName == nullptr) {
    isFallback = false;
    return 0;
  }
  SkString fname(familyName);
  int* p = genericNames.find(fname);
  if (p) {
    isFallback = false;
    return *p;
  } else {
    p = fallbackNames.find(fname);
    if (p) {
      isFallback = true;
      return *p;
    }
  }
  return -1;
}

/*! Find the closest matching typeface
 * \param typefaceSet a typeface set belonging to the same font style set
 * \param pattern the font style to be matching
 * \return The typeface object which is the closest matching to 'pattern'
 * \n      Return null, if the count of typeface is 0
 */
sk_sp<SkTypeface_OHOS> FontConfig_OHOS::matchFontStyle(
    const TypefaceSet& typefaceSet,
    const SkFontStyle& pattern) {
  int count = typefaceSet.size();
  if (count == 0) {
    return nullptr;
  } else if (count == 1) {
    return typefaceSet[0];
  }
  uint32_t minDiff = 0xFFFFFFFF;
  int index = 0;
  for (int i = 0; i < count; i++) {
    const SkFontStyle& fontStyle = typefaceSet[i]->fontStyle();
    uint32_t diff = getFontStyleDifference(pattern, fontStyle);
    if (diff < minDiff) {
      minDiff = diff;
      index = i;
    }
  }
  return typefaceSet[index];
}

/*! To get the difference between a font style and the matching font style
 * \param dstStyle the style to be matching
 * \param srcStyle a font style
 * \return The difference value of a specified style with the matching style
 */
uint32_t FontConfig_OHOS::getFontStyleDifference(const SkFontStyle& dstStyle,
                                                 const SkFontStyle& srcStyle) {
  int normalWidth = SkFontStyle::kNormal_Width;
  int dstWidth = dstStyle.width();
  int srcWidth = srcStyle.width();

  uint32_t widthDiff = 0;
  // The maximum font width is kUltraExpanded_Width i.e. '9'.
  // If dstWidth <= kNormal_Width (5), first check narrower values, then wider
  // values. If dstWidth > kNormal_Width, first check wider values, then
  // narrower values. When dstWidth and srcWidth are at different side of
  // kNormal_Width, the width difference between them should be more than 5
  // (9/2+1)
  if (dstWidth <= normalWidth) {
    if (srcWidth <= dstWidth) {
      widthDiff = dstWidth - srcWidth;
    } else {
      widthDiff = srcWidth - dstWidth + 5;
    }
  } else {
    if (srcWidth >= dstWidth) {
      widthDiff = srcWidth - dstWidth;
    } else {
      widthDiff = dstWidth - srcWidth + 5;
    }
  }

  int diffSlantValue[3][3] = {{0, 2, 1}, {2, 0, 1}, {2, 1, 0}};
  uint32_t slantDiff = diffSlantValue[dstStyle.slant()][srcStyle.slant()];

  int dstWeight = dstStyle.weight();
  int srcWeight = srcStyle.weight();
  uint32_t weightDiff = 0;
  // If dstWeight == kNormal_Weight (400), first check kMedium_Weight (500),
  // then smaller values, and then bigger values. If dstWeight ==
  // kMedium_Weight, first check kNormal_Weight, then bigger values, and then
  // smaller values. so we set the difference of kNormal_Weight and
  // kMedium_Weight is 50 in order to differ from other cases. If dstWeight <
  // kNormal_Weight, first check smaller values, then bigger values. If
  // dstWeight > kNormal_Weight, first check bigger values, then smaller values.
  // The maximum weight is kExtraBlack_Weight (1000), when dstWeight and
  // srcWeight are at the different side of kNormal_Weight, the weight
  // difference between them should be more than 500 (1000/2)
  if ((dstWeight == SkFontStyle::kNormal_Weight &&
       srcWeight == SkFontStyle::kMedium_Weight) ||
      (dstWeight == SkFontStyle::kMedium_Weight &&
       srcWeight == SkFontStyle::kNormal_Weight)) {
    weightDiff = 50;
  } else if (dstWeight <= SkFontStyle::kNormal_Weight) {
    if (srcWeight <= dstWeight) {
      weightDiff = dstWeight - srcWeight;
    } else {
      weightDiff = srcWeight - dstWeight + 500;
    }
  } else if (dstWeight > SkFontStyle::kNormal_Weight) {
    if (srcWeight >= dstWeight) {
      weightDiff = srcWeight - dstWeight;
    } else {
      weightDiff = dstWeight - srcWeight + 500;
    }
  }
  // The first 2 bytes to save weight difference, the third byte to save slant
  // difference, and the fourth byte to save width difference
  uint32_t diff = (widthDiff << 24) + (slantDiff << 16) + weightDiff;
  return diff;
}
#if ENABLE_DEBUG
/*! To print out the font information
 * \param font the font object to be printed
 */
void FontConfig_OHOS::dumpFont(const FontInfo& font) const {
  LOGI(
      "name=%s, family=%s, weight=%d, width=%d, slant=%d, index=%d, "
      "stream=%p\n",
      font.fname.c_str(), font.familyName.c_str(), font.style.weight(),
      font.style.width(), font.style.slant(), font.index, font.stream.get());
  int count = font.axisSet.axis.size();
  if (count > 0) {
    SkString str;
    for (unsigned int i = 0; i < count; i++) {
      str.appendU32(SkFixedFloorToInt(font.axisSet.axis[i]));
      if (i < count - 1) {
        str.append(",");
      }
    }
    LOGI("axis={%s}\n", str.c_str());
  }
}

/*! To print out the information of generic font style set
 */
void FontConfig_OHOS::dumpGeneric() const {
  LOGI("\n");
  for (unsigned int i = 0; i < genericFamilySet.size(); i++) {
    LOGI("[%d] familyName : %s - %d\n", i,
         genericFamilySet[i]->familyName.c_str(),
         static_cast<int>(genericFamilySet[i]->typefaceSet->size()));
    for (int j = 0; j < genericFamilySet[i]->typefaceSet->size(); j++) {
      if ((*(genericFamilySet[i]->typefaceSet))[j].get()) {
        const FontInfo* font =
            (*(genericFamilySet[i]->typefaceSet))[j]->getFontInfo();
        if (font) {
          dumpFont(*font);
        } else {
          LOGE("font [%d] is null\n", j);
        }
      } else {
        LOGE("typefeace [%d] is null\n", j);
      }
    }
  }
}

/*! To print out the information of fallback font style set
 */
void FontConfig_OHOS::dumpFallback() const {
  LOGI("\n");
  int count = 0;
  fallbackForMap.foreach (
      [this, &count](const SkString& key, const FallbackSetPos& setIndex) {
        LOGI("[%d] family : %s - %d\n", count++, key.c_str(), setIndex.count);
        for (unsigned int i = setIndex.index;
             i < setIndex.index + setIndex.count; i++) {
          const TypefaceSet& tpSet = *(fallbackSet[i]->typefaceSet.get());
          LOGI("[%s] - %d\n", fallbackSet[i]->familyName.c_str(),
               static_cast<int>(tpSet.size()));

          for (unsigned int j = 0; j < tpSet.size(); j++) {
            const FontInfo* font = tpSet[j]->getFontInfo();
            if (font) {
              this->dumpFont(*font);
            } else {
              LOGE("font [%d] is null\n", j);
            }
          }
        }
      });
}
#endif

/*! To get the axis value and set to 'font'
 * \param axisDefs the axis ranges of a font
 * \param variation the variation data from which axis values are generated
 * \param[out] font the axis values will be written to and returned to the
 * caller
 */
void FontConfig_OHOS::getAxisValues(const AxisDefinitions& axisDefs,
                                    const VariationInfo& variation,
                                    FontInfo& font) const {
  SkFontArguments::VariationPosition position;
  position.coordinateCount = variation.axis.size();
  position.coordinates = variation.axis.data();

  int count = axisDefs.size();
  SkFixed axisValues[count];
  SkFontScanner_FreeType::computeAxisValues(axisDefs, position, axisValues,
                                            font.familyName, &font.style);
  font.axisSet.axis.clear();
  font.axisSet.range.clear();
  for (int i = 0; i < count; i++) {
    font.axisSet.axis.emplace_back(axisValues[i]);
    font.axisSet.range.emplace_back(axisDefs[i]);
  }
}

/*! To insert a ttc font into a font style set
 * \param count the count of typeface in a ttc font
 * \param font an object of the FontInfo with font information
 * \return true, if the font is a ttc font and added to corresponding font style
 * set \return false, if the font is not a ttc font
 */
bool FontConfig_OHOS::insertTtcFont(int count, FontInfo& font) {
  bool ret = false;
  ttcIndexMap.foreach ([this, count, &font, &ret](const SkString& familyName,
                                                  TtcIndexInfo* info) {
    if (info->familyName == font.familyName && info->ttcIndex < count) {
      SkString specifiedName;
      TypefaceSet* tpSet = this->getTypefaceSet(familyName, specifiedName);
      if (tpSet) {
        FontInfo newFont(font);
        newFont.familyName = familyName;
        newFont.index = info->ttcIndex;
        sk_sp<SkTypeface_OHOS> typeface =
            sk_make_sp<SkTypeface_OHOS>(specifiedName, newFont);
        tpSet->push_back(std::move(typeface));
        ret = true;
      }
    }
  });
  return ret;
}

/*! To insert a variable font into a font style set
 * \param axisDefs the axis ranges of a variable font
 * \param font an object of the FontInfo with font information
 * \return true, if the font is a variable and some typefaces are added to the
 * corresponding font style set \return false, if the font is not variable
 */
bool FontConfig_OHOS::insertVariableFont(const AxisDefinitions& axisDefs,
                                         FontInfo& font) {
  const SkString& key = font.familyName;
  if (variationMap.find(key) == nullptr || axisDefs.size() == 0) {
    return false;
  }
  SkString specifiedName;
  TypefaceSet* tpSet = getTypefaceSet(key, specifiedName);
  if (tpSet == nullptr) {
    return false;
  }
  const std::vector<VariationInfo>& variationSet = *(variationMap.find(key));
  for (unsigned int i = 0; i < variationSet.size(); i++) {
    FontInfo newFont(font);
    getAxisValues(axisDefs, variationSet[i], newFont);
    int width = font.style.width();
    SkFontStyle::Slant slant = font.style.slant();
    if (variationSet[i].width != -1) {
      width = variationSet[i].width;
    }
    if (variationSet[i].slant != -1) {
      slant = (SkFontStyle::Slant)variationSet[i].slant;
    }
    newFont.style = SkFontStyle(variationSet[i].weight, width, slant);
    sk_sp<SkTypeface_OHOS> typeface =
        sk_make_sp<SkTypeface_OHOS>(specifiedName, newFont);
    tpSet->push_back(std::move(typeface));
  }
  return true;
}

/*! To get the typeface set of a font style set
 * \param familyName the family name of a font style set
 * \param[out] specifiedName the specified family name of a font style set
 * returned to the caller \return The object of typeface set \n      Return
 * null, if the family name is not found in the system
 */
TypefaceSet* FontConfig_OHOS::getTypefaceSet(const SkString& familyName,
                                             SkString& specifiedName) const {
  if (aliasMap.find(familyName) != nullptr) {
    const std::vector<AliasInfo>& aliasSet = *(aliasMap.find(familyName));
    if (aliasSet.size()) {
      int index = aliasSet[0].pos;
      specifiedName = genericFamilySet[index]->familyName;
      return genericFamilySet[index]->typefaceSet.get();
    }
  } else if (fallbackNames.find(familyName) != nullptr) {
    int index = *(fallbackNames.find(familyName));
    return fallbackSet[index]->typefaceSet.get();
  }
  return nullptr;
}

/*! To load font information from a font file
 * \param scanner a scanner used to parse the font file
 * \param fname the full name of a font file
 * \return NO_ERROR successful
 * \return ERROR_FONT_NOT_EXIST font file is not exist
 * \return ERROR_FONT_INVALID_STREAM the stream is not recognized
 */
int FontConfig_OHOS::loadFont(const SkFontScanner& fontScanner,
                              const char* fname,
                              const bool& installedOrStyle) {
  std::unique_ptr<SkStreamAsset> stream = SkStream::MakeFromFile(fname);
  int count = 1;
  SkFontScanner::AxisDefinitions axisDefs;
  FontInfo font(fname, 0);
  if (stream == nullptr ||
      fontScanner.scanFile(stream.get(), &count) == false ||
      fontScanner.scanInstance(stream.get(), 0, 0, &font.familyName,
                               &font.style, &font.isFixedWidth,
                               &axisDefs) == false) {
    int err = NO_ERROR;
    if (stream == nullptr) {
      err = ERROR_FONT_NOT_EXIST;
    } else {
      err = ERROR_FONT_INVALID_STREAM;
    }
    LOGE("%s : %s\n", errToString(err), fname);
    char* fnameCopy = strdup(fname);
    errSet.emplace_back(err, basename(fnameCopy));
    free(fnameCopy);
    return err;
  }
  int installPathLen = strlen("/data/service/el1/public/for-all-app/fonts/");
  if (installedOrStyle &&
      strncmp(fname, OHOS_FONT_INSTALL_DIR.c_str(), installPathLen) == 0) {
    SkString fallbackFor("");
    unsigned int startPos = 0;
    std::unique_ptr<FallbackInfo> fallback = std::make_unique<FallbackInfo>();
    fallback->familyName = SkString(font.familyName);
    fallback->typefaceSet = std::make_shared<TypefaceSet>();
    fallbackNames.set(SkString(font.familyName), fallbackSet.size());
    fallbackSet.emplace_back(std::move(fallback));
    FallbackSetPos setPos = {startPos,
                             (unsigned int)(fallbackSet.size() - startPos)};
    fallbackForMap.set(fallbackFor, setPos);
  }
  // for adjustMap - update weight
  if (adjustMap.find(font.familyName) != nullptr) {
    const std::vector<AdjustInfo> adjustSet =
        *(adjustMap.find(font.familyName));
    for (unsigned int i = 0; i < adjustSet.size(); i++) {
      if (font.style.weight() == adjustSet[i].origValue) {
        font.style = SkFontStyle(adjustSet[i].newValue, font.style.width(),
                                 font.style.slant());
        break;
      }
    }
  }
  bool ret = false;
  if (count > 1) {
    ret = insertTtcFont(count, font);
  } else if (axisDefs.size() > 0) {
    ret = insertVariableFont(axisDefs, font);
  }
  auto familyName = pathToFamily.find(SkString(fname));
  if (familyName) {
    font.familyName = familyName->c_str();
  }
  if (!ret) {
    SkString specifiedName;
    TypefaceSet* tpSet = getTypefaceSet(font.familyName, specifiedName);
    if (tpSet) {
      sk_sp<SkTypeface_OHOS> typeface =
          sk_make_sp<SkTypeface_OHOS>(specifiedName, font);
      tpSet->push_back(std::move(typeface));
    }
  }
  return NO_ERROR;
}

/*! To scan the system font directories
 * \param fontScanner the scanner used to parse a font file
 * \return NO_ERROR success
 * \return ERROR_DIR_NOT_FOUND a font directory is not exist
 */
int FontConfig_OHOS::scanFonts(const SkFontScanner& fontScanner,
                               const SkString& path,
                               const bool& installedOrStyle) {
  int err = NO_ERROR;
  if (fontDirSet.size() == 0) {
    fontDirSet.emplace_back(path);
  }
  for (unsigned int i = 0; i < fontDirSet.size(); i++) {
    DIR* dir = opendir(fontDirSet[i].c_str());
    if (dir == nullptr) {
      err = logErrInfo(ERROR_DIR_NOT_FOUND, fontDirSet[i].c_str());
      continue;
    }
    struct dirent* node = nullptr;
    while ((node = readdir(dir))) {
      if (node->d_type != DT_REG) {
        continue;
      }
      const char* fname = node->d_name;
      int len = strlen(fname);
      int suffixLen = strlen(".ttf");
      if (len < suffixLen ||
          (strncasecmp(fname + len - suffixLen, ".ttf", suffixLen) &&
           strncasecmp(fname + len - suffixLen, ".otf", suffixLen) &&
           strncasecmp(fname + len - suffixLen, ".ttc", suffixLen) &&
           strncasecmp(fname + len - suffixLen, ".otc", suffixLen))) {
        continue;
      }
      SkString fullname = fontDirSet[i];
      if (fullname[fullname.size() - 1] != '/') {
        fullname.append("/");
      }
      fullname.append(fname);
      loadFont(fontScanner, fullname.c_str(), installedOrStyle);
    }
    closedir(dir);
  }
  fontDirSet.clear();
  return err;
}

/*! To reset the generic family
 * \n 1. To sort the typefaces for each font style set in generic list
 * \n 2. To build typeface set for those font style sets which have single
 * weight value
 */
void FontConfig_OHOS::resetGenericValue() {
  aliasMap.foreach (
      [this](const SkString& key, std::vector<AliasInfo>* pAliasSet) {
        std::vector<AliasInfo>& aliasSet = *pAliasSet;
        int index = aliasSet[0].pos;
        if (genericFamilySet[index]->typefaceSet->size() == 0) {
          this->logErrInfo(ERROR_FAMILY_NOT_FOUND, key.c_str());
        } else {
          sortTypefaceSet(genericFamilySet[index]->typefaceSet);
          for (unsigned int i = 1; i < aliasSet.size(); i++) {
            if (aliasSet[i].weight == 0) {
              continue;
            }
            buildSubTypefaceSet(genericFamilySet[index]->typefaceSet,
                                genericFamilySet[index + i]->typefaceSet,
                                genericFamilySet[index + i]->familyName,
                                aliasSet[i].weight);
            if (genericFamilySet[index + i]->typefaceSet->size() == 0) {
              this->logErrInfo(ERROR_FAMILY_NOT_FOUND,
                               genericFamilySet[index + i]->familyName.c_str());
            }
          }
        }
      });

  aliasMap.reset();
  adjustMap.reset();
  variationMap.reset();
  ttcIndexMap.reset();
}

/*! To build a sub typeface set according to weight from a typeface set
 * \param typefaceSet the parent typeface set
 * \param[out] subSet the sub typeface set returned to the caller
 * \param familyName the family name of the sub typeface set
 * \param weight the weight of the sub typeface set
 */
void FontConfig_OHOS::buildSubTypefaceSet(
    const std::shared_ptr<TypefaceSet>& typefaceSet,
    std::shared_ptr<TypefaceSet>& subSet,
    const SkString& familyName,
    int weight) {
  if (typefaceSet->size() == 0) {
    return;
  }
  for (unsigned int i = 0; i < typefaceSet->size(); i++) {
    const SkTypeface_OHOS* typeface = (*typefaceSet)[i].get();
    if (typeface && typeface->fontStyle().weight() == weight) {
      const FontInfo* pFont = typeface->getFontInfo();
      if (pFont == nullptr) {
        continue;
      }
      FontInfo font(*pFont);
      sk_sp<SkTypeface_OHOS> newTypeface =
          sk_make_sp<SkTypeface_OHOS>(familyName, font);
      subSet->push_back(std::move(newTypeface));
    }
  }
}

/*! To reset the fallback value
 * \n To sort the typefaces for each font style set in fallback list.
 */
void FontConfig_OHOS::resetFallbackValue() {
  for (unsigned int i = 0; i < fallbackSet.size(); i++) {
    if (fallbackSet[i]->typefaceSet->size() == 0) {
      logErrInfo(ERROR_FAMILY_NOT_FOUND, fallbackSet[i]->familyName.c_str());
    }
    sortTypefaceSet(fallbackSet[i]->typefaceSet);
  }
}

/*! To check if an error happened
 * \param err the id of an error
 * \param text the key to indicate the part with the error happened
 * \return false, this kind of error did not happen
 * \return true, the error happened
 */
bool FontConfig_OHOS::hasError(int err, const SkString& text) const {
  for (unsigned int i = 0; i < errSet.size(); i++) {
    if (errSet[i].err == err && errSet[i].text == text) {
      return true;
    }
  }
  return false;
}

/*! To get the total count of errors happened
 * \return The count of errors
 */
int FontConfig_OHOS::getErrorCount() const {
  return errSet.size();
}

/*! To sort the typeface set
 * \param typefaceSet the typeface set to be sorted
 */
void FontConfig_OHOS::sortTypefaceSet(
    std::shared_ptr<TypefaceSet>& typefaceSet) {
  if (typefaceSet.get() == nullptr || typefaceSet->size() <= 1) {
    return;
  }
  TypefaceSet& tpSet = *(typefaceSet.get());
  for (unsigned int i = 0; i < tpSet.size(); i++) {
    for (unsigned int j = 0; j < tpSet.size() - 1; j++) {
      if ((tpSet[j]->fontStyle().weight() >
           tpSet[j + 1]->fontStyle().weight()) ||
          (tpSet[j]->fontStyle().weight() ==
               tpSet[j + 1]->fontStyle().weight() &&
           tpSet[j]->fontStyle().slant() > tpSet[j + 1]->fontStyle().slant())) {
        tpSet[j].swap(tpSet[j + 1]);
      }
    }
  }
}

/*! To get the display text of an error
 * \param err the id of an error
 * \return The text to explain the error
 */
const char* FontConfig_OHOS::errToString(int err) {
  switch (err) {
    case NO_ERROR: {
      return "successful";
    }
    case ERROR_CONFIG_NOT_FOUND: {
      return "config file is not found";
    }
    case ERROR_CONFIG_FORMAT_NOT_SUPPORTED: {
      return "the format of config file is not supported";
    }
    case ERROR_CONFIG_MISSING_TAG: {
      return "missing tag";
    }
    case ERROR_CONFIG_INVALID_VALUE_TYPE: {
      return "invalid value type";
    }
    case ERROR_FONT_NOT_EXIST: {
      return "font file is not exist";
    }
    case ERROR_FONT_NO_STREAM: {
      return "no font stream";
    }
    case ERROR_FONT_INVALID_STREAM: {
      return "invalid font stream";
    }
    case ERROR_FAMILY_NOT_FOUND: {
      return "family is not found";
    }
    case ERROR_NO_AVAILABLE_FAMILY: {
      return "no available family in the system";
    }
    case ERROR_DIR_NOT_FOUND: {
      return "no such directory";
    }
    default: {
      return "unknown error";
    }
  }
  return "unknown error";
}

/*! To log the error information
 * \param err the id of an error
 * \param key the key which indicates the the part with the error
 * \param expected the expected type of json node.
 * \n     It's used only for err 'ERROR_CONFIG_INVALID_VALUE_TYPE'
 * \param actual the actual type of json node.
 * \n     It's used only for err 'ERROR_CONFIG_INVALID_VALUE_TYPE'
 * \return err
 */
int FontConfig_OHOS::logErrInfo(int err,
                                const char* key,
                                Json::ValueType expected,
                                Json::ValueType actual) {
  errSet.emplace_back(err, key);
  if (err != ERROR_CONFIG_INVALID_VALUE_TYPE) {
    LOGE("%s : %s\n", errToString(err), key);
  } else {
    const char* types[] = {
        "null", "int", "unit", "real", "string", "boolean", "array", "object",
    };
    int size = sizeof(types) / sizeof(char*);
    if ((expected >= 0 && expected < size) && (actual >= 0 && actual < size)) {
      LOGE("%s : '%s' should be '%s', but here it's '%s'\n", errToString(err),
           key, types[expected], types[actual]);
    } else {
      LOGE("%s : %s\n", errToString(err), key);
    }
  }
  return err;
}

#if BUILDFLAG(ARKWEB_THEME_FONT)
void FontConfig_OHOS::InvalidateThemeFont(const SkFontScanner& fontScanner,
                                          int fd) {
  sk_sp<SkData> data(SkData::MakeFromFD(fd));
  std::unique_ptr<SkStreamAsset> stream =
      (data ? std::make_unique<SkMemoryStream>(std::move(data)) : nullptr);

  FontInfo font;
  int count = 0;
  if (stream == nullptr || !fontScanner.scanFile(stream.get(), &count) ||
      !fontScanner.scanInstance(stream.get(), 0, 0, &font.familyName,
                                &font.style, &font.isFixedWidth, nullptr)) {
    themeFontTypeface.reset();
    LOGE("[themefont] InvalidateThemeFont failed, stream(%p) count(%d).\n",
         stream.get(), count);
    return;
  }

  font.stream = std::move(stream);
  themeFontTypeface = sk_make_sp<SkTypeface_OHOS>(SkString(), font);
}

SkTypeface_OHOS* FontConfig_OHOS::getThemeFontTypeface() const {
  return themeFontTypeface.get();
}
#endif
