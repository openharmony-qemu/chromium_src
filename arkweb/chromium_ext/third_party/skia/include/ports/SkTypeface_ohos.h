// Copyright (c) 2021 Huawei Device Co., Ltd. All rights reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SkTypeface_ohos_DEFINED
#define SkTypeface_ohos_DEFINED

#include "include/core/SkFontStyle.h"
#include "include/core/SkStream.h"
#include "arkweb/chromium_ext/third_party/skia/include/ports/SkFontInfo_ohos.h"
#include "src/ports/SkTypeface_FreeType.h"

/*!
 * \brief The implementation of SkTypeface for ohos platform
 */
class SkTypeface_OHOS final: public SkTypeface_FreeType {
public:
    SkTypeface_OHOS(const SkString& specifiedName, FontInfo& info);
    explicit SkTypeface_OHOS(FontInfo& info);
    ~SkTypeface_OHOS() override;
    const FontInfo* getFontInfo() const;
protected:
    std::unique_ptr<SkStreamAsset> onOpenStream(int* ttcIndex) const override;
    std::unique_ptr<SkFontData> onMakeFontData() const override;
    void onGetFontDescriptor(SkFontDescriptor* descriptor, bool* isLocal) const override;
    void onGetFamilyName(SkString* familyName) const override;
    sk_sp<SkTypeface> onMakeClone(const SkFontArguments& args) const override;
private:
    SkString specifiedName; // specified family name which is defined in the configuration file
    std::unique_ptr<FontInfo> fontInfo; // the font information of this typeface
};

#endif /* SkTypeface_ohos_DEFINED */
