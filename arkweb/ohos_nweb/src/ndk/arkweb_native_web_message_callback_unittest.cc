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
#include "arkweb_native_web_message_callback.h"

#include <iostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "nweb_value.h"
#include "nweb_web_message.h"

using ::testing::_;
using ::testing::Return;

namespace OHOS::NWeb {

class MockNWebHapValue : public NWebHapValue {
 public:
  MOCK_METHOD(Type, GetType, (), (override));
  MOCK_METHOD(std::string, GetString, (), (override));
  MOCK_METHOD(std::vector<uint8_t>, GetBinary, (), (override));

  void SetType(Type) override {}
  int GetInt() override { return 0; }
  void SetInt(int) override {}
  bool GetBool() override { return false; }
  void SetBool(bool) override {}
  double GetDouble() override { return 0.0; }
  void SetDouble(double) override {}
  void SetString(const std::string&) override {}
  const char* GetBinary(int& length) override {
    length = 0;
    return nullptr;
  }
  void SetBinary(int, const char*) override {}
  std::map<std::string, std::shared_ptr<NWebHapValue>> GetDictValue() override {
    return {};
  }
  std::vector<std::shared_ptr<NWebHapValue>> GetListValue() override {
    return {};
  }
  std::shared_ptr<NWebHapValue> NewChildValue() override { return nullptr; }
  void SaveDictChildValue(const std::string&) override {}
  void SaveListChildValue() override {}
  int64_t GetInt64() override { return 0; }
  void SetInt64(int64_t) override {}
  void SetBinary(const std::vector<uint8_t>&) override {}
  std::vector<bool> GetBoolArray() override { return {}; }
  void SetBoolArray(const std::vector<bool>&) override {}
  std::vector<int64_t> GetInt64Array() override { return {}; }
  void SetInt64Array(const std::vector<int64_t>&) override {}
  std::vector<double> GetDoubleArray() override { return {}; }
  void SetDoubleArray(const std::vector<double>&) override {}
  std::vector<std::string> GetStringArray() override { return {}; }
  void SetStringArray(const std::vector<std::string>&) override {}
  std::string GetErrMsg() override { return ""; }
  void SetErrMsg(const std::string&) override {}
  std::string GetErrName() override { return ""; }
  void SetErrName(const std::string&) override {}
};

class ArkWebNativeWebMessageCallbackTest : public ::testing::Test {
 protected:
  void SetUp() override {
    callback_called = false;
    last_message = ArkWeb_WebMessage{};
  }

  void TearDown() override {
    callback_called = false;
    last_message = ArkWeb_WebMessage{};
  }

  std::unique_ptr<ArkWebNativeWebMessageCallback> CreateCallback() {
    return std::make_unique<ArkWebNativeWebMessageCallback>(
        ArkWebNativeWebMessageCallback::WebMessageFunc(
            [this](const ArkWeb_WebMessagePtr message) {
              last_message = *message;
              callback_called = true;
            }));
  }

  ArkWeb_WebMessage last_message;
  bool callback_called = false;
};

TEST_F(ArkWebNativeWebMessageCallbackTest, OnReceiveValue_ResultIsNull) {
  auto callback = CreateCallback();
  callback->OnReceiveValue(nullptr);
  EXPECT_FALSE(callback_called);
}

TEST_F(ArkWebNativeWebMessageCallbackTest, OnReceiveValue_CallbackIsNull) {
  auto mockMessage = std::make_shared<NWebMessage>();
  ArkWebNativeWebMessageCallback emptyCallback;
  emptyCallback.OnReceiveValue(mockMessage);
  EXPECT_FALSE(callback_called);
}

TEST_F(ArkWebNativeWebMessageCallbackTest, OnReceiveValue_StringTypeSuccess) {
  auto callback = CreateCallback();
  auto mockMessage = std::make_shared<NWebMessage>();
  mockMessage->SetType(NWebValue::Type::STRING);
  mockMessage->SetString("HelloArkWeb");
  callback->OnReceiveValue(mockMessage);

  EXPECT_TRUE(callback_called);
  EXPECT_EQ(last_message.webMessageType, ArkWeb_WebMessageType::ARKWEB_STRING);
}

TEST_F(ArkWebNativeWebMessageCallbackTest, OnReceiveValue_BinaryTypeSuccess) {
  auto callback = CreateCallback();
  auto mockMessage = std::make_shared<NWebMessage>();
  mockMessage->SetType(NWebValue::Type::BINARY);
  std::vector<uint8_t> bin{1, 2, 3, 4, 5};
  mockMessage->SetBinary(bin);
  callback->OnReceiveValue(mockMessage);

  EXPECT_TRUE(callback_called);
  EXPECT_EQ(last_message.webMessageType, ArkWeb_WebMessageType::ARKWEB_BUFFER);
  EXPECT_EQ(last_message.dataLength, bin.size());
}

TEST_F(ArkWebNativeWebMessageCallbackTest, OnReceiveValue_BooleanType) {
  auto callback = CreateCallback();
  auto mockMessage = std::make_shared<NWebMessage>(NWebValue::Type::BOOLEAN);
  callback->OnReceiveValue(mockMessage);
  EXPECT_FALSE(callback_called);
}

TEST_F(ArkWebNativeWebMessageCallbackTest, OnReceiveValue_IntegerType) {
  auto callback = CreateCallback();
  auto mockMessage = std::make_shared<NWebMessage>(NWebValue::Type::INTEGER);
  mockMessage->SetInt64(123);
  callback->OnReceiveValue(mockMessage);
  EXPECT_FALSE(callback_called);
}

TEST_F(ArkWebNativeWebMessageCallbackTest, OnReceiveValue_DoubleType) {
  auto callback = CreateCallback();
  auto mockMessage = std::make_shared<NWebMessage>(NWebValue::Type::DOUBLE);
  callback->OnReceiveValue(mockMessage);
  EXPECT_FALSE(callback_called);
}

TEST_F(ArkWebNativeWebMessageCallbackTest, OnReceiveValue_ErrorType) {
  auto callback = CreateCallback();
  auto mockMessage = std::make_shared<NWebMessage>(NWebValue::Type::ERROR);
  mockMessage->SetErrName("ERR_NAME");
  mockMessage->SetErrMsg("ERR_MSG");
  callback->OnReceiveValue(mockMessage);
  EXPECT_FALSE(callback_called);
}

TEST_F(ArkWebNativeWebMessageCallbackTest, OnReceiveValue_StringArrayType) {
  auto callback = CreateCallback();
  auto mockMessage =
      std::make_shared<NWebMessage>(NWebValue::Type::STRINGARRAY);
  mockMessage->SetStringArray({"a", "b", "c"});
  callback->OnReceiveValue(mockMessage);
  EXPECT_FALSE(callback_called);
}

TEST_F(ArkWebNativeWebMessageCallbackTest, OnReceiveValue_BooleanArrayType) {
  auto callback = CreateCallback();
  auto mockMessage =
      std::make_shared<NWebMessage>(NWebValue::Type::BOOLEANARRAY);
  mockMessage->SetBooleanArray({true, false});
  callback->OnReceiveValue(mockMessage);
  EXPECT_FALSE(callback_called);
}

TEST_F(ArkWebNativeWebMessageCallbackTest, OnReceiveValue_DoubleArrayType) {
  auto callback = CreateCallback();
  auto mockMessage =
      std::make_shared<NWebMessage>(NWebValue::Type::DOUBLEARRAY);
  mockMessage->SetDoubleArray({1.1, 2.2});
  callback->OnReceiveValue(mockMessage);
  EXPECT_FALSE(callback_called);
}

TEST_F(ArkWebNativeWebMessageCallbackTest, OnReceiveValue_Int64ArrayType) {
  auto callback = CreateCallback();
  auto mockMessage = std::make_shared<NWebMessage>(NWebValue::Type::INT64ARRAY);
  mockMessage->SetInt64Array({100, 200});
  callback->OnReceiveValue(mockMessage);
  EXPECT_FALSE(callback_called);
}

TEST_F(ArkWebNativeWebMessageCallbackTest, OnReceiveValueV2_Empty) {
  auto mockHapValue = std::make_shared<MockNWebHapValue>();
  EXPECT_CALL(*mockHapValue, GetType())
      .WillRepeatedly(Return(NWebHapValue::Type::NONE));
  auto callback = CreateCallback();
  callback->OnReceiveValueV2(mockHapValue);
  EXPECT_FALSE(callback_called);
}

TEST_F(ArkWebNativeWebMessageCallbackTest, OnReceiveValueV2_StringType) {
  auto mockHapValue = std::make_shared<MockNWebHapValue>();
  EXPECT_CALL(*mockHapValue, GetType())
      .WillRepeatedly(Return(NWebHapValue::Type::STRING));
  EXPECT_CALL(*mockHapValue, GetString()).WillRepeatedly(Return("abc"));
  auto callback = CreateCallback();
  callback->OnReceiveValueV2(mockHapValue);
  EXPECT_TRUE(callback_called);
  EXPECT_EQ(last_message.webMessageType, ArkWeb_WebMessageType::ARKWEB_STRING);
}

TEST_F(ArkWebNativeWebMessageCallbackTest, OnReceiveValueV2_BinaryType) {
  auto mockHapValue = std::make_shared<MockNWebHapValue>();
  std::vector<uint8_t> bin{10, 20, 30};
  EXPECT_CALL(*mockHapValue, GetType())
      .WillRepeatedly(Return(NWebHapValue::Type::BINARY));
  EXPECT_CALL(*mockHapValue, GetBinary()).WillRepeatedly(Return(bin));
  auto callback = CreateCallback();
  callback->OnReceiveValueV2(mockHapValue);
  EXPECT_TRUE(callback_called);
  EXPECT_EQ(last_message.webMessageType, ArkWeb_WebMessageType::ARKWEB_BUFFER);
}

}  // namespace OHOS::NWeb