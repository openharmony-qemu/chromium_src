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

#include "device/bluetooth/ohos/bluetooth_remote_gatt_descriptor_ohos.h"

#include <utility>

#include "base/containers/to_vector.h"
#include "base/memory/ptr_util.h"
#include "base/ohos/task_scheduler/task_runner_ohos.h"
#include "device/bluetooth/ohos/bluetooth_adapter_types.h"

using namespace ohos::adapter::device;

namespace device {
std::unique_ptr<BluetoothRemoteGattDescriptorOHOS>
BluetoothRemoteGattDescriptorOHOS::Create(
    BluetoothRemoteGattCharacteristic* characteristic,
    BluetoothUUID uuid,
    const std::string& identifier,
    const std::string& device_id) {
  return base::WrapUnique(new BluetoothRemoteGattDescriptorOHOS(
      characteristic, uuid, identifier, device_id));
}

BluetoothRemoteGattDescriptorOHOS::~BluetoothRemoteGattDescriptorOHOS() {
  destructor_called_ = true;
  if (HasPendingRead()) {
    std::move(read_value_callback_)
        .Run(BluetoothGattService::GattErrorCode::kFailed,
             std::vector<uint8_t>());
  }
  if (HasPendingWrite()) {
    std::move(write_value_callbacks_)
        .second.Run(BluetoothGattService::GattErrorCode::kFailed);
  }
}

std::string BluetoothRemoteGattDescriptorOHOS::GetIdentifier() const {
  return identifier_;
}

BluetoothUUID BluetoothRemoteGattDescriptorOHOS::GetUUID() const {
  return uuid_;
}

BluetoothGattCharacteristic::Permissions
BluetoothRemoteGattDescriptorOHOS::GetPermissions() const {
  NOTIMPLEMENTED();
  return BluetoothGattCharacteristic::Permissions();
}

const std::vector<uint8_t>& BluetoothRemoteGattDescriptorOHOS::GetValue()
    const {
  return value_;
}

BluetoothRemoteGattCharacteristic*
BluetoothRemoteGattDescriptorOHOS::GetCharacteristic() const {
  return characteristic_;
}

void BluetoothRemoteGattDescriptorOHOS::ProcessReadResult(
    int error_code, const std::vector<uint8_t>& value) {
  if (error_code < 0) {
    this->value_ = value;
    std::move(read_value_callback_).Run(std::nullopt, value);
  } else {
    BluetoothGattService::GattErrorCode code =
        static_cast<BluetoothGattService::GattErrorCode>(error_code);
    std::move(read_value_callback_).Run(code, value);
  }
}

void BluetoothRemoteGattDescriptorOHOS::ReadRemoteDescriptor(
    ValueCallback callback) {
  if (destructor_called_ || HasPendingRead() || HasPendingWrite()) {
    base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(
        FROM_HERE,
        base::BindOnce(std::move(callback),
                       BluetoothGattService::GattErrorCode::kInProgress,
                       std::vector<uint8_t>()));
    return;
  }
  read_value_callback_ = std::move(callback);
  auto read_callback = [&](int error_code, const std::vector<uint8_t>& value) {
    base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(
        FROM_HERE,
        base::BindOnce(&BluetoothRemoteGattDescriptorOHOS::ProcessReadResult,
                       weak_ptr_factory_.GetWeakPtr(),
                       error_code,
                       std::move(value)));
  };
  BluetoothBleAttributeAdapter::ReadDescriptor(identifier_,
                                               device_id_,
                                               std::move(read_callback));
}

void BluetoothRemoteGattDescriptorOHOS::ProcessWriteResult(int error_code) {
  std::pair<base::OnceClosure, ErrorCallback> callbacks;
  callbacks.swap(write_value_callbacks_);
  if (error_code < 0) {
    std::move(callbacks.first).Run();
  } else {
    BluetoothGattService::GattErrorCode code =
        static_cast<BluetoothGattService::GattErrorCode>(error_code);
    std::move(callbacks.second).Run(code);
  }
}

void BluetoothRemoteGattDescriptorOHOS::WriteRemoteDescriptor(
    base::span<const uint8_t> new_value,
    base::OnceClosure callback,
    ErrorCallback error_callback) {
  if (destructor_called_ || HasPendingRead() || HasPendingWrite()) {
    base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(
        FROM_HERE,
        base::BindOnce(std::move(error_callback),
                       BluetoothGattService::GattErrorCode::kInProgress));
    return;
  }

  write_value_callbacks_ =
      std::make_pair(std::move(callback), std::move(error_callback));
  auto write_descriptor_callback = [&](int error_code) {
    base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(
        FROM_HERE,
        base::BindOnce(&BluetoothRemoteGattDescriptorOHOS::ProcessWriteResult,
                       weak_ptr_factory_.GetWeakPtr(),
                       error_code));
  };
  std::vector<uint8_t> value_vector = base::ToVector(new_value);
  BluetoothBleAttributeAdapter::WriteDescriptor(
      identifier_,
      device_id_, value_vector,
      std::move(write_descriptor_callback));
}

BluetoothRemoteGattDescriptorOHOS::BluetoothRemoteGattDescriptorOHOS(
    BluetoothRemoteGattCharacteristic* characteristic,
    BluetoothUUID uuid,
    const std::string identifier,
    const std::string device_id)
    : characteristic_(characteristic),
      identifier_(identifier),
      uuid_(std::move(uuid)),
      device_id_(device_id) {}
}  // namespace device
