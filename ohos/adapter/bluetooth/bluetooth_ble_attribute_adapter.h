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

#ifndef OHOS_ADAPTER_BLUETOOTH_BLUETOOTH_BLE_ATTRIBUTE_ADAPTER_H_
#define OHOS_ADAPTER_BLUETOOTH_BLUETOOTH_BLE_ATTRIBUTE_ADAPTER_H_

#include <functional>
#include <string>
#include <vector>

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/export.h"

namespace ohos::adapter::device {

struct ServiceInfo {
  std::string identifier;
  std::string device_id;
  std::string service_uuid;
  bool is_primary;
};
struct DescriptorInfo {
  std::string identifier;
  std::string device_id;
  std::string service_uuid;
  std::string characteristic_uuid;
  std::string descriptor_uuid;
};

struct CharacteristicInfo {
  std::string identifier;
  std::string device_id;
  std::string service_uuid;
  std::string characteristic_uuid;
  int32_t properties;
};

class ADAPTER_EXPORT_API BluetoothBleAttributeAdapter {
 public:
  using CreateServicesCallback =
      std::function<void(const std::vector<ServiceInfo>&)>;
  using CreateCharacteristicCallback =
      std::function<void(const std::vector<CharacteristicInfo>&)>;
  using CreateDescriptorCallback =
      std::function<void(const std::vector<DescriptorInfo>&)>;
  using ReadValueCallback =
      std::function<void(int error_code, const std::vector<uint8_t>& value)>;
  using WriteCallback = std::function<void(int error_code)>;
  using OnValueChangeCallback =
      std::function<void(int error_code, const std::vector<uint8_t>& value)>;
  using OnUnsubscribeCallback = std::function<void(int error_code)>;

  static void StartGattDiscovery(const std::string& device_id,
                                 CreateServicesCallback callback);

  static void CreateCharacteristic(const std::string& service_id,
                                   const std::string& device_id,
                                   CreateCharacteristicCallback callback);

  static void CreateDescriptor(const std::string& characteristic_id,
                               const std::string& device_id,
                               CreateDescriptorCallback callback);

  static void ReadDescriptor(const std::string& descriptor_id,
                             const std::string& device_id,
                             ReadValueCallback callback);

  static void ReadCharacteristic(const std::string& characteristic_id,
                                 const std::string& device_id,
                                 ReadValueCallback callback);

  static void WriteDescriptor(const std::string& descriptor_id,
                              const std::string& device_id,
                              const std::vector<uint8_t>& value,
                              WriteCallback callback);

  static void DeprecatedWriteCharacteristic(const std::string& characteristic_id,
                                            const std::string& device_id,
                                            const std::vector<uint8_t>& value,
                                            WriteCallback callback);

  static void WriteCharacteristic(const std::string& characteristic_id,
                                  const std::string& device_id,
                                  const int write_type,
                                  const std::vector<uint8_t>& value,
                                  WriteCallback callback);

  static void UnsubscribeFromNotifications(const std::string& characteristic_id,
                                           const std::string& device_id,
                                           OnUnsubscribeCallback callback);

  static void SubscribeToNotifications(const std::string& characteristic_id,
                                       const std::string& device_id,
                                       OnValueChangeCallback callback);

 private:
  BluetoothBleAttributeAdapter() = default;
  virtual ~BluetoothBleAttributeAdapter() = default;
};

}  // namespace ohos::adapter::device
#endif  // OHOS_ADAPTER_BLUETOOTH_BLUETOOTH_BLE_ATTRIBUTE_ADAPTER_H_
