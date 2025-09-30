/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "ohos_nweb/src/ndk/scheme_handler/resource_handler.h"
#include "ohos_nweb/src/ndk/scheme_handler/resource_request.h"

#include <gmock/gmock.h>

#include "gtest/gtest.h"

using namespace testing;

class ArkWeb_ResourceHandler_Test : public Test {
 public:
  ArkWeb_ResourceHandler_Test() = default;
  ~ArkWeb_ResourceHandler_Test() = default;

  std::shared_ptr<ArkWeb_ResourceHandler_> _awrhst;
};

TEST_F(ArkWeb_ResourceHandler_Test, ArkWeb_ResourceHandler_Test_DidReceiveResponse01) {
  ArkWeb_ResourceRequest request(nullptr);
  _awrhst = std::make_shared<ArkWeb_ResourceHandler_>(&request, nullptr, "test", false);
  _awrhst->pipe_resource_handler_.reset();
  testing::internal::CaptureStderr();
  _awrhst->DidReceiveResponse(nullptr);
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("scheme_handler pipe_resource_handler_ is nullptr."), std::string::npos);
   _awrhst.reset();
}

TEST_F(ArkWeb_ResourceHandler_Test, ArkWeb_ResourceHandler_Test_DidReceiveResponse02) {
   ArkWeb_ResourceRequest request(nullptr);
  _awrhst = std::make_shared<ArkWeb_ResourceHandler_>(&request, nullptr, "test", false);
  ArkWeb_Response* null_response = nullptr;
  testing::internal::CaptureStderr();
  _awrhst->DidReceiveResponse(null_response);
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("scheme_handler response is nullptr"), std::string::npos);
  _awrhst.reset();
}

TEST_F(ArkWeb_ResourceHandler_Test, ArkWeb_ResourceHandler_Test_DidReceiveResponse03) {
   ArkWeb_ResourceRequest request(nullptr);
  _awrhst = std::make_shared<ArkWeb_ResourceHandler_>(&request, nullptr, "test", false);
  ArkWeb_Response response;
  testing::internal::CaptureStderr();
  _awrhst->DidReceiveResponse(&response);
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output.find("scheme_handler cef response is nullptr"), std::string::npos);
  _awrhst.reset();
}

TEST_F(ArkWeb_ResourceHandler_Test, ArkWeb_ResourceHandler_Test_DidReceiveResponse04) {
   ArkWeb_ResourceRequest request(nullptr);
  _awrhst = std::make_shared<ArkWeb_ResourceHandler_>(&request, nullptr, "test", false);
  ArkWeb_Response response;
  response.cef_response.reset();
  testing::internal::CaptureStderr();
  _awrhst->DidReceiveResponse(&response);
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("scheme_handler cef response is nullptr"), std::string::npos);
  _awrhst.reset();
}

TEST_F(ArkWeb_ResourceHandler_Test, ArkWeb_ResourceHandler_Test_DidReceiveData01) {
   ArkWeb_ResourceRequest request(nullptr);
  _awrhst = std::make_shared<ArkWeb_ResourceHandler_>(&request, nullptr, "test", false);
  _awrhst->pipe_resource_handler_.reset();
  testing::internal::CaptureStderr();
  const uint8_t* null_buffer = nullptr;
  _awrhst->DidReceiveData(null_buffer, 0);
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("scheme_handler pipe_resource_handler_ is nullptr."), std::string::npos);
  _awrhst.reset();
}

TEST_F(ArkWeb_ResourceHandler_Test, ArkWeb_ResourceHandler_Test_DidReceiveData02) {
   ArkWeb_ResourceRequest request(nullptr);
  _awrhst = std::make_shared<ArkWeb_ResourceHandler_>(&request, nullptr, "test", false);
  const uint8_t* null_buffer = nullptr;
  testing::internal::CaptureStderr();
  _awrhst->DidReceiveData(null_buffer, 0);
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("scheme_handler buffer is nullptr."), std::string::npos);
  _awrhst.reset();
}

TEST_F(ArkWeb_ResourceHandler_Test, ArkWeb_ResourceHandler_Test_DidReceiveData03) {
   ArkWeb_ResourceRequest request(nullptr);
  _awrhst = std::make_shared<ArkWeb_ResourceHandler_>(&request, nullptr, "test", false);
  uint8_t buf = 1;
  const uint8_t* buffer = &buf;
  int64_t len = sizeof(buf);
  testing::internal::CaptureStderr();
  _awrhst->DidReceiveData(buffer, len);
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output.find("scheme_handler buffer is nullptr."), std::string::npos);
  _awrhst.reset();
}

TEST_F(ArkWeb_ResourceHandler_Test, ArkWeb_ResourceHandler_Test_DidFinish01) {
   ArkWeb_ResourceRequest request(nullptr);
  _awrhst = std::make_shared<ArkWeb_ResourceHandler_>(&request, nullptr, "test", false);
  testing::internal::CaptureStderr();
  _awrhst->DidFinish();
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output.find("scheme_handler pipe_resource_handler_ is nullptr."), std::string::npos);
  _awrhst.reset();
}

TEST_F(ArkWeb_ResourceHandler_Test, ArkWeb_ResourceHandler_Test_DidFinish02) {
   ArkWeb_ResourceRequest request(nullptr);
  _awrhst = std::make_shared<ArkWeb_ResourceHandler_>(&request, nullptr, "test", false);
  testing::internal::CaptureStderr();
  _awrhst->pipe_resource_handler_.reset();
  _awrhst->DidFinish();
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("scheme_handler pipe_resource_handler_ is nullptr."), std::string::npos);
  _awrhst.reset();
}

TEST_F(ArkWeb_ResourceHandler_Test, ArkWeb_ResourceHandler_Test_FailWithError01) {
   ArkWeb_ResourceRequest request(nullptr);
  _awrhst = std::make_shared<ArkWeb_ResourceHandler_>(&request, nullptr, "test", false);
  testing::internal::CaptureStderr();
  _awrhst->DidFailWithError(ArkWeb_NetError::ARKWEB_NET_OK, true);
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output.find("scheme_handler pipe_resource_handler_ is nullptr."), std::string::npos);
  _awrhst.reset();
}

TEST_F(ArkWeb_ResourceHandler_Test, ArkWeb_ResourceHandler_Test_FailWithError02) {
   ArkWeb_ResourceRequest request(nullptr);
  _awrhst = std::make_shared<ArkWeb_ResourceHandler_>(&request, nullptr, "test", false);
  testing::internal::CaptureStderr();
  _awrhst->pipe_resource_handler_.reset();
  _awrhst->DidFailWithError(ArkWeb_NetError::ARKWEB_NET_OK, true);
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("scheme_handler pipe_resource_handler_ is nullptr."), std::string::npos);
  _awrhst.reset();
}