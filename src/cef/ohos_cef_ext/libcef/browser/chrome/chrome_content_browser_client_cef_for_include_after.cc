/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "arkweb/build/features/features.h"

#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
#include "base/timer/timer.h"
#include "cef/ohos_cef_ext/libcef/browser/arkweb_browser_info_manager_utils.h"
#endif

#if BUILDFLAG(ARKWEB_SITE_ISOLATION)
#include "components/site_isolation/site_isolation_policy.h"
#include "content/public/browser/site_isolation_mode.h"
#endif

#if BUILDFLAG(ARKWEB_COOKIE)
#include "cef/ohos_cef_ext/include/cef_cookie_ext.h"
#include "cef/ohos_cef_ext/libcef/browser/net_service/cookie_manager_impl_ext.h"
#endif  // BUILDFLAG(ARKWEB_COOKIE)

#if BUILDFLAG(ARKWEB_RESOURCE_INTERCEPTION)
#include "libcef/browser/report_manager.h"
#endif

#if BUILDFLAG(ARKWEB_URL_TRUST_LIST)
#include "libcef/browser/ohos_safe_browsing/ohos_url_trust_list_navigation_throttle.h"
#endif

#if BUILDFLAG(ARKWEB_ACTIVITY_STATE)
#include "components/performance_manager/embedder/binders.h"
#endif

#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
#include "chrome/common/chrome_constants.h"
#endif  // BUILDFLAG(ARKWEB_INCOGNITO_MODE)

#if BUILDFLAG(ARKWEB_SITE_ISOLATION)
bool g_siteIsolationMode = false;
#endif

namespace {

#if BUILDFLAG(ARKWEB_MAX_CACHE_SIZE)
constexpr int64_t LARGE_CAPACITY_DEVICE_THRESHOLD =
    static_cast<int64_t>(100) * 1024 * 1024 * 1024;
constexpr int32_t LARGE_CAPACITY_DEVICE_CACHE_SIZE = 100 * 1024 * 1024;
constexpr int32_t SMALL_CAPACITY_DEVICE_CACHE_SIZE = 20 * 1024 * 1024;
constexpr char WEB_CACHE_PATH[] = "/data/storage/el2/base/cache/web";
#endif

#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
class PopupWindowCallbackImpl : public CefCallback {
 public:
  explicit PopupWindowCallbackImpl(
      content::mojom::FrameHost::GetCreateNewWindowCallback callback)
      : callback_(std::move(callback)),
        task_runner_(base::SequencedTaskRunner::GetCurrentDefault()) {}

  ~PopupWindowCallbackImpl() override {}

  void Continue() override {
    if (task_runner_ && !task_runner_->RunsTasksInCurrentSequence()) {
      task_runner_->PostTask(
          FROM_HERE, base::BindOnce(&PopupWindowCallbackImpl::Continue, this));
      return;
    }
    LOG(INFO) << "PopupWindowCallbackImpl Continue";
    if (!callback_.is_null()) {
      std::move(callback_).Run(content::mojom::CreateNewWindowStatus::kSuccess);
    }
  }

  void Cancel() override {
    if (task_runner_ && !task_runner_->RunsTasksInCurrentSequence()) {
      task_runner_->PostTask(
          FROM_HERE, base::BindOnce(&PopupWindowCallbackImpl::Cancel, this));
      return;
    }
    LOG(INFO) << "PopupWindowCallbackImpl Cancel";
    if (!callback_.is_null()) {
      std::move(callback_).Run(content::mojom::CreateNewWindowStatus::kBlocked);
    }
  }

 private:
  content::mojom::FrameHost::GetCreateNewWindowCallback callback_;
  scoped_refptr<base::SequencedTaskRunner> task_runner_;

  IMPLEMENT_REFCOUNTING(PopupWindowCallbackImpl);
};

std::optional<bool> ArkWebInnerCanCreateWindow(content::RenderFrameHost* opener,
                                               bool user_gesture) {
  content::WebContents* web_contents =
      content::WebContents::FromRenderFrameHost(opener);
  if (!web_contents) {
    LOG(ERROR) << "web_contents is null!";
    return false;
  }
  CefRefPtr<CefBrowserHostBase> browser_host =
      CefBrowserHostBase::GetBrowserForContents(web_contents);
  if (!browser_host) {
    LOG(ERROR) << "browser_host is null!";
    return false;
  }
  if (!browser_host->settings().supports_multiple_windows) {
    if (browser_host->settings().javascript_can_open_windows_automatically ||
        user_gesture) {
      LOG(INFO) << "allow load url";
      return true;
    }
    LOG(INFO) << "supports_multiple_windows is false";
    return false;
  }
  if (!browser_host->settings().javascript_can_open_windows_automatically &&
      !user_gesture) {
    LOG(INFO) << "javascript_can_open_windows_automatically false";
    return false;
  }
  return std::nullopt;
}
#endif  // BUILDFLAG(ARKWEB_MULTI_WINDOW)

void ArkWebInnerConfigureNetworkContextParamsBefore(
    content::BrowserContext* context,
    network::mojom::NetworkContextParams* network_context_params) {
#if BUILDFLAG(ARKWEB_CACHE)
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kHttpCacheMaxSize)) {
    if (base::CommandLine::ForCurrentProcess()->HasSwitch(
            switches::kEnableNwebEx)) {
      // In order to make better use of cache, we use the same strategy as
      // chromium for httpcache.
      // Determined by DiskCache itself.
      network_context_params->http_cache_max_size = 0;
    } else {
#if BUILDFLAG(ARKWEB_MAX_CACHE_SIZE)
      int64_t totalDiskSpace =
          base::SysInfo::AmountOfTotalDiskSpace(base::FilePath(WEB_CACHE_PATH));
      if (totalDiskSpace >= LARGE_CAPACITY_DEVICE_THRESHOLD) {
        LOG(DEBUG)
            << "Set http cache max size to 100MB for large capacity device.";
        network_context_params->http_cache_max_size =
            LARGE_CAPACITY_DEVICE_CACHE_SIZE;
      } else {
        LOG(DEBUG)
            << "Set http cache max size to 20MB for samll capacity device.";
        network_context_params->http_cache_max_size =
            SMALL_CAPACITY_DEVICE_CACHE_SIZE;
      }
#else
      network_context_params->http_cache_max_size =
          LARGE_CAPACITY_DEVICE_CACHE_SIZE;
#endif
    }
  }
#endif  // BUILDFLAG(ARKWEB_CACHE)

#if BUILDFLAG(ARKWEB_COOKIE)
  mojo::PendingRemote<network::mojom::CookieManager> cookie_manager_remote;
  network_context_params->cookie_manager =
      cookie_manager_remote.InitWithNewPipeAndPassReceiver();
  CefRefPtr<CefCookieManagerExt> cookie_manager =
      context->IsOffTheRecord()
          ? CefCookieManagerExt::GetGlobalIncognitoManager(nullptr)
          : CefCookieManagerExt::GetGlobalManager(nullptr);

  if (cookie_manager) {
    static_cast<CefCookieManagerImplExt*>(cookie_manager.get())
        ->SetNetWorkCookieManager(std::move(cookie_manager_remote));
  }
#endif  // BUILDFLAG(ARKWEB_COOKIE)
}

void ArkWebInnerConfigureNetworkContextParamsAfter(
    content::BrowserContext* context,
    network::mojom::NetworkContextParams* network_context_params) {
#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
  network_context_params->file_paths =
      ::network::mojom::NetworkContextFilePaths::New();
  base::FilePath cache_path;
  if (context->IsOffTheRecord() || context->GetPath().empty()) {
    network_context_params->http_cache_enabled = false;
  } else if (base::PathService::Get(base::DIR_CACHE, &cache_path)) {
    network_context_params->file_paths->data_directory = cache_path;
    network_context_params->file_paths->cookie_database_name =
        base::FilePath(chrome::kCookieFilename);

    network_context_params->http_cache_enabled = true;
    network_context_params->file_paths->http_cache_directory =
        cache_path.AppendASCII(chrome::kCacheDirname);
  }
#endif
}

void ArkWebInnerExposeInterfacesToRenderer(
    service_manager::BinderRegistry* registry,
    blink::AssociatedInterfaceRegistry* associated_registry,
    content::RenderProcessHost* host) {
#if BUILDFLAG(ARKWEB_RESOURCE_INTERCEPTION)
  CefReportManager::ExposeInterfacesToRenderer(registry, associated_registry,
                                               host);
#endif
}

void ArkWebInnerCreateThrottlesForNavigation(
    content::NavigationThrottleRegistry& registry,
    std::vector<std::unique_ptr<content::NavigationThrottle>>& throttles) {
#if BUILDFLAG(ARKWEB_URL_TRUST_LIST)
  throttles.push_back(
      ohos_safe_browsing::UrlTrustListNavigationThrottle::Create(registry));
#endif
}

void ArkWebInnerRegisterBrowserInterfaceBindersForFrame(
    mojo::BinderMapWithContext<content::RenderFrameHost*>* map) {
#if BUILDFLAG(ARKWEB_ACTIVITY_STATE)
  auto* registry =
      performance_manager::PerformanceManagerRegistry::GetInstance();
  if (registry) {
    registry->GetBinders().ExposeInterfacesToRenderFrame(map);
  }
#endif
}

}  // namespace

#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
bool ChromeContentBrowserClientCef::CanCreateWindow(
    content::RenderFrameHost* opener,
    const GURL& target_url,
    WindowOpenDisposition disposition,
    bool user_gesture,
    content::mojom::FrameHost::GetCreateNewWindowCallback callback) {
  CEF_REQUIRE_UIT();
  content::WebContents* web_contents =
      content::WebContents::FromRenderFrameHost(opener);
  CefRefPtr<CefBrowserHostBase> browser_host =
      CefBrowserHostBase::GetBrowserForContents(web_contents);
  if (!browser_host) {
    std::move(callback).Run(content::mojom::CreateNewWindowStatus::kBlocked);
    return false;
  }
  if (!browser_host->settings().javascript_can_open_windows_automatically &&
      !user_gesture) {
    LOG(INFO) << "javascript_can_open_windows_automatically false";
    std::move(callback).Run(content::mojom::CreateNewWindowStatus::kBlocked);
    return false;
  }
  if (!browser_host->settings().supports_multiple_windows) {
    LOG(INFO) << "supports_multiple_windows is false";
    std::move(callback).Run(content::mojom::CreateNewWindowStatus::kBlocked);
    return false;
  }
  CefRefPtr<PopupWindowCallbackImpl> callbackImpl =
      new PopupWindowCallbackImpl(std::move(callback));
  auto arkweb_browser_info_manager_utils =
      CefBrowserInfoManager::GetInstance()->GetUtils();
  bool result = arkweb_browser_info_manager_utils->CanCreateWindow(
      opener, target_url, disposition, user_gesture, callbackImpl);
  return result;
}
#endif  // BUILDFLAG(ARKWEB_MULTI_WINDOW)

#if BUILDFLAG(ARKWEB_INCOGNITO_MODE)
CefRefPtr<CefRequestContextImpl>
ChromeContentBrowserClientCef::off_the_record_request_context() const {
  return browser_main_parts_->off_the_record_request_context();
}
#endif

#if BUILDFLAG(ARKWEB_SITE_ISOLATION)
bool ChromeContentBrowserClientCef::ShouldDisableSiteIsolation(
    content::SiteIsolationMode site_isolation_mode) {
  if (g_siteIsolationMode) {
    return site_isolation::SiteIsolationPolicy::
        ShouldDisableSiteIsolationDueToMemoryThreshold(site_isolation_mode);
  } else if (site_isolation_mode == content::SiteIsolationMode::kPartialSiteIsolation) {
    return false;
  }
  return true;
}

bool ChromeContentBrowserClientCef::ShouldLockProcessToSite(
    content::BrowserContext* browser_context,
    const GURL& effective_url) {
  return false;
}
#endif
