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

#include "services/device/usb/usb_service_ohos.h"

#include <string>

#include "base/containers/contains.h"
#include "base/functional/bind.h"
#include "base/functional/callback_helpers.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/memory/ptr_util.h"
#include "base/memory/weak_ptr.h"
#include "base/sequence_checker.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/thread_pool.h"
#include "base/threading/scoped_blocking_call.h"
#include "device/device_watcher/device_watcher.h"
#include "ohos/adapter/device/device_adapter.h"
#include "services/device/public/mojom/usb_device.mojom.h"
#include "services/device/usb/usb_device_handle.h"
#include "services/device/usb/usb_device_ohos.h"
#include "services/device/usb/webusb_descriptors.h"

using namespace ohos::adapter::device;
namespace device {

const uint8_t K_DEVICE_CLASS_HUB = 0x09;

const int OFF_SET_MAJOR = 8;
const int OFF_SET_MINOR = 4;

mojom::UsbAlternateInterfaceInfoPtr AddEndpoints(
    mojom::UsbAlternateInterfaceInfoPtr alt_interface_info,
    const USBInterface& interface) {
  for (const auto& endpoint : interface.endpoints) {
    mojom::UsbEndpointInfoPtr endpoint_info = mojom::UsbEndpointInfo::New();
    endpoint_info->endpoint_number = endpoint.number;
    endpoint_info->packet_size = endpoint.maxPacketSize;
    endpoint_info->polling_interval = endpoint.interval;

    alt_interface_info->endpoints.push_back(std::move(endpoint_info));
  }
  return alt_interface_info;
}

mojom::UsbConfigurationInfoPtr AddInterfaces(
    mojom::UsbConfigurationInfoPtr config_info,
    const ohos::adapter::device::USBConfiguration& config) {
  for (const auto& interface : config.interfaces) {
    mojom::UsbInterfaceInfoPtr interface_info = mojom::UsbInterfaceInfo::New();
    mojom::UsbAlternateInterfaceInfoPtr alt_interface_info =
        mojom::UsbAlternateInterfaceInfo::New();
    alt_interface_info->protocol_code = interface.protocol;
    alt_interface_info->class_code = interface.clazz;
    alt_interface_info->subclass_code = interface.subClass;
    alt_interface_info->alternate_setting = interface.alternateSetting;
    alt_interface_info->interface_name = base::UTF8ToUTF16(interface.name);
    // Add endpoints
    alt_interface_info = AddEndpoints(std::move(alt_interface_info), interface);

    interface_info->alternates.push_back(std::move(alt_interface_info));
    config_info->interfaces.push_back(std::move(interface_info));
  }
  return config_info;
}

mojom::UsbDeviceInfoPtr GetDeviceInfo(const USBDevice& device) {
  mojom::UsbDeviceInfoPtr usb_info = mojom::UsbDeviceInfo::New();
  usb_info->bus_number = device.busNum;
  usb_info->port_number = device.devAddress;
  if (!device.version.empty()) {
    usb_info->usb_version_major = std::stoi(device.version) >> OFF_SET_MAJOR;
    usb_info->usb_version_minor =
        (std::stoi(device.version) >> OFF_SET_MINOR) & 0xf;
    usb_info->usb_version_subminor = std::stoi(device.version) & 0xf;
    usb_info->device_version_major = std::stoi(device.version) >> OFF_SET_MAJOR;
    usb_info->device_version_minor =
        (std::stoi(device.version) >> OFF_SET_MINOR) & 0xf;
    usb_info->device_version_subminor = std::stoi(device.version) & 0xf;
  }
  usb_info->serial_number = base::UTF8ToUTF16(device.serial);
  usb_info->manufacturer_name = base::UTF8ToUTF16(device.manufacturerName);
  usb_info->product_name = base::UTF8ToUTF16(device.productName);
  usb_info->vendor_id = device.vendorId;
  usb_info->product_id = device.productId;
  usb_info->class_code = device.clazz;
  usb_info->subclass_code = device.subClass;
  usb_info->protocol_code = device.protocol;
  // Add configurations
  for (const auto& config : device.configs) {
    mojom::UsbConfigurationInfoPtr config_info =
        mojom::UsbConfigurationInfo::New();
    config_info->configuration_value = config.id;
    config_info->configuration_name = base::UTF8ToUTF16(config.name);
    config_info->maximum_power = config.maxPower;
    config_info->remote_wakeup = config.isRemoteWakeup;
    config_info->self_powered = config.isSelfPowered;
    // Add interfaces
    config_info = AddInterfaces(std::move(config_info), config);

    usb_info->configurations.push_back(std::move(config_info));
  }
  return usb_info;
}

bool FilterUSBDevice(const USBDevice& device) {
  if (device.clazz == K_DEVICE_CLASS_HUB) {
    return false;
  }
  return true;
}

class UsbServiceOhos::BlockingTaskRunnerHelper
    : public DeviceWatcher::Observer {
 public:
  explicit BlockingTaskRunnerHelper(
      base::WeakPtr<UsbServiceOhos> service,
      scoped_refptr<base::SequencedTaskRunner> task_runner);

  BlockingTaskRunnerHelper(const BlockingTaskRunnerHelper&) = delete;
  BlockingTaskRunnerHelper& operator=(const BlockingTaskRunnerHelper&) = delete;

  ~BlockingTaskRunnerHelper() override;

 private:
  void OnDeviceAdded(const USBDevice& device) override;
  void OnDeviceRemoved(const USBDevice& device) override;
  void OnDeviceChanged(const USBDevice& device) override;

  std::unique_ptr<DeviceWatcher> watcher_;

  // This weak pointer is only valid when checked on this task runner.
  base::WeakPtr<UsbServiceOhos> service_;
  scoped_refptr<base::SequencedTaskRunner> task_runner_;
  SEQUENCE_CHECKER(sequence_checker_);
};

UsbServiceOhos::BlockingTaskRunnerHelper::BlockingTaskRunnerHelper(
    base::WeakPtr<UsbServiceOhos> service,
    scoped_refptr<base::SequencedTaskRunner> task_runner)
    : service_(std::move(service)), task_runner_(std::move(task_runner)) {
  LOG(ERROR) << "[usb] BlockingTaskRunnerHelper";
  watcher_ = DeviceWatcher::StartWatching(this, FilterUSBDevice);
  if (watcher_) {
    watcher_->EnumerateExistingDevices();
  }

  task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&UsbServiceOhos::HelperStarted, service_));
}

UsbServiceOhos::BlockingTaskRunnerHelper::~BlockingTaskRunnerHelper() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
}

void UsbServiceOhos::BlockingTaskRunnerHelper::OnDeviceAdded(
    const USBDevice& device) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  base::ScopedBlockingCall scoped_blocking_call(FROM_HERE,
                                                base::BlockingType::MAY_BLOCK);
  if (device.name.empty()) {
    LOG(WARNING) << "receive a invalid device.";
    return;
  }
  mojom::UsbDeviceInfoPtr device_info = GetDeviceInfo(device);
  task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&UsbServiceOhos::OnDeviceAdded, service_,
                     device, std::move(device_info)));
}

void UsbServiceOhos::BlockingTaskRunnerHelper::OnDeviceRemoved(
    const USBDevice& device) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  base::ScopedBlockingCall scoped_blocking_call(FROM_HERE,
                                                base::BlockingType::MAY_BLOCK);
  if (!device.name.empty()) {
    task_runner_->PostTask(
        FROM_HERE, base::BindOnce(&UsbServiceOhos::OnDeviceRemoved, service_,
                                  std::string(device.name)));
  }
}

void UsbServiceOhos::BlockingTaskRunnerHelper::OnDeviceChanged(
    const USBDevice& device) {}

UsbServiceOhos::UsbServiceOhos() {
  helper_ = base::SequenceBound<BlockingTaskRunnerHelper>(
      CreateBlockingTaskRunner(), weak_factory_.GetWeakPtr(),
      base::SequencedTaskRunner::GetCurrentDefault());
}

UsbServiceOhos::~UsbServiceOhos() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  NotifyWillDestroyUsbService();
}

void UsbServiceOhos::GetDevices(GetDevicesCallback callback) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  if (EnumerationReady()) {
    UsbService::GetDevices(std::move(callback));
  } else {
    enumeration_callbacks_.push_back(std::move(callback));
  }
}

void UsbServiceOhos::OnDeviceAdded(const USBDevice& usb_device_info,
                                   mojom::UsbDeviceInfoPtr device_info) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);

  if (base::Contains(device_map_, usb_device_info.name)) {
    LOG(ERROR) << "Got duplicate add event for path: " << usb_device_info.name;
    return;
  }

  // Devices that appear during initial enumeration are gathered into the first
  // result returned by GetDevices() and prevent device add/remove notifications
  // from being sent.
  if (!EnumerationReady()) {
    ++first_enumeration_countdown_;
  }

  scoped_refptr<UsbDeviceOhos> device(
      new UsbDeviceOhos(usb_device_info, std::move(device_info)));
  device_map_[device->device_id()] = device;

  DeviceReady(device);
}

void UsbServiceOhos::DeviceReady(scoped_refptr<UsbDeviceOhos> device) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  bool enumeration_became_ready = false;
  if (!EnumerationReady()) {
    DCHECK_GT(first_enumeration_countdown_, 0u);
    first_enumeration_countdown_--;
    if (EnumerationReady()) {
      enumeration_became_ready = true;
    }
  }

  // If |device| was disconnected while descriptors were being read then it
  // will have been removed from |devices_by_path_|.
  bool device_added = base::Contains(device_map_, device->device_id());
  if (device_added) {
    DCHECK(!base::Contains(devices(), device->guid()));
    devices()[device->guid()] = device;
  }

  if (enumeration_became_ready) {
    std::vector<scoped_refptr<UsbDevice>> result;
    result.reserve(devices().size());
    for (const auto& map_entry : devices()) {
      result.push_back(map_entry.second);
    }
    for (auto& callback : enumeration_callbacks_) {
      std::move(callback).Run(result);
    }
    enumeration_callbacks_.clear();
  } else if (device_added && EnumerationReady()) {
    NotifyDeviceAdded(device);
  }
}

void UsbServiceOhos::OnDeviceRemoved(const std::string& device_id) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  auto by_id_it = device_map_.find(device_id);
  if (by_id_it == device_map_.end()) {
    return;
  }

  scoped_refptr<UsbDeviceOhos> device = by_id_it->second;
  device_map_.erase(by_id_it);
  device->OnDisconnect();

  auto by_guid_it = devices().find(device->guid());
  if (by_guid_it != devices().end() && EnumerationReady()) {
    devices().erase(by_guid_it);
    NotifyDeviceRemoved(device);
  }
}

void UsbServiceOhos::HelperStarted() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  helper_started_ = true;
  if (EnumerationReady()) {
    std::vector<scoped_refptr<UsbDevice>> result;
    result.reserve(devices().size());
    for (const auto& map_entry : devices()) {
      result.push_back(map_entry.second);
    }
    for (auto& callback : enumeration_callbacks_) {
      std::move(callback).Run(result);
    }
    enumeration_callbacks_.clear();
  }
}
}  // namespace device
