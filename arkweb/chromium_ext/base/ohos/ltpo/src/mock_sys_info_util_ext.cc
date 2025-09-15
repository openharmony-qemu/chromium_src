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

#include "mock_sys_info_util_ext.h"

namespace base::ohos {

#ifdef __cplusplus
extern "C" {
#endif
std::string __wrap_OsVersion() {
  return SysInfoUtilsMock::GetInstance().OsVersion();
}
std::string __wrap_CompatibleDeviceType() {
  return SysInfoUtilsMock::GetInstance().CompatibleDeviceType();
}
bool __wrap_IsMobileDevice() {
  return SysInfoUtilsMock::GetInstance().IsMobileDevice();
}
bool __wrap_IsTabletDevice() {
  return SysInfoUtilsMock::GetInstance().IsTabletDevice();
}
bool __wrap_IsPcDevice() {
  return SysInfoUtilsMock::GetInstance().IsPcDevice();
}
std::string __wrap_BaseOsName() {
  return SysInfoUtilsMock::GetInstance().BaseOsName();
}
int32_t __wrap_MajorVersion() {
  return SysInfoUtilsMock::GetInstance().MajorVersion();
}
int32_t __wrap_SeniorVersion() {
  return SysInfoUtilsMock::GetInstance().SeniorVersion();
}
#ifdef __cplusplus
}
#endif
} // namespace base::ohos