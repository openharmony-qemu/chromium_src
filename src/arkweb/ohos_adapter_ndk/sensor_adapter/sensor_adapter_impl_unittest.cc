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
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>
#include <thread>

#include "arkweb/ohos_adapter_ndk/interfaces/ohos_adapter_helper.h"
#include "arkweb/ohos_adapter_ndk/ohos_adapter_helper_ext.h"

#include "third_party/bounds_checking_function/include/securec.h"
#if BUILDFLAG(ARKWEB_TEST)
#define private public
#include "arkweb/ohos_adapter_ndk/sensor_adapter/sensor_adapter_impl.cpp"
#include "arkweb/ohos_adapter_ndk/sensor_adapter/sensor_adapter_impl.h"
#undef private
#else
#include "arkweb/ohos_adapter_ndk/sensor_adapter/sensor_adapter_impl.cpp"
#include "arkweb/ohos_adapter_ndk/sensor_adapter/sensor_adapter_impl.h"
#endif
#include "arkweb/ohos_adapter_ndk/mock_ndk_api/include/mock_ndk_api.h"

using namespace MockNdkApi;
using namespace OHOS::NWeb;
using namespace testing;

struct Sensor_Event {
    int32_t sensorType;
    int64_t timeStamp;
    float data[6];
    uint32_t dataLength;
};

const size_t SENSOR_SUBSCRIBER_SIZE = 64;
const size_t SENSOR_SUBSCRIPTION_ID_SIZE = 64;
const size_t SENSOR_SUBSCRIPTION_ATTRIBUTE_SIZE = 64;
class SensorAdapterImplTest : public testing::Test {
 public:
  void SetUp() override {
    sensorAdapter_ = std::make_shared<SensorAdapterImpl>();
    MockOhSensorSupport::EnableAll(true);
    ClearAllMocks();
  }

  void TearDown() {
    ClearAllMocks();
    MockOhSensorSupport::EnableAll(false);
    {
      std::lock_guard<std::mutex> lock(sensorAdapter_->sensorCallbackMapMutex_);
      sensorAdapter_->sensorCallbackMap.clear();
    }
    ClearSensorSubscriberMap();
  }

  void ClearSensorSubscriberMap() {
    std::lock_guard<std::mutex> lock(sensorAdapter_->sensorSubscriberMapMutex_);
    for (auto it = sensorSubscriberMap.begin();
         it != sensorSubscriberMap.end();) {
      if (it->second) {
        if (it->second->subscriber) {
          MockOhSensorSupport::getInstance().OH_Sensor_DestroySubscriber(
              it->second->subscriber);
        }
        if (it->second->id) {
          MockOhSensorSupport::getInstance().OH_Sensor_DestroySubscriptionId(
              it->second->id);
        }
        if (it->second->attr) {
          MockOhSensorSupport::getInstance()
              .OH_Sensor_DestroySubscriptionAttribute(it->second->attr);
        }
      }
      it = sensorSubscriberMap.erase(it);
    }
  }

  void ClearAllMocks() {
    auto& mock = MockOhSensorSupport::getInstance();
    testing::Mock::VerifyAndClearExpectations(&mock);
  }

  static Sensor_Info** CreateTestSensorInfos(
      const std::vector<Sensor_Type>& sensorTypes) {
    uint32_t count = sensorTypes.size();
    Sensor_Info** infos = new Sensor_Info*[count];
    for (uint32_t i = 0; i < count; ++i) {
      infos[i] = reinterpret_cast<Sensor_Info*>(new int(sensorTypes[i]));
    }
    return infos;
  }

  static void DestroyTestSensorInfos(Sensor_Info** infos, uint32_t count) {
    if (infos == nullptr) {
      return;
    }
    for (uint32_t i = 0; i < count; ++i) {
      delete reinterpret_cast<int*>(infos[i]);
    }
    delete[] infos;
  }

  void PrepareSubscribeSensor() {
    auto& mock = MockOhSensorSupport::getInstance();
    EXPECT_CALL(mock, OH_Sensor_CreateSubscriber())
        .WillOnce(Invoke([]() -> Sensor_Subscriber* {
          return reinterpret_cast<Sensor_Subscriber*>(
              new char[SENSOR_SUBSCRIBER_SIZE]);
        }));
    EXPECT_CALL(mock, OH_SensorSubscriber_SetCallback(_, _))
        .WillOnce(Return(0));
    EXPECT_CALL(mock, OH_Sensor_CreateSubscriptionId())
        .WillOnce(Invoke([]() -> Sensor_SubscriptionId* {
          return reinterpret_cast<Sensor_SubscriptionId*>(
              new char[SENSOR_SUBSCRIPTION_ID_SIZE]);
        }));
    EXPECT_CALL(mock, OH_SensorSubscriptionId_SetType(_, _))
        .WillOnce(Return(0));
    EXPECT_CALL(mock, OH_Sensor_CreateSubscriptionAttribute())
        .WillOnce(Invoke([]() -> Sensor_SubscriptionAttribute* {
          return reinterpret_cast<Sensor_SubscriptionAttribute*>(
              new char[SENSOR_SUBSCRIPTION_ATTRIBUTE_SIZE]);
        }));
    EXPECT_CALL(mock, OH_SensorSubscriptionAttribute_SetSamplingInterval(_, _))
        .WillOnce(Return(0));
    EXPECT_CALL(mock, OH_Sensor_Subscribe(_, _, _))
        .WillOnce(Return(static_cast<Sensor_Result>(0)));
  }

  std::shared_ptr<SensorAdapterImpl> sensorAdapter_;
};

class MockSensorCallbackAdapter : public SensorCallbackAdapter {
 public:
  ~MockSensorCallbackAdapter() = default;
  MOCK_METHOD(void,UpdateOhosSensorData,
      (double timestamp,double value1,double value2,
      double value3,double value4),
      (override));
};

TEST_F(SensorAdapterImplTest, UpdateOhosSensorData_CallbackAdapter) {
  auto mockCallback = std::make_shared<MockSensorCallbackAdapter>();
  EXPECT_CALL(*mockCallback,
      UpdateOhosSensorData(123456789.0, 1.0, 2.0, 3.0, 4.0)).Times(1);
  auto sensorCallback = std::make_shared<SensorCallbackImpl>(mockCallback);
  EXPECT_NO_FATAL_FAILURE(
      sensorCallback->UpdateOhosSensorData(123456789.0, 1.0, 2.0, 3.0, 4.0));
}

TEST_F(SensorAdapterImplTest, UpdateOhosSensorData_NullCallbackAdapter) {
  auto sensorCallback = std::make_shared<SensorCallbackImpl>(nullptr);
  EXPECT_NO_FATAL_FAILURE(
      sensorCallback->UpdateOhosSensorData(123456789.0, 1.0, 2.0, 3.0, 4.0));
  SUCCEED();
}

TEST_F(SensorAdapterImplTest, IsOhosSensorSupported_UnsupportedSensorTypeId) {
  int32_t sensorTypeId = 1;
  int32_t result = sensorAdapter_->IsOhosSensorSupported(sensorTypeId);
  EXPECT_EQ(result, SENSOR_ERROR);
}

TEST_F(SensorAdapterImplTest, IsOhosSensorSupported_GetSensorCountFailed) {
  int32_t sensorTypeId = 2;
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_Sensor_GetInfos(nullptr, NotNull()))
      .Times(2)
      .WillOnce(Return(static_cast<Sensor_Result>(-1)));
  EXPECT_CALL(mock, OH_Sensor_CreateInfos(0)).WillOnce(Return(nullptr));
  EXPECT_CALL(mock, OH_Sensor_DestroyInfos(nullptr, 0)).WillOnce(Return(0));
  int32_t result = sensorAdapter_->IsOhosSensorSupported(sensorTypeId);
  EXPECT_EQ(result, SENSOR_ERROR);
}

TEST_F(SensorAdapterImplTest, IsOhosSensorSupported_GetSensorInfosFailed) {
  int32_t sensorTypeId = 2;
  uint32_t expectedCount = 2;
  bool isFirstCall = true;
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_Sensor_GetInfos(nullptr, NotNull()))
      .WillOnce(
          Invoke([&](Sensor_Info** infos, uint32_t* count) -> Sensor_Result {
            *count = expectedCount;
            return static_cast<Sensor_Result>(0);
          }));
  EXPECT_CALL(mock, OH_Sensor_CreateInfos(expectedCount))
      .WillOnce(Invoke([](uint32_t count) -> Sensor_Info** {
        return new Sensor_Info*[count];
      }));
  EXPECT_CALL(mock, OH_Sensor_GetInfos(NotNull(), NotNull()))
      .WillOnce(Return(static_cast<Sensor_Result>(-1)));
  bool destroyCalled = false;
  EXPECT_CALL(mock, OH_Sensor_DestroyInfos(NotNull(), expectedCount))
      .WillOnce(Invoke([&](Sensor_Info** infos, uint32_t count) -> int32_t {
        destroyCalled = true;
        delete[] infos;
        return 0;
      }));
  int32_t result = sensorAdapter_->IsOhosSensorSupported(sensorTypeId);
  EXPECT_EQ(result, SENSOR_ERROR);
  EXPECT_TRUE(destroyCalled);
}

TEST_F(SensorAdapterImplTest, IsOhosSensorSupported_NoMatchingSensor) {
  int32_t sensorTypeId = 2;
  Sensor_Type targetType = SENSOR_TYPE_ACCELEROMETER;
  std::vector<Sensor_Type> deviceSensors = {SENSOR_TYPE_GYROSCOPE,
      SENSOR_TYPE_MAGNETIC_FIELD};
  uint32_t sensorCount = deviceSensors.size();
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_Sensor_GetInfos(nullptr, NotNull()))
      .WillOnce(
          Invoke([&](Sensor_Info** infos, uint32_t* count) -> Sensor_Result {
            *count = sensorCount;
            return static_cast<Sensor_Result>(0);
          }));
  Sensor_Info** testInfos = CreateTestSensorInfos(deviceSensors);
  EXPECT_CALL(mock, OH_Sensor_CreateInfos(sensorCount))
      .WillOnce(Return(testInfos));
  EXPECT_CALL(mock, OH_Sensor_GetInfos(testInfos, NotNull()))
      .WillOnce(
          Invoke([&](Sensor_Info** infos, uint32_t* count) -> Sensor_Result {
            *count = sensorCount;
            return static_cast<Sensor_Result>(0);
          }));
  EXPECT_CALL(mock, OH_SensorInfo_GetType(NotNull(), NotNull()))
      .Times(sensorCount)
      .WillRepeatedly(
          Invoke([](Sensor_Info* info, Sensor_Type* type) -> int32_t {
            *type = static_cast<Sensor_Type>(*reinterpret_cast<int*>(info));
            return 0;
          }));
  bool destroyCalled = false;
  EXPECT_CALL(mock, OH_Sensor_DestroyInfos(testInfos, sensorCount))
      .WillOnce(Invoke([&](Sensor_Info** infos, uint32_t count) -> int32_t {
        destroyCalled = true;
        DestroyTestSensorInfos(infos, count);
        return 0;
      }));
  int32_t result = sensorAdapter_->IsOhosSensorSupported(sensorTypeId);
  EXPECT_EQ(result, SENSOR_ERROR);
  EXPECT_TRUE(destroyCalled);
}

TEST_F(SensorAdapterImplTest, IsOhosSensorSupported_MatchingSensor) {
  int32_t sensorTypeId = 2;
  Sensor_Type targetType = SENSOR_TYPE_ACCELEROMETER;
  std::vector<Sensor_Type> deviceSensors = {SENSOR_TYPE_GYROSCOPE, targetType,
      SENSOR_TYPE_MAGNETIC_FIELD};
  uint32_t sensorCount = deviceSensors.size();
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_Sensor_GetInfos(nullptr, NotNull()))
      .WillOnce(
          Invoke([&](Sensor_Info** infos, uint32_t* count) -> Sensor_Result {
            *count = sensorCount;
            return static_cast<Sensor_Result>(0);
          }));
  Sensor_Info** testInfos = CreateTestSensorInfos(deviceSensors);
  EXPECT_CALL(mock, OH_Sensor_CreateInfos(sensorCount))
      .WillOnce(Return(testInfos));
  EXPECT_CALL(mock, OH_Sensor_GetInfos(testInfos, NotNull()))
      .WillOnce(
          Invoke([&](Sensor_Info** infos, uint32_t* count) -> Sensor_Result {
            *count = sensorCount;
            return static_cast<Sensor_Result>(0);
          }));
  EXPECT_CALL(mock, OH_SensorInfo_GetType(NotNull(), NotNull()))
      .Times(2)
      .WillRepeatedly(
          Invoke([](Sensor_Info* info, Sensor_Type* type) -> int32_t {
            *type = static_cast<Sensor_Type>(*reinterpret_cast<int*>(info));
            return 0;
          }));
  bool destroyCalled = false;
  EXPECT_CALL(mock, OH_Sensor_DestroyInfos(testInfos, sensorCount))
      .WillOnce(Invoke([&](Sensor_Info** infos, uint32_t count) -> int32_t {
        destroyCalled = true;
        DestroyTestSensorInfos(infos, count);
        return 0;
      }));
  int32_t result = sensorAdapter_->IsOhosSensorSupported(sensorTypeId);
  EXPECT_EQ(result, static_cast<Sensor_Result>(0));
  EXPECT_TRUE(destroyCalled);
}

TEST_F(SensorAdapterImplTest, IsOhosSensorSupported_GetSensorTypeFailed) {
  int32_t sensorTypeId = 2;
  std::vector<Sensor_Type> deviceSensors = {SENSOR_TYPE_ACCELEROMETER};
  uint32_t sensorCount = deviceSensors.size();
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_Sensor_GetInfos(nullptr, NotNull()))
      .WillOnce(
          Invoke([&](Sensor_Info** infos, uint32_t* count) -> Sensor_Result {
            *count = sensorCount;
            return static_cast<Sensor_Result>(0);
          }));
  Sensor_Info** testInfos = CreateTestSensorInfos(deviceSensors);
  EXPECT_CALL(mock, OH_Sensor_CreateInfos(sensorCount))
      .WillOnce(Return(testInfos));
  EXPECT_CALL(mock, OH_Sensor_GetInfos(testInfos, NotNull()))
      .WillOnce(Return(static_cast<Sensor_Result>(0)));
  EXPECT_CALL(mock, OH_SensorInfo_GetType(NotNull(), NotNull()))
      .WillOnce(Return(-1));
  bool destroyCalled = false;
  EXPECT_CALL(mock, OH_Sensor_DestroyInfos(testInfos, sensorCount))
      .WillOnce(Invoke([&](Sensor_Info** infos, uint32_t count) -> int32_t {
        destroyCalled = true;
        DestroyTestSensorInfos(infos, count);
        return 0;
      }));
  int32_t result = sensorAdapter_->IsOhosSensorSupported(sensorTypeId);
  EXPECT_EQ(result, SENSOR_ERROR);
  EXPECT_TRUE(destroyCalled);
}

TEST_F(SensorAdapterImplTest, GetOhosSensorReportingMode_ContinuousReporting) {
  std::vector<int32_t> continuousSensorTypes = {2, 3, 4, 5, 6, 8, 9, 11};
  for (int32_t sensorTypeId : continuousSensorTypes) {
    int32_t result = sensorAdapter_->GetOhosSensorReportingMode(sensorTypeId);
    EXPECT_EQ(result, SENSOR_DATA_REPORT_CONTINUOUS);
  }
}

TEST_F(SensorAdapterImplTest, GetOhosSensorReportingMode_InvalidSensorType) {
  std::vector<int32_t> invalidSensorTypes = {0, 999};
  for (int32_t sensorTypeId : invalidSensorTypes) {
    int32_t result = sensorAdapter_->GetOhosSensorReportingMode(sensorTypeId);
    EXPECT_EQ(result, -1);
  }
}

TEST_F(SensorAdapterImplTest,GetOhosSensorDefaultSupportedFrequency_ConvertSensorTypeSuccess) {
  auto adapterImpl = std::make_shared<SensorAdapterImpl>();
  EXPECT_NE(adapterImpl, nullptr);
  double result = adapterImpl->GetOhosSensorDefaultSupportedFrequency(3);
  EXPECT_EQ(result, 5.0);
}

TEST_F(SensorAdapterImplTest,GetOhosSensorDefaultSupportedFrequency_ReturnZero) {
  auto adapterImpl = std::make_shared<SensorAdapterImpl>();
  EXPECT_NE(adapterImpl, nullptr);
  double result = adapterImpl->GetOhosSensorDefaultSupportedFrequency(1);
  EXPECT_EQ(result, 0.0);
}

TEST_F(SensorAdapterImplTest, GetOhosSensorMinSupportedFrequency_001) {
  double result = sensorAdapter_->GetOhosSensorMinSupportedFrequency(1);
  EXPECT_DOUBLE_EQ(result, 0.0);
}

TEST_F(SensorAdapterImplTest, GetOhosSensorMinSupportedFrequency_002) {
  int32_t sensorTypeId = 2;
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_Sensor_GetInfos(nullptr, NotNull()))
      .Times(2)
      .WillOnce(
          Invoke([&](Sensor_Info** infos, uint32_t* count) -> Sensor_Result {
            *count = 0;
            return static_cast<Sensor_Result>(-1);
          }));
  EXPECT_CALL(mock, OH_Sensor_CreateInfos(0)).WillOnce(Return(nullptr));
  EXPECT_CALL(mock, OH_Sensor_DestroyInfos(nullptr, 0)).WillOnce(Return(0));
  double result = sensorAdapter_->GetOhosSensorMinSupportedFrequency(sensorTypeId);
  EXPECT_DOUBLE_EQ(result, 0.0);
}

TEST_F(SensorAdapterImplTest, GetOhosSensorMinSupportedFrequency_003) {
  int32_t sensorTypeId = 2;
  uint32_t expectedCount = 2;
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_Sensor_GetInfos(nullptr, NotNull()))
      .WillOnce(
          Invoke([&](Sensor_Info** infos, uint32_t* count) -> Sensor_Result {
            *count = expectedCount;
            return static_cast<Sensor_Result>(0);
          }));
  EXPECT_CALL(mock, OH_Sensor_CreateInfos(expectedCount))
      .WillOnce(Invoke([](uint32_t count) -> Sensor_Info** {
        return new Sensor_Info*[count];
      }));
  EXPECT_CALL(mock, OH_Sensor_GetInfos(NotNull(), NotNull()))
      .WillOnce(Return(static_cast<Sensor_Result>(-1)));
  bool destroyCalled = false;
  EXPECT_CALL(mock, OH_Sensor_DestroyInfos(NotNull(), expectedCount))
      .WillOnce(Invoke([&](Sensor_Info** infos, uint32_t count) -> int32_t {
        destroyCalled = true;
        delete[] infos;
        return 0;
      }));
  double result = sensorAdapter_->GetOhosSensorMinSupportedFrequency(sensorTypeId);
  EXPECT_DOUBLE_EQ(result, 0.0);
  EXPECT_TRUE(destroyCalled);
}

TEST_F(SensorAdapterImplTest, GetOhosSensorMinSupportedFrequency_004) {
  int32_t sensorTypeId = 2;
  std::vector<Sensor_Type> deviceSensors = {SENSOR_TYPE_GYROSCOPE,SENSOR_TYPE_MAGNETIC_FIELD};
  uint32_t sensorCount = deviceSensors.size();
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_Sensor_GetInfos(nullptr, NotNull()))
      .WillOnce(
          Invoke([&](Sensor_Info** infos, uint32_t* count) -> Sensor_Result {
            *count = sensorCount;
            return static_cast<Sensor_Result>(0);
          }));
  Sensor_Info** testInfos = CreateTestSensorInfos(deviceSensors);
  EXPECT_CALL(mock, OH_Sensor_CreateInfos(sensorCount)).WillOnce(Return(testInfos));
  EXPECT_CALL(mock, OH_Sensor_GetInfos(testInfos, NotNull()))
      .WillOnce(
          Invoke([&](Sensor_Info** infos, uint32_t* count) -> Sensor_Result {
            *count = sensorCount;
            return static_cast<Sensor_Result>(0);
          }));
  EXPECT_CALL(mock, OH_SensorInfo_GetType(NotNull(), NotNull()))
      .WillRepeatedly(Return(-1));
  bool destroyCalled = false;
  EXPECT_CALL(mock, OH_Sensor_DestroyInfos(testInfos, sensorCount))
      .WillOnce(Invoke([&](Sensor_Info** infos, uint32_t count) -> int32_t {
        destroyCalled = true;
        DestroyTestSensorInfos(infos, count);
        return 0;
      }));
  double result = sensorAdapter_->GetOhosSensorMinSupportedFrequency(sensorTypeId);
  EXPECT_DOUBLE_EQ(result, 0.0);
  EXPECT_TRUE(destroyCalled);
}

TEST_F(SensorAdapterImplTest, GetOhosSensorMinSupportedFrequency_005) {
  int32_t sensorTypeId = 2;
  std::vector<Sensor_Type> deviceSensors = {SENSOR_TYPE_GYROSCOPE,SENSOR_TYPE_MAGNETIC_FIELD};
  uint32_t sensorCount = deviceSensors.size();
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_Sensor_GetInfos(nullptr, NotNull()))
      .WillOnce(
          Invoke([&](Sensor_Info** infos, uint32_t* count) -> Sensor_Result {
            *count = sensorCount;
            return static_cast<Sensor_Result>(0);
          }));
  Sensor_Info** testInfos = CreateTestSensorInfos(deviceSensors);
  EXPECT_CALL(mock, OH_Sensor_CreateInfos(sensorCount)).WillOnce(Return(testInfos));
  EXPECT_CALL(mock, OH_Sensor_GetInfos(testInfos, NotNull()))
      .WillOnce(
          Invoke([&](Sensor_Info** infos, uint32_t* count) -> Sensor_Result {
            *count = sensorCount;
            return static_cast<Sensor_Result>(0);
          }));
  EXPECT_CALL(mock, OH_SensorInfo_GetType(NotNull(), NotNull()))
      .Times(sensorCount)
      .WillRepeatedly(
          Invoke([](Sensor_Info* info, Sensor_Type* type) -> int32_t {
            *type = static_cast<Sensor_Type>(*reinterpret_cast<int*>(info));
            return 0;
          }));
  bool destroyCalled = false;
  EXPECT_CALL(mock, OH_Sensor_DestroyInfos(testInfos, sensorCount))
      .WillOnce(Invoke([&](Sensor_Info** infos, uint32_t count) -> int32_t {
        destroyCalled = true;
        DestroyTestSensorInfos(infos, count);
        return 0;
      }));
  double result = sensorAdapter_->GetOhosSensorMinSupportedFrequency(sensorTypeId);
  EXPECT_DOUBLE_EQ(result, 0.0);
  EXPECT_TRUE(destroyCalled);
}

TEST_F(SensorAdapterImplTest, GetOhosSensorMinSupportedFrequency_006) {
  int32_t sensorTypeId = 2;
  Sensor_Type targetType = SENSOR_TYPE_ACCELEROMETER;
  std::vector<Sensor_Type> deviceSensors = {targetType};
  uint32_t sensorCount = 1;
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_Sensor_GetInfos(nullptr, NotNull()))
      .WillOnce(
          Invoke([&](Sensor_Info** infos, uint32_t* count) -> Sensor_Result {
            *count = sensorCount;
            return static_cast<Sensor_Result>(0);
          }));
  Sensor_Info** testInfos = CreateTestSensorInfos(deviceSensors);
  EXPECT_CALL(mock, OH_Sensor_CreateInfos(sensorCount)).WillOnce(Return(testInfos));
  EXPECT_CALL(mock, OH_Sensor_GetInfos(testInfos, NotNull()))
      .WillOnce(
          Invoke([&](Sensor_Info** infos, uint32_t* count) -> Sensor_Result {
            *count = sensorCount;
            return static_cast<Sensor_Result>(0);
          }));
  EXPECT_CALL(mock, OH_SensorInfo_GetType(NotNull(), NotNull()))
      .WillOnce(
          Invoke([targetType](Sensor_Info* info, Sensor_Type* type) -> int32_t {
            *type = targetType;
            return 0;
          }));
  EXPECT_CALL(mock, OH_SensorInfo_GetMaxSamplingInterval(NotNull(), NotNull()))
      .WillOnce(Return(-1));
  bool destroyCalled = false;
  EXPECT_CALL(mock, OH_Sensor_DestroyInfos(testInfos, sensorCount))
      .WillOnce(Invoke([&](Sensor_Info** infos, uint32_t count) -> int32_t {
        destroyCalled = true;
        DestroyTestSensorInfos(infos, count);
        return 0;
      }));
  double result = sensorAdapter_->GetOhosSensorMinSupportedFrequency(sensorTypeId);
  EXPECT_DOUBLE_EQ(result, 0.0);
  EXPECT_TRUE(destroyCalled);
}

TEST_F(SensorAdapterImplTest, GetOhosSensorMinSupportedFrequency_007) {
  int32_t sensorTypeId = 2;
  Sensor_Type targetType = SENSOR_TYPE_ACCELEROMETER;
  std::vector<Sensor_Type> deviceSensors = {targetType};
  uint32_t sensorCount = 1;
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_Sensor_GetInfos(nullptr, NotNull()))
      .WillOnce(
          Invoke([&](Sensor_Info** infos, uint32_t* count) -> Sensor_Result {
            *count = sensorCount;
            return static_cast<Sensor_Result>(0);
          }));
  Sensor_Info** testInfos = CreateTestSensorInfos(deviceSensors);
  EXPECT_CALL(mock, OH_Sensor_CreateInfos(sensorCount))
      .WillOnce(Return(testInfos));
  EXPECT_CALL(mock, OH_Sensor_GetInfos(testInfos, NotNull()))
      .WillOnce(
          Invoke([&](Sensor_Info** infos, uint32_t* count) -> Sensor_Result {
            *count = sensorCount;
            return static_cast<Sensor_Result>(0);
          }));
  EXPECT_CALL(mock, OH_SensorInfo_GetType(NotNull(), NotNull()))
      .WillOnce(
          Invoke([targetType](Sensor_Info* info, Sensor_Type* type) -> int32_t {
            *type = targetType;
            return 0;
          }));
  EXPECT_CALL(mock, OH_SensorInfo_GetMaxSamplingInterval(NotNull(), NotNull()))
      .WillOnce(Invoke([](Sensor_Info* info, int64_t* maxInterval) -> int32_t {
        *maxInterval = 0;
        return 0;
      }));
  bool destroyCalled = false;
  EXPECT_CALL(mock, OH_Sensor_DestroyInfos(testInfos, sensorCount))
      .WillOnce(Invoke([&](Sensor_Info** infos, uint32_t count) -> int32_t {
        destroyCalled = true;
        DestroyTestSensorInfos(infos, count);
        return 0;
      }));
  double result =
      sensorAdapter_->GetOhosSensorMinSupportedFrequency(sensorTypeId);
  EXPECT_DOUBLE_EQ(result, 0.0);
  EXPECT_TRUE(destroyCalled);
}

TEST_F(SensorAdapterImplTest, GetOhosSensorMinSupportedFrequency_008) {
  int32_t sensorTypeId = 2;
  Sensor_Type targetType = SENSOR_TYPE_ACCELEROMETER;
  std::vector<Sensor_Type> deviceSensors = {targetType};
  uint32_t sensorCount = 1;
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_Sensor_GetInfos(nullptr, NotNull()))
      .WillOnce(
          Invoke([&](Sensor_Info** infos, uint32_t* count) -> Sensor_Result {
            *count = sensorCount;
            return static_cast<Sensor_Result>(0);
          }));
  Sensor_Info** testInfos = CreateTestSensorInfos(deviceSensors);
  EXPECT_CALL(mock, OH_Sensor_CreateInfos(sensorCount))
      .WillOnce(Return(testInfos));
  EXPECT_CALL(mock, OH_Sensor_GetInfos(testInfos, NotNull()))
      .WillOnce(
          Invoke([&](Sensor_Info** infos, uint32_t* count) -> Sensor_Result {
            *count = sensorCount;
            return static_cast<Sensor_Result>(0);
          }));
  EXPECT_CALL(mock, OH_SensorInfo_GetType(NotNull(), NotNull()))
      .WillOnce(
          Invoke([targetType](Sensor_Info* info, Sensor_Type* type) -> int32_t {
            *type = targetType;
            return 0;
          }));
  const double expectedMinFreq =
      NANOSECONDS_IN_SECOND / static_cast<double>(1000000000);
  EXPECT_CALL(mock, OH_SensorInfo_GetMaxSamplingInterval(NotNull(), NotNull()))
      .WillOnce(Invoke([](Sensor_Info* info, int64_t* maxInterval) -> int32_t {
        *maxInterval = 1000000000;
        return 0;
      }));
  bool destroyCalled = false;
  EXPECT_CALL(mock, OH_Sensor_DestroyInfos(testInfos, sensorCount))
      .WillOnce(Invoke([&](Sensor_Info** infos, uint32_t count) -> int32_t {
        destroyCalled = true;
        DestroyTestSensorInfos(infos, count);
        return 0;
      }));
  double result =
      sensorAdapter_->GetOhosSensorMinSupportedFrequency(sensorTypeId);
  EXPECT_DOUBLE_EQ(result, expectedMinFreq);
  EXPECT_TRUE(destroyCalled);
}

TEST_F(SensorAdapterImplTest, GetOhosSensorMaxSupportedFrequency_001) {
  double result =
      sensorAdapter_->GetOhosSensorMaxSupportedFrequency(1);
  EXPECT_DOUBLE_EQ(result, 0.0);
}

TEST_F(SensorAdapterImplTest, GetOhosSensorMaxSupportedFrequency_002) {
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_Sensor_GetInfos(nullptr, NotNull()))
      .Times(2)
      .WillOnce(
          Invoke([&](Sensor_Info** infos, uint32_t* count) -> Sensor_Result {
            *count = 0;
            return static_cast<Sensor_Result>(-1);
          }));
  EXPECT_CALL(mock, OH_Sensor_CreateInfos(0)).WillOnce(Return(nullptr));
  EXPECT_CALL(mock, OH_Sensor_DestroyInfos(nullptr, 0)).WillOnce(Return(0));
  double result = sensorAdapter_->GetOhosSensorMaxSupportedFrequency(2);
  EXPECT_DOUBLE_EQ(result, 0.0);
}

TEST_F(SensorAdapterImplTest, GetOhosSensorMaxSupportedFrequency_003) {
  int32_t sensorTypeId = 2;
  uint32_t expectedCount = 2;
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_Sensor_GetInfos(nullptr, NotNull()))
      .WillOnce(
          Invoke([&](Sensor_Info** infos, uint32_t* count) -> Sensor_Result {
            *count = expectedCount;
            return static_cast<Sensor_Result>(0);
          }));
  EXPECT_CALL(mock, OH_Sensor_CreateInfos(expectedCount))
      .WillOnce(Invoke([](uint32_t count) -> Sensor_Info** {
        return new Sensor_Info*[count];
      }));
  EXPECT_CALL(mock, OH_Sensor_GetInfos(NotNull(), NotNull()))
      .WillOnce(Return(static_cast<Sensor_Result>(-1)));
  bool destroyCalled = false;
  EXPECT_CALL(mock, OH_Sensor_DestroyInfos(NotNull(), expectedCount))
      .WillOnce(Invoke([&](Sensor_Info** infos, uint32_t count) -> int32_t {
        destroyCalled = true;
        delete[] infos;
        return 0;
      }));
  double result = sensorAdapter_->GetOhosSensorMaxSupportedFrequency(sensorTypeId);
  EXPECT_DOUBLE_EQ(result, 0.0);
  EXPECT_TRUE(destroyCalled);
}

TEST_F(SensorAdapterImplTest, GetOhosSensorMaxSupportedFrequency_004) {
  int32_t sensorTypeId = 2;
  std::vector<Sensor_Type> deviceSensors = {SENSOR_TYPE_GRAVITY,SENSOR_TYPE_LINEAR_ACCELERATION};
  uint32_t sensorCount = deviceSensors.size();
  auto& mock = MockOhSensorSupport::getInstance();

  EXPECT_CALL(mock, OH_Sensor_GetInfos(nullptr, NotNull()))
      .WillOnce(
          Invoke([&](Sensor_Info** infos, uint32_t* count) -> Sensor_Result {
            *count = sensorCount;
            return static_cast<Sensor_Result>(0);
          }));
  Sensor_Info** testInfos = CreateTestSensorInfos(deviceSensors);
  EXPECT_CALL(mock, OH_Sensor_CreateInfos(sensorCount))
      .WillOnce(Return(testInfos));
  EXPECT_CALL(mock, OH_Sensor_GetInfos(testInfos, NotNull()))
      .WillOnce(
          Invoke([&](Sensor_Info** infos, uint32_t* count) -> Sensor_Result {
            *count = sensorCount;
            return static_cast<Sensor_Result>(0);
          }));
  EXPECT_CALL(mock, OH_SensorInfo_GetType(NotNull(), NotNull()))
      .WillRepeatedly(Return(-1));
  bool destroyCalled = false;
  EXPECT_CALL(mock, OH_Sensor_DestroyInfos(testInfos, sensorCount))
      .WillOnce(Invoke([&](Sensor_Info** infos, uint32_t count) -> int32_t {
        destroyCalled = true;
        DestroyTestSensorInfos(infos, count);
        return 0;
      }));
  double result = sensorAdapter_->GetOhosSensorMaxSupportedFrequency(sensorTypeId);
  EXPECT_DOUBLE_EQ(result, 0.0);
  EXPECT_TRUE(destroyCalled);
}

TEST_F(SensorAdapterImplTest, GetOhosSensorMaxSupportedFrequency_005) {
  int32_t sensorTypeId = 2;
  std::vector<Sensor_Type> deviceSensors = {SENSOR_TYPE_GRAVITY,SENSOR_TYPE_LINEAR_ACCELERATION};
  uint32_t sensorCount = deviceSensors.size();
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_Sensor_GetInfos(nullptr, NotNull()))
      .WillOnce(
          Invoke([&](Sensor_Info** infos, uint32_t* count) -> Sensor_Result {
            *count = sensorCount;
            return static_cast<Sensor_Result>(0);
          }));
  Sensor_Info** testInfos = CreateTestSensorInfos(deviceSensors);
  EXPECT_CALL(mock, OH_Sensor_CreateInfos(sensorCount))
      .WillOnce(Return(testInfos));
  EXPECT_CALL(mock, OH_Sensor_GetInfos(testInfos, NotNull()))
      .WillOnce(
          Invoke([&](Sensor_Info** infos, uint32_t* count) -> Sensor_Result {
            *count = sensorCount;
            return static_cast<Sensor_Result>(0);
          }));
  EXPECT_CALL(mock, OH_SensorInfo_GetType(NotNull(), NotNull()))
      .Times(sensorCount)
      .WillRepeatedly(
          Invoke([](Sensor_Info* info, Sensor_Type* type) -> int32_t {
            *type = static_cast<Sensor_Type>(*reinterpret_cast<int*>(info));
            return 0;
          }));
  bool destroyCalled = false;
  EXPECT_CALL(mock, OH_Sensor_DestroyInfos(testInfos, sensorCount))
      .WillOnce(Invoke([&](Sensor_Info** infos, uint32_t count) -> int32_t {
        destroyCalled = true;
        DestroyTestSensorInfos(infos, count);
        return 0;
      }));
  double result =
      sensorAdapter_->GetOhosSensorMaxSupportedFrequency(sensorTypeId);
  EXPECT_DOUBLE_EQ(result, 0.0);
  EXPECT_TRUE(destroyCalled);
}

TEST_F(SensorAdapterImplTest, GetOhosSensorMaxSupportedFrequency_006) {
  int32_t sensorTypeId = 2;
  Sensor_Type targetType = SENSOR_TYPE_ACCELEROMETER;
  std::vector<Sensor_Type> deviceSensors = {targetType};
  uint32_t sensorCount = 1;
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_Sensor_GetInfos(nullptr, NotNull()))
      .WillOnce(
          Invoke([&](Sensor_Info** infos, uint32_t* count) -> Sensor_Result {
            *count = sensorCount;
            return static_cast<Sensor_Result>(0);
          }));
  Sensor_Info** testInfos = CreateTestSensorInfos(deviceSensors);
  EXPECT_CALL(mock, OH_Sensor_CreateInfos(sensorCount))
      .WillOnce(Return(testInfos));
  EXPECT_CALL(mock, OH_Sensor_GetInfos(testInfos, NotNull()))
      .WillOnce(
          Invoke([&](Sensor_Info** infos, uint32_t* count) -> Sensor_Result {
            *count = sensorCount;
            return static_cast<Sensor_Result>(0);
          }));
  EXPECT_CALL(mock, OH_SensorInfo_GetType(NotNull(), NotNull()))
      .WillOnce(
          Invoke([targetType](Sensor_Info* info, Sensor_Type* type) -> int32_t {
            *type = targetType;
            return 0;
          }));
  EXPECT_CALL(mock, OH_SensorInfo_GetMinSamplingInterval(NotNull(), NotNull()))
      .WillOnce(Return(-1));
  bool destroyCalled = false;
  EXPECT_CALL(mock, OH_Sensor_DestroyInfos(testInfos, sensorCount))
      .WillOnce(Invoke([&](Sensor_Info** infos, uint32_t count) -> int32_t {
        destroyCalled = true;
        DestroyTestSensorInfos(infos, count);
        return 0;
      }));
  double result =
      sensorAdapter_->GetOhosSensorMaxSupportedFrequency(sensorTypeId);
  EXPECT_DOUBLE_EQ(result, 0.0);
  EXPECT_TRUE(destroyCalled);
}

TEST_F(SensorAdapterImplTest, GetOhosSensorMaxSupportedFrequency_007) {
  int32_t sensorTypeId = 2;
  Sensor_Type targetType = SENSOR_TYPE_ACCELEROMETER;
  std::vector<Sensor_Type> deviceSensors = {targetType};
  uint32_t sensorCount = 1;
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_Sensor_GetInfos(nullptr, NotNull()))
      .WillOnce(
          Invoke([&](Sensor_Info** infos, uint32_t* count) -> Sensor_Result {
            *count = sensorCount;
            return static_cast<Sensor_Result>(0);
          }));
  Sensor_Info** testInfos = CreateTestSensorInfos(deviceSensors);
  EXPECT_CALL(mock, OH_Sensor_CreateInfos(sensorCount))
      .WillOnce(Return(testInfos));
  EXPECT_CALL(mock, OH_Sensor_GetInfos(testInfos, NotNull()))
      .WillOnce(
          Invoke([&](Sensor_Info** infos, uint32_t* count) -> Sensor_Result {
            *count = sensorCount;
            return static_cast<Sensor_Result>(0);
          }));
  EXPECT_CALL(mock, OH_SensorInfo_GetType(NotNull(), NotNull()))
      .WillOnce(
          Invoke([targetType](Sensor_Info* info, Sensor_Type* type) -> int32_t {
            *type = targetType;
            return 0;
          }));
  EXPECT_CALL(mock, OH_SensorInfo_GetMinSamplingInterval(NotNull(), NotNull()))
      .WillOnce(Invoke([](Sensor_Info* info, int64_t* minInterval) -> int32_t {
        *minInterval = -1000000;
        return 0;
      }));
  bool destroyCalled = false;
  EXPECT_CALL(mock, OH_Sensor_DestroyInfos(testInfos, sensorCount))
      .WillOnce(Invoke([&](Sensor_Info** infos, uint32_t count) -> int32_t {
        destroyCalled = true;
        DestroyTestSensorInfos(infos, count);
        return 0;
      }));
  double result =
      sensorAdapter_->GetOhosSensorMaxSupportedFrequency(sensorTypeId);
  EXPECT_DOUBLE_EQ(result, 0.0);
  EXPECT_TRUE(destroyCalled);
}

TEST_F(SensorAdapterImplTest, GetOhosSensorMaxSupportedFrequency_008) {
  int32_t sensorTypeId = 2;
  Sensor_Type targetType = SENSOR_TYPE_ACCELEROMETER;
  std::vector<Sensor_Type> deviceSensors = {targetType};
  uint32_t sensorCount = 1;
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_Sensor_GetInfos(nullptr, NotNull()))
      .WillOnce(
          Invoke([&](Sensor_Info** infos, uint32_t* count) -> Sensor_Result {
            *count = sensorCount;
            return static_cast<Sensor_Result>(0);
          }));
  Sensor_Info** testInfos = CreateTestSensorInfos(deviceSensors);
  EXPECT_CALL(mock, OH_Sensor_CreateInfos(sensorCount))
      .WillOnce(Return(testInfos));
  EXPECT_CALL(mock, OH_Sensor_GetInfos(testInfos, NotNull()))
      .WillOnce(
          Invoke([&](Sensor_Info** infos, uint32_t* count) -> Sensor_Result {
            *count = sensorCount;
            return static_cast<Sensor_Result>(0);
          }));
  EXPECT_CALL(mock, OH_SensorInfo_GetType(NotNull(), NotNull()))
      .WillOnce(
          Invoke([targetType](Sensor_Info* info, Sensor_Type* type) -> int32_t {
            *type = targetType;
            return 0;
          }));
  const double expectedMaxFreq =
      NANOSECONDS_IN_SECOND / static_cast<double>(100000000);
  EXPECT_CALL(mock, OH_SensorInfo_GetMinSamplingInterval(NotNull(), NotNull()))
      .WillOnce(Invoke([](Sensor_Info* info, int64_t* minInterval) -> int32_t {
        *minInterval = 100000000;
        return 0;
      }));
  bool destroyCalled = false;
  EXPECT_CALL(mock, OH_Sensor_DestroyInfos(testInfos, sensorCount))
      .WillOnce(Invoke([&](Sensor_Info** infos, uint32_t count) -> int32_t {
        destroyCalled = true;
        DestroyTestSensorInfos(infos, count);
        return 0;
      }));
  double result =
      sensorAdapter_->GetOhosSensorMaxSupportedFrequency(sensorTypeId);
  EXPECT_DOUBLE_EQ(result, expectedMaxFreq);
  EXPECT_TRUE(destroyCalled);
}

TEST_F(SensorAdapterImplTest, OhosSensorCallback_001) {
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_SensorEvent_GetType(NotNull(), NotNull()))
      .WillOnce(Invoke([](Sensor_Event* event, Sensor_Type* type) -> int32_t {
        *type = SENSOR_TYPE_ACCELEROMETER;
        return 0;
      }));
  EXPECT_CALL(mock, OH_SensorEvent_GetData(NotNull(), NotNull(), NotNull()))
      .WillOnce(Invoke(
          [](Sensor_Event* event, float** data, uint32_t* length) -> int32_t {
            static float testData[3] = {1.0f, 2.0f, 3.0f};
            *data = testData;
            *length = 3;
            return 0;
          }));
  EXPECT_CALL(mock, OH_SensorEvent_GetTimestamp(NotNull(), NotNull()))
      .WillOnce(Invoke([](Sensor_Event* event, int64_t* timestamp) -> int32_t {
        *timestamp = 123456789;
        return 0;
      }));
  auto mockAdapter = std::make_shared<MockSensorCallbackAdapter>();
  EXPECT_CALL(*mockAdapter,
      UpdateOhosSensorData(123456789.0, 1.0f, 2.0f, 3.0f, 0.0f))
      .Times(1);
  auto testCallback =
      std::make_shared<OHOS::NWeb::SensorCallbackImpl>(mockAdapter);
  {
    std::lock_guard<std::mutex> lock(sensorAdapter_->sensorCallbackMapMutex_);
    sensorAdapter_->sensorCallbackMap[SENSOR_TYPE_ACCELEROMETER] = testCallback;
  }
  Sensor_Event testEvent{};
  sensorAdapter_->OhosSensorCallback(&testEvent);
  bool isVerified =
      testing::Mock::VerifyAndClearExpectations(mockAdapter.get());
  EXPECT_TRUE(isVerified);
}

TEST_F(SensorAdapterImplTest, OhosSensorCallback_002) {
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_SensorEvent_GetType(NotNull(), NotNull()))
      .WillOnce(Return(-1));
  auto mockAdapter = std::make_shared<MockSensorCallbackAdapter>();
  EXPECT_CALL(*mockAdapter, UpdateOhosSensorData).Times(0);
  auto testCallback =
      std::make_shared<OHOS::NWeb::SensorCallbackImpl>(mockAdapter);
  {
    std::lock_guard<std::mutex> lock(sensorAdapter_->sensorCallbackMapMutex_);
    sensorAdapter_->sensorCallbackMap[SENSOR_TYPE_ACCELEROMETER] = testCallback;
  }
  Sensor_Event testEvent{};
  sensorAdapter_->OhosSensorCallback(&testEvent);
  bool isVerified =
      testing::Mock::VerifyAndClearExpectations(mockAdapter.get());
  EXPECT_TRUE(isVerified);
}

TEST_F(SensorAdapterImplTest, OhosSensorCallback_003) {
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_SensorEvent_GetType(NotNull(), NotNull()))
      .WillOnce(Invoke([](Sensor_Event* event, Sensor_Type* type) -> int32_t {
        *type = SENSOR_TYPE_ACCELEROMETER;
        return 0;
      }));
  {
    std::lock_guard<std::mutex> lock(sensorAdapter_->sensorCallbackMapMutex_);
    sensorAdapter_->sensorCallbackMap.erase(SENSOR_TYPE_ACCELEROMETER);
  }
  Sensor_Event testEvent{};
  sensorAdapter_->OhosSensorCallback(&testEvent);
}

TEST_F(SensorAdapterImplTest, OhosSensorCallback_004) {
  auto mockAdapter = std::make_shared<MockSensorCallbackAdapter>();
  EXPECT_CALL(*mockAdapter, UpdateOhosSensorData).Times(0);
  auto testCallback =
      std::make_shared<OHOS::NWeb::SensorCallbackImpl>(mockAdapter);
  {
    std::lock_guard<std::mutex> lock(sensorAdapter_->sensorCallbackMapMutex_);
    sensorAdapter_->sensorCallbackMap[SENSOR_TYPE_ACCELEROMETER] = testCallback;
  }
  sensorAdapter_->OhosSensorCallback(nullptr);
  testing::Mock::VerifyAndClearExpectations(mockAdapter.get());
}

TEST_F(SensorAdapterImplTest, OhosSensorCallback_005) {
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_SensorEvent_GetType(NotNull(), NotNull()))
      .WillOnce(Invoke([](Sensor_Event* event, Sensor_Type* type) -> int32_t {
        *type = SENSOR_TYPE_ACCELEROMETER;
        return 0;
      }));
  EXPECT_CALL(mock, OH_SensorEvent_GetData(NotNull(), NotNull(), NotNull()))
      .WillOnce(Return(-1));
  auto mockAdapter = std::make_shared<MockSensorCallbackAdapter>();
  EXPECT_CALL(*mockAdapter, UpdateOhosSensorData).Times(0);
  auto testCallback =
      std::make_shared<OHOS::NWeb::SensorCallbackImpl>(mockAdapter);
  {
    std::lock_guard<std::mutex> lock(sensorAdapter_->sensorCallbackMapMutex_);
    sensorAdapter_->sensorCallbackMap[SENSOR_TYPE_ACCELEROMETER] = testCallback;
  }
  Sensor_Event testEvent{};
  sensorAdapter_->OhosSensorCallback(&testEvent);
  bool isVerified =
      testing::Mock::VerifyAndClearExpectations(mockAdapter.get());
  EXPECT_TRUE(isVerified);
}

TEST_F(SensorAdapterImplTest, OhosSensorCallback_006) {
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_SensorEvent_GetType(NotNull(), NotNull()))
      .WillOnce(Invoke([](Sensor_Event* event, Sensor_Type* type) -> int32_t {
        *type = SENSOR_TYPE_ACCELEROMETER;
        return 0;
      }));
  EXPECT_CALL(mock, OH_SensorEvent_GetData(NotNull(), NotNull(), NotNull()))
      .WillOnce(Invoke(
          [](Sensor_Event* event, float** data, uint32_t* length) -> int32_t {
            static float testData[3] = {1.0f, 2.0f, 3.0f};
            *data = testData;
            *length = 3;
            return 0;
          }));
  EXPECT_CALL(mock, OH_SensorEvent_GetTimestamp(NotNull(), NotNull()))
      .WillOnce(Return(-1));
  auto mockAdapter = std::make_shared<MockSensorCallbackAdapter>();
  EXPECT_CALL(*mockAdapter, UpdateOhosSensorData).Times(0);
  auto testCallback =
      std::make_shared<OHOS::NWeb::SensorCallbackImpl>(mockAdapter);
  {
    std::lock_guard<std::mutex> lock(sensorAdapter_->sensorCallbackMapMutex_);
    sensorAdapter_->sensorCallbackMap[SENSOR_TYPE_ACCELEROMETER] = testCallback;
  }
  Sensor_Event testEvent{};
  sensorAdapter_->OhosSensorCallback(&testEvent);
  bool isVerified =
      testing::Mock::VerifyAndClearExpectations(mockAdapter.get());
  EXPECT_TRUE(isVerified);
}

TEST_F(SensorAdapterImplTest, OhosSensorCallback_007) {
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_SensorEvent_GetType(NotNull(), NotNull()))
      .WillOnce(Invoke([](Sensor_Event* event, Sensor_Type* type) -> int32_t {
        *type = SENSOR_TYPE_GYROSCOPE;
        return 0;
      }));
  EXPECT_CALL(mock, OH_SensorEvent_GetData(NotNull(), NotNull(), NotNull()))
      .WillOnce(Invoke(
          [](Sensor_Event* event, float** data, uint32_t* length) -> int32_t {
            static float testData[3] = {4.0f, 5.0f, 6.0f};
            *data = testData;
            *length = 3;
            return 0;
          }));
  EXPECT_CALL(mock, OH_SensorEvent_GetTimestamp(NotNull(), NotNull()))
      .WillOnce(Invoke([](Sensor_Event* event, int64_t* timestamp) -> int32_t {
        *timestamp = 987654321;
        return 0;
      }));
  auto mockAdapter = std::make_shared<MockSensorCallbackAdapter>();
  EXPECT_CALL(*mockAdapter,
      UpdateOhosSensorData(987654321.0, 4.0f, 5.0f, 6.0f, 0.0f))
      .Times(1);
  auto testCallback =
      std::make_shared<OHOS::NWeb::SensorCallbackImpl>(mockAdapter);
  {
    std::lock_guard<std::mutex> lock(sensorAdapter_->sensorCallbackMapMutex_);
    sensorAdapter_->sensorCallbackMap[SENSOR_TYPE_GYROSCOPE] = testCallback;
  }
  Sensor_Event testEvent{};
  sensorAdapter_->OhosSensorCallback(&testEvent);
  bool isVerified =
      testing::Mock::VerifyAndClearExpectations(mockAdapter.get());
  EXPECT_TRUE(isVerified);
}

TEST_F(SensorAdapterImplTest, OhosSensorCallback_008) {
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_SensorEvent_GetType(NotNull(), NotNull()))
      .WillOnce(Invoke([](Sensor_Event* event, Sensor_Type* type) -> int32_t {
        *type = SENSOR_TYPE_ACCELEROMETER;
        return 0;
      }));
  EXPECT_CALL(mock, OH_SensorEvent_GetData(NotNull(), NotNull(), NotNull()))
      .WillOnce(Invoke(
          [](Sensor_Event* event, float** data, uint32_t* length) -> int32_t {
            static float testData[0] = {};
            *data = testData;
            *length = 0;
            return 0;
          }));
  EXPECT_CALL(mock, OH_SensorEvent_GetTimestamp(NotNull(), NotNull()))
      .WillOnce(Invoke([](Sensor_Event* event, int64_t* timestamp) -> int32_t {
        *timestamp = 111222333;
        return 0;
      }));
  auto mockAdapter = std::make_shared<MockSensorCallbackAdapter>();
  EXPECT_CALL(*mockAdapter, UpdateOhosSensorData).Times(0);
  auto testCallback =
      std::make_shared<OHOS::NWeb::SensorCallbackImpl>(mockAdapter);
  {
    std::lock_guard<std::mutex> lock(sensorAdapter_->sensorCallbackMapMutex_);
    sensorAdapter_->sensorCallbackMap[SENSOR_TYPE_ACCELEROMETER] = testCallback;
  }
  Sensor_Event testEvent{};
  sensorAdapter_->OhosSensorCallback(&testEvent);
  bool isVerified =
      testing::Mock::VerifyAndClearExpectations(mockAdapter.get());
  EXPECT_TRUE(isVerified);
}

TEST_F(SensorAdapterImplTest, OhosSensorCallback_009) {
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_SensorEvent_GetType(NotNull(), NotNull()))
      .WillOnce(Invoke([](Sensor_Event* event, Sensor_Type* type) -> int32_t {
        *type = SENSOR_TYPE_ACCELEROMETER;
        return 0;
      }));

  EXPECT_CALL(mock, OH_SensorEvent_GetData(NotNull(), NotNull(), NotNull()))
      .WillOnce(Invoke(
          [](Sensor_Event* event, float** data, uint32_t* length) -> int32_t {
            static float testData[10] = {
                1.0f, 2.0f, 3.0f, 4.0f, 5.0f,
                6.0f, 7.0f, 8.0f, 9.0f, 10.0f};
            *data = testData;
            *length = 10;
            return 0;
          }));
  EXPECT_CALL(mock, OH_SensorEvent_GetTimestamp(NotNull(), NotNull()))
      .WillOnce(Invoke([](Sensor_Event* event, int64_t* timestamp) -> int32_t {
        *timestamp = 444555666;
        return 0;
      }));
  auto mockAdapter = std::make_shared<MockSensorCallbackAdapter>();
  EXPECT_CALL(*mockAdapter, UpdateOhosSensorData).Times(0);
  auto testCallback =
      std::make_shared<OHOS::NWeb::SensorCallbackImpl>(mockAdapter);
  {
    std::lock_guard<std::mutex> lock(sensorAdapter_->sensorCallbackMapMutex_);
    sensorAdapter_->sensorCallbackMap[SENSOR_TYPE_ACCELEROMETER] = testCallback;
  }
  Sensor_Event testEvent{};
  sensorAdapter_->OhosSensorCallback(&testEvent);
  bool isVerified =
      testing::Mock::VerifyAndClearExpectations(mockAdapter.get());
  EXPECT_TRUE(isVerified);
}

TEST_F(SensorAdapterImplTest, SubscribeOhosSensor_001) {
  int32_t result1 = sensorAdapter_->SubscribeOhosSensor(2, 0);
  int32_t result2 = sensorAdapter_->SubscribeOhosSensor(2, -500);
  EXPECT_EQ(result1, OHOS::NWeb::SENSOR_PARAMETER_ERROR);
  EXPECT_EQ(result2, OHOS::NWeb::SENSOR_PARAMETER_ERROR);
}

TEST_F(SensorAdapterImplTest, SubscribeOhosSensor_002) {
  int32_t invalidTypeId = 999;
  int32_t result = sensorAdapter_->SubscribeOhosSensor(invalidTypeId, 1000000);
  EXPECT_EQ(result, OHOS::NWeb::SENSOR_PARAMETER_ERROR);
}

TEST_F(SensorAdapterImplTest, SubscribeOhosSensor_003) {
  const int32_t mockErrCode = -1;
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_Sensor_CreateSubscriber())
      .WillOnce(Invoke([]() -> Sensor_Subscriber* {
        return reinterpret_cast<Sensor_Subscriber*>(
            new char[SENSOR_SUBSCRIBER_SIZE]);
      }));
  EXPECT_CALL(mock, OH_SensorSubscriber_SetCallback(_, _))
      .WillOnce(Return(mockErrCode));
  int32_t result = sensorAdapter_->SubscribeOhosSensor(2, 1000000);
  EXPECT_EQ(result, mockErrCode);
}

TEST_F(SensorAdapterImplTest, SubscribeOhosSensor_004) {
  const int32_t mockErrCode = -2;
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_Sensor_CreateSubscriber())
      .WillOnce(Invoke([]() -> Sensor_Subscriber* {
        return reinterpret_cast<Sensor_Subscriber*>(
            new char[SENSOR_SUBSCRIBER_SIZE]);
      }));
  EXPECT_CALL(mock, OH_SensorSubscriber_SetCallback(_, _)).WillOnce(Return(0));
  EXPECT_CALL(mock, OH_Sensor_CreateSubscriptionId())
      .WillOnce(Invoke([]() -> Sensor_SubscriptionId* {
        return reinterpret_cast<Sensor_SubscriptionId*>(
            new char[SENSOR_SUBSCRIPTION_ID_SIZE]);
      }));
  EXPECT_CALL(mock, OH_SensorSubscriptionId_SetType(_, _))
      .WillOnce(Return(mockErrCode));
  int32_t result = sensorAdapter_->SubscribeOhosSensor(2, 1000000);
  EXPECT_EQ(result, mockErrCode);
}

TEST_F(SensorAdapterImplTest, SubscribeOhosSensor_005) {
  const int32_t mockErrCode = -3;
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_Sensor_CreateSubscriber())
      .WillOnce(Invoke([]() -> Sensor_Subscriber* {
        return reinterpret_cast<Sensor_Subscriber*>(
            new char[SENSOR_SUBSCRIBER_SIZE]);
      }));
  EXPECT_CALL(mock, OH_SensorSubscriber_SetCallback(_, _)).WillOnce(Return(0));
  EXPECT_CALL(mock, OH_Sensor_CreateSubscriptionId())
      .WillOnce(Invoke([]() -> Sensor_SubscriptionId* {
        return reinterpret_cast<Sensor_SubscriptionId*>(
            new char[SENSOR_SUBSCRIPTION_ID_SIZE]);
      }));
  EXPECT_CALL(mock, OH_SensorSubscriptionId_SetType(_, _)).WillOnce(Return(0));
  EXPECT_CALL(mock, OH_Sensor_CreateSubscriptionAttribute())
      .WillOnce(Invoke([]() -> Sensor_SubscriptionAttribute* {
        return reinterpret_cast<Sensor_SubscriptionAttribute*>(
            new char[SENSOR_SUBSCRIPTION_ATTRIBUTE_SIZE]);
      }));
  EXPECT_CALL(mock, OH_SensorSubscriptionAttribute_SetSamplingInterval(_, _))
      .WillOnce(Return(mockErrCode));
  int32_t result = sensorAdapter_->SubscribeOhosSensor(2, 1000000);
  EXPECT_EQ(result, mockErrCode);
}

TEST_F(SensorAdapterImplTest, SubscribeOhosSensor_006) {
  const Sensor_Result mockErrCode = static_cast<Sensor_Result>(-4);
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_Sensor_CreateSubscriber())
      .WillOnce(Invoke([]() -> Sensor_Subscriber* {
        return reinterpret_cast<Sensor_Subscriber*>(
            new char[SENSOR_SUBSCRIBER_SIZE]);
      }));
  EXPECT_CALL(mock, OH_SensorSubscriber_SetCallback(_, _)).WillOnce(Return(0));
  EXPECT_CALL(mock, OH_Sensor_CreateSubscriptionId())
      .WillOnce(Invoke([]() -> Sensor_SubscriptionId* {
        return reinterpret_cast<Sensor_SubscriptionId*>(
            new char[SENSOR_SUBSCRIPTION_ID_SIZE]);
      }));
  EXPECT_CALL(mock, OH_SensorSubscriptionId_SetType(_, _)).WillOnce(Return(0));
  EXPECT_CALL(mock, OH_Sensor_CreateSubscriptionAttribute())
      .WillOnce(Invoke([]() -> Sensor_SubscriptionAttribute* {
        return reinterpret_cast<Sensor_SubscriptionAttribute*>(
            new char[SENSOR_SUBSCRIPTION_ATTRIBUTE_SIZE]);
      }));
  EXPECT_CALL(mock, OH_SensorSubscriptionAttribute_SetSamplingInterval(_, _))
      .WillOnce(Return(0));
  EXPECT_CALL(mock, OH_Sensor_Subscribe(_, _, _)).WillOnce(Return(mockErrCode));
  int32_t result = sensorAdapter_->SubscribeOhosSensor(2, 1000000);
  EXPECT_EQ(result, static_cast<int32_t>(mockErrCode));
}

TEST_F(SensorAdapterImplTest, SubscribeOhosSensor_007) {
  const int32_t sensorTypeId = 2;
  const int64_t samplingInterval = 1000000;
  Sensor_Type ohosType = SENSOR_TYPE_ACCELEROMETER;
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_Sensor_CreateSubscriber())
      .WillOnce(Invoke([]() -> Sensor_Subscriber* {
        return reinterpret_cast<Sensor_Subscriber*>(
            new char[SENSOR_SUBSCRIBER_SIZE]);
      }));
  EXPECT_CALL(mock, OH_SensorSubscriber_SetCallback(_, _)).WillOnce(Return(0));
  EXPECT_CALL(mock, OH_Sensor_CreateSubscriptionId())
      .WillOnce(Invoke([]() -> Sensor_SubscriptionId* {
        return reinterpret_cast<Sensor_SubscriptionId*>(
            new char[SENSOR_SUBSCRIPTION_ID_SIZE]);
      }));
  EXPECT_CALL(mock, OH_SensorSubscriptionId_SetType(_, _)).WillOnce(Return(0));
  EXPECT_CALL(mock, OH_Sensor_CreateSubscriptionAttribute())
      .WillOnce(Invoke([]() -> Sensor_SubscriptionAttribute* {
        return reinterpret_cast<Sensor_SubscriptionAttribute*>(
            new char[SENSOR_SUBSCRIPTION_ATTRIBUTE_SIZE]);
      }));
  EXPECT_CALL(mock, OH_SensorSubscriptionAttribute_SetSamplingInterval(_, _))
      .WillOnce(Return(0));
  EXPECT_CALL(mock, OH_Sensor_Subscribe(_, _, _))
      .WillOnce(Return(static_cast<Sensor_Result>(0)));
  int32_t result =
      sensorAdapter_->SubscribeOhosSensor(sensorTypeId, samplingInterval);
  EXPECT_EQ(result, OHOS::NWeb::SENSOR_SUCCESS);
  std::lock_guard<std::mutex> lock(sensorAdapter_->sensorSubscriberMapMutex_);
  auto iter = sensorSubscriberMap.find(ohosType);
  ASSERT_NE(iter, sensorSubscriberMap.end());
  EXPECT_NE(iter->second->subscriber, nullptr);
  EXPECT_NE(iter->second->id, nullptr);
  EXPECT_NE(iter->second->attr, nullptr);
}

TEST_F(SensorAdapterImplTest, UnsubscribeOhosSensor_InvalidSensorTypeId) {
  int32_t invalidTypeId = 999;
  int32_t result = sensorAdapter_->UnsubscribeOhosSensor(invalidTypeId);
  EXPECT_EQ(result, OHOS::NWeb::SENSOR_PARAMETER_ERROR);
}

TEST_F(SensorAdapterImplTest, UnsubscribeOhosSensor_NoSubscription) {
  int32_t sensorTypeId = 2;
  {
    std::lock_guard<std::mutex> lock(sensorAdapter_->sensorSubscriberMapMutex_);
    EXPECT_TRUE(sensorSubscriberMap.empty());
  }
  int32_t result = sensorAdapter_->UnsubscribeOhosSensor(sensorTypeId);
  EXPECT_EQ(result, OHOS::NWeb::SENSOR_PARAMETER_ERROR);
}

TEST_F(SensorAdapterImplTest, UnsubscribeOhosSensor_Success) {
  const int32_t sensorTypeId = 2;
  Sensor_Type ohosType = SENSOR_TYPE_ACCELEROMETER;
  PrepareSubscribeSensor();
  int32_t subscribeResult =
      sensorAdapter_->SubscribeOhosSensor(sensorTypeId, 1000000);
  EXPECT_EQ(subscribeResult, OHOS::NWeb::SENSOR_SUCCESS);
  {
    std::lock_guard<std::mutex> lock(sensorAdapter_->sensorSubscriberMapMutex_);
    auto iter = sensorSubscriberMap.find(ohosType);
    ASSERT_NE(iter, sensorSubscriberMap.end());
    EXPECT_NE(iter->second->subscriber, nullptr);
    EXPECT_NE(iter->second->id, nullptr);
    EXPECT_NE(iter->second->attr, nullptr);
  }
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_Sensor_Unsubscribe(_, _))
      .WillOnce(Return(static_cast<Sensor_Result>(0)));
  EXPECT_CALL(mock, OH_Sensor_DestroySubscriber(_)).WillOnce(Return(0));
  EXPECT_CALL(mock, OH_Sensor_DestroySubscriptionId(_)).WillOnce(Return(0));
  EXPECT_CALL(mock, OH_Sensor_DestroySubscriptionAttribute(_))
      .WillOnce(Return(0));
  int32_t unsubscribeResult =
      sensorAdapter_->UnsubscribeOhosSensor(sensorTypeId);
  EXPECT_EQ(unsubscribeResult, OHOS::NWeb::SENSOR_SUCCESS);
  {
    std::lock_guard<std::mutex> lock(sensorAdapter_->sensorSubscriberMapMutex_);
    auto iter = sensorSubscriberMap.find(ohosType);
    EXPECT_EQ(iter, sensorSubscriberMap.end());
  }
  {
    std::lock_guard<std::mutex> lock(sensorAdapter_->sensorCallbackMapMutex_);
    auto iter = sensorAdapter_->sensorCallbackMap.find(ohosType);
    EXPECT_EQ(iter, sensorAdapter_->sensorCallbackMap.end());
  }
}

TEST_F(SensorAdapterImplTest, UnsubscribeOhosSensor_UnsubscribeFailed) {
  const int32_t sensorTypeId = 2;
  PrepareSubscribeSensor();
  int32_t subscribeResult =
      sensorAdapter_->SubscribeOhosSensor(sensorTypeId, 1000000);
  EXPECT_EQ(subscribeResult, OHOS::NWeb::SENSOR_SUCCESS);
  const Sensor_Result mockErrCode = static_cast<Sensor_Result>(-5);
  auto& mock = MockOhSensorSupport::getInstance();
  EXPECT_CALL(mock, OH_Sensor_Unsubscribe(_, _)).WillOnce(Return(mockErrCode));
  int32_t unsubscribeResult =
      sensorAdapter_->UnsubscribeOhosSensor(sensorTypeId);
  EXPECT_EQ(unsubscribeResult, static_cast<int32_t>(mockErrCode));
}

TEST_F(SensorAdapterImplTest, SensorTypeToSensorUserName_ValidSensorTypes) {
  std::map<int32_t, std::string> expectedMappings = {
      {2, "OhosAccelerometerService"},  {3, "OhosLinearAccelerometerService"},
      {4, "OhosGravityService"},        {5, "OhosCyroscopeService"},
      {6, "OhosMagnetometerService"},   {8, "OhosOrientationService"},
      {9, "OhosRotationVectorService"}, {11, "OhosGameRotationVectorService"}};
  for (const auto& mapping : expectedMappings) {
    std::string result = SensorTypeToSensorUserName(mapping.first);
    EXPECT_EQ(result, mapping.second);
  }
}

TEST_F(SensorAdapterImplTest, SensorTypeToSensorUserName_InvalidSensorTypeId) {
  std::vector<int32_t> invalidSensorTypeIds = {0, 1, 7, 10, 12, 999, -1};
  for (int32_t sensorTypeId : invalidSensorTypeIds) {
    std::string result = SensorTypeToSensorUserName(sensorTypeId);
    EXPECT_EQ(result, "OhosSensorService");
  }
}
