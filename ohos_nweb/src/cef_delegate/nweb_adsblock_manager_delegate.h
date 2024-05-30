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

#ifndef NWEB_ADSBLOCK_MANAGER_DELEGATE_H
#define NWEB_ADSBLOCK_MANAGER_DELEGATE_H

#include "cef/include/cef_base.h"
#include "cef/include/cef_adsblock_manager.h"
#include "nweb_adsblock_manager_delegate_interface.h"
#include "nweb_errors.h"

namespace OHOS::NWeb {
class NWebAdsBlockManagerDelegate : public NWebAdsBlockManagerDelegateInterface {
 public:
  NWebAdsBlockManagerDelegate() = default;
  ~NWebAdsBlockManagerDelegate() = default;

  void SetAdsBlockRules(const std::string &rulesFile, bool replace) override;

  void AddAdsBlockDisallowedList(const std::vector<std::string> &domainSuffixes) override;

  void RemoveAdsBlockDisallowedList(const std::vector<std::string> &domainSuffixes) override;

  void ClearAdsBlockDisallowedList() override;

  void AddAdsBlockAllowedList(const std::vector<std::string> &domainSuffixes) override;

  void RemoveAdsBlockAllowedList(const std::vector<std::string> &domainSuffixes) override;

  void ClearAdsBlockAllowedList() override;

 private:
  CefRefPtr<CefAdsBlockManager> GetGlobalAdsBlockManager();
  CefRefPtr<CefAdsBlockManager> adsblock_manager_ = nullptr;
};
}  // namespace OHOS::NWeb

#endif  // NWEB_ADSBLOCK_MANAGER_DELEGATE_H
