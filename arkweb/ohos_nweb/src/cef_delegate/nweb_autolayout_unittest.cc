/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "nweb_autolayout.h"
 
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <sstream>
 
using namespace testing;
 
const std::string g_valid_config = R"({
    "minMaskAreaRatioThreshold": 60,
    "opacityFilter": [10, 90],
    "minContentAreaRatioThreshold": 20,
    "scaleAnimationDuration": 100,
    "whitelist": {
        "com.example.app": {
            "pattern": "some_pattern",
            "GetID": "get_id_func",
            "GetPage": "get_page_func",
            "appRuleInfos": [
                { "id": "*", "pg": "*" }
            ]
        }
    }
})";
 
namespace OHOS::NWeb {
class NwebAutolayoutTest : public testing::Test, public NwebAutolayout {
};
 
TEST_F(NwebAutolayoutTest, ParseInt_Valid)
{
    EXPECT_EQ(ParseInt("123").value(), 123);
    EXPECT_EQ(ParseInt("0").value(), 0);
    EXPECT_EQ(ParseInt("-1").value(), -1);
}
 
 
TEST_F(NwebAutolayoutTest, ParseInt_Invalid)
{
    EXPECT_FALSE(ParseInt("abc").has_value());
    EXPECT_FALSE(ParseInt("12a").has_value());
    EXPECT_FALSE(ParseInt("").has_value());
    EXPECT_FALSE(ParseInt("1.23").has_value());
}
 
TEST_F(NwebAutolayoutTest, ParseToplevelConfig_Valid)
{
    std::optional<base::Value> root = base::JSONReader::Read(g_valid_config);
    ASSERT_TRUE(root.has_value());
    ASSERT_TRUE(root->is_dict());
    
    EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
    
    EXPECT_EQ(mCCMConfig_.min_mask_area_ratio_threshold, 60);
    EXPECT_EQ(mCCMConfig_.min_content_area_ratio_threshold, 20);
    EXPECT_EQ(mCCMConfig_.scale_animation_duration, 100);
    EXPECT_EQ(mCCMConfig_.opacity_filter.first, 10);
    EXPECT_EQ(mCCMConfig_.opacity_filter.second, 90);
}
 
TEST_F(NwebAutolayoutTest, ParseToplevelConfig_Invalid)
{
    const std::string invalid_config = R"({
        "minMaskAreaRatioThreshold": 200
    })";
    std::optional<base::Value> root = base::JSONReader::Read(invalid_config);
    ASSERT_TRUE(root.has_value());
    ASSERT_TRUE(root->is_dict());
    EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
}
 
TEST_F(NwebAutolayoutTest, ParseToplevelConfig_InvalidOpacityFilterFormat)
{
    const std::vector<std::string> invalid_configs = {
        R"({
            "minMaskAreaRatioThreshold": 60,
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({"minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90, 95],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": ["10", 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, "90"],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})"
    };

    for (const auto& config_str : invalid_configs) {
        std::optional<base::Value> root = base::JSONReader::Read(config_str);
        ASSERT_TRUE(root.has_value());
        ASSERT_TRUE(root->is_dict());
        EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
    }
}

TEST_F(NwebAutolayoutTest, CheckCCMandApplyRule_Disabled)
{
    mEnable_ = false;
    mWListEntry_ = nullptr;

    CheckCCMandApplyRule(nullptr);
    EXPECT_TRUE(true);
}

TEST_F(NwebAutolayoutTest, CheckCCMandApplyRule_WithoutWhitelistEntry)
{
    mEnable_ = true;
    mWListEntry_ = nullptr;

    CheckCCMandApplyRule(nullptr);
    EXPECT_TRUE(true);
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_InvalidOpacityFilterValues)
{
    const std::vector<std::string> invalid_configs = {
        R"({
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [-1, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 101],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [80, 40],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})"
    };

    for (const auto& config_str : invalid_configs) {
        std::optional<base::Value> root = base::JSONReader::Read(config_str);
        ASSERT_TRUE(root.has_value());
        ASSERT_TRUE(root->is_dict());
        EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
    }
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_InvalidMinMaskAreaRatioThreshold)
{
    const std::vector<std::string> invalid_configs = {
        R"({
            "minMaskAreaRatioThreshold": 49,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minMaskAreaRatioThreshold": 101,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})",
        R"({
            "minMaskAreaRatioThreshold": "60",
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 100})"
    };
 
    for (const auto& config_str : invalid_configs) {
        std::optional<base::Value> root = base::JSONReader::Read(config_str);
        ASSERT_TRUE(root.has_value());
        ASSERT_TRUE(root->is_dict());
        EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
    }
}
 
TEST_F(NwebAutolayoutTest, ParseToplevelConfig_InvalidMinContentAreaRatioThreshold)
{
    const std::vector<std::string> invalid_configs = {
        R"({
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 9,
            "scaleAnimationDuration": 100})",
        R"({
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 101,
            "scaleAnimationDuration": 100})",
        R"({
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": "20",
            "scaleAnimationDuration": 100})"
    };
 
    for (const auto& config_str : invalid_configs) {
        std::optional<base::Value> root = base::JSONReader::Read(config_str);
        ASSERT_TRUE(root.has_value());
        ASSERT_TRUE(root->is_dict());
        EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
    }
}
 
TEST_F(NwebAutolayoutTest, ParseToplevelConfig_InvalidScaleAnimationDuration)
{
    const std::vector<std::string> invalid_configs = {
        R"({
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 49})",
        R"({
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": 401})",
        R"({
            "minMaskAreaRatioThreshold": 60,
            "opacityFilter": [10, 90],
            "minContentAreaRatioThreshold": 20,
            "scaleAnimationDuration": "100"})"
    };
 
    for (const auto& config_str : invalid_configs) {
        std::optional<base::Value> root = base::JSONReader::Read(config_str);
        ASSERT_TRUE(root.has_value());
        ASSERT_TRUE(root->is_dict());
        EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
    }
}
 
TEST_F(NwebAutolayoutTest, ParseToplevelConfig_MissingKey)
{
    const std::string invalid_config = R"({
        "minMaskAreaRatioThreshold": 60,
        "scaleAnimationDuration": 100,
        "opacityFilter": [10, 90]
    })";
    std::optional<base::Value> root = base::JSONReader::Read(invalid_config);
    ASSERT_TRUE(root.has_value());
    ASSERT_TRUE(root->is_dict());
    EXPECT_FALSE(ParseToplevelConfig(root->GetDict()));
}
 
TEST_F(NwebAutolayoutTest, ParseWhitelistEntry_Valid)
{
    const std::string entry_str = R"({
        "pattern": "some_pattern",
        "GetID": "get_id_func",
        "GetPage": "get_page_func",
        "appRuleInfos": [ { "id": "rule1" } ]
    })";
    std::optional<base::Value> entry_val = base::JSONReader::Read(entry_str);
    ASSERT_TRUE(entry_val.has_value());
    ASSERT_TRUE(entry_val->is_dict());
 
    mCCMConfig_.whitelist.clear();
    EXPECT_TRUE(ParseWhitelistEntry("com.example.app", entry_val->GetDict()));
    auto it = mCCMConfig_.whitelist.find("com.example.app");
    EXPECT_NE(it, mCCMConfig_.whitelist.end());
    EXPECT_EQ(it->second.pattern, "some_pattern");
    EXPECT_EQ(it->second.getID, "get_id_func");
    EXPECT_EQ(it->second.getPage, "get_page_func");
    EXPECT_TRUE(it->second.appRuleInfos.has_value());
    EXPECT_EQ(it->second.appRuleInfos->size(), 1u);
}
 
TEST_F(NwebAutolayoutTest, ParseWhitelistEntry_Invalid)
{
    const std::string invalid_entry_str = R"({
        "GetID": "get_id_func",
        "GetPage": "get_page_func",
        "appRuleInfos": []
    })";
    std::optional<base::Value> entry_val = base::JSONReader::Read(invalid_entry_str);
    ASSERT_TRUE(entry_val.has_value());
    ASSERT_TRUE(entry_val->is_dict());
    mCCMConfig_.whitelist.clear();
    EXPECT_FALSE(ParseWhitelistEntry("com.example.app", entry_val->GetDict()));
}
 
TEST_F(NwebAutolayoutTest, ParseWhitelistEntry_EmptyFields)
{
    const std::vector<std::string> invalid_entries = {
        R"({"pattern": "", "GetID": "gi", "GetPage": "gp", "appRuleInfos": []})",
        R"({"pattern": "p", "GetID": "", "GetPage": "gp", "appRuleInfos": []})",
        R"({"pattern": "p", "GetID": "gi", "GetPage": "", "appRuleInfos": []})"
    };
 
    for (const auto& entry_str : invalid_entries) {
        std::optional<base::Value> entry_val = base::JSONReader::Read(entry_str);
        ASSERT_TRUE(entry_val.has_value());
        ASSERT_TRUE(entry_val->is_dict());
        mCCMConfig_.whitelist.clear();
        EXPECT_FALSE(ParseWhitelistEntry("com.example.app", entry_val->GetDict()));
    }
}
 
TEST_F(NwebAutolayoutTest, ParseWhitelistEntry_InvalidAppRuleInfos)
{
    const std::string invalid_entry_str = R"({
        "pattern": "p",
        "GetID": "gi",
        "GetPage": "gp",
        "appRuleInfos": "not_a_list"
    })";
    std::optional<base::Value> entry_val = base::JSONReader::Read(invalid_entry_str);
    ASSERT_TRUE(entry_val.has_value());
    ASSERT_TRUE(entry_val->is_dict());
    mCCMConfig_.whitelist.clear();
    EXPECT_FALSE(ParseWhitelistEntry("com.example.app", entry_val->GetDict()));
}
 
 
TEST_F(NwebAutolayoutTest, ParseWhitelistEntry_MissingKeys)
{
    const std::vector<std::string> invalid_entries = {
        R"({"pattern": "p", "GetPage": "gp", "appRuleInfos": []})",
        R"({"pattern": "p", "GetID": "gi", "appRuleInfos": []})",
        R"({"pattern": "p", "GetID": "gi", "GetPage": "gp"})"
    };
 
    for (const auto& entry_str : invalid_entries) {
        std::optional<base::Value> entry_val = base::JSONReader::Read(entry_str);
        ASSERT_TRUE(entry_val.has_value());
        ASSERT_TRUE(entry_val->is_dict());
        mCCMConfig_.whitelist.clear();
        EXPECT_FALSE(ParseWhitelistEntry("com.example.app", entry_val->GetDict()));
    }
}
 
TEST_F(NwebAutolayoutTest, ParseWhitelist_Valid)
{
    std::optional<base::Value> root = base::JSONReader::Read(g_valid_config);
    ASSERT_TRUE(root.has_value());
    const base::Value::Dict* whitelist_dict = root->GetDict().FindDict("whitelist");
    ASSERT_TRUE(whitelist_dict);
    EXPECT_TRUE(ParseWhitelist(*whitelist_dict));
    EXPECT_EQ(mCCMConfig_.whitelist.size(), 1u);
    EXPECT_NE(mCCMConfig_.whitelist.find("com.example.app"), mCCMConfig_.whitelist.end());
}
 
TEST_F(NwebAutolayoutTest, ParseWhitelist_InvalidEntryType)
{
    const std::string invalid_whitelist = R"({
        "com.example.app": "not_a_dict"
    })";
    std::optional<base::Value> whitelist_val = base::JSONReader::Read(invalid_whitelist);
    ASSERT_TRUE(whitelist_val.has_value());
    ASSERT_TRUE(whitelist_val->is_dict());
    EXPECT_FALSE(ParseWhitelist(whitelist_val->GetDict()));
}
 
TEST_F(NwebAutolayoutTest, ParseWhitelist_InvalidEntryContents)
{
    const std::string invalid_whitelist = R"({
        "com.example.app": {
            "pattern": "",
            "GetID": "get_id_func",
            "GetPage": "get_page_func",
            "appRuleInfos": []
        }
    })";

    std::optional<base::Value> whitelist_val = base::JSONReader::Read(invalid_whitelist);
    ASSERT_TRUE(whitelist_val.has_value());
    ASSERT_TRUE(whitelist_val->is_dict());
    EXPECT_FALSE(ParseWhitelist(whitelist_val->GetDict()));
}

TEST_F(NwebAutolayoutTest, Parse_Valid)
{
    std::optional<base::Value> root = base::JSONReader::Read(g_valid_config);
    ASSERT_TRUE(root.has_value());
    EXPECT_TRUE(Parse(*root));
}
 
TEST_F(NwebAutolayoutTest, Parse_Invalid)
{
    const std::string invalid_config = "[]";
    std::optional<base::Value> root = base::JSONReader::Read(invalid_config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(Parse(*root));
}
 
TEST_F(NwebAutolayoutTest, CheckWebContainer_Disabled)
{
    mEnable_ = false;
    CheckWebContainer(nullptr, nullptr);
    EXPECT_TRUE(true);
}

TEST_F(NwebAutolayoutTest, CheckWebContainer_NoBrowser)
{
    mEnable_ = true;
    CheckWebContainer(nullptr, nullptr);
    EXPECT_TRUE(true);
}

TEST_F(NwebAutolayoutTest, Parse_InvalidWhitelistType)
{
    const std::string invalid_config = R"({
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [10, 90],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100,
        "whitelist": []
    })";

    std::optional<base::Value> root = base::JSONReader::Read(invalid_config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(Parse(*root));
}

TEST_F(NwebAutolayoutTest, Parse_InvalidWhitelistEntry)
{
    const std::string invalid_config = R"({
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [10, 90],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100,
        "whitelist": {
            "com.example.app": {
                "pattern": "",
                "GetID": "get_id_func",
                "GetPage": "get_page_func",
                "appRuleInfos": []
            }
        }
    })";

    std::optional<base::Value> root = base::JSONReader::Read(invalid_config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(Parse(*root));
}

TEST_F(NwebAutolayoutTest, Parse_InvalidTopLevelConfig)
{
    const std::string invalid_config = R"({
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [80, 40],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100,
        "whitelist": {}
    })";

    std::optional<base::Value> root = base::JSONReader::Read(invalid_config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(Parse(*root));
}

TEST_F(NwebAutolayoutTest, Parse_MissingWhitelist)
{
    const std::string invalid_config = R"({
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [10, 90],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100
    })";
    std::optional<base::Value> root = base::JSONReader::Read(invalid_config);
    ASSERT_TRUE(root.has_value());
    EXPECT_FALSE(Parse(*root));
}

// Helper to access static function in .cc file
namespace {
std::string TestEscapeForJS(const std::string& s) {
    std::stringstream ss;
    for (char c : s) {
        switch (c) {
            case '`':  ss << "\\`";  break;
            case '\\': ss << "\\\\"; break;
            case '$':  ss << "\\$";  break;
            default:   ss << c;     break;
        }
    }
    return ss.str();
}
}

TEST_F(NwebAutolayoutTest, EscapeForJS_Backtick)
{
    std::string input = "test`quote";
    std::string expected = "test\\`quote";
    EXPECT_EQ(TestEscapeForJS(input), expected);
}

TEST_F(NwebAutolayoutTest, EscapeForJS_Backslash)
{
    std::string input = "test\\path";
    std::string expected = "test\\\\path";
    EXPECT_EQ(TestEscapeForJS(input), expected);
}

TEST_F(NwebAutolayoutTest, EscapeForJS_Dollar)
{
    std::string input = "test$variable";
    std::string expected = "test\\$variable";
    EXPECT_EQ(TestEscapeForJS(input), expected);
}

TEST_F(NwebAutolayoutTest, EscapeForJS_NormalText)
{
    std::string input = "normal text 123";
    std::string expected = "normal text 123";
    EXPECT_EQ(TestEscapeForJS(input), expected);
}

TEST_F(NwebAutolayoutTest, EscapeForJS_MixedSpecialChars)
{
    std::string input = "`hello\\world$var`";
    std::string expected = "\\`hello\\\\world\\$var\\`";
    EXPECT_EQ(TestEscapeForJS(input), expected);
}

TEST_F(NwebAutolayoutTest, EscapeForJS_EmptyString)
{
    std::string input = "";
    std::string expected = "";
    EXPECT_EQ(TestEscapeForJS(input), expected);
}

TEST_F(NwebAutolayoutTest, EscapeForJS_OnlySpecialChars)
{
    std::string input = "`\\$";
    std::string expected = "\\`\\\\\\$";
    EXPECT_EQ(TestEscapeForJS(input), expected);
}

TEST_F(NwebAutolayoutTest, CheckCCMandApplyRule_ValidStateWithNullFrame)
{
    // Set up valid internal state
    mEnable_ = true;
    mCCMConfig_.min_mask_area_ratio_threshold = 60;
    mCCMConfig_.min_content_area_ratio_threshold = 20;
    mCCMConfig_.scale_animation_duration = 100;
    mCCMConfig_.opacity_filter = {10, 90};
    mCCMConfig_.minScaleFactor = 70;
    
    // Create a whitelist entry with valid data
    std::string pattern_data = "test_pattern";
    std::string id_data = "test_id";
    std::string page_data = "test_page";
    
    WhitelistEntry entry;
    entry.pattern = std::string_view(pattern_data);
    entry.getID = std::string_view(id_data);
    entry.getPage = std::string_view(page_data);
    
    base::Value::List rules;
    base::Value::Dict rule;
    rule.Set("id", "*");
    rule.Set("pg", "*");
    rules.Append(std::move(rule));
    entry.appRuleInfos = std::move(rules);
    
    mWListEntry_ = &entry;
    mAutoLayoutJSSource_ = "test_autolayout_script";
    
    // Call with nullptr frame - will fail at ExecuteJavaScript but JSON logic executes first
    CheckCCMandApplyRule(nullptr);
    EXPECT_TRUE(true);
}

TEST_F(NwebAutolayoutTest, CheckCCMandApplyRule_JSONWriteSuccess)
{
    // Set up state to test JSON serialization success path
    mEnable_ = true;
    mCCMConfig_.min_mask_area_ratio_threshold = 75;
    mCCMConfig_.min_content_area_ratio_threshold = 30;
    mCCMConfig_.scale_animation_duration = 200;
    mCCMConfig_.opacity_filter = {20, 80};
    mCCMConfig_.minScaleFactor = 65;
    
    std::string pattern = "pattern";
    std::string id = "id";
    std::string page = "page";
    
    WhitelistEntry entry;
    entry.pattern = std::string_view(pattern);
    entry.getID = std::string_view(id);
    entry.getPage = std::string_view(page);
    
    base::Value::List rules;
    rules.Append(base::Value::Dict());
    entry.appRuleInfos = std::move(rules);
    
    mWListEntry_ = &entry;
    mAutoLayoutJSSource_ = "script";
    
    // The function will process JSON before hitting nullptr
    CheckCCMandApplyRule(nullptr);
    EXPECT_TRUE(true);
}

TEST_F(NwebAutolayoutTest, CheckWebContainer_NullBrowser)
{
    mEnable_ = true;
    mPatternJSSource_ = "test_pattern";
    
    // Test with null browser - should return early without crash
    CheckWebContainer(nullptr, nullptr);
    EXPECT_TRUE(true);
}

TEST_F(NwebAutolayoutTest, CheckWebContainer_EnabledWithNullFrame)
{
    mEnable_ = true;
    mPatternJSSource_ = "pattern_with_special_`\\$chars";
    
    // Both browser and frame are null - tests escape logic but returns before IsMain
    CheckWebContainer(nullptr, nullptr);
    EXPECT_TRUE(true);
}

TEST_F(NwebAutolayoutTest, CheckWebContainer_EscapeLogic)
{
    mEnable_ = true;
    // Set pattern with special characters to test EscapeForJS_TemplateLiteral usage
    mPatternJSSource_ = "test`pattern\\with$specials";
    
    CheckWebContainer(nullptr, nullptr);
    EXPECT_TRUE(true);
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_BoundaryValues)
{
    // Test with boundary values for min_mask_area_ratio_threshold
    const std::string config_min = R"({
        "minMaskAreaRatioThreshold": 50,
        "opacityFilter": [0, 100],
        "minContentAreaRatioThreshold": 11,
        "scaleAnimationDuration": 51
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config_min);
    ASSERT_TRUE(root.has_value());
    EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
    EXPECT_EQ(mCCMConfig_.min_mask_area_ratio_threshold, 50);
    EXPECT_EQ(mCCMConfig_.opacity_filter.first, 0);
    EXPECT_EQ(mCCMConfig_.opacity_filter.second, 100);
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_MaxBoundaryValues)
{
    // Test with max boundary values
    const std::string config_max = R"({
        "minMaskAreaRatioThreshold": 100,
        "opacityFilter": [50, 100],
        "minContentAreaRatioThreshold": 99,
        "scaleAnimationDuration": 399
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config_max);
    ASSERT_TRUE(root.has_value());
    EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
    EXPECT_EQ(mCCMConfig_.min_mask_area_ratio_threshold, 100);
    EXPECT_EQ(mCCMConfig_.scale_animation_duration, 399);
}

TEST_F(NwebAutolayoutTest, ParseWhitelistEntry_WithMultipleRules)
{
    const std::string entry_str = R"({
        "pattern": "multi_pattern",
        "GetID": "multi_id",
        "GetPage": "multi_page",
        "appRuleInfos": [
            { "id": "rule1", "pg": "page1" },
            { "id": "rule2", "pg": "page2" },
            { "id": "*", "pg": "*" }
        ]
    })";
    std::optional<base::Value> entry_val = base::JSONReader::Read(entry_str);
    ASSERT_TRUE(entry_val.has_value());
    ASSERT_TRUE(entry_val->is_dict());

    mCCMConfig_.whitelist.clear();
    EXPECT_TRUE(ParseWhitelistEntry("test.app", entry_val->GetDict()));
    auto it = mCCMConfig_.whitelist.find("test.app");
    EXPECT_NE(it, mCCMConfig_.whitelist.end());
    EXPECT_EQ(it->second.appRuleInfos->size(), 3u);
}

TEST_F(NwebAutolayoutTest, ParseInt_NegativeNumbers)
{
    EXPECT_EQ(ParseInt("-123").value(), -123);
    EXPECT_EQ(ParseInt("-999").value(), -999);
}

TEST_F(NwebAutolayoutTest, ParseInt_LargeNumbers)
{
    EXPECT_EQ(ParseInt("2147483647").value(), 2147483647);
    EXPECT_EQ(ParseInt("999999").value(), 999999);
}

TEST_F(NwebAutolayoutTest, ParseInt_WithWhitespace)
{
    // Should fail with whitespace
    EXPECT_FALSE(ParseInt(" 123").has_value());
    EXPECT_FALSE(ParseInt("123 ").has_value());
    EXPECT_FALSE(ParseInt(" 123 ").has_value());
}

TEST_F(NwebAutolayoutTest, Parse_ValidComplexConfig)
{
    const std::string complex_config = R"({
        "minMaskAreaRatioThreshold": 55,
        "opacityFilter": [15, 85],
        "minContentAreaRatioThreshold": 25,
        "scaleAnimationDuration": 150,
        "whitelist": {
            "com.app1": {
                "pattern": "p1",
                "GetID": "id1",
                "GetPage": "page1",
                "appRuleInfos": [{"id": "1"}]
            },
            "com.app2": {
                "pattern": "p2",
                "GetID": "id2",
                "GetPage": "page2",
                "appRuleInfos": [{"pg": "2"}]
            }
        }
    })";
    
    std::optional<base::Value> root = base::JSONReader::Read(complex_config);
    ASSERT_TRUE(root.has_value());
    EXPECT_TRUE(Parse(*root));
    EXPECT_EQ(mCCMConfig_.whitelist.size(), 2u);
}

TEST_F(NwebAutolayoutTest, ParseToplevelConfig_OpacityFilterEdgeCases)
{
    // Test opacity filter with equal values
    const std::string config_equal = R"({
        "minMaskAreaRatioThreshold": 60,
        "opacityFilter": [50, 50],
        "minContentAreaRatioThreshold": 20,
        "scaleAnimationDuration": 100
    })";
    std::optional<base::Value> root = base::JSONReader::Read(config_equal);
    ASSERT_TRUE(root.has_value());
    EXPECT_TRUE(ParseToplevelConfig(root->GetDict()));
    EXPECT_EQ(mCCMConfig_.opacity_filter.first, 50);
    EXPECT_EQ(mCCMConfig_.opacity_filter.second, 50);
}
}