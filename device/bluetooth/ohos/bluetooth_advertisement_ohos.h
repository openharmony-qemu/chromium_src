// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_BLUETOOTH_OHOS_BLUETOOTH_ADVERTISEMENT_OHOS_H_
#define DEVICE_BLUETOOTH_OHOS_BLUETOOTH_ADVERTISEMENT_OHOS_H_

#include <optional>

#include "base/memory/raw_ptr.h"
#include "base/task/single_thread_task_runner.h"
#include "device/bluetooth/bluetooth_adapter.h"
#include "device/bluetooth/bluetooth_advertisement.h"
#include "device/bluetooth/bluetooth_export.h"
#include "device/bluetooth/ohos/bluetooth_low_energy_advertisement_manager_ohos.h"

namespace device {

class BluetoothLowEnergyAdvertisementManagerOhos;
// Simple implementation of BluetoothAdvertisement for Ohos. The primary logic
// is currently handled in BluetoothLowEnergyAdvertisementManagerOhos.
class DEVICE_BLUETOOTH_EXPORT BluetoothAdvertisementOhos
    : public BluetoothAdvertisement {
 public:
  enum Status {
    WAITING_FOR_ADAPTER,
    ADVERTISEMENT_PENDING,
    ADVERTISING,
    ERROR_ADVERTISING,
    UNREGISTERED,
  };

  BluetoothAdvertisementOhos(
      std::optional<BluetoothAdvertisement::UUIDList> service_uuids,
      BluetoothAdapter::CreateAdvertisementCallback callback,
      BluetoothAdapter::AdvertisementErrorCallback error_callback,
      BluetoothLowEnergyAdvertisementManagerOhos* advertisement_manager);

  BluetoothAdvertisementOhos(const BluetoothAdvertisementOhos&) = delete;
  BluetoothAdvertisementOhos& operator=(const BluetoothAdvertisementOhos&) =
      delete;

  // BluetoothAdvertisement overrides:
  void Unregister(SuccessCallback success_callback,
                  ErrorCallback error_callback) override;

  Status Getstatus() const { return status_; }

  bool IsWaitingForAdapter() { return status_ == WAITING_FOR_ADAPTER; }

  bool IsAdvertising() { return status_ == ADVERTISING; }

  bool IsAdvertisementPending() { return status_ == ADVERTISEMENT_PENDING; }

  const BluetoothAdvertisement::UUIDList& service_uuids() {
    return *service_uuids_;
  }

 private:
  friend class BluetoothLowEnergyAdvertisementManagerOhos;

  ~BluetoothAdvertisementOhos() override;

  // Called by BluetoothLowEnergyAdvertisementManagerOhos.
  void OnAdvertisementPending();
  void OnAdvertisementError(base::SingleThreadTaskRunner* task_runner,
                            BluetoothAdvertisement::ErrorCode error_code);
  void OnAdvertisementSuccess(base::SingleThreadTaskRunner* task_runner);
  void OnAdapterReset();
  void OnAdvertisementRestarted();

  void InvokeSuccessCallback();

  std::optional<BluetoothAdvertisement::UUIDList> service_uuids_;
  BluetoothAdapter::CreateAdvertisementCallback success_callback_;
  BluetoothAdapter::AdvertisementErrorCallback error_callback_;
  raw_ptr<BluetoothLowEnergyAdvertisementManagerOhos> advertisement_manager_;
  Status status_;
};

}  // namespace device

#endif  // DEVICE_BLUETOOTH_OHOS_BLUETOOTH_ADVERTISEMENT_OHOS_H_
