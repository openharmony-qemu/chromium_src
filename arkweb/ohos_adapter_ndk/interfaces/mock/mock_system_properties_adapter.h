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

#ifndef SYSTEM_PROPERTIES_ADAPTER_MOCK_H
#define SYSTEM_PROPERTIES_ADAPTER_MOCK_H


#include <gmock/gmock.h>

#include "system_properties_adapter.h"
#include "adapter_base.h"

namespace OHOS::NWeb {

class MockSystemPropertiesAdapter : public SystemPropertiesAdapter {
 public:
  MOCK_METHOD(bool, GetResourceUseHapPathEnable, (), (override));
  MOCK_METHOD(std::string, GetDeviceInfoProductModel, (), (override));
  MOCK_METHOD(std::string, GetDeviceInfoBrand, (), (override));
  MOCK_METHOD(int32_t, GetDeviceInfoMajorVersion, (), (override));
  MOCK_METHOD(ProductDeviceType, GetProductDeviceType, (), (override));
  MOCK_METHOD(bool, GetWebOptimizationValue, (), (override));
  MOCK_METHOD(bool, IsAdvancedSecurityMode, (), (override));
  MOCK_METHOD(std::string, GetUserAgentOSName, (), (override));
  MOCK_METHOD(int32_t, GetSoftwareMajorVersion, (), (override));
  MOCK_METHOD(int32_t, GetSoftwareSeniorVersion, (), (override));
  MOCK_METHOD(std::string, GetNetlogMode, (), (override));
  MOCK_METHOD(bool, GetTraceDebugEnable, (), (override));
  MOCK_METHOD(std::string, GetSiteIsolationMode, (), (override));
  MOCK_METHOD(int32_t, GetFlowBufMaxFd, (), (override));
  MOCK_METHOD(bool, GetOOPGPUEnable, (), (override));
  MOCK_METHOD(void, SetOOPGPUDisable, (), (override));
  MOCK_METHOD(void,
              AttachSysPropObserver,
              (PropertiesKey key, SystemPropertiesObserver* observer),
              (override));
  MOCK_METHOD(void,
              DetachSysPropObserver,
              (PropertiesKey key, SystemPropertiesObserver* observer),
              (override));
  MOCK_METHOD(bool,
              GetBoolParameter,
              (const std::string& key, bool defaultValue),
              (override));
  MOCK_METHOD(std::vector<FrameRateSetting>,
              GetLTPOConfig,
              (const std::string& settingName),
              (override));
  MOCK_METHOD(std::string, GetOOPGPUStatus, (), (override));
  MOCK_METHOD(bool,
              IsLTPODynamicApp,
              (const std::string& bundleName),
              (override));
  MOCK_METHOD(int32_t, GetLTPOStrategy, (), (override));
  MOCK_METHOD(std::string, GetUserAgentBaseOSName, (), (override));
  MOCK_METHOD(std::string, GetUserAgentOSVersion, (), (override));
  MOCK_METHOD(std::string, GetVulkanStatus, (), (override));
  MOCK_METHOD(std::string, GetCompatibleDeviceType, (), (override));
  MOCK_METHOD(std::string, GetDeviceInfoApiVersion, (), (override));
  MOCK_METHOD(std::string, GetPRPPreloadMode, (), (override));
  MOCK_METHOD(std::string, GetScrollVelocityScale, (), (override));
  MOCK_METHOD(std::string, GetScrollFriction, (), (override));
  MOCK_METHOD(std::string, GetBundleName, (), (override));
  MOCK_METHOD(std::string, GetStringParameter, 
              (const std::string& key, const std::string& defaultValue), (override));
  MOCK_METHOD(int32_t, GetInitialCongestionWindowSize, (), (override));
  MOCK_METHOD(int32_t, GetIntParameter, (const std::string& key, int32_t defaultValue), (override));
  MOCK_METHOD(int32_t, GetLoadUrlStrategy, (), (override));
};

}  // namespace OHOS::NWeb

#endif  // SYSTEM_PROPERTIES_ADAPTER_MOCK_H
