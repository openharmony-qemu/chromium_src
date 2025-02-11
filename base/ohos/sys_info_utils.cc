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

#include "base/command_line.h"
#include "base/threading/scoped_blocking_call.h"
#include "content/public/common/content_switches.h"
#include "base/logging.h"
#include "ohos_adapter_helper.h"
#include "ui/base/clipboard/clipboard.h"

namespace base {
namespace ohos {

namespace {

constexpr char kProductModeEmulator[] = "emulator";
constexpr char kCompatiblePhone[] = "Phone";
constexpr char kCompatibleTablet[] = "Tablet";

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

  bool is_emulator() { return product_model_ == kProductModeEmulator; }

  int32_t major_version() { return major_version_; }

  int32_t senior_version() { return senior_version_; }

  std::string os_name() { return os_name_; }

  std::string os_version() { return os_version_; }

  std::string base_os_name() { return base_os_name_; }

  std::string product_model() { return product_model_; }

  bool is_compatible_mode() {
    LOG(INFO) << "systemProperties compatible type is " << compatible_device_type_.c_str();
    return is_2in1() &&
           (compatible_device_type_ == kCompatiblePhone || compatible_device_type_ == kCompatibleTablet);
  }
#ifdef OHOS_SCROLLBAR
  float get_pixel_ratio() { return virtual_pixel_ratio_;}
  void set_pixel_ratio(float ratio) { virtual_pixel_ratio_ = ratio;}
#endif
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
  std::string product_model_;
  std::string compatible_device_type_;
#ifdef OHOS_SCROLLBAR
  float virtual_pixel_ratio_ = 2.0;
#endif
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
                   .GetUserAgentBaseOSName()),
      product_model_(OhosAdapterHelper::GetInstance()
                         .GetSystemPropertiesInstance()
                         .GetDeviceInfoProductModel()),
      compatible_device_type_(OhosAdapterHelper::GetInstance()
                             .GetSystemPropertiesInstance()
                             .GetCompatibleDeviceType()) {}

}  // namespace

#ifdef OHOS_SCROLLBAR
BASE_EXPORT float GetPixelRatio() {
  return SystemProperties::Instance()->get_pixel_ratio();
}

BASE_EXPORT void SetPixelRatio(float ratio) {
  SystemProperties::Instance()->set_pixel_ratio(ratio);
}
#endif

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

BASE_EXPORT bool IsEmulator() {
  return SystemProperties::Instance()->is_emulator();
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

BASE_EXPORT int32_t ApplicationApiVersion() {
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kOhosAppApiVersion)) {
    std::string apiVersion =
        base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
            switches::kOhosAppApiVersion);
    if (!apiVersion.empty()) {
      // Same as API_VERSION_MOD in js_ui_ability.cpp of ability_runtime
      static int32_t kApiVersionMod = 100;
      return (std::stoi(apiVersion) % kApiVersionMod);
    }
  }
  return -1;
}

BASE_EXPORT bool IsCompatibleMode() {
  return SystemProperties::Instance()->is_compatible_mode();
}

}  // namespace ohos

}  // namespace base