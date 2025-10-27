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
#include <gtest/gtest.h>

#define private public
#include "sensor_adapter.h"
#include "sensor_adapter_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::NWeb {
namespace {
std::shared_ptr<OHOS::NWeb::SensorCallbackImpl> g_sensoCallback;
std::shared_ptr<OHOS::NWeb::SensorAdapterImpl> g_sensorAdapter;
}
constexpr double NANOSECONDS_IN_SECOND = 1000000000.0;
constexpr double DEFAULT_SAMPLE_PERIOD = 200000000.0;

class SensorCallbackImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

class SensorAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

class SensorCallbackAdapterMock : public SensorCallbackAdapter {
public:
    ~SensorCallbackAdapterMock() = default;
    void UpdateOhosSensorData(double timestamp,
        double value1, double value2, double value3, double value4) {}
};

void SensorCallbackImplTest::SetUpTestCase(void) {}

void SensorCallbackImplTest::TearDownTestCase(void) {}

void SensorCallbackImplTest::SetUp(void)
{
    auto callbackAdapter = std::make_shared<SensorCallbackAdapterMock>();
    g_sensoCallback = std::make_shared<SensorCallbackImpl>(callbackAdapter);
    ASSERT_NE(g_sensoCallback, nullptr);
}

void SensorCallbackImplTest::TearDown(void)
{
    g_sensoCallback = nullptr;
}

void SensorAdapterImplTest::SetUpTestCase(void) {}

void SensorAdapterImplTest::TearDownTestCase(void) {}

void SensorAdapterImplTest::SetUp(void)
{
    g_sensorAdapter = std::make_shared<SensorAdapterImpl>();
    ASSERT_NE(g_sensorAdapter, nullptr);
}

void SensorAdapterImplTest::TearDown(void)
{
    g_sensorAdapter = nullptr;
}

/**
 * @tc.name: SensorAdapterImplTest_SensorCallbackImpl_001.
 * @tc.desc: test of SensorCallbackImpl :: UpdateOhosSensorData()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(SensorCallbackImplTest, SensorAdapterImplTest_SensorCallbackImplTest_001, TestSize.Level1)
{
    double temp = 1.1;
    g_sensoCallback->callbackAdapter_ = nullptr;
    g_sensoCallback->UpdateOhosSensorData(temp, temp, temp, temp, temp);
    EXPECT_EQ(g_sensoCallback->callbackAdapter_, nullptr);

    auto callbackAdapter = std::make_shared<SensorCallbackAdapterMock>();
    g_sensoCallback->callbackAdapter_ = callbackAdapter;
    g_sensoCallback->UpdateOhosSensorData(temp, temp, temp, temp, temp);
    EXPECT_NE(g_sensoCallback->callbackAdapter_, nullptr);
}

/**
 * @tc.name: SensorAdapterImplTest_SensorAdapterImpl_001.
 * @tc.desc: test of SensorAdapterImpl :: IsOhosSensorSupported()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(SensorAdapterImplTest, SensorAdapterImplTest_SensorAdapterImpl_001, TestSize.Level1)
{
    int32_t temp = 1;
    auto number = g_sensorAdapter->IsOhosSensorSupported(temp);
    EXPECT_EQ(number, SENSOR_ERROR);

    temp = 9;
    g_sensorAdapter->IsOhosSensorSupported(temp);
}

/**
 * @tc.name: SensorAdapterImplTest_SensorAdapterImpl_002.
 * @tc.desc: test of SensorAdapterImpl :: GetOhosSensorReportingMode()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(SensorAdapterImplTest, SensorAdapterImplTest_SensorAdapterImpl_002, TestSize.Level1)
{
    int32_t temp = 11;
    int32_t temp1 = SENSOR_DATA_REPORT_CONTINUOUS;
    auto mode = g_sensorAdapter->GetOhosSensorReportingMode(temp);
    EXPECT_EQ(mode, temp1);

    temp = 1;
    temp1 = -1;
    mode = g_sensorAdapter->GetOhosSensorReportingMode(temp);
    EXPECT_EQ(mode, temp1);
}

/**
 * @tc.name: SensorAdapterImplTest_SensorAdapterImpl_003.
 * @tc.desc: test of SensorAdapterImpl :: GetOhosSensorDefaultSupportedFrequency()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(SensorAdapterImplTest, SensorAdapterImplTest_SensorAdapterImpl_003, TestSize.Level1)
{
    int32_t temp = 1;
    double temp1 = 0.0;
    auto mode = g_sensorAdapter->GetOhosSensorDefaultSupportedFrequency(temp);
    EXPECT_EQ(mode, temp1);

    temp = 2;
    temp1 = NANOSECONDS_IN_SECOND / DEFAULT_SAMPLE_PERIOD;
    mode = g_sensorAdapter->GetOhosSensorDefaultSupportedFrequency(temp);
    EXPECT_EQ(mode, temp1);
}

/**
 * @tc.name: SensorAdapterImplTest_SensorAdapterImpl_004.
 * @tc.desc: test of SensorAdapterImpl :: GetOhosSensorMinSupportedFrequency()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(SensorAdapterImplTest, SensorAdapterImplTest_SensorAdapterImpl_004, TestSize.Level1)
{
    int32_t temp = 1;
    double temp1 = 0.0;
    auto mode = g_sensorAdapter->GetOhosSensorMinSupportedFrequency(temp);
    EXPECT_EQ(mode, temp1);

    temp = 2;
    mode = g_sensorAdapter->GetOhosSensorMinSupportedFrequency(temp);
    EXPECT_NE(mode, temp1);    
}

/**
 * @tc.name: SensorAdapterImplTest_SensorAdapterImpl_005.
 * @tc.desc: test of SensorAdapterImpl :: GetOhosSensorMaxSupportedFrequency()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(SensorAdapterImplTest, SensorAdapterImplTest_SensorAdapterImpl_005, TestSize.Level1)
{
    int32_t temp = 1;
    double temp1 = 0.0;
    auto mode = g_sensorAdapter->GetOhosSensorMaxSupportedFrequency(temp);
    EXPECT_EQ(mode, temp1);

    temp = 2;
    mode = g_sensorAdapter->GetOhosSensorMaxSupportedFrequency(temp);
    EXPECT_NE(mode, temp1);    
}

/**
 * @tc.name: SensorAdapterImplTest_SensorAdapterImpl_006.
 * @tc.desc: test of SensorAdapterImpl :: SubscribeOhosSensor()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(SensorAdapterImplTest, SensorAdapterImplTest_SensorAdapterImpl_006, TestSize.Level1)
{
    int32_t id = 1;
    int64_t temp = 0;
    auto status = g_sensorAdapter->SubscribeOhosSensor(id, temp);
    EXPECT_EQ(status, SENSOR_PARAMETER_ERROR);

    temp = 1;
    status = g_sensorAdapter->SubscribeOhosSensor(id, temp);
    EXPECT_EQ(status, SENSOR_PARAMETER_ERROR);

    id = 2;
    status = g_sensorAdapter->SubscribeOhosSensor(id, temp);
    EXPECT_EQ(status, SENSOR_SUCCESS);
}

/**
 * @tc.name: SensorAdapterImplTest_SensorAdapterImpl_007.
 * @tc.desc: test of SensorAdapterImpl :: RegistOhosSensorCallback()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(SensorAdapterImplTest, SensorAdapterImplTest_SensorAdapterImpl_007, TestSize.Level1)
{
    int32_t id = 1;
    auto callbackAdapter = std::make_shared<SensorCallbackAdapterMock>();
    auto status = g_sensorAdapter->RegistOhosSensorCallback(id, callbackAdapter);
    EXPECT_EQ(status, SENSOR_PARAMETER_ERROR);

    id = 2;
    status = g_sensorAdapter->RegistOhosSensorCallback(id, callbackAdapter);
    EXPECT_EQ(status, SENSOR_SUCCESS);
}

/**
 * @tc.name: SensorAdapterImplTest_SensorAdapterImpl_008.
 * @tc.desc: test of SensorAdapterImpl :: UnsubscribeOhosSensor()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(SensorAdapterImplTest, SensorAdapterImplTest_SensorAdapterImpl_008, TestSize.Level1)
{
    int32_t id = 1;
    auto status = g_sensorAdapter->UnsubscribeOhosSensor(id);
    EXPECT_EQ(status, SENSOR_PARAMETER_ERROR);

    id = 2;
    status = g_sensorAdapter->UnsubscribeOhosSensor(id);
    EXPECT_NE(status, SENSOR_SUCCESS);
}


/**
 * @tc.name: SensorAdapterImplTest_SensorAdapterImpl_009.
 * @tc.desc: test of SensorAdapterImpl :: OhosSensorCallback()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(SensorAdapterImplTest, SensorAdapterImplTest_SensorAdapterImpl_009, TestSize.Level1)
{
    std::vector<std::shared_ptr<OHOS::NWeb::SensorCallbackImpl>> callback(9);
    for(int32_t i=0; i<9; i++) {
        auto callbackAdapter = std::make_shared<SensorCallbackAdapterMock>();
        callback[i] = std::make_shared<OHOS::NWeb::SensorCallbackImpl>(callbackAdapter);
        SensorAdapterImpl::sensorCallbackMap.emplace(i+2,callback[i]);
    }

    SensorEvent* event = new SensorEvent;
    uint8_t *data = new uint8_t[10];
    event->data = data;
    g_sensorAdapter->OhosSensorCallback(event);
    event->sensorTypeId = 7;
    g_sensorAdapter->OhosSensorCallback(event);
    event->sensorTypeId = 2;
    g_sensorAdapter->OhosSensorCallback(event);
    event->sensorTypeId = 3;
    g_sensorAdapter->OhosSensorCallback(event);
    event->sensorTypeId = 4;
    g_sensorAdapter->OhosSensorCallback(event);
    event->sensorTypeId = 5;
    g_sensorAdapter->OhosSensorCallback(event);
    event->sensorTypeId = 6;
    g_sensorAdapter->OhosSensorCallback(event);
    event->sensorTypeId = 8;
    g_sensorAdapter->OhosSensorCallback(event);
    event->sensorTypeId = 9;
    g_sensorAdapter->OhosSensorCallback(event);
    event->sensorTypeId = 11;
    g_sensorAdapter->OhosSensorCallback(event);
    EXPECT_NE(g_sensorAdapter, nullptr);
    delete event;
    SensorAdapterImpl::sensorCallbackMap.clear();
}

/**
 * @tc.name: SensorAdapterImplTest_SensorAdapterImpl_010.
 * @tc.desc: test of SensorAdapterImpl :: handleAccelerometerData()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(SensorAdapterImplTest, SensorAdapterImplTest_SensorAdapterImpl_010, TestSize.Level1)
{
    auto callbackAdapter = std::make_shared<SensorCallbackAdapterMock>();
    auto callback = std::make_shared<OHOS::NWeb::SensorCallbackImpl>(callbackAdapter);
    SensorEvent* event = new SensorEvent;
    g_sensorAdapter->handleAccelerometerData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);

    uint8_t *data = new uint8_t[10];
    event->data = data;
    g_sensorAdapter->handleAccelerometerData(callback, event);
    delete event->data;
    EXPECT_NE(g_sensorAdapter, nullptr);

    delete event;
    event =nullptr;
    g_sensorAdapter->handleAccelerometerData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);

    event = new SensorEvent;
    callback = nullptr;
    g_sensorAdapter->handleAccelerometerData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);

    delete event;
    event =nullptr;
    g_sensorAdapter->handleAccelerometerData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);
}
/**
 * @tc.name: SensorAdapterImplTest_SensorAdapterImpl_011.
 * @tc.desc: test of SensorAdapterImpl :: handleLinearAccelerometerData()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(SensorAdapterImplTest, SensorAdapterImplTest_SensorAdapterImpl_011, TestSize.Level1)
{
    auto callbackAdapter = std::make_shared<SensorCallbackAdapterMock>();
    auto callback = std::make_shared<OHOS::NWeb::SensorCallbackImpl>(callbackAdapter);
    SensorEvent* event = new SensorEvent;
    g_sensorAdapter->handleLinearAccelerometerData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);

    uint8_t *data = new uint8_t[10];
    event->data = data;
    g_sensorAdapter->handleLinearAccelerometerData(callback, event);
    delete event->data;
    EXPECT_NE(g_sensorAdapter, nullptr);

    delete event;
    event =nullptr;
    g_sensorAdapter->handleLinearAccelerometerData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);

    event = new SensorEvent;
    callback = nullptr;
    g_sensorAdapter->handleLinearAccelerometerData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);

    delete event;
    event =nullptr;
    g_sensorAdapter->handleLinearAccelerometerData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);
}

/**
 * @tc.name: SensorAdapterImplTest_SensorAdapterImpl_012.
 * @tc.desc: test of SensorAdapterImpl :: handleGravityData()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(SensorAdapterImplTest, SensorAdapterImplTest_SensorAdapterImpl_012, TestSize.Level1)
{
    auto callbackAdapter = std::make_shared<SensorCallbackAdapterMock>();
    auto callback = std::make_shared<OHOS::NWeb::SensorCallbackImpl>(callbackAdapter);
    SensorEvent* event = new SensorEvent;
    g_sensorAdapter->handleGravityData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);

    uint8_t *data = new uint8_t[10];
    event->data = data;
    g_sensorAdapter->handleGravityData(callback, event);
    delete event->data;
    EXPECT_NE(g_sensorAdapter, nullptr);

    delete event;
    event =nullptr;
    g_sensorAdapter->handleGravityData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);

    event = new SensorEvent;
    callback = nullptr;
    g_sensorAdapter->handleGravityData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);

    delete event;
    event =nullptr;
    g_sensorAdapter->handleGravityData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);
}

/**
 * @tc.name: SensorAdapterImplTest_SensorAdapterImpl_013.
 * @tc.desc: test of SensorAdapterImpl :: handleCyroscopeData()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(SensorAdapterImplTest, SensorAdapterImplTest_SensorAdapterImpl_013, TestSize.Level1)
{
    auto callbackAdapter = std::make_shared<SensorCallbackAdapterMock>();
    auto callback = std::make_shared<OHOS::NWeb::SensorCallbackImpl>(callbackAdapter);
    SensorEvent* event = new SensorEvent;
    g_sensorAdapter->handleCyroscopeData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);

    uint8_t *data = new uint8_t[10];
    event->data = data;
    g_sensorAdapter->handleCyroscopeData(callback, event);
    delete event->data;
    EXPECT_NE(g_sensorAdapter, nullptr);

    delete event;
    event =nullptr;
    g_sensorAdapter->handleCyroscopeData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);

    event = new SensorEvent;
    callback = nullptr;
    g_sensorAdapter->handleCyroscopeData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);

    delete event;
    event =nullptr;
    g_sensorAdapter->handleCyroscopeData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);
}

/**
 * @tc.name: SensorAdapterImplTest_SensorAdapterImpl_014.
 * @tc.desc: test of SensorAdapterImpl :: handleMagnetometerData()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(SensorAdapterImplTest, SensorAdapterImplTest_SensorAdapterImpl_014, TestSize.Level1)
{
    auto callbackAdapter = std::make_shared<SensorCallbackAdapterMock>();
    auto callback = std::make_shared<OHOS::NWeb::SensorCallbackImpl>(callbackAdapter);
    SensorEvent* event = new SensorEvent;
    g_sensorAdapter->handleMagnetometerData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);

    uint8_t *data = new uint8_t[10];
    event->data = data;
    g_sensorAdapter->handleMagnetometerData(callback, event);
    delete event->data;
    EXPECT_NE(g_sensorAdapter, nullptr);

    delete event;
    event =nullptr;
    g_sensorAdapter->handleMagnetometerData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);

    event = new SensorEvent;
    callback = nullptr;
    g_sensorAdapter->handleMagnetometerData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);

    delete event;
    event =nullptr;
    g_sensorAdapter->handleMagnetometerData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);
}

/**
 * @tc.name: SensorAdapterImplTest_SensorAdapterImpl_015.
 * @tc.desc: test of SensorAdapterImpl :: handleOrientationData()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(SensorAdapterImplTest, SensorAdapterImplTest_SensorAdapterImpl_015, TestSize.Level1)
{
    auto callbackAdapter = std::make_shared<SensorCallbackAdapterMock>();
    auto callback = std::make_shared<OHOS::NWeb::SensorCallbackImpl>(callbackAdapter);
    SensorEvent* event = new SensorEvent;
    g_sensorAdapter->handleOrientationData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);

    uint8_t *data = new uint8_t[10];
    event->data = data;
    g_sensorAdapter->handleOrientationData(callback, event);
    delete event->data;
    EXPECT_NE(g_sensorAdapter, nullptr);

    delete event;
    event =nullptr;
    g_sensorAdapter->handleOrientationData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);

    event = new SensorEvent;
    callback = nullptr;
    g_sensorAdapter->handleOrientationData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);

    delete event;
    event =nullptr;
    g_sensorAdapter->handleOrientationData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);
}

/**
 * @tc.name: SensorAdapterImplTest_SensorAdapterImpl_016.
 * @tc.desc: test of SensorAdapterImpl :: handleRotationVectorData()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(SensorAdapterImplTest, SensorAdapterImplTest_SensorAdapterImpl_016, TestSize.Level1)
{
    auto callbackAdapter = std::make_shared<SensorCallbackAdapterMock>();
    auto callback = std::make_shared<OHOS::NWeb::SensorCallbackImpl>(callbackAdapter);
    SensorEvent* event = new SensorEvent;
    g_sensorAdapter->handleRotationVectorData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);

    uint8_t *data = new uint8_t[10];
    event->data = data;
    g_sensorAdapter->handleRotationVectorData(callback, event);
    delete event->data;
    EXPECT_NE(g_sensorAdapter, nullptr);

    delete event;
    event =nullptr;
    g_sensorAdapter->handleRotationVectorData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);

    event = new SensorEvent;
    callback = nullptr;
    g_sensorAdapter->handleRotationVectorData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);

    delete event;
    event =nullptr;
    g_sensorAdapter->handleRotationVectorData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);
}

/**
 * @tc.name: SensorAdapterImplTest_SensorAdapterImpl_017.
 * @tc.desc: test of SensorAdapterImpl :: handleGameRotationVectorData()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(SensorAdapterImplTest, SensorAdapterImplTest_SensorAdapterImpl_017, TestSize.Level1)
{
    auto callbackAdapter = std::make_shared<SensorCallbackAdapterMock>();
    auto callback = std::make_shared<OHOS::NWeb::SensorCallbackImpl>(callbackAdapter);
    SensorEvent* event = new SensorEvent;
    g_sensorAdapter->handleGameRotationVectorData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);

    uint8_t *data = new uint8_t[10];
    event->data = data;
    g_sensorAdapter->handleGameRotationVectorData(callback, event);
    delete event->data;
    EXPECT_NE(g_sensorAdapter, nullptr);

    delete event;
    event =nullptr;
    g_sensorAdapter->handleGameRotationVectorData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);

    event = new SensorEvent;
    callback = nullptr;
    g_sensorAdapter->handleGameRotationVectorData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);

    delete event;
    event =nullptr;
    g_sensorAdapter->handleGameRotationVectorData(callback, event);
    EXPECT_NE(g_sensorAdapter, nullptr);
}
} // namespace OHOS::NWeb