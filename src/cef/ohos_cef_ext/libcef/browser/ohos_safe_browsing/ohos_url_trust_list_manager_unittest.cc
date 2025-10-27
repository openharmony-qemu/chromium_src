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

#include "base/json/json_reader.h"
#include "base/json/json_value_converter.h"
#include "base/logging.h"
#include "gtest/gtest.h"
#define private public
#define protected public
#include "ohos_url_trust_list_interface.h"
#include "ohos_url_trust_list_manager.h"
#undef protected
#undef privateout

namespace ohos_safe_browsing {
static constexpr int MAX_PATH_SIZE = 0x10000;
class UrlTrustListManagerTest : public ::testing::Test {
 protected:
  void SetUp();
  void TearDown();
  UrlTrustListManager* url_trust_list_manager_ = nullptr;
};

void UrlTrustListManagerTest::SetUp() {
  url_trust_list_manager_ = new UrlTrustListManager();
};

void UrlTrustListManagerTest::TearDown() {
  delete url_trust_list_manager_;
  url_trust_list_manager_ = nullptr;
};

TEST_F(UrlTrustListManagerTest, ParseString) {
  std::string output = std::string();
  EXPECT_FALSE(ParseString(nullptr, &output));

  base::Value value("test");
  EXPECT_FALSE(ParseString(&value, nullptr));

  value = base::Value(42);
  output = std::string();
  EXPECT_TRUE(ParseString(&value, &output));
  EXPECT_EQ("", output);

  const std::string expected = "hello";
  value = base::Value(expected);
  output = std::string();
  EXPECT_TRUE(ParseString(&value, &output));
  EXPECT_EQ(expected, output);
}

TEST_F(UrlTrustListManagerTest, ParseInt) {
  int output = 0;
  EXPECT_FALSE(ParseInt(nullptr, &output));

  base::Value value(42);
  EXPECT_FALSE(ParseInt(&value, nullptr));

  value = base::Value("test");
  output = 0;
  EXPECT_TRUE(ParseInt(&value, &output));
  EXPECT_EQ(-1, output);

  const int expected = 42;
  value = base::Value(expected);
  output = 0;
  EXPECT_TRUE(ParseInt(&value, &output));
  EXPECT_EQ(expected, output);
}

TEST_F(UrlTrustListManagerTest, UrlTrustRule_RegisterJSONConverter) {
  UrlTrustRule::RegisterJSONConverter(nullptr);

  const std::string json_data = R"({
    "scheme": "https",
    "host": "example.com",
    "port": 443,
    "path": "/test/path"
  })";
  auto parsed_json = base::JSONReader::Read(json_data);
  ASSERT_TRUE(parsed_json.has_value());
  ASSERT_TRUE(parsed_json->is_dict());

  base::JSONValueConverter<UrlTrustRule> converter;
  UrlTrustRule::RegisterJSONConverter(&converter);
  UrlTrustRule url_trust_rule;
  bool result = converter.Convert(parsed_json->GetDict(), &url_trust_rule);
  ASSERT_TRUE(result);

  EXPECT_EQ("https", url_trust_rule.scheme);
  EXPECT_EQ("example.com", url_trust_rule.host);
  EXPECT_EQ(443, url_trust_rule.port);
  EXPECT_EQ("/test/path", url_trust_rule.path);
}

TEST_F(UrlTrustListManagerTest,
       UrlTrustListManager_SetUrlTrustListWithErrMsg_001) {
  std::string err_msg = std::string();
  UrlListSetResult result =
      url_trust_list_manager_->SetUrlTrustListWithErrMsg(std::string(), err_msg);
  EXPECT_EQ(result, UrlListSetResult::SET_OK);

  err_msg = std::string();
  result =
      url_trust_list_manager_->SetUrlTrustListWithErrMsg("{invalid json", err_msg);
  EXPECT_EQ(result, UrlListSetResult::PARAM_ERROR);
  EXPECT_EQ(err_msg, "json format failed");

  err_msg = std::string();
  result = url_trust_list_manager_->SetUrlTrustListWithErrMsg(
      "[\"array\", \"value\"]", err_msg);
  EXPECT_EQ(result, UrlListSetResult::PARAM_ERROR);
  EXPECT_EQ(err_msg, "json format failed");

  err_msg = std::string();
  result = url_trust_list_manager_->SetUrlTrustListWithErrMsg(
      "{\"otherField\": \"value\"}", err_msg);
  EXPECT_EQ(result, UrlListSetResult::PARAM_ERROR);
  EXPECT_EQ(err_msg, "can not find UrlPermissionList");

  err_msg = std::string();
  result = url_trust_list_manager_->SetUrlTrustListWithErrMsg(
      "{\"UrlPermissionList\": []}", err_msg);
  EXPECT_EQ(result, UrlListSetResult::SET_OK);
}

TEST_F(UrlTrustListManagerTest,
       UrlTrustListManager_SetUrlTrustListWithErrMsg_002) {
  std::string err_msg = std::string();
  std::string rule_json = R"({
    "UrlPermissionList": [{
        "scheme": "ftp",
        "host": "example.com",
        "port": 443,
        "path": "/test/path" }]})";
  UrlListSetResult result =
      url_trust_list_manager_->SetUrlTrustListWithErrMsg(rule_json, err_msg);
  EXPECT_EQ(result, UrlListSetResult::PARAM_ERROR);
  EXPECT_EQ(err_msg, "rule 1 check error, host example.com scheme is invalid");
}

TEST_F(UrlTrustListManagerTest,
       UrlTrustListManager_SetUrlTrustListWithErrMsg_003) {
  std::string err_msg = std::string();
  std::string rule_json = R"({
    "UrlPermissionList": [{
        "scheme": "http",
        "host": "",
        "port": 443,
        "path": "/test/path" }]})";
  UrlListSetResult result =
      url_trust_list_manager_->SetUrlTrustListWithErrMsg(rule_json, err_msg);
  EXPECT_EQ(result, UrlListSetResult::PARAM_ERROR);
  EXPECT_EQ(err_msg, "rule 1 check error, empty host");
}

TEST_F(UrlTrustListManagerTest,
       UrlTrustListManager_SetUrlTrustListWithErrMsg_004) {
  std::string err_msg = std::string();
  std::string rule_json = R"({
    "UrlPermissionList": [{
        "scheme": "https",
        "host": "example.com",
        "port": -1,
        "path": "/test/path" }]})";
  UrlListSetResult result =
      url_trust_list_manager_->SetUrlTrustListWithErrMsg(rule_json, err_msg);
  EXPECT_EQ(result, UrlListSetResult::PARAM_ERROR);
  EXPECT_EQ(err_msg, "rule 1 check error, host example.com port is invalid");
}

TEST_F(UrlTrustListManagerTest,
       UrlTrustListManager_SetUrlTrustListWithErrMsg_005) {
  std::string err_msg = std::string();
  std::string longPath(MAX_PATH_SIZE + 1, 'a');
  std::string rule_json = R"({
    "UrlPermissionList": [{
        "scheme": "http",
        "host": "example.com",
        "port": 442,
        "path": ")" + longPath +
                         R"(" }]})";
  UrlListSetResult result =
      url_trust_list_manager_->SetUrlTrustListWithErrMsg(rule_json, err_msg);
  EXPECT_EQ(result, UrlListSetResult::PARAM_ERROR);
  EXPECT_EQ(err_msg, "rule 1 check error, host example.com path len too long");
}

TEST_F(UrlTrustListManagerTest,
       UrlTrustListManager_SetUrlTrustListWithErrMsg_006) {
  std::string err_msg = std::string();
  std::string rule_json = R"({
    "UrlPermissionList": [{
        "scheme": "https",
        "host": "example.com",
        "port": 0,
        "path": "/test/path" }]})";
  UrlListSetResult result =
      url_trust_list_manager_->SetUrlTrustListWithErrMsg(rule_json, err_msg);
  EXPECT_EQ(result, UrlListSetResult::SET_OK);
  EXPECT_EQ(err_msg, "");
}

TEST_F(UrlTrustListManagerTest,
       UrlTrustListManager_SetUrlTrustListWithErrMsg_007) {
  std::string err_msg = std::string();
  std::string rule_json = R"({
    "UrlPermissionList": [{
        "scheme": "",
        "host": "example.com",
        "port": 442,
        "path": "" }]})";
  UrlListSetResult result =
      url_trust_list_manager_->SetUrlTrustListWithErrMsg(rule_json, err_msg);
  EXPECT_EQ(result, UrlListSetResult::SET_OK);
  EXPECT_EQ(err_msg, "");
}

TEST_F(UrlTrustListManagerTest, UrlTrustListManager_CheckUrlTrustList_001) {
  GURL test_url("https://test.example.com");
  UrlTrustCheckResult result =
      url_trust_list_manager_->CheckUrlTrustList(test_url);
  EXPECT_EQ(result, UrlTrustCheckResult::RESULT_ALLOW);
}

TEST_F(UrlTrustListManagerTest, UrlTrustListManager_CheckUrlTrustList_002) {
  std::string err_msg = std::string();
  std::string rule_json = R"({
    "UrlPermissionList": [{
        "scheme": "https",
        "host": "example.com",
        "port": 440,
        "path": "/test/path" }]})";
  url_trust_list_manager_->SetUrlTrustListWithErrMsg(rule_json, err_msg);
  GURL test_url("ftp://test.example.com");
  UrlTrustCheckResult result =
      url_trust_list_manager_->CheckUrlTrustList(test_url);
  EXPECT_EQ(result, UrlTrustCheckResult::RESULT_ALLOW);
}

TEST_F(UrlTrustListManagerTest, UrlTrustListManager_CheckUrlTrustList_003) {
  std::string err_msg = std::string();
  std::string rule_json = R"({
    "UrlPermissionList": [{
        "scheme": "https",
        "host": "example.com",
        "port": 440,
        "path": "/test/path" }]})";
  url_trust_list_manager_->SetUrlTrustListWithErrMsg(rule_json, err_msg);
  GURL test_url("http://baidu.com");
  UrlTrustCheckResult result =
      url_trust_list_manager_->CheckUrlTrustList(test_url);
  EXPECT_EQ(result, UrlTrustCheckResult::RESULT_DENY);
}

TEST_F(UrlTrustListManagerTest, UrlTrustListManager_CheckUrlTrustList_004) {
  std::string err_msg = std::string();
  std::string rule_json = R"({
    "UrlPermissionList": [{
        "scheme": "https",
        "host": "example.com",
        "port": 440,
        "path": "/test/path" }]})";
  url_trust_list_manager_->SetUrlTrustListWithErrMsg(rule_json, err_msg);
  GURL test_url("http://example.com");
  UrlTrustCheckResult result =
      url_trust_list_manager_->CheckUrlTrustList(test_url);
  EXPECT_EQ(result, UrlTrustCheckResult::RESULT_DENY);
}

TEST_F(UrlTrustListManagerTest, UrlTrustListManager_CheckUrlTrustList_005) {
  std::string err_msg = std::string();
  std::string rule_json = R"({
    "UrlPermissionList": [{
        "scheme": "",
        "host": "example.com",
        "port": 440,
        "path": "/test/path" }]})";
  url_trust_list_manager_->SetUrlTrustListWithErrMsg(rule_json, err_msg);
  GURL test_url("http://example.com");
  UrlTrustCheckResult result =
      url_trust_list_manager_->CheckUrlTrustList(test_url);
  EXPECT_EQ(result, UrlTrustCheckResult::RESULT_DENY);
}

TEST_F(UrlTrustListManagerTest, UrlTrustListManager_CheckUrlTrustList_006) {
  std::string err_msg = std::string();
  std::string rule_json = R"({
    "UrlPermissionList": [{
        "scheme": "",
        "host": "example.com",
        "port": 0,
        "path": "/test" }]})";
  url_trust_list_manager_->SetUrlTrustListWithErrMsg(rule_json, err_msg);
  GURL test_url("http://example.com//stest/path");
  UrlTrustCheckResult result =
      url_trust_list_manager_->CheckUrlTrustList(test_url);
  EXPECT_EQ(result, UrlTrustCheckResult::RESULT_DENY);
}

TEST_F(UrlTrustListManagerTest, UrlTrustListManager_CheckUrlTrustList_007) {
  std::string err_msg = std::string();
  std::string rule_json = R"({
    "UrlPermissionList": [{
        "scheme": "",
        "host": "example.com",
        "port": 440,
        "path": "/test" }]})";
  url_trust_list_manager_->SetUrlTrustListWithErrMsg(rule_json, err_msg);
  GURL test_url("http://example.com:440//tests/path");
  UrlTrustCheckResult result =
      url_trust_list_manager_->CheckUrlTrustList(test_url);
  EXPECT_EQ(result, UrlTrustCheckResult::RESULT_DENY);
}

TEST_F(UrlTrustListManagerTest, UrlTrustListManager_CheckUrlTrustList_008) {
  std::string err_msg = std::string();
  std::string rule_json = R"({
    "UrlPermissionList": [{
        "scheme": "",
        "host": "example.com",
        "port": 440,
        "path": "/test/" }]})";
  url_trust_list_manager_->SetUrlTrustListWithErrMsg(rule_json, err_msg);
  GURL test_url("http://example.com:440//test/path");
  UrlTrustCheckResult result =
      url_trust_list_manager_->CheckUrlTrustList(test_url);
  EXPECT_EQ(result, UrlTrustCheckResult::RESULT_ALLOW);
}

TEST_F(UrlTrustListManagerTest, UrlTrustListManager_CheckUrlTrustList_009) {
  std::string err_msg = std::string();
  std::string rule_json = R"({
    "UrlPermissionList": [{
        "scheme": "",
        "host": "example.com",
        "port": 440,
        "path": "/test" }]})";
  url_trust_list_manager_->SetUrlTrustListWithErrMsg(rule_json, err_msg);
  GURL test_url("http://example.com:440//test");
  UrlTrustCheckResult result =
      url_trust_list_manager_->CheckUrlTrustList(test_url);
  EXPECT_EQ(result, UrlTrustCheckResult::RESULT_ALLOW);
}
}  // namespace ohos_safe_browsing
