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

#ifndef OHOS_ADAPTER_DEVICE_DEVICE_ADAPTER_H_
#define OHOS_ADAPTER_DEVICE_DEVICE_ADAPTER_H_

#include <functional>
#include <string>
#include <vector>
#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/export.h"

namespace ohos::adapter::device {
struct USBDevice;
enum USBRequestDirection {
  USB_REQUEST_DIR_TO_DEVICE = 0,
  USB_REQUEST_DIR_FROM_DEVICE = 0x80,
};

struct USBEndpoint {
  int32_t address;
  int32_t attributes;
  int32_t interval;
  int32_t maxPacketSize;
  USBRequestDirection direction;
  int32_t number;
  int32_t type;
  int32_t interfaceId;
};

struct USBInterface {
  int32_t id;
  int32_t protocol;
  int32_t clazz;
  int32_t subClass;
  int32_t alternateSetting;
  std::string name;
  std::vector<USBEndpoint> endpoints;
};

struct USBConfiguration {
  int32_t id;
  int32_t attributes;
  int32_t maxPower;
  std::string name;
  bool isRemoteWakeup;
  bool isSelfPowered;
  std::vector<USBInterface> interfaces;
};

struct USBDevice {
  int32_t busNum = 0;
  int32_t devAddress = 0;
  std::string serial;
  std::string name;
  std::string manufacturerName;
  std::string productName;
  std::string version;
  int32_t vendorId = 0;
  int32_t productId = 0;
  int32_t clazz = 0;
  int32_t subClass = 0;
  int32_t protocol = 0;
  std::vector<USBConfiguration> configs;
};

struct USBDevicePipe {
  int32_t busNum = -1;
  int32_t devAddress = -1;
};

enum class EventType : uint8_t {
  DEVICE_ATTATCHED = 1,
  DEVICE_DETATCHED,
  OTHRE
};

class ADAPTER_EXPORT_API HotplugCallback
    : public std::enable_shared_from_this<HotplugCallback> {
 public:
  HotplugCallback() = default;
  virtual ~HotplugCallback() = default;
  void ParseEvent(int32_t event_type, aki::Value event_data, int32_t len);

  virtual void OnDeviceAttatched(USBDevice& device) = 0;
  virtual void OnDeviceRemoved(USBDevice& device) = 0;
};

class ADAPTER_EXPORT_API DeviceAdapter {
 public:
  DeviceAdapter() = default;
  virtual ~DeviceAdapter() = default;

  bool GetDevices(std::vector<USBDevice>& device_list) const;
  void RegisterHotplugCallback(HotplugCallback* call_back) const;
  void UnregisterHotplugCallback() const;
  static bool HasRight(const std::string& device_name);
  static bool OpenDevice(const USBDevice& device, USBDevicePipe& pipe);
  static std::vector<uint8_t> GetRawDescriptor(const USBDevicePipe& pipe);
  static int32_t GetFileDescriptor(const USBDevicePipe& pipe);
  static bool RemoveRight(const std::string& device_name);
  static int32_t ReleaseInterface(const USBDevicePipe& pipe,
                                  const USBInterface& iface);
  static int32_t ClosePipe(const USBDevicePipe& pipe);
  static bool RequestRight(const std::string& device_name);
};

}  // namespace ohos::adapter::device

using USBDevice = ohos::adapter::device::USBDevice;
using DeviceAdapter = ohos::adapter::device::DeviceAdapter;

#endif  // OHOS_ADAPTER_DEVICE_DEVICE_ADAPTER_H_
