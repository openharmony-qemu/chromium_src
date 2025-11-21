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

#include <memory>
#include <string>
#include <vector>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "arkweb/chromium_ext/components/url_formatter/arkweb_elide_url_ext.h"
#include "base/strings/utf_string_conversions.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "url/gurl.h"
#include "url/third_party/mozilla/url_parse.h"
#include "url/url_constants.h"

namespace url_formatter {

class ArkWebElideUrlExtTest : public testing::Test {
 public:
  void SetUp() override {
    // Setup test environment
  }

  void TearDown() override {
    // Cleanup
  }

 protected:
  // Helper function to create test UTF-16 strings
  std::u16string ToUTF16(const std::string& input) {
    return base::UTF8ToUTF16(input);
  }
};

#if BUILDFLAG(IS_ARKWEB_EXT)

// Test ParseInput with invalid input (empty string)
TEST_F(ArkWebElideUrlExtTest, ParseInput_EmptyString) {
  std::u16string input = u"";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::INVALID);
}

// Test ParseInput with whitespace only input
TEST_F(ArkWebElideUrlExtTest, ParseInput_WhitespaceOnly) {
  std::u16string input = u"   \t\n\r   ";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::INVALID);
}

// Test ParseInput with valid HTTP URL
TEST_F(ArkWebElideUrlExtTest, ParseInput_ValidHttpUrl) {
  std::u16string input = u"http://example.com";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
  EXPECT_EQ(scheme, u"http");
  EXPECT_TRUE(canonicalized_url.is_valid());
  EXPECT_EQ(canonicalized_url.host(), "example.com");
}

// Test ParseInput with valid HTTPS URL
TEST_F(ArkWebElideUrlExtTest, ParseInput_ValidHttpsUrl) {
  std::u16string input = u"https://secure.example.com/path";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
  EXPECT_EQ(scheme, u"https");
  EXPECT_TRUE(canonicalized_url.is_valid());
  EXPECT_EQ(canonicalized_url.host(), "secure.example.com");
}

// Test ParseInput with file scheme
TEST_F(ArkWebElideUrlExtTest, ParseInput_FileScheme) {
  std::u16string input = u"file:///path/to/file.html";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
  EXPECT_EQ(scheme, u"file");
}

// Test ParseInput with file scheme without explicit prefix
TEST_F(ArkWebElideUrlExtTest, ParseInput_FileWithoutPrefix) {
  std::u16string input = u"/path/to/file.html";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  // Should be detected as file but return QUERY since it doesn't start with file://
  EXPECT_TRUE(result == UrlType::QUERY || result == UrlType::URL);
}

// Test ParseInput with JavaScript scheme (valid code)
TEST_F(ArkWebElideUrlExtTest, ParseInput_JavaScriptScheme_ValidCode) {
  std::u16string input = u"javascript:alert('Hello World');";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
  EXPECT_EQ(scheme, u"javascript");
}

// Test ParseInput with JavaScript scheme (simple query-like)
TEST_F(ArkWebElideUrlExtTest, ParseInput_JavaScriptScheme_QueryLike) {
  std::u16string input = u"javascript:test";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::UNKNOWN);
  EXPECT_EQ(scheme, u"javascript");
}

// Test ParseInput with data scheme
TEST_F(ArkWebElideUrlExtTest, ParseInput_DataScheme) {
  std::u16string input = u"data:text/html,<html><body>Hello</body></html>";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
  EXPECT_EQ(scheme, u"data");
}

// Test ParseInput with blob scheme
TEST_F(ArkWebElideUrlExtTest, ParseInput_BlobScheme) {
  std::u16string input = u"blob:https://example.com/550e8400-e29b-41d4-a716-446655440000";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
  EXPECT_EQ(scheme, u"blob");
}

// Test ParseInput with arkweb scheme
TEST_F(ArkWebElideUrlExtTest, ParseInput_ArkWebScheme) {
  std::u16string input = u"arkweb://resource/page";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
  EXPECT_EQ(scheme, u"arkweb");
}

// Test ParseInput with dataability scheme
TEST_F(ArkWebElideUrlExtTest, ParseInput_DataabilityScheme) {
  std::u16string input = u"dataability://com.example.provider/data";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
  EXPECT_EQ(scheme, u"dataability");
}

// Test ParseInput with unknown scheme
TEST_F(ArkWebElideUrlExtTest, ParseInput_UnknownScheme) {
  std::u16string input = u"unknown://example.com";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::UNKNOWN);
  EXPECT_EQ(scheme, u"unknown");
}

// Test ParseInput with IPv4 address
TEST_F(ArkWebElideUrlExtTest, ParseInput_IPv4Address) {
  std::u16string input = u"192.168.1.1";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_TRUE(result == UrlType::URL || result == UrlType::QUERY);
}

// Test ParseInput with IPv4 address with port
TEST_F(ArkWebElideUrlExtTest, ParseInput_IPv4AddressWithPort) {
  std::u16string input = u"192.168.1.1:8080";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
}

// Test ParseInput with IPv6 address
TEST_F(ArkWebElideUrlExtTest, ParseInput_IPv6Address) {
  std::u16string input = u"[2001:db8::1]";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
}

// Test ParseInput with localhost
TEST_F(ArkWebElideUrlExtTest, ParseInput_Localhost) {
  std::u16string input = u"localhost";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
}

// Test ParseInput with localhost and port
TEST_F(ArkWebElideUrlExtTest, ParseInput_LocalhostWithPort) {
  std::u16string input = u"localhost:3000";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
}

// Test ParseInput with domain ending with slash
TEST_F(ArkWebElideUrlExtTest, ParseInput_DomainWithSlash) {
  std::u16string input = u"example.com/";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
}

// Test ParseInput with domain ending with backslash
TEST_F(ArkWebElideUrlExtTest, ParseInput_DomainWithBackslash) {
  std::u16string input = u"example.com\\";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
}

// Test ParseInput with username and password
TEST_F(ArkWebElideUrlExtTest, ParseInput_WithCredentials) {
  std::u16string input = u"user:pass@example.com";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
}

// Test ParseInput with username containing space
TEST_F(ArkWebElideUrlExtTest, ParseInput_UsernameWithSpace) {
  std::u16string input = u"user name:pass@example.com";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::UNKNOWN);
}

// Test ParseInput with known TLD
TEST_F(ArkWebElideUrlExtTest, ParseInput_KnownTLD) {
  std::u16string input = u"example.com";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
}

// Test ParseInput with unknown TLD
TEST_F(ArkWebElideUrlExtTest, ParseInput_UnknownTLD) {
  std::u16string input = u"example.unknowntld";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::UNKNOWN);
}

// Test ParseInput with .example domain
TEST_F(ArkWebElideUrlExtTest, ParseInput_ExampleDomain) {
  std::u16string input = u"test.example";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
}

// Test ParseInput with .test domain
TEST_F(ArkWebElideUrlExtTest, ParseInput_TestDomain) {
  std::u16string input = u"site.test";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
}

// Test ParseInput with .local domain
TEST_F(ArkWebElideUrlExtTest, ParseInput_LocalDomain) {
  std::u16string input = u"printer.local";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
}

// Test ParseInput with search query
TEST_F(ArkWebElideUrlExtTest, ParseInput_SearchQuery) {
  std::u16string input = u"search for something";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::QUERY);
}

// Test ParseInput with path but no scheme
TEST_F(ArkWebElideUrlExtTest, ParseInput_PathNoScheme) {
  std::u16string input = u"example.com/path/to/page";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
}

// Test ParseInput with query parameters
TEST_F(ArkWebElideUrlExtTest, ParseInput_WithQueryParams) {
  std::u16string input = u"example.com/search?q=test&page=1";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
}

// Test ParseInput with fragment
TEST_F(ArkWebElideUrlExtTest, ParseInput_WithFragment) {
  std::u16string input = u"example.com/page#section";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
}

// Test ParseInput with null parts parameter
TEST_F(ArkWebElideUrlExtTest, ParseInput_NullParts) {
  std::u16string input = u"https://example.com";
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, nullptr, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
  EXPECT_EQ(scheme, u"https");
}

// Test ParseInput with null scheme parameter
TEST_F(ArkWebElideUrlExtTest, ParseInput_NullScheme) {
  std::u16string input = u"https://example.com";
  url::Parsed parts;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, nullptr, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
}

// Test ParseInput with null canonicalized_url parameter
TEST_F(ArkWebElideUrlExtTest, ParseInput_NullCanonicalizedUrl) {
  std::u16string input = u"https://example.com";
  url::Parsed parts;
  std::u16string scheme;
  
  UrlType result = ParseInput(input, &parts, &scheme, nullptr);
  EXPECT_EQ(result, UrlType::URL);
  EXPECT_EQ(scheme, u"https");
}

// Test ParseInput with all null optional parameters
TEST_F(ArkWebElideUrlExtTest, ParseInput_AllNullParams) {
  std::u16string input = u"https://example.com";
  
  UrlType result = ParseInput(input, nullptr, nullptr, nullptr);
  EXPECT_EQ(result, UrlType::URL);
}

// Test ParseInput with complex URL
TEST_F(ArkWebElideUrlExtTest, ParseInput_ComplexUrl) {
  std::u16string input = u"https://user:pass@sub.example.com:8080/path/to/page?param=value&other=123#section";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
  EXPECT_EQ(scheme, u"https");
  EXPECT_TRUE(canonicalized_url.is_valid());
}

// Test ParseInput with internationalized domain
TEST_F(ArkWebElideUrlExtTest, ParseInput_InternationalizedDomain) {
  std::u16string input = u"https://测试.中国";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
  EXPECT_EQ(scheme, u"https");
}

// Test ParseInput with leading/trailing whitespace
TEST_F(ArkWebElideUrlExtTest, ParseInput_WithWhitespace) {
  std::u16string input = u"  https://example.com  ";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
  EXPECT_EQ(scheme, u"https");
}

// Test ParseInput with very long URL
TEST_F(ArkWebElideUrlExtTest, ParseInput_VeryLongUrl) {
  std::string long_path(10000, 'a');
  std::u16string input = ToUTF16("https://example.com/" + long_path);
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
  EXPECT_EQ(scheme, u"https");
}

// Test ParseInput with special characters in path
TEST_F(ArkWebElideUrlExtTest, ParseInput_SpecialCharactersInPath) {
  std::u16string input = u"https://example.com/path with spaces & symbols!";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
  EXPECT_EQ(scheme, u"https");
}

// Test ParseInput edge case: invalid domain
TEST_F(ArkWebElideUrlExtTest, ParseInput_InvalidDomain) {
  std::u16string input = u"invalid";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::UNKNOWN);
}

// Test ParseInput edge case: zero IP
TEST_F(ArkWebElideUrlExtTest, ParseInput_ZeroIP) {
  std::u16string input = u"0.0.0.0";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
}

// Test ParseInput performance with many components
TEST_F(ArkWebElideUrlExtTest, ParseInput_ManyComponents) {
  std::u16string input = u"user:pass@host.example.com:8080/path?query=value&other=test#fragment";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
}

// Test ParseInput with mixed case scheme
TEST_F(ArkWebElideUrlExtTest, ParseInput_MixedCaseScheme) {
  std::u16string input = u"HTTpS://Example.COM";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
  EXPECT_EQ(scheme, u"https");
}

// Test ParseInput with filesystem scheme
TEST_F(ArkWebElideUrlExtTest, ParseInput_FileSystemScheme) {
  std::u16string input = u"filesystem:https://example.com/temporary/file.txt";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
  EXPECT_EQ(scheme, u"filesystem");
}

// Test ParseInput with content scheme
TEST_F(ArkWebElideUrlExtTest, ParseInput_ContentScheme) {
  std::u16string input = u"content://com.example.provider/data";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::UNKNOWN);
  EXPECT_EQ(scheme, u"content");
}

// Test ParseInput with resources scheme
TEST_F(ArkWebElideUrlExtTest, ParseInput_ResourcesScheme) {
  std::u16string input = u"resources://app/page.html";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::UNKNOWN);
  EXPECT_EQ(scheme, u"resources");
}

// Test ParseInput with non-ASCII scheme
TEST_F(ArkWebElideUrlExtTest, ParseInput_NonASCIIScheme) {
  std::u16string input = u"测试://example.com";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::UNKNOWN);
}

// Test ParseInput with standard scheme not in special list
TEST_F(ArkWebElideUrlExtTest, ParseInput_StandardScheme) {
  std::u16string input = u"ftp://ftp.example.com/file.txt";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);  // ftp is a standard scheme
  EXPECT_EQ(scheme, u"ftp");
}

// Test ParseInput with multiple non-host components
TEST_F(ArkWebElideUrlExtTest, ParseInput_MultipleComponents) {
  std::u16string input = u"example.com:8080/path?query=value#fragment";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);  // Should be URL due to multiple components
}

// Test ParseInput with single component (hostname only)
TEST_F(ArkWebElideUrlExtTest, ParseInput_SingleComponent) {
  std::u16string input = u"unknownhost";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::UNKNOWN);  // Single unknown component
}

// Test ParseInput with IPv4 partial address
TEST_F(ArkWebElideUrlExtTest, ParseInput_IPv4Partial) {
  std::u16string input = u"192.168";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::QUERY);  // Partial IPv4 should be query
}

// Test ParseInput with port only
TEST_F(ArkWebElideUrlExtTest, ParseInput_PortOnly) {
  std::u16string input = u":8080";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_TRUE(result == UrlType::QUERY || result == UrlType::UNKNOWN || result == UrlType::INVALID);
}

// Test ParseInput with username but no password
TEST_F(ArkWebElideUrlExtTest, ParseInput_UsernameOnly) {
  std::u16string input = u"user@example.com";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::UNKNOWN);  // Username without password
}

// Test ParseInput with complex JavaScript
TEST_F(ArkWebElideUrlExtTest, ParseInput_ComplexJavaScript) {
  std::u16string input = u"javascript:void(0);";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);  // Complex JS should be URL
  EXPECT_EQ(scheme, u"javascript");
}

// Test ParseInput with query in URL format
TEST_F(ArkWebElideUrlExtTest, ParseInput_QueryInUrlFormat) {
  std::u16string input = u"search query with multiple words";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::QUERY);
}

// Test ParseInput with malformed URL
TEST_F(ArkWebElideUrlExtTest, ParseInput_MalformedUrl) {
  std::u16string input = u"http://";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_TRUE(result == UrlType::QUERY || result == UrlType::UNKNOWN);
}

// Test ParseInput with username and password in HTTP context
TEST_F(ArkWebElideUrlExtTest, ParseInput_HttpWithCredentials) {
  std::u16string input = u"user:pass@example.com";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;
  
  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  // Should attempt to parse as HTTP URL with credentials
  EXPECT_TRUE(result == UrlType::URL || result == UrlType::UNKNOWN);
}

// Test ParseInput with special domains
TEST_F(ArkWebElideUrlExtTest, ParseInput_SpecialDomains) {
  std::vector<std::u16string> special_domains = {
    u"sub.example",     // Should be URL (special .example domain)
    u"test.test",       // Should be URL (special .test domain)  
    u"device.local",    // Should be URL (special .local domain)
    u"example",         // Should be UNKNOWN (just "example")
    u"test",            // Should be UNKNOWN (just "test")
    u"local"            // Should be UNKNOWN (just "local")
  };
  
  std::vector<UrlType> expected_results = {
    UrlType::URL,
    UrlType::URL,
    UrlType::URL,
    UrlType::UNKNOWN,
    UrlType::UNKNOWN,
    UrlType::UNKNOWN
  };
  
  for (size_t i = 0; i < special_domains.size(); ++i) {
    url::Parsed parts;
    std::u16string scheme;
    GURL canonicalized_url;
    
    UrlType result = ParseInput(special_domains[i], &parts, &scheme, &canonicalized_url);
    EXPECT_EQ(result, expected_results[i]) << "Failed for domain: " << base::UTF16ToUTF8(special_domains[i]);
  }
}

// Test ParseInput with stress test - many similar URLs
TEST_F(ArkWebElideUrlExtTest, ParseInput_StressTest) {
  for (int i = 0; i < 100; ++i) {
    std::u16string input = ToUTF16("https://test" + std::to_string(i) + ".example.com");
    url::Parsed parts;
    std::u16string scheme;
    GURL canonicalized_url;
    
    UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
    EXPECT_EQ(result, UrlType::URL);
    EXPECT_EQ(scheme, u"https");
    EXPECT_TRUE(canonicalized_url.is_valid());
  }
}

// Test ParseInput memory management
TEST_F(ArkWebElideUrlExtTest, ParseInput_MemoryManagement) {
  // Test repeated calls to ensure no memory leaks
  for (int i = 0; i < 50; ++i) {
    std::u16string input = u"https://example.com/test";
    url::Parsed parts;
    std::u16string scheme;
    GURL canonicalized_url;
    
    UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
    EXPECT_EQ(result, UrlType::URL);
    
    // Force cleanup between iterations
    scheme.clear();
  }
}

TEST_F(ArkWebElideUrlExtTest, NumNonHostComponents_SchemeNonEmpty) {
  url::Parsed parts;
  parts.scheme = url::Component(0, 4);
  parts.username = url::Component();
  parts.password = url::Component();
  parts.port = url::Component();
  parts.path = url::Component();
  parts.query = url::Component();
  parts.ref = url::Component();

  int result = TestNumNonHostComponents(parts);
  EXPECT_EQ(result, 1);
}

TEST_F(ArkWebElideUrlExtTest, NumNonHostComponents_PasswordNonEmpty) {
  url::Parsed parts;
  parts.scheme = url::Component();
  parts.username = url::Component();
  parts.password = url::Component(0, 6);
  parts.port = url::Component();
  parts.path = url::Component();
  parts.query = url::Component();
  parts.ref = url::Component();

  int result = TestNumNonHostComponents(parts);
  EXPECT_EQ(result, 1);
}

TEST_F(ArkWebElideUrlExtTest, GetInputTypeForScheme_EmptyScheme) {
  std::string scheme = "";

  UrlType result = TestGetInputTypeForScheme(scheme);
  EXPECT_EQ(result, UrlType::INVALID);
}

TEST_F(ArkWebElideUrlExtTest, GetInputTypeForScheme_NonASCIIScheme) {
  std::string scheme = "htt\x80p";
  UrlType result = TestGetInputTypeForScheme(scheme);
  EXPECT_NE(result, UrlType::URL);
}

TEST_F(ArkWebElideUrlExtTest, GetInputTypeForScheme_ResourceScheme) {
  std::string scheme = "resource";
  UrlType result = TestGetInputTypeForScheme(scheme);
  EXPECT_EQ(result, UrlType::URL);
}

TEST_F(ArkWebElideUrlExtTest, GetInputTypeForScheme_DataabilityScheme) {
  std::string scheme = "dataability";
  UrlType result = TestGetInputTypeForScheme(scheme);
  EXPECT_EQ(result, UrlType::URL);
}

TEST_F(ArkWebElideUrlExtTest, ParseInput_UsernameHasSpace) {
  std::u16string input = u"user name@example.com/path";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;

  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::UNKNOWN);
  EXPECT_TRUE(parts.path.is_nonempty());
  EXPECT_TRUE(parts.username.is_nonempty());
}

TEST_F(ArkWebElideUrlExtTest, ParseInput_Has_Port) {
  std::u16string input = u"unknown-domain:8080";
  url::Parsed parts;
  std::u16string scheme;
  GURL canonicalized_url;

  UrlType result = ParseInput(input, &parts, &scheme, &canonicalized_url);
  EXPECT_EQ(result, UrlType::URL);
  EXPECT_TRUE(canonicalized_url.has_port());
  EXPECT_EQ(canonicalized_url.port(), "8080");
}

#endif  // BUILDFLAG(IS_ARKWEB_EXT)

}  // namespace url_formatter