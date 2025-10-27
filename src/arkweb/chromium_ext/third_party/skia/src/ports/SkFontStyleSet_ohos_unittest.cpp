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
#include "SkFontStyleSet_ohos.h"
#undef private
#include <gtest/gtest.h>
#include "FontConfig_ohos.h"
#include <gmock/gmock.h>

#include "src/ports/SkFontScanner_FreeType_priv.h"

using testing::_;
using testing::Return;
using testing::NotNull;

namespace skia {

class MockFontConfig : public FontConfig_OHOS {
public:
    MockFontConfig(const SkFontScanner& scanner, const char* path) 
        : FontConfig_OHOS(scanner, path) {}
    MOCK_METHOD2(getTypefaceCount, int(int, bool));
    MOCK_METHOD(SkTypeface*, getTypeface, (int, int, bool), ());
    MOCK_METHOD(SkTypeface*, getTypeface, (int, const SkFontStyle&, bool), ());
};

class SkFontStyleSet_OHOSTest : public testing::Test {
protected:
    void SetUp() {
        mockConfig = std::make_shared<MockFontConfig>(fontScanner, nullptr);
        fontConfig = std::make_shared<FontConfig_OHOS>(fontScanner, nullptr);
    }

    std::shared_ptr<MockFontConfig> mockConfig;
    std::shared_ptr<FontConfig_OHOS> fontConfig;
    SkFontScanner_FreeType fontScanner;  // the scanner to parse a font file
};

TEST_F(SkFontStyleSet_OHOSTest, Constructor_WithNullConfig) {
    SkFontStyleSet_OHOS styleSet(nullptr, 0);
    EXPECT_EQ(styleSet.count(), -1);
}

TEST_F(SkFontStyleSet_OHOSTest, Constructor_WithValidConfig) {
    auto localMockConfig = std::make_shared<MockFontConfig>(fontScanner, nullptr);

    SkFontStyleSet_OHOS styleSet(localMockConfig, 1, false);
    testing::Mock::VerifyAndClearExpectations(localMockConfig.get());
    EXPECT_EQ(styleSet.count(), 0);
}

TEST_F(SkFontStyleSet_OHOSTest, Constructor001) {
    SkFontStyleSet_OHOS styleSet(nullptr, 0);
    EXPECT_EQ(styleSet.count(), -1);
}

TEST_F(SkFontStyleSet_OHOSTest, Constructor002) {
    int expectedCount = fontConfig->getTypefaceCount(0, false);
    
    SkFontStyleSet_OHOS styleSet(fontConfig, 0);
    EXPECT_EQ(styleSet.count(), expectedCount);
}

TEST_F(SkFontStyleSet_OHOSTest, Count) {
    SkFontStyleSet_OHOS styleSet(fontConfig, 0);
    EXPECT_EQ(styleSet.count(), fontConfig->getTypefaceCount(0, false));
    EXPECT_EQ(styleSet.count(), fontConfig->getTypefaceCount(0, false));
}

TEST_F(SkFontStyleSet_OHOSTest, GetStyle_WithInvalidIndex) {
    SkFontStyleSet_OHOS styleSet(fontConfig, 0);
    
    SkFontStyle style;
    SkString name;

    styleSet.getStyle(-1, &style, &name);

    styleSet.getStyle(999, &style, &name);
}

TEST_F(SkFontStyleSet_OHOSTest, GetStyle_WithValidIndex) {
    SkFontStyleSet_OHOS styleSet(fontConfig, 0);
    
    if (styleSet.count() > 0) {
        SkFontStyle style;
        SkString name;
        
        styleSet.getStyle(0, &style, &name);
        EXPECT_NE(name.size(), 0);
    }
}

TEST_F(SkFontStyleSet_OHOSTest, GetStyle_WithNullOutputs) {
    SkFontStyleSet_OHOS styleSet(fontConfig, 0);
    
    if (styleSet.count() > 0) {
        SkString name;
        styleSet.getStyle(0, nullptr, &name);
        SkFontStyle style;
        styleSet.getStyle(0, &style, nullptr);

        styleSet.getStyle(0, nullptr, nullptr);
    }
}

TEST_F(SkFontStyleSet_OHOSTest, CreateTypeface_WithInvalidIndex) {
    SkFontStyleSet_OHOS styleSet(fontConfig, 0);

    auto tf1 = styleSet.createTypeface(-1);
    EXPECT_EQ(tf1, nullptr);

    auto tf2 = styleSet.createTypeface(999);
    EXPECT_EQ(tf2, nullptr);
}

TEST_F(SkFontStyleSet_OHOSTest, CreateTypeface_WithValidIndex) {
    SkFontStyleSet_OHOS styleSet(fontConfig, 0);
    
    if (styleSet.count() > 0) {
        auto tf = styleSet.createTypeface(0);
        EXPECT_NE(tf, nullptr);

        SkString familyName;
        tf->getFamilyName(&familyName);
        EXPECT_FALSE(familyName.isEmpty());
        
        tf.reset();
    }
}

TEST_F(SkFontStyleSet_OHOSTest, MatchStyle_WithVariousPatterns) {
    SkFontStyleSet_OHOS styleSet(fontConfig, 0);

    SkFontStyle patterns[] = {
        SkFontStyle::Normal(),
        SkFontStyle::Bold(),
        SkFontStyle::Italic(),
        SkFontStyle::BoldItalic(),
        SkFontStyle(500, SkFontStyle::kNormal_Width, SkFontStyle::kUpright_Slant)
    };
    
    for (const auto& pattern : patterns) {
        auto tf = styleSet.matchStyle(pattern);
        EXPECT_NE(tf, nullptr);
        
        if (tf) {
            SkFontStyle matchedStyle = tf->fontStyle();
            
            tf.reset();
        }
    }
}

TEST_F(SkFontStyleSet_OHOSTest, getStyle_NullFontConfig) {
  auto styleSet = std::make_unique<SkFontStyleSet_OHOS>(fontConfig, 0, false);
  styleSet->fontConfig_ = nullptr;
  SkFontStyle style;
  SkString styleName;
  styleSet->getStyle(0, &style, &styleName);
  EXPECT_EQ(style, SkFontStyle());
  EXPECT_TRUE(styleName.isEmpty());
}

TEST_F(SkFontStyleSet_OHOSTest, getStyle_NullTypeface) {
  auto styleSet = std::make_unique<SkFontStyleSet_OHOS>(fontConfig, 0, false);
  styleSet->styleIndex = -1;
  SkFontStyle style;
  SkString styleName;
  styleSet->getStyle(0, &style, &styleName);
  EXPECT_EQ(style, SkFontStyle());
  EXPECT_TRUE(styleName.isEmpty());
}

TEST_F(SkFontStyleSet_OHOSTest, createTypeface_NullFontConfig) {
  auto styleSet = std::make_unique<SkFontStyleSet_OHOS>(fontConfig, 0, false);
  styleSet->fontConfig_ = nullptr;
  auto result = styleSet->createTypeface(0);
  EXPECT_EQ(result, nullptr);
}

TEST_F(SkFontStyleSet_OHOSTest, matchStyle_NullFontConfig) {
  auto styleSet = std::make_unique<SkFontStyleSet_OHOS>(fontConfig, 0, false);
  styleSet->fontConfig_ = nullptr;
  SkFontStyle pattern(SkFontStyle::kNormal_Weight, SkFontStyle::kNormal_Width,
                      SkFontStyle::kUpright_Slant);
  auto result = styleSet->matchStyle(pattern);
  EXPECT_EQ(result, nullptr);
}
} // namespace skia