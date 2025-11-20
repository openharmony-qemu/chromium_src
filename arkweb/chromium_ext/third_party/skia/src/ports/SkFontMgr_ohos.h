/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 * 2021.2.10 SkFontMgr on ohos.
 *           Copyright (c) 2021 Huawei Device Co., Ltd. All rights reserved.
 */

#ifndef SKFONTMGR_OHOS_H
#define SKFONTMGR_OHOS_H

#include "FontConfig_ohos.h"
#include "SkFontStyleSet_ohos.h"
#include "arkweb/build/features/features.h"
#include "include/core/SkFontMgr.h"
#include "src/core/SkFontDescriptor.h"
#include "src/ports/SkFontScanner_FreeType_priv.h"

/*!
 * \brief To implement the SkFontMgr for ohos platform
 */
class SkFontMgr_OHOS : public SkFontMgr {
 public:
  explicit SkFontMgr_OHOS(const char* path = nullptr);
  virtual ~SkFontMgr_OHOS() override = default;

 protected:
  virtual int onCountFamilies() const override;
  virtual void onGetFamilyName(int index, SkString* familyName) const override;
  virtual sk_sp<SkFontStyleSet> onCreateStyleSet(int index) const override;

  virtual sk_sp<SkFontStyleSet> onMatchFamily(
      const char familyName[]) const override;

  virtual sk_sp<SkTypeface> onMatchFamilyStyle(
      const char familyName[],
      const SkFontStyle& style) const override;
  virtual sk_sp<SkTypeface> onMatchFamilyStyleCharacter(
      const char familyName[],
      const SkFontStyle& style,
      const char* bcp47[],
      int bcp47Count,
      SkUnichar character) const override;

  virtual sk_sp<SkTypeface> onMakeFromData(sk_sp<SkData> data,
                                           int ttcIndex) const override;
  virtual sk_sp<SkTypeface> onMakeFromStreamIndex(
      std::unique_ptr<SkStreamAsset> stream,
      int ttcIndex) const override;
  virtual sk_sp<SkTypeface> onMakeFromStreamArgs(
      std::unique_ptr<SkStreamAsset> stream,
      const SkFontArguments& args) const override;
  virtual sk_sp<SkTypeface> onMakeFromFile(const char path[],
                                           int ttcIndex) const override;

  virtual sk_sp<SkTypeface> onLegacyMakeTypeface(
      const char familyName[],
      SkFontStyle style) const override;

#if BUILDFLAG(ARKWEB_THEME_FONT)
  virtual void onInvalidateThemeFont(const std::vector<int>& fds) override;
#endif

 private:
  std::shared_ptr<FontConfig_OHOS> fontConfig =
      nullptr;                         // the pointer of FontConfig_OHOS
  SkFontScanner_FreeType fontScanner;  // the scanner to parse a font file
  int familyCount = 0;  // the count of font style sets in generic family list

  int compareLangs(const SkString& langs,
                   const char* bcp47[],
                   int bcp47Count,
                   const int tps[]) const;
  sk_sp<SkTypeface> makeTypeface(std::unique_ptr<SkStreamAsset> stream,
                                 const SkFontArguments& args,
                                 const char path[]) const;
  sk_sp<SkTypeface> makeTypeface(SkFontData* fontData) const;
  sk_sp<SkTypeface> findTypeface(const FallbackSetPos& fallbackItem,
                           const SkFontStyle& style,
                           const char* bcp47[],
                           int bcp47Count,
                           SkUnichar character) const;
};

SK_API sk_sp<SkFontMgr> SkFontMgr_New_OHOS(const char* path);

#endif /* SKFONTMGR_OHOS_H */
