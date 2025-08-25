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

#include "ohos_nweb/src/capi/arkweb_model.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <string>
#include <vector>

#include "base/test/mock_log.h"
#include "arkweb_native_object.h"

using ::testing::IsNull;
using ::testing::Return;
using ::testing::_;
std::function<
    std::shared_ptr<OHOS::NWeb::NWebValue>(std::vector<std::vector<uint8_t>>&,
                                           std::vector<size_t>&)>
CreateProxyCallback(ArkWeb_OnJavaScriptProxyCallbackWithResult callback,
                    std::string tag,
                    void* data);
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
bool CreateWebMessagePortsInternal(const char* webTag, size_t* size,
                                   std::vector<std::string>& ports,
                                   ArkWeb_WebMessagePortPtr** wPortsResult);
#ifdef __cplusplus
}
#endif
// __cplusplus

class ArkWeb_ArkWebModel_Test : public testing::Test {
 protected:
  void SetUp() {
    web_object_ = std::make_shared<OHOS::NWeb::ArkWebNativeObject>("test_tag");
    nweb_impl_ = std::make_shared<OHOS::NWeb::NWebImpl>(id_++);
  }

  void TearDown() {
    web_object_.reset();
    nweb_impl_ = nullptr;
  }
  int id_ = 0;
  std::shared_ptr<OHOS::NWeb::ArkWebNativeObject> web_object_;
  std::shared_ptr<OHOS::NWeb::NWebImpl> nweb_impl_;
  std::weak_ptr<OHOS::NWeb::NWebImpl> nweb_weak_;
};

ArkWeb_JavaScriptValuePtr MockCallback(const char* webTag,
                                       const ArkWeb_JavaScriptBridgeData* data,
                                       size_t size,
                                       void* userData) {
  if (userData == nullptr) {
    return nullptr;
  } else if (userData == (void*)1) {
    ArkWeb_JavaScriptValuePtr result = new ArkWeb_JavaScriptValue;
    result->type = ArkWeb_JavaScriptValueType::ARKWEB_JAVASCRIPT_BOOL;
    result->data = new bool(true);
    return result;
  } else if (userData == (void*)2) {
    ArkWeb_JavaScriptValuePtr result = new ArkWeb_JavaScriptValue;
    result->type = ArkWeb_JavaScriptValueType::ARKWEB_JAVASCRIPT_STRING;
    result->data = new char[6]{'H', 'e', 'l', '1', 'o', '\0'};
    return result;
  }
  return nullptr;
}

TEST_F(ArkWeb_ArkWebModel_Test, CreateProxyCallback_001) {
  auto callback = CreateProxyCallback(nullptr, "testTag", nullptr);
  std::vector<std::vector<uint8_t>> dataList;
  std::vector<size_t> dataSize;
  EXPECT_EQ(callback(dataList, dataSize), nullptr);
}

TEST_F(ArkWeb_ArkWebModel_Test, CreateProxyCallback_002) {
  auto callback = CreateProxyCallback(MockCallback, "testTag", nullptr);
  std::vector<std::vector<uint8_t>> dataList;
  std::vector<size_t> dataSize;
  EXPECT_EQ(callback(dataList, dataSize), nullptr);
}

TEST_F(ArkWeb_ArkWebModel_Test, CreateProxyCallback_003) {
  auto callback = CreateProxyCallback(MockCallback, "testTag", (void*)1);
  std::vector<std::vector<uint8_t>> dataList;
  std::vector<size_t> dataSize;
  auto result = callback(dataList, dataSize);
  ASSERT_NE(result, nullptr);
  EXPECT_EQ(result->GetType(), OHOS::NWeb::NWebValue::Type::BOOLEAN);
  EXPECT_EQ(result->GetBoolean(), true);
}
