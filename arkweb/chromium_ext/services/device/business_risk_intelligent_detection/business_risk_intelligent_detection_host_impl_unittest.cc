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

#include "services/device/business_risk_intelligent_detection/business_risk_intelligent_detection_host_impl.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "arkweb/ohos_nweb_ex/third_party/securec/include/securec.h"
#include "base/test/bind.h"
#include "content/public/test/browser_task_environment.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/receiver.h"
#include "mojo/public/cpp/bindings/remote.h"

using ::testing::_;
using ::testing::Return;

namespace device {

using DetectSimulatedClickRiskEnhancedCallback
    = base::OnceCallback<void(int32_t, int32_t, const std::string&)>;

extern "C" {
  int32_t MockDetectSimulatedClickRiskEnhancedFFI(
      const char *&result,
      const int8_t *nonceArr,
      size_t nonceLength,
      int32_t alg,
      int32_t version) {
    if (nonceLength < 24 || nonceLength > 80) {
      return 1012500007;
    }
    char* cStr = new char[8];
    strcpy_s(cStr, 8, "success");
    result = cStr;
    return 0;
  }
}

class MockBusinessRiskIntelligentDetectionHostImpl
    : public BusinessRiskIntelligentDetectionHostImpl {
 public:
  MOCK_METHOD(DetectSimulatedClickRiskEnhancedFFI, GetDetectFunc, (), (override));
};

class BusinessRiskIntelligentDetectionTest : public ::testing::Test {
 protected:
  BusinessRiskIntelligentDetectionTest()
      : receiver_(&mockImpl_, proxy_.BindNewPipeAndPassReceiver()) {}

  base::test::TaskEnvironment task_environment_;
  MockBusinessRiskIntelligentDetectionHostImpl mockImpl_;
  mojo::Remote<mojom::BusinessRiskIntelligentDetection> proxy_;
  mojo::Receiver<mojom::BusinessRiskIntelligentDetection> receiver_;
};

TEST_F(BusinessRiskIntelligentDetectionTest,
       DetectSimulatedClickRiskEnhanced_Success) {
  int32_t requestID = 1;
  int32_t algorithm = 0;
  std::vector<int8_t> nonce(48, 1);
  int32_t version = 1;

  bool callbackCalled = false;
  int32_t callbackCode = 0;
  std::string callbackResult = "";

  DetectSimulatedClickRiskEnhancedCallback callback
      = base::BindLambdaForTesting([&callbackCalled, &callbackCode, &callbackResult](
          int32_t id, int32_t code, const std::string& result) {
        callbackCalled = true;
        callbackCode = code;
        callbackResult = result;
  });

  EXPECT_CALL(mockImpl_, GetDetectFunc())
      .WillOnce(Return(MockDetectSimulatedClickRiskEnhancedFFI));

  mockImpl_.DetectSimulatedClickRiskEnhanced(requestID, algorithm, nonce,
                                             version, std::move(callback));

  EXPECT_TRUE(callbackCalled);
  EXPECT_EQ(callbackCode, 0);
  EXPECT_EQ(callbackResult, "success");
}

TEST_F(BusinessRiskIntelligentDetectionTest,
       DetectSimulatedClickRiskEnhanced_Failure) {
  int32_t requestID = 1;
  int32_t algorithm = 0;
  std::vector<int8_t> nonce(48, 1);
  int32_t version = 1;

  bool callbackCalled = false;
  int32_t callbackCode = 0;
  std::string callbackResult = "";

  DetectSimulatedClickRiskEnhancedCallback callback
      = base::BindLambdaForTesting([&callbackCalled, &callbackCode, &callbackResult](
          int32_t id, int32_t code, const std::string& result) {
        callbackCalled = true;
        callbackCode = code;
        callbackResult = result;
  });

  EXPECT_CALL(mockImpl_, GetDetectFunc())
      .WillOnce(Return(nullptr));

  mockImpl_.DetectSimulatedClickRiskEnhanced(requestID, algorithm, nonce,
                                             version, std::move(callback));

  EXPECT_TRUE(callbackCalled);
  EXPECT_EQ(callbackCode, -1);
  EXPECT_EQ(callbackResult, "");
}

TEST_F(BusinessRiskIntelligentDetectionTest,
       DetectSimulatedClickRiskEnhanced_InvalidParam) {
  int32_t requestID = 1;
  int32_t algorithm = 0;
  std::vector<int8_t> nonce = {1, 2, 3};
  int32_t version = 1;

  bool callbackCalled = false;
  int32_t callbackCode = 0;
  std::string callbackResult = "";

  DetectSimulatedClickRiskEnhancedCallback callback
      = base::BindLambdaForTesting([&callbackCalled, &callbackCode, &callbackResult](
          int32_t id, int32_t code, const std::string& result) {
        callbackCalled = true;
        callbackCode = code;
        callbackResult = result;
  });

  EXPECT_CALL(mockImpl_, GetDetectFunc())
      .WillOnce(Return(MockDetectSimulatedClickRiskEnhancedFFI));

  mockImpl_.DetectSimulatedClickRiskEnhanced(requestID, algorithm, nonce,
                                             version, std::move(callback));

  EXPECT_TRUE(callbackCalled);
  EXPECT_EQ(callbackCode, 1012500007);
  EXPECT_EQ(callbackResult, "");
}

TEST_F(BusinessRiskIntelligentDetectionTest, GetDetectFunc_Success) {
  BusinessRiskIntelligentDetectionHostImpl realImpl;
  DetectSimulatedClickRiskEnhancedFFI func = realImpl.GetDetectFunc();
  EXPECT_NE(func, nullptr);
}

}  // namespace device
