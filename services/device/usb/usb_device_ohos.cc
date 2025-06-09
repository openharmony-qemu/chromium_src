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

#include "services/device/usb/usb_device_ohos.h"

#include "base/files/file_util.h"
#include "base/functional/bind.h"
#include "base/functional/callback_helpers.h"
#include "base/logging.h"
#include "base/task/single_thread_task_runner.h"
#include "ohos/adapter/device/device_adapter.h"
#include "services/device/public/mojom/usb_device.mojom.h"
#include "services/device/usb/usb_descriptors.h"
#include "services/device/usb/usb_device.h"
#include "services/device/usb/usb_service.h"

namespace device {

const uint8_t K_DEVICE_CLASS_HUB = 0x09;

UsbDeviceOhos::UsbDeviceOhos(const USBDevice& usb_device_info,
                             mojom::UsbDeviceInfoPtr device_info)
    : UsbDevice(std::move(device_info)), usb_device_info_(usb_device_info) {
  is_has_right_ = DeviceAdapter::HasRight(device_id());
}

UsbDeviceOhos::~UsbDeviceOhos() {
  if (is_has_right_) {
    DeviceAdapter::RemoveRight(device_id());
    is_has_right_ = false;
  }
}

void UsbDeviceOhos::Open(OpenCallback callback) {
  if (!is_has_right_) {
    is_has_right_ = DeviceAdapter::RequestRight(device_id());
  }
  if (is_has_right_) {
    scoped_refptr<base::SequencedTaskRunner> blocking_task_runner =
        UsbService::CreateBlockingTaskRunner();
    blocking_task_runner->PostTask(
        FROM_HERE,
        base::BindOnce(&UsbDeviceOhos::OpenOnBlockingThread, this,
                       std::move(callback),
                       base::SingleThreadTaskRunner::GetCurrentDefault(),
                       blocking_task_runner));
  } else {
    std::move(callback).Run(nullptr);
  }
}

void UsbDeviceOhos::OpenOnBlockingThread(
    OpenCallback callback,
    scoped_refptr<base::SequencedTaskRunner> task_runner,
    scoped_refptr<base::SequencedTaskRunner> blocking_task_runner) {
  ohos::adapter::device::USBDevicePipe pipe;
  DeviceAdapter::OpenDevice(usb_device_info_, pipe);
  std::vector<uint8_t> descriptor_vector =
      DeviceAdapter::GetRawDescriptor(pipe);
  std::unique_ptr<UsbDeviceDescriptor> descriptor =
      std::make_unique<UsbDeviceDescriptor>();
  if (!descriptor->Parse(base::make_span(descriptor_vector.begin(),
                                         descriptor_vector.end()))) {
    std::move(callback).Run(nullptr);
    return;
  }

  if (descriptor->device_info->class_code == K_DEVICE_CLASS_HUB) {
    // Don't try to enumerate hubs. We never want to connect to a hub.
    std::move(callback).Run(nullptr);
    return;
  }

  descriptor->device_info->manufacturer_name = device_info_->manufacturer_name;
  descriptor->device_info->product_name = device_info_->product_name;
  descriptor->device_info->serial_number = device_info_->serial_number;
  descriptor->device_info->bus_number = device_info_->bus_number;
  descriptor->device_info->port_number = device_info_->port_number;
  device_info_ = std::move(descriptor->device_info);

  int32_t file_descriptor = DeviceAdapter::GetFileDescriptor(pipe);
  if (usb_device_info_.configs.size() > 0 &&
      usb_device_info_.configs[0].interfaces.size() > 0) {
    DeviceAdapter::ReleaseInterface(pipe,
                                    usb_device_info_.configs[0].interfaces[0]);
  }
  base::ScopedFD fd(HANDLE_EINTR(file_descriptor));
  std::string empty_client_id = "";
  if (fd.is_valid()) {
    task_runner->PostTask(
        FROM_HERE, base::BindOnce(&UsbDeviceOhos::Opened, this, std::move(fd),
                                  base::ScopedFD(), empty_client_id,
                                  std::move(callback), blocking_task_runner));
  } else {
    LOG(WARNING) << "Failed to open " << device_id();
    task_runner->PostTask(FROM_HERE,
                          base::BindOnce(std::move(callback), nullptr));
  }
}

void UsbDeviceOhos::Opened(
    base::ScopedFD fd,
    base::ScopedFD lifeline_fd,
    const std::string& client_id,
    OpenCallback callback,
    scoped_refptr<base::SequencedTaskRunner> blocking_task_runner) {
  scoped_refptr<UsbDeviceHandle> device_handle =
      new UsbDeviceHandleUsbfs(this, std::move(fd), std::move(lifeline_fd),
                               client_id, blocking_task_runner);
  handles().push_back(device_handle.get());
  std::move(callback).Run(device_handle);
}

}  // namespace device
