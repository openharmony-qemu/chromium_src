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

#include "device/bluetooth/ohos/bluetooth_remote_gatt_characteristic_ohos.h"

#include "base/containers/to_vector.h"
#include "base/logging.h"
#include "base/ohos/task_scheduler/task_runner_ohos.h"
#include "device/bluetooth/ohos/bluetooth_adapter_types.h"
#include "device/bluetooth/ohos/bluetooth_remote_gatt_descriptor_ohos.h"

using namespace ohos::adapter::device;

namespace device {

std::unique_ptr<BluetoothRemoteGattCharacteristicOHOS>
BluetoothRemoteGattCharacteristicOHOS::Create(
    BluetoothRemoteGattService* service,
    const std::string device_id,
    BluetoothUUID uuid,
    Properties properties,
    const std::string& identifier) {
  return base::WrapUnique(new BluetoothRemoteGattCharacteristicOHOS(
      service, device_id, uuid, properties, identifier));
}

BluetoothRemoteGattCharacteristicOHOS::
    ~BluetoothRemoteGattCharacteristicOHOS() {
  destructor_called_ = true;
  if (HasPendingRead()) {
    std::move(read_characteristic_value_callback_)
        .Run(BluetoothGattService::GattErrorCode::kFailed,
             std::vector<uint8_t>());
  }
  if (HasPendingWrite()) {
    std::move(write_characteristic_value_callbacks_.second)
        .Run(BluetoothGattService::GattErrorCode::kFailed);
  }
}

std::string BluetoothRemoteGattCharacteristicOHOS::GetIdentifier() const {
  return identifier_;
}

BluetoothUUID BluetoothRemoteGattCharacteristicOHOS::GetUUID() const {
  return uuid_;
}

BluetoothGattCharacteristic::Properties
BluetoothRemoteGattCharacteristicOHOS::GetProperties() const {
  return properties_;
}

BluetoothGattCharacteristic::Permissions
BluetoothRemoteGattCharacteristicOHOS::GetPermissions() const {
  // Not supported for remote characteristics for CoreBluetooth.
  NOTIMPLEMENTED();
  return PERMISSION_NONE;
}

const std::vector<uint8_t>& BluetoothRemoteGattCharacteristicOHOS::GetValue()
    const {
  return value_;
}

BluetoothRemoteGattService* BluetoothRemoteGattCharacteristicOHOS::GetService()
    const {
  return service_;
}

bool BluetoothRemoteGattCharacteristicOHOS::IsReadable() const {
  return GetProperties() & BluetoothGattCharacteristic::PROPERTY_READ;
}

void BluetoothRemoteGattCharacteristicOHOS::ProcessReadResult(
    int error_code, const std::vector<uint8_t>& value) {
  if (error_code < 0) {
    this->value_ = value;
    std::move(read_characteristic_value_callback_).Run(std::nullopt, value);
  } else {
    BluetoothGattService::GattErrorCode code =
        static_cast<BluetoothGattService::GattErrorCode>(error_code);
    this->value_ = value;
    std::move(read_characteristic_value_callback_).Run(code, value);
  }
}

void BluetoothRemoteGattCharacteristicOHOS::ReadRemoteCharacteristic(
    ValueCallback callback) {
  if (!IsReadable()) {
    base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(
        FROM_HERE,
        base::BindOnce(std::move(callback),
                       BluetoothGattService::GattErrorCode::kNotPermitted,
                       std::vector<uint8_t>()));
    return;
  }

  if (destructor_called_ || HasPendingRead() || HasPendingWrite()) {
    base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(
        FROM_HERE,
        base::BindOnce(std::move(callback),
                       BluetoothGattService::GattErrorCode::kInProgress,
                       std::vector<uint8_t>()));
    return;
  }

  read_characteristic_value_callback_ = std::move(callback);
  auto read_callback = [&](int error_code, const std::vector<uint8_t>& value) {
    base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(
        FROM_HERE,
        base::BindOnce(
            &BluetoothRemoteGattCharacteristicOHOS::ProcessReadResult,
            weak_ptr_factory_.GetWeakPtr(),
            error_code,
            std::move(value)));
  };
  BluetoothBleAttributeAdapter::ReadCharacteristic(identifier_,
                                                   device_id_,
                                                   std::move(read_callback));
}

void BluetoothRemoteGattCharacteristicOHOS::ProcessWriteResult(int error_code) {
  std::pair<base::OnceClosure, ErrorCallback> callbacks_write;
  callbacks_write.swap(write_characteristic_value_callbacks_);
  if (error_code < 0) {
    std::move(callbacks_write.first).Run();
  } else {
    BluetoothGattService::GattErrorCode code =
        static_cast<BluetoothGattService::GattErrorCode>(error_code);
    std::move(callbacks_write.second).Run(code);
  }
}

void BluetoothRemoteGattCharacteristicOHOS::WriteRemoteCharacteristic(
    base::span<const uint8_t> value,
    WriteType write_type,
    base::OnceClosure callback,
    ErrorCallback error_callback) {
  if (destructor_called_ || HasPendingRead() || HasPendingWrite()) {
    base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(
        FROM_HERE,
        base::BindOnce(std::move(error_callback),
                       BluetoothGattService::GattErrorCode::kInProgress));
    return;
  }
  write_characteristic_value_callbacks_ =
      std::make_pair(std::move(callback), std::move(error_callback));

  GattWriteType gatt_write_type;
  switch (write_type) {
    case WriteType::kWithResponse:
      gatt_write_type = GattWriteType::WriteWithResponse;
      break;
    case WriteType::kWithoutResponse:
      gatt_write_type = GattWriteType::WriteWithoutResponse;
      break;
  }
  auto write_characteristic_callback = [&](int error_code) {
    base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(
        FROM_HERE,
        base::BindOnce(
            &BluetoothRemoteGattCharacteristicOHOS::ProcessWriteResult,
            weak_ptr_factory_.GetWeakPtr(),
            error_code));
  };
  std::vector<uint8_t> value_vector = base::ToVector(value);
  BluetoothBleAttributeAdapter::WriteCharacteristic(
      identifier_,
      device_id_,
      static_cast<int>(gatt_write_type),
      value_vector,
      std::move(write_characteristic_callback));
}

void BluetoothRemoteGattCharacteristicOHOS::DeprecatedWriteRemoteCharacteristic(
    base::span<const uint8_t> value,
    base::OnceClosure callback,
    ErrorCallback error_callback) {
  if (destructor_called_ || HasPendingRead() || HasPendingWrite()) {
    base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(
        FROM_HERE,
        base::BindOnce(std::move(error_callback),
                       BluetoothGattService::GattErrorCode::kInProgress));
    return;
  }
  write_characteristic_value_callbacks_ =
      std::make_pair(std::move(callback), std::move(error_callback));

  auto deprecate_write_callback = [&](int error_code) {
    base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(
        FROM_HERE,
        base::BindOnce(
            &BluetoothRemoteGattCharacteristicOHOS::ProcessWriteResult,
            weak_ptr_factory_.GetWeakPtr(),
            error_code));
  };
  std::vector<uint8_t> value_vector = base::ToVector(value);
  BluetoothBleAttributeAdapter::DeprecatedWriteCharacteristic(
      identifier_,
      device_id_,
      value_vector,
      std::move(deprecate_write_callback));
}

void BluetoothRemoteGattCharacteristicOHOS::OnCreateDescriptor(
    const std::vector<DescriptorInfo> &descriptor_infos) {
  DescriptorMap descriptors;
  for (DescriptorInfo info : descriptor_infos) {
    auto descriptor = BluetoothRemoteGattDescriptorOHOS::Create(
        this, BluetoothUUID(info.descriptor_uuid), info.identifier,
        info.device_id);
    if (!descriptor) {
      continue;
    }
    std::string identifier = descriptor->GetIdentifier();
    auto iter = descriptors_.find(identifier);
    if (iter != descriptors_.end()) {
      descriptors.emplace(std::move(*iter));
    } else {
      descriptors.emplace(std::move(identifier), std::move(descriptor));
    }
  }
  std::swap(descriptors, descriptors_);
}

BluetoothRemoteGattCharacteristicOHOS::BluetoothRemoteGattCharacteristicOHOS(
    BluetoothRemoteGattService* service,
    const std::string device_id,
    BluetoothUUID uuid,
    Properties properties,
    const std::string identifier)
    : service_(service),
      device_id_(device_id),
      uuid_(std::move(uuid)),
      properties_(properties),
      identifier_(identifier) {
  auto callback = [this](std::vector<DescriptorInfo> descriptor_infos) {
    base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(
        FROM_HERE,
        base::BindOnce(
            &BluetoothRemoteGattCharacteristicOHOS::OnCreateDescriptor,
            weak_ptr_factory_.GetWeakPtr(),
            std::move(descriptor_infos)));
  };
  BluetoothBleAttributeAdapter::CreateDescriptor(identifier_,
                                                 device_id_,
                                                 std::move(callback));
}

void BluetoothRemoteGattCharacteristicOHOS::SetValue(
    const std::vector<uint8_t> &value) {
  value_ = value;
}

void BluetoothRemoteGattCharacteristicOHOS::SubscribeToNotifications(
    BluetoothRemoteGattDescriptor* ccc_descriptor,
    base::OnceClosure callback,
    ErrorCallback error_callback) {
  auto onChangeCallback = [this](int error_code,
                                 const std::vector<uint8_t>& value) {
    base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(
        FROM_HERE,
        base::BindOnce(
            &BluetoothRemoteGattCharacteristicOHOS::SetValue,
            weak_ptr_factory_.GetWeakPtr(),
            std::move(value)));
  };
  BluetoothBleAttributeAdapter::SubscribeToNotifications(
      identifier_,
      device_id_,
      std::move(onChangeCallback));
}

void BluetoothRemoteGattCharacteristicOHOS::UnsubscribeFromNotifications(
    BluetoothRemoteGattDescriptor* ccc_descriptor,
    base::OnceClosure callback,
    ErrorCallback error_callback) {
  auto UnsubscribeCallback = [](int error_code) {};
  BluetoothBleAttributeAdapter::UnsubscribeFromNotifications(
      identifier_,
      device_id_,
      std::move(UnsubscribeCallback));
}
}  // namespace device
