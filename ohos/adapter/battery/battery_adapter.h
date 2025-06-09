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

#ifndef OHOS_ADAPTER_BATTERY_BATTERY_ADAPTER_H_
#define OHOS_ADAPTER_BATTERY_BATTERY_ADAPTER_H_

#include <atomic>
#include <string>

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/export.h"

namespace ohos::adapter::battery {

static constexpr int32_t INVALID_BATT_INT_VALUE = -1;
static constexpr bool INVALID_BATT_BOOL_VALUE = true;

enum class BatteryChargeState : uint32_t {
  CHARGE_STATE_NONE,     // Battery is discharge.
  CHARGE_STATE_ENABLE,   // Battery is charging.
  CHARGE_STATE_DISABLE,  // Battery is not charging.
  CHARGE_STATE_FULL,     // Battery charge full.
  CHARGE_STATE_BUTT      // The bottom of the enum.
};

struct BatteryInfo {
  int32_t battery_soc = INVALID_BATT_INT_VALUE;
  BatteryChargeState charging_status = BatteryChargeState::CHARGE_STATE_BUTT;
  bool is_battery_present = INVALID_BATT_BOOL_VALUE;
  int32_t estimated_remaining_charge_time = INVALID_BATT_INT_VALUE;
  int32_t now_current = INVALID_BATT_INT_VALUE;
  int32_t remaining_energy = INVALID_BATT_INT_VALUE;
};

class ADAPTER_EXPORT_API BatteryUpdateCallback
    : public std::enable_shared_from_this<BatteryUpdateCallback> {
 public:
  BatteryUpdateCallback() = default;
  virtual ~BatteryUpdateCallback() = default;
  virtual void OnBatteryStatusChanged(const BatteryInfo& battery_info) = 0;
};

class ADAPTER_EXPORT_API BatteryAdapter {
 public:
  static BatteryAdapter& GetInstance();
  void ConvertBatteryInfo(aki::Value battery_info,
                          BatteryInfo& ohos_battery_info);
  bool GetBatteryInfo(BatteryInfo& battery_info_ohos);
  bool RegisterBatteryUpdateCallback(
      std::shared_ptr<BatteryUpdateCallback> battery_update_callback);
  void UnregisterBatteryUpdateCallback();

 private:
  BatteryAdapter() = default;
  ~BatteryAdapter() = default;
  BatteryAdapter(const BatteryAdapter&) = delete;
  BatteryAdapter& operator=(const BatteryAdapter&) = delete;

  std::atomic_bool is_active_ = false;
};

}  // namespace ohos::adapter::battery

using BatteryInfo = ohos::adapter::battery::BatteryInfo;

#endif  // OHOS_ADAPTER_BATTERY_BATTERY_ADAPTER_H_
