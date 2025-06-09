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
// Simple implementation of BluetoothAdvertisement for Ohos. The primary logic is
// currently handled in BluetoothLowEnergyAdvertisementManagerOhos.
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
