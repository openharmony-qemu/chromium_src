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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "base/check_op.h"
#include "base/notreached.h"
#include "base/numerics/checked_math.h"
#include "base/ranges/algorithm.h"
#include "base/strings/string_util.h"
#include "components/url_pattern_index/flat/url_pattern_index_generated.h"
#include "components/url_pattern_index/fuzzy_pattern_matching.h"
#include "components/url_pattern_index/string_splitter.h"
#include "components/url_pattern_index/url_pattern.h"
#include "components/url_pattern_index/url_pattern_index.h"
#include "third_party/flatbuffers/src/include/flatbuffers/flatbuffers.h"
#include "url/gurl.h"
#include "url/third_party/mozilla/url_parse.h"
#if BUILDFLAG(ARKWEB_ADBLOCK)
#include "arkweb/chromium_ext/components/url_pattern_index/flat/css_pattern_index_generated.h"
#endif
#include "arkweb/chromium_ext/components/url_pattern_index/url_pattern_index_ext.h"

namespace url_pattern_index {

class UrlPatternExtTest : public testing::Test {
 public:
  void SetUp() override {}
  void TearDown() override {}

 protected:
#if BUILDFLAG(ARKWEB_ADBLOCK)
  // Helper function to create a FlatBuffer CSS rule
  const flat::CssRule* CreateFlatCssRule(
      const std::string& selector,
      const std::vector<std::string>& included_domains = {},
      const std::vector<std::string>& excluded_domains = {},
      uint8_t options = 0) {
    flatbuffers::FlatBufferBuilder builder;

    // Create domain vectors
    std::vector<flatbuffers::Offset<flatbuffers::String>> included_offsets;
    std::vector<flatbuffers::Offset<flatbuffers::String>> excluded_offsets;

    for (const auto& domain : included_domains) {
      included_offsets.push_back(builder.CreateString(domain));
    }
    for (const auto& domain : excluded_domains) {
      excluded_offsets.push_back(builder.CreateString(domain));
    }

    flatbuffers::Offset<
        flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>>
        included_vector;
    flatbuffers::Offset<
        flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>>
        excluded_vector;
    if (!included_offsets.empty()) {
      included_vector = builder.CreateVector(included_offsets);
    }
    if (!excluded_offsets.empty()) {
      excluded_vector = builder.CreateVector(excluded_offsets);
    }

    auto css_selector = builder.CreateString(selector);
    auto rule_offset = flat::CreateCssRule(builder, options, included_vector,
                                           excluded_vector, css_selector);
    builder.Finish(rule_offset);
    const flat::CssRule* flat_rule =
        flatbuffers::GetTemporaryPointer(builder, rule_offset);
    EXPECT_NE(flat_rule, nullptr);
    return flat_rule;
  }
#endif
};

#if BUILDFLAG(ARKWEB_ADBLOCK)
TEST_F(UrlPatternExtTest, CssPattern_MatchCase_Default) {
  CssPattern pattern;
  EXPECT_TRUE(pattern.match_case());  // Default should be case sensitive
}

// Test CssPattern MatchesCss method
TEST_F(UrlPatternExtTest, CssPattern_MatchesCss) {
  CssPattern pattern;
  GURL test_url("https://example.com/page");
  CssPattern::UrlInfo url_info(test_url);
  bool result = pattern.MatchesCss(url_info);
  EXPECT_TRUE(result);
}

// Test CssPattern UrlInfo construction
TEST_F(UrlPatternExtTest, CssPattern_UrlInfo_Construction) {
  GURL test_url("https://example.com:8080/path?query=value#fragment");
  EXPECT_NO_FATAL_FAILURE({ CssPattern::UrlInfo url_info(test_url); });
}

// Test CssPattern UrlInfo spec method
TEST_F(UrlPatternExtTest, CssPattern_UrlInfo_Spec) {
  GURL test_url("https://example.com/path");
  CssPattern::UrlInfo url_info(test_url);
  std::string_view spec = url_info.spec();
  EXPECT_EQ(spec, "https://example.com/path");
}

// Test CssPattern UrlInfo GetLowerCaseSpec with all lowercase
TEST_F(UrlPatternExtTest, CssPattern_UrlInfo_GetLowerCaseSpec_AllLowercase) {
  GURL test_url("https://example.com/path");
  CssPattern::UrlInfo url_info(test_url);

  std::string_view lower_spec = url_info.GetLowerCaseSpec();
  EXPECT_EQ(lower_spec, "https://example.com/path");

  // Call again to test caching
  std::string_view lower_spec2 = url_info.GetLowerCaseSpec();
  EXPECT_EQ(lower_spec2, "https://example.com/path");
  EXPECT_EQ(lower_spec.data(), lower_spec2.data());  // Should be cached
}

// Test CssPattern UrlInfo GetLowerCaseSpec with mixed case
TEST_F(UrlPatternExtTest, CssPattern_UrlInfo_GetLowerCaseSpec_MixedCase) {
  GURL test_url("HTTPS://EXAMPLE.COM/PATH");
  CssPattern::UrlInfo url_info(test_url);

  std::string_view lower_spec = url_info.GetLowerCaseSpec();
  EXPECT_EQ(lower_spec, "https://example.com/path");

  // Call again to test caching
  std::string_view lower_spec2 = url_info.GetLowerCaseSpec();
  EXPECT_EQ(lower_spec2, "https://example.com/path");
  EXPECT_EQ(lower_spec.data(), lower_spec2.data());  // Should be cached
  EXPECT_EQ(lower_spec, "https://example.com/path");
}

// Test CssPattern UrlInfo host method
TEST_F(UrlPatternExtTest, CssPattern_UrlInfo_Host) {
  GURL test_url("https://example.com:8080/path");
  CssPattern::UrlInfo url_info(test_url);

  url::Component host_component = url_info.host();
  EXPECT_TRUE(host_component.is_valid());
  EXPECT_GT(host_component.len, 0);

  // Extract host from the spec to verify
  std::string_view spec = url_info.spec();
  std::string host(spec.substr(host_component.begin, host_component.len));
  EXPECT_EQ(host, "example.com");
}

// Test CssPattern with different match case settings
TEST_F(UrlPatternExtTest, CssPattern_DifferentMatchCaseSettings) {
  CssPattern pattern1(*CreateFlatCssRule("rule", {"example.com"}, {}));
  EXPECT_TRUE(pattern1.match_case());

  CssPattern pattern2(*CreateFlatCssRule("rule", {"example.com"}, {},
                                         flat::OptionFlag_IS_CASE_INSENSITIVE));
  EXPECT_FALSE(pattern2.match_case());
}
#endif  // BUILDFLAG(ARKWEB_ADBLOCK)

}  // namespace url_pattern_index