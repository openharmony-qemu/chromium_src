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

#include <fuzzer/FuzzedDataProvider.h>

#include "base/at_exit.h"
#include "base/task/single_thread_task_executor.h"
#include "base/task/thread_pool/thread_pool_instance.h"
#include "base/test/task_environment.h"
#include "base/test/test_simple_task_runner.h"
#include "services/device/generic_sensor/platform_sensor_provider_ohos.h"

namespace device {

class FuzzPlatformSensorProviderOHOS : public PlatformSensorProviderOHOS {
 public:
  using PlatformSensorProviderOHOS::CreateSensorInternal;
};

static void FuzzCreateSensorCallback(scoped_refptr<PlatformSensor> sensor) {}

void PlatformSensorProviderOHOSFuzzTest(const uint8_t* data, size_t size) {
  FuzzedDataProvider provider(data, size);

  auto sensor_provider = std::make_unique<FuzzPlatformSensorProviderOHOS>();

  auto weak_ptr = sensor_provider->AsWeakPtr();
  if (!weak_ptr) {
    return;
  }

  while (provider.remaining_bytes() > 0) {
    int sensor_type_int = provider.ConsumeIntegralInRange<int>(
        0, static_cast<int>(mojom::SensorType::kMaxValue));
    auto sensor_type = static_cast<mojom::SensorType>(sensor_type_int);

    sensor_provider->CreateSensorInternal(
        sensor_type,
        base::BindOnce(&FuzzCreateSensorCallback));
  }
}

}  // namespace device

static base::AtExitManager* g_at_exit = nullptr;
base::SequencedTaskRunner::CurrentDefaultHandle *g_stcd = nullptr;
base::SingleThreadTaskRunner::CurrentDefaultHandle *g_sttcd = nullptr;

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (data == nullptr || size == 0) {
    return 0;
  }

  if (!initialized) {
    g_at_exit = new base::AtExitManager();
    base::ThreadPoolInstance::CreateAndStartWithDefaultParams("fuzzer");
    scoped_refptr<base::SequencedTaskRunner> sequencedTaskRunner = base::MakeRefCounted<base::TestSimpleTaskRunner>();
    scoped_refptr<base::SingleThreadTaskRunner> singleThreadTaskRunner(base::MakeRefCounted<base::TestSimpleTaskRunner>());
    g_stcd = new base::SequencedTaskRunner::CurrentDefaultHandle(sequencedTaskRunner);
    g_sttcd = new base::SingleThreadTaskRunner::CurrentDefaultHandle(singleThreadTaskRunner);
    initialized = true;
  }

  device::PlatformSensorProviderOHOSFuzzTest(data, size);
  return 0;
}
