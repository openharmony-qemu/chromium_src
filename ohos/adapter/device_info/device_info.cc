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
 
#include "ohos/adapter/device_info/device_info.h"
#include "ohos/adapter/common/logging.h"
#include <deviceinfo.h>
#include <map>
#include <string>
 
using stringFunction = std::function<std::string()>;
 
namespace ohos::adapter::device_info {
int DeviceInfo::sdk_api_ = -1;
std::map<std::string, std::string> DEFAULT_VALUE = {
    {kManufacture, "Unknown"},
    {kProductModel, "Unknown"},
    {kDistributionOSName, "OHOS"},
    {kDistributionOSVersion, "1.0.0.0"},
    {kDisplayVersion, "Unknown"},
    {kOsReleaseType, "Unknown"}
  };
 
std::map<std::string, stringFunction> DEVICE_INFO = {
    {kManufacture, []() {return OH_GetManufacture();}},
    {kProductModel, []() {return OH_GetProductModel();}},
    {kDistributionOSName, []() {return OH_GetDistributionOSName();}},
    {kDistributionOSVersion, []() {return OH_GetDistributionOSVersion();}},
    {kDisplayVersion,  []() {return OH_GetDisplayVersion();}},
    {kOsReleaseType,  []() {return OH_GetOsReleaseType();}}
  };
std::map<std::string, DeviceType> DEVICE_TYPE = {
    {"2in1", DeviceType::_2IN1},
    {"tablet", DeviceType::_TABLET} 
 };
DeviceInfo& DeviceInfo::GetInstance() {
  static DeviceInfo device_info_;
  return device_info_;
}
// set default device type to 2in1
DeviceType DeviceInfo::device_type_ = DeviceType::_2IN1;
DeviceInfo::DeviceInfo() {
  std::string deviceType = OH_GetDeviceType();
  auto it = DEVICE_TYPE.find(deviceType);
  if (it != DEVICE_TYPE.end()) {
    DeviceInfo::device_type_ = it->second;

  } else {
    LOGE("Got invalid device type: %{public}s", deviceType.c_str());
  }
}

std::string DeviceInfo::Get(const std::string& property) {
  std::string deviceInfo = "";
  auto it = DEVICE_INFO.find(property);
  if (it != DEVICE_INFO.end()) {
    deviceInfo = it->second();
  }
  return deviceInfo == "" ? DEFAULT_VALUE.at(property) : deviceInfo;
}

int DeviceInfo::SdkApi() {
  if (DeviceInfo::sdk_api_ == -1) {
    DeviceInfo::sdk_api_ = OH_GetSdkApiVersion();
  }
  return DeviceInfo::sdk_api_;
}
DeviceType DeviceInfo::GetDeviceType() {
  return DeviceInfo::device_type_;  
}
}  // namespace ohos::adapter::device_info
