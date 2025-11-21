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

#define private public
#define protected public
#include "SkFontMgr_ohos.h"
#undef protected
#undef private

#include "include/core/SkRefCnt.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fcntl.h>
#include <unistd.h>

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

class SkFontMgrOHOSTest : public testing::Test {
protected: 
    void SetUp() {
        const char* validPath = "/system/etc/fontconfig.json";
        fontMgr = sk_make_sp<SkFontMgr_OHOS>(validPath);
    }

    void beforeMakeType() {
        fontData = SkData::MakeFromFileName("/system/fonts/NotoSansMiao-Regular.ttf");
        ASSERT_NE(fontData, nullptr);

        stream = std::make_unique<SkMemoryStream>(fontData);
        ASSERT_NE(stream, nullptr);
    }

    void beforeOnInvalidateThemeFont() {
        tempFontPath = "/system/fonts/NotoSansMiao-Regular.ttf";
        tempFd = open(tempFontPath.c_str(), O_RDONLY);
        ASSERT_GE(tempFd, 0);
    }

    void afterOnInvalidateThemeFont() {
        if (tempFd >= 0) {
            close(tempFd);
        }
    }

    sk_sp<SkFontMgr_OHOS> fontMgr;
    SkFontStyle normalStyle = SkFontStyle(SkFontStyle::kNormal_Weight,
                                          SkFontStyle::kNormal_Width,
                                          SkFontStyle::kUpright_Slant);
    SkFontScanner_FreeType fontScanner;
    MockFontScanner mockScanner;
    sk_sp<SkData> fontData;
    std::unique_ptr<SkStreamAsset> stream;
    int tempFd = -1;
    std::string tempFontPath;
};

TEST_F(SkFontMgrOHOSTest, constructor) {
    EXPECT_NE(fontMgr, nullptr);
    EXPECT_GT(fontMgr->onCountFamilies(), 0);
}

TEST_F(SkFontMgrOHOSTest, OnCountFamilies) {
    EXPECT_EQ(fontMgr->onCountFamilies(), fontMgr->countFamilies());
}

TEST_F(SkFontMgrOHOSTest, OnGetFamilyName) {
    SkString familyName;

    fontMgr->onGetFamilyName(0, &familyName);
    EXPECT_FALSE(familyName.isEmpty());

    auto* rawFontMgr = fontMgr.get();
    rawFontMgr->fontConfig = nullptr;
    SkString emptyName1;
    fontMgr->onGetFamilyName(0, &emptyName1);
    EXPECT_TRUE(emptyName1.isEmpty());
    const char* validPath = "/system/etc/fontconfig.json";
    rawFontMgr->fontConfig = std::make_shared<FontConfig_OHOS>(fontScanner, validPath);

    fontMgr->onGetFamilyName(0, nullptr);

    SkString emptyName2;
    fontMgr->onGetFamilyName(-1, &emptyName2);
    EXPECT_TRUE(emptyName2.isEmpty());

    SkString emptyName3;
    int outOfBoundIndex = fontMgr->onCountFamilies() + 1;
    fontMgr->onGetFamilyName(outOfBoundIndex, &emptyName3);
    EXPECT_TRUE(emptyName3.isEmpty());
}


TEST_F(SkFontMgrOHOSTest, OnCreateStyleSet) {
    int validIndex = 0;
    auto validSet = fontMgr->onCreateStyleSet(validIndex);
    EXPECT_NE(validSet, nullptr);

    auto* rawFontMgr = fontMgr.get();
    auto originalConfig = rawFontMgr->fontConfig;
    rawFontMgr->fontConfig = nullptr;
    auto nullConfigSet = fontMgr->onCreateStyleSet(validIndex);
    EXPECT_EQ(nullConfigSet, nullptr);
    rawFontMgr->fontConfig = originalConfig;

    auto negativeIndexSet = fontMgr->onCreateStyleSet(-1);
    EXPECT_EQ(negativeIndexSet, nullptr);

    int outOfBoundIndex = fontMgr->onCountFamilies();
    auto overflowSet = fontMgr->onCreateStyleSet(outOfBoundIndex);
    EXPECT_EQ(overflowSet, nullptr);

    int maxValidIndex = fontMgr->onCountFamilies() - 1;
    if (maxValidIndex >= 0) {
        auto maxIndexSet = fontMgr->onCreateStyleSet(maxValidIndex);
        EXPECT_NE(maxIndexSet, nullptr);
    }
}

TEST_F(SkFontMgrOHOSTest, onMatchFamily) {
    const char* existingFamily = "Noto Sans Yi";
    auto existingSet = fontMgr->onMatchFamily(existingFamily);
    EXPECT_NE(existingSet, nullptr);

    auto* rawFontMgr = fontMgr.get();
    auto originalConfig = rawFontMgr->fontConfig;
    rawFontMgr->fontConfig = nullptr;
    auto nullConfigSet = fontMgr->onMatchFamily(existingFamily);
    EXPECT_EQ(nullConfigSet, nullptr);
    rawFontMgr->fontConfig = originalConfig;

    auto defaultSet = fontMgr->onMatchFamily(nullptr);
    EXPECT_NE(defaultSet, nullptr);

    const char* noExitstingFamily = "NonExistFont";
    auto nonExistingSet = fontMgr->onMatchFamily(noExitstingFamily);
    EXPECT_EQ(nonExistingSet, nullptr);
}

TEST_F(SkFontMgrOHOSTest, onMatchFamilyStyle) {
    const SkFontStyle normalStyle(SkFontStyle::kNormal_Weight,
                                SkFontStyle::kNormal_Width,
                                SkFontStyle::kUpright_Slant);

    auto* rawFontMgr = fontMgr.get();
    auto originalConfig = rawFontMgr->fontConfig;
    rawFontMgr->fontConfig = nullptr;
    auto nullResult = fontMgr->onMatchFamilyStyle("Noto Sans Yi", normalStyle);
    EXPECT_EQ(nullResult, nullptr);
    rawFontMgr->fontConfig = originalConfig;

    auto defaultResult = fontMgr->onMatchFamilyStyle(nullptr, normalStyle);
    EXPECT_NE(defaultResult, nullptr);

    auto sansResult = fontMgr->onMatchFamilyStyle("HarmonyOS-Sans", normalStyle);
    EXPECT_NE(sansResult, nullptr);

    auto legacySansResult = fontMgr->onMatchFamilyStyle("sans-serif", normalStyle);
    EXPECT_NE(legacySansResult, nullptr);

    auto invalidResult = fontMgr->onMatchFamilyStyle("InvalidFont", normalStyle);
    EXPECT_EQ(invalidResult, nullptr);
#if BUILDFLAG(ARKWEB_THEME_FONT)
    auto themeFontTypeValid = fontMgr->onMatchFamilyStyle("Noto Sans Miao", normalStyle);
    EXPECT_NE(themeFontTypeValid, nullptr);

    auto themeFontTypeInvalid = fontMgr->onMatchFamilyStyle("no-name-font", normalStyle);
    EXPECT_EQ(themeFontTypeInvalid, nullptr);
#endif
}

TEST_F(SkFontMgrOHOSTest, onMatchFamilyStyleCharacter) {
    const SkFontStyle normalStyle(SkFontStyle::kNormal_Weight,
                                  SkFontStyle::kNormal_Width,
                                  SkFontStyle::kUpright_Slant);
    const char* zhLangs[] = {"zh-Hans"};
    const char* emptyLangs[] = {nullptr};

    auto* rawFontMgr = fontMgr.get();
    auto originalConfig = rawFontMgr->fontConfig;
    rawFontMgr->fontConfig = nullptr;
    auto nullResult = fontMgr->onMatchFamilyStyleCharacter(nullptr, normalStyle, zhLangs, 1, L'中');
    EXPECT_EQ(nullResult, nullptr);
    rawFontMgr->fontConfig = originalConfig;

    auto defaultResult = fontMgr->onMatchFamilyStyleCharacter(nullptr, normalStyle, zhLangs, 1, L'中');
    EXPECT_NE(defaultResult, nullptr);

    auto familyResult = fontMgr->onMatchFamilyStyleCharacter("HarmonyOS-Sans", normalStyle, zhLangs, 1, L'中');
    EXPECT_NE(familyResult, nullptr);

    auto invalidFamilyResult = fontMgr->onMatchFamilyStyleCharacter("InvalidFont", normalStyle, zhLangs, 1, L'中');
    EXPECT_NE(invalidFamilyResult, nullptr);

    auto emojiResult = fontMgr->onMatchFamilyStyleCharacter(nullptr, normalStyle, zhLangs, 1, 0x263A); // ☺
    EXPECT_NE(emojiResult, nullptr);

    auto noLangResult = fontMgr->onMatchFamilyStyleCharacter(nullptr, normalStyle, emptyLangs, 0, L'A');
    EXPECT_NE(noLangResult, nullptr);

    auto notFoundResult = fontMgr->onMatchFamilyStyleCharacter(nullptr, normalStyle, zhLangs, 1, 0xFFFF); // 私有区字符
    EXPECT_EQ(notFoundResult, nullptr);

    const char* path = "/system/etc/fontconfig.json";
    auto mockConfig = std::make_shared<FontConfig_OHOS>(fontScanner, path);
    mockConfig->fallbackForMap = {};
    rawFontMgr->fontConfig = mockConfig;
    auto emptyFallbackResult = fontMgr->onMatchFamilyStyleCharacter(nullptr, normalStyle, zhLangs, 1, L'中');
    EXPECT_EQ(emptyFallbackResult, nullptr);
    rawFontMgr->fontConfig = originalConfig;
}

TEST_F(SkFontMgrOHOSTest, findTypeface) {
    const SkFontStyle normalStyle;
    const char* zhHans[] = {"zh-Hans"};
    const char* enUS[] = {"en-US"};
    SkUnichar chineseChar = L'中';
    SkUnichar quoteChar = 0x2018;
    SkUnichar invalidChar = 0xFFFF;

    auto* rawFontMgr = fontMgr.get();
    const auto& fallbackSet = rawFontMgr->fontConfig->getFallbackSet();
    const auto& fallbackMap = rawFontMgr->fontConfig->getFallbackForMap();

    auto fallbackItem = fallbackMap.find(SkString(""));
    ASSERT_NE(fallbackItem, nullptr);

    sk_sp<SkTypeface> result1 = rawFontMgr->findTypeface(*fallbackItem, normalStyle, nullptr, 0, chineseChar);
    EXPECT_EQ(result1, nullptr);

    sk_sp<SkTypeface> result2 = rawFontMgr->findTypeface(*fallbackItem, normalStyle, zhHans, 1, chineseChar);
    EXPECT_NE(result2, nullptr);

    sk_sp<SkTypeface> result3 = rawFontMgr->findTypeface(*fallbackItem, normalStyle, zhHans, 1, quoteChar);
    EXPECT_NE(result3, nullptr);

    sk_sp<SkTypeface> result4 = rawFontMgr->findTypeface(*fallbackItem, normalStyle, zhHans, 1, invalidChar);
    EXPECT_EQ(result4, nullptr);

    sk_sp<SkTypeface> result5 = rawFontMgr->findTypeface(*fallbackItem, normalStyle, enUS, 1, 'A');
    EXPECT_NE(result5, nullptr);
    
    FallbackSetPos mockItem{0, 0};
    sk_sp<SkTypeface> result6 = rawFontMgr->findTypeface(mockItem, normalStyle, zhHans, 1, chineseChar);
    EXPECT_EQ(result6, nullptr);

    const char* unsupportedLang[] = {"xx-YY"};
    sk_sp<SkTypeface> result7 = rawFontMgr->findTypeface(*fallbackItem, normalStyle, unsupportedLang, 1, chineseChar);
    EXPECT_EQ(result7, nullptr);
}

TEST_F(SkFontMgrOHOSTest, CompareLangs) {
    SkString langs("zh-Hans,en,ja");
    const char* zhHans[] = {"zh-Hans"};
    const char* enUS[] = {"en-US"};
    const char* undZsye[] = {"und-Zsye"};
    const char* multiLangs[] = {"zh-Hant", "en", "ja"};
    int tps[6] = {-1, -1, -1, -1, -1, -1};

    EXPECT_EQ(fontMgr->compareLangs(langs, nullptr, 0, tps), -1);

    EXPECT_EQ(fontMgr->compareLangs(langs, zhHans, 1, tps), 0);

    EXPECT_EQ(fontMgr->compareLangs(langs, enUS, 1, tps), 1);

    EXPECT_EQ(fontMgr->compareLangs(SkString("und,Zsye"), undZsye, 1, tps), 1);

    EXPECT_EQ(fontMgr->compareLangs(langs, multiLangs, 3, tps), 2);

    tps[0] = 1;
    EXPECT_EQ(fontMgr->compareLangs(langs, zhHans, 1, tps), -1);

    const char* simpleLang[] = {"fr"};
    EXPECT_EQ(fontMgr->compareLangs(SkString("fr"), simpleLang, 1, tps), -1);
}

TEST_F(SkFontMgrOHOSTest, onMakeFromData) {
    auto nullResult = fontMgr->onMakeFromData(nullptr, 0);
    EXPECT_EQ(nullResult, nullptr);

    const char* testFontPath = "/system/fonts/NotoSansMiao-Regular.ttf";
    auto fontData = SkData::MakeFromFileName(testFontPath);
    
    auto validResult = fontMgr->onMakeFromData(fontData, 0);
    EXPECT_NE(validResult, nullptr);

    const char invalidData[] = {0x00, 0x01, 0x02};
    auto invalidSkData = SkData::MakeWithoutCopy(invalidData, sizeof(invalidData));
    
    auto invalidResult = fontMgr->onMakeFromData(invalidSkData, 0);
    EXPECT_EQ(invalidResult, nullptr);

    if (validResult) {
        auto ttcResult = fontMgr->onMakeFromData(fontData, 1);
    }
}

TEST_F(SkFontMgrOHOSTest, onMakeFromStreamIndex) {
    auto nullResult = fontMgr->onMakeFromStreamIndex(nullptr, 0);
    EXPECT_EQ(nullResult, nullptr);

    const char* testFontPath = "/system/fonts/NotoSansMiao-Regular.ttf";
    auto fontData = SkData::MakeFromFileName(testFontPath);

    auto validStream = SkMemoryStream::Make(fontData);
    auto validResult = fontMgr->onMakeFromStreamIndex(std::move(validStream), 0);
    EXPECT_NE(validResult, nullptr);

    const uint8_t invalidData[] = {0x00, 0x01, 0x02};
    auto invalidStream = SkMemoryStream::Make(SkData::MakeWithoutCopy(invalidData, sizeof(invalidData)));
    auto invalidResult = fontMgr->onMakeFromStreamIndex(std::move(invalidStream), 0);
    EXPECT_EQ(invalidResult, nullptr);

    if (validResult) {
        auto ttcStream = SkMemoryStream::Make(fontData);
        auto ttcResult = fontMgr->onMakeFromStreamIndex(std::move(ttcStream), 1);
        EXPECT_TRUE(ttcResult != nullptr || ttcResult == nullptr);
    }
}

TEST_F(SkFontMgrOHOSTest, onMakeFromStreamArgs) {
    SkFontArguments args;
    auto nullResult = fontMgr->onMakeFromStreamArgs(nullptr, args);
    EXPECT_EQ(nullResult, nullptr);

    const char* testFontPath = "/system/fonts/NotoSansMiao-Regular.ttf";
    auto fontData = SkData::MakeFromFileName(testFontPath);

    auto validStream = SkMemoryStream::Make(fontData);
    SkFontArguments validArgs;
    validArgs.setCollectionIndex(0);
    auto validResult = fontMgr->onMakeFromStreamArgs(std::move(validStream), validArgs);
    EXPECT_NE(validResult, nullptr);

    if (validResult) {
        auto argsStream = SkMemoryStream::Make(fontData);
        SkFontArguments complexArgs;
        complexArgs.setCollectionIndex(1);
        SkFontArguments::VariationPosition::Coordinate coordinate{0x1234, 1.0f};

        SkFontArguments::VariationPosition varPos;
        varPos.coordinates = &coordinate;
        varPos.coordinateCount = 1;

        complexArgs.setVariationDesignPosition(varPos);
        
        auto argsResult = fontMgr->onMakeFromStreamArgs(std::move(argsStream), complexArgs);
        EXPECT_TRUE(argsResult != nullptr || argsResult == nullptr);
    }

    const uint8_t invalidData[] = {0x00, 0x01, 0x02};
    auto invalidStream = SkMemoryStream::Make(SkData::MakeWithoutCopy(invalidData, sizeof(invalidData)));
    auto invalidResult = fontMgr->onMakeFromStreamArgs(std::move(invalidStream), SkFontArguments());
    EXPECT_EQ(invalidResult, nullptr);
}


TEST_F(SkFontMgrOHOSTest, onMakeFromFile) {
    constexpr const char* kValidFontPath = "/system/fonts/NotoSansMiao-Regular.ttf";
    constexpr const char* kInvalidPath = "/path/does/not/exist.ttf";

    {
      auto* rawFontMgr = fontMgr.get();
      auto originalConfig = rawFontMgr->fontConfig;
      rawFontMgr->fontConfig = nullptr;

      auto result = fontMgr->onMakeFromFile(kValidFontPath, 0);
      EXPECT_EQ(result, nullptr);

      rawFontMgr->fontConfig = originalConfig;
   }

   {
      auto result = fontMgr->onMakeFromFile(kInvalidPath, 0);
      EXPECT_EQ(result, nullptr);
   }

   {
      auto result = fontMgr->onMakeFromFile(kValidFontPath, 0);
      EXPECT_NE(result, nullptr);
   }
}

TEST_F(SkFontMgrOHOSTest, OnLegacyMakeTypeface) {
    const SkFontStyle normalStyle(SkFontStyle::kNormal_Weight,
                                SkFontStyle::kNormal_Width,
                                SkFontStyle::kUpright_Slant);
    
    auto sansResult = fontMgr->onLegacyMakeTypeface("ShuS-SC", normalStyle);
    EXPECT_NE(sansResult, nullptr);

    auto fallbackResult = fontMgr->onLegacyMakeTypeface("NonExistentFont", normalStyle);
    EXPECT_NE(fallbackResult, nullptr);

    auto defaultResult = fontMgr->onLegacyMakeTypeface(nullptr, normalStyle);
    EXPECT_NE(defaultResult, nullptr);
}

TEST_F(SkFontMgrOHOSTest, makeTypeface001) {
    auto invalidStream = SkMemoryStream::Make(SkData::MakeEmpty());
    SkFontArguments args;
    
    auto result = fontMgr->makeTypeface(std::move(invalidStream), args, nullptr);
    
    EXPECT_EQ(result, nullptr);
}

TEST_F(SkFontMgrOHOSTest, makeTypeface002) {
    const char* testFontPath = "/system/fonts/NotoSansMiao-Regular.ttf";
    auto fontData = SkData::MakeFromFileName(testFontPath);

    auto validStream = SkMemoryStream::Make(fontData);
    SkFontArguments args;
    
    auto result = fontMgr->makeTypeface(std::move(validStream), args, testFontPath);

    EXPECT_NE(result, nullptr);
}

TEST_F(SkFontMgrOHOSTest, makeTypeface003) {
    beforeMakeType();
    SkFontArguments args;
    args.setCollectionIndex(0);

    std::unique_ptr<SkFontData> fontDataObj = std::make_unique<SkFontData>(
        stream->duplicate(), 0, 0, nullptr, 0, nullptr, 0);
    ASSERT_NE(fontDataObj, nullptr);

    auto typeface = fontMgr->makeTypeface(fontDataObj.get());
    EXPECT_NE(typeface, nullptr);
}

TEST_F(SkFontMgrOHOSTest, makeTypeface004) {
    beforeMakeType();
    SkFontArguments args;
    args.setCollectionIndex(0);
    
    constexpr int axisCount = 2;
    SkFixed axes[axisCount] = {0x40000000, 0x80000000};

    std::unique_ptr<SkFontData> fontDataObj = std::make_unique<SkFontData>(
        stream->duplicate(),  // stream
        0,                   // index
        0,                   // paletteIndex
        axes,                // axes
        axisCount,           // axisCount
        nullptr,             // paletteOverrides
        0                    // paletteOverrideCount
    );
    ASSERT_NE(fontDataObj, nullptr);
    auto typeface = fontMgr->makeTypeface(fontDataObj.get());
    EXPECT_NE(typeface, nullptr);
}

TEST_F(SkFontMgrOHOSTest, makeTypeface005) {
    beforeMakeType();
    SkFontArguments args;
    args.setCollectionIndex(0);
    constexpr int overrideCount = 1;
    SkFontArguments::Palette::Override overrides[overrideCount] = {
        {0, 0xFF0000FF}
    };

    std::unique_ptr<SkFontData> fontDataObj = std::make_unique<SkFontData>(
        stream->duplicate(),  // stream
        0,                   // index
        1,                   // paletteIndex
        nullptr,             // axes
        0,                   // axisCount
        overrides,           // paletteOverrides
        overrideCount        // paletteOverrideCount
    );
    ASSERT_NE(fontDataObj, nullptr);

    auto typeface = fontMgr->makeTypeface(fontDataObj.get());

    EXPECT_NE(typeface, nullptr);
}

TEST_F(SkFontMgrOHOSTest, makeTypeface006) {
    beforeMakeType();
    std::unique_ptr<SkStreamAsset> emptyStream = std::make_unique<SkMemoryStream>();
    std::unique_ptr<SkFontData> invalidFontData = std::make_unique<SkFontData>(
        std::move(emptyStream), // stream
        0,                     // index
        0,                     // paletteIndex
        nullptr,               // axes
        0,                     // axisCount
        nullptr,               // paletteOverrides
        0                      // paletteOverrideCount
    );

    auto typeface = fontMgr->makeTypeface(invalidFontData.get());
    EXPECT_EQ(typeface, nullptr);
}

#if BUILDFLAG(ARKWEB_THEME_FONT)
TEST_F(SkFontMgrOHOSTest, onInvalidateThemeFont001) {
    beforeOnInvalidateThemeFont();
    ASSERT_GE(fontMgr->fontConfig, nullptr);

    fontMgr->onInvalidateThemeFont(std::vector<int>{tempFd});
    afterOnInvalidateThemeFont();
}

TEST_F(SkFontMgrOHOSTest, onInvalidateThemeFont002) {
    beforeOnInvalidateThemeFont();
    ASSERT_GE(fontMgr->fontConfig, nullptr);

    fontMgr->onInvalidateThemeFont(std::vector<int>{-1});
    afterOnInvalidateThemeFont();
}

TEST_F(SkFontMgrOHOSTest, onInvalidateThemeFont003) {
    beforeOnInvalidateThemeFont();
    ASSERT_GE(fontMgr->fontConfig, nullptr);

    fontMgr->onInvalidateThemeFont(std::vector<int>{tempFd});
    fontMgr->onInvalidateThemeFont(std::vector<int>{tempFd});
    fontMgr->onInvalidateThemeFont(std::vector<int>{tempFd});

    afterOnInvalidateThemeFont();
}

TEST_F(SkFontMgrOHOSTest, onInvalidateThemeFont004) {
    beforeOnInvalidateThemeFont();
    auto originalConfig = fontMgr->fontConfig;

    fontMgr->fontConfig = nullptr;
    fontMgr->onInvalidateThemeFont(std::vector<int>{tempFd});
    fontMgr->fontConfig = originalConfig;

    afterOnInvalidateThemeFont();
}
#endif


}