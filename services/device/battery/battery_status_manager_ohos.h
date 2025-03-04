// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_DEVICE_BATTERY_BATTERY_STATUS_MANAGER_OHOS_H_
#define SERVICES_DEVICE_BATTERY_BATTERY_STATUS_MANAGER_OHOS_H_

#include "ohos/adapter/battery/battery_adapter.h"
#include "services/device/battery/battery_status_manager.h"
#include "services/device/battery/battery_status_service.h"
#include "services/device/public/mojom/battery_status.mojom.h"

namespace device {

typedef BatteryStatusService::BatteryUpdateCallback BatteryCallback;
// Returns WebBatteryStatus corresponding to the given BatteryStatus.
mojom::BatteryStatus ComputeWebBatteryStatus(const BatteryInfo& battery_info);

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
