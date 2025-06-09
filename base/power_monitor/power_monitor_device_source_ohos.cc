/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "base/power_monitor/power_monitor_device_source.h"

#include <memory>

namespace ohos::adapter {
class OhosPowerMonitorImpl final : public ohos::adapter::OhosPowerMonitor {
 public:
  OhosPowerMonitorImpl() = default;
  OhosPowerMonitorImpl(const OhosPowerMonitorImpl&) = delete;
  OhosPowerMonitorImpl& operator=(const OhosPowerMonitorImpl&) = delete;
  void OnSuspend() override {
    ProcessPowerEventHelper(base::PowerMonitorSource::SUSPEND_EVENT);
  }
  void OnResume() override {
    ProcessPowerEventHelper(base::PowerMonitorSource::RESUME_EVENT);
  }
  void OnPowerStateChanged() override {
    ProcessPowerEventHelper(base::PowerMonitorSource::POWER_STATE_EVENT);
  }
};
}  // namespace ohos::adapter

namespace base {

// A helper function which is a friend of PowerMonitorSource.
void ProcessPowerEventHelper(PowerMonitorSource::PowerEvent event) {
  PowerMonitorSource::ProcessPowerEvent(event);
}

void PowerMonitorDeviceSource::PlatformInit() {
  if (impl == nullptr) {
    impl = std::make_unique<ohos::adapter::OhosPowerMonitorImpl>();
    ::ohos::adapter::PowerMonitor::SetMonitor(impl.get());
  }
}

void PowerMonitorDeviceSource::PlatformDestroy() {
  if (impl != nullptr) {
    ::ohos::adapter::PowerMonitor::ReleaseMonitor();
    impl = nullptr;
  }
}

PowerStateObserver::BatteryPowerStatus
PowerMonitorDeviceSource::GetBatteryPowerStatus() const {
  if (::ohos::adapter::PowerMonitor::IsOnBatteryPower()) {
    return PowerStateObserver::BatteryPowerStatus::kBatteryPower;
  } else {
    return PowerStateObserver::BatteryPowerStatus::kExternalPower;
  }
}
}  // namespace base
