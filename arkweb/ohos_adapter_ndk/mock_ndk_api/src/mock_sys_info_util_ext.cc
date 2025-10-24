/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "arkweb/ohos_adapter_ndk/mock_ndk_api/include/mock_sys_info_util_ext.h"

namespace base::ohos {

bool SysInfoUtilsMock::mockOsVersion = false;
bool SysInfoUtilsMock::mockCompatibleDeviceType = false;
bool SysInfoUtilsMock::mockIsMobileDevice = false;
bool SysInfoUtilsMock::mockIsTabletDevice = false;
bool SysInfoUtilsMock::mockIsPcDevice = false;
bool SysInfoUtilsMock::mockBaseOsName = false;
bool SysInfoUtilsMock::mockMajorVersion = false;
bool SysInfoUtilsMock::mockSeniorVersion = false;

#ifdef __cplusplus
extern "C" {
#endif

std::string __wrap_OsVersion() {
  if (SysInfoUtilsMock::mockOsVersion == true) {
    return SysInfoUtilsMock::GetInstance().OsVersion();
  } else {
    return __real_OsVersion();
  }
}

std::string __wrap_CompatibleDeviceType() {
  if (SysInfoUtilsMock::mockCompatibleDeviceType == true) {
    return SysInfoUtilsMock::GetInstance().CompatibleDeviceType();
  } else {
    return __real_CompatibleDeviceType();
  }
}

bool __wrap_IsMobileDevice() {
  if (SysInfoUtilsMock::mockIsMobileDevice == true) {
    return SysInfoUtilsMock::GetInstance().IsMobileDevice();
  } else {
    return __real_IsMobileDevice();
  }
}

bool __wrap_IsTabletDevice() {
  if (SysInfoUtilsMock::mockIsTabletDevice == true) {
    return SysInfoUtilsMock::GetInstance().IsTabletDevice();
  } else {
    return __real_IsTabletDevice();
  }
}

bool __wrap_IsPcDevice() {
  if (SysInfoUtilsMock::mockIsPcDevice == true) {
    return SysInfoUtilsMock::GetInstance().IsPcDevice();
  } else {
    return __real_IsPcDevice();
  }
}

std::string __wrap_BaseOsName() {
  if (SysInfoUtilsMock::mockBaseOsName == true) {
    return SysInfoUtilsMock::GetInstance().BaseOsName();
  } else {
    return __real_BaseOsName();
  }
}

int32_t __wrap_MajorVersion() {
  if (SysInfoUtilsMock::mockMajorVersion == true) {
    return SysInfoUtilsMock::GetInstance().MajorVersion();
  } else {
    return __real_MajorVersion();
  }
}

int32_t __wrap_SeniorVersion() {
  if (SysInfoUtilsMock::mockSeniorVersion == true) {
    return SysInfoUtilsMock::GetInstance().SeniorVersion();
  } else {
    return __real_SeniorVersion();
  }
}
#ifdef __cplusplus
}
#endif
}  // namespace base::ohos