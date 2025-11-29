// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_DEVICE_USB_USB_DEVICE_OHOS_H_
#define SERVICES_DEVICE_USB_USB_DEVICE_OHOS_H_

#include "ohos/adapter/device/device_adapter.h"
#include "services/device/usb/usb_device.h"
#include "services/device/usb/usb_device_handle_usbfs.h"

namespace base {
class SequencedTaskRunner;
}

namespace device {
struct UsbDeviceDescriptor;
class UsbDeviceOhos : public UsbDevice {
 public:
  UsbDeviceOhos(const UsbDeviceOhos&) = delete;
  UsbDeviceOhos& operator=(const UsbDeviceOhos&) = delete;

  void Open(OpenCallback callback) override;
  const std::string& device_id() const { return usb_device_info_.name; }

 protected:
  friend class UsbServiceOhos;

  UsbDeviceOhos(const USBDevice& usb_device_info,
                mojom::UsbDeviceInfoPtr device_info);
  ~UsbDeviceOhos() override;

 private:
  void OpenOnBlockingThread(
      OpenCallback callback,
      scoped_refptr<base::SequencedTaskRunner> task_runner,
      scoped_refptr<base::SequencedTaskRunner> blocking_task_runner);

  void Opened(base::ScopedFD fd,
              base::ScopedFD lifeline_fd,
              const std::string& client_id,
              OpenCallback callback,
              scoped_refptr<base::SequencedTaskRunner> blocking_task_runner);

  USBDevice usb_device_info_;
  bool is_has_right_;
};

}  // namespace device

#endif  // SERVICES_DEVICE_USB_USB_DEVICE_OHOS_H_
