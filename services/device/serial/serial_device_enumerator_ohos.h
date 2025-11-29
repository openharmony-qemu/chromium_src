// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_DEVICE_SERIAL_SERIAL_DEVICE_ENUMERATOR_OHOS_H_
#define SERVICES_DEVICE_SERIAL_SERIAL_DEVICE_ENUMERATOR_OHOS_H_

#include "services/device/serial/serial_device_enumerator.h"

namespace device {

// Discovers and enumerates serial devices available to the host.
class SerialDeviceEnumeratorOhos : public SerialDeviceEnumerator {
 public:
  static std::unique_ptr<SerialDeviceEnumeratorOhos> Create();

  explicit SerialDeviceEnumeratorOhos(
      const base::FilePath& tty_driver_info_path);

  SerialDeviceEnumeratorOhos(const SerialDeviceEnumeratorOhos&) = delete;
  SerialDeviceEnumeratorOhos& operator=(const SerialDeviceEnumeratorOhos&) =
      delete;

  ~SerialDeviceEnumeratorOhos() override;

 private:
  const base::FilePath tty_driver_info_path_;
};

}  // namespace device

#endif  // SERVICES_DEVICE_SERIAL_SERIAL_DEVICE_ENUMERATOR_OHOS_H_
