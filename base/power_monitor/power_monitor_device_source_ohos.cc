// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>

#include "base/power_monitor/power_monitor_device_source.h"

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
