/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <atomic>

#include "arkweb/ohos_adapter_ndk/mock_ndk_api/include/mock_ndk_api.h"

#define private public
#include "arkweb/ohos_adapter_ndk/vibrator_adapter/vibrator_adapter_impl.cpp"
#include "arkweb/ohos_adapter_ndk/vibrator_adapter/vibrator_adapter_impl.h"
#undef private

using namespace OHOS::NWeb;
using namespace testing;
using namespace MockNdkApi;

constexpr int32_t TEST_VIBRATOR_SUCCESS = 0;
constexpr int32_t TEST_VIBRATOR_ERR_INVALID_PARAM = -1;

class VibratorAdapterImplTest : public testing::Test {
 protected:
  void SetUp() override {
    adapter_ = std::make_unique<VibratorAdapterImpl>();
    MockVibratorSupport::EnableAll(true);
    ClearAllMocks();
  }

  void TearDown() override {
    ClearAllMocks();
    MockVibratorSupport::EnableAll(false);
    adapter_.reset();
  }

  void ClearAllMocks() {
    auto& mock = MockVibratorSupport::getInstance();
    testing::Mock::VerifyAndClearExpectations(&mock);
  }

  std::unique_ptr<VibratorAdapterImpl> adapter_;
};

TEST_F(VibratorAdapterImplTest, VibrateSuccess) {
  auto& mock = MockVibratorSupport::getInstance();
  EXPECT_CALL(mock, OH_Vibrator_PlayVibration(_, _))
      .WillOnce(Return(TEST_VIBRATOR_SUCCESS));
  
  int32_t duration = 100;
  int32_t ret = adapter_->Vibrate(duration);
  EXPECT_EQ(ret, TEST_VIBRATOR_SUCCESS);
}

TEST_F(VibratorAdapterImplTest, VibrateWithZeroDuration) {
  auto& mock = MockVibratorSupport::getInstance();
  EXPECT_CALL(mock, OH_Vibrator_PlayVibration(_, _))
      .WillOnce(Return(TEST_VIBRATOR_SUCCESS));
  
  int32_t duration = 0;
  int32_t ret = adapter_->Vibrate(duration);
  EXPECT_EQ(ret, TEST_VIBRATOR_SUCCESS);
}

TEST_F(VibratorAdapterImplTest, VibrateWithNegativeDuration) {
  auto& mock = MockVibratorSupport::getInstance();
  EXPECT_CALL(mock, OH_Vibrator_PlayVibration(_, _))
      .Times(0);
  
  int32_t duration = -100;
  int32_t ret = adapter_->Vibrate(duration);
  EXPECT_EQ(ret, TEST_VIBRATOR_ERR_INVALID_PARAM);
}

TEST_F(VibratorAdapterImplTest, VibrateWithShortDuration) {
  auto& mock = MockVibratorSupport::getInstance();
  EXPECT_CALL(mock, OH_Vibrator_PlayVibration(_, _))
      .WillOnce(Return(TEST_VIBRATOR_SUCCESS));
  
  int32_t duration = 1;
  int32_t ret = adapter_->Vibrate(duration);
  EXPECT_EQ(ret, TEST_VIBRATOR_SUCCESS);
}

TEST_F(VibratorAdapterImplTest, VibrateWithLongDuration) {
  auto& mock = MockVibratorSupport::getInstance();
  EXPECT_CALL(mock, OH_Vibrator_PlayVibration(_, _))
      .WillOnce(Return(TEST_VIBRATOR_SUCCESS));
  
  int32_t duration = 5000;
  int32_t ret = adapter_->Vibrate(duration);
  EXPECT_EQ(ret, TEST_VIBRATOR_SUCCESS);
}

TEST_F(VibratorAdapterImplTest, VibrateWithMaxInt32Duration) {
  auto& mock = MockVibratorSupport::getInstance();
  EXPECT_CALL(mock, OH_Vibrator_PlayVibration(_, _))
      .WillOnce(Return(TEST_VIBRATOR_SUCCESS));
  
  int32_t duration = INT32_MAX;
  int32_t ret = adapter_->Vibrate(duration);
  EXPECT_EQ(ret, TEST_VIBRATOR_SUCCESS);
}

TEST_F(VibratorAdapterImplTest, VibrateWithDurationOverLimit) {
  auto& mock = MockVibratorSupport::getInstance();
  EXPECT_CALL(mock, OH_Vibrator_PlayVibration(_, _))
      .WillOnce(Invoke([](int32_t duration, Vibrator_Attribute attr) -> int32_t {
        EXPECT_EQ(duration, 10000);
        return TEST_VIBRATOR_SUCCESS;
      }));
  
  int32_t duration = 15000;
  int32_t ret = adapter_->Vibrate(duration);
  EXPECT_EQ(ret, TEST_VIBRATOR_SUCCESS);
}

TEST_F(VibratorAdapterImplTest, VibrateWithDurationUnderLimit) {
  auto& mock = MockVibratorSupport::getInstance();
  EXPECT_CALL(mock, OH_Vibrator_PlayVibration(_, _))
      .WillOnce(Invoke([](int32_t duration, Vibrator_Attribute attr) -> int32_t {
        EXPECT_EQ(duration, 1);
        return TEST_VIBRATOR_SUCCESS;
      }));
  
  int32_t duration = 0;
  int32_t ret = adapter_->Vibrate(duration);
  EXPECT_EQ(ret, TEST_VIBRATOR_SUCCESS);
}

TEST_F(VibratorAdapterImplTest, VibrateFailed) {
  auto& mock = MockVibratorSupport::getInstance();
  EXPECT_CALL(mock, OH_Vibrator_PlayVibration(_, _))
      .WillOnce(Return(-2));
  
  int32_t duration = 100;
  int32_t ret = adapter_->Vibrate(duration);
  EXPECT_EQ(ret, -2);
}

TEST_F(VibratorAdapterImplTest, CancelSuccess) {
  auto& mock = MockVibratorSupport::getInstance();
  EXPECT_CALL(mock, OH_Vibrator_Cancel())
      .WillOnce(Return(TEST_VIBRATOR_SUCCESS));
  
  int32_t ret = adapter_->Cancel();
  EXPECT_EQ(ret, TEST_VIBRATOR_SUCCESS);
}

TEST_F(VibratorAdapterImplTest, CancelFailed) {
  auto& mock = MockVibratorSupport::getInstance();
  EXPECT_CALL(mock, OH_Vibrator_Cancel())
      .WillOnce(Return(-3));
  
  int32_t ret = adapter_->Cancel();
  EXPECT_EQ(ret, -3);
}

TEST_F(VibratorAdapterImplTest, VibrateThenCancel) {
  auto& mock = MockVibratorSupport::getInstance();
  EXPECT_CALL(mock, OH_Vibrator_PlayVibration(_, _))
      .WillOnce(Return(TEST_VIBRATOR_SUCCESS));
  EXPECT_CALL(mock, OH_Vibrator_Cancel())
      .WillOnce(Return(TEST_VIBRATOR_SUCCESS));
  
  int32_t duration = 200;
  int32_t ret = adapter_->Vibrate(duration);
  EXPECT_EQ(ret, TEST_VIBRATOR_SUCCESS);

  ret = adapter_->Cancel();
  EXPECT_EQ(ret, TEST_VIBRATOR_SUCCESS);
}

TEST_F(VibratorAdapterImplTest, MultipleVibrateCalls) {
  auto& mock = MockVibratorSupport::getInstance();
  EXPECT_CALL(mock, OH_Vibrator_PlayVibration(_, _))
      .Times(10)
      .WillRepeatedly(Return(TEST_VIBRATOR_SUCCESS));
  
  int32_t duration = 50;
  for (int i = 0; i < 10; i++) {
    int32_t ret = adapter_->Vibrate(duration);
    EXPECT_EQ(ret, TEST_VIBRATOR_SUCCESS);
  }
}

TEST_F(VibratorAdapterImplTest, MultipleCancelCalls) {
  auto& mock = MockVibratorSupport::getInstance();
  EXPECT_CALL(mock, OH_Vibrator_Cancel())
      .Times(5)
      .WillRepeatedly(Return(TEST_VIBRATOR_SUCCESS));
  
  for (int i = 0; i < 5; i++) {
    int32_t ret = adapter_->Cancel();
    EXPECT_EQ(ret, TEST_VIBRATOR_SUCCESS);
  }
}

TEST_F(VibratorAdapterImplTest, CancelThenVibrate) {
  auto& mock = MockVibratorSupport::getInstance();
  EXPECT_CALL(mock, OH_Vibrator_Cancel())
      .WillOnce(Return(TEST_VIBRATOR_SUCCESS));
  EXPECT_CALL(mock, OH_Vibrator_PlayVibration(_, _))
      .WillOnce(Return(TEST_VIBRATOR_SUCCESS));
  
  int32_t ret = adapter_->Cancel();
  EXPECT_EQ(ret, TEST_VIBRATOR_SUCCESS);

  int32_t duration = 100;
  ret = adapter_->Vibrate(duration);
  EXPECT_EQ(ret, TEST_VIBRATOR_SUCCESS);
}

TEST_F(VibratorAdapterImplTest, VibrateCancelCycle) {
  auto& mock = MockVibratorSupport::getInstance();
  EXPECT_CALL(mock, OH_Vibrator_PlayVibration(_, _))
      .Times(5)
      .WillRepeatedly(Return(TEST_VIBRATOR_SUCCESS));
  EXPECT_CALL(mock, OH_Vibrator_Cancel())
      .Times(5)
      .WillRepeatedly(Return(TEST_VIBRATOR_SUCCESS));
  
  for (int i = 0; i < 5; i++) {
    int32_t duration = 50 + i * 10;
    int32_t ret = adapter_->Vibrate(duration);
    EXPECT_EQ(ret, TEST_VIBRATOR_SUCCESS);

    ret = adapter_->Cancel();
    EXPECT_EQ(ret, TEST_VIBRATOR_SUCCESS);
  }
}

TEST_F(VibratorAdapterImplTest, ThreadSafetyMultipleVibrate) {
  auto& mock = MockVibratorSupport::getInstance();
  EXPECT_CALL(mock, OH_Vibrator_PlayVibration(_, _))
      .Times(1000)
      .WillRepeatedly(Return(TEST_VIBRATOR_SUCCESS));
  
  const int threadCount = 10;
  const int callsPerThread = 100;
  std::vector<std::thread> threads;
  std::atomic<int> successCount(0);
  std::atomic<int> failCount(0);

  for (int i = 0; i < threadCount; i++) {
    threads.emplace_back([this, &successCount, &failCount]() {
      for (int j = 0; j < 100; j++) {
        int32_t duration = 50 + (j % 10) * 10;
        int32_t ret = adapter_->Vibrate(duration);
        if (ret == TEST_VIBRATOR_SUCCESS) {
          successCount++;
        } else {
          failCount++;
        }
      }
    });
  }

  for (auto& thread : threads) {
    thread.join();
  }

  EXPECT_EQ(successCount + failCount, threadCount * callsPerThread);
  EXPECT_GT(successCount, 0);
}

TEST_F(VibratorAdapterImplTest, ThreadSafetyMultipleCancel) {
  auto& mock = MockVibratorSupport::getInstance();
  EXPECT_CALL(mock, OH_Vibrator_Cancel())
      .Times(500)
      .WillRepeatedly(Return(TEST_VIBRATOR_SUCCESS));
  
  const int threadCount = 10;
  const int callsPerThread = 50;
  std::vector<std::thread> threads;
  std::atomic<int> successCount(0);

  for (int i = 0; i < threadCount; i++) {
    threads.emplace_back([this, &successCount]() {
      for (int j = 0; j < 50; j++) {
        int32_t ret = adapter_->Cancel();
        if (ret == TEST_VIBRATOR_SUCCESS) {
          successCount++;
        }
      }
    });
  }

  for (auto& thread : threads) {
    thread.join();
  }

  EXPECT_EQ(successCount, threadCount * callsPerThread);
}

TEST_F(VibratorAdapterImplTest, ThreadSafetyMixedOperations) {
  auto& mock = MockVibratorSupport::getInstance();
  EXPECT_CALL(mock, OH_Vibrator_PlayVibration(_, _))
      .Times(testing::Between(400, 600))
      .WillRepeatedly(Return(TEST_VIBRATOR_SUCCESS));
  EXPECT_CALL(mock, OH_Vibrator_Cancel())
      .Times(testing::Between(400, 600))
      .WillRepeatedly(Return(TEST_VIBRATOR_SUCCESS));
  
  const int threadCount = 8;
  const int callsPerThread = 100;
  std::vector<std::thread> threads;
  std::atomic<int> vibrateCount(0);
  std::atomic<int> cancelCount(0);

  for (int i = 0; i < threadCount; i++) {
    threads.emplace_back([this, &vibrateCount, &cancelCount, i]() {
      for (int j = 0; j < 100; j++) {
        if ((i + j) % 2 == 0) {
          int32_t duration = 50 + (j % 20) * 10;
          int32_t ret = adapter_->Vibrate(duration);
          if (ret == TEST_VIBRATOR_SUCCESS) {
            vibrateCount++;
          }
        } else {
          int32_t ret = adapter_->Cancel();
          if (ret == TEST_VIBRATOR_SUCCESS) {
            cancelCount++;
          }
        }
      }
    });
  }

  for (auto& thread : threads) {
    thread.join();
  }

  EXPECT_GT(vibrateCount, 0);
  EXPECT_GT(cancelCount, 0);
  EXPECT_EQ(vibrateCount + cancelCount, threadCount * callsPerThread);
}

TEST_F(VibratorAdapterImplTest, EdgeCaseBoundaryValues) {
  auto& mock = MockVibratorSupport::getInstance();
  EXPECT_CALL(mock, OH_Vibrator_PlayVibration(_, _))
      .Times(3)
      .WillRepeatedly(Return(TEST_VIBRATOR_SUCCESS));
  
  int32_t ret = adapter_->Vibrate(1);
  EXPECT_EQ(ret, TEST_VIBRATOR_SUCCESS);

  ret = adapter_->Vibrate(INT32_MAX);
  EXPECT_EQ(ret, TEST_VIBRATOR_SUCCESS);

  ret = adapter_->Vibrate(0);
  EXPECT_EQ(ret, TEST_VIBRATOR_SUCCESS);
}

TEST_F(VibratorAdapterImplTest, VibrateAttributeUsage) {
  auto& mock = MockVibratorSupport::getInstance();
  EXPECT_CALL(mock, OH_Vibrator_PlayVibration(_, _))
      .WillOnce(Invoke([](int32_t duration, Vibrator_Attribute attr) -> int32_t {
        EXPECT_EQ(attr.usage, VIBRATOR_USAGE_RING);
        return TEST_VIBRATOR_SUCCESS;
      }));
  
  int32_t duration = 100;
  int32_t ret = adapter_->Vibrate(duration);
  EXPECT_EQ(ret, TEST_VIBRATOR_SUCCESS);
}

TEST_F(VibratorAdapterImplTest, VibrateMixedSuccessAndFailure) {
  auto& mock = MockVibratorSupport::getInstance();
  EXPECT_CALL(mock, OH_Vibrator_PlayVibration(_, _))
      .WillOnce(Return(TEST_VIBRATOR_SUCCESS))
      .WillOnce(Return(-1))
      .WillOnce(Return(TEST_VIBRATOR_SUCCESS))
      .WillOnce(Return(-2));
  
  EXPECT_EQ(adapter_->Vibrate(100), TEST_VIBRATOR_SUCCESS);
  EXPECT_EQ(adapter_->Vibrate(200), -1);
  EXPECT_EQ(adapter_->Vibrate(300), TEST_VIBRATOR_SUCCESS);
  EXPECT_EQ(adapter_->Vibrate(400), -2);
}

TEST_F(VibratorAdapterImplTest, CancelMixedSuccessAndFailure) {
  auto& mock = MockVibratorSupport::getInstance();
  EXPECT_CALL(mock, OH_Vibrator_Cancel())
      .WillOnce(Return(TEST_VIBRATOR_SUCCESS))
      .WillOnce(Return(-1))
      .WillOnce(Return(TEST_VIBRATOR_SUCCESS));
  
  EXPECT_EQ(adapter_->Cancel(), TEST_VIBRATOR_SUCCESS);
  EXPECT_EQ(adapter_->Cancel(), -1);
  EXPECT_EQ(adapter_->Cancel(), TEST_VIBRATOR_SUCCESS);
}
