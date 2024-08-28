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

#include <gmock/gmock.h>
#include "gtest/gtest.h"

#include "ohos_nweb/src/ndk/scheme_handler/resource_request.h"

using namespace testing;

class ArkWeb_ResourceRequest_Test : public Test {
 protected:
  void SetUp() {
    std::cout << "set up for ArkWeb_ResourceRequest\n";
    _awrrt = std::make_shared<ArkWeb_ResourceRequest_>(nullptr);
  }

  void TearDown() {
    std::cout << "tear down for  ArkWeb_ResourceRequest\n";
    _awrrt.reset();
  }

  std::shared_ptr<ArkWeb_ResourceRequest_> _awrrt;
};

TEST_F(ArkWeb_ResourceRequest_Test,
       ArkWeb_ResourceRequest_Test_GetHttpBodyStream) {
  ArkWeb_HttpBodyStream** null_http_body_stream = nullptr;

  _awrrt->GetHttpBodyStream(null_http_body_stream);
}

TEST_F(ArkWeb_ResourceRequest_Test, ArkWeb_ResourceRequest_Test_GetMethod) {
  char** null_string = nullptr;

  _awrrt->GetMethod(null_string);
}

TEST_F(ArkWeb_ResourceRequest_Test, ArkWeb_ResourceRequest_Test_GetUrl) {
  char** null_url = nullptr;

  _awrrt->GetUrl(null_url);
}

TEST_F(ArkWeb_ResourceRequest_Test, ArkWeb_ResourceRequest_Test_IsRedirect) {
  _awrrt->IsRedirect();
}

TEST_F(ArkWeb_ResourceRequest_Test, ArkWeb_ResourceRequest_Test_IsMainFrame) {
  _awrrt->IsMainFrame();
}

TEST_F(ArkWeb_ResourceRequest_Test,
       ArkWeb_ResourceRequest_Test_HasUserGesture) {
  _awrrt->HasUserGesture();
}

TEST_F(ArkWeb_ResourceRequest_Test, ArkWeb_ResourceRequest_Test_GetReferrer) {
  char** null_string = nullptr;
  _awrrt->GetReferrer(null_string);
}
