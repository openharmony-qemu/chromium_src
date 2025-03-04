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

#include "nweb_adsblock_manager_impl.h"

#include "nweb_adsblock_manager_delegate.h"
#include "nweb_default_engine_init_args_impl.h"
#include "nweb_hilog.h"
#include "nweb_impl.h"

namespace OHOS::NWeb {

// static
std::shared_ptr<NWebAdsBlockManager> NWebAdsBlockManagerImpl::GetInstance() {
  static std::shared_ptr<NWebAdsBlockManagerImpl> adsblock_manager =
      std::make_shared<NWebAdsBlockManagerImpl>();
  return adsblock_manager;
}

NWebAdsBlockManagerImpl::NWebAdsBlockManagerImpl() {
#if defined(USE_CEF)
  delegate_ = std::make_shared<NWebAdsBlockManagerDelegate>();
#endif

  std::shared_ptr<NWebEngineInitArgs> init_args =
      std::make_shared<NWebDefaultEngineInitArgsImpl>();
  (void)NWebImpl::InitializeICUStatic(init_args);
}

void NWebAdsBlockManagerImpl::SetAdsBlockRules(const std::string& rulesFile,
                                               bool replace) {
  if (delegate_ != nullptr) {
    delegate_->SetAdsBlockRules(rulesFile, replace);
  }
}

void NWebAdsBlockManagerImpl::AddAdsBlockDisallowedList(
    const std::vector<std::string>& domainSuffixes) {
  if (delegate_ != nullptr) {
    delegate_->AddAdsBlockDisallowedList(domainSuffixes);
  }
}

void NWebAdsBlockManagerImpl::RemoveAdsBlockDisallowedList(
    const std::vector<std::string>& domainSuffixes) {
  if (delegate_ != nullptr) {
    delegate_->RemoveAdsBlockDisallowedList(domainSuffixes);
  }
}

void NWebAdsBlockManagerImpl::ClearAdsBlockDisallowedList() {
  if (delegate_ != nullptr) {
    delegate_->ClearAdsBlockDisallowedList();
  }
}

void NWebAdsBlockManagerImpl::AddAdsBlockAllowedList(
    const std::vector<std::string>& domainSuffixes) {
  if (delegate_ != nullptr) {
    delegate_->AddAdsBlockAllowedList(domainSuffixes);
  }
}

void NWebAdsBlockManagerImpl::RemoveAdsBlockAllowedList(
    const std::vector<std::string>& domainSuffixes) {
  if (delegate_ != nullptr) {
    delegate_->RemoveAdsBlockAllowedList(domainSuffixes);
  }
}

void NWebAdsBlockManagerImpl::ClearAdsBlockAllowedList() {
  if (delegate_ != nullptr) {
    delegate_->ClearAdsBlockAllowedList();
  }
}

}  // namespace OHOS::NWeb
