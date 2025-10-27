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
#define protected public
#define private public
#include "SkTypeface_ohos.h"
#undef protected
#undef private
#include <gtest/gtest.h>
#include <memory>
#include <fstream>

namespace skia {

class SkTypeface_OHOSTest : public testing::Test {
protected:
    void SetUp() {
        tempFontPath = "/system/fonts/NotoSansYi-Regular.ttf";
        std::ofstream tempFile(tempFontPath, std::ios::binary);
        tempFile.write("\0\1\0\0", 4);
        tempFile.close();

        baseInfo.fname = SkString(tempFontPath);
        baseInfo.familyName = "TestFont";
        baseInfo.style = SkFontStyle::Normal();
        baseInfo.isFixedWidth = false;
        baseInfo.index = 0;
    }

    void TearDown() override {
        remove(tempFontPath.c_str());
    }

    std::string tempFontPath;
    FontInfo baseInfo;
};

TEST_F(SkTypeface_OHOSTest, Constructor_WithFamilyName) {
    SkString familyName("CustomFont");
    FontInfo info(baseInfo);
    
    SkTypeface_OHOS typeface(familyName, info);

    SkString retrievedName;
    typeface.onGetFamilyName(&retrievedName);
    EXPECT_STREQ(retrievedName.c_str(), "CustomFont");
}

TEST_F(SkTypeface_OHOSTest, Constructor_WithoutFamilyName) {
    FontInfo info(baseInfo);
    SkTypeface_OHOS typeface(info);

    SkString retrievedName;
    typeface.onGetFamilyName(&retrievedName);
    EXPECT_STREQ(retrievedName.c_str(), "TestFont");
}

TEST_F(SkTypeface_OHOSTest, OnOpenStream_ValidFont) {
    FontInfo info(baseInfo);
    SkTypeface_OHOS typeface(info);
    
    int ttcIndex = -1;
    auto stream = typeface.onOpenStream(&ttcIndex);
    
    EXPECT_NE(stream, nullptr);
    EXPECT_EQ(ttcIndex, 0);
}

TEST_F(SkTypeface_OHOSTest, OnOpenStream_InvalidFont) {
    FontInfo info(baseInfo);
    info.fname = "./nonexistent_font.ttf";
    SkTypeface_OHOS typeface(info);
    
    auto stream = typeface.onOpenStream(nullptr);
    EXPECT_EQ(stream, nullptr);
}

TEST_F(SkTypeface_OHOSTest, OnOpenStream_NullTtcIndex) {
    FontInfo info(baseInfo);
    SkTypeface_OHOS typeface(info);
    
    auto stream = typeface.onOpenStream(nullptr);
    EXPECT_NE(stream, nullptr);
}

TEST_F(SkTypeface_OHOSTest, OnMakeFontData_ValidFont) {
    FontInfo info(baseInfo);
    SkTypeface_OHOS typeface(info);
    
    auto fontData = typeface.onMakeFontData();
    EXPECT_NE(fontData, nullptr);

    EXPECT_EQ(fontData->getIndex(), 0);
    EXPECT_EQ(fontData->getAxisCount(), 0);
}

TEST_F(SkTypeface_OHOSTest, OnMakeFontData_InvalidFont) {
    FontInfo info(baseInfo);
    info.fname = "./nonexistent_font.ttf";
    SkTypeface_OHOS typeface(info);
    
    auto fontData = typeface.onMakeFontData();
    EXPECT_EQ(fontData, nullptr);
}

TEST_F(SkTypeface_OHOSTest, OnMakeFontData_Nullptr) {
    FontInfo info;
    SkTypeface_OHOS typeface(info);
    std::unique_ptr<SkFontData> fontData = typeface.onMakeFontData();
    EXPECT_EQ(nullptr, fontData);
}

TEST_F(SkTypeface_OHOSTest, OnGetFontDescriptor) {
    FontInfo info(baseInfo);
    SkTypeface_OHOS typeface(info);
    
    SkFontDescriptor descriptor;
    bool isLocal = true;
    typeface.onGetFontDescriptor(&descriptor, &isLocal);
    
    EXPECT_FALSE(isLocal);
    EXPECT_STREQ(descriptor.getFamilyName(), "TestFont");
    EXPECT_EQ(descriptor.getStyle(), SkFontStyle::Normal());
}

TEST_F(SkTypeface_OHOSTest, OnGetFontDescriptor_NullParams) {
    FontInfo info(baseInfo);
    SkTypeface_OHOS typeface(info);

    typeface.onGetFontDescriptor(nullptr, nullptr);
    typeface.onGetFontDescriptor(nullptr, nullptr);
}

TEST_F(SkTypeface_OHOSTest, OnGetFamilyName_WithSpecifiedName) {
    SkString familyName("CustomFont");
    FontInfo info(baseInfo);
    SkTypeface_OHOS typeface(familyName, info);
    
    SkString retrievedName;
    typeface.onGetFamilyName(&retrievedName);
    EXPECT_STREQ(retrievedName.c_str(), "CustomFont");
}

TEST_F(SkTypeface_OHOSTest, OnGetFamilyName_WithoutSpecifiedName) {
    FontInfo info(baseInfo);
    SkTypeface_OHOS typeface(info);
    
    SkString retrievedName;
    typeface.onGetFamilyName(&retrievedName);
    EXPECT_STREQ(retrievedName.c_str(), "TestFont");
}

TEST_F(SkTypeface_OHOSTest, OnGetFamilyName_NullOutput) {
    FontInfo info(baseInfo);
    SkTypeface_OHOS typeface(info);

    typeface.onGetFamilyName(nullptr);
}

TEST_F(SkTypeface_OHOSTest, OnMakeClone_SimpleClone) {
    FontInfo info(baseInfo);
    SkTypeface_OHOS typeface(info);
    
    SkFontArguments args;
    auto cloned = typeface.onMakeClone(args);
    
    EXPECT_NE(cloned, nullptr);
    EXPECT_EQ(cloned.get(), &typeface);
}

TEST_F(SkTypeface_OHOSTest, OnMakeClone_WithVariation) {
    FontInfo info(baseInfo);
    SkTypeface_OHOS typeface(info);
    
    SkFontArguments args;
    args.setCollectionIndex(0);
    
    SkFontArguments::VariationPosition position;
    SkFontArguments::VariationPosition::Coordinate axes[] = {
      {SkSetFourByteTag('w','g','h','t'), 1.0f}
    };
    position.coordinates = axes;
    position.coordinateCount = 1;
    args.setVariationDesignPosition(position);
    
    auto cloned = typeface.onMakeClone(args);

    if (cloned) {
        EXPECT_NE(cloned.get(), &typeface);
    }
}

TEST_F(SkTypeface_OHOSTest, OnMakeClone_InvalidFont) {
    FontInfo info(baseInfo);
    info.fname = "./nonexistent_font.ttf";
    SkTypeface_OHOS typeface(info);
    
    SkFontArguments args;
    auto cloned = typeface.onMakeClone(args);
    
    EXPECT_NE(cloned, nullptr);
}

TEST_F(SkTypeface_OHOSTest, GetFontInfo) {
    FontInfo info(baseInfo);
    SkTypeface_OHOS typeface(info);
    
    const FontInfo* retrievedInfo = typeface.getFontInfo();
    EXPECT_NE(retrievedInfo, nullptr);
    EXPECT_STREQ(retrievedInfo->fname.c_str(), tempFontPath.c_str());
}

TEST_F(SkTypeface_OHOSTest, onOpenStream_NullFontInfo) {
  FontInfo info;
  info.familyName = "TestFont";
  info.fname = "test.ttf";
  info.style =
      SkFontStyle(SkFontStyle::kNormal_Weight, SkFontStyle::kNormal_Width,
                  SkFontStyle::kUpright_Slant);
  auto typeface = sk_make_sp<SkTypeface_OHOS>(info);
  typeface->fontInfo = nullptr;
  int ttcIndex = 0;
  auto result = typeface->onOpenStream(&ttcIndex);
  EXPECT_EQ(result, nullptr);
}

TEST_F(SkTypeface_OHOSTest, onMakeFontData_NullFontInfo) {
  FontInfo info;
  info.familyName = "TestFont";
  info.fname = "test.ttf";
  info.style =
      SkFontStyle(SkFontStyle::kNormal_Weight, SkFontStyle::kNormal_Width,
                  SkFontStyle::kUpright_Slant);
  auto typeface = sk_make_sp<SkTypeface_OHOS>(info);
  typeface->fontInfo = nullptr;
  auto result = typeface->onMakeFontData();
  EXPECT_EQ(result, nullptr);
}

TEST_F(SkTypeface_OHOSTest, onGetFamilyName_NullFontInfo) {
  FontInfo info;
  info.familyName = "TestFont";
  info.fname = "test.ttf";
  info.style =
      SkFontStyle(SkFontStyle::kNormal_Weight, SkFontStyle::kNormal_Width,
                  SkFontStyle::kUpright_Slant);
  auto typeface = sk_make_sp<SkTypeface_OHOS>(info);
  typeface->specifiedName = "";
  typeface->fontInfo = nullptr;
  SkString result;
  typeface->onGetFamilyName(&result);
  EXPECT_EQ(result.size(), 0);
}
} // namespace skia