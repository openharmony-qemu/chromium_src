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
 
TEST_F(NwebAutolayoutTest, ParseToplevelConfig_InvalidMinMaskAreaRatioThreshold)
{
    const std::vector<std::string> invalid_configs = {
        R"({"minMaskAreaRatioThreshold": 49, "opacityFilter": [10, 90], "minContentAreaRatioThreshold": 20, "scaleAnimationDuration": 100})",
        R"({"minMaskAreaRatioThreshold": 101, "opacityFilter": [10, 90], "minContentAreaRatioThreshold": 20, "scaleAnimationDuration": 100})",
        R"({"minMaskAreaRatioThreshold": "60", "opacityFilter": [10, 90], "minContentAreaRatioThreshold": 20, "scaleAnimationDuration": 100})"
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
        R"({"minMaskAreaRatioThreshold": 60, "opacityFilter": [10, 90], "minContentAreaRatioThreshold": 9, "scaleAnimationDuration": 100})",
        R"({"minMaskAreaRatioThreshold": 60, "opacityFilter": [10, 90], "minContentAreaRatioThreshold": 101, "scaleAnimationDuration": 100})",
        R"({"minMaskAreaRatioThreshold": 60, "opacityFilter": [10, 90], "minContentAreaRatioThreshold": "20", "scaleAnimationDuration": 100})"
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
        R"({"minMaskAreaRatioThreshold": 60, "opacityFilter": [10, 90], "minContentAreaRatioThreshold": 20, "scaleAnimationDuration": 49})",
        R"({"minMaskAreaRatioThreshold": 60, "opacityFilter": [10, 90], "minContentAreaRatioThreshold": 20, "scaleAnimationDuration": 401})",
        R"({"minMaskAreaRatioThreshold": 60, "opacityFilter": [10, 90], "minContentAreaRatioThreshold": 20, "scaleAnimationDuration": "100"})"
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
}