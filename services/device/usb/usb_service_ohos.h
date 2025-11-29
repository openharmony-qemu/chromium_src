// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_DEVICE_USB_USB_SERVICE_OHOS_H_
#define SERVICES_DEVICE_USB_USB_SERVICE_OHOS_H_

#include <list>
#include <memory>
#include <unordered_map>

#include "base/memory/raw_ptr_exclusion.h"
#include "base/memory/scoped_refptr.h"
#include "base/memory/weak_ptr.h"
#include "base/threading/sequence_bound.h"
#include "ohos/adapter/device/device_adapter.h"
#include "services/device/public/mojom/usb_device.mojom.h"
#include "services/device/usb/usb_service.h"

namespace device {

struct UsbDeviceDescriptor;
class UsbDeviceOhos;

class UsbServiceOhos final : public UsbService {
 public:
  UsbServiceOhos();
  UsbServiceOhos(const UsbServiceOhos&) = delete;
  UsbServiceOhos& operator=(const UsbServiceOhos&) = delete;

  ~UsbServiceOhos() override;
  void GetDevices(GetDevicesCallback callback) override;

 private:
  using DeviceMap =
      std::unordered_map<std::string, scoped_refptr<UsbDeviceOhos>>;
  class BlockingTaskRunnerHelper;

  void OnDeviceAdded(const USBDevice& device,
                     mojom::UsbDeviceInfoPtr device_info);
  void DeviceReady(scoped_refptr<UsbDeviceOhos> device);
  void OnDeviceRemoved(const std::string& device_id);
  void HelperStarted();
  bool EnumerationReady() {
    return helper_started_ && first_enumeration_countdown_ == 0;
  }

  // |helper_started_| is set once OnDeviceAdded has been called for all devices
  // initially found on the system. |first_enumeration_countdown_| is then
  // decremented as DeviceReady is called for these devices.
  // |enumeration_callbacks_| holds the callbacks passed to GetDevices before
  // this process completes and the device list is ready.
  bool helper_started_ = false;
  uint32_t first_enumeration_countdown_ = 0;
  std::list<GetDevicesCallback> enumeration_callbacks_;

  base::SequenceBound<BlockingTaskRunnerHelper> helper_;
  DeviceMap device_map_;
  base::WeakPtrFactory<UsbServiceOhos> weak_factory_{this};
};

}  // namespace device

#endif  // SERVICES_DEVICE_USB_USB_SERVICE_OHOS_H_
