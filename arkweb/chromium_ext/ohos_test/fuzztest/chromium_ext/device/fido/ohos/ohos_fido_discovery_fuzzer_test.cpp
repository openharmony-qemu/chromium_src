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

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

#include "base/at_exit.h"
#include "base/run_loop.h"
#include "base/task/single_thread_task_executor.h"
#include "device/fido/fido_authenticator.h"
#include "device/fido/fido_discovery_base.h"
#include "device/fido/ohos/ohos_authenticator.h"
#include "device/fido/ohos/ohos_fido_discovery.h"
#include "device/fido/ohos/ohos_webauthn_api.h"

namespace {

class MockDiscoveryObserver : public device::FidoDiscoveryBase::Observer {
 public:
  MockDiscoveryObserver() = default;
  ~MockDiscoveryObserver() override = default;

  void DiscoveryStarted(device::FidoDiscoveryBase* discovery,
                        bool success,
                        std::vector<device::FidoAuthenticator*> authenticators)
                        override {
    discovery_started_called_ = true;
    discovery_success_ = success;
    authenticator_count_ = authenticators.size();
  }

  void AuthenticatorAdded(device::FidoDiscoveryBase* discovery,
                          device::FidoAuthenticator* authenticator) override {}

  void AuthenticatorRemoved(device::FidoDiscoveryBase* discovery,
                            device::FidoAuthenticator* authenticator) override {}

  bool discovery_started_called_ = false;
  bool discovery_success_ = false;
  size_t authenticator_count_ = 0;
};

void FuzzStartWithoutObserver(FuzzedDataProvider& provider) {
  device::OhosFidoDiscovery discovery;
  discovery.Start();
}

void FuzzStartWithObserver(FuzzedDataProvider& provider) {
  device::OhosFidoDiscovery discovery;
  MockDiscoveryObserver observer;
  discovery.set_observer(&observer);
  discovery.Start();

  base::RunLoop().RunUntilIdle();

  (void)observer.discovery_started_called_;
  (void)observer.discovery_success_;
  (void)observer.authenticator_count_;
}

void FuzzMultipleStarts(FuzzedDataProvider& provider) {
  device::OhosFidoDiscovery discovery;
  MockDiscoveryObserver observer;
  discovery.set_observer(&observer);

  auto count = provider.ConsumeIntegralInRange<int>(1, 3);
  for (int i = 0; i < count; ++i) {
    discovery.Start();
    base::RunLoop().RunUntilIdle();
  }
}

void FuzzTransportAndObserver(FuzzedDataProvider& provider) {
  device::OhosFidoDiscovery discovery;

  auto transport = discovery.transport();
  (void)transport;

  auto* obs = discovery.observer();
  (void)obs;

  MockDiscoveryObserver observer;
  discovery.set_observer(&observer);
  obs = discovery.observer();
  (void)obs;
}

}  // namespace

static base::AtExitManager* g_at_exit = nullptr;
static std::optional<base::SingleThreadTaskExecutor> g_executor;

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  static bool initialized = false;
  if (!initialized) {
    g_at_exit = new base::AtExitManager();
    g_executor.emplace(base::MessagePumpType::DEFAULT);
    initialized = true;
  }

  FuzzedDataProvider provider(data, size);

  uint8_t action = provider.ConsumeIntegralInRange<uint8_t>(0, 3);
  switch (action) {
    case 0:
      FuzzStartWithoutObserver(provider);
      break;
    case 1:
      FuzzStartWithObserver(provider);
      break;
    case 2:
      FuzzMultipleStarts(provider);
      break;
    case 3:
      FuzzTransportAndObserver(provider);
      break;
    default:
      break;
  }

  return 0;
}
