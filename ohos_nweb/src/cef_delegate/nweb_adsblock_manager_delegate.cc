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

#include "nweb_adsblock_manager_delegate.h"
#include <memory>
#include <thread>
#include <vector>
#include "base/logging.h"

using namespace OHOS::NWeb;

namespace OHOS::NWeb {

CefRefPtr<CefAdsBlockManager>
NWebAdsBlockManagerDelegate::GetGlobalAdsBlockManager() {
  if (!adsblock_manager_) {
    adsblock_manager_ = CefAdsBlockManager::GetGlobalAdsBlockManager(nullptr);
  }
  return adsblock_manager_;
}

void NWebAdsBlockManagerDelegate::SetAdsBlockRules(const std::string &rulesFile, bool replace) {
  LOG(INFO) << "[AdBlock] SetAdsBlockRules:" << rulesFile << ", replace";
  CefRefPtr<CefAdsBlockManager> adsblock_manager = GetGlobalAdsBlockManager();
  if (adsblock_manager == nullptr) {
    LOG(ERROR) << "GetGlobalAdsBlockManager failed";
    return;
  }

  adsblock_manager->SetAdsBlockRules(CefString(rulesFile), replace);
}

void NWebAdsBlockManagerDelegate::AddAdsBlockDisallowedList(const std::vector<std::string> &domainSuffixes) {
  LOG(INFO) << "[AdBlock] AddAdsBlockDisallowedList counts:" << domainSuffixes.size();
  CefRefPtr<CefAdsBlockManager> adsblock_manager = GetGlobalAdsBlockManager();
  if (adsblock_manager == nullptr) {
    LOG(ERROR) << "GetGlobalAdsBlockManager failed";
    return;
  }

  std::vector<CefString> cefDomainSuffixes;
  for (size_t i = 0; i < domainSuffixes.size(); i++) {
    cefDomainSuffixes.push_back(CefString(domainSuffixes[i]));
  }

  adsblock_manager->AddAdsBlockDisallowedList(cefDomainSuffixes);
}

void NWebAdsBlockManagerDelegate::RemoveAdsBlockDisallowedList(const std::vector<std::string> &domainSuffixes) {
  LOG(INFO) << "[AdBlock] RemoveAdsBlockDisallowedList counts:" << domainSuffixes.size();
  CefRefPtr<CefAdsBlockManager> adsblock_manager = GetGlobalAdsBlockManager();
  if (adsblock_manager == nullptr) {
    LOG(ERROR) << "GetGlobalAdsBlockManager failed";
    return;
  }

  std::vector<CefString> cefDomainSuffixes;
  for (size_t i = 0; i < domainSuffixes.size(); i++) {
    cefDomainSuffixes.push_back(CefString(domainSuffixes[i]));
  }

  adsblock_manager->RemoveAdsBlockDisallowedList(cefDomainSuffixes);
}

void NWebAdsBlockManagerDelegate::ClearAdsBlockDisallowedList() {
  LOG(INFO) << "[AdBlock] ClearAdsBlockDisallowedList";
  CefRefPtr<CefAdsBlockManager> adsblock_manager = GetGlobalAdsBlockManager();
  if (adsblock_manager == nullptr) {
    LOG(ERROR) << "GetGlobalAdsBlockManager failed";
    return;
  }

  adsblock_manager->ClearAdsBlockDisallowedList();
}

void NWebAdsBlockManagerDelegate::AddAdsBlockAllowedList(const std::vector<std::string> &domainSuffixes) {
  LOG(INFO) << "[AdBlock] AddAdsBlockAllowedList counts:" << domainSuffixes.size();
  CefRefPtr<CefAdsBlockManager> adsblock_manager = GetGlobalAdsBlockManager();
  if (adsblock_manager == nullptr) {
    LOG(ERROR) << "GetGlobalAdsBlockManager failed";
    return;
  }

  std::vector<CefString> cefDomainSuffixes;
  for (size_t i = 0; i < domainSuffixes.size(); i++) {
    cefDomainSuffixes.push_back(CefString(domainSuffixes[i]));
  }

  adsblock_manager->AddAdsBlockAllowedList(cefDomainSuffixes);
}

void NWebAdsBlockManagerDelegate::RemoveAdsBlockAllowedList(const std::vector<std::string> &domainSuffixes) {
  LOG(INFO) << "[AdBlock] RemoveAdsBlockAllowedList counts:" << domainSuffixes.size();
  CefRefPtr<CefAdsBlockManager> adsblock_manager = GetGlobalAdsBlockManager();
  if (adsblock_manager == nullptr) {
    LOG(ERROR) << "GetGlobalAdsBlockManager failed";
    return;
  }

  std::vector<CefString> cefDomainSuffixes;
  for (size_t i = 0; i < domainSuffixes.size(); i++) {
    cefDomainSuffixes.push_back(CefString(domainSuffixes[i]));
  }

  adsblock_manager->RemoveAdsBlockAllowedList(cefDomainSuffixes);
}

void NWebAdsBlockManagerDelegate::ClearAdsBlockAllowedList() {
  LOG(INFO) << "[AdBlock] ClearAdsBlockAllowedList";
  CefRefPtr<CefAdsBlockManager> adsblock_manager = GetGlobalAdsBlockManager();
  if (adsblock_manager == nullptr) {
    LOG(ERROR) << "GetGlobalAdsBlockManager failed";
    return;
  }

  adsblock_manager->ClearAdsBlockAllowedList();
}

}  // namespace OHOS::NWeb
