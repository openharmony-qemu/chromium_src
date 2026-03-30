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

#include <fuzzer/FuzzedDataProvider.h>

#include <limits>
#include <memory>

#include "base/at_exit.h"
#include "base/task/single_thread_task_executor.h"
#include "base/task/thread_pool/thread_pool_instance.h"
#include "base/test/task_environment.h"
#include "base/test/test_simple_task_runner.h"
#define private public
#define protected public
#include "services/device/generic_sensor/platform_sensor_ohos.h"
#include "services/device/public/cpp/generic_sensor/platform_sensor_configuration.h"
#include "services/device/public/cpp/generic_sensor/sensor_reading.h"
#include "services/device/public/cpp/generic_sensor/sensor_reading_shared_buffer.h"
#include "services/device/public/mojom/sensor.mojom.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#undef protected
#undef private

namespace device {

using mojom::SensorType;

void FuzzIsSupported(FuzzedDataProvider& provider) {
  int sensor_type_int = provider.ConsumeIntegralInRange<int>(
      static_cast<int>(SensorType::kMinValue),
      static_cast<int>(SensorType::kMaxValue));
  auto type = static_cast<SensorType>(sensor_type_int);
  PlatformSensorOHOS::IsSupported(type);
}

void FuzzConstructorNullBuffer(FuzzedDataProvider& provider) {
  int sensor_type_int = provider.ConsumeIntegralInRange<int>(
      static_cast<int>(SensorType::kMinValue),
      static_cast<int>(SensorType::kMaxValue));
  auto type = static_cast<SensorType>(sensor_type_int);

  auto sensor = PlatformSensorOHOS::Create(
      type, &buffer, base::WeakPtr<device::PlatformSensorProvider>());
  if (!sensor) {
    return;
  }

  sensor->GetReportingMode();
  sensor->GetDefaultConfiguration();
  sensor->GetMaximumSupportedFrequency();
  sensor->GetMinimumSupportedFrequency();
  sensor->IsSupported();
}

void FuzzStartSensorEdgeCases(FuzzedDataProvider& provider) {
  auto type = SensorType::ACCELEROMETER;
  device::SensorReadingSharedBuffer buffer;

  auto sensor = PlatformSensorOHOS::Create(
      type, &buffer, base::WeakPtr<device::PlatformSensorProvider>());
  if (!sensor) {
    return;
  }

  double frequency = provider.ConsumeFloatingPoint<double>();
  device::PlatformSensorConfiguration config(frequency);
  sensor->StartSensor(config);

  sensor->StartSensor(device::PlatformSensorConfiguration(0.0));
  sensor->StartSensor(device::PlatformSensorConfiguration(-1.0));
  sensor->StartSensor(device::PlatformSensorConfiguration(0.0005));
  sensor->StartSensor(device::PlatformSensorConfiguration(0.001));
  sensor->StartSensor(device::PlatformSensorConfiguration(0.002));
  sensor->StartSensor(device::PlatformSensorConfiguration(1000000.0));
  sensor->StartSensor(
      device::PlatformSensorConfiguration(std::numeric_limits<double>::max()));
  sensor->StartSensor(
      device::PlatformSensorConfiguration(std::numeric_limits<double>::lowest()));
  sensor->StartSensor(
      device::PlatformSensorConfiguration(std::numeric_limits<double>::quiet_NaN()));
  sensor->StartSensor(
      device::PlatformSensorConfiguration(std::numeric_limits<double>::infinity()));

  sensor->StopSensor();
}

void FuzzCheckSensorConfiguration(FuzzedDataProvider& provider) {
  auto type = SensorType::GYROSCOPE;
  device::SensorReadingSharedBuffer buffer;

  auto sensor = PlatformSensorOHOS::Create(
      type, &buffer, base::WeakPtr<device::PlatformSensorProvider>());
  if (!sensor) {
    return;
  }

  double frequency = provider.ConsumeFloatingPoint<double>();
  device::PlatformSensorConfiguration config(frequency);
  sensor->CheckSensorConfiguration(config);

  sensor->CheckSensorConfiguration(device::PlatformSensorConfiguration(0.0));
  sensor->CheckSensorConfiguration(device::PlatformSensorConfiguration(-10.0));
  sensor->CheckSensorConfiguration(device::PlatformSensorConfiguration(0.001));
  sensor->CheckSensorConfiguration(device::PlatformSensorConfiguration(60.0));
  sensor->CheckSensorConfiguration(device::PlatformSensorConfiguration(100.0));
  sensor->CheckSensorConfiguration(
      device::PlatformSensorConfiguration(std::numeric_limits<double>::max()));
}

void FuzzUpdatePlatformSensorReading(FuzzedDataProvider& provider) {
  auto type = SensorType::MAGNETOMETER;
  device::SensorReadingSharedBuffer buffer;

  auto sensor = PlatformSensorOHOS::Create(
      type, &buffer, base::WeakPtr<device::PlatformSensorProvider>());
  if (!sensor) {
    return;
  }

  double timestamp = provider.ConsumeFloatingPoint<double>();
  double value1 = provider.ConsumeFloatingPoint<double>();
  double value2 = provider.ConsumeFloatingPoint<double>();
  double value3 = provider.ConsumeFloatingPoint<double>();
  double value4 = provider.ConsumeFloatingPoint<double>();

  sensor->UpdatePlatformSensorReading(timestamp, value1, value2, value3, value4);

  sensor->UpdatePlatformSensorReading(0.0, 0.0, 0.0, 0.0, 0.0);
  sensor->UpdatePlatformSensorReading(-1.0, 1.0, -1.0, 1.0, -1.0);
  sensor->UpdatePlatformSensorReading(
      std::numeric_limits<double>::max(),
      std::numeric_limits<double>::max(),
      std::numeric_limits<double>::lowest(),
      std::numeric_limits<double>::infinity(),
      std::numeric_limits<double>::quiet_NaN());
  sensor->UpdatePlatformSensorReading(1e18, 1e18, -1e18, 1e-18, -1e-18);
}

void FuzzAllSensorTypes(FuzzedDataProvider& provider) {
  device::SensorReadingSharedBuffer buffer;

  int sensor_type_int = provider.ConsumeIntegralInRange<int>(
      static_cast<int>(SensorType::kMinValue),
      static_cast<int>(SensorType::kMaxValue));
  auto type = static_cast<SensorType>(sensor_type_int);

  auto sensor = base::MakeRefCounted<PlatformSensorOHOS>(
      type, &buffer, base::WeakPtr<device::PlatformSensorProvider>());

  sensor->GetReportingMode();
  sensor->GetDefaultConfiguration();
  sensor->GetMaximumSupportedFrequency();
  sensor->GetMinimumSupportedFrequency();

  double frequency = provider.ConsumeFloatingPointInRange<double>(0.0, 200.0);
  device::PlatformSensorConfiguration config(frequency);
  sensor->CheckSensorConfiguration(config);
  sensor->StartSensor(config);
  sensor->StopSensor();

  double ts = provider.ConsumeFloatingPoint<double>();
  double v1 = provider.ConsumeFloatingPoint<double>();
  double v2 = provider.ConsumeFloatingPoint<double>();
  double v3 = provider.ConsumeFloatingPoint<double>();
  double v4 = provider.ConsumeFloatingPoint<double>();
  sensor->UpdatePlatformSensorReading(ts, v1, v2, v3, v4);
}

}  // namespace

static base::AtExitManager* g_at_exit = nullptr;
base::SequencedTaskRunner::CurrentDefaultHandle *g_stcd = nullptr;
base::SingleThreadTaskRunner::CurrentDefaultHandle *g_sttcd = nullptr;

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  static bool initialized = false;
  if (!initialized) {
    g_at_exit = new base::AtExitManager();
    base::ThreadPoolInstance::CreateAndStartWithDefaultParams("fuzzer");
    scoped_refptr<base::SequencedTaskRunner> sequencedTaskRunner = base::MakeRefCounted<base::TestSimpleTaskRunner>();
    scoped_refptr<base::SingleThreadTaskRunner> singleThreadTaskRunner(base::MakeRefCounted<base::TestSimpleTaskRunner>());
    g_stcd = new base::SequencedTaskRunner::CurrentDefaultHandle(sequencedTaskRunner);
    g_sttcd = new base::SingleThreadTaskRunner::CurrentDefaultHandle(singleThreadTaskRunner);
    initialized = true;
  }

  FuzzedDataProvider provider(data, size);

  uint8_t action = provider.ConsumeIntegralInRange<uint8_t>(0, 5);
  switch (action) {
    case 0:
      device::FuzzIsSupported(provider);
      break;
    case 1:
      device::FuzzConstructorNullBuffer(provider);
      break;
    case 2:
      device::FuzzStartSensorEdgeCases(provider);
      break;
    case 3:
      device::FuzzCheckSensorConfiguration(provider);
      break;
    case 4:
      device::FuzzUpdatePlatformSensorReading(provider);
      break;
    case 5:
      device::FuzzAllSensorTypes(provider);
      break;
    default:
      break;
  }

  return 0;
}
