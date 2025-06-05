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
