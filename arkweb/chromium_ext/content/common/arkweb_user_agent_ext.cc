// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "arkweb/chromium_ext/content/common/arkweb_user_agent_ext.h"

namespace content {


#if BUILDFLAG(ARKWEB_USERAGENT)
namespace {
std::atomic<bool> is_compatible_type_setted{false};
static std::string compatible_device_type;
}  // namespace

std::string GetDistVersion() {
  std::string dist_version;
  int versionPartOne;
  int versionPartTwo;
  int versionPartThree;
  std::string os_version = base::ohos::OsVersion();
  int ret = sscanf_s(os_version.c_str(), "%d.%d.%d", &versionPartOne,
                     &versionPartTwo, &versionPartThree);
  if (ret <= 0) {
    return dist_version;
  }
  base::StringAppendF(&dist_version, "%d.%d", versionPartOne, versionPartTwo);
  return dist_version;
}

std::string GetOhosFullname() {
  std::string device_type_string = "Phone";
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  if (command_line != nullptr) {
    if (command_line->HasSwitch(::switches::kUserAgentValue)) {
      device_type_string =
          command_line->GetSwitchValueASCII(::switches::kUserAgentValue);
    }
  }

  if (!is_compatible_type_setted) {
    compatible_device_type = base::ohos::CompatibleDeviceType();
    is_compatible_type_setted = true;
  }

  if (!compatible_device_type.empty()) {
    if (compatible_device_type == "Phone" || compatible_device_type == "PC" ||
        compatible_device_type == "Tablet") {
      LOG(DEBUG) << "compatible device type is: " << compatible_device_type;
      device_type_string = compatible_device_type;
    } else {
      LOG(DEBUG) << "unknown compatible device type: " << compatible_device_type;
    }
  }

  int32_t ohos_major_version = base::ohos::MajorVersion();
  int32_t ohos_senior_version = base::ohos::SeniorVersion();
  std::string base_os_name = base::ohos::BaseOsName();
  std::string ohos_fullname_str;
  if (base_os_name.empty() || ohos_major_version == -1 ||
      ohos_senior_version == -1) {
    std::string base_os_name_default = "OpenHarmony";
    base::StringAppendF(&ohos_fullname_str, "%s; %s",
                        device_type_string.c_str(),
                        base_os_name_default.c_str());
  } else {
    base::StringAppendF(&ohos_fullname_str, "%s; %s %d.%d",
                        device_type_string.c_str(), base_os_name.c_str(),
                        ohos_major_version, ohos_senior_version);
  }
  std::string dist_os_name = base::ohos::BaseOsName();
  std::string dist_version = GetDistVersion();
  if (!dist_version.empty() && !dist_os_name.empty() &&
      dist_os_name != base_os_name) {
    base::StringAppendF(&ohos_fullname_str, "; %s %s", dist_os_name.c_str(),
                        dist_version.c_str());
  }
  return ohos_fullname_str;
}

void SetProductString(std::string& user_agent) {
  std::string product_string = "";

  if (!is_compatible_type_setted) {
    compatible_device_type = base::ohos::CompatibleDeviceType();
    is_compatible_type_setted = true;
  }

  base::StringAppendF(&product_string, " ArkWeb/%s", ARKWEB_VERSION);
  if (base::ohos::IsMobileDevice()) {
    product_string += " Mobile";
  } else if (base::ohos::IsTabletDevice() &&
             (compatible_device_type == "Phone")) {
    product_string += " Mobile";
  } else if (base::ohos::IsPcDevice() && (compatible_device_type == "Phone")) {
    product_string += " Mobile";
  }

  base::StringAppendF(&user_agent, "%s", product_string.c_str());
}
#endif  // ARKWEB_USERAGENT

}  // namespace content
