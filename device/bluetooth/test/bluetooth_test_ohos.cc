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
 
#include "device/bluetooth/test/bluetooth_test_ohos.h"
#include "device/bluetooth/ohos/bluetooth_adapter_ohos.h"
 
namespace device {
 
class BluetoothAdapterOhosTest final : public BluetoothAdapterOhos {
 public:
  BluetoothAdapterOhosTest(
      scoped_refptr<base::SingleThreadTaskRunner> ui_task_runner)
      : BluetoothAdapterOhos() {
    ui_task_runner_ = ui_task_runner;
  }
  ~BluetoothAdapterOhosTest() override {}
  bool IsPowered() const override { return is_powered_; }
 
  void SetPowered(bool powered,
                  base::OnceClosure callback,
                  ErrorCallback error_callback) override {
    is_powered_ = powered;
    NotifyAdapterPoweredChanged(is_powered_);
    std::move(callback).Run();
  }
 
 private:
  bool is_powered_{false};
};
 
BluetoothTestOhos::BluetoothTestOhos() = default;
 
BluetoothTestOhos::~BluetoothTestOhos() = default;
 
bool BluetoothTestOhos::PlatformSupportsLowEnergy() {
  return true;
}
 
void BluetoothTestOhos::InitWithFakeAdapter() {
  auto adapter = base::WrapRefCounted(new BluetoothAdapterOhosTest(
      task_environment_.GetMainThreadTaskRunner()));
  adapter_ = std::move(adapter);
  adapter_->SetPowered(true, base::DoNothing(), base::DoNothing());
}
 
BluetoothDevice* BluetoothTestOhos::SimulateLowEnergyDevice(
    int device_ordinal) {
  LowEnergyDeviceData data = GetLowEnergyDeviceData(device_ordinal);
  ohos::adapter::device::ScanResult scan_result;
  scan_result.device_name = data.name.value();
  scan_result.device_id = data.address;
  scan_result.rssi = data.rssi;
  scan_result.data = nullptr;
  scan_result.data_length = 0;
  scan_result.connectable = false;
  static_cast<BluetoothLowEnergyAdapterOhos*>(adapter_.get())
      ->LowEnergyDeviceUpdated(scan_result);
  BluetoothDevice* device_ptr = adapter_->GetDevice(data.address);
  device_ptr->UpdateAdvertisementData(
      data.rssi, data.flags, data.advertised_uuids, data.tx_power,
      data.service_data, data.manufacturer_data);
  return device_ptr;
}
 
}  // namespace device
