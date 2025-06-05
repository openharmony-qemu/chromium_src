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

#ifndef SERVICES_DEVICE_BATTERY_BATTERY_STATUS_MANAGER_OHOS_H_
#define SERVICES_DEVICE_BATTERY_BATTERY_STATUS_MANAGER_OHOS_H_

#include "ohos/adapter/battery/battery_adapter.h"
#include "services/device/battery/battery_status_manager.h"
#include "services/device/battery/battery_status_service.h"
#include "services/device/public/mojom/battery_status.mojom.h"

namespace device {

typedef BatteryStatusService::BatteryUpdateCallback BatteryCallback;
// Returns WebBatteryStatus corresponding to the given BatteryStatus.
mojom::BatteryStatus ComputeWebBatteryStatus(
    const BatteryInfo& battery_info);

class BatteryStatusObserver {
 public:
  explicit BatteryStatusObserver(const BatteryCallback& callback);
  BatteryStatusObserver(const BatteryStatusObserver&) = delete;
  BatteryStatusObserver& operator=(const BatteryStatusObserver&) = delete;
  ~BatteryStatusObserver();

  void Start();
  void Stop();
  void BatteryChanged(const BatteryInfo& battery_info);

 private:
  BatteryCallback callback_;
  std::shared_ptr<ohos::adapter::battery::BatteryUpdateCallback>
      battery_update_callback_ = nullptr;
};

class BatteryStatusManagerOhos : public BatteryStatusManager {
 public:
  explicit BatteryStatusManagerOhos(const BatteryCallback& callback);
  BatteryStatusManagerOhos(const BatteryStatusManagerOhos&) = delete;
  BatteryStatusManagerOhos& operator=(const BatteryStatusManagerOhos&) = delete;
  ~BatteryStatusManagerOhos() override;

  bool StartListeningBatteryChange() override;
  void StopListeningBatteryChange() override;

 private:
  std::unique_ptr<BatteryStatusObserver> battery_observer_ = nullptr;
};

}  // namespace device

#endif  // SERVICES_DEVICE_BATTERY_BATTERY_STATUS_MANAGER_OHOS_H_
