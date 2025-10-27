// Copyright (c) 2019 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "libcef/browser/adsblock_manager_impl.h"

#include "arkweb/build/features/features.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/path_service.h"
#include "content/public/browser/browser_context.h"

#if BUILDFLAG(ARKWEB_ADBLOCK)
#include "cef/ohos_cef_ext/libcef/browser/subresource_filter/adblock_list.h"
#include "components/subresource_filter/content/browser/ohos_adblock_config.h"
#endif

namespace {

// Always execute the callback asynchronously.
void RunAsyncCompletionOnUIThread(CefRefPtr<CefCompletionCallback> callback) {
  if (!callback.get()) {
    return;
  }
  CEF_POST_TASK(CEF_UIT, base::BindOnce(&CefCompletionCallback::OnComplete,
                                        callback.get()));
}
}  // namespace

void CefAdsBlockManagerImpl::Initialize(
    CefBrowserContext::Getter browser_context_getter,
    CefRefPtr<CefCompletionCallback> callback) {
  CEF_REQUIRE_UIT();
  DCHECK(!initialized_);
  DCHECK(!browser_context_getter.is_null());
  DCHECK(browser_context_getter_.is_null());

  if (initialized_) {
    LOG(INFO) << "[AdBlock] CefAdsBlockManagerImpl already initialized.";
    return;
  }

  browser_context_getter_ = browser_context_getter;
  initialized_ = true;
  if (!init_callbacks_.empty()) {
    for (auto& init_callback : init_callbacks_) {
      std::move(init_callback).Run();
    }
    init_callbacks_.clear();
  }

  RunAsyncCompletionOnUIThread(callback);
}

void CefAdsBlockManagerImpl::StoreOrTriggerInitCallback(
    base::OnceClosure callback) {
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(
        CEF_UIT,
        base::BindOnce(&CefAdsBlockManagerImpl::StoreOrTriggerInitCallback,
                       this, std::move(callback)));
    return;
  }

  if (initialized_) {
    std::move(callback).Run();
  } else {
    init_callbacks_.emplace_back(std::move(callback));
  }
}

bool CefAdsBlockManagerImpl::ValidContext() const {
  return CEF_CURRENTLY_ON_UIT() && initialized_;
}

// CefAdsBlockManager methods
// ----------------------------------------------------

void CefAdsBlockManagerImpl::SetAdsBlockRules(const CefString& rulesFile,
                                              bool replace) {
  if (!ValidContext()) {
    StoreOrTriggerInitCallback(
        base::BindOnce(&CefAdsBlockManagerImpl::SetAdsBlockRulesInternal, this,
                       rulesFile, replace));
    return;
  }

  SetAdsBlockRulesInternal(rulesFile, replace);
}

void CefAdsBlockManagerImpl::SetAdsBlockRulesInternal(
    const CefString& rulesFile,
    bool replace) {
  DCHECK(ValidContext());

  if (!base::PathExists(base::FilePath(rulesFile))) {
    LOG(ERROR) << "[AdBlock] User Easylist file does not exist:"
               << rulesFile.ToString();
    return;
  }

  OHOS::adblock::AdBlockConfig::GetInstance()->SetAdsBlockRules(rulesFile,
                                                                replace);

  ::adblock::AdBlockList::UpdateUserEasyListRules(rulesFile);
}

void CefAdsBlockManagerImpl::AddAdsBlockDisallowedList(
    const std::vector<CefString>& domainSuffixes) {
  if (!ValidContext()) {
    StoreOrTriggerInitCallback(base::BindOnce(
        &CefAdsBlockManagerImpl::AddAdsBlockDisallowedListInternal, this,
        domainSuffixes));
    return;
  }

  AddAdsBlockDisallowedListInternal(domainSuffixes);
}

void CefAdsBlockManagerImpl::AddAdsBlockDisallowedListInternal(
    const std::vector<CefString>& domainSuffixes) {
  DCHECK(ValidContext());

  std::vector<std::string> domainSuffixes_str;
  for (auto& domainSuffix : domainSuffixes) {
    domainSuffixes_str.push_back(domainSuffix.ToString());
  }

  OHOS::adblock::AdBlockConfig::GetInstance()->AddAdsBlockDisallowList(
      domainSuffixes_str);
}

void CefAdsBlockManagerImpl::RemoveAdsBlockDisallowedList(
    const std::vector<CefString>& domainSuffixes) {
  if (!ValidContext()) {
    StoreOrTriggerInitCallback(base::BindOnce(
        &CefAdsBlockManagerImpl::RemoveAdsBlockDisallowedListInternal, this,
        domainSuffixes));
    return;
  }

  RemoveAdsBlockDisallowedListInternal(domainSuffixes);
}

void CefAdsBlockManagerImpl::RemoveAdsBlockDisallowedListInternal(
    const std::vector<CefString>& domainSuffixes) {
  DCHECK(ValidContext());

  std::vector<std::string> domainSuffixes_str;
  for (auto& domainSuffix : domainSuffixes) {
    domainSuffixes_str.push_back(domainSuffix.ToString());
  }

  OHOS::adblock::AdBlockConfig::GetInstance()->RemoveAdsBlockDisallowedList(
      domainSuffixes_str);
}

void CefAdsBlockManagerImpl::ClearAdsBlockDisallowedList() {
  if (!ValidContext()) {
    StoreOrTriggerInitCallback(base::BindOnce(
        &CefAdsBlockManagerImpl::ClearAdsBlockDisallowedListInternal, this));
    return;
  }

  ClearAdsBlockDisallowedListInternal();
}

void CefAdsBlockManagerImpl::ClearAdsBlockDisallowedListInternal() {
  DCHECK(ValidContext());

  OHOS::adblock::AdBlockConfig::GetInstance()->ClearAdsBlockDisallowedList();
}

void CefAdsBlockManagerImpl::AddAdsBlockAllowedList(
    const std::vector<CefString>& domainSuffixes) {
  if (!ValidContext()) {
    StoreOrTriggerInitCallback(
        base::BindOnce(&CefAdsBlockManagerImpl::AddAdsBlockAllowedListInternal,
                       this, domainSuffixes));
    return;
  }

  AddAdsBlockAllowedListInternal(domainSuffixes);
}

void CefAdsBlockManagerImpl::AddAdsBlockAllowedListInternal(
    const std::vector<CefString>& domainSuffixes) {
  DCHECK(ValidContext());

  std::vector<std::string> domainSuffixes_str;
  for (auto& domainSuffix : domainSuffixes) {
    domainSuffixes_str.push_back(domainSuffix.ToString());
  }

  OHOS::adblock::AdBlockConfig::GetInstance()->AddAdsBlockAllowList(
      domainSuffixes_str);
}

void CefAdsBlockManagerImpl::RemoveAdsBlockAllowedList(
    const std::vector<CefString>& domainSuffixes) {
  if (!ValidContext()) {
    StoreOrTriggerInitCallback(base::BindOnce(
        &CefAdsBlockManagerImpl::RemoveAdsBlockAllowedListInternal, this,
        domainSuffixes));
    return;
  }

  RemoveAdsBlockAllowedListInternal(domainSuffixes);
}

void CefAdsBlockManagerImpl::RemoveAdsBlockAllowedListInternal(
    const std::vector<CefString>& domainSuffixes) {
  DCHECK(ValidContext());

  std::vector<std::string> domainSuffixes_str;
  for (auto& domainSuffix : domainSuffixes) {
    domainSuffixes_str.push_back(domainSuffix.ToString());
  }

  OHOS::adblock::AdBlockConfig::GetInstance()->RemoveAdsBlockAllowedList(
      domainSuffixes_str);
}

void CefAdsBlockManagerImpl::ClearAdsBlockAllowedList() {
  if (!ValidContext()) {
    StoreOrTriggerInitCallback(base::BindOnce(
        &CefAdsBlockManagerImpl::ClearAdsBlockAllowedListInternal, this));
    return;
  }

  ClearAdsBlockAllowedListInternal();
}

void CefAdsBlockManagerImpl::ClearAdsBlockAllowedListInternal() {
  DCHECK(ValidContext());

  OHOS::adblock::AdBlockConfig::GetInstance()->ClearAdsBlockAllowedList();
}

// static methods ----------------------------------------------------

// static
CefRefPtr<CefAdsBlockManagerImpl> CefAdsBlockManagerImpl::GetInstance() {
  static CefRefPtr<CefAdsBlockManagerImpl> instance =
      new CefAdsBlockManagerImpl();
  return instance;
}

// static
CefRefPtr<CefAdsBlockManager> CefAdsBlockManager::GetGlobalAdsBlockManager(
    CefRefPtr<CefCompletionCallback> callback) {
  CefRefPtr<CefRequestContext> context = CefRequestContext::GetGlobalContext();
  return context ? context->GetAdsBlockManager(callback) : nullptr;
}
