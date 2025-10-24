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
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace base::ohos {
class SysInfoUtilsMock {
public:
  static SysInfoUtilsMock& GetInstance() {
    static SysInfoUtilsMock instance;
    return instance;
  }
  MOCK_METHOD(std::string, OsVersion, (), ());
  MOCK_METHOD(std::string, CompatibleDeviceType, (), ());
  MOCK_METHOD(bool, IsMobileDevice, (), ());
  MOCK_METHOD(bool, IsTabletDevice, (), ());
  MOCK_METHOD(bool, IsPcDevice, (), ());
  MOCK_METHOD(std::string, BaseOsName, (), ());
  MOCK_METHOD(int32_t, MajorVersion, (), ());
  MOCK_METHOD(int32_t, SeniorVersion, (), ());

  static bool mockOsVersion;
  static bool mockCompatibleDeviceType;
  static bool mockIsMobileDevice;
  static bool mockIsTabletDevice;
  static bool mockIsPcDevice;
  static bool mockBaseOsName;
  static bool mockMajorVersion;
  static bool mockSeniorVersion;
};

#ifdef __cplusplus
extern "C" {
#endif
  std::string __wrap_OsVersion();
  std::string __wrap_CompatibleDeviceType();
  bool __wrap_IsMobileDevice();
  bool __wrap_IsTabletDevice();
  bool __wrap_IsPcDevice();
  std::string __wrap_BaseOsName();
  int32_t __wrap_MajorVersion();
  int32_t __wrap_SeniorVersion();

  std::string __real_OsVersion();
  std::string __real_CompatibleDeviceType();
  bool __real_IsMobileDevice();
  bool __real_IsTabletDevice();
  bool __real_IsPcDevice();
  std::string __real_BaseOsName();
  int32_t __real_MajorVersion();
  int32_t __real_SeniorVersion();
#ifdef __cplusplus
}
#endif

} // namespace base::ohos