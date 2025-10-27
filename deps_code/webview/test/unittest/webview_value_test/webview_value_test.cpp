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

#include <gtest/gtest.h>
#include "webview_value.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;

namespace OHOS::NWeb {
class WebviewValueTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void WebviewValueTest::SetUpTestCase(void)
{}

void WebviewValueTest::TearDownTestCase(void)
{}

void WebviewValueTest::SetUp(void)
{}

void WebviewValueTest::TearDown(void)
{}

/**
 * @tc.name: WebviewValue_BOOLEAN_001
 * @tc.desc: Test set and get boolean/type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WebviewValueTest, WebviewValue_BOOLEAN_001, TestSize.Level1)
{
    std::shared_ptr<WebViewValue> webviewValue = std::make_shared<WebViewValue>(NWebRomValue::Type::NONE);
    NWebRomValue::Type type = webviewValue->GetType();
    EXPECT_EQ(NWebRomValue::Type::NONE, type);
    webviewValue->SetType(NWebRomValue::Type::BOOLEAN);
    webviewValue->SetBool(true);
    bool value = webviewValue->GetBool();
    type = webviewValue->GetType();
    EXPECT_EQ(NWebRomValue::Type::BOOLEAN, type);
    EXPECT_TRUE(value);
    webviewValue->SetBool(false);
    value = webviewValue->GetBool();
    EXPECT_FALSE(value);

    webviewValue->SetType(NWebRomValue::Type::BOOLEANARRAY);
    const std::vector<bool> testArray = {true, false};
    webviewValue->SetBoolArray(testArray);
    std::vector<bool> actualArray = webviewValue->GetBoolArray();
    EXPECT_EQ(testArray, actualArray);
}

/**
 * @tc.name: WebviewValue_STRING_002
 * @tc.desc: Test set and get string.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WebviewValueTest, WebviewValue_STRING_002, TestSize.Level1)
{
    std::shared_ptr<WebViewValue> webviewValue = std::make_shared<WebViewValue>(NWebRomValue::Type::NONE);
    webviewValue->SetType(NWebRomValue::Type::STRING);
    const std::string testData = "Hello,String";
    webviewValue->SetString(testData);
    std::string actual= webviewValue->GetString();
    EXPECT_STREQ(testData.c_str(), actual.c_str());

    webviewValue->SetType(NWebRomValue::Type::STRINGARRAY);
    const std::vector<std::string> testArray = {"str1", "str2", "str3"};
    webviewValue->SetStringArray(testArray);
    std::vector<std::string> actualArray = webviewValue->GetStringArray();
    EXPECT_EQ(testArray, actualArray);
}

/**
 * @tc.name: WebviewValue_INT_003
 * @tc.desc: Test set and get int.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WebviewValueTest, WebviewValue_INT_003, TestSize.Level1)
{
    std::shared_ptr<WebViewValue> webviewValue = std::make_shared<WebViewValue>(NWebRomValue::Type::NONE);
    webviewValue->SetType(NWebRomValue::Type::INTEGER);
    int testData = 12345;
    webviewValue->SetInt(testData);
    int actual = webviewValue->GetInt();
    EXPECT_EQ(testData, actual);

    int64_t testDataInt64 = 12345;
    webviewValue->SetInt64(testDataInt64);
    int64_t actualInt64 = webviewValue->GetInt64();
    EXPECT_EQ(testDataInt64, actualInt64);

    webviewValue->SetType(NWebRomValue::Type::INT64ARRAY);
    const std::vector<int64_t> testArray = {12345, 54321};
    webviewValue->SetInt64Array(testArray);
    std::vector<int64_t> actualArray = webviewValue->GetInt64Array();
    EXPECT_EQ(testArray, actualArray);
}

/**
 * @tc.name: WebviewValue_DOUBLE_004
 * @tc.desc: Test set and get double.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WebviewValueTest, WebviewValue_DOUBLE_004, TestSize.Level1)
{
    std::shared_ptr<WebViewValue> webviewValue = std::make_shared<WebViewValue>(NWebRomValue::Type::NONE);
    webviewValue->SetType(NWebRomValue::Type::DOUBLE);
    double testData = 1.234567;
    webviewValue->SetDouble(testData);
    double actual = webviewValue->GetDouble();
    EXPECT_DOUBLE_EQ(testData, actual);

    webviewValue->SetType(NWebRomValue::Type::DOUBLEARRAY);
    const std::vector<double> testArray = {1.234567, 7.654321};
    webviewValue->SetDoubleArray(testArray);
    std::vector<double> actualArray = webviewValue->GetDoubleArray();
    EXPECT_EQ(testArray, actualArray);
}

/**
 * @tc.name: WebviewValue_BINARY_005
 * @tc.desc: Test set and get binary.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WebviewValueTest, WebviewValue_BINARY_005, TestSize.Level1)
{
    std::shared_ptr<WebViewValue> webviewValue = std::make_shared<WebViewValue>(NWebRomValue::Type::NONE);
    webviewValue->SetType(NWebRomValue::Type::BINARY);
    const char* testData = "Hello,Binary";
    int length = 12;
    webviewValue->SetBinary(length, testData);
    int actualLength = -1;
    const char* actual = webviewValue->GetBinary(actualLength);
    EXPECT_EQ(length, actualLength);
    EXPECT_NE(nullptr, actual);
    if (actual && actualLength > 0) {
        std::string str1 = std::string(testData, length);
        std::string str2 = std::string(actual, actualLength);
        EXPECT_EQ(str1, str2);
    }

    std::vector<uint8_t> testArray = {0x01, 0x02, 0x03};
    webviewValue->SetBinary(testArray);
    std::vector<uint8_t> actualArray = webviewValue->GetBinary();
    EXPECT_EQ(testArray, actualArray);
}

/**
 * @tc.name: WebviewValue_DICT_006
 * @tc.desc: Test set and get dict/list.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WebviewValueTest, WebviewValue_DICT_006, TestSize.Level1)
{
    std::shared_ptr<WebViewValue> webviewValue = std::make_shared<WebViewValue>(NWebRomValue::Type::NONE);
    // !child_node
    const std::string key = "Dict";
    webviewValue->SaveDictChildValue(key);
    webviewValue->SaveListChildValue();
    // child_node
    std::shared_ptr<NWebRomValue> childNode = webviewValue->NewChildValue();
    EXPECT_NE(nullptr, childNode);
    webviewValue->SetType(NWebRomValue::Type::DICTIONARY);
    webviewValue->SaveDictChildValue(key);
    std::map<std::string, std::shared_ptr<NWebRomValue>> result = webviewValue->GetDictValue();
    EXPECT_NE(nullptr, childNode);
    EXPECT_EQ(childNode, result[key]);

    std::shared_ptr<NWebRomValue> childNode1 = webviewValue->NewChildValue();
    EXPECT_NE(nullptr, childNode);
    webviewValue->SetType(NWebRomValue::Type::LIST);
    webviewValue->SaveListChildValue();
    std::vector<std::shared_ptr<NWebRomValue>> result1 = webviewValue->GetListValue();
    EXPECT_EQ(1, result1.size());
    if (result1.size() == 1) {
        EXPECT_EQ(childNode1, result1[0]);
    }
}

/**
 * @tc.name: WebviewValue_ERR_007
 * @tc.desc: Test Err.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WebviewValueTest, WebviewValue_ERR_007, TestSize.Level1)
{
    std::shared_ptr<WebViewValue> webviewValue = std::make_shared<WebViewValue>(NWebRomValue::Type::NONE);
    webviewValue->SetType(NWebRomValue::Type::ERROR);
    const std::string name = "Hello,Err";
    const std::string msg = "ErrMsg";
    webviewValue->SetErrName(name);
    webviewValue->SetErrMsg(msg);
    std::string errName = webviewValue->GetErrName();
    std::string errMsg = webviewValue->GetErrMsg();
    EXPECT_EQ(name, errName);
    EXPECT_EQ(msg, errMsg);
}
}