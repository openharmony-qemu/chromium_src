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

#include <gmock/gmock.h>
#include "gtest/gtest.h"

#include "ohos_nweb/src/ndk/scheme_handler/http_body_stream.h"

#include "base/logging.h"
#include "cef/include/cef_request.h"
#include "ohos_nweb/src/capi/arkweb_scheme_handler.h"
#include "ohos_nweb/src/ndk/scheme_handler/resource_request.h"

using namespace testing;

class ArkWeb_HttpBodyStream_Test : public Test {
 protected:
  void SetUp() {
    std::cout << "set up for ArkWeb_HttpBodyStream_\n";
    _awhbst = std::make_shared<ArkWeb_HttpBodyStream_>(nullptr);
  }

  void TearDown() {
    std::cout << "tear down for  ArkWeb_HttpBodyStream\n";
    _awhbst.reset();
  }

  std::shared_ptr<ArkWeb_HttpBodyStream_> _awhbst;
};
void HttpBodyStreamReadCallbackTest(const ArkWeb_HttpBodyStream* httpBodyStream,
                                    uint8_t* buffer,
                                    int bytesRead) {}
void HttpBodyStreamInitCallbackTest(const ArkWeb_HttpBodyStream* httpBodyStream,
                                    ArkWeb_NetError result) {}

TEST_F(ArkWeb_HttpBodyStream_Test, ArkWeb_HttpBodyStream_Test_SetReadCallback) {
  ArkWeb_HttpBodyStreamReadCallback null_read_callback =
      &HttpBodyStreamReadCallbackTest;

  _awhbst->SetReadCallback(null_read_callback);
}

TEST_F(ArkWeb_HttpBodyStream_Test, ArkWeb_HttpBodyStream_Test_Init) {
  ArkWeb_HttpBodyStreamInitCallback HttpBodyStreamInitCallback =
      &HttpBodyStreamInitCallbackTest;
  _awhbst->post_data_stream = nullptr;
  EXPECT_DEATH(_awhbst->Init(HttpBodyStreamInitCallback), ".*");
}

TEST_F(ArkWeb_HttpBodyStream_Test, ArkWeb_HttpBodyStream_Test_Read) {
  void* TestBuffer = nullptr;
  int64_t buf_len = 0;
  _awhbst->Read(TestBuffer, buf_len);
}

TEST_F(ArkWeb_HttpBodyStream_Test, ArkWeb_HttpBodyStream_Test_GetSize) {
  _awhbst->GetSize();
}

TEST_F(ArkWeb_HttpBodyStream_Test, ArkWeb_HttpBodyStream_Test_GetPosition) {
  _awhbst->GetPosition();
}

TEST_F(ArkWeb_HttpBodyStream_Test, ArkWeb_HttpBodyStream_Test_IsChunked) {
  _awhbst->IsChunked();
}

TEST_F(ArkWeb_HttpBodyStream_Test, ArkWeb_HttpBodyStream_Test_IsEOF) {
  _awhbst->IsEOF();
}

TEST_F(ArkWeb_HttpBodyStream_Test, ArkWeb_HttpBodyStream_Test_IsInMemory) {
  _awhbst->IsInMemory();
}

TEST_F(ArkWeb_HttpBodyStream_Test, ArkWeb_HttpBodyStream_Test_SetUserData) {
  void* TestUserData = (void*)new std::string("test");
  _awhbst->SetUserData(TestUserData);
}

TEST_F(ArkWeb_HttpBodyStream_Test, ArkWeb_HttpBodyStream_Test_GetUserData) {
  void* TestUserData = (void*)new std::string("test");
  _awhbst->SetUserData(TestUserData);
  _awhbst->GetUserData();
}

TEST_F(ArkWeb_HttpBodyStream_Test, ArkWeb_HttpBodyStream_Test_OnInitComplete) {
  _awhbst->OnInitComplete(0);
}

TEST_F(ArkWeb_HttpBodyStream_Test, ArkWeb_HttpBodyStream_Test_OnReadComplete) {
  _awhbst->OnReadComplete(nullptr, 0);
}

TEST_F(ArkWeb_HttpBodyStream_Test, ArkWeb_HttpBodyStream_Test_Reset) {
  _awhbst->Reset();
}
