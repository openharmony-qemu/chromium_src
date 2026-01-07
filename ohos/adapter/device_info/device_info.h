// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef OHOS_ADAPTER_DEVICE_INFO_DEVICE_INFO_H_
#define OHOS_ADAPTER_DEVICE_INFO_DEVICE_INFO_H_

#include "ohos/adapter/export.h"

#include <string>

namespace ohos::adapter::device_info {

const std::string kManufacture = "manufacture";
const std::string kProductModel = "productModel";
const std::string kDistributionOSName = "distributionOSName";
const std::string kDistributionOSVersion = "distributionOSVersion";
const std::string kDisplayVersion = "displayVersion";
const std::string kOsReleaseType = "osReleaseType";

enum SdkVersion {
  SDK_VERSION_12 = 12,
  SDK_VERSION_13 = 13,
  SDK_VERSION_14 = 14,
  SDK_VERSION_15 = 15,
  SDK_VERSION_19 = 19,
  SDK_VERSION_20 = 20,
  SDK_VERSION_21 = 21,
  SDK_VERSION_22 = 22,
};

enum class DeviceType {
  _2IN1,
  _TABLET
};

class ADAPTER_EXPORT_API DeviceInfo {
 public:
  DeviceInfo(const DeviceInfo&) = delete;
  DeviceInfo& operator=(const DeviceInfo&) = delete;

  static DeviceInfo& GetInstance();
  static std::string Get(const std::string& property);
  static int SdkApi();
  DeviceType GetDeviceType();

private:
  DeviceInfo();
  ~DeviceInfo() = default;
  static int sdk_api_;
  static DeviceType device_type_;
};
}  // namespace ohos::adapter::device_info
#endif  // OHOS_ADAPTER_DEVICE_INFO_DEVICE_INFO_H_
