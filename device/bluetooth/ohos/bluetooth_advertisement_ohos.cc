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
