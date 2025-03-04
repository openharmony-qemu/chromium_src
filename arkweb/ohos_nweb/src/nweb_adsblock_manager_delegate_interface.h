/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NWEB_ADSBLOCK_MANAGER_DELEGATE_INTERFACE_H
#define NWEB_ADSBLOCK_MANAGER_DELEGATE_INTERFACE_H

#include <memory>
#include <string>

#include "nweb_value_callback.h"

namespace OHOS::NWeb {
class NWebAdsBlockManagerDelegateInterface {
 public:
  virtual ~NWebAdsBlockManagerDelegateInterface() = default;

  virtual void SetAdsBlockRules(const std::string& rulesFile, bool replace) = 0;

  virtual void AddAdsBlockDisallowedList(
      const std::vector<std::string>& domainSuffixes) = 0;

  virtual void RemoveAdsBlockDisallowedList(
      const std::vector<std::string>& domainSuffixes) = 0;

  virtual void ClearAdsBlockDisallowedList() = 0;

  virtual void AddAdsBlockAllowedList(
      const std::vector<std::string>& domainSuffixes) = 0;

  virtual void RemoveAdsBlockAllowedList(
      const std::vector<std::string>& domainSuffixes) = 0;

  virtual void ClearAdsBlockAllowedList() = 0;
};
}  // namespace OHOS::NWeb

#endif  // NWEB_ADSBLOCK_MANAGER_DELEGATE_INTERFACE_H
