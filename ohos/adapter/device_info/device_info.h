// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/export.h"

#include <string>

namespace ohos::adapter::device_info {

const std::string kManufacture = "manufacture";
const std::string kProductModel = "productModel";
const std::string kDistributionOSName = "distributionOSName";
const std::string kDistributionOSVersion = "distributionOSVersion";
const std::string kDeviceType = "deviceType";

class ADAPTER_EXPORT_API DeviceInfo {
 public:
  static std::string Get(const std::string &property);
};
}  // namespace ohos::adapter::device_info
