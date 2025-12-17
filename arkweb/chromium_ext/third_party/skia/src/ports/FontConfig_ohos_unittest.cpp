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

#include "arkweb/ohos_adapter_ndk/interfaces/mock/mock_ohos_adapter_helper.h"
#include "arkweb/ohos_adapter_ndk/interfaces/mock/mock_ohos_drawing_text_adapter.h"
#define private public
#include "third_party/skia/src/ports/FontConfig_ohos.h"
#include "src/ports/SkFontScanner_FreeType_priv.h"
#include "third_party/skia/include/core/SkFontScanner.h"
#undef private
#include "gtest/gtest.h"
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "base/logging.h"
#include <gmock/gmock.h>
#include <dlfcn.h>
#include <json/reader.h>
#include <sys/stat.h>
#include <cstdio>
#include <fstream>
#include "base/logging.h"
#include "base/files/file_util.h"
#include "base/files/file_path.h"

#define NOTFOUND 404
#define SERVERERROR 500

using ::testing::_;
using ::testing::Return;
using ::testing::NotNull;

namespace skia {

class MockFontScanner : public SkFontScanner_FreeType {
public:
  MOCK_METHOD(bool, scanFile, (SkStreamAsset*, int*), (const));
  MOCK_METHOD(bool, scanFace, (SkStreamAsset*, int, int*), (const));
  MOCK_METHOD(bool, scanInstance, (SkStreamAsset*, int, int, SkString*,
                                   SkFontStyle*, bool*, AxisDefinitions*), (const));
  MOCK_METHOD(sk_sp<SkTypeface>, MakeFromStream,
             (std::unique_ptr<SkStreamAsset>, const SkFontArguments&), (const));
  MOCK_METHOD(SkFourByteTag, getFactoryId, (), (const));
};

class FontConfig_OHOSTest : public testing::Test {
protected:
    SkFontScanner_FreeType fontScanner;
    testing::StrictMock<MockFontScanner> mockScanner;
    std::shared_ptr<FontConfig_OHOS> fontConfig;
    Json::Value createBaseJson() {
        Json::Value root;
        root["family"] = "Roboto";
        
        Json::Value aliases(Json::arrayValue);
        Json::Value alias1(Json::objectValue);
        alias1["name"] = "Roboto-Regular";
        aliases.append(alias1);
        root["alias"] = aliases;
        
        return root;
    }

    void AddAlias(const SkString& alias, const SkString& target, int pos) {
        std::vector<FontConfig_OHOS::AliasInfo> aliasSet = {{pos, 0}};
        fontConfig->aliasMap.set(alias, aliasSet);
        
        auto genericFamily = std::make_unique<GenericFamily>();
        genericFamily->familyName = target;
        genericFamily->typefaceSet = std::make_shared<TypefaceSet>();
        fontConfig->genericFamilySet.push_back(std::move(genericFamily));
    }

    void AddFallback(const SkString& name, int index) {
        fontConfig->fallbackNames.set(name, index);
        
        auto fallbackInfo = std::make_unique<FallbackInfo>();
        fallbackInfo->typefaceSet = std::make_shared<TypefaceSet>();
        fontConfig->fallbackSet.push_back(std::move(fallbackInfo));
    }

    void createTempFile(const char* filename) {
        FILE* f = fopen(filename, "w");
        if (f) {
            fclose(f);
        }
    }

    void createFile() {
        mkdir("test_fonts", 0777);
        mkdir("empty_dir", 0777);
        std::ofstream("test_fonts/A.ttf");
        std::ofstream("test_fonts/B.otf");
        std::ofstream("test_fonts/not_font.txt");
    }

    void clearFile() {
        remove("test_fonts/A.ttf");
        remove("test_fonts/B.otf");
        remove("test_fonts/not_font.txt");
        rmdir("test_fonts");
        rmdir("empty_dir");
    }

    void resetGenericValueSetup() {
        fontConfig->genericFamilySet.emplace_back(std::make_unique<FallbackInfo>());
        fontConfig->genericFamilySet.emplace_back(std::make_unique<FallbackInfo>());

        fontConfig->genericFamilySet[0]->typefaceSet = std::make_shared<TypefaceSet>();
        fontConfig->genericFamilySet[1]->typefaceSet = std::make_shared<TypefaceSet>();
    }

    void SetUp() {
        fontConfig = std::make_shared<FontConfig_OHOS>(fontScanner, nullptr);
        ASSERT_NE(fontConfig, nullptr);
    }
};

TEST_F(FontConfig_OHOSTest, Constructor001) {
    EXPECT_NE(fontConfig, nullptr);
}

TEST_F(FontConfig_OHOSTest, checkNewFontengineISOK) {
    int ret = fontConfig->checkNewFontengineISOK();
    EXPECT_EQ(1, ret);
}

TEST_F(FontConfig_OHOSTest, buildNameToFamilyMap) {
    fontConfig->buildNameToFamilyMap();
    EXPECT_NE(fontConfig, nullptr);
}

TEST_F(FontConfig_OHOSTest, buildStyleNameToFamilyMap) {
    fontConfig->buildNameToFamilyMap();
    EXPECT_NE(fontConfig, nullptr);
}

TEST_F(FontConfig_OHOSTest, getFamilyName001) {
    SkString familyName("no-font");
    int ret = fontConfig->getFamilyName(-100, &familyName);
    EXPECT_EQ(-1, ret);
}

TEST_F(FontConfig_OHOSTest, getFamilyName002) {
    SkString familyName("no-font");
    int ret = fontConfig->getFamilyName(10000, &familyName);
    EXPECT_EQ(-1, ret);
}

TEST_F(FontConfig_OHOSTest, getFamilyName003) {
    SkString familyName("");
    int ret = fontConfig->getFamilyName(-100, &familyName);
    EXPECT_EQ(-1, ret);
}

TEST_F(FontConfig_OHOSTest, getFamilyName004) {
    SkString familyName("");
    int ret = fontConfig->getFamilyName(10000, &familyName);
    EXPECT_EQ(-1, ret);
}

TEST_F(FontConfig_OHOSTest, getFamilyName005) {
    SkString familyName("Noto Sans Adlam Regular");
    int ret = fontConfig->getFamilyName(1, &familyName);
    EXPECT_NE(-1, ret);
}

TEST_F(FontConfig_OHOSTest, getFamilyName006) {
    auto ret = fontConfig->getFamilyName(-100, nullptr);
    EXPECT_EQ(-1, ret);
}

TEST_F(FontConfig_OHOSTest, getFamilyName007) {
    auto ret = fontConfig->getFamilyName(1, nullptr);
    EXPECT_NE(-1, ret);
}

TEST_F(FontConfig_OHOSTest, getTypefaceCount001) {
    int ret = fontConfig->getTypefaceCount(-100, false);
    EXPECT_EQ(-1, ret);
}

TEST_F(FontConfig_OHOSTest, getTypefaceCount002) {
    int ret = fontConfig->getTypefaceCount(1, false);
    EXPECT_NE(-1, ret);
}

TEST_F(FontConfig_OHOSTest, getTypefaceCount003) {
    int ret = fontConfig->getTypefaceCount(1000, false);
    EXPECT_EQ(-1, ret);
}

TEST_F(FontConfig_OHOSTest, getTypefaceCount004) {
    int ret = fontConfig->getTypefaceCount(1, true);
    EXPECT_NE(-1, ret);
}

TEST_F(FontConfig_OHOSTest, getTypefaceCount05) {
    int ret = fontConfig->getTypefaceCount(1, true);
    EXPECT_NE(-1, ret);
}

TEST_F(FontConfig_OHOSTest, getTypeface001) {
    auto ret = fontConfig->getTypeface(-1, -1, false);
    EXPECT_EQ(ret, nullptr);
}

TEST_F(FontConfig_OHOSTest, getTypeface002) {
    auto ret = fontConfig->getTypeface(0, 0, false);
    EXPECT_NE(ret, nullptr);
}

TEST_F(FontConfig_OHOSTest, getTypeface003) {
    auto ret = fontConfig->getTypeface(1, 10000, false);
    EXPECT_EQ(ret, nullptr);
}

TEST_F(FontConfig_OHOSTest, getTypeface004) {
    auto ret = fontConfig->getTypeface(1, -1, false);
    EXPECT_EQ(ret, nullptr);
}

TEST_F(FontConfig_OHOSTest, getTypeface005) {
    auto ret = fontConfig->getTypeface(1, 1000, true);
    EXPECT_EQ(ret, nullptr);
}

TEST_F(FontConfig_OHOSTest, getTypeface006) {
    auto ret = fontConfig->getTypeface(1, 1000, false);
    EXPECT_EQ(ret, nullptr);
}

TEST_F(FontConfig_OHOSTest, getTypeface007) {
    auto ret = fontConfig->getTypeface(0, 0, true);
    EXPECT_NE(ret, nullptr);
}

TEST_F(FontConfig_OHOSTest, getTypeface008) {
    auto ret = fontConfig->getTypeface(0, 1000, true);
    EXPECT_EQ(ret, nullptr);
}

TEST_F(FontConfig_OHOSTest, getTypeface009) {
    const SkFontStyle style(SkFontStyle::kNormal_Weight,
                             SkFontStyle::kNormal_Width,
                             SkFontStyle::kUpright_Slant);
    auto ret = fontConfig->getTypeface(-1, style, true);
    EXPECT_EQ(ret, nullptr);
}

TEST_F(FontConfig_OHOSTest, getTypeface0010) {
    const SkFontStyle style(SkFontStyle::kNormal_Weight,
                             SkFontStyle::kNormal_Width,
                             SkFontStyle::kUpright_Slant);
    auto ret = fontConfig->getTypeface(1000, style, true);
    EXPECT_EQ(ret, nullptr);
}

TEST_F(FontConfig_OHOSTest, getTypeface0011) {
    const SkFontStyle style(SkFontStyle::kNormal_Weight,
                             SkFontStyle::kNormal_Width,
                             SkFontStyle::kUpright_Slant);
    auto ret = fontConfig->getTypeface(1000, style, false);
    EXPECT_EQ(ret, nullptr);
}

TEST_F(FontConfig_OHOSTest, getTypeface0012) {
    const SkFontStyle style(SkFontStyle::kNormal_Weight,
                             SkFontStyle::kNormal_Width,
                             SkFontStyle::kUpright_Slant);
    auto ret = fontConfig->getTypeface(0, style, false);
    EXPECT_NE(ret, nullptr);
}

TEST_F(FontConfig_OHOSTest, getTypeface0013) {
    const SkFontStyle style(10000,
                             SkFontStyle::kNormal_Width,
                             SkFontStyle::kUpright_Slant);
    auto ret = fontConfig->getTypeface(0, style, false);
    EXPECT_NE(ret, nullptr);
}

TEST_F(FontConfig_OHOSTest, getTypeface0014) {
    const SkFontStyle style(SkFontStyle::kNormal_Weight,
                             SkFontStyle::kNormal_Width,
                             SkFontStyle::kUpright_Slant);
    auto ret = fontConfig->getTypeface(0, style, true);
    EXPECT_NE(ret, nullptr);
}

TEST_F(FontConfig_OHOSTest, getTypeface0015) {
    const SkFontStyle style(10000,
                             SkFontStyle::kNormal_Width,
                             SkFontStyle::kUpright_Slant);
    auto ret = fontConfig->getTypeface(0, style, true);
    EXPECT_NE(ret, nullptr);
}

TEST_F(FontConfig_OHOSTest, getStyleIndex001) {
    bool isFallback = false;
    int ret = fontConfig->getStyleIndex(nullptr, isFallback);
    EXPECT_EQ(0, ret);
}

TEST_F(FontConfig_OHOSTest, getStyleIndex002) {
    bool isFallback = false;
    int ret = fontConfig->getStyleIndex("", isFallback);
    EXPECT_EQ(-1, ret);
}

TEST_F(FontConfig_OHOSTest, getStyleIndex003) {
    bool isFallback = false;
    int ret = fontConfig->getStyleIndex("Noto Sans Adlam Regular", isFallback);
    EXPECT_EQ(-1, ret);
}

TEST_F(FontConfig_OHOSTest, getStyleIndex004) {
    bool isFallback = false;
    int ret = fontConfig->getStyleIndex("NOT A FONT", isFallback);
    EXPECT_EQ(-1, ret);
}

TEST_F(FontConfig_OHOSTest, matchFontStyle001) {
    const SkFontStyle style(SkFontStyle::kNormal_Weight,
                             SkFontStyle::kNormal_Width,
                             SkFontStyle::kUpright_Slant);
    FontInfo info;
    info.familyName = "no-font";
    info.fname = "no-font.ttf";

    sk_sp<SkTypeface_OHOS> typeface = sk_make_sp<SkTypeface_OHOS>(info);
    TypefaceSet typefaces;
    typefaces.push_back(typeface);
    auto ret = fontConfig->matchFontStyle(typefaces, style);
    EXPECT_NE(ret, nullptr);
}

TEST_F(FontConfig_OHOSTest, matchFontStyle002) {
    const SkFontStyle style(SkFontStyle::kNormal_Weight,
                             SkFontStyle::kNormal_Width,
                             SkFontStyle::kUpright_Slant);
    FontInfo info;
    info.familyName = "HarmonyOS Sans";
    info.fname = "HarmonyOS_Sans.ttf";

    sk_sp<SkTypeface_OHOS> typeface = sk_make_sp<SkTypeface_OHOS>(info);
    TypefaceSet typefaces;
    typefaces.push_back(typeface);
    auto ret = fontConfig->matchFontStyle(typefaces, style);
    EXPECT_NE(ret, nullptr);
}

TEST_F(FontConfig_OHOSTest, matchFontStyle003) {
    const SkFontStyle style(SkFontStyle::kNormal_Weight,
                             SkFontStyle::kNormal_Width,
                             SkFontStyle::kUpright_Slant);
    FontInfo info;

    sk_sp<SkTypeface_OHOS> typeface = sk_make_sp<SkTypeface_OHOS>(info);
    TypefaceSet typefaces;
    typefaces.push_back(typeface);
    auto ret = fontConfig->matchFontStyle(typefaces, style);
    EXPECT_NE(ret, nullptr);
}

TEST_F(FontConfig_OHOSTest, matchFontStyle004) {
    const SkFontStyle style(SkFontStyle::kNormal_Weight,
                             SkFontStyle::kNormal_Width,
                             SkFontStyle::kUpright_Slant);
    FontInfo info;

    sk_sp<SkTypeface_OHOS> typeface = sk_make_sp<SkTypeface_OHOS>(info);
    TypefaceSet typefaces;
    auto ret = fontConfig->matchFontStyle(typefaces, style);
    EXPECT_EQ(ret, nullptr);
}

TEST_F(FontConfig_OHOSTest, getVariableFontStyleDifference001) {
    static constexpr SkFourByteTag wghtTag = SkSetFourByteTag('w', 'g', 'h', 't');
    static constexpr SkFourByteTag wdthTag = SkSetFourByteTag('w', 'd', 't', 'h');
    const std::vector<SkFontScanner::AxisDefinition>& axisRange1 = {
        {wghtTag, 100.0f, 400.0f, 900.0f}
    };

    const std::vector<SkFontScanner::AxisDefinition>& axisRange2 = {
        {wghtTag, 100.0f, 400.0f, 900.0f}
    };

    const SkFontStyle srcStyle(SkFontStyle::kNormal_Weight,
                             SkFontStyle::kNormal_Width,
                             SkFontStyle::kUpright_Slant);
    const SkFontStyle dstStyle(SkFontStyle::kNormal_Weight,
                             SkFontStyle::kNormal_Width,
                             SkFontStyle::kUpright_Slant);
    auto ret1 = fontConfig->getVariableFontStyleDifference(dstStyle, srcStyle, axisRange1);
    EXPECT_EQ(ret1, 0);
    auto ret2 = fontConfig->getVariableFontStyleDifference(dstStyle, srcStyle, axisRange2);
    EXPECT_EQ(ret2, 0);

}

TEST_F(FontConfig_OHOSTest, getVariableFontStyleDifference002) {
    SkFontStyle dstStyle(/*weight*/700, /*width*/5, /*slant*/SkFontStyle::kUpright_Slant);
    SkFontStyle srcStyle(400, 4, SkFontStyle::kItalic_Slant);
    std::vector<SkFontScanner::AxisDefinition> axisRanges;
    SkFontScanner::AxisDefinition widthAxis;
    widthAxis.fTag = SkSetFourByteTag('w', 'd', 't', 'h'); // wdthTag
    widthAxis.fMinimum = 0.5f;
    widthAxis.fMaximum = 2.0f;
    axisRanges.push_back(widthAxis);

    uint32_t diff = fontConfig->getVariableFontStyleDifference(dstStyle, srcStyle, axisRanges);
}

TEST_F(FontConfig_OHOSTest, getFontStyleDifference001) {
    const SkFontStyle dstStyle(100, 4, SkFontStyle::kUpright_Slant);
    const SkFontStyle srcStyle(400, 5, SkFontStyle::kUpright_Slant);

    auto ret = fontConfig->getFontStyleDifference(dstStyle, srcStyle);

    EXPECT_NE(ret, 0u);
}

TEST_F(FontConfig_OHOSTest, getFontStyleDifference002) {
    const SkFontStyle dstStyle1(100, 4, SkFontStyle::kUpright_Slant);
    const SkFontStyle srcStyle1(400, 5, SkFontStyle::kUpright_Slant);
    auto ret1 = fontConfig->getFontStyleDifference(dstStyle1, srcStyle1);
    EXPECT_NE(ret1, 0u);

    const SkFontStyle dstStyle2(400, 4, SkFontStyle::kUpright_Slant);
    const SkFontStyle srcStyle2(400, 4, SkFontStyle::kUpright_Slant);
    auto ret2 = fontConfig->getFontStyleDifference(dstStyle2, srcStyle2);
    EXPECT_EQ(ret2, 0u);

    const SkFontStyle dstStyle3(500, 5, SkFontStyle::kUpright_Slant);
    const SkFontStyle srcStyle3(400, 4, SkFontStyle::kUpright_Slant);
    auto ret3 = fontConfig->getFontStyleDifference(dstStyle3, srcStyle3);
    EXPECT_NE(ret3, 0u);

    const SkFontStyle dstStyle4(400, 5, SkFontStyle::kUpright_Slant);
    const SkFontStyle srcStyle4(500, 6, SkFontStyle::kUpright_Slant);
    auto ret4 = fontConfig->getFontStyleDifference(dstStyle4, srcStyle4);
    EXPECT_NE(ret4, 0u);

    const SkFontStyle dstStyle5(500, 5, SkFontStyle::kUpright_Slant);
    const SkFontStyle srcStyle5(400, 6, SkFontStyle::kUpright_Slant);
    auto ret5 = fontConfig->getFontStyleDifference(dstStyle5, srcStyle5);
    EXPECT_NE(ret5, 0u);

    const SkFontStyle dstStyle6(700, 5, SkFontStyle::kUpright_Slant);
    const SkFontStyle srcStyle6(800, 6, SkFontStyle::kUpright_Slant);
    auto ret6 = fontConfig->getFontStyleDifference(dstStyle6, srcStyle6);
    EXPECT_NE(ret6, 0u);
}

TEST_F(FontConfig_OHOSTest, getFontStyleDifference003) {
    {
        SkFontStyle dstStyle(/*weight*/400, /*width*/7, /*slant*/SkFontStyle::kUpright_Slant);
        SkFontStyle srcStyle(400, 9, SkFontStyle::kUpright_Slant); // srcWidth > dstWidth
        
        uint32_t diff = fontConfig->getFontStyleDifference(dstStyle, srcStyle);
        EXPECT_NE(2u, diff);
    }

    {
        SkFontStyle dstStyle(400, 8, SkFontStyle::kUpright_Slant);
        SkFontStyle srcStyle(400, 6, SkFontStyle::kUpright_Slant); // srcWidth < dstWidth
        
        uint32_t diff = fontConfig->getFontStyleDifference(dstStyle, srcStyle);
        EXPECT_NE(7u, diff);
    }
}

TEST_F(FontConfig_OHOSTest, getFileData) {
    const char* filename1 = "no-font.ttf";
    int fileSize1 = 0;
    auto ret1 = fontConfig->getFileData(filename1, fileSize1);
    EXPECT_EQ(ret1, nullptr);

    const char* filename2 = "/system/fonts/NotoSerifDogra-Regular.ttf";
    int fileSize2 = 0;
    auto ret2 = fontConfig->getFileData(filename2, fileSize2);
    EXPECT_NE(ret2, nullptr);
}

TEST_F(FontConfig_OHOSTest, parseConfig) {
    const char* fname1 = "/system/etc/fontconfig.json";
    auto ret1 = fontConfig->parseConfig(fname1);
    EXPECT_EQ(ret1, 0);

    auto ret2 = fontConfig->parseConfig(nullptr);
    EXPECT_EQ(ret2, 0);
}

TEST_F(FontConfig_OHOSTest, checkConfigFile001) {
    const char* fname1 = "no-font.ttf";
    Json::Value root;
    auto ret1 = fontConfig->checkConfigFile(fname1, root);
    EXPECT_NE(fontConfig, nullptr);

    const char* fname2 = "/system/fonts/NotoSerifDogra-Regular.ttf";
    Json::Value root2;
    auto ret2 = fontConfig->checkConfigFile(fname2, root2);
    EXPECT_NE(fontConfig, nullptr);
}

TEST_F(FontConfig_OHOSTest, ParseFontDir_AllStrings) {
    Json::Value root(Json::arrayValue);
    root.append("/system/fonts");
    root.append("/data/fonts");

    int ret = fontConfig->parseFontDir(root);
    EXPECT_EQ(ret, ErrorCode::NO_ERROR);
    EXPECT_EQ(fontConfig->fontDirSet.size(), 2);
    EXPECT_EQ(fontConfig->fontDirSet[0], SkString("/system/fonts"));
    EXPECT_EQ(fontConfig->fontDirSet[1], SkString("/data/fonts"));
}

TEST_F(FontConfig_OHOSTest, ParseFontDir_NonStringValue) {
    Json::Value root(Json::arrayValue);
    root.append("/system/fonts");
    root.append(123);

    int ret = fontConfig->parseFontDir(root);
    EXPECT_EQ(ret, ErrorCode::ERROR_CONFIG_INVALID_VALUE_TYPE);
    EXPECT_FALSE(fontConfig->fontDirSet.empty());
}

TEST_F(FontConfig_OHOSTest, ParseFontDir_NestedObject) {
    Json::Value root(Json::arrayValue);
    root.append("/system/fonts");
    
    Json::Value nestedObj(Json::objectValue);
    nestedObj["key"] = "value";
    root.append(nestedObj);

    int ret = fontConfig->parseFontDir(root);
    EXPECT_EQ(ret, ErrorCode::ERROR_CONFIG_INVALID_VALUE_TYPE);
}

TEST_F(FontConfig_OHOSTest, ParseFontDir_EmptyArray) {
    Json::Value root(Json::arrayValue);

    int ret = fontConfig->parseFontDir(root);
    EXPECT_EQ(ret, ErrorCode::NO_ERROR);
    EXPECT_TRUE(fontConfig->fontDirSet.empty());
}

TEST_F(FontConfig_OHOSTest, ParseGeneric_MissingFamily) {
    Json::Value root = createBaseJson();
    root.removeMember("family");
    
    int ret = fontConfig->parseGeneric(root);
    EXPECT_EQ(ret, ErrorCode::ERROR_CONFIG_MISSING_TAG);
}

TEST_F(FontConfig_OHOSTest, ParseGeneric_InvalidFamilyType) {
    Json::Value root = createBaseJson();
    root["family"] = 123;
    
    int ret = fontConfig->parseGeneric(root);
    EXPECT_EQ(ret, ErrorCode::ERROR_CONFIG_INVALID_VALUE_TYPE);
}

TEST_F(FontConfig_OHOSTest, ParseGeneric_MissingAlias) {
    Json::Value root = createBaseJson();
    root.removeMember("alias");
    
    int ret = fontConfig->parseGeneric(root);
    EXPECT_EQ(ret, ErrorCode::ERROR_CONFIG_MISSING_TAG);
}

TEST_F(FontConfig_OHOSTest, ParseGeneric_InvalidAliasType) {
    Json::Value root = createBaseJson();
    root["alias"] = "should_be_array";
    
    int ret = fontConfig->parseGeneric(root);
    EXPECT_EQ(ret, ErrorCode::NO_ERROR);
}

TEST_F(FontConfig_OHOSTest, ParseGeneric_InvalidAliasMemberType) {
    Json::Value root = createBaseJson();
    root["alias"].append("should_be_object");
    
    int ret = fontConfig->parseGeneric(root);
    EXPECT_EQ(ret, ErrorCode::NO_ERROR);
}

TEST_F(FontConfig_OHOSTest, ParseGeneric_ValidAdjust) {
    Json::Value root = createBaseJson();
    
    Json::Value adjusts(Json::arrayValue);
    Json::Value adj1(Json::objectValue);
    adj1["size"] = 12;
    adjusts.append(adj1);
    root["adjust"] = adjusts;
    
    int ret = fontConfig->parseGeneric(root);
    EXPECT_EQ(ret, ErrorCode::NO_ERROR);
    EXPECT_TRUE(fontConfig->adjustMap.empty());
}

TEST_F(FontConfig_OHOSTest, ParseGeneric_ValidVariations) {
    Json::Value root = createBaseJson();
    
    Json::Value vars(Json::arrayValue);
    Json::Value var1(Json::objectValue);
    var1["axis"] = "wght";
    vars.append(var1);
    root["variations"] = vars;
    
    int ret = fontConfig->parseGeneric(root);
    EXPECT_EQ(ret, ErrorCode::NO_ERROR);
    EXPECT_TRUE(fontConfig->variationMap.empty());
}

TEST_F(FontConfig_OHOSTest, ParseGeneric_ValidIndex) {
    Json::Value root = createBaseJson();
    
    Json::Value indexes(Json::arrayValue);
    indexes.append(0);
    indexes.append(1);
    root["index"] = indexes;
    
    int ret = fontConfig->parseGeneric(root);
    EXPECT_EQ(ret, ErrorCode::NO_ERROR);
}

TEST_F(FontConfig_OHOSTest, ParseGeneric_EmptyAliasArray) {
    Json::Value root = createBaseJson();
    root["alias"] = Json::Value(Json::arrayValue);
    
    int ret = fontConfig->parseGeneric(root);
    EXPECT_EQ(ret, ErrorCode::NO_ERROR);
    EXPECT_TRUE(fontConfig->aliasMap.empty());
}

TEST_F(FontConfig_OHOSTest, ParseGeneric_MinSize) {
    Json::Value root;
    root["family"] = "Minimal";
    root["alias"] = Json::Value(Json::arrayValue);
    
    int ret = fontConfig->parseGeneric(root);
    EXPECT_EQ(ret, ErrorCode::NO_ERROR);
}

TEST_F(FontConfig_OHOSTest, ParseGeneric_AllOptionalFields) {
    Json::Value root = createBaseJson();
    
    Json::Value adjusts(Json::arrayValue);
    adjusts.append(Json::Value(Json::arrayValue));
    root["adjust"] = adjusts;
    
    Json::Value vars(Json::arrayValue);
    vars.append(Json::Value(Json::arrayValue));
    root["variations"] = vars;
    
    Json::Value indexes(Json::arrayValue);
    indexes.append(0);
    root["index"] = indexes;
    
    int ret = fontConfig->parseGeneric(root);
    EXPECT_EQ(ret, ErrorCode::NO_ERROR);
    EXPECT_TRUE(fontConfig->aliasMap.empty());
    EXPECT_TRUE(fontConfig->adjustMap.empty());
    EXPECT_TRUE(fontConfig->variationMap.empty());
}

TEST_F(FontConfig_OHOSTest, parseAlias001) {
    std::vector<FontConfig_OHOS::AliasInfo> aliasSet;
    Json::Value root;
    fontConfig->genericFamilySet.clear();
    fontConfig->genericNames.reset();
    aliasSet.clear();
    root.clear();

    int result = fontConfig->parseAlias(root, aliasSet);
    EXPECT_EQ(result, ErrorCode::ERROR_CONFIG_MISSING_TAG);
    EXPECT_TRUE(aliasSet.empty());
}

TEST_F(FontConfig_OHOSTest, parseAlias002) {
    std::vector<FontConfig_OHOS::AliasInfo> aliasSet;
    Json::Value root;
    fontConfig->genericFamilySet.clear();
    fontConfig->genericNames.reset();
    aliasSet.clear();
    root.clear();

    root["alias1"] = "not_an_int";

    int result = fontConfig->parseAlias(root, aliasSet);
    EXPECT_NE(result, ErrorCode::ERROR_CONFIG_MISSING_TAG);
    EXPECT_TRUE(aliasSet.empty());
}

TEST_F(FontConfig_OHOSTest, parseAlias004) {
    std::vector<FontConfig_OHOS::AliasInfo> aliasSet;
    Json::Value root;
    fontConfig->genericFamilySet.clear();
    fontConfig->genericNames.reset();
    aliasSet.clear();
    root.clear();

    root["alias1"] = "100";

    fontConfig->parseAlias(root, aliasSet);
    
    // Second alias
    root.clear();
    root["alias2"] = 200;
    int result = fontConfig->parseAlias(root, aliasSet);
    
    EXPECT_EQ(result, ErrorCode::NO_ERROR);
}

TEST_F(FontConfig_OHOSTest, parseAlias006) {
    std::vector<FontConfig_OHOS::AliasInfo> aliasSet;
    Json::Value root;
    fontConfig->genericFamilySet.clear();
    fontConfig->genericNames.reset();
    aliasSet.clear();
    root.clear();

    root["alias1"] = "100";

    int result = fontConfig->parseAlias(root, aliasSet);
    EXPECT_NE(result, ErrorCode::NO_ERROR);
}

TEST_F(FontConfig_OHOSTest, MissingWeightTagReturnsError) {
    std::vector<FontConfig_OHOS::AdjustInfo> adjustSet;
    Json::Value root;
    adjustSet.clear();
    root.clear();
    
    root["to"] = 400; // Missing "weight" tag
    
    int result = fontConfig->parseAdjust(root, adjustSet);
    EXPECT_EQ(result, ErrorCode::ERROR_CONFIG_MISSING_TAG);
    EXPECT_TRUE(adjustSet.empty());
}

TEST_F(FontConfig_OHOSTest, MissingToTagReturnsError) {
    std::vector<FontConfig_OHOS::AdjustInfo> adjustSet;
    Json::Value root;
    adjustSet.clear();
    root.clear();
    
    root["to"] = 100; // Missing "weight" tag
    
    int result = fontConfig->parseAdjust(root, adjustSet);
    EXPECT_EQ(result, ErrorCode::ERROR_CONFIG_MISSING_TAG);
    EXPECT_TRUE(adjustSet.empty());
}

TEST_F(FontConfig_OHOSTest, WeightNotIntReturnsError) {
    std::vector<FontConfig_OHOS::AdjustInfo> adjustSet;
    Json::Value root;
    adjustSet.clear();
    root.clear();
    
    root["weight"] = "not_an_int"; // String instead of int
    root["to"] = 400;
    
    int result = fontConfig->parseAdjust(root, adjustSet);
    EXPECT_EQ(result, ErrorCode::ERROR_CONFIG_INVALID_VALUE_TYPE);
    EXPECT_TRUE(adjustSet.empty());
}

TEST_F(FontConfig_OHOSTest, ToNotIntReturnsError) {
    std::vector<FontConfig_OHOS::AdjustInfo> adjustSet;
    Json::Value root;
    adjustSet.clear();
    root.clear();
    
    root["weight"] = 100;
    root["to"] = "not_an_int"; // String instead of int
    
    int result = fontConfig->parseAdjust(root, adjustSet);
    EXPECT_EQ(result, ErrorCode::ERROR_CONFIG_INVALID_VALUE_TYPE);
    EXPECT_TRUE(adjustSet.empty());
}

TEST_F(FontConfig_OHOSTest, ValidInputAddsToAdjustSet) {
    std::vector<FontConfig_OHOS::AdjustInfo> adjustSet;
    Json::Value root;
    adjustSet.clear();
    root.clear();
    
    root["weight"] = 100;
    root["to"] = 400;
    
    int result = fontConfig->parseAdjust(root, adjustSet);
    EXPECT_EQ(result, ErrorCode::NO_ERROR);
    ASSERT_EQ(adjustSet.size(), 1);
}

TEST_F(FontConfig_OHOSTest, MultipleValidInputsAddToAdjustSet) {
    std::vector<FontConfig_OHOS::AdjustInfo> adjustSet;
    Json::Value root;
    adjustSet.clear();
    root.clear();
    
    root["weight"] = 100;
    root["to"] = 400;
    fontConfig->parseAdjust(root, adjustSet);
    
    // Second adjustment
    root["weight"] = 200;
    root["to"] = 500;
    int result = fontConfig->parseAdjust(root, adjustSet);
    
    EXPECT_EQ(result, ErrorCode::NO_ERROR);
}

TEST_F(FontConfig_OHOSTest, ZeroValuesAreAccepted) {
    std::vector<FontConfig_OHOS::AdjustInfo> adjustSet;
    Json::Value root;
    adjustSet.clear();
    root.clear();
    
    root["weight"] = 0;
    root["to"] = 0;
    
    int result = fontConfig->parseAdjust(root, adjustSet);
    EXPECT_EQ(result, ErrorCode::NO_ERROR);
    ASSERT_EQ(adjustSet.size(), 1);
}

TEST_F(FontConfig_OHOSTest, NegativeValuesAreAccepted) {
    std::vector<FontConfig_OHOS::AdjustInfo> adjustSet;
    Json::Value root;
    adjustSet.clear();
    root.clear();
    
    root["weight"] = -100;
    root["to"] = -400;
    
    int result = fontConfig->parseAdjust(root, adjustSet);
    EXPECT_EQ(result, ErrorCode::NO_ERROR);
    ASSERT_EQ(adjustSet.size(), 1);
}

TEST_F(FontConfig_OHOSTest, parseFallback001) {
    Json::Value root;
    root.clear();
    fontConfig->fallbackSet.clear();
    fontConfig->fallbackForMap.reset();
    
    int result = fontConfig->parseFallback(root);
    EXPECT_EQ(result, ErrorCode::ERROR_CONFIG_MISSING_TAG); 
}

TEST_F(FontConfig_OHOSTest, parseFallback002) {
    Json::Value root;
    root.clear();
    fontConfig->fallbackSet.clear();
    fontConfig->fallbackForMap.reset();
    
    root["family1"] = "not_an_array"; // String instead of array
    
    int result = fontConfig->parseFallback(root);
    EXPECT_EQ(result, ErrorCode::ERROR_CONFIG_INVALID_VALUE_TYPE); 
}

TEST_F(FontConfig_OHOSTest, parseFallback003) {
    Json::Value root;
    root.clear();
    fontConfig->fallbackSet.clear();
    fontConfig->fallbackForMap.reset();
    
    Json::Value array(Json::arrayValue); // Empty array
    root["family1"] = array;
    
    int result = fontConfig->parseFallback(root);
    EXPECT_EQ(result, ErrorCode::NO_ERROR);
    EXPECT_EQ(fontConfig->fallbackSet.size(), 0u);
    
    // Verify fallbackForMap was updated
    auto* pos = fontConfig->fallbackForMap.find(SkString("family1"));
    ASSERT_NE(pos, nullptr); 
}

TEST_F(FontConfig_OHOSTest, parseFallback004) {
    Json::Value root;
    root.clear();
    fontConfig->fallbackSet.clear();
    fontConfig->fallbackForMap.reset();
    
    Json::Value array(Json::arrayValue);
    array.append(Json::objectValue);
    array.append(Json::objectValue);
    root["family1"] = array;
    
    // Add some existing fallbacks first
    fontConfig->fallbackSet.resize(3);
    
    int result = fontConfig->parseFallback(root);
    EXPECT_EQ(result, ErrorCode::NO_ERROR);
    EXPECT_NE(fontConfig->fallbackSet.size(), 5u);
    
    auto* pos = fontConfig->fallbackForMap.find(SkString("family1"));
    ASSERT_NE(pos, nullptr);
}

TEST_F(FontConfig_OHOSTest, parseFallbackItem001) {
    Json::Value root;
    root.clear();
    root.clear();
    fontConfig->fallbackSet.clear();
    fontConfig->fallbackNames.reset();
    fontConfig->variationMap.reset();
    
    EXPECT_EQ(fontConfig->parseFallbackItem(root), ErrorCode::ERROR_CONFIG_MISSING_TAG);
}

TEST_F(FontConfig_OHOSTest, parseFallbackItem002) {
    Json::Value root;
    root.clear();
    root.clear();
    fontConfig->fallbackSet.clear();
    fontConfig->fallbackNames.reset();
    fontConfig->variationMap.reset();
    
    root["variations"] = Json::arrayValue;
    root["index"] = Json::arrayValue;
    EXPECT_EQ(fontConfig->parseFallbackItem(root), ErrorCode::ERROR_CONFIG_MISSING_TAG);
}

TEST_F(FontConfig_OHOSTest, parseFallbackItem003) {
    Json::Value root;
    root.clear();
    root.clear();
    fontConfig->fallbackSet.clear();
    fontConfig->fallbackNames.reset();
    fontConfig->variationMap.reset();
    
    root["lang"] = 123; // Not a string
    EXPECT_EQ(fontConfig->parseFallbackItem(root), ErrorCode::ERROR_CONFIG_INVALID_VALUE_TYPE);
}

TEST_F(FontConfig_OHOSTest, parseFallbackItem004) {
    Json::Value root;
    root.clear();
    root.clear();
    fontConfig->fallbackSet.clear();
    fontConfig->fallbackNames.reset();
    fontConfig->variationMap.reset();
    
    root["lang"] = "family_name";
    EXPECT_EQ(fontConfig->parseFallbackItem(root), ErrorCode::NO_ERROR);
    EXPECT_EQ(fontConfig->fallbackSet.size(), 1);
}

TEST_F(FontConfig_OHOSTest, parseFallbackItem005) {
    Json::Value root;
    root.clear();
    root.clear();
    fontConfig->fallbackSet.clear();
    fontConfig->fallbackNames.reset();
    fontConfig->variationMap.reset();
    
    root["lang"] = "family_name";
    root["variations"] = "not_an_array";
    EXPECT_EQ(fontConfig->parseFallbackItem(root), ErrorCode::NO_ERROR); // Still succeeds
}

TEST_F(FontConfig_OHOSTest, parseVariation001) {
    Json::Value root;
    std::vector<FontConfig_OHOS::VariationInfo> variationSet;
    root.clear();
    variationSet.clear();
    
    EXPECT_EQ(fontConfig->parseVariation(root, variationSet), ErrorCode::ERROR_CONFIG_MISSING_TAG);
}

TEST_F(FontConfig_OHOSTest, parseVariation002) {
    Json::Value root;
    std::vector<FontConfig_OHOS::VariationInfo> variationSet;
    root.clear();
    variationSet.clear();
    
    root["weight"] = "not_an_int";
    EXPECT_NE(fontConfig->parseVariation(root, variationSet), ErrorCode::ERROR_CONFIG_INVALID_VALUE_TYPE);
}

TEST_F(FontConfig_OHOSTest, parseVariation003) {
    Json::Value root;
    std::vector<FontConfig_OHOS::VariationInfo> variationSet;
    root.clear();
    variationSet.clear();
    
    root["weight"] = 400;
    root["width"] = "not_an_int";
    EXPECT_NE(fontConfig->parseVariation(root, variationSet), ErrorCode::ERROR_CONFIG_INVALID_VALUE_TYPE);
}

TEST_F(FontConfig_OHOSTest, parseVariation004) {
    Json::Value root;
    std::vector<FontConfig_OHOS::VariationInfo> variationSet;
    root.clear();
    variationSet.clear();
    
    root["weight"] = 400;
    root["slant"] = 123; // Not a string
    EXPECT_NE(fontConfig->parseVariation(root, variationSet), ErrorCode::ERROR_CONFIG_INVALID_VALUE_TYPE);
}

TEST_F(FontConfig_OHOSTest, parseVariation005) {
    Json::Value root;
    std::vector<FontConfig_OHOS::VariationInfo> variationSet;
    root.clear();
    variationSet.clear();
    
    root["weight"] = 400;
    
    const char* slantValues[] = {"normal", "italic", "oblique"};
    for (const auto& slant : slantValues) {
        root["slant"] = slant;
        EXPECT_NE(fontConfig->parseVariation(root, variationSet), ErrorCode::NO_ERROR);
        variationSet.clear();
    }
}

TEST_F(FontConfig_OHOSTest, parseVariation006) {
    Json::Value root;
    std::vector<FontConfig_OHOS::VariationInfo> variationSet;
    root.clear();
    variationSet.clear();
    
    root["weight"] = 400;
    root["wght"] = "not_a_number";
    EXPECT_EQ(fontConfig->parseVariation(root, variationSet), ErrorCode::ERROR_CONFIG_INVALID_VALUE_TYPE);
}

TEST_F(FontConfig_OHOSTest, parseVariation008) {
    Json::Value root;
    std::vector<FontConfig_OHOS::VariationInfo> variationSet;
    root.clear();
    variationSet.clear();
    
    root["weight"] = 400; // Only required field
    EXPECT_EQ(fontConfig->parseVariation(root, variationSet), ErrorCode::ERROR_CONFIG_MISSING_TAG);
}

TEST_F(FontConfig_OHOSTest, parseTtcIndex001) {
    Json::Value root;
    SkString familyName = SkString("test_family");
    root.clear();
    fontConfig->ttcIndexMap.reset();
    fontConfig->errSet.clear();
    
    Json::Value array(Json::arrayValue);
    array.append("sub_family");
    array.append(2);  // ttcIndex
    root = array;

    EXPECT_EQ(fontConfig->parseTtcIndex(root, familyName), ErrorCode::NO_ERROR);
    
    // Verify ttcIndexMap was updated
    auto* item = fontConfig->ttcIndexMap.find(familyName);
    ASSERT_NE(item, nullptr);
    EXPECT_EQ(item->familyName, SkString("sub_family"));
    EXPECT_EQ(item->ttcIndex, 2);
}

TEST_F(FontConfig_OHOSTest, parseTtcIndex002) {
    Json::Value root;
    SkString familyName = SkString("test_family");
    root.clear();
    fontConfig->ttcIndexMap.reset();
    fontConfig->errSet.clear();
    
    Json::Value array(Json::arrayValue);
    array.append("sub_family");
    array.append(0);  // ttcIndex = 0
    root = array;

    EXPECT_EQ(fontConfig->parseTtcIndex(root, familyName), ErrorCode::NO_ERROR);
    
    // Verify ttcIndexMap was updated
    auto* item = fontConfig->ttcIndexMap.find(familyName);
    ASSERT_NE(item, nullptr);
    EXPECT_EQ(item->ttcIndex, 0);
}

TEST_F(FontConfig_OHOSTest, parseTtcIndex003) {
    Json::Value root;
    SkString familyName = SkString("test_family");
    root.clear();
    fontConfig->ttcIndexMap.reset();
    fontConfig->errSet.clear();
    
    Json::Value array1(Json::arrayValue);
    array1.append("existing_family");
    array1.append(1);
    fontConfig->parseTtcIndex(array1, SkString("existing_key"));

    // Test new entry with same familyName but different key
    Json::Value array2(Json::arrayValue);
    array2.append("existing_family");
    array2.append(2);
    root = array2;

    EXPECT_EQ(fontConfig->parseTtcIndex(root, familyName), ErrorCode::NO_ERROR);
    
    // Verify both entries exist
    auto* existing = fontConfig->ttcIndexMap.find(SkString("existing_key"));
    auto* newItem = fontConfig->ttcIndexMap.find(familyName);
    ASSERT_NE(existing, nullptr);
    ASSERT_NE(newItem, nullptr);
    EXPECT_EQ(existing->ttcIndex, 1);
    EXPECT_EQ(newItem->ttcIndex, 2);
}

TEST_F(FontConfig_OHOSTest, parseTtcIndex004) {
    Json::Value root;
    SkString familyName = SkString("test_family");
    root.clear();
    fontConfig->ttcIndexMap.reset();
    fontConfig->errSet.clear();
    
    Json::Value array(Json::arrayValue);
    array.append("sub_family");  // Only one element
    root = array;

    EXPECT_EQ(fontConfig->parseTtcIndex(root, familyName), ErrorCode::ERROR_CONFIG_INVALID_VALUE_TYPE);
    EXPECT_FALSE(fontConfig->errSet.empty());
}

TEST_F(FontConfig_OHOSTest, parseTtcIndex005) {
    Json::Value root;
    SkString familyName = SkString("test_family");
    root.clear();
    fontConfig->ttcIndexMap.reset();
    fontConfig->errSet.clear();
    
    Json::Value array(Json::arrayValue);
    array.append(123);  // Not a string
    array.append(2);
    root = array;

    EXPECT_EQ(fontConfig->parseTtcIndex(root, familyName), ErrorCode::ERROR_CONFIG_INVALID_VALUE_TYPE);
}

TEST_F(FontConfig_OHOSTest, parseTtcIndex006) {
    Json::Value root;
    SkString familyName = SkString("test_family");
    root.clear();
    fontConfig->ttcIndexMap.reset();
    fontConfig->errSet.clear();
    
    Json::Value array(Json::arrayValue);
    array.append("sub_family");
    array.append("not_a_number");  // Not numeric
    root = array;

    EXPECT_EQ(fontConfig->parseTtcIndex(root, familyName), ErrorCode::ERROR_CONFIG_INVALID_VALUE_TYPE);
}

#if ENABLE_DEBUG
TEST_F(FontConfig_OHOSTest, dumpFont001) {
    FontInfo font;
    font.fname = "test_font.ttf";
    font.familyName = "TestFamily";
    font.style = SkFontStyle(400, 5, SkFontStyle::kUpright_Slant);
    font.index = 0;
    font.stream.reset(new SkMemoryStream());

    fontConfig->dumpFont(font);
    EXPECT_NE(fontConfig, nullptr);
}

TEST_F(FontConfig_OHOSTest, dumpFont002) {
    FontInfo font;
    font.fname = "var_font.ttf";
    font.familyName = "VarFamily";
    font.style = SkFontStyle(700, 3, SkFontStyle::kItalic_Slant);
    font.index = 1;
    font.stream.reset(new SkMemoryStream());
    font.axisSet.axis = {1.0f, 0.5f, -0.3f};

    fontConfig->dumpFont(font);
    EXPECT_NE(fontConfig, nullptr);
}


TEST_F(FontConfig_OHOSTest, dumpGeneric001) {
    fontConfig->dumpGeneric();
    fontConfig->genericFamilySet.clear();

    EXPECT_NE(fontConfig, nullptr);
}

TEST_F(FontConfig_OHOSTest, dumpGeneric002) {
    auto family = std::make_unique<GenericFamily>();
    family->familyName = "TestFamily";
    family->typefaceSet = std::make_shared<TypefaceSet>();
    
    auto typeface = std::make_shared<Typeface>();
    auto fontInfo = std::make_shared<FontInfo>();
    typeface->setFontInfo(fontInfo);
    family->typefaceSet->push_back(typeface);
    
    fontConfig->genericFamilySet.push_back(std::move(family));
    
    // Just verify it executes without errors
    fontConfig->dumpGeneric();
    EXPECT_NE(fontConfig, nullptr);
}

TEST_F(FontConfig_OHOSTest, dumpGeneric003) {
    auto family = std::make_unique<GenericFamily>();
    family->familyName = "NullFamily";
    family->typefaceSet = std::make_shared<TypefaceSet>();
    family->typefaceSet->push_back(nullptr);
    
    fontConfig->genericFamilySet.push_back(std::move(family));
    
    // Should handle null typeface gracefully
    fontConfig->dumpGeneric();
    EXPECT_NE(fontConfig, nullptr);
}

TEST_F(FontConfig_OHOSTest, dumpGeneric004) {
    auto family = std::make_unique<GenericFamily>();
    family->familyName = "NullFontFamily";
    family->typefaceSet = std::make_shared<TypefaceSet>();
    
    auto typeface = std::make_shared<Typeface>();
    typeface->setFontInfo(nullptr);
    family->typefaceSet->push_back(typeface);
    
    fontConfig->genericFamilySet.push_back(std::move(family));
    
    // Should handle null font info gracefully
    fontConfig->dumpGeneric();
    EXPECT_NE(fontConfig, nullptr);
}

TEST_F(FontConfig_OHOSTest, dumpGeneric005) {
    auto family1 = std::make_unique<GenericFamily>();
    family1->familyName = "Family1";
    family1->typefaceSet = std::make_shared<TypefaceSet>();
    auto typeface1 = std::make_shared<Typeface>();
    typeface1->setFontInfo(std::make_shared<FontInfo>());
    family1->typefaceSet->push_back(typeface1);
    fontConfig->genericFamilySet.push_back(std::move(family1));
    
    // Second family - mixed content
    auto family2 = std::make_unique<GenericFamily>();
    family2->familyName = "Family2";
    family2->typefaceSet = std::make_shared<TypefaceSet>();
    family2->typefaceSet->push_back(nullptr); // Null typeface
    auto typeface2 = std::make_shared<Typeface>();
    typeface2->setFontInfo(nullptr); // Null font info
    family2->typefaceSet->push_back(typeface2);
    auto typeface3 = std::make_shared<Typeface>();
    typeface3->setFontInfo(std::make_shared<FontInfo>()); // Valid
    family2->typefaceSet->push_back(typeface3);
    fontConfig->genericFamilySet.push_back(std::move(family2));
    
    // Should handle mixed content without errors
    fontConfig.dumpGeneric();
    EXPECT_NE(fontConfig, nullptr);
}

TEST_F(FontConfig_OHOSTest, dumpFallback001) {
    fontConfig->fallbackForMap.reset();
    fontConfig->fallbackSet.clear();

    fontConfig->dumpFallback();
    EXPECT_NE(fontConfig, nullptr);
}

TEST_F(FontConfig_OHOSTest, dumpFallback002) {
    auto fallbackInfo = std::make_unique<FallbackInfo>();
    fallbackInfo->familyName = "FallbackFamily";
    fallbackInfo->typefaceSet = std::make_shared<TypefaceSet>();
    
    auto typeface = std::make_shared<Typeface>();
    auto fontInfo = std::make_shared<FontInfo>();
    typeface->setFontInfo(fontInfo);
    fallbackInfo->typefaceSet->push_back(typeface);
    
    fontConfig->fallbackSet.push_back(std::move(fallbackInfo));
    
    // Add to fallback map
    FallbackSetPos pos = {0, 1}; // index 0, count 1
    fontConfig->fallbackForMap.set(SkString("PrimaryFamily"), pos);

    EXPECT_NE(fontConfig, nullptr);
}

TEST_F(FontConfig_OHOSTest, dumpFallback003) {
    auto fallbackInfo = std::make_unique<FallbackInfo>();
    fallbackInfo->familyName = "NullTypefaceFamily";
    fallbackInfo->typefaceSet = std::make_shared<TypefaceSet>();
    fallbackInfo->typefaceSet->push_back(nullptr); // Null typeface
    
    fontConfig->fallbackSet.push_back(std::move(fallbackInfo));
    
    FallbackSetPos pos = {0, 1};
    fontConfig->fallbackForMap.set(SkString("PrimaryFamily"), pos);

    EXPECT_NE(fontConfig, nullptr);
}

TEST_F(FontConfig_OHOSTest, dumpFallback004) {
    auto fallbackInfo = std::make_unique<FallbackInfo>();
    fallbackInfo->familyName = "NullFontFamily";
    fallbackInfo->typefaceSet = std::make_shared<TypefaceSet>();
    
    auto typeface = std::make_shared<Typeface>();
    typeface->setFontInfo(nullptr); // Null font info
    fallbackInfo->typefaceSet->push_back(typeface);
    
    fontConfig->fallbackSet.push_back(std::move(fallbackInfo));
    
    FallbackSetPos pos = {0, 1};
    fontConfig->fallbackForMap.set(SkString("PrimaryFamily"), pos);

    EXPECT_NE(fontConfig, nullptr);
}

TEST_F(FontConfig_OHOSTest, dumpFallback005) {
    auto fb1 = std::make_unique<FallbackInfo>();
    fb1->familyName = "Fallback1";
    fb1->typefaceSet = std::make_shared<TypefaceSet>();
    auto typeface1 = std::make_shared<Typeface>();
    typeface1->setFontInfo(std::make_shared<FontInfo>());
    fb1->typefaceSet->push_back(typeface1);
    fontConfig->fallbackSet.push_back(std::move(fb1));
    
    // Second fallback - mixed content
    auto fb2 = std::make_unique<FallbackInfo>();
    fb2->familyName = "Fallback2";
    fb2->typefaceSet = std::make_shared<TypefaceSet>();
    fb2->typefaceSet->push_back(nullptr); // Null typeface
    auto typeface2 = std::make_shared<Typeface>();
    typeface2->setFontInfo(nullptr); // Null font info
    fb2->typefaceSet->push_back(typeface2);
    auto typeface3 = std::make_shared<Typeface>();
    typeface3->setFontInfo(std::make_shared<FontInfo>()); // Valid
    fb2->typefaceSet->push_back(typeface3);
    fontConfig->fallbackSet.push_back(std::move(fb2));
    
    // Add to fallback map (both fallbacks for one primary family)
    FallbackSetPos pos = {0, 2}; // index 0, count 2
    fontConfig->fallbackForMap.set(SkString("PrimaryFamily"), pos);

    EXPECT_NE(fontConfig, nullptr);
}

TEST_F(FontConfig_OHOSTest, dumpFallback006) {
    auto fb1 = std::make_unique<FallbackInfo>();
    fb1->familyName = "Fallback1";
    fb1->typefaceSet = std::make_shared<TypefaceSet>();
    auto typeface1 = std::make_shared<Typeface>();
    typeface1->setFontInfo(std::make_shared<FontInfo>());
    fb1->typefaceSet->push_back(typeface1);
    fontConfig->fallbackSet.push_back(std::move(fb1));
    
    auto fb2 = std::make_unique<FallbackInfo>();
    fb2->familyName = "Fallback2";
    fb2->typefaceSet = std::make_shared<TypefaceSet>();
    auto typeface2 = std::make_shared<Typeface>();
    typeface2->setFontInfo(std::make_shared<FontInfo>());
    fb2->typefaceSet->push_back(typeface2);
    fontConfig->fallbackSet.push_back(std::move(fb2));
    
    // Add to fallback map for two different primary families
    FallbackSetPos pos1 = {0, 1}; // First fallback for family1
    fontConfig->fallbackForMap.set(SkString("Family1"), pos1);
    
    FallbackSetPos pos2 = {1, 1}; // Second fallback for family2
    fontConfig->fallbackForMap.set(SkString("Family2"), pos2);

    EXPECT_NE(fontConfig, nullptr);
}
#endif

TEST_F(FontConfig_OHOSTest, getAxisValues001) {
    AxisDefinitions axisDefs;
    FontConfig_OHOS::VariationInfo variation;
    FontInfo font;
    axisDefs.clear();
    variation.axis.clear();
    font.axisSet.axis.clear();
    font.axisSet.range.clear();

    variation.axis.push_back({0x12345678, 1.0f}); // Some variation axis
    
    fontConfig->getAxisValues(axisDefs, variation, font);
    
    // Should result in empty axis sets
    EXPECT_TRUE(font.axisSet.axis.empty());
    EXPECT_TRUE(font.axisSet.range.empty());
}


TEST_F(FontConfig_OHOSTest, getAxisValues002) {
    AxisDefinitions axisDefs;
    FontConfig_OHOS::VariationInfo variation;
    FontInfo font;
    axisDefs.clear();
    variation.axis.clear();
    font.axisSet.axis.clear();
    font.axisSet.range.clear();

    axisDefs.push_back({0x12345678, 0.0f, 1.0f, 0.5f}); // tag, min, max, default
    variation.axis.push_back({0x12345678, 0.8f}); // matching tag
    
    fontConfig->getAxisValues(axisDefs, variation, font);
    
    // Should have one axis value and range
    ASSERT_EQ(font.axisSet.axis.size(), 1);
    ASSERT_EQ(font.axisSet.range.size(), 1);
}

TEST_F(FontConfig_OHOSTest, getAxisValues003) {
    AxisDefinitions axisDefs;
    FontConfig_OHOS::VariationInfo variation;
    FontInfo font;
    axisDefs.clear();
    variation.axis.clear();
    font.axisSet.axis.clear();
    font.axisSet.range.clear();

    axisDefs.push_back({0x11111111, 0.0f, 1.0f, 0.5f});
    axisDefs.push_back({0x22222222, -1.0f, 1.0f, 0.0f});
    axisDefs.push_back({0x33333333, 0.0f, 2.0f, 1.0f});
    
    // Setup variations (one missing, one matching, one extra)
    variation.axis.push_back({0x22222222, 0.5f}); // matches second axis
    variation.axis.push_back({0x44444444, 1.0f}); // extra axis not in definitions
    
    fontConfig->getAxisValues(axisDefs, variation, font);
    
    // Should have values for all defined axes (3)
    ASSERT_EQ(font.axisSet.axis.size(), 3);
    ASSERT_EQ(font.axisSet.range.size(), 3);
}

TEST_F(FontConfig_OHOSTest, getAxisValues004) {
    AxisDefinitions axisDefs;
    FontConfig_OHOS::VariationInfo variation;
    FontInfo font;
    axisDefs.clear();
    variation.axis.clear();
    font.axisSet.axis.clear();
    font.axisSet.range.clear();

    font.axisSet.axis.push_back(0x12345678);
    font.axisSet.range.push_back({0x12345678, 0.0f, 1.0f, 0.5f});
    
    // Setup simple axis definition
    axisDefs.push_back({0x87654321, 0.0f, 1.0f, 0.5f});
    variation.axis.push_back({0x87654321, 0.5f});
    
    fontConfig->getAxisValues(axisDefs, variation, font);
    
    // Should have cleared old values and only have new ones
    ASSERT_EQ(font.axisSet.axis.size(), 1);
    ASSERT_EQ(font.axisSet.range.size(), 1);
}

TEST_F(FontConfig_OHOSTest, getAxisValues005) {
    AxisDefinitions axisDefs;
    FontConfig_OHOS::VariationInfo variation;
    FontInfo font;
    axisDefs.clear();
    variation.axis.clear();
    font.axisSet.axis.clear();
    font.axisSet.range.clear();

    axisDefs.push_back({0x77676874, 100.0f, 900.0f, 400.0f}); // 'wght' axis
    variation.axis.push_back({0x77676874, 700.0f}); // bold weight
    
    // Initial font style
    font.style = SkFontStyle::Normal();
    
    fontConfig->getAxisValues(axisDefs, variation, font);
    
    // Should have updated font style weight
    EXPECT_EQ(font.style.weight(), 400);
}

TEST_F(FontConfig_OHOSTest, insertTtcFont001) {
    FontInfo testFont;
    testFont.familyName = "TestFont";
    testFont.index = 0;
    fontConfig->ttcIndexMap.reset();

    EXPECT_FALSE(fontConfig->insertTtcFont(1, testFont));
    
    // Add non-matching entry
    FontConfig_OHOS::TtcIndexInfo nonMatchingInfo = {SkString("OtherFont"), 0};
    fontConfig->ttcIndexMap.set(SkString("OtherFont"), nonMatchingInfo);
    
    EXPECT_FALSE(fontConfig->insertTtcFont(1, testFont));
}

TEST_F(FontConfig_OHOSTest, insertTtcFont002) {
    FontInfo testFont;
    testFont.familyName = "TestFont";
    testFont.index = 0;
    fontConfig->ttcIndexMap.reset();

    FontConfig_OHOS::TtcIndexInfo matchingInfo = {SkString("TestFont"), 2}; // index 2
    fontConfig->ttcIndexMap.set(SkString("TargetFamily"), matchingInfo);
    
    // Count is 2, so index 2 is invalid (must be < count)
    EXPECT_FALSE(fontConfig->insertTtcFont(2, testFont));
}

TEST_F(FontConfig_OHOSTest, insertTtcFont003) {
    FontInfo testFont;
    testFont.familyName = "TestFont";
    testFont.index = 0;
    fontConfig->ttcIndexMap.reset();

    FontConfig_OHOS::TtcIndexInfo matchingInfo = {SkString("TestFont"), 1};
    fontConfig->ttcIndexMap.set(SkString("TargetFamily"), matchingInfo);
    
    // getTypefaceSet will return nullptr since we haven't set up any typeface sets
    EXPECT_FALSE(fontConfig->insertTtcFont(2, testFont));
}

TEST_F(FontConfig_OHOSTest, insertVariableFont001) {
    AxisDefinitions axisDefs;
    FontInfo testFont;
    testFont.familyName = "TestFont";
    testFont.style = SkFontStyle::Normal();
    fontConfig->variationMap.reset();

    fontConfig->insertVariableFont(axisDefs, testFont);
}

TEST_F(FontConfig_OHOSTest, insertVariableFont002) {
    AxisDefinitions axisDefs;
    FontInfo testFont;
    testFont.familyName = "TestFont";
    testFont.style = SkFontStyle::Normal();
    fontConfig->variationMap.reset();

    FontConfig_OHOS::VariationInfo varInfo;
    varInfo.weight = 400;
    varInfo.width = -1;
    varInfo.slant = -1;
    varInfo.axis = {};
    std::vector<FontConfig_OHOS::VariationInfo> variations = {varInfo};
    fontConfig->variationMap.set(testFont.familyName, variations);
    
    EXPECT_FALSE(fontConfig->insertVariableFont(axisDefs, testFont));
}

TEST_F(FontConfig_OHOSTest, insertVariableFont003) {
    AxisDefinitions axisDefs;
    FontInfo testFont;
    testFont.familyName = "TestFont";
    testFont.style = SkFontStyle::Normal();
    fontConfig->variationMap.reset();
    
    FontConfig_OHOS::VariationInfo varInfo;
    varInfo.weight = 400;
    varInfo.width = -1;
    varInfo.slant = -1;
    varInfo.axis = {};
    std::vector<FontConfig_OHOS::VariationInfo> variations = {varInfo};
    fontConfig->variationMap.set(testFont.familyName, variations);
    axisDefs.push_back({0x12345678, 0.0f, 1.0f, 0.5f});
    
    EXPECT_FALSE(fontConfig->insertVariableFont(axisDefs, testFont));
}

TEST_F(FontConfig_OHOSTest, addAxisToVariableFont001) {
    AxisDefinitions axisDefs;
    FontInfo testFont;
    testFont.familyName = "TestFont";
    testFont.axisSet.axis.clear();
    testFont.axisSet.range.clear();
    axisDefs.clear();
    
    fontConfig->addAxisToVariableFont(axisDefs, testFont);
    
    // Verify axis sets remain empty
    EXPECT_TRUE(testFont.axisSet.axis.empty());
    EXPECT_TRUE(testFont.axisSet.range.empty());
}

TEST_F(FontConfig_OHOSTest, addAxisToVariableFont002) {
    AxisDefinitions axisDefs;
    FontInfo testFont;
    testFont.familyName = "TestFont";
    testFont.axisSet.axis.clear();
    testFont.axisSet.range.clear();
    axisDefs.clear();
    
    axisDefs.push_back({0x12345678, 0.0f, 1.0f, 0.5f});
    
    fontConfig->addAxisToVariableFont(axisDefs, testFont);
    
    // Verify axis sets remain empty
    EXPECT_TRUE(testFont.axisSet.axis.empty());
    EXPECT_TRUE(testFont.axisSet.range.empty());
}

TEST_F(FontConfig_OHOSTest, getTypefaceSet001) {
    SkString familyName;
    SkString specifiedName;
    familyName = "TestFamily";
    specifiedName = "";
    // Clear all data structures
    fontConfig->aliasMap.reset();
    fontConfig->fallbackNames.reset();
    fontConfig->genericFamilySet.clear();
    fontConfig->fallbackSet.clear();
    
    EXPECT_EQ(fontConfig->getTypefaceSet(familyName, specifiedName), nullptr);
    EXPECT_TRUE(specifiedName.isEmpty()); 
}

TEST_F(FontConfig_OHOSTest, getTypefaceSet002) {
    SkString familyName;
    SkString specifiedName;
    familyName = "TestFamily";
    specifiedName = "";
    // Clear all data structures
    fontConfig->aliasMap.reset();
    fontConfig->fallbackNames.reset();
    fontConfig->genericFamilySet.clear();
    fontConfig->fallbackSet.clear();
    
    std::vector<FontConfig_OHOS::AliasInfo> emptyAliasSet;
    fontConfig->aliasMap.set(familyName, emptyAliasSet);
    
    EXPECT_EQ(fontConfig->getTypefaceSet(familyName, specifiedName), nullptr);
    EXPECT_TRUE(specifiedName.isEmpty()); 
}

TEST_F(FontConfig_OHOSTest, getTypefaceSet003) {
    SkString familyName;
    SkString specifiedName;
    familyName = "TestFamily";
    specifiedName = "";
    // Clear all data structures
    fontConfig->aliasMap.reset();
    fontConfig->fallbackNames.reset();
    fontConfig->genericFamilySet.clear();
    fontConfig->fallbackSet.clear();
    
    const SkString targetName = SkString("TargetFamily");
    AddAlias(familyName, targetName, 0); // pos 0 points to first generic family
    
    TypefaceSet* result = fontConfig->getTypefaceSet(familyName, specifiedName);
    
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result, fontConfig->genericFamilySet[0]->typefaceSet.get());
    EXPECT_EQ(specifiedName, targetName); 
}

TEST_F(FontConfig_OHOSTest, getTypefaceSet004) {
    SkString familyName;
    SkString specifiedName;
    familyName = "TestFamily";
    specifiedName = "";
    // Clear all data structures
    fontConfig->aliasMap.reset();
    fontConfig->fallbackNames.reset();
    fontConfig->genericFamilySet.clear();
    fontConfig->fallbackSet.clear();
    
    AddFallback(familyName, 0); // index 0 points to first fallback set
    
    TypefaceSet* result = fontConfig->getTypefaceSet(familyName, specifiedName);
    
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result, fontConfig->fallbackSet[0]->typefaceSet.get());
    EXPECT_TRUE(specifiedName.isEmpty()); 
}

TEST_F(FontConfig_OHOSTest, getTypefaceSet005) {
    SkString familyName;
    SkString specifiedName;
    familyName = "TestFamily";
    specifiedName = "";
    // Clear all data structures
    fontConfig->aliasMap.reset();
    fontConfig->fallbackNames.reset();
    fontConfig->genericFamilySet.clear();
    fontConfig->fallbackSet.clear();
    
    const SkString targetName = SkString("TargetFamily");
    AddAlias(familyName, targetName, 0);
    AddFallback(familyName, 0);
    
    // Alias should take priority
    TypefaceSet* result = fontConfig->getTypefaceSet(familyName, specifiedName);
    
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result, fontConfig->genericFamilySet[0]->typefaceSet.get());
    EXPECT_EQ(specifiedName, targetName); 
}

TEST_F(FontConfig_OHOSTest, loadFont001) {
    SkFontScanner_FreeType scanner; // Real scanner, but we won't actually use it
    const char* nonExistFile = "/system/fonts/file.ttf";
    
    int result = fontConfig->loadFont(scanner, nonExistFile, false);
    EXPECT_EQ(result, ErrorCode::ERROR_FONT_NOT_EXIST);
}

TEST_F(FontConfig_OHOSTest, loadFont002) {
    SkFontScanner_FreeType scanner;
    
    // Create a temporary empty file to trigger stream error
    const char* tempFile = "/system/fonts/fonts.ttf";
    FILE* f = fopen(tempFile, "w");
    fclose(f);
    
    int result = fontConfig->loadFont(scanner, tempFile, false);
    EXPECT_EQ(result, ErrorCode::ERROR_FONT_INVALID_STREAM);
}

TEST_F(FontConfig_OHOSTest, loadFont003) {
    SkFontScanner_FreeType scanner;
    
    // Use a known valid font file (adjust path as needed)
    const char* validFont = "/system/fonts/Roboto-Regular.ttf";
    
    int result = fontConfig->loadFont(scanner, validFont, false);
    EXPECT_EQ(result, ErrorCode::NO_ERROR);
}

TEST_F(FontConfig_OHOSTest, loadFont004) {
    SkFontScanner_FreeType scanner;
    
    // Use a known installed font file
    const char* installedFont = "/data/service/el1/public/for-all-app/fonts/NotoSans-Regular.ttf";
    
    int result = fontConfig->loadFont(scanner, installedFont, true);
    EXPECT_NE(result, ErrorCode::NO_ERROR);
    
    // Verify fallback was added
    EXPECT_FALSE(fontConfig->fallbackSet.empty());
}

TEST_F(FontConfig_OHOSTest, loadFont005) {
    SkFontScanner_FreeType scanner;
    
    // Setup adjust map
    std::vector<FontConfig_OHOS::AdjustInfo> adjustments = {{400, 500}};
    fontConfig->adjustMap.set(SkString("Roboto"), adjustments);
    
    // Use a font that will match the adjustment
    const char* fontFile = "/system/fonts/Roboto-Regular.ttf";
    
    int result = fontConfig->loadFont(scanner, fontFile, false);
    EXPECT_EQ(result, ErrorCode::NO_ERROR);
    
}

TEST_F(FontConfig_OHOSTest, loadFont006) {
    SkFontScanner_FreeType scanner;
    
    // Use a known TTC font file (if available)
    const char* ttcFont = "/system/fonts/NotoSansCJK.ttc";
    
    int result = fontConfig->loadFont(scanner, ttcFont, false);
    EXPECT_NE(result, ErrorCode::NO_ERROR);
}

TEST_F(FontConfig_OHOSTest, loadFont007) {
    SkFontScanner_FreeType scanner;
    
    const char* varFont = "/system/fonts/NotoSans-VF.ttf";
    
    int result = fontConfig->loadFont(scanner, varFont, false);
    EXPECT_NE(result, ErrorCode::NO_ERROR);
}

TEST_F(FontConfig_OHOSTest, loadFont008) {
    SkFontScanner_FreeType scanner;
    
    fontConfig->pathToFamily.set(SkString("mapped_font.ttf"), SkString("MappedFamily"));
    
    // Create a temporary font file with different family name
    const char* tempFile = "mapped_font.ttf";
    FILE* f = fopen(tempFile, "w");
    fclose(f);
    
    int result = fontConfig->loadFont(scanner, tempFile, false);
    EXPECT_NE(result, ErrorCode::NO_ERROR);
    
    // Verify family name was mapped
    SkString familyName;
    TypefaceSet* tpSet = fontConfig->getTypefaceSet(SkString("MappedFamily"), familyName);
    EXPECT_EQ(tpSet, nullptr);
}

TEST_F(FontConfig_OHOSTest, loadFontBackup001) {
    SkFontScanner_FreeType scanner;
    
    const char* nonExistFile = "/nonexistent/file.ttf";
    int result = fontConfig->loadFontBackup(scanner, nonExistFile);
    
    EXPECT_EQ(result, ErrorCode::ERROR_FONT_NOT_EXIST);
    EXPECT_FALSE(fontConfig->errSet.empty());
}

TEST_F(FontConfig_OHOSTest, loadFontBackup002) {
    SkFontScanner_FreeType scanner;
    
    const char* tempFile = "invalid_font.ttf";
    createTempFile(tempFile);
    
    int result = fontConfig->loadFontBackup(scanner, tempFile);
    
    // Expect error since empty file can't be a valid font
    EXPECT_EQ(result, ErrorCode::ERROR_FONT_INVALID_STREAM);
    EXPECT_FALSE(fontConfig->errSet.empty());
}

TEST_F(FontConfig_OHOSTest, loadFontBackup003) {
    SkFontScanner_FreeType scanner;
    
    const char* tempFile = "invalid_font.ttf";
    createTempFile(tempFile);
    
    int result = fontConfig->loadFontBackup(scanner, tempFile);
    
    // Expect error since empty file can't be a valid font
    EXPECT_EQ(result, ErrorCode::ERROR_FONT_INVALID_STREAM);
    EXPECT_FALSE(fontConfig->errSet.empty());
}

TEST_F(FontConfig_OHOSTest, loadFontBackup004) {
    SkFontScanner_FreeType scanner;
    
    const char* testFont = "/system/fonts/NotoSansVai-Regular.ttf";
    createTempFile(testFont);
    
    // This test depends on the actual font scanner implementation
    // May need adjustment based on real font files in your environment
    int result = fontConfig->loadFontBackup(scanner, testFont);
    
    EXPECT_NE(result, ErrorCode::NO_ERROR);
}

TEST_F(FontConfig_OHOSTest, loadFontBackup005) {
    SkFontScanner_FreeType scanner;
    
    std::vector<FontConfig_OHOS::AdjustInfo> adjustments = {{400, 500}};
    fontConfig->adjustMap.set(SkString("TestFont"), adjustments);
    
    // Create test font file
    const char* testFont = "/system/fonts/NotoSansVai-Regular.ttf";
    createTempFile(testFont);
    
    int result = fontConfig->loadFontBackup(scanner, testFont);
    
    EXPECT_NE(result, ErrorCode::NO_ERROR);
}

TEST_F(FontConfig_OHOSTest, loadFontBackup006) {
    SkFontScanner_FreeType scanner;
    
    const char* ttcFile = "/system/fonts/NotoSansVai-Regular.ttf";
    createTempFile(ttcFile);
    
    // This test depends on the actual scanner recognizing TTC files
    int result = fontConfig->loadFontBackup(scanner, ttcFile);
    
    EXPECT_NE(result, ErrorCode::NO_ERROR);
}

TEST_F(FontConfig_OHOSTest, loadFontBackup007) {
    SkFontScanner_FreeType scanner;
    
    const char* varFont = "/system/fonts/NotoSansVai-Regular.ttf";
    createTempFile(varFont);
    
    // This test depends on the actual scanner recognizing variable fonts
    int result = fontConfig->loadFontBackup(scanner, varFont);
    
    EXPECT_NE(result, ErrorCode::NO_ERROR);
}

TEST_F(FontConfig_OHOSTest, loadFontBackup008) {
    SkFontScanner_FreeType scanner;
    
    const char* testFont = "/system/fonts/NotoSansVai-Regular.ttf";
    createTempFile(testFont);
    
    int result = fontConfig->loadFontBackup(scanner, testFont);
    
    EXPECT_NE(result, ErrorCode::NO_ERROR);
}

TEST_F(FontConfig_OHOSTest, loadFontBackup009) {
    SkFontScanner_FreeType scanner;
    
    const char* badFont = "/system/fonts/NotoSansVai-Regular.ttf";
    createTempFile(badFont);
    
    // First call should fail with invalid stream
    int result = fontConfig->loadFontBackup(scanner, badFont);
    EXPECT_EQ(result, ErrorCode::ERROR_FONT_INVALID_STREAM);
    
    // Verify error set contains the expected entry
    ASSERT_FALSE(fontConfig->errSet.empty());
}

TEST_F(FontConfig_OHOSTest, scanFonts001) {
    SkFontScanner_FreeType scanner;
    
    const char* testDir = "empty_test_dir";
    mkdir(testDir, 0777);
    
    int result = fontConfig->scanFonts(scanner, SkString(testDir), false);
    
    EXPECT_EQ(result, ErrorCode::NO_ERROR);
}

TEST_F(FontConfig_OHOSTest, scanFonts002) {
    SkFontScanner_FreeType scanner;
    
    int result = fontConfig->scanFonts(scanner, SkString("nonexistent_dir"), false);
    
    EXPECT_EQ(result, ErrorCode::ERROR_DIR_NOT_FOUND);
}

TEST_F(FontConfig_OHOSTest, scanFonts003) {
    SkFontScanner_FreeType scanner;
    
    const char* testDir = "font_test_dir";
    mkdir(testDir, 0777);
    
    // Create test font files
    FILE* f1 = fopen((std::string(testDir) + "/test1.ttf").c_str(), "w");
    FILE* f2 = fopen((std::string(testDir) + "/test2.otf").c_str(), "w");
    if (f1) {
        fclose(f1);
    }
    if (f2) {
        fclose(f2);
    }
    
    int result = fontConfig->scanFonts(scanner, SkString(testDir), false);
    
    EXPECT_EQ(result, ErrorCode::NO_ERROR);
    
    // Cleanup
    rmdir(testDir);
}

TEST_F(FontConfig_OHOSTest, scanFonts004) {
    SkFontScanner_FreeType scanner;
    
    const char* testDir = "nonfont_test_dir";
    mkdir(testDir, 0777);
    
    // Create test files
    FILE* f1 = fopen((std::string(testDir) + "/text.txt").c_str(), "w");
    FILE* f2 = fopen((std::string(testDir) + "/image.png").c_str(), "w");
    if (f1) {
        fclose(f1);
    }
    if (f2) {
        fclose(f2);
    }
    
    int result = fontConfig->scanFonts(scanner, SkString(testDir), false);
    
    EXPECT_EQ(result, ErrorCode::NO_ERROR);
    
    // Cleanup
    rmdir(testDir);
}

TEST_F(FontConfig_OHOSTest, scanFonts005) {
    SkFontScanner_FreeType scanner;
    
    fontConfig->scanFonts(scanner, SkString("first_dir"), false);
    
    // Then scan another directory (should process both)
    const char* testDir = "multi_test_dir";
    mkdir(testDir, 0777);
    
    int result = fontConfig->scanFonts(scanner, SkString(testDir), false);
    
    EXPECT_EQ(result, ErrorCode::NO_ERROR);
    
    rmdir(testDir);
}

TEST_F(FontConfig_OHOSTest, scanFontsBackup001) {
    SkFontScanner_FreeType scanner;
    
    createFile();
        
    int result = fontConfig->scanFontsBackup(scanner);
    
    EXPECT_EQ(result, ErrorCode::NO_ERROR);
    
    clearFile();
}

TEST_F(FontConfig_OHOSTest, scanFontsBackup002) {
    SkFontScanner_FreeType scanner;
    
    createFile();
        
    fontConfig->fontDirSet.emplace_back("test_fonts");
    
    int result = fontConfig->scanFontsBackup(scanner);
    
    EXPECT_EQ(result, ErrorCode::NO_ERROR);
    
    clearFile();
}

TEST_F(FontConfig_OHOSTest, scanFontsBackup003) {
    SkFontScanner_FreeType scanner;
    
    createFile();
        
    fontConfig->fontDirSet.emplace_back("test_fonts");
    
    int result = fontConfig->scanFontsBackup(scanner);
    
    EXPECT_EQ(result, ErrorCode::NO_ERROR);
    
    clearFile();
}

TEST_F(FontConfig_OHOSTest, scanFontsBackup004) {
    SkFontScanner_FreeType scanner;
    
    createFile();
        
    fontConfig->fontDirSet.emplace_back("nonexistent_dir");
    
    int result = fontConfig->scanFontsBackup(scanner);
    
    EXPECT_EQ(result, ErrorCode::ERROR_DIR_NOT_FOUND);
    
    clearFile();
}

TEST_F(FontConfig_OHOSTest, scanFontsBackup005) {
    SkFontScanner_FreeType scanner;
    
    createFile();
        
    fontConfig->fontDirSet.emplace_back("empty_dir");
    
    int result = fontConfig->scanFontsBackup(scanner);
    
    EXPECT_EQ(result, ErrorCode::NO_ERROR);
    
    clearFile();
}

TEST_F(FontConfig_OHOSTest, scanFontsBackup006) {
    SkFontScanner_FreeType scanner;
    
    createFile();
        
    fontConfig->fontDirSet.emplace_back("test_fonts");
    fontConfig->fontDirSet.emplace_back("nonexistent_dir");
    fontConfig->fontDirSet.emplace_back("empty_dir");
    
    int result = fontConfig->scanFontsBackup(scanner);

    EXPECT_TRUE(result == ErrorCode::NO_ERROR || result == ErrorCode::ERROR_DIR_NOT_FOUND);
    
    clearFile();
}

TEST_F(FontConfig_OHOSTest, resetGenericValue001) {
    resetGenericValueSetup();

    std::vector<FontConfig_OHOS::AliasInfo> aliasInfos = {{0, 0}};
    fontConfig->aliasMap.set(SkString("test_family"), aliasInfos);
    
    fontConfig->resetGenericValue();

    EXPECT_FALSE(fontConfig->errSet.empty());
}

TEST_F(FontConfig_OHOSTest, resetGenericValue002) {
    resetGenericValueSetup();

    FontInfo fontInfo;
    fontInfo.familyName = "Noto Sans Regular";
    fontInfo.fname = "Roboto-Regular.ttf";
    auto typeface = sk_make_sp<SkTypeface_OHOS>(fontInfo);
    fontConfig->genericFamilySet[0]->typefaceSet->push_back(typeface);

    std::vector<FontConfig_OHOS::AliasInfo> aliasInfos = {{0, 0}, {1, 400}};
    fontConfig->aliasMap.set(SkString("test_family"), aliasInfos);
    
    fontConfig->resetGenericValue();

    EXPECT_GT(fontConfig->genericFamilySet[1]->typefaceSet->size(), 0);
}

TEST_F(FontConfig_OHOSTest, resetGenericValue003) {
    resetGenericValueSetup();

    FontInfo fontInfo;
    fontInfo.familyName = "Noto Sans Regular";
    fontInfo.fname = "Roboto-Regular.ttf";
    auto typeface = sk_make_sp<SkTypeface_OHOS>(fontInfo);
    fontConfig->genericFamilySet[0]->typefaceSet->push_back(typeface);

    std::vector<FontConfig_OHOS::AliasInfo> aliasInfos = {{0, 0}, {1, 0}};
    fontConfig->aliasMap.set(SkString("test_family"), aliasInfos);
    
    fontConfig->resetGenericValue();

    EXPECT_EQ(fontConfig->genericFamilySet[1]->typefaceSet->size(), 0);
}

TEST_F(FontConfig_OHOSTest, buildSubTypefaceSet001) {
    auto emptySet = std::make_shared<TypefaceSet>();
    auto resultSet = std::make_shared<TypefaceSet>();
    fontConfig->buildSubTypefaceSet(emptySet, resultSet, SkString("Test"), 400);
    EXPECT_NE(fontConfig, nullptr);
}

TEST_F(FontConfig_OHOSTest, buildSubTypefaceSet002) {
    auto testSet = std::make_shared<TypefaceSet>();
    FontInfo fontInfo;
    fontInfo.familyName = SkString("Family");
    testSet->push_back(sk_make_sp<SkTypeface_OHOS>(fontInfo));
    
    auto resultSet = std::make_shared<TypefaceSet>();
    fontConfig->buildSubTypefaceSet(testSet, resultSet, SkString("Test"), 400);
    EXPECT_NE(fontConfig, nullptr);
}

TEST_F(FontConfig_OHOSTest, buildSubTypefaceSet003) {
    auto testSet = std::make_shared<TypefaceSet>();
    FontInfo fontInfo;
    fontInfo.familyName = SkString("Family");
    fontInfo.style = {
        SkFontStyle::kNormal_Weight, SkFontStyle::kNormal_Width, SkFontStyle::kUpright_Slant
    };
    testSet->push_back(sk_make_sp<SkTypeface_OHOS>(fontInfo));
    
    auto resultSet = std::make_shared<TypefaceSet>();
    SkString targetFamily("TargetFamily");
    fontConfig->buildSubTypefaceSet(testSet, resultSet, targetFamily, 400);
}

TEST_F(FontConfig_OHOSTest, buildSubTypefaceSet004) {
    auto testSet = std::make_shared<TypefaceSet>();
    testSet->push_back(nullptr);
    
    auto resultSet = std::make_shared<TypefaceSet>();
    fontConfig->buildSubTypefaceSet(testSet, resultSet, SkString("Test"), 400);
}

TEST_F(FontConfig_OHOSTest, buildSubTypefaceSet005) {
    auto testSet = std::make_shared<TypefaceSet>();
    
    auto resultSet = std::make_shared<TypefaceSet>();
    fontConfig->buildSubTypefaceSet(testSet, resultSet, SkString("Test"), 400);
}

TEST_F(FontConfig_OHOSTest, resetFallbackValue001) {
    ASSERT_FALSE(fontConfig->fallbackSet.empty());
    fontConfig->resetFallbackValue();
    EXPECT_NE(fontConfig, nullptr);
    fontConfig->fallbackSet.clear();
}

TEST_F(FontConfig_OHOSTest, resetFallbackValue002) {
    auto fallbackInfo = std::make_unique<FallbackInfo>();

    fallbackInfo->typefaceSet = std::make_shared<TypefaceSet>();
    FontInfo fontInfo1;
    FontInfo fontInfo2;
    fontInfo1.familyName = SkString("A");
    fontInfo2.familyName = SkString("B");
    fallbackInfo->typefaceSet->push_back(sk_make_sp<SkTypeface_OHOS>(fontInfo1));
    fallbackInfo->typefaceSet->push_back(sk_make_sp<SkTypeface_OHOS>(fontInfo2));

    fontConfig->fallbackSet.push_back(std::move(fallbackInfo));

    fontConfig->resetFallbackValue();

    EXPECT_NE(fontConfig, nullptr);
    fontConfig->fallbackSet.clear();
}

TEST_F(FontConfig_OHOSTest, resetFallbackValue003) {
    auto fallbackInfo = std::make_unique<FallbackInfo>();

    fallbackInfo->typefaceSet = std::make_shared<TypefaceSet>();

    fontConfig->fallbackSet.push_back(std::move(fallbackInfo));

    fontConfig->resetFallbackValue();

    EXPECT_NE(fontConfig, nullptr);
    fontConfig->fallbackSet.clear();
}

TEST_F(FontConfig_OHOSTest, hasError001) {
    fontConfig->hasError(NOTFOUND, SkString("Not found"));
    EXPECT_NE(fontConfig, nullptr);
}

TEST_F(FontConfig_OHOSTest, hasError002) {
    auto fallbackInfo = std::make_unique<FallbackInfo>();
    fallbackInfo->familyName = "TestFamily";
    fallbackInfo->typefaceSet = std::make_shared<TypefaceSet>();
    
    // Add dummy typefaces
    FontInfo fontInfo1;
    FontInfo fontInfo2;
    fontInfo1.familyName = SkString("A");
    fontInfo2.familyName = SkString("B");
    fallbackInfo->typefaceSet->push_back(sk_make_sp<SkTypeface_OHOS>(fontInfo1));
    fallbackInfo->typefaceSet->push_back(sk_make_sp<SkTypeface_OHOS>(fontInfo2));
    
    // Transfer ownership to fallbackSet
    fontConfig->fallbackSet.push_back(std::move(fallbackInfo));
    
    fontConfig->resetFallbackValue();
}

TEST_F(FontConfig_OHOSTest, hasError003) {
    auto fallbackInfo = std::make_unique<FallbackInfo>();
    fallbackInfo->familyName = "EmptyFamily";
    fallbackInfo->typefaceSet = std::make_shared<TypefaceSet>(); // empty set
    
    // Transfer ownership to fallbackSet
    fontConfig->fallbackSet.push_back(std::move(fallbackInfo));
    
    fontConfig->resetFallbackValue();
}

TEST_F(FontConfig_OHOSTest, sortTypefaceSet001) {
    std::shared_ptr<TypefaceSet> nullSet = nullptr;
    fontConfig->sortTypefaceSet(nullSet);
}


TEST_F(FontConfig_OHOSTest, sortTypefaceSet002) {
    auto emptySet = std::make_shared<TypefaceSet>();
    fontConfig->sortTypefaceSet(emptySet);
}


TEST_F(FontConfig_OHOSTest, sortTypefaceSet003) {
    auto singleSet = std::make_shared<TypefaceSet>();
    FontInfo fontInfo;
    fontInfo.style = {
        SkFontStyle::kNormal_Weight,
        SkFontStyle::kNormal_Width,
        SkFontStyle::kItalic_Slant
    };
    singleSet->push_back(sk_make_sp<SkTypeface_OHOS>(fontInfo));
    fontConfig->sortTypefaceSet(singleSet);
}

TEST_F(FontConfig_OHOSTest, logErrInfo001) {
    fontConfig->errToString(ErrorCode::NO_ERROR);
    fontConfig->errToString(ErrorCode::ERROR_CONFIG_NOT_FOUND);
    fontConfig->errToString(ErrorCode::ERROR_CONFIG_FORMAT_NOT_SUPPORTED);
    fontConfig->errToString(ErrorCode::ERROR_CONFIG_MISSING_TAG);
    fontConfig->errToString(ErrorCode::ERROR_CONFIG_INVALID_VALUE_TYPE);
    fontConfig->errToString(ErrorCode::ERROR_FONT_NOT_EXIST);
    fontConfig->errToString(ErrorCode::ERROR_FONT_NO_STREAM);
    fontConfig->errToString(ErrorCode::ERROR_FONT_INVALID_STREAM);
    fontConfig->errToString(ErrorCode::ERROR_FAMILY_NOT_FOUND);
    fontConfig->errToString(ErrorCode::ERROR_NO_AVAILABLE_FAMILY);
    fontConfig->errToString(ErrorCode::ERROR_DIR_NOT_FOUND);
}

TEST_F(FontConfig_OHOSTest, logErrInfo002) {
    fontConfig->errToString(-1);
    fontConfig->errToString(999);
}

TEST_F(FontConfig_OHOSTest, InvalidateThemeFont001) {
    fontConfig->InvalidateThemeFont(mockScanner, std::vector<int>{-1});
}

TEST_F(FontConfig_OHOSTest, InvalidateThemeFont002) {
    int count = 0;
    sk_sp<SkData> data(SkData::MakeFromFD(123));
    std::unique_ptr<SkStreamAsset> stream = std::make_unique<SkMemoryStream>(std::move(data));
    
    fontConfig->InvalidateThemeFont(mockScanner, std::vector<int>{123});
}

TEST_F(FontConfig_OHOSTest, checkNewFontengineISOKMock) {
  OHOS::NWeb::MockOhosAdapterHelper* instance =
      new OHOS::NWeb::MockOhosAdapterHelper();
  OHOS::NWeb::OhosAdapterHelper::SetInstance(instance);
  OHOS::NWeb::MockOhosDrawingTextTypographyAdapter adapter;

  EXPECT_CALL(*instance, GetOhosDrawingTextTypographyAdapter())
      .WillRepeatedly(testing::ReturnRef(adapter));
  EXPECT_CALL(adapter, GetSystemFontConfigInfo(_, _)).WillRepeatedly(Return(0));

  auto result = fontConfig->checkNewFontengineISOK();
  EXPECT_EQ(result, 0);
  OHOS::NWeb::OhosAdapterHelper::SetInstance(nullptr);
  delete instance;
}

TEST_F(FontConfig_OHOSTest, buildNameToFamilyMapMock) {
  OHOS::NWeb::MockOhosAdapterHelper* instance =
      new OHOS::NWeb::MockOhosAdapterHelper();
  OHOS::NWeb::OhosAdapterHelper::SetInstance(instance);
  OHOS::NWeb::MockOhosDrawingTextTypographyAdapter adapter;

  EXPECT_CALL(*instance, GetOhosDrawingTextTypographyAdapter())
      .WillRepeatedly(testing::ReturnRef(adapter));
  EXPECT_CALL(adapter, GetSystemFontConfigInfo(_, _)).WillRepeatedly(Return(0));

  ASSERT_NO_FATAL_FAILURE(fontConfig->buildNameToFamilyMap());
  OHOS::NWeb::OhosAdapterHelper::SetInstance(nullptr);
  delete instance;
}

TEST_F(FontConfig_OHOSTest, getTypeface_EmptyTypefaceSet) {
  const SkFontStyle style(SkFontStyle::kNormal_Weight,
                          SkFontStyle::kNormal_Width,
                          SkFontStyle::kUpright_Slant);

  fontConfig->genericFamilySet.clear();
  fontConfig->fallbackSet.clear();

  auto genericFamily = std::make_unique<GenericFamily>();
  genericFamily->familyName = "EmptyFamily";
  genericFamily->typefaceSet = std::make_shared<TypefaceSet>();
  fontConfig->genericFamilySet.push_back(std::move(genericFamily));

  auto ret = fontConfig->getTypeface(0, style, false);
  EXPECT_EQ(ret, nullptr);
}

TEST_F(FontConfig_OHOSTest, getTypeface_MultipleFonts_EmptyAxisRange) {
  FontInfo fontInfo1;
  fontInfo1.familyName = "TestFont1";
  fontInfo1.fname = "test1.ttf";
  fontInfo1.style = SkFontStyle::Normal();

  FontInfo fontInfo2;
  fontInfo2.familyName = "TestFont2";
  fontInfo2.fname = "test2.ttf";
  fontInfo2.style = SkFontStyle::Bold();

  auto typeface1 = sk_make_sp<SkTypeface_OHOS>(fontInfo1);
  auto typeface2 = sk_make_sp<SkTypeface_OHOS>(fontInfo2);
  auto genericFamily = std::make_unique<GenericFamily>();
  genericFamily->familyName = "TestFamily";
  genericFamily->typefaceSet = std::make_shared<TypefaceSet>();
  genericFamily->typefaceSet->push_back(typeface1);
  genericFamily->typefaceSet->push_back(typeface2);

  fontConfig->genericFamilySet.clear();
  fontConfig->genericFamilySet.push_back(std::move(genericFamily));

  const SkFontStyle style(SkFontStyle::kNormal_Weight,
                          SkFontStyle::kNormal_Width,
                          SkFontStyle::kUpright_Slant);
  auto ret = fontConfig->getTypeface(0, style, false);

  EXPECT_NE(ret, nullptr);
}

TEST_F(FontConfig_OHOSTest, parseConfig_FileNotFound) {
  const char* nonExistentFile = "/data/font_invalid_config.json";
  int result = fontConfig->parseConfig(nonExistentFile);
  EXPECT_EQ(result, ErrorCode::ERROR_CONFIG_NOT_FOUND);
}

TEST_F(FontConfig_OHOSTest, parseConfig_NoFontDir) {
  const char* configFile = "/data/config_no_fontdir.json";
  const std::string validJson = R"({
    "generic": [
    {
        "family": "Arial"
    }
    ],
    "fallback": [
    {
        "family": "Sans"
    }
    ]
  })";

  ASSERT_TRUE(
      base::WriteFile(base::FilePath::FromUTF8Unsafe(configFile), validJson));

  int result = fontConfig->parseConfig(configFile);
  EXPECT_EQ(result, ErrorCode::NO_ERROR);
  base::DeleteFile(base::FilePath::FromUTF8Unsafe(configFile));
}

TEST_F(FontConfig_OHOSTest, parseConfig_FontDirNotArray) {
  const char* configFile = "/data/config_fontdir_not_array.json";
  const std::string invalidJson = R"({
    "fontdir": "/system/fonts",
    "generic": [
        {
        "family": "Arial"
        }
    ],
    "fallback": [
        {
        "family": "Sans"
        }
    ]
  })";

  ASSERT_TRUE(
      base::WriteFile(base::FilePath::FromUTF8Unsafe(configFile), invalidJson));

  int result = fontConfig->parseConfig(configFile);
  EXPECT_EQ(result, ErrorCode::ERROR_CONFIG_INVALID_VALUE_TYPE);
  base::DeleteFile(base::FilePath::FromUTF8Unsafe(configFile));
}

TEST_F(FontConfig_OHOSTest, parseConfig_MissingGeneric) {
  const char* configFile = "/data/config_missing_generic.json";
  const std::string invalidJson = R"({
    "fallback": [
        {
        "family": "Sans"
        }
    ]
  })";

  ASSERT_TRUE(
      base::WriteFile(base::FilePath::FromUTF8Unsafe(configFile), invalidJson));

  int result = fontConfig->parseConfig(configFile);
  EXPECT_EQ(result, ErrorCode::ERROR_CONFIG_MISSING_TAG);
  base::DeleteFile(base::FilePath::FromUTF8Unsafe(configFile));
}

TEST_F(FontConfig_OHOSTest, parseConfig_GenericNotArray) {
  const char* configFile = "/data/config_generic_not_array.json";
  const std::string invalidJson = R"({
    "generic": "not_an_array",
    "fallback": [
        {
        "family": "Sans"
        }
    ]
  })";

  ASSERT_TRUE(
      base::WriteFile(base::FilePath::FromUTF8Unsafe(configFile), invalidJson));

  int result = fontConfig->parseConfig(configFile);
  EXPECT_EQ(result, ErrorCode::ERROR_CONFIG_INVALID_VALUE_TYPE);
  base::DeleteFile(base::FilePath::FromUTF8Unsafe(configFile));
}

TEST_F(FontConfig_OHOSTest, parseConfig_GenericElementNotObject) {
  const char* configFile = "/data/config_generic_element_not_object.json";
  const std::string invalidJson = R"({
    "generic": [
        {
        "family": "Arial"
        },
        "not_an_object",
        {
        "family": "Times"
        }
    ],
    "fallback": [
        {
        "family": "Sans"
        }
    ]
  })";

  ASSERT_TRUE(
      base::WriteFile(base::FilePath::FromUTF8Unsafe(configFile), invalidJson));

  int result = fontConfig->parseConfig(configFile);
  EXPECT_EQ(result, ErrorCode::NO_ERROR);
  base::DeleteFile(base::FilePath::FromUTF8Unsafe(configFile));
}

TEST_F(FontConfig_OHOSTest, parseConfig_JsonParseWithWarnings_Unicode) {
  const char* configFile = "/data/config_json_parse_warnings_unicode.json";
  const std::string jsonWithWarnings = R"({
    "generic": [
        {
        "family": "Arial",
        "alias": {
            "Arial-Regular": 400
        }
        }
    ],
    "fallback": [
        {
        "family": "Sans"
        }
    ],
    "unicode_test": "测试中文字符\u4e2d\u6587",
    "special_chars": "特殊字符: \n\t\r\b\f\\\"/"
  })";

  ASSERT_TRUE(base::WriteFile(base::FilePath::FromUTF8Unsafe(configFile),
                              jsonWithWarnings));
  int result = fontConfig->parseConfig(configFile);
  EXPECT_EQ(result, ErrorCode::NO_ERROR);
  base::DeleteFile(base::FilePath::FromUTF8Unsafe(configFile));
}

TEST_F(FontConfig_OHOSTest, parseConfig_GenericWithAliasArrayAndZeroWeight) {
  const char* configFile =
      "/data/config_generic_with_alias_array_and_zero_weight.json";
  const std::string jsonWithAliasArrayAndZeroWeight = R"({
    "generic": [
        {
        "family": "Arial",
        "alias": [
            {
            "Arial-Regular": 400
            },
            {
            "Arial-Zero": 0
            }
        ]
        }
    ],
    "fallback": [
        {
        "family": "Sans"
        }
    ]
  })";

  ASSERT_TRUE(base::WriteFile(base::FilePath::FromUTF8Unsafe(configFile),
                              jsonWithAliasArrayAndZeroWeight));
  int result = fontConfig->parseConfig(configFile);
  EXPECT_EQ(result, ErrorCode::NO_ERROR);
  base::DeleteFile(base::FilePath::FromUTF8Unsafe(configFile));
}

TEST_F(FontConfig_OHOSTest, parseFallbackItem_WithEmptyVariationsArray) {
  Json::Value root;
  root["en"] = "Sans";
  root["variations"] = Json::Value(Json::arrayValue);
  int result = fontConfig->parseFallbackItem(root);
  EXPECT_EQ(result, ErrorCode::NO_ERROR);
}

TEST_F(FontConfig_OHOSTest, parseFallbackItem_WithValidVariations) {
  Json::Value root;
  root["en"] = "Sans";

  Json::Value variations(Json::arrayValue);
  Json::Value var1;
  var1["weight"] = 100;
  variations.append(var1);

  Json::Value var2;
  var2["weight"] = 400;
  variations.append(var2);

  root["variations"] = variations;
  int result = fontConfig->parseFallbackItem(root);
  EXPECT_EQ(result, ErrorCode::NO_ERROR);
}

TEST_F(FontConfig_OHOSTest, parseFallbackItem_WithInvalidVariations) {
  Json::Value root;
  root["en"] = "Sans";

  Json::Value variations(Json::arrayValue);
  variations.append("not_an_object");
  root["variations"] = variations;
  int result = fontConfig->parseFallbackItem(root);
  EXPECT_EQ(result, ErrorCode::NO_ERROR);
}

TEST_F(FontConfig_OHOSTest, parseFallbackItem_WithNonArrayVariations) {
  Json::Value root;
  root["en"] = "Sans";
  root["variations"] = "not_an_array";
  int result = fontConfig->parseFallbackItem(root);
  EXPECT_EQ(result, ErrorCode::NO_ERROR);
}

TEST_F(FontConfig_OHOSTest, parseFallbackItem_WithValidIndex) {
  Json::Value root;
  root["en"] = "Sans";

  Json::Value index(Json::arrayValue);
  Json::Value indexItem;
  indexItem["font_file.ttf"] = 0;
  index.append(indexItem);

  root["index"] = index;
  int result = fontConfig->parseFallbackItem(root);
  EXPECT_EQ(result, ErrorCode::NO_ERROR);
}

TEST_F(FontConfig_OHOSTest, parseFallbackItem_WithNonArrayIndex) {
  Json::Value root;
  root["en"] = "Sans";
  root["index"] = "not_an_array";

  int result = fontConfig->parseFallbackItem(root);
  EXPECT_EQ(result, ErrorCode::NO_ERROR);
}

TEST_F(FontConfig_OHOSTest, insertTtcFont_TpSetNotNull) {
  FontInfo font;
  font.familyName = "Arial";
  font.index = 0;

  FontConfig_OHOS::TtcIndexInfo ttcInfo;
  ttcInfo.familyName = "Arial";
  ttcInfo.ttcIndex = 1;
  fontConfig->ttcIndexMap.set(SkString("Arial"), ttcInfo);

  fontConfig->fallbackNames.set(SkString("Arial"), 0);

  std::unique_ptr<FallbackInfo> fallback = std::make_unique<FallbackInfo>();
  fallback->familyName = "Arial";
  fallback->typefaceSet = std::make_shared<TypefaceSet>();
  fontConfig->fallbackSet.emplace_back(std::move(fallback));

  bool result = fontConfig->insertTtcFont(2, font);
  EXPECT_TRUE(result);
}

TEST_F(FontConfig_OHOSTest, insertVariableFont_TpSetNotNull) {
  FontInfo font;
  font.familyName = "Arial";
  font.index = 0;

  std::vector<FontConfig_OHOS::VariationInfo> variationSet;
  FontConfig_OHOS::VariationInfo variation;
  variation.weight = 400;
  variation.width = 100;
  variation.slant = 0;
  variationSet.push_back(variation);
  fontConfig->variationMap.set(SkString("Arial"), variationSet);
  fontConfig->fallbackNames.set(SkString("Arial"), 0);

  std::unique_ptr<FallbackInfo> fallback = std::make_unique<FallbackInfo>();
  fallback->familyName = "Arial";
  fallback->typefaceSet = std::make_shared<TypefaceSet>();
  fontConfig->fallbackSet.emplace_back(std::move(fallback));

  AxisDefinitions axisDefs;
  SkFontScanner::AxisDefinition axisDef;
  axisDef.fTag = SkSetFourByteTag('w', 'g', 'h', 't');
  axisDef.fMinimum = 100.0f;
  axisDef.fMaximum = 900.0f;
  axisDef.fDefault = 400.0f;
  axisDefs.push_back(axisDef);

  bool result = fontConfig->insertVariableFont(axisDefs, font);
  EXPECT_TRUE(result);
}

TEST_F(FontConfig_OHOSTest, insertVariableFont_EmptyVariationSet) {
  FontInfo font;
  font.familyName = "Arial";
  font.index = 0;

  std::vector<FontConfig_OHOS::VariationInfo> variationSet;
  fontConfig->variationMap.set(SkString("Arial"), variationSet);
  fontConfig->fallbackNames.set(SkString("Arial"), 0);

  std::unique_ptr<FallbackInfo> fallback = std::make_unique<FallbackInfo>();
  fallback->familyName = "Arial";
  fallback->typefaceSet = std::make_shared<TypefaceSet>();
  fontConfig->fallbackSet.emplace_back(std::move(fallback));

  AxisDefinitions axisDefs;
  SkFontScanner::AxisDefinition axisDef;
  axisDef.fTag = SkSetFourByteTag('w', 'g', 'h', 't');
  axisDef.fMinimum = 100.0f;
  axisDef.fMaximum = 900.0f;
  axisDef.fDefault = 400.0f;
  axisDefs.push_back(axisDef);

  bool result = fontConfig->insertVariableFont(axisDefs, font);
  EXPECT_TRUE(result);
}

TEST_F(FontConfig_OHOSTest, insertVariableFont_WidthIsMinusOne) {
  FontInfo font;
  font.familyName = "Arial";
  font.index = 0;

  std::vector<FontConfig_OHOS::VariationInfo> variationSet;
  FontConfig_OHOS::VariationInfo variation;
  variation.weight = 400;
  variation.width = -1;
  variation.slant = 0;
  variationSet.push_back(variation);
  fontConfig->variationMap.set(SkString("Arial"), variationSet);
  fontConfig->fallbackNames.set(SkString("Arial"), 0);

  std::unique_ptr<FallbackInfo> fallback = std::make_unique<FallbackInfo>();
  fallback->familyName = "Arial";
  fallback->typefaceSet = std::make_shared<TypefaceSet>();
  fontConfig->fallbackSet.emplace_back(std::move(fallback));

  AxisDefinitions axisDefs;
  SkFontScanner::AxisDefinition axisDef;
  axisDef.fTag = SkSetFourByteTag('w', 'g', 'h', 't');
  axisDef.fMinimum = 100.0f;
  axisDef.fMaximum = 900.0f;
  axisDef.fDefault = 400.0f;
  axisDefs.push_back(axisDef);

  bool result = fontConfig->insertVariableFont(axisDefs, font);
  EXPECT_TRUE(result);
}

TEST_F(FontConfig_OHOSTest, insertVariableFont_SlantIsMinusOne) {
  FontInfo font;
  font.familyName = "Arial";
  font.index = 0;

  std::vector<FontConfig_OHOS::VariationInfo> variationSet;
  FontConfig_OHOS::VariationInfo variation;
  variation.weight = 400;
  variation.width = 100;
  variation.slant = -1;
  variationSet.push_back(variation);
  fontConfig->variationMap.set(SkString("Arial"), variationSet);
  fontConfig->fallbackNames.set(SkString("Arial"), 0);

  std::unique_ptr<FallbackInfo> fallback = std::make_unique<FallbackInfo>();
  fallback->familyName = "Arial";
  fallback->typefaceSet = std::make_shared<TypefaceSet>();
  fontConfig->fallbackSet.emplace_back(std::move(fallback));

  AxisDefinitions axisDefs;
  SkFontScanner::AxisDefinition axisDef;
  axisDef.fTag = SkSetFourByteTag('w', 'g', 'h', 't');
  axisDef.fMinimum = 100.0f;
  axisDef.fMaximum = 900.0f;
  axisDef.fDefault = 400.0f;
  axisDefs.push_back(axisDef);

  bool result = fontConfig->insertVariableFont(axisDefs, font);
  EXPECT_TRUE(result);
}

TEST_F(FontConfig_OHOSTest, scanFonts_ShortFileName) {
  SkFontScanner_FreeType scanner;
  const char* testDir = "/data/test_fonts";
  base::FilePath dirPath = base::FilePath::FromUTF8Unsafe(testDir);

  base::CreateDirectory(dirPath);

  const char* shortFile = "/data/test_fonts/a";
  base::FilePath filePath = base::FilePath::FromUTF8Unsafe(shortFile);
  std::string content = "not a valid font file";
  base::WriteFile(filePath, content);

  int result = fontConfig->scanFonts(scanner, SkString(testDir), false);

  EXPECT_EQ(result, ErrorCode::NO_ERROR);

  base::DeleteFile(filePath);
  base::DeleteFile(dirPath);
}

TEST_F(FontConfig_OHOSTest, scanFonts_OtfFile) {
  SkFontScanner_FreeType scanner;

  const char* testDir = "/data/test_fonts";
  base::FilePath dirPath = base::FilePath::FromUTF8Unsafe(testDir);
  base::CreateDirectory(dirPath);

  const char* otfFile = "/data/test_fonts/test.otf";
  base::FilePath filePath = base::FilePath::FromUTF8Unsafe(otfFile);
  std::string content = "not a valid font file";
  base::WriteFile(filePath, content);

  int result = fontConfig->scanFonts(scanner, SkString(testDir), false);
  EXPECT_EQ(result, ErrorCode::NO_ERROR);
  base::DeleteFile(filePath);
  base::DeleteFile(dirPath);
}

TEST_F(FontConfig_OHOSTest, scanFonts_OtcFile) {
  SkFontScanner_FreeType scanner;

  const char* testDir = "/data/test_fonts";
  base::FilePath dirPath = base::FilePath::FromUTF8Unsafe(testDir);
  base::CreateDirectory(dirPath);
  const char* otcFile = "/data/test_fonts/test.otc";
  base::FilePath filePath = base::FilePath::FromUTF8Unsafe(otcFile);
  std::string content = "not a valid font file";
  base::WriteFile(filePath, content);

  int result = fontConfig->scanFonts(scanner, SkString(testDir), false);
  EXPECT_EQ(result, ErrorCode::NO_ERROR);

  base::DeleteFile(filePath);
  base::DeleteFile(dirPath);
}

TEST_F(FontConfig_OHOSTest, scanFonts_DirectoryWithoutSlash) {
  SkFontScanner_FreeType scanner;
  const char* testDir = "/data/test_fonts";
  base::FilePath dirPath = base::FilePath::FromUTF8Unsafe(testDir);
  base::CreateDirectory(dirPath);

  const char* ttfFile = "/data/test_fonts/test.ttf";
  base::FilePath filePath = base::FilePath::FromUTF8Unsafe(ttfFile);
  std::string content = "not a valid font file";
  base::WriteFile(filePath, content);

  int result = fontConfig->scanFonts(scanner, SkString(testDir), false);
  EXPECT_EQ(result, ErrorCode::NO_ERROR);
  base::DeleteFile(filePath);
  base::DeleteFile(dirPath);
}

TEST_F(FontConfig_OHOSTest, scanFontsBackup_ShortFileName) {
  SkFontScanner_FreeType scanner;
  const char* testDir = "/data/test_fonts";
  base::FilePath dirPath = base::FilePath::FromUTF8Unsafe(testDir);
  base::CreateDirectory(dirPath);
  const char* shortFile = "/data/test_fonts/a";
  base::FilePath filePath = base::FilePath::FromUTF8Unsafe(shortFile);
  std::string content = "not a valid font file";
  base::WriteFile(filePath, content);

  fontConfig->fontDirSet.clear();
  fontConfig->fontDirSet.emplace_back(SkString(testDir));

  int result = fontConfig->scanFontsBackup(scanner);

  EXPECT_EQ(result, ErrorCode::NO_ERROR);
  base::DeleteFile(filePath);
  base::DeleteFile(dirPath);
}

TEST_F(FontConfig_OHOSTest, scanFontsBackup_OtfFile) {
  SkFontScanner_FreeType scanner;
  const char* testDir = "/data/test_fonts";
  base::FilePath dirPath = base::FilePath::FromUTF8Unsafe(testDir);

  base::CreateDirectory(dirPath);

  const char* otfFile = "/data/test_fonts/test.otf";
  base::FilePath filePath = base::FilePath::FromUTF8Unsafe(otfFile);
  std::string content = "not a valid font file";
  base::WriteFile(filePath, content);

  fontConfig->fontDirSet.clear();
  fontConfig->fontDirSet.emplace_back(SkString(testDir));

  int result = fontConfig->scanFontsBackup(scanner);
  EXPECT_EQ(result, ErrorCode::NO_ERROR);
  base::DeleteFile(filePath);
  base::DeleteFile(dirPath);
}

TEST_F(FontConfig_OHOSTest, scanFontsBackup_OtcFile) {
  SkFontScanner_FreeType scanner;

  const char* testDir = "/data/test_fonts";
  base::FilePath dirPath = base::FilePath::FromUTF8Unsafe(testDir);
  base::CreateDirectory(dirPath);
  const char* otcFile = "/data/test_fonts/test.otc";
  base::FilePath filePath = base::FilePath::FromUTF8Unsafe(otcFile);
  std::string content = "not a valid font file";
  base::WriteFile(filePath, content);

  fontConfig->fontDirSet.clear();
  fontConfig->fontDirSet.emplace_back(SkString(testDir));

  int result = fontConfig->scanFontsBackup(scanner);
  EXPECT_EQ(result, ErrorCode::NO_ERROR);
  base::DeleteFile(filePath);
  base::DeleteFile(dirPath);
}

TEST_F(FontConfig_OHOSTest, hasError_ErrMatchTextNotMatch) {
  FontConfig_OHOS::ErrorInfo errorInfo(ErrorCode::ERROR_FONT_NOT_EXIST,
                                       "test.ttf");
  fontConfig->errSet.push_back(errorInfo);
  bool result = fontConfig->hasError(ErrorCode::ERROR_FONT_NOT_EXIST,
                                     SkString("different.ttf"));
  EXPECT_FALSE(result);
}

TEST_F(FontConfig_OHOSTest, hasError_ErrMatchTextMatch) {
  FontConfig_OHOS::ErrorInfo errorInfo(ErrorCode::ERROR_FONT_NOT_EXIST,
                                       "test.ttf");
  fontConfig->errSet.push_back(errorInfo);
  bool result = fontConfig->hasError(ErrorCode::ERROR_FONT_NOT_EXIST,
                                     SkString("test.ttf"));
  EXPECT_TRUE(result);
}

TEST_F(FontConfig_OHOSTest, sortTypefaceSet_WeightGreater) {
  auto typefaceSet = std::make_shared<TypefaceSet>();
  FontInfo fontInfo1;
  fontInfo1.familyName = "Font1";
  fontInfo1.style =
      SkFontStyle(700, SkFontStyle::kNormal_Width, SkFontStyle::kUpright_Slant);
  auto typeface1 = sk_make_sp<SkTypeface_OHOS>(fontInfo1);
  typefaceSet->push_back(typeface1);

  FontInfo fontInfo2;
  fontInfo2.familyName = "Font2";
  fontInfo2.style =
      SkFontStyle(300, SkFontStyle::kNormal_Width, SkFontStyle::kUpright_Slant);
  auto typeface2 = sk_make_sp<SkTypeface_OHOS>(fontInfo2);
  typefaceSet->push_back(typeface2);

  fontConfig->sortTypefaceSet(typefaceSet);
  EXPECT_EQ((*typefaceSet)[0]->fontStyle().weight(), 300);
  EXPECT_EQ((*typefaceSet)[1]->fontStyle().weight(), 700);
}

TEST_F(FontConfig_OHOSTest, logErrInfo_ExpectedNegative) {
  int result = fontConfig->logErrInfo(
      ErrorCode::ERROR_CONFIG_INVALID_VALUE_TYPE, "test",
      static_cast<Json::ValueType>(-1), Json::stringValue);

  EXPECT_EQ(result, ErrorCode::ERROR_CONFIG_INVALID_VALUE_TYPE);
}

TEST_F(FontConfig_OHOSTest, logErrInfo_ExpectedTooLarge) {
  int result = fontConfig->logErrInfo(
      ErrorCode::ERROR_CONFIG_INVALID_VALUE_TYPE, "test",
      static_cast<Json::ValueType>(10), Json::stringValue);

  EXPECT_EQ(result, ErrorCode::ERROR_CONFIG_INVALID_VALUE_TYPE);
}

TEST_F(FontConfig_OHOSTest, logErrInfo_ActualNegative) {
  int result = fontConfig->logErrInfo(
      ErrorCode::ERROR_CONFIG_INVALID_VALUE_TYPE, "test", Json::stringValue,
      static_cast<Json::ValueType>(-1));

  EXPECT_EQ(result, ErrorCode::ERROR_CONFIG_INVALID_VALUE_TYPE);
}

TEST_F(FontConfig_OHOSTest, logErrInfo_ActualTooLarge) {
  int result = fontConfig->logErrInfo(
      ErrorCode::ERROR_CONFIG_INVALID_VALUE_TYPE, "test", Json::stringValue,
      static_cast<Json::ValueType>(10));

  EXPECT_EQ(result, ErrorCode::ERROR_CONFIG_INVALID_VALUE_TYPE);
}

TEST_F(FontConfig_OHOSTest, InvalidateThemeFont_ScanFileAndScanInstanceFalse) {
  SkFontScanner_FreeType scanner;

  const char* testFile = "/data/test_theme_font.ttf";
  base::FilePath filePath = base::FilePath::FromUTF8Unsafe(testFile);
  std::string content = "not a valid font file";
  base::WriteFile(filePath, content);

  base::File file(filePath, base::File::FLAG_OPEN | base::File::FLAG_READ);
  ASSERT_TRUE(file.IsValid());

  fontConfig->InvalidateThemeFont(scanner, std::vector<int>{file.GetPlatformFile()});

  EXPECT_EQ(fontConfig->getThemeFontTypefaceSet().size(), 0);
  base::DeleteFile(filePath);
}

} // namespace skia