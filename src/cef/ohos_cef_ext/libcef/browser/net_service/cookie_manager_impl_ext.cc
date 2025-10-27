#include "libcef/browser/net_service/cookie_manager_impl_ext.h"

#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "base/command_line.h"
#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/ohos/sys_info_utils_ext.h"
#include "base/path_service.h"
#include "base/synchronization/waitable_event.h"
#include "base/threading/thread_restrictions.h"
#include "cc/base/completion_event.h"
#include "cef/include/base/cef_callback_helpers.h"
#include "cef/libcef/browser/net_service/cookie_helper.h"
#include "cef/libcef/browser/net_service/cookie_manager_impl.h"
#include "cef/libcef/common/time_util.h"
#include "cef/ohos_cef_ext/libcef/browser/net_service/net_helpers.h"
#include "chrome/browser/profiles/profile.h"
#include "components/cookie_config/cookie_store_util.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/cookie_store_factory.h"
#include "content/public/browser/storage_partition.h"
#include "content/public/common/content_switches.h"
#include "libcef/browser/request_context_impl.h"
#include "libcef/common/net_service/net_service_util.h"
#include "net/cookies/cookie_monster.h"
#include "net/cookies/cookie_util.h"
#include "services/network/cookie_access_delegate_impl.h"
#include "services/network/cookie_manager.h"
#if BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST) || BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#include "services/network/public/cpp/resource_request.h"
#endif
#include "services/network/public/mojom/cookie_manager.mojom.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "url/gurl.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#if BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
#include "base/command_line.h"
#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "components/content_settings/core/browser/host_content_settings_map.h"
#include "content/public/common/content_switches.h"
#endif  // BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)

#if BUILDFLAG(ARKWEB_COOKIE)
#include "cef/ohos_cef_ext/libcef/common/net_service/net_service_util_ext.h"
#endif // BUILDFLAG(ARKWEB_COOKIE)

#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
#include "arkweb/chromium_ext/base/process/process_handle_posix_ex.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/res_sched_client_adapter.h"
#endif // BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)

using network::mojom::CookieManager;

namespace {
base::Lock g_lock;

struct SaveCookiesProgress {
  base::OnceCallback<void(int, net::CookieList)> done_callback_;
  int total_count_;
  net::CookieList allowed_cookies_;
  int num_cookie_lines_left_;
};

// Do not keep a reference to the object returned by this method.
CefBrowserContext* GetBrowserContext(const CefBrowserContext::Getter& getter) {
  CEF_REQUIRE_UIT();

  DCHECK(!getter.is_null());

  // Will return nullptr if the BrowserContext has been shut down.
  return getter.Run();
}

// Always execute the callback asynchronously.
void RunAsyncCompletionOnUIThreadExt(
    CefRefPtr<CefCompletionCallback> callback) {
  if (!callback.get()) {
    return;
  }
  CEF_POST_TASK(CEF_UIT, base::BindOnce(&CefCompletionCallback::OnComplete,
                                        callback.get()));
}

void DeleteCanonicalCookie(CookieManager* cookie_manager,
                           const net::CanonicalCookie& cookie,
                           base::OnceCallback<void(bool)> callback) {
  if (!cookie_manager) {
    if (!callback.is_null()) {
      std::move(callback).Run(false);
    }
    return;
  }
  cookie_manager->DeleteCanonicalCookie(cookie, std::move(callback));
}

void ExecuteVisitor(CefRefPtr<CefCookieVisitor> visitor,
                    CookieManager* cookie_manager,
                    const std::vector<net::CanonicalCookie>& cookies) {
  int total = cookies.size(), count = 0;
  if (total == 0) {
    CefCookie cookie;
    bool deleteCookie = false;
    visitor->Visit(cookie, 0, 0, deleteCookie);
    return;
  }

  for (const auto& cc : cookies) {
    CefCookie cookie;
    net_service::MakeCefCookie(cc, cookie);

    bool deleteCookie = false;
    bool keepLooping = visitor->Visit(cookie, count, total, deleteCookie);
    if (deleteCookie) {
      DeleteCanonicalCookie(cookie_manager, cc,
                            CookieManager::DeleteCanonicalCookieCallback());
    }
    if (!keepLooping) {
      break;
    }
    count++;
  }
  std::string cookie_line = net::CanonicalCookie::BuildCookieLine(cookies);
  visitor->SetCookieLine(CefString(cookie_line));
}

bool FixInvalidGurl(const CefString& url, GURL& gurl) {
  if (!gurl.is_valid()) {
    GURL fixedGurl = GURL("https://" + url.ToString());
    if (fixedGurl.is_valid() && fixedGurl.host() == url.ToString()) {
      gurl = fixedGurl;
      return true;
    }
    return false;
  }
  return true;
}

base::OnceClosure SignalEventClosure(base::WaitableEvent* completion) {
  return base::BindOnce(&base::WaitableEvent::Signal,
                        base::Unretained(completion));
}

void DiscardBool(base::OnceClosure f,
                 CefRefPtr<CefSetCookieCallback> callback,
                 bool b) {
  std::move(f).Run();
  if (callback) {
    callback->OnComplete(b);
  }
}

base::OnceCallback<void(bool)> OnceClosureToBoolCallback(
    base::OnceClosure f,
    const CefRefPtr<CefSetCookieCallback>& callback) {
  return base::BindOnce(&DiscardBool, std::move(f), callback);
}

void DiscardUint32(base::OnceClosure f,
                   CefRefPtr<CefDeleteCookiesCallback> callback,
                   uint32_t b) {
  std::move(f).Run();
  if (callback) {
    callback->OnComplete(b);
  }
}

base::OnceCallback<void(uint32_t)> OnceClosureToUint32Callback(
    base::OnceClosure f,
    const CefRefPtr<CefDeleteCookiesCallback>& callback) {
  return base::BindOnce(&DiscardUint32, std::move(f), callback);
}

bool GetCookieAccessResultInclude(
    net::CookieAccessResult cookie_access_result) {
  const bool is_include = cookie_access_result.status.IsInclude();
  if (!is_include) {
    LOG(WARNING) << "SetCookie failed with reason: "
                 << cookie_access_result.status.GetDebugString();
  }
  return is_include;
}

void SetCanonicalCookieCallback(SaveCookiesProgress* progress,
                                const net::CanonicalCookie& cookie,
                                net::CookieAccessResult access_result) {
  progress->num_cookie_lines_left_--;
  if (access_result.status.IsInclude()) {
    progress->allowed_cookies_.push_back(cookie);
  }

  // If all the cookie lines have been handled the request can be continued.
  if (progress->num_cookie_lines_left_ == 0) {
    CEF_POST_TASK(CEF_IOT,
                  base::BindOnce(std::move(progress->done_callback_),
                                 progress->total_count_,
                                 std::move(progress->allowed_cookies_)));
    delete progress;
  }
}

}  // namespace

// Do not keep a reference to the object returned by this method.
CookieManager* GetCookieManager(CefBrowserContext* browser_context) {
  CEF_REQUIRE_UIT();
  return browser_context->AsBrowserContext()
      ->GetDefaultStoragePartition()
      ->GetCookieManagerForBrowserProcess();
}

bool CefCookieManagerImplExt::VisitAllCookies(
    CefRefPtr<CefCookieVisitor> visitor,
    bool is_sync) {
  if (!visitor.get()) {
    return false;
  }
  LOG(DEBUG) << "CefCookieManagerImpl::VisitAllCookies is_sync: " << is_sync;
  return VisitAllCookiesInternal(visitor, is_sync);
}

bool CefCookieManagerImplExt::VisitUrlCookies(
    const CefString& url,
    bool includeHttpOnly,
    CefRefPtr<CefCookieVisitor> visitor,
    bool is_sync,
    bool is_from_ndk) {
  if (!visitor.get()) {
    return false;
  }

  GURL gurl = GURL(url.ToString());
  if (!FixInvalidGurl(url, gurl)) {
    return false;
  }
  LOG(DEBUG) << "CefCookieManagerImpl::VisitUrlCookies is_sync: " << is_sync;
  return VisitUrlCookiesInternal(gurl, includeHttpOnly, visitor, is_sync,
                                 is_from_ndk);
}

bool CefCookieManagerImplExt::SetCookie(
    const CefString& url,
    const CefCookie& cookie,
    CefRefPtr<CefSetCookieCallback> callback,
    bool is_sync,
    const CefString& str_cookie,
    bool includeHttpOnly) {
  GURL gurl = GURL(url.ToString());
  if (!ValidContext()) {
    StoreOrTriggerInitCallback(base::BindOnce(
        base::IgnoreResult(&CefCookieManagerImplExt::SetCookieInternal), this,
        gurl, cookie, callback, is_sync, str_cookie, includeHttpOnly));
    return true;
  }

  return SetCookieInternal(gurl, cookie, callback, is_sync, str_cookie,
                           includeHttpOnly);
}

bool CefCookieManagerImplExt::DeleteCookies(
    const CefString& url,
    const CefString& cookie_name,
    bool is_session,
    CefRefPtr<CefDeleteCookiesCallback> callback,
    bool is_sync) {
  GURL gurl = GURL(url.ToString());
  if (!gurl.is_empty() && !gurl.is_valid()) {
    return false;
  }
  LOG(DEBUG) << "CefCookieManagerImpl::DeleteCookies is_sync: " << is_sync;
  return DeleteCookiesInternal(gurl, cookie_name, is_session, callback,
                               is_sync);
}

void CefCookieManagerImplExt::RunCookieTaskSync(
    base::OnceCallback<void(base::OnceClosure)> task) {
  base::WaitableEvent completion(
      base::WaitableEvent::ResetPolicy::AUTOMATIC,
      base::WaitableEvent::InitialState::NOT_SIGNALED);
  RunCookieTaskAsync(
      base::BindOnce(std::move(task), SignalEventClosure(&completion)));
  base::ScopedAllowBaseSyncPrimitivesOutsideBlockingScope wait;
  completion.Wait();
}

void CefCookieManagerImplExt::RunCookieTaskSync(
    base::OnceCallback<void(base::OnceCallback<void(bool)>)> task,
    const CefRefPtr<CefSetCookieCallback>& callback) {
  base::WaitableEvent completion(
      base::WaitableEvent::ResetPolicy::AUTOMATIC,
      base::WaitableEvent::InitialState::NOT_SIGNALED);
  RunCookieTaskAsync(base::BindOnce(
      std::move(task),
      OnceClosureToBoolCallback(SignalEventClosure(&completion), callback)));
  base::ScopedAllowBaseSyncPrimitivesOutsideBlockingScope wait;
  completion.Wait();
}

void CefCookieManagerImplExt::RunCookieTaskSync(
    base::OnceCallback<void(base::OnceCallback<void(uint32_t)>)> task,
    const CefRefPtr<CefDeleteCookiesCallback>& callback) {
  base::WaitableEvent completion(
      base::WaitableEvent::ResetPolicy::AUTOMATIC,
      base::WaitableEvent::InitialState::NOT_SIGNALED);
  RunCookieTaskAsync(base::BindOnce(
      std::move(task),
      OnceClosureToUint32Callback(SignalEventClosure(&completion), callback)));
  base::ScopedAllowBaseSyncPrimitivesOutsideBlockingScope wait;
  completion.Wait();
}

void CefCookieManagerImplExt::RunCookieTaskAsync(base::OnceClosure task) {
  cookie_store_task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&CefCookieManagerImplExt::RunCookieTasks,
                                base::Unretained(this), std::move(task)));
}

void CefCookieManagerImplExt::RunCookieTasks(base::OnceClosure task) {
  DCHECK(cookie_store_task_runner_->RunsTasksInCurrentSequence());
  if (!task.is_null()) {
    tasks_.push(std::move(task));
  }
  if (setting_network_cookie_manager_) {
    LOG(DEBUG) << "network service starting";
    return;
  }

  while (!tasks_.empty()) {
    auto t = std::move(tasks_.front());
    tasks_.pop();
    std::move(t).Run();
  }
}

net::CookieStore* CefCookieManagerImplExt::GetCookieStore() {
  DCHECK(cookie_store_task_runner_->RunsTasksInCurrentSequence());

  if (!cookie_store_) {
    base::FilePath cookie_store_path;
    bool persist_session_cookies = false;
    bool restore_old_session_cookies = false;
    if (!support_incognito_) {
      restore_old_session_cookies = true;
      cookie_store_path = cookie_store_path_;
      persist_session_cookies =
          !(base::CommandLine::ForCurrentProcess()->HasSwitch(
              switches::kCookieConfigPersistSession));
    }

    content::CookieStoreConfig cookie_config(cookie_store_path,
                                             restore_old_session_cookies,
                                             persist_session_cookies);
    cookie_config.client_task_runner = cookie_store_task_runner_;
    cookie_config.background_task_runner =
        cookie_store_backend_thread_.task_runner();
    cookie_config.crypto_delegate = cookie_config::GetCookieCryptoDelegate();

    cookie_config.cookieable_schemes.insert(
        cookie_config.cookieable_schemes.begin(),
        net::CookieMonster::kDefaultCookieableSchemes,
        net::CookieMonster::kDefaultCookieableSchemes +
            net::CookieMonster::kDefaultCookieableSchemesCount);
    if (allow_file_scheme_cookies_) {
      cookie_config.cookieable_schemes.push_back(url::kFileScheme);
    }
    cookie_store_created_ = true;

    cookie_store_ =
        content::CreateCookieStore(std::move(cookie_config), nullptr);
    auto cookie_access_delegate_type =
        network::mojom::CookieAccessDelegateType::ALWAYS_LEGACY;
    cookie_store_->SetCookieAccessDelegate(
        std::make_unique<network::CookieAccessDelegateImpl>(
            cookie_access_delegate_type, nullptr /* first_party_sets */));
  }

  return cookie_store_.get();
}

void CefCookieManagerImplExt::SetNetWorkCookieManagerRemoteComplete(
    mojo::PendingRemote<CookieManager> cookie_manager_remote,
    base::OnceClosure complete) {
  LOG(INFO) << "CefCookieManagerImplExt::SetNetWorkCookieManagerRemoteComplete";
  DCHECK(cookie_store_task_runner_->RunsTasksInCurrentSequence());
  setting_network_cookie_manager_ = false;
  network_cookie_manager_.Bind(std::move(cookie_manager_remote));
  if (!complete.is_null()) {
    std::move(complete).Run();
  }
  RunCookieTasks(base::NullCallback());
  remote_network_cookie_manager_inited_ = true;
}

void CefCookieManagerImplExt::SetNetWorkCookieManagerRemoteAsync(
    mojo::PendingRemote<CookieManager> cookie_manager_remote,
    base::OnceClosure complete) {
  LOG(INFO) << "CefCookieManagerImplExt::SetNetWorkCookieManagerRemoteAsync";
  DCHECK(cookie_store_task_runner_->RunsTasksInCurrentSequence());

  setting_network_cookie_manager_ = true;
  if (!cookie_store_created_) {
    SetNetWorkCookieManagerRemoteComplete(std::move(cookie_manager_remote),
                                          std::move(complete));
    return;
  }
  GetCookieStore()->FlushStore(base::BindOnce(
      &CefCookieManagerImplExt::SetNetWorkCookieManagerRemoteComplete,
      base::Unretained(this), std::move(cookie_manager_remote),
      std::move(complete)));
}

void CefCookieManagerImplExt::SetNetWorkCookieManager(
    mojo::PendingRemote<CookieManager> cookie_manager_remote) {
  LOG(INFO) << "CefCookieManagerImplExt::SetNetWorkCookieManager "
               "cookie_manager_remote";
  RunCookieTaskSync(base::BindOnce(
      &CefCookieManagerImplExt::SetNetWorkCookieManagerRemoteAsync,
      base::Unretained(this), std::move(cookie_manager_remote)));
}

// static
CefRefPtr<CefCookieManagerImplExt> CefCookieManagerImplExt::GetInstance(
    bool support_incognito) {
  base::AutoLock lock(g_lock);
  if (!support_incognito) {
    static CefRefPtr<CefCookieManagerImplExt> instance =
        new CefCookieManagerImplExt();
    return instance;
  } else {
    static CefRefPtr<CefCookieManagerImplExt> instance =
        new CefCookieManagerImplExt(support_incognito);
    return instance;
  }
}

network::mojom::CookieManager*
CefCookieManagerImplExt::GetNetworkCookieManager() {
  DCHECK(cookie_store_task_runner_->RunsTasksInCurrentSequence());
  if (!network_cookie_manager_.is_bound()) {
    return nullptr;
  }
  return network_cookie_manager_.get();
}

CefCookieManagerImplExt::CefCookieManagerImplExt(bool support_incognito)
    : support_incognito_(support_incognito),
      cookie_store_created_(false),
      setting_network_cookie_manager_(false),
      cookie_store_task_thread_("CookieMonsterClient"),
      cookie_store_backend_thread_("CookieMonsterBackend") {
  cookie_store_task_thread_.Start();
  cookie_store_backend_thread_.Start();
  cookie_store_task_runner_ = cookie_store_task_thread_.task_runner();

  base::PathService::Get(base::DIR_CACHE, &cookie_store_path_);
  cookie_store_path_ = cookie_store_path_.Append("Cookies");
#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  bool cmd_value = false;
  if (command_line) {
    cmd_value = command_line->HasSwitch(switches::kEnableReportCookieMonsterClient);
  }
  if (cmd_value) {
    OHOS::NWeb::ResSchedClientAdapter::ReportKeyThread(
      OHOS::NWeb::ResSchedStatusAdapter::THREAD_CREATED, base::GetCurrentRealPid(),
      cookie_store_task_thread_.GetThreadRealId(), OHOS::NWeb::ResSchedRoleAdapter::USER_INTERACT);
  }
#endif // BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
}

void CefCookieManagerImplExt::Initialize(
    CefBrowserContext::Getter browser_context_getter,
    CefRefPtr<CefCompletionCallback> callback) {
  CEF_REQUIRE_UIT();
  browser_context_getter_ = browser_context_getter;
  RunAsyncCompletionOnUIThreadExt(callback);
}

bool CefCookieManagerImplExt::IsAcceptCookieAllowed() {
  return net_service::NetHelpers::IsAllowAcceptCookies();
}

void CefCookieManagerImplExt::PutAcceptCookieEnabled(bool accept) {
  net_service::NetHelpers::accept_cookies = accept;
#if BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableNwebEx)) {
    if (browser_context_getter_) {
      auto cef_browser_context = browser_context_getter_.Run();
      if (cef_browser_context) {
        HostContentSettingsMap* host_content_settings_map =
            HostContentSettingsMapFactory::GetForProfile(
                cef_browser_context->AsBrowserContext());
        if (host_content_settings_map) {
          host_content_settings_map->SetDefaultContentSetting(
              ContentSettingsType::COOKIES,
              net_service::NetHelpers::IsAllowAcceptCookies()
                  ? ContentSetting::CONTENT_SETTING_ALLOW
                  : ContentSetting::CONTENT_SETTING_BLOCK);
        }
      }
    }
  }
#endif  // BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
}

bool CefCookieManagerImplExt::IsThirdPartyCookieAllowed() {
  return net_service::NetHelpers::IsThirdPartyCookieAllowed();
}

void CefCookieManagerImplExt::PutAcceptThirdPartyCookieEnabled(bool accept) {
  if (!ValidContext()) {
    StoreOrTriggerInitCallback(base::BindOnce(
        base::IgnoreResult(
            &CefCookieManagerImplExt::PutAcceptThirdPartyCookieEnabledInternal),
        this, accept));
    return;
  }

  PutAcceptThirdPartyCookieEnabledInternal(accept);
}

bool CefCookieManagerImplExt::IsFileURLSchemeCookiesAllowed() {
  return allow_file_scheme_cookies_;
}

void CefCookieManagerImplExt::PutAcceptFileURLSchemeCookiesEnabled(bool allow) {
  LOG(INFO)
      << "CefCookieManagerImplExt::PutAcceptFileURLSchemeCookiesEnabled allow: "
      << allow;
  PutAcceptFileURLSchemeCookiesEnabledInternal(allow);
}

bool CefCookieManagerImplExt::PutAcceptThirdPartyCookieEnabledInternal(
    bool accept) {
  DCHECK(ValidContext());
  auto browser_context = GetBrowserContext(browser_context_getter_);
  if (!browser_context) {
    return false;
  }
  LOG(INFO) << __func__ << " accept: " << accept;
  net_service::NetHelpers::third_party_cookies = accept;
  GetCookieManager(browser_context)->BlockThirdPartyCookies(!accept);
  return true;
}

void CefCookieManagerImplExt::PutAcceptFileURLSchemeCookiesEnabledCompleted(
    bool allow,
    bool can_change_schemes) {
  if (can_change_schemes) {
    allow_file_scheme_cookies_ = allow;
  }
}

void CefCookieManagerImplExt::PutAcceptFileURLSchemeCookiesEnabledInternal(
    bool allow) {
  if (!cookie_store_task_runner_->RunsTasksInCurrentSequence()) {
    cookie_store_task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(&CefCookieManagerImplExt::
                           PutAcceptFileURLSchemeCookiesEnabledInternal,
                       base::Unretained(this), allow));
    return;
  }
  DCHECK(cookie_store_task_runner_->RunsTasksInCurrentSequence());
  auto cookie_manager = GetNetworkCookieManager();
  if (!cookie_manager) {
    LOG(INFO) << "network service not started";
    bool can_change_schemes = !cookie_store_created_;
    PutAcceptFileURLSchemeCookiesEnabledCompleted(allow, can_change_schemes);
    return;
  }
  cookie_manager->AllowFileSchemeCookies(
      allow, base::BindOnce(&CefCookieManagerImplExt::
                                PutAcceptFileURLSchemeCookiesEnabledCompleted,
                            base::Unretained(this), allow));
}

void CefCookieManagerImplExt::GetAllCookieListCompleted(
    CefRefPtr<CefCookieVisitor> visitor,
    CookieManager* cookie_manager,
    base::OnceClosure complete,
    const net::CookieList& cookies) {
  DCHECK(cookie_store_task_runner_->RunsTasksInCurrentSequence());
  ExecuteVisitor(visitor, cookie_manager, cookies);
  if (!complete.is_null()) {
    std::move(complete).Run();
  }
}

void CefCookieManagerImplExt::GetAllCookieListCookieTask(
    CefRefPtr<CefCookieVisitor> visitor,
    base::OnceClosure complete) {
  LOG(DEBUG) << "CefCookieManagerImplExt::GetAllCookieListCookieTask";
  DCHECK(cookie_store_task_runner_->RunsTasksInCurrentSequence());

  CookieManager* cookie_manager = GetNetworkCookieManager();
  if (!cookie_manager) {
    GetCookieStore()->GetAllCookiesAsync(base::BindOnce(
        &CefCookieManagerImplExt::GetAllCookieListCompleted,
        base::Unretained(this), visitor, nullptr, std::move(complete)));
    return;
  }
  cookie_manager->GetAllCookies(base::BindOnce(
      &CefCookieManagerImplExt::GetAllCookieListCompleted,
      base::Unretained(this), visitor, cookie_manager, std::move(complete)));
}

bool CefCookieManagerImplExt::VisitAllCookiesInternal(
    CefRefPtr<CefCookieVisitor> visitor,
    bool is_sync) {
  DCHECK(visitor);
  LOG(DEBUG) << "CefCookieManagerImplExt::VisitAllCookiesInternal is_sync : "
             << is_sync;
  if (is_sync) {
    RunCookieTaskSync(
        base::BindOnce(&CefCookieManagerImplExt::GetAllCookieListCookieTask,
                       base::Unretained(this), visitor));
  } else {
    RunCookieTaskAsync(
        base::BindOnce(&CefCookieManagerImplExt::GetAllCookieListCookieTask,
                       base::Unretained(this), visitor, base::NullCallback()));
  }
  return true;
}

void CefCookieManagerImplExt::GetCookieListCookieTask(
    const GURL& url,
    net::CookieOptions options,
    CefRefPtr<CefCookieVisitor> visitor,
    base::OnceClosure complete) {
  LOG(DEBUG) << "CefCookieManagerImplExt::GetCookieListCookieTask";
  DCHECK(cookie_store_task_runner_->RunsTasksInCurrentSequence());

  CookieManager* cookie_manager = GetNetworkCookieManager();
  if (!cookie_manager) {
    GetCookieStore()->GetCookieListWithOptionsAsync(
        url, options, net::CookiePartitionKeyCollection(),
        base::BindOnce(&CefCookieManagerImplExt::GetCookiesCallbackImpl,
                       base::Unretained(this), visitor, std::move(complete),
                       nullptr));
    return;
  }

  cookie_manager->GetCookieList(
      url, options, net::CookiePartitionKeyCollection(),
      base::BindOnce(&CefCookieManagerImplExt::GetCookiesCallbackImpl,
                     base::Unretained(this), visitor, std::move(complete),
                     cookie_manager));
}

bool CefCookieManagerImplExt::VisitUrlCookiesInternal(
    const GURL& url,
    bool includeHttpOnly,
    CefRefPtr<CefCookieVisitor> visitor,
    bool is_sync,
    bool is_from_ndk) {
  DCHECK(visitor);
  DCHECK(url.is_valid());

  net::CookieOptions options = net::CookieOptions::MakeAllInclusive();
  if (is_from_ndk && !includeHttpOnly) {
    options.set_exclude_httponly();
  }
  LOG(DEBUG) << "CefCookieManagerImplExt::VisitUrlCookiesInternal is_sync : "
             << is_sync;
  if (is_sync) {
    RunCookieTaskSync(base::BindOnce(
        &CefCookieManagerImplExt::GetCookieListCookieTask,
        base::Unretained(this), std::move(url), options, visitor));
  } else {
    RunCookieTaskAsync(
        base::BindOnce(&CefCookieManagerImplExt::GetCookieListCookieTask,
                       base::Unretained(this), std::move(url), options, visitor,
                       base::NullCallback()));
  }
  return true;
}

void CefCookieManagerImplExt::SetCookieInternalCookieTask(
    std::unique_ptr<net::CanonicalCookie> cookie,
    const GURL& url,
    const net::CookieOptions& options,
    base::OnceCallback<void(bool)> complete) {
  LOG(DEBUG) << "CefCookieManagerImplExt::SetCookieInternalCookieTask";
  DCHECK(cookie_store_task_runner_->RunsTasksInCurrentSequence());
  if (!cookie) {
    if (!complete.is_null()) {
      std::move(complete).Run(false);
    }
    return;
  }

  CookieManager* cookie_manager = GetNetworkCookieManager();
  if (!cookie_manager) {
    GetCookieStore()->SetCanonicalCookieAsync(
        std::move(cookie), url, options,
        base::BindOnce(GetCookieAccessResultInclude).Then(std::move(complete)));
    return;
  }
  cookie_manager->SetCanonicalCookie(
      *cookie, url, options,
      base::BindOnce(GetCookieAccessResultInclude).Then(std::move(complete)));
}

bool CefCookieManagerImplExt::SetCookieInternal(
    const GURL& url,
    const CefCookie& cookie,
    CefRefPtr<CefSetCookieCallback> callback,
    bool is_sync,
    const CefString& str_cookie,
    bool includeHttpOnly) {
  DCHECK(url.is_valid());

  std::unique_ptr<net::CanonicalCookie> canonical_cookie(
      net::CanonicalCookie::Create(
          url, str_cookie.ToString(), base::Time::Now(),
          std::nullopt /* server_time */,
          net::CookiePartitionKey::FromWire(
              net::SchemefulSite(url),
              net::CookiePartitionKey::AncestorChainBit::
                  kSameSite /* 132 new element */,
              std::nullopt),
#if BUILDFLAG(ARKWEB_COOKIE)
      net::CookieSourceType::kOther, /*status=*/nullptr, false));
#else // BUILDFLAG(ARKWEB_COOKIE)
      net::CookieSourceType::kOther, /*status=*/nullptr));
#endif // BUILDFLAG(ARKWEB_COOKIE)
  if (!canonical_cookie) {
    LOG(WARNING)
        << "SetCookie failed with reason: create canonical cookie failed";
    SetCookieCallbackImpl(callback, false);
    return true;
  }

  net::CookieOptions options;
  if (includeHttpOnly || cookie.httponly) {
    options.set_include_httponly();
  }
  options.set_same_site_cookie_context(
      net::CookieOptions::SameSiteCookieContext::MakeInclusive());

  LOG(DEBUG) << "CefCookieManagerImpl::SetCookieInternal is_sync : " << is_sync;
  if (is_sync) {
    RunCookieTaskSync(
        base::BindOnce(&CefCookieManagerImplExt::SetCookieInternalCookieTask,
                       base::Unretained(this), std::move(canonical_cookie),
                       std::move(url), std::move(options)),
        callback);
  } else {
    RunCookieTaskAsync(base::BindOnce(
        &CefCookieManagerImplExt::SetCookieInternalCookieTask,
        base::Unretained(this), std::move(canonical_cookie), std::move(url),
        std::move(options),
        base::BindOnce(&CefCookieManagerImplExt::SetCookieCallbackImpl,
                       base::Unretained(this), callback)));
  }
  return true;
}

void CefCookieManagerImplExt::DeleteCookiesInternalCookieTask(
    network::mojom::CookieDeletionFilterPtr filter,
    base::OnceCallback<void(uint32_t)> complete) {
  LOG(DEBUG) << "CefCookieManagerImpl::DeleteCookiesInternalCookieTask";
  DCHECK(cookie_store_task_runner_->RunsTasksInCurrentSequence());
  if (!filter) {
    if (!complete.is_null()) {
      std::move(complete).Run(0);
    }
    return;
  }

  CookieManager* cookie_manager = GetNetworkCookieManager();
  if (!cookie_manager) {
    GetCookieStore()->DeleteAllMatchingInfoAsync(
        network::DeletionFilterToInfo(std::move(filter)), std::move(complete));
    return;
  }
  cookie_manager->DeleteCookies(std::move(filter), std::move(complete));
}

bool CefCookieManagerImplExt::DeleteCookiesInternal(
    const GURL& url,
    const CefString& cookie_name,
    bool is_session,
    CefRefPtr<CefDeleteCookiesCallback> callback,
    bool is_sync) {
  DCHECK(url.is_empty() || url.is_valid());

  network::mojom::CookieDeletionFilterPtr deletion_filter =
      network::mojom::CookieDeletionFilter::New();

  if (url.is_empty()) {
    // Delete all cookies.
  } else if (cookie_name.empty()) {
    // Delete all matching host cookies.
    deletion_filter->host_name = url.host();
  } else {
    // Delete all matching host and domain cookies.
    deletion_filter->url = url;
    deletion_filter->cookie_name = cookie_name;
  }

  if (is_session) {
    deletion_filter->session_control =
        network::mojom::CookieDeletionSessionControl::SESSION_COOKIES;
  }

  LOG(DEBUG) << "CefCookieManagerImplExt::DeleteCookiesInternal is_sync : "
             << is_sync;
  if (is_sync) {
    RunCookieTaskSync(
        base::BindOnce(
            &CefCookieManagerImplExt::DeleteCookiesInternalCookieTask,
            base::Unretained(this), std::move(deletion_filter)),
        callback);
  } else {
    RunCookieTaskAsync(base::BindOnce(
        &CefCookieManagerImplExt::DeleteCookiesInternalCookieTask,
        base::Unretained(this), std::move(deletion_filter),
        base::BindOnce(&CefCookieManagerImplExt::DeleteCookiesCallbackImpl,
                       base::Unretained(this), callback)));
  }
  return true;
}

void CefCookieManagerImplExt::FlushStoreInternalCookieTask(
    base::OnceClosure complete) {
  LOG(DEBUG) << "CefCookieManagerImplExt::FlushStoreInternalCookieTask";
  DCHECK(cookie_store_task_runner_->RunsTasksInCurrentSequence());

  CookieManager* cookie_manager = GetNetworkCookieManager();
  if (!cookie_manager) {
    GetCookieStore()->FlushStore(std::move(complete));
    return;
  }
  cookie_manager->FlushCookieStore(std::move(complete));
}

bool CefCookieManagerImplExt::FlushStore(
    CefRefPtr<CefCompletionCallback> callback) {
  DCHECK(ValidContext());

  auto browser_context = GetBrowserContext(browser_context_getter_);
  if (!browser_context) {
    return false;
  }
  FlushStoreInternal(callback);
  return true;
}

bool CefCookieManagerImplExt::FlushStoreInternal(
    CefRefPtr<CefCompletionCallback> callback) {
  RunCookieTaskAsync(base::BindOnce(
      &CefCookieManagerImplExt::FlushStoreInternalCookieTask,
      base::Unretained(this),
      base::BindOnce(&CefCookieManagerImplExt::RunAsyncCompletionOnTaskRunner,
                     base::Unretained(this), callback)));
  return true;
}

// CefCookieManager methods ----------------------------------------------------

// static
CefRefPtr<CefCookieManagerExt> CefCookieManagerExt::GetGlobalManager(
    CefRefPtr<CefCompletionCallback> callback) {
  CefRefPtr<CefRequestContext> context = CefRequestContext::GetGlobalContext();
  if (context) {
    return context->GetCookieManagerExt(false, callback);
  }
  if (callback) {
    callback->OnComplete();
  }
  return CefCookieManagerImplExt::GetInstance(false).get();
}

// static
CefRefPtr<CefCookieManagerExt> CefCookieManagerExt::GetGlobalIncognitoManager(
    CefRefPtr<CefCompletionCallback> callback) {
  CefRefPtr<CefRequestContext> context = CefRequestContext::GetGlobalOTRContext();
  if (context) {
    return context->GetCookieManagerExt(true, callback);
  }
  if (callback) {
    callback->OnComplete();
  }
  return CefCookieManagerImplExt::GetInstance(true).get();
}

bool CefCookieManager::CreateCefCookie(const CefString& url,
                                       const CefString& value,
#if BUILDFLAG(ARKWEB_COOKIE)
                                       bool block_truncated,
#endif // BUILDFLAG(ARKWEB_COOKIE)
                                       CefCookie& cef_cookie) {
#if BUILDFLAG(ARKWEB_COOKIE)
  return net_service::MakeCefCookieEXT(GURL(url.ToString()), value.ToString(), block_truncated,
                                       cef_cookie);
#else // BUILDFLAG(ARKWEB_COOKIE)
  return net_service::MakeCefCookie(GURL(url.ToString()), value.ToString(),
                                    cef_cookie);
#endif // BUILDFLAG(ARKWEB_COOKIE)
}

// Always execute the set callback on cookie_store_task_runner_.
void CefCookieManagerImplExt::SetCookieCallbackImpl(
    CefRefPtr<CefSetCookieCallback> callback,
    bool access_result) {
  if (!callback.get()) {
    return;
  }
  callback->OnComplete(access_result);
}

void CefCookieManagerImplExt::RunAsyncCompletionOnTaskRunner(
    CefRefPtr<CefCompletionCallback> callback) {
  if (!callback.get()) {
    return;
  }
  callback->OnComplete();
}

void CefCookieManagerImplExt::GetAllCookiesCallbackImpl(
    CefRefPtr<CefCookieVisitor> visitor,
    base::OnceClosure complete,
    CookieManager* cookie_manager,
    const net::CookieList& cookies) {
  ExecuteVisitor(visitor, cookie_manager, cookies);
  if (!complete.is_null()) {
    std::move(complete).Run();
  }
}

void CefCookieManagerImplExt::GetCookiesCallbackImpl(
    CefRefPtr<CefCookieVisitor> visitor,
    base::OnceClosure complete,
    CookieManager* cookie_manager,
    const net::CookieAccessResultList& include_cookies,
    const net::CookieAccessResultList&) {
  net::CookieList cookies;
  for (const auto& status : include_cookies) {
    cookies.push_back(status.cookie);
  }
  GetAllCookiesCallbackImpl(visitor, std::move(complete), cookie_manager,
                            cookies);
}

void CefCookieManagerImplExt::DeleteCookiesCallbackImpl(
    CefRefPtr<CefDeleteCookiesCallback> callback,
    uint32_t num_deleted) {
  if (!callback.get()) {
    return;
  }
  callback->OnComplete(num_deleted);
}

void CefCookieManagerImplExt::LoadCookiesCallback(
    net::CookieStore::GetCookieListCallback callback,
    const net::CookieAccessResultList& include_cookies,
    const net::CookieAccessResultList& excluded_cookies) {
  std::move(callback).Run(include_cookies, excluded_cookies);
}

void CefCookieManagerImplExt::LoadCookiesOnAsyncThread(
    const GURL& url,
    const net::CookieOptions& options,
    net::CookiePartitionKeyCollection cookie_partition_key_collection,
    net::CookieStore::GetCookieListCallback callback) {
  if (!cookie_store_task_runner_->RunsTasksInCurrentSequence()) {
    cookie_store_task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(base::IgnoreResult(
                           &CefCookieManagerImplExt::LoadCookiesOnAsyncThread),
                       base::Unretained(this), url, options,
                       std::move(cookie_partition_key_collection),
                       std::move(callback)));
    return;
  }

  CookieManager* cookie_manager = GetNetworkCookieManager();
  if (!cookie_manager) {
    GetCookieStore()->GetCookieListWithOptionsAsync(
        url, options, cookie_partition_key_collection,
        base::BindOnce(&CefCookieManagerImplExt::LoadCookiesCallback,
                       base::Unretained(this), std::move(callback)));
    return;
  }
  LOG(DEBUG) << "CefCookieManagerImpl::LoadCookiesOnAsyncThread";
  cookie_manager->GetCookieList(
      url, options, cookie_partition_key_collection,
      base::BindOnce(&CefCookieManagerImplExt::LoadCookiesCallback,
                     base::Unretained(this), std::move(callback)));
  return;
}

bool CefCookieManagerImplExt::SupportAsyncThreadCookieLoad() {
  return remote_network_cookie_manager_inited_ &&
         base::FeatureList::IsEnabled(kArkwebLoadCookiesOnAsyncThread);
}

void CefCookieManagerImplExt::SaveCookiesOnAsyncThread(
    const GURL& url,
    const net::CookieOptions& options,
    int total_count,
    net::CookieList cookies,
    base::OnceCallback<void(int, net::CookieList)> done_callback) {
  if (!cookie_store_task_runner_->RunsTasksInCurrentSequence()) {
    cookie_store_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(base::IgnoreResult(&CefCookieManagerImplExt::SaveCookiesOnAsyncThread),
                     base::Unretained(this), url, options, total_count, std::move(cookies), std::move(done_callback)));
    return;
  }

  // |done_callback| needs to be executed once and only once after the list has
  // been fully processed. |num_cookie_lines_left_| keeps track of how many
  // async callbacks are currently pending.
  auto progress = new SaveCookiesProgress;
  progress->done_callback_ = std::move(done_callback);
  progress->total_count_ = total_count;

  // Make sure to wait for the loop to complete.
  progress->num_cookie_lines_left_ = 1;

  CookieManager* cookie_manager = GetNetworkCookieManager();
  if (!cookie_manager) {
    for (const auto& cookie : cookies) {
      progress->num_cookie_lines_left_++;
      auto cookie_ptr = std::make_unique<net::CanonicalCookie>(cookie);
      GetCookieStore()->SetCanonicalCookieAsync(
          std::move(cookie_ptr), url, options,
          base::BindOnce(&SetCanonicalCookieCallback, base::Unretained(progress), cookie));
    }
  } else {
    for (const auto& cookie : cookies) {
      progress->num_cookie_lines_left_++;
      cookie_manager->SetCanonicalCookie(
          cookie, url, options,
          base::BindOnce(&SetCanonicalCookieCallback, base::Unretained(progress), cookie));
    }
  }

  SetCanonicalCookieCallback(
      progress, net::CanonicalCookie(),
      net::CookieAccessResult(net::CookieInclusionStatus(
          net::CookieInclusionStatus::EXCLUDE_UNKNOWN_ERROR)));
}

#if BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
bool CefCookieManagerImplExt::CanSaveOrLoadCookies(const network::ResourceRequest& request) {
  if (!host_content_settings_map_) {
    return true;
  }

  ContentSettingsForOneType cookie_settings =
      host_content_settings_map_->GetSettingsForOneType(
          ContentSettingsType::COOKIES);

  const auto& entry = base::ranges::find_if(
      cookie_settings, [&](const ContentSettingPatternSource& entry) {
        // The primary pattern is for the request URL; the secondary pattern
        // is for the first-party URL (which is the top-frame origin [if
        // available] or the site-for-cookies).
        return !entry.IsExpired() &&
               entry.primary_pattern.Matches(request.url) &&
               entry.secondary_pattern.Matches(request.url);
      });
  const ContentSettingPatternSource* match =
      (entry == cookie_settings.end() ? nullptr : &*entry);
  return !(match && match->GetContentSetting() == CONTENT_SETTING_BLOCK);
}

void CefCookieManagerImplExt::UpdateHostContentSettingsMap() {
  if (host_content_settings_map_) {
    return;
  }
  auto cef_browser_context = browser_context_getter_.Run();
  if (cef_browser_context) {
    host_content_settings_map_ =
        HostContentSettingsMapFactory::GetForProfile(cef_browser_context->AsBrowserContext());
  }
}
#endif

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
void CefCookieManagerImplExt::SetOriginAccessListForOrigin(
    const url::Origin& source_origin,
    std::vector<network::mojom::CorsOriginPatternPtr> allow_patterns,
    std::vector<network::mojom::CorsOriginPatternPtr> block_patterns) {
  std::unique_lock<std::mutex> lock(origin_access_list_mutex_);

  origin_access_list_.SetAllowListForOrigin(source_origin, allow_patterns);
  origin_access_list_.SetBlockListForOrigin(source_origin, block_patterns);
}

bool CefCookieManagerImplExt::ShouldForceIgnoreSiteForCookies(
    const network::ResourceRequest& request) {
  if (!base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableNwebEx)) {
    return false;
  }

  std::unique_lock<std::mutex> lock(origin_access_list_mutex_);

  if (request.request_initiator.has_value() &&
      network::cors::OriginAccessList::AccessState::kAllowed ==
          origin_access_list_.CheckAccessState(
              request.request_initiator.value(), request.url)) {
    return true;
  }

  url::Origin site_origin =
      url::Origin::Create(request.site_for_cookies.RepresentativeUrl());
  if (!site_origin.opaque() && request.request_initiator.has_value()) {
    bool site_can_access_target =
        network::cors::OriginAccessList::AccessState::kAllowed ==
        origin_access_list_.CheckAccessState(site_origin, request.url);
    bool site_can_access_initiator =
        network::cors::OriginAccessList::AccessState::kAllowed ==
        origin_access_list_.CheckAccessState(
            site_origin, request.request_initiator->GetURL());
    net::SiteForCookies site_of_initiator =
        net::SiteForCookies::FromOrigin(request.request_initiator.value());
    bool are_initiator_and_target_same_site =
        site_of_initiator.IsFirstParty(request.url);
    if (site_can_access_initiator && site_can_access_target &&
        are_initiator_and_target_same_site) {
      return true;
    }
  }

  return false;
}
#endif
