// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "base/no_destructor.h"
#include "arkweb/chromium_ext/content/common/arkweb_user_agent_ext.h"

#if !defined(COMPONENT_BUILD)
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "cef/ohos_cef_ext/libcef/browser/useragent/ua_push_config.h"
#endif

namespace content {

#if BUILDFLAG(ARKWEB_USERAGENT)
#if BUILDFLAG(ARKWEB_TEST)
std::atomic<bool> is_compatible_type_setted{false};
std::string& g_compatible_device_type() {
  static base::NoDestructor<std::string> compatible_device_type;
  return *compatible_device_type;
}
void SetArkwebUserAgentExtStateForTest(bool is_setted) {
  is_compatible_type_setted.store(is_setted, std::memory_order_relaxed);
}

void ResetArkwebUserAgentExtStateForTest() {
  SetArkwebUserAgentExtStateForTest(false);
  g_compatible_device_type().clear();
}
#else
namespace {
std::atomic<bool> is_compatible_type_setted{false};
std::string& g_compatible_device_type() {
  static base::NoDestructor<std::string> compatible_device_type;
  return *compatible_device_type;
}
}  // namespace

#endif

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
void UpdateBaseOsName(std::string& base_os_name_front_str,
                       std::string& base_os_name_back_str,
                       const std::string device_type_string) {
#if !defined(COMPONENT_BUILD)
  if (ohos_user_agent::UAPushConfig::GetInstance()) {
    ohos_user_agent::OSPositionPrefsInfo defaultPrefs{"", ""};
    ohos_user_agent::OSPositionPrefsInfo os_postion_prefs =
        ohos_user_agent::UAPushConfig::GetInstance()
            ->GetLastOsPositionStr(device_type_string)
            .value_or(defaultPrefs);
    base_os_name_front_str = os_postion_prefs.front_str;
    base_os_name_back_str = os_postion_prefs.back_str;
  }
#endif
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
    g_compatible_device_type() = base::ohos::CompatibleDeviceType();
    is_compatible_type_setted = true;
  }

  if (!g_compatible_device_type().empty()) {
    if (g_compatible_device_type() == "Phone" || g_compatible_device_type() == "PC" ||
        g_compatible_device_type() == "Tablet") {
      LOG(DEBUG) << "compatible device type is: " << g_compatible_device_type();
      device_type_string = g_compatible_device_type();
    } else {
      LOG(DEBUG) << "unknown compatible device type: " << g_compatible_device_type();
    }
  }

  int32_t ohos_major_version = base::ohos::MajorVersion();
  int32_t ohos_senior_version = base::ohos::SeniorVersion();
  std::string base_os_name = base::ohos::BaseOsName();
  std::string ohos_fullname_str;
  std::string base_os_name_front_str = "";
  std::string base_os_name_back_str = "";
  UpdateBaseOsName(base_os_name_front_str, base_os_name_back_str,
                    device_type_string);
  if (base_os_name.empty() || ohos_major_version == -1 ||
      ohos_senior_version == -1) {
    std::string base_os_name_default = "OpenHarmony";
    base::StringAppendF(
        &ohos_fullname_str, "%s; %s%s%s", device_type_string.c_str(),
        base_os_name_front_str.c_str(), base_os_name_default.c_str(),
        base_os_name_back_str.c_str());
  } else {
    base::StringAppendF(
        &ohos_fullname_str, "%s; %s%s %d.%d%s", device_type_string.c_str(),
        base_os_name_front_str.c_str(), base_os_name.c_str(),
        ohos_major_version, ohos_senior_version, base_os_name_back_str.c_str());
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
    g_compatible_device_type() = base::ohos::CompatibleDeviceType();
    is_compatible_type_setted = true;
  }

  base::StringAppendF(&product_string, " ArkWeb/%s", ARKWEB_VERSION);
  if (base::ohos::IsMobileDevice()) {
    product_string += " Mobile";
  } else if (base::ohos::IsTabletDevice() &&
             (g_compatible_device_type() == "Phone")) {
    product_string += " Mobile";
  } else if (base::ohos::IsPcDevice() && (g_compatible_device_type() == "Phone")) {
    product_string += " Mobile";
  }

  base::StringAppendF(&user_agent, "%s", product_string.c_str());
}

void UpdateHighEntropyCh(blink::UserAgentMetadata& metadata) {
  metadata.architecture = "";
  metadata.bitness = base::ohos::IsPcDevice() ? base::ohos::kBitness64 : "";
  metadata.full_version = "";
  metadata.form_factors.clear();
  metadata.form_factors = {base::ohos::GetDeviceTypeString()};
  metadata.model =
      base::ohos::IsMobileDevice() ? base::ohos::GetProductModel() : "";
  metadata.platform_version = base::ohos::GetOSDistVersion();
  metadata.brand_full_version_list.emplace_back(base::ohos::kWebName,
                                                ARKWEB_VERSION);
}
 
void UpdateLowEntropyCh(blink::UserAgentMetadata& metadata) {
  metadata.brand_version_list.emplace_back(base::ohos::kWebName,
                                           GetDistVersion());
  metadata.mobile = base::ohos::IsMobileDevice();
  metadata.platform = base::ohos::kPlatformName;
}

std::string GetUserAgentByHost(const std::string& host) {
  return "";
}
#endif  // ARKWEB_USERAGENT

}  // namespace content
