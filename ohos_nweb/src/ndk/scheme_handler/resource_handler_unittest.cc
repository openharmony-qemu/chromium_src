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

#include "ohos_nweb/src/ndk/scheme_handler/resource_handler.h"

using namespace testing;

class ArkWeb_ResourceHandler_Test : public Test {
 protected:
  void SetUp() {
    std::cout << "set up for ArkWeb_ResourceHandler\n";
    _awrhst = std::make_shared<ArkWeb_ResourceHandler_>(nullptr, nullptr,
                                                        "test", false);
  }

  void TearDown() {
    std::cout << "tear down for  ArkWeb_ResourceHandler\n";
    _awrhst.reset();
  }

  std::shared_ptr<ArkWeb_ResourceHandler_> _awrhst;
};

TEST_F(ArkWeb_ResourceHandler_Test,
       ArkWeb_ResourceHandler_Test_DidReceiveResponse) {
  ArkWeb_Response* null_response = nullptr;

  _awrhst->DidReceiveResponse(null_response);
}

TEST_F(ArkWeb_ResourceHandler_Test,
       ArkWeb_ResourceHandler_Test_DidReceiveData) {
  const uint8_t* null_buffer = nullptr;

  _awrhst->DidReceiveData(null_buffer, 0);
}

TEST_F(ArkWeb_ResourceHandler_Test, ArkWeb_ResourceHandler_Test_DidFinish) {
  _awrhst->DidFinish();
}

TEST_F(ArkWeb_ResourceHandler_Test, ArkWeb_ResourceHandler_Test_FailWithError) {
  _awrhst->DidFailWithError(ArkWeb_NetError::ARKWEB_NET_OK);
}
