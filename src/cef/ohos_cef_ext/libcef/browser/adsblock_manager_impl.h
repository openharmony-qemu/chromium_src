// Copyright (c) 2019 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_ADSBLOCK_MANAGER_IMPL_H_
#define CEF_LIBCEF_BROWSER_ADSBLOCK_MANAGER_IMPL_H_

#include "include/cef_adsblock_manager.h"
#include "libcef/browser/adsblock_manager_impl.h"
#include "libcef/browser/browser_context.h"
#include "libcef/browser/thread_util.h"

// Implementation of the CefAdsBlockManager interface. May be created on any
// thread.
class CefAdsBlockManagerImpl : public CefAdsBlockManager {
 public:
  static CefRefPtr<CefAdsBlockManagerImpl> GetInstance();

  CefAdsBlockManagerImpl() = default;

  CefAdsBlockManagerImpl(const CefAdsBlockManagerImpl&) = delete;
  CefAdsBlockManagerImpl& operator=(const CefAdsBlockManagerImpl&) = delete;

  void Initialize(CefBrowserContext::Getter browser_context_getter,
                  CefRefPtr<CefCompletionCallback> callback);

  // Cache methods called before browser context initialized;
  void StoreOrTriggerInitCallback(base::OnceClosure callback);

  // CefAdsBlockManager methods.
  void SetAdsBlockRules(const CefString& rulesFile, bool replace) override;

  void AddAdsBlockDisallowedList(
      const std::vector<CefString>& domainSuffixes) override;

  void RemoveAdsBlockDisallowedList(
      const std::vector<CefString>& domainSuffixes) override;

  void ClearAdsBlockDisallowedList() override;

  void AddAdsBlockAllowedList(
      const std::vector<CefString>& domainSuffixes) override;

  void RemoveAdsBlockAllowedList(
      const std::vector<CefString>& domainSuffixes) override;

  void ClearAdsBlockAllowedList() override;

 private:
  bool ValidContext() const;

  void SetAdsBlockRulesInternal(const CefString& rulesFile, bool replace);

  void AddAdsBlockDisallowedListInternal(
      const std::vector<CefString>& domainSuffixes);

  void RemoveAdsBlockDisallowedListInternal(
      const std::vector<CefString>& domainSuffixes);

  void ClearAdsBlockDisallowedListInternal();

  void AddAdsBlockAllowedListInternal(
      const std::vector<CefString>& domainSuffixes);

  void RemoveAdsBlockAllowedListInternal(
      const std::vector<CefString>& domainSuffixes);

  void ClearAdsBlockAllowedListInternal();

  // Only accessed on the UI thread. Will be non-null after Initialize().
  CefBrowserContext::Getter browser_context_getter_;

  bool initialized_ = false;
  std::vector<base::OnceClosure> init_callbacks_;

  IMPLEMENT_REFCOUNTING(CefAdsBlockManagerImpl);
};
#endif  // CEF_LIBCEF_BROWSER_ADSBLOCK_MANAGER_IMPL_H_
