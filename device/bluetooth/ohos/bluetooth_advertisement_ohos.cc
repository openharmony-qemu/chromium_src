// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "device/bluetooth/ohos/bluetooth_advertisement_ohos.h"

#include "base/functional/bind.h"
#include "base/task/single_thread_task_runner.h"
#include "device/bluetooth/ohos/bluetooth_adapter_ohos.h"

namespace device {

BluetoothAdvertisementOhos::BluetoothAdvertisementOhos(
    std::optional<BluetoothAdvertisement::UUIDList> service_uuids,
    BluetoothAdapter::CreateAdvertisementCallback success_callback,
    BluetoothAdapter::AdvertisementErrorCallback error_callback,
    BluetoothLowEnergyAdvertisementManagerOhos* advertisement_manager)
    : service_uuids_(std::move(service_uuids)),
      success_callback_(std::move(success_callback)),
      error_callback_(std::move(error_callback)),
      advertisement_manager_(advertisement_manager),
      status_(BluetoothAdvertisementOhos::WAITING_FOR_ADAPTER) {}

void BluetoothAdvertisementOhos::Unregister(SuccessCallback success_callback,
                                            ErrorCallback error_callback) {
  if (status_ == Status::UNREGISTERED) {
    std::move(success_callback).Run();
    return;
  }

  status_ = Status::UNREGISTERED;
  advertisement_manager_->UnregisterAdvertisement(
      this, std::move(success_callback), std::move(error_callback));
}

BluetoothAdvertisementOhos::~BluetoothAdvertisementOhos() {}

void BluetoothAdvertisementOhos::OnAdvertisementPending() {
  status_ = Status::ADVERTISEMENT_PENDING;
}

void BluetoothAdvertisementOhos::OnAdvertisementError(
    base::SingleThreadTaskRunner* task_runner,
    BluetoothAdvertisement::ErrorCode error_code) {
  status_ = Status::ERROR_ADVERTISING;
  task_runner->PostTask(FROM_HERE,
                        base::BindOnce(std::move(error_callback_), error_code));
}

void BluetoothAdvertisementOhos::OnAdvertisementSuccess(
    base::SingleThreadTaskRunner* task_runner) {
  status_ = Status::ADVERTISING;
  task_runner->PostTask(
      FROM_HERE,
      base::BindOnce(&BluetoothAdvertisementOhos::InvokeSuccessCallback, this));
}

void BluetoothAdvertisementOhos::OnAdapterReset() {
  status_ = Status::UNREGISTERED;
}

void BluetoothAdvertisementOhos::InvokeSuccessCallback() {
  std::move(success_callback_).Run(this);
}

}  // namespace device
