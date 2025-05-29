// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "device/bluetooth/ohos/bluetooth_low_energy_advertisement_manager_ohos.h"

#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/strings/sys_string_conversions.h"
#include "base/task/single_thread_task_runner.h"
#include "device/bluetooth/bluetooth_advertisement.h"
#include "device/bluetooth/ohos/bluetooth_adapter_types.h"

using namespace ohos::adapter::device;

namespace device {

BluetoothLowEnergyAdvertisementManagerOhos::
    BluetoothLowEnergyAdvertisementManagerOhos() {}

BluetoothLowEnergyAdvertisementManagerOhos::
    ~BluetoothLowEnergyAdvertisementManagerOhos() {}

void BluetoothLowEnergyAdvertisementManagerOhos::Init(
    scoped_refptr<base::SingleThreadTaskRunner> ui_task_runner) {
  ui_task_runner_ = ui_task_runner;
}

void BluetoothLowEnergyAdvertisementManagerOhos::
    OnPeripheralManagerStateChanged() {
  if (!active_advertisement_) {
    return;
  }

  if (active_advertisement_->IsAdvertising()) {
    StartAdvertising();
  }
}

void BluetoothLowEnergyAdvertisementManagerOhos::StartAdvertising() {
  std::function<void(uint8_t)> get_advertising_id =
      [=](uint8_t advertising_id) { advertising_id_ = advertising_id; };
  BluetoothLowEnergyAdapter::StartAdvertising(advertisement_param_,
                                              get_advertising_id);
  active_advertisement_->OnAdvertisementPending();
}

void BluetoothLowEnergyAdvertisementManagerOhos::RegisterAdvertisement(
    std::unique_ptr<BluetoothAdvertisement::Data> advertisement_data,
    BluetoothAdapter::CreateAdvertisementCallback callback,
    BluetoothAdapter::AdvertisementErrorCallback error_callback) {
  std::optional<BluetoothAdvertisement::ErrorCode> error_code;

  const auto& service_uuids = advertisement_data->service_uuids();
  if (!service_uuids || advertisement_data->manufacturer_data() ||
      advertisement_data->solicit_uuids() ||
      advertisement_data->service_data()) {
    DVLOG(1) << "Ohos only supports advertising service UUIDs.";
    error_code = BluetoothAdvertisement::ERROR_UNSUPPORTED_PLATFORM;
  }

  if (active_advertisement_ && active_advertisement_->Getstatus() !=
                                   BluetoothAdvertisementOhos::UNREGISTERED) {
    DVLOG(1) << "Only one active BLE advertisement is currently supported.";
    error_code = BluetoothAdvertisement::ERROR_ADVERTISEMENT_ALREADY_EXISTS;
  }

  if (error_code) {
    ui_task_runner_->PostTask(
        FROM_HERE, base::BindOnce(std::move(error_callback), *error_code));
    return;
  }

  advertisement_param_.connectable =
      advertisement_data->type() ==
      BluetoothAdvertisement::AdvertisementType::ADVERTISEMENT_TYPE_PERIPHERAL;
  advertisement_param_.service_uuids = service_uuids.value();
  advertisement_param_.manufacturer_data =
      advertisement_data->manufacturer_data().value();
  advertisement_param_.service_data =
      advertisement_data->service_data().value();
  advertisement_param_.scan_response_data =
      advertisement_data->scan_response_data().value();

  active_advertisement_ = base::MakeRefCounted<BluetoothAdvertisementOhos>(
      std::move(service_uuids), std::move(callback), std::move(error_callback),
      this);
}

void BluetoothLowEnergyAdvertisementManagerOhos::UnregisterAdvertisement(
    BluetoothAdvertisementOhos* advertisement,
    BluetoothAdvertisement::SuccessCallback success_callback,
    BluetoothAdvertisement::ErrorCallback error_callback) {
  if (advertisement != active_advertisement_.get()) {
    DVLOG(1) << "Cannot unregister none-active advertisement.";
    ui_task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(std::move(error_callback),
                       BluetoothAdvertisement::ERROR_RESET_ADVERTISING));
    return;
  }

  active_advertisement_ = nullptr;
  BluetoothLowEnergyAdapter::StopAdvertising(advertising_id_);
  BluetoothLowEnergyAdapter::OffAdvertisingStateChange();
  ui_task_runner_->PostTask(FROM_HERE, std::move(success_callback));
}

}  // namespace device
