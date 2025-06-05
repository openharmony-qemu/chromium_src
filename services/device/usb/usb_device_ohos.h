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
