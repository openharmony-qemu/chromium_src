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

#include "ohos_nweb/src/ndk/scheme_handler/response.h"

using namespace testing;

class ArkWeb_Response_Test : public Test {
 protected:
  void SetUp() {
    std::cout << "set up for ArkWeb_Response\n";
    _awrt = std::make_shared<ArkWeb_Response_>();
  }

  void TearDown() {
    std::cout << "tear down for ArkWeb_Response\n";
    _awrt.reset();
  }

  std::shared_ptr<ArkWeb_Response_> _awrt;
};

TEST_F(ArkWeb_Response_Test, ArkWeb_Response_Test_SetUrl) {
  const char* url = new char[100]{"http://www.baidu.com"};

  _awrt->SetUrl(url);
}

TEST_F(ArkWeb_Response_Test, ArkWeb_Response_Test_GetUrl) {
  char* url = new char[100]{};

  _awrt->GetUrl(&url);
}

TEST_F(ArkWeb_Response_Test, ArkWeb_Response_Test_SetError) {
  _awrt->SetError(ArkWeb_NetError::ARKWEB_NET_OK);
}

TEST_F(ArkWeb_Response_Test, ArkWeb_Response_Test_GetError) {
  _awrt->GetError();
}

TEST_F(ArkWeb_Response_Test, ArkWeb_Response_Test_SetStatus) {
  _awrt->SetStatus(0);
}

TEST_F(ArkWeb_Response_Test, ArkWeb_Response_Test_GetStatus) {
  _awrt->GetStatus();
}

TEST_F(ArkWeb_Response_Test, ArkWeb_Response_Test_SetStatusText) {
  const char* test_status_text = new char[100]{"test_status_text"};
  _awrt->SetStatusText(test_status_text);
}

TEST_F(ArkWeb_Response_Test, ArkWeb_Response_Test_GetStatusText) {
  char* test_status_text = new char[100]{};
  _awrt->GetStatusText(&test_status_text);
}

TEST_F(ArkWeb_Response_Test, ArkWeb_Response_Test_SetMimeType) {
  const char* test_mine_type = new char[100]{"test_mine_type"};
  _awrt->SetMimeType(test_mine_type);
}

TEST_F(ArkWeb_Response_Test, ArkWeb_Response_Test_GetMimeType) {
  char* test_mine_type = new char[100]{};
  _awrt->GetMimeType(&test_mine_type);
}

TEST_F(ArkWeb_Response_Test, ArkWeb_Response_Test_SetCharset) {
  const char* test_char_set = new char[100]{"test_char_set"};
  _awrt->SetCharset(test_char_set);
}

TEST_F(ArkWeb_Response_Test, ArkWeb_Response_Test_GetCharset) {
  char* test_char_set = new char[100]{};
  _awrt->GetCharset(&test_char_set);
}

TEST_F(ArkWeb_Response_Test, ArkWeb_Response_Test_SetHeaderByName) {
  const char* test_header_by_name = new char[100]{"test_header_by_name"};
  _awrt->SetHeaderByName("Charset", test_header_by_name, true);
}

TEST_F(ArkWeb_Response_Test, ArkWeb_Response_Test_GetHeaderByName) {
  char* test_header_by_name = new char[100]{};
  _awrt->GetHeaderByName("Charset", &test_header_by_name);
}
