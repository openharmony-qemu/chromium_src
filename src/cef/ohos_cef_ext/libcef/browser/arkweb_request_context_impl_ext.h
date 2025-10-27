// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef ARKWEB_REQUEST_CONTEXT_IMPL_EXT_H_
#define ARKWEB_REQUEST_CONTEXT_IMPL_EXT_H_
#pragma once

#include "base/memory/raw_ptr.h"
#include "cef/include/cef_request_context.h"
#include "cef/libcef/browser/browser_context.h"
#include "cef/libcef/browser/media_router/media_router_impl.h"
#include "cef/libcef/browser/net_service/cookie_manager_impl.h"
#include "cef/libcef/browser/thread_util.h"
#include "cef/ohos_cef_ext/libcef/browser/adsblock_manager_impl.h"
#include "libcef/browser/net_database/cef_data_base_impl.h"

#if BUILDFLAG(ARKWEB_WEBSTORAGE)
#include "cef/ohos_cef_ext/libcef/browser/storage/web_storage_impl.h"
#endif

#include "cef/libcef/browser/request_context_impl.h"

class CefBrowserContext;

// Implementation of the CefRequestContextImpl interface. All methods are thread-
// safe unless otherwise indicated. Will be deleted on the UI thread.
class ArkWebRequestContextImplExt : public CefRequestContextImpl {
 public:
  ArkWebRequestContextImplExt(CefRequestContextImpl::Config&& config);
  ArkWebRequestContextImplExt(const ArkWebRequestContextImplExt&) = delete;
  ArkWebRequestContextImplExt& operator=(const ArkWebRequestContextImplExt&) = delete;
  ~ArkWebRequestContextImplExt() override;

  ArkWebRequestContextImplExt *AsArkWebRequestContextImpl() override { return this; }

#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
  // Creates the singleton global RequestContext in incognito mode. Called
  // from AlloyBrowserMainParts::PreMainMessageLoopRun.
  static CefRefPtr<CefRequestContextImpl> CreateGlobalOTRRequestContext(
      const CefRequestContextSettings& settings);
#endif

#if BUILDFLAG(ARKWEB_COOKIE)
  CefRefPtr<CefCookieManagerExt> GetCookieManagerExt(
      bool support_incognito,
      CefRefPtr<CefCompletionCallback> callback) override;
#endif  // BUILDFLAG(ARKWEB_COOKIE)
  CefRefPtr<CefAdsBlockManager> GetAdsBlockManager(
      CefRefPtr<CefCompletionCallback> callback) override;
  CefRefPtr<CefDataBase> GetDataBase() override;
#if BUILDFLAG(ARKWEB_CERT_AUTHENTICATION)
  void ClearClientAuthenticationCache(
      CefRefPtr<CefCompletionCallback> callback) override;
#endif  // ARKWEB_CERT_AUTHENTICATION
#if BUILDFLAG(ARKWEB_WEBSTORAGE)
  CefRefPtr<CefWebStorage> GetWebStorage(
      CefRefPtr<CefCompletionCallback> callback) override;
#endif

 private:
  friend class CefRequestContext;

#if BUILDFLAG(ARKWEB_CERT_AUTHENTICATION)
  void ClearClientAuthenticationCacheInternal(
      CefRefPtr<CefCompletionCallback> callback,
      CefBrowserContext::Getter browser_context_getter);
#endif  // ARKWEB_CERT_AUTHENTICATION

#if BUILDFLAG(ARKWEB_WEBSTORAGE)
  void InitializeWebStorageInternal(CefRefPtr<CefWebStorageImpl> web_storage,
                                    CefRefPtr<CefCompletionCallback> callback);
#endif

  void InitializeAdsBlockManagerInternal(
      CefRefPtr<CefAdsBlockManagerImpl> adsblock_manager,
      CefRefPtr<CefCompletionCallback> callback);
};

#endif  // ARKWEB_REQUEST_CONTEXT_IMPL_EXT_H_
