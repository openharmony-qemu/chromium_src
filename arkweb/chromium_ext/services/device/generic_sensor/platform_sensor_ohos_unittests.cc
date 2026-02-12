// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <gtest/gtest.h>
#include "base/functional/bind.h"
#include "base/metrics/field_trial_params.h"
#include "base/test/test_simple_task_runner.h"
#include "services/device/public/cpp/device_features.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "services/device/public/cpp/generic_sensor/sensor_reading_shared_buffer.h"

#define private public
#define protected public
#include "services/device/generic_sensor/platform_sensor_ohos.h"
#include "services/device/generic_sensor/platform_sensor_provider_ohos.h"

using namespace testing;
using namespace base;
namespace device {

class PlatformSensorOHOSTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    void CreateSensorCallback(scoped_refptr<PlatformSensor> platformSensor) {}
    SequencedTaskRunner::CurrentDefaultHandle *stcd;
    SingleThreadTaskRunner::CurrentDefaultHandle *sttcd;
    scoped_refptr<PlatformSensorOHOS> sensor;
};

void PlatformSensorOHOSTest::SetUpTestCase() {}

void PlatformSensorOHOSTest::TearDownTestCase() {}

void PlatformSensorOHOSTest::SetUp()
{
    scoped_refptr<SequencedTaskRunner> sequencedTaskRunner = MakeRefCounted<TestSimpleTaskRunner>();
    scoped_refptr<SingleThreadTaskRunner> singleThreadTaskRunner(MakeRefCounted<TestSimpleTaskRunner>());
    stcd = new SequencedTaskRunner::CurrentDefaultHandle(sequencedTaskRunner);
    sttcd = new SingleThreadTaskRunner::CurrentDefaultHandle(singleThreadTaskRunner);
    mojom::SensorType type = mojom::SensorType::ACCELEROMETER;
    SensorReadingSharedBuffer reading_buffer;
    PlatformSensorProviderOHOS platformSensorProviderOHOS;
    WeakPtr<PlatformSensorProvider> provider = platformSensorProviderOHOS.AsWeakPtr();
    sensor = MakeRefCounted<PlatformSensorOHOS>(type, &reading_buffer, provider);
}

void PlatformSensorOHOSTest::TearDown()
{
    delete sttcd;
    delete stcd;
}

TEST_F(PlatformSensorOHOSTest, GetReportingModeTest_001)
{
    auto ret = sensor->GetReportingMode();
    EXPECT_EQ(ret, mojom::ReportingMode::CONTINUOUS);
    sensor->sensor_adapter_ = nullptr;
    ret = sensor->GetReportingMode();
    EXPECT_EQ(ret, mojom::ReportingMode::CONTINUOUS);
}

TEST_F(PlatformSensorOHOSTest, GetDefaultConfiguration_001)
{
    auto configuration = sensor->GetDefaultConfiguration();
    EXPECT_EQ(configuration.frequency(), 5);
    sensor->sensor_adapter_ = nullptr;
    configuration = sensor->GetDefaultConfiguration();
    EXPECT_EQ(configuration.frequency(), 0);
}

TEST_F(PlatformSensorOHOSTest, GetMaximumSupportedFrequencyTest_001)
{
    auto ret = sensor->GetMaximumSupportedFrequency();
    EXPECT_EQ(ret, 500);
    sensor->sensor_adapter_ = nullptr;
    ret = sensor->GetMaximumSupportedFrequency();
    EXPECT_EQ(ret, 0);
}

TEST_F(PlatformSensorOHOSTest, GetMinimumSupportedFrequencyTest_001)
{
    auto ret = sensor->GetMinimumSupportedFrequency();
    EXPECT_EQ(ret, 5);
    sensor->sensor_adapter_ = nullptr;
    ret = sensor->GetMinimumSupportedFrequency();
    EXPECT_EQ(ret, 0);
}

TEST_F(PlatformSensorOHOSTest, CheckSensorConfigurationTest_001)
{
    PlatformSensorConfiguration configuration(5.0);
    auto checkVal = sensor->CheckSensorConfiguration(configuration);
    EXPECT_TRUE(checkVal);
    configuration.set_frequency(0.0);
    checkVal = sensor->CheckSensorConfiguration(configuration);
    EXPECT_FALSE(checkVal);
    configuration.set_frequency(1000.0);
    checkVal = sensor->CheckSensorConfiguration(configuration);
    EXPECT_FALSE(checkVal);
    sensor->sensor_adapter_ = nullptr;
    checkVal = sensor->CheckSensorConfiguration(configuration);
    EXPECT_FALSE(checkVal);
}

TEST_F(PlatformSensorOHOSTest, StartSensorTest_001)
{
    PlatformSensorConfiguration configuration(5.0);
    auto startVal = sensor->StartSensor(configuration);
    EXPECT_TRUE(startVal);
    configuration.set_frequency(1000000001.0);
    startVal = sensor->StartSensor(configuration);
    EXPECT_FALSE(startVal);
    sensor->type_ = static_cast<mojom::SensorType>(0);
    startVal = sensor->StartSensor(configuration);
    EXPECT_TRUE(startVal);
    sensor->current_sampling_interval_ = 1;
    startVal = sensor->StartSensor(configuration);
    EXPECT_FALSE(startVal);
    configuration.set_frequency(0.0);
    startVal = sensor->StartSensor(configuration);
    EXPECT_FALSE(startVal);
    sensor->sensor_adapter_ = nullptr;
    startVal = sensor->StartSensor(configuration);
    sensor->StopSensor();
    EXPECT_FALSE(startVal);
}

TEST_F(PlatformSensorOHOSTest, CreateTest_001)
{
    mojom::SensorType type = mojom::SensorType::ACCELEROMETER;
    SensorReadingSharedBuffer reading_buffer;
    PlatformSensorProviderOHOS platformSensorProviderOHOS;
    platformSensorProviderOHOS.CreateSensorInternal(mojom::SensorType::GRAVITY,
        BindOnce(&PlatformSensorOHOSTest::CreateSensorCallback, Unretained(this)));
    platformSensorProviderOHOS.CreateSensorInternal(mojom::SensorType::LINEAR_ACCELERATION,
        BindOnce(&PlatformSensorOHOSTest::CreateSensorCallback, Unretained(this)));
    platformSensorProviderOHOS.CreateSensorInternal(mojom::SensorType::ABSOLUTE_ORIENTATION_EULER_ANGLES,
        BindOnce(&PlatformSensorOHOSTest::CreateSensorCallback, Unretained(this)));
    platformSensorProviderOHOS.CreateSensorInternal(mojom::SensorType::ABSOLUTE_ORIENTATION_QUATERNION,
        BindOnce(&PlatformSensorOHOSTest::CreateSensorCallback, Unretained(this)));
    platformSensorProviderOHOS.CreateSensorInternal(mojom::SensorType::RELATIVE_ORIENTATION_EULER_ANGLES,
        BindOnce(&PlatformSensorOHOSTest::CreateSensorCallback, Unretained(this)));
    platformSensorProviderOHOS.CreateSensorInternal(mojom::SensorType::RELATIVE_ORIENTATION_QUATERNION,
        BindOnce(&PlatformSensorOHOSTest::CreateSensorCallback, Unretained(this)));
    platformSensorProviderOHOS.CreateSensorInternal(static_cast<mojom::SensorType>(0),
        BindOnce(&PlatformSensorOHOSTest::CreateSensorCallback, Unretained(this)));
    WeakPtr<PlatformSensorProvider> provider = platformSensorProviderOHOS.AsWeakPtr();
    auto ret = PlatformSensorOHOS::Create(type, &reading_buffer, provider);
    EXPECT_NE(ret, nullptr);
    ret = PlatformSensorOHOS::Create(static_cast<mojom::SensorType>(0), &reading_buffer, provider);
    EXPECT_EQ(ret, nullptr);
}

TEST_F(PlatformSensorOHOSTest, IsSupportedTest_001)
{
    auto ret = PlatformSensorOHOS::IsSupported(static_cast<mojom::SensorType>(0));
    EXPECT_FALSE(ret);
    ret = PlatformSensorOHOS::IsSupported(mojom::SensorType::ACCELEROMETER);
    EXPECT_TRUE(ret);
}

TEST_F(PlatformSensorOHOSTest, StopTest_001)
{
    sensor->current_sampling_interval_ = 1;
    sensor->StopSensor();
    EXPECT_EQ(sensor->current_sampling_interval_, 0);
    sensor->sensor_adapter_ = nullptr;
    sensor->StopSensor();
}

TEST_F(PlatformSensorOHOSTest, IsSupportedInstanceTest_001)
{
    auto ret = sensor->IsSupported();
    EXPECT_TRUE(ret);
}

TEST_F(PlatformSensorOHOSTest, ConstructorTest_001)
{
    mojom::SensorType type = mojom::SensorType::GYROSCOPE;
    SensorReadingSharedBuffer reading_buffer;
    PlatformSensorProviderOHOS platformSensorProviderOHOS;
    WeakPtr<PlatformSensorProvider> provider = platformSensorProviderOHOS.AsWeakPtr();
    auto sensor2 = MakeRefCounted<PlatformSensorOHOS>(type, &reading_buffer, provider);
    EXPECT_NE(sensor2, nullptr);
}

TEST_F(PlatformSensorOHOSTest, GetReportingModeTest_002)
{
    mojom::SensorType type = mojom::SensorType::GYROSCOPE;
    SensorReadingSharedBuffer reading_buffer;
    PlatformSensorProviderOHOS platformSensorProviderOHOS;
    WeakPtr<PlatformSensorProvider> provider = platformSensorProviderOHOS.AsWeakPtr();
    auto gyroSensor = MakeRefCounted<PlatformSensorOHOS>(type, &reading_buffer, provider);
    auto ret = gyroSensor->GetReportingMode();
    EXPECT_EQ(ret, mojom::ReportingMode::CONTINUOUS);
}

TEST_F(PlatformSensorOHOSTest, GetDefaultConfigurationTest_002)
{
    mojom::SensorType type = mojom::SensorType::GYROSCOPE;
    SensorReadingSharedBuffer reading_buffer;
    PlatformSensorProviderOHOS platformSensorProviderOHOS;
    WeakPtr<PlatformSensorProvider> provider = platformSensorProviderOHOS.AsWeakPtr();
    auto gyroSensor = MakeRefCounted<PlatformSensorOHOS>(type, &reading_buffer, provider);
    auto configuration = gyroSensor->GetDefaultConfiguration();
    EXPECT_EQ(configuration.frequency(), 5);
}

TEST_F(PlatformSensorOHOSTest, CheckSensorConfigurationTest_002)
{
    PlatformSensorConfiguration configuration(10.0);
    auto checkVal = sensor->CheckSensorConfiguration(configuration);
    EXPECT_TRUE(checkVal);
    configuration.set_frequency(500.0);
    checkVal = sensor->CheckSensorConfiguration(configuration);
    EXPECT_TRUE(checkVal);
    configuration.set_frequency(501.0);
    checkVal = sensor->CheckSensorConfiguration(configuration);
    EXPECT_FALSE(checkVal);
    configuration.set_frequency(4.9);
    checkVal = sensor->CheckSensorConfiguration(configuration);
    EXPECT_FALSE(checkVal);
}

TEST_F(PlatformSensorOHOSTest, StartSensorTest_002)
{
    PlatformSensorConfiguration configuration(10.0);
    auto startVal = sensor->StartSensor(configuration);
    EXPECT_TRUE(startVal);
    sensor->StopSensor();
    configuration.set_frequency(500.0);
    startVal = sensor->StartSensor(configuration);
    EXPECT_TRUE(startVal);
    sensor->StopSensor();
}

TEST_F(PlatformSensorOHOSTest, CreateTest_002)
{
    mojom::SensorType type = mojom::SensorType::GYROSCOPE;
    SensorReadingSharedBuffer reading_buffer;
    PlatformSensorProviderOHOS platformSensorProviderOHOS;
    WeakPtr<PlatformSensorProvider> provider = platformSensorProviderOHOS.AsWeakPtr();
    auto ret = PlatformSensorOHOS::Create(type, &reading_buffer, provider);
    EXPECT_NE(ret, nullptr);
    type = mojom::SensorType::MAGNETOMETER;
    ret = PlatformSensorOHOS::Create(type, &reading_buffer, provider);
    EXPECT_NE(ret, nullptr);
}

} // namespace device