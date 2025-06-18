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

#include "services/device/battery/battery_status_manager_ohos.h"

#include <memory>
#include <string>

#include "base/logging.h"
#include "ohos/adapter/battery/battery_adapter.h"

using namespace ohos::adapter::battery;
namespace device {

class BatteryUpdateCallbackImpl : public BatteryUpdateCallback {
 public:
  explicit BatteryUpdateCallbackImpl(
      BatteryStatusObserver* battery_status_observer_ohos)
      : observer_ohos_(battery_status_observer_ohos) {}
  virtual ~BatteryUpdateCallbackImpl() = default;
  void OnBatteryStatusChanged(const BatteryInfo& battery_info) override;

 private:
  raw_ptr<BatteryStatusObserver> observer_ohos_ = nullptr;
};

void BatteryUpdateCallbackImpl::OnBatteryStatusChanged(const BatteryInfo& battery_info) {
  if (observer_ohos_) {
    observer_ohos_->BatteryChanged(battery_info);
  }
}

mojom::BatteryStatus ComputeWebBatteryStatus(const BatteryInfo& battery_info) {
  mojom::BatteryStatus status;
  double full_battery_soc = 100.f;
  int max_battery_level = 1;
  if (battery_info.battery_soc != INVALID_BATT_INT_VALUE) {
    status.level = battery_info.battery_soc / full_battery_soc;
  }

  if (battery_info.charging_status != BatteryChargeState::CHARGE_STATE_ENABLE &&
      battery_info.charging_status != BatteryChargeState::CHARGE_STATE_BUTT) {
    status.charging = false;
  }

  if (status.charging) {
    bool is_charge_full = (status.level >= max_battery_level);
    bool is_charge_time_valid =
        (battery_info.estimated_remaining_charge_time != INVALID_BATT_INT_VALUE);
    status.charging_time =
        (is_charge_full
             ? 0
             : (is_charge_time_valid ? battery_info.estimated_remaining_charge_time
                                  : std::numeric_limits<double>::infinity()));
  } else {
    if (battery_info.now_current != INVALID_BATT_INT_VALUE &&
        battery_info.remaining_energy != INVALID_BATT_INT_VALUE) {
      status.discharging_time =
          static_cast<double>(battery_info.remaining_energy) /
          battery_info.now_current;
    } else {
      status.discharging_time = std::numeric_limits<double>::infinity();
    }
    status.charging_time = std::numeric_limits<double>::infinity();
  }

  return status;
}

BatteryStatusObserver::BatteryStatusObserver(const BatteryCallback& callback)
    : callback_(callback) {}

BatteryStatusObserver::~BatteryStatusObserver() {
  if (battery_update_callback_ != nullptr) {
    battery_update_callback_.reset();
  }
}

void BatteryStatusObserver::Start() {
  BatteryInfo battery_info;
  if (BatteryAdapter::GetInstance().GetBatteryInfo(battery_info)) {
    callback_.Run(ComputeWebBatteryStatus(battery_info));
  } else {
    callback_.Run(mojom::BatteryStatus());
  }

  battery_update_callback_ = std::make_shared<BatteryUpdateCallbackImpl>(this);
  if (!battery_update_callback_) {
    LOG(WARNING) << "create battery update callback failed";
    return;
  }
  if (!BatteryAdapter::GetInstance().RegisterBatteryUpdateCallback(
      battery_update_callback_)) {
    LOG(WARNING) << "register battery update callback failed";
  }
}

void BatteryStatusObserver::Stop() {
  BatteryAdapter::GetInstance().UnregisterBatteryUpdateCallback();
}

void BatteryStatusObserver::BatteryChanged(const BatteryInfo& battery_info) {
  callback_.Run(ComputeWebBatteryStatus(battery_info));
}

BatteryStatusManagerOhos::BatteryStatusManagerOhos(
    const BatteryCallback& callback)
    : battery_observer_(std::make_unique<BatteryStatusObserver>(callback)) {}

BatteryStatusManagerOhos::~BatteryStatusManagerOhos() {
  if (battery_observer_ != nullptr) {
    battery_observer_->Stop();
    battery_observer_.reset();
  }
}

bool BatteryStatusManagerOhos::StartListeningBatteryChange() {
  if (battery_observer_ != nullptr) {
    battery_observer_->Start();
    return true;
  }
  return false;
}

void BatteryStatusManagerOhos::StopListeningBatteryChange() {
  if (battery_observer_ != nullptr) {
    battery_observer_->Stop();
  }
}

// static
std::unique_ptr<BatteryStatusManager> BatteryStatusManager::Create(
    const BatteryStatusService::BatteryUpdateCallback& callback) {
  return std::make_unique<BatteryStatusManagerOhos>(callback);
}

}  // namespace device
