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
 
#ifndef SERVICES_DEVICE_GENERIC_SENSOR_PLATFORM_SENSOR_OHOS_H_
#define SERVICES_DEVICE_GENERIC_SENSOR_PLATFORM_SENSOR_OHOS_H_
 
#include "base/memory/scoped_refptr.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/thread_pool.h"
#include "services/device/generic_sensor/platform_sensor.h"
 
#include "third_party/ohos_ndk/includes/ohos_adapter/sensor_adapter.h"
 
namespace device {
 
class PlatformSensorOHOS : public PlatformSensor {
 public:
  // Creates a new PlatformSensorOHOS for the given sensor type, returning
  // nullptr if it is not supported by the platform.
  static scoped_refptr<PlatformSensorOHOS> Create(
      mojom::SensorType type,
      SensorReadingSharedBuffer* reading_buffer,
      base::WeakPtr<PlatformSensorProvider> provider);
 
  static bool IsSupported(
      mojom::SensorType type);
 
  PlatformSensorOHOS(mojom::SensorType type,
                        SensorReadingSharedBuffer* reading_buffer,
                        base::WeakPtr<PlatformSensorProvider> provider);
 
  PlatformSensorOHOS(const PlatformSensorOHOS&) = delete;
  PlatformSensorOHOS& operator=(const PlatformSensorOHOS&) = delete;
 
  mojom::ReportingMode GetReportingMode() override;
  PlatformSensorConfiguration GetDefaultConfiguration() override;
  double GetMaximumSupportedFrequency() override;
  double GetMinimumSupportedFrequency() override;
  void NotifyPlatformSensorError();
 
  void UpdatePlatformSensorReading(
      double timestamp,
      double value1,
      double value2,
      double value3,
      double value4);
 
  bool IsSupported() {
    return is_supported_;
  }
 
 protected:
  ~PlatformSensorOHOS() override;
  bool StartSensor(const PlatformSensorConfiguration& configuration) override;
  void StopSensor() override;
  bool CheckSensorConfiguration(
      const PlatformSensorConfiguration& configuration) override;
 
 private:
  const scoped_refptr<base::SequencedTaskRunner> sequenced_task_runner_ =
      base::ThreadPool::CreateSequencedTaskRunner({base::MayBlock()});
  std::unique_ptr<OHOS::NWeb::SensorAdapter> sensor_adapter_;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  int64_t current_sampling_interval_ = 0;
  bool is_supported_ = false;
  base::WeakPtrFactory<PlatformSensorOHOS> weak_factory_{this};
};
 
class OHOSSensorCallback : public OHOS::NWeb::SensorCallbackAdapter {
 public:
  OHOSSensorCallback(
    const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
    base::WeakPtr<PlatformSensorOHOS> platform_sensor_ohos);
  virtual ~OHOSSensorCallback();
  OHOSSensorCallback(const OHOSSensorCallback&) = delete;
  OHOSSensorCallback& operator=(const OHOSSensorCallback&) = delete;
 
  void UpdateOhosSensorData(double timestamp, double value1, 
                                      double value2, double value3, double value4) override;
 private:
  base::WeakPtr<PlatformSensorOHOS> platform_sensor_ohos_;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
};
}  // namespace device
 
#endif  // SERVICES_DEVICE_GENERIC_SENSOR_PLATFORM_SENSOR_ANDROID_H_