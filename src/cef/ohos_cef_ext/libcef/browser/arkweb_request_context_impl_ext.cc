// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "cef/libcef/browser/request_context_impl.h"
#include "cef/ohos_cef_ext/libcef/browser/arkweb_request_context_impl_ext.h"

#include "arkweb/build/features/features.h"
#include "base/atomic_sequence_num.h"
#include "base/logging.h"
#include "base/strings/stringprintf.h"
#include "cef/libcef/browser/browser_context.h"
#include "cef/libcef/browser/context.h"
#include "cef/libcef/browser/prefs/pref_helper.h"
#include "cef/libcef/browser/thread_util.h"
#include "cef/libcef/common/app_manager.h"
#include "cef/libcef/common/task_runner_impl.h"
#include "cef/libcef/common/values_impl.h"
#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/themes/theme_service.h"
#include "chrome/browser/themes/theme_service_factory.h"
#include "components/content_settings/core/browser/host_content_settings_map.h"
#include "components/prefs/pref_service.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/child_process_host.h"
#include "content/public/browser/ssl_host_state_delegate.h"
#include "libcef/browser/alloy/alloy_client_cert_lookup_table.h"
#include "libcef/browser/net_service/cookie_manager_impl_ext.h"
#include "mojo/public/cpp/bindings/receiver.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "net/cert/cert_database.h"
#include "net/dns/host_resolver.h"
#include "services/network/public/cpp/resolve_host_client_base.h"
#include "services/network/public/mojom/clear_data_filter.mojom.h"
#include "services/network/public/mojom/network_context.mojom.h"
#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
#include "cef/ohos_cef_ext/libcef/browser/net_database/cef_incognito_data_base_impl.h"
#endif

using content::BrowserThread;

namespace {

#if BUILDFLAG(ARKWEB_CERT_AUTHENTICATION)
void NotifyClientCertificatesChanged() {
  LOG(INFO) << "CefRequestContextImpl::NotifyClientCertificatesChanged";
  net::CertDatabase::GetInstance()->NotifyObserversClientCertStoreChanged();
}
#endif  // ARKWEB_CERT_AUTHENTICATION

}  // namespace

#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
// static
CefRefPtr<CefRequestContext> CefRequestContext::GetGlobalOTRContext() {
  // Verify that the context is in a valid state.
  if (!CONTEXT_STATE_VALID()) {
    NOTREACHED() << "context not valid";
  }

  CefRequestContextImpl::Config config;
  config.is_global = true;
  config.incognito_mode = true;
  return CefRequestContextImpl::GetOrCreateRequestContext(std::move(config));
}
#endif

// CefRequestContextImpl

ArkWebRequestContextImplExt::ArkWebRequestContextImplExt(
    CefRequestContextImpl::Config&& config)
    : CefRequestContextImpl(std::move(config)) {}

ArkWebRequestContextImplExt::~ArkWebRequestContextImplExt() = default;

#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
// static
CefRefPtr<CefRequestContextImpl> ArkWebRequestContextImplExt::CreateGlobalOTRRequestContext(
    const CefRequestContextSettings& settings) {
  Config config;
  config.is_global = true;
  config.settings = settings;
  config.incognito_mode = true;
  CefRefPtr<CefRequestContextImpl> impl =
      new ArkWebRequestContextImplExt(std::move(config));
  impl->Initialize();
  return impl;
}
#endif

#if BUILDFLAG(ARKWEB_COOKIE)
CefRefPtr<CefCookieManagerExt> ArkWebRequestContextImplExt::GetCookieManagerExt(
    bool support_incognito,
    CefRefPtr<CefCompletionCallback> callback) {
  CefRefPtr<CefCookieManagerImplExt> cookie_manager =
      CefCookieManagerImplExt::GetInstance(support_incognito);
  InitializeCookieManagerInternal(cookie_manager, callback);
  return cookie_manager.get();
}
#endif  // BUILDFLAG(ARKWEB_COOKIE)

CefRefPtr<CefAdsBlockManager> ArkWebRequestContextImplExt::GetAdsBlockManager(
    CefRefPtr<CefCompletionCallback> callback) {
  CefRefPtr<CefAdsBlockManagerImpl> adsblock_manager =
      CefAdsBlockManagerImpl::GetInstance();
  InitializeAdsBlockManagerInternal(adsblock_manager, callback);
  return adsblock_manager;
}

void ArkWebRequestContextImplExt::InitializeAdsBlockManagerInternal(
    CefRefPtr<CefAdsBlockManagerImpl> adsblock_manager,
    CefRefPtr<CefCompletionCallback> callback) {
  GetBrowserContext(content::GetUIThreadTaskRunner({}),
                    base::BindOnce(
                        [](CefRefPtr<CefAdsBlockManagerImpl> adsblock_manager,
                           CefRefPtr<CefCompletionCallback> callback,
                           CefBrowserContext::Getter browser_context_getter) {
                          adsblock_manager->Initialize(browser_context_getter,
                                                       callback);
                        },
                        adsblock_manager, callback));
}

CefRefPtr<CefDataBase> ArkWebRequestContextImplExt::GetDataBase() {
#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
  if (config_.incognito_mode) {
    CefRefPtr<CefIncognitoDataBaseImpl> data_base =
        new CefIncognitoDataBaseImpl();
    return data_base;
  } else {
    CefRefPtr<CefDataBaseImpl> data_base = new CefDataBaseImpl();
    return data_base;
  }
#else
  CefRefPtr<CefDataBaseImpl> data_base = new CefDataBaseImpl();
  return data_base;
#endif
}

#if BUILDFLAG(ARKWEB_CERT_AUTHENTICATION)
void ArkWebRequestContextImplExt::ClearClientAuthenticationCache(
    CefRefPtr<CefCompletionCallback> callback) {
  LOG(INFO) << "ArkWebRequestContextImplExt::ClearClientAuthenticationCache";
  GetBrowserContext(
      content::GetUIThreadTaskRunner({}),
      base::BindOnce(
          &ArkWebRequestContextImplExt::ClearClientAuthenticationCacheInternal, this,
          callback));
}
#endif  // ARKWEB_CERT_AUTHENTICATION

#if BUILDFLAG(ARKWEB_CERT_AUTHENTICATION)
void ArkWebRequestContextImplExt::ClearClientAuthenticationCacheInternal(
    CefRefPtr<CefCompletionCallback> callback,
    CefBrowserContext::Getter browser_context_getter) {
  LOG(INFO) << "ArkWebRequestContextImplExt::ClearClientAuthenticationCacheInternal";
  auto browser_context = browser_context_getter.Run();
  if (!browser_context) {
    return;
  }

  AlloyClientCertLookupTable::Clear();

  if (content::BrowserThread::CurrentlyOn(content::BrowserThread::IO)) {
    NotifyClientCertificatesChanged();
  } else {
    content::GetIOThreadTaskRunner({})->PostTask(
        FROM_HERE, base::BindOnce(&NotifyClientCertificatesChanged));
  }

  if (callback) {
    CEF_POST_TASK(CEF_UIT,
                  base::BindOnce(&CefCompletionCallback::OnComplete, callback));
  }
}
#endif  // ARKWEB_CERT_AUTHENTICATION

#if BUILDFLAG(ARKWEB_WEBSTORAGE)
CefRefPtr<CefWebStorage> ArkWebRequestContextImplExt::GetWebStorage(
    CefRefPtr<CefCompletionCallback> callback) {
  CefRefPtr<CefWebStorageImpl> web_storage = new CefWebStorageImpl();
  InitializeWebStorageInternal(web_storage, callback);
  return web_storage;
}
void ArkWebRequestContextImplExt::InitializeWebStorageInternal(
    CefRefPtr<CefWebStorageImpl> web_storage,
    CefRefPtr<CefCompletionCallback> callback) {
  GetBrowserContext(content::GetUIThreadTaskRunner({}),
                    base::BindOnce(
                        [](CefRefPtr<CefWebStorageImpl> web_storage,
                           CefRefPtr<CefCompletionCallback> callback,
                           CefBrowserContext::Getter browser_context_getter) {
                          web_storage->Initialize(browser_context_getter,
                                                  callback);
                        },
                        web_storage, callback));
}
#endif  // #if BUILDFLAG(ARKWEB_WEBSTORAGE)
