#ifndef COOKIE_MANAGER_IMPL_EXT_H_
#define COOKIE_MANAGER_IMPL_EXT_H_
#pragma once
#include "base/feature_list.h"
#include "base/files/file_path.h"
#include "base/threading/thread.h"
#include "cef/libcef/browser/net_service/cookie_manager_impl.h"
#include "include/cef_cookie.h"
#include "libcef/browser/browser_context.h"
#include "libcef/browser/thread_util.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "net/cookies/cookie_store.h"
#include "services/network/cookie_manager.h"
#include "services/network/public/mojom/cookie_manager.mojom.h"
#include "arkweb/build/features/features.h"
#if BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "components/content_settings/core/browser/host_content_settings_map.h"
#endif

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#include <mutex>
#include "services/network/public/cpp/cors/origin_access_list.h"
#endif

BASE_FEATURE(kArkwebLoadCookiesOnAsyncThread,
             "ArkwebLoadCookiesOnAsyncThread",
             base::FEATURE_ENABLED_BY_DEFAULT);

class CefCookieManagerImplExt : public CefCookieManagerImpl,
                                public CefCookieManagerExt {
 public:
  std::atomic<bool> allow_file_scheme_cookies_ = false;
  bool IsAcceptCookieAllowed() override;
  void PutAcceptCookieEnabled(bool accept) override;
  bool IsThirdPartyCookieAllowed() override;
  void PutAcceptThirdPartyCookieEnabled(bool accept) override;
  bool IsFileURLSchemeCookiesAllowed() override;
  void PutAcceptFileURLSchemeCookiesEnabled(bool allow) override;
  CefRefPtr<CefCookieManagerExt> AsCefCookieManagerExt() override {
    return this;
  }

  bool PutAcceptThirdPartyCookieEnabledInternal(bool accept);

  void SetNetWorkCookieManagerRemoteComplete(
      mojo::PendingRemote<network::mojom::CookieManager> cookie_manager_remote,
      base::OnceClosure complete);
  void SetNetWorkCookieManagerRemoteAsync(
      mojo::PendingRemote<network::mojom::CookieManager> cookie_manager_remote,
      base::OnceClosure complete);

  void SetNetWorkCookieManager(
      mojo::PendingRemote<network::mojom::CookieManager> cookie_manager_remote);

  net::CookieStore* GetCookieStore();

  bool FlushStore(CefRefPtr<CefCompletionCallback> callback) override;

  bool FlushStoreInternal(CefRefPtr<CefCompletionCallback> callback);

  CefCookieManagerImplExt(bool support_incognito = false);

  bool VisitAllCookies(CefRefPtr<CefCookieVisitor> visitor,
                       bool is_sync) override;

  bool VisitUrlCookies(const CefString& url,
                       bool includeHttpOnly,
                       CefRefPtr<CefCookieVisitor> visitor,
                       bool is_sync,
                       bool is_from_ndk) override;

  bool SetCookie(const CefString& url,
                 const CefCookie& cookie,
                 CefRefPtr<CefSetCookieCallback> callback,
                 bool is_sync,
                 const CefString& str_cookie,
                 bool includeHttpOnly) override;

  bool DeleteCookies(const CefString& url,
                     const CefString& cookie_name,
                     bool is_session,
                     CefRefPtr<CefDeleteCookiesCallback> callback,
                     bool is_sync) override;

  static CefRefPtr<CefCookieManagerImplExt> GetInstance(bool support_incognito);

  void LoadCookiesCallback(net::CookieStore::GetCookieListCallback callback,
                           const net::CookieAccessResultList& include_cookies,
                           const net::CookieAccessResultList&);

  // If LoadCookiesOnAsyncThread is not support, false will return.
  void LoadCookiesOnAsyncThread(
      const GURL& url,
      const net::CookieOptions& options,
      net::CookiePartitionKeyCollection cookie_partition_key_collection,
      net::CookieStore::GetCookieListCallback callback);

void SaveCookiesOnAsyncThread(
    const GURL& url,
    const net::CookieOptions& options,
    int total_count,
    net::CookieList cookies,
    base::OnceCallback<void(int, net::CookieList)> done_callback);

  bool SupportAsyncThreadCookieLoad();

#if BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
bool CanSaveOrLoadCookies(const network::ResourceRequest& request);
void UpdateHostContentSettingsMap();
#endif

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  void SetOriginAccessListForOrigin(
      const url::Origin& source_origin,
      std::vector<network::mojom::CorsOriginPatternPtr> allow_patterns,
      std::vector<network::mojom::CorsOriginPatternPtr> block_patterns);
  bool ShouldForceIgnoreSiteForCookies(const network::ResourceRequest& request);
#endif

 private:
  void RunCookieTaskSync(base::OnceCallback<void(base::OnceClosure)> task);
  void RunCookieTaskSync(
      base::OnceCallback<void(base::OnceCallback<void(bool)>)> task,
      const CefRefPtr<CefSetCookieCallback>& callback);
  void RunCookieTaskSync(
      base::OnceCallback<void(base::OnceCallback<void(uint32_t)>)> task,
      const CefRefPtr<CefDeleteCookiesCallback>& callback);
  void RunCookieTasks(base::OnceClosure task);
  void RunCookieTaskAsync(base::OnceClosure task);
  network::mojom::CookieManager* GetNetworkCookieManager();
  void Initialize(CefBrowserContext::Getter browser_context_getter,
                  CefRefPtr<CefCompletionCallback> callback);
  void PutAcceptFileURLSchemeCookiesEnabledCompleted(bool allow,
                                                     bool can_change_schemes);
  void PutAcceptFileURLSchemeCookiesEnabledInternal(bool allow);
  void GetAllCookieListCompleted(CefRefPtr<CefCookieVisitor> visitor,
                                 network::mojom::CookieManager* cookie_manager,
                                 base::OnceClosure complete,
                                 const net::CookieList& cookies);
  void GetAllCookieListCookieTask(CefRefPtr<CefCookieVisitor> visitor,
                                  base::OnceClosure complete);
  bool VisitAllCookiesInternal(CefRefPtr<CefCookieVisitor> visitor,
                               bool is_sync);
  void GetCookieListCookieTask(const GURL& url,
                               net::CookieOptions options,
                               CefRefPtr<CefCookieVisitor> visitor,
                               base::OnceClosure complete);
  bool VisitUrlCookiesInternal(const GURL& url,
                               bool includeHttpOnly,
                               CefRefPtr<CefCookieVisitor> visitor,
                               bool is_sync,
                               bool is_from_ndk);
  void SetCookieInternalCookieTask(std::unique_ptr<net::CanonicalCookie> cookie,
                                   const GURL& url,
                                   const net::CookieOptions& options,
                                   base::OnceCallback<void(bool)> complete);
  bool SetCookieInternal(const GURL& url,
                         const CefCookie& cookie,
                         CefRefPtr<CefSetCookieCallback> callback,
                         bool is_sync,
                         const CefString& str_cookie,
                         bool includeHttpOnly);
  void DeleteCookiesInternalCookieTask(
      network::mojom::CookieDeletionFilterPtr filter,
      base::OnceCallback<void(uint32_t)> complete);
  bool DeleteCookiesInternal(const GURL& url,
                             const CefString& cookie_name,
                             bool is_session,
                             CefRefPtr<CefDeleteCookiesCallback> callback,
                             bool is_sync);
  void FlushStoreInternalCookieTask(base::OnceClosure complete);
  void SetCookieCallbackImpl(CefRefPtr<CefSetCookieCallback> callback,
                             bool access_result);
  void RunAsyncCompletionOnTaskRunner(
      CefRefPtr<CefCompletionCallback> callback);
  void GetAllCookiesCallbackImpl(CefRefPtr<CefCookieVisitor> visitor,
                                 base::OnceClosure complete,
                                 network::mojom::CookieManager* cookie_manager,
                                 const net::CookieList& cookies);
  void GetCookiesCallbackImpl(
      CefRefPtr<CefCookieVisitor> visitor,
      base::OnceClosure complete,
      network::mojom::CookieManager* cookie_manager,
      const net::CookieAccessResultList& include_cookies,
      const net::CookieAccessResultList&);
  void DeleteCookiesCallbackImpl(CefRefPtr<CefDeleteCookiesCallback> callback,
                                 uint32_t num_deleted);
  scoped_refptr<base::SingleThreadTaskRunner> cookie_store_task_runner_;
  mojo::Remote<network::mojom::CookieManager> network_cookie_manager_;
  bool support_incognito_;
  bool cookie_store_created_;
  bool setting_network_cookie_manager_;
  std::unique_ptr<net::CookieStore> cookie_store_;
  base::FilePath cookie_store_path_;
  std::queue<base::OnceClosure> tasks_;
  base::Thread cookie_store_task_thread_;
  base::Thread cookie_store_backend_thread_;
  mutable bool remote_network_cookie_manager_inited_{false};
#if BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
  HostContentSettingsMap* host_content_settings_map_ = nullptr;
#endif
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  network::cors::OriginAccessList origin_access_list_;
  std::mutex origin_access_list_mutex_;
#endif
};
#endif  // COOKIE_MANAGER_IMPL_EXT_H_
