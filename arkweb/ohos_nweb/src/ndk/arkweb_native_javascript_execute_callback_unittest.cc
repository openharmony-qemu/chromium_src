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

#include "arkweb_native_javascript_execute_callback.h"

#include "gtest/gtest.h"
#include "nweb_value.h"
#include "nweb_web_message.h"

namespace OHOS::NWeb {

class ArkWebNativeJavaScriptExecuteCallbackTest : public ::testing::Test {
 protected:
  void SetUp() override {
    callback_called_ = false;
    last_data_.clear();
  }

  std::unique_ptr<ArkWebNativeJavaScriptExecuteCallback> CreateCallback() {
    return std::make_unique<ArkWebNativeJavaScriptExecuteCallback>(
        ArkWebNativeJavaScriptExecuteCallback::JSExecuteFunc(
            [this](const std::vector<uint8_t>& data) {
              last_data_ = data;
              callback_called_ = true;
            }));
  }

  std::vector<uint8_t> last_data_;
  bool callback_called_ = false;
};

TEST_F(ArkWebNativeJavaScriptExecuteCallbackTest, OnReceiveValue_ResultIsNull) {
  auto callback = CreateCallback();
  callback->OnReceiveValue(nullptr);
  EXPECT_FALSE(callback_called_);
}

TEST_F(ArkWebNativeJavaScriptExecuteCallbackTest, OnReceiveValue_CallbackIsNull) {
  auto message = std::make_shared<NWebMessage>();
  message->SetType(NWebValue::Type::STRING);
  ArkWebNativeJavaScriptExecuteCallback emptyCallback;
  emptyCallback.OnReceiveValue(message);
  EXPECT_FALSE(callback_called_);
}

TEST_F(ArkWebNativeJavaScriptExecuteCallbackTest, OnReceiveValue_String) {
  auto callback = CreateCallback();
  auto message = std::make_shared<NWebMessage>();
  message->SetType(NWebValue::Type::STRING);
  message->SetString("hello");
  callback->OnReceiveValue(message);
  EXPECT_TRUE(callback_called_);
  std::string result(last_data_.begin(), last_data_.end());
  EXPECT_EQ(result, "hello");
}

TEST_F(ArkWebNativeJavaScriptExecuteCallbackTest, OnReceiveValue_Binary) {
  auto callback = CreateCallback();
  auto message = std::make_shared<NWebMessage>();
  message->SetType(NWebValue::Type::BINARY);
  std::vector<uint8_t> bin = {1, 2, 3, 4};
  message->SetBinary(bin);
  callback->OnReceiveValue(message);
  EXPECT_TRUE(callback_called_);
  EXPECT_EQ(last_data_, bin);
}

TEST_F(ArkWebNativeJavaScriptExecuteCallbackTest, OnReceiveValue_Boolean) {
  auto callback = CreateCallback();
  auto message = std::make_shared<NWebMessage>();
  message->SetType(NWebValue::Type::BOOLEAN);
  callback->OnReceiveValue(message);
  EXPECT_TRUE(callback_called_);
  EXPECT_TRUE(last_data_.empty());
}

TEST_F(ArkWebNativeJavaScriptExecuteCallbackTest, OnReceiveValue_Integer) {
  auto callback = CreateCallback();
  auto message = std::make_shared<NWebMessage>();
  message->SetType(NWebValue::Type::INTEGER);
  message->SetInt64(42);
  callback->OnReceiveValue(message);
  EXPECT_TRUE(callback_called_);
  EXPECT_TRUE(last_data_.empty());
}

TEST_F(ArkWebNativeJavaScriptExecuteCallbackTest, OnReceiveValue_Double) {
  auto callback = CreateCallback();
  auto message = std::make_shared<NWebMessage>();
  message->SetType(NWebValue::Type::DOUBLE);
  callback->OnReceiveValue(message);
  EXPECT_TRUE(callback_called_);
  EXPECT_TRUE(last_data_.empty());
}

TEST_F(ArkWebNativeJavaScriptExecuteCallbackTest, OnReceiveValue_StringArray) {
  auto callback = CreateCallback();
  auto message = std::make_shared<NWebMessage>();
  message->SetType(NWebValue::Type::STRINGARRAY);
  message->SetStringArray({"a", "b"});
  callback->OnReceiveValue(message);
  EXPECT_TRUE(callback_called_);
  EXPECT_TRUE(last_data_.empty());
}

TEST_F(ArkWebNativeJavaScriptExecuteCallbackTest, OnReceiveValue_BooleanArray) {
  auto callback = CreateCallback();
  auto message = std::make_shared<NWebMessage>();
  message->SetType(NWebValue::Type::BOOLEANARRAY);
  message->SetBooleanArray({true, false});
  callback->OnReceiveValue(message);
  EXPECT_TRUE(callback_called_);
  EXPECT_TRUE(last_data_.empty());
}

TEST_F(ArkWebNativeJavaScriptExecuteCallbackTest, OnReceiveValue_DoubleArray) {
  auto callback = CreateCallback();
  auto message = std::make_shared<NWebMessage>();
  message->SetType(NWebValue::Type::DOUBLEARRAY);
  message->SetDoubleArray({1.1, 2.2});
  callback->OnReceiveValue(message);
  EXPECT_TRUE(callback_called_);
  EXPECT_TRUE(last_data_.empty());
}

TEST_F(ArkWebNativeJavaScriptExecuteCallbackTest, OnReceiveValue_Int64Array) {
  auto callback = CreateCallback();
  auto message = std::make_shared<NWebMessage>();
  message->SetType(NWebValue::Type::INT64ARRAY);
  message->SetInt64Array({10, 20});
  callback->OnReceiveValue(message);
  EXPECT_TRUE(callback_called_);
  EXPECT_TRUE(last_data_.empty());
}

TEST_F(ArkWebNativeJavaScriptExecuteCallbackTest, OnReceiveValue_None) {
  auto callback = CreateCallback();
  auto message = std::make_shared<NWebMessage>();
  message->SetType(NWebValue::Type::NONE);
  callback->OnReceiveValue(message);
  EXPECT_TRUE(callback_called_);
  EXPECT_TRUE(last_data_.empty());
}

TEST_F(ArkWebNativeJavaScriptExecuteCallbackTest, OnReceiveValue_Dictionary) {
  auto callback = CreateCallback();
  auto message = std::make_shared<NWebMessage>();
  message->SetType(NWebValue::Type::DICTIONARY);
  callback->OnReceiveValue(message);
  EXPECT_TRUE(callback_called_);
  EXPECT_TRUE(last_data_.empty());
}

TEST_F(ArkWebNativeJavaScriptExecuteCallbackTest, OnReceiveValue_List) {
  auto callback = CreateCallback();
  auto message = std::make_shared<NWebMessage>();
  message->SetType(NWebValue::Type::LIST);
  callback->OnReceiveValue(message);
  EXPECT_TRUE(callback_called_);
  EXPECT_TRUE(last_data_.empty());
}

TEST_F(ArkWebNativeJavaScriptExecuteCallbackTest, OnReceiveValue_Error) {
  auto callback = CreateCallback();
  auto message = std::make_shared<NWebMessage>();
  message->SetType(NWebValue::Type::ERROR);
  message->SetErrName("err");
  message->SetErrMsg("msg");
  callback->OnReceiveValue(message);
  EXPECT_TRUE(callback_called_);
  EXPECT_TRUE(last_data_.empty());
}

}  // namespace OHOS::NWeb
