/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "base/ohos/sys_info_utils.h"

#include "base/threading/scoped_blocking_call.h"
#include "ohos_adapter_helper.h"
#include "ui/base/clipboard/clipboard.h"

namespace base {
namespace ohos {

namespace {

using namespace OHOS::NWeb;

class SystemProperties {
 public:
  static SystemProperties* Instance() {
    static NoDestructor<SystemProperties> instance;
    return instance.get();
  }

  bool is_mobile() {
    return device_type_ == ProductDeviceType::DEVICE_TYPE_MOBILE;
  }

  bool is_tablet() {
    return device_type_ == ProductDeviceType::DEVICE_TYPE_TABLET;
  }

  bool is_2in1() { return device_type_ == ProductDeviceType::DEVICE_TYPE_2IN1; }

  int32_t major_version() { return major_version_; }

  int32_t senior_version() { return senior_version_; }

  std::string os_name() { return os_name_; }

  std::string os_version() { return os_version_; }

  std::string base_os_name() { return base_os_name_; }

 private:
  friend class NoDestructor<SystemProperties>;

  SystemProperties();
  ~SystemProperties() = default;

  int32_t major_version_;
  int32_t senior_version_;
  OHOS::NWeb::ProductDeviceType device_type_;
  std::string os_name_;
  std::string os_version_;
  std::string base_os_name_;
};

SystemProperties::SystemProperties()
    : major_version_(OhosAdapterHelper::GetInstance()
                         .GetSystemPropertiesInstance()
                         .GetSoftwareMajorVersion()),
      senior_version_(OhosAdapterHelper::GetInstance()
                          .GetSystemPropertiesInstance()
                          .GetSoftwareSeniorVersion()),
      device_type_(OhosAdapterHelper::GetInstance()
                       .GetSystemPropertiesInstance()
                       .GetProductDeviceType()),
      os_name_(OhosAdapterHelper::GetInstance()
                   .GetSystemPropertiesInstance()
                   .GetUserAgentOSName()),
      os_version_(OhosAdapterHelper::GetInstance()
                   .GetSystemPropertiesInstance()
                   .GetUserAgentOSVersion()),
      base_os_name_(OhosAdapterHelper::GetInstance()
                   .GetSystemPropertiesInstance()
                   .GetUserAgentBaseOSName()){}

}  // namespace

BASE_EXPORT bool IsMobileDevice() {
  return SystemProperties::Instance()->is_mobile();
}

BASE_EXPORT bool IsTabletDevice() {
  return SystemProperties::Instance()->is_tablet();
}

BASE_EXPORT bool IsPcDevice() {
  // 2in1 is treated as pc device on ohos platform now.
  return SystemProperties::Instance()->is_2in1();
}

BASE_EXPORT int32_t MajorVersion() {
  return SystemProperties::Instance()->major_version();
}

BASE_EXPORT int32_t SeniorVersion() {
  return SystemProperties::Instance()->senior_version();
}

BASE_EXPORT std::string OsName() {
  return SystemProperties::Instance()->os_name();
}

BASE_EXPORT std::string OsVersion() {
  return SystemProperties::Instance()->os_version();
}

BASE_EXPORT std::string BaseOsName() {
  return SystemProperties::Instance()->base_os_name();
}

}  // namespace ohos

}  // namespace base