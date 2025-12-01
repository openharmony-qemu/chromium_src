// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "services/device/serial/serial_device_enumerator_ohos.h"

#include "base/files/file_util.h"

namespace device {

// static
std::unique_ptr<SerialDeviceEnumeratorOhos>
SerialDeviceEnumeratorOhos::Create() {
  return std::make_unique<SerialDeviceEnumeratorOhos>(
      base::FilePath("/proc/tty/drivers"));
}

SerialDeviceEnumeratorOhos::SerialDeviceEnumeratorOhos(
    const base::FilePath& tty_driver_info_path)
    : tty_driver_info_path_(tty_driver_info_path) {}

SerialDeviceEnumeratorOhos::~SerialDeviceEnumeratorOhos() {}

}  // namespace device
