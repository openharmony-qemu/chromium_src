// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/device_info/device_info.h"

#include <map>
#include <string>

#include "ohos/adapter/aki_hook/aki_hook.h"

namespace ohos::adapter::device_info {
namespace {
std::map<std::string, std::string> DEFAULT_VALUE = {
    {kManufacture, "Unknown"},
    {kProductModel, "Unknown"},
    {kDistributionOSName, "OHOS"},
    {kDistributionOSVersion, "1.0.0.0"},
    {kDeviceType, "Unknown"}};
}

std::string DeviceInfo::Get(const std::string& property) {
  if (auto getFunc = ohos::adapter::GetJSFunction("DeviceInfoAdapter.Get")) {
    return getFunc->Invoke<std::string>(property);
  }
  return DEFAULT_VALUE.at(property);
}
}  // namespace ohos::adapter::device_info
