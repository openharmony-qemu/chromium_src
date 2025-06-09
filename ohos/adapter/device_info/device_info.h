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
};
enum class DeviceType { _2IN1, _TABLET };

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
