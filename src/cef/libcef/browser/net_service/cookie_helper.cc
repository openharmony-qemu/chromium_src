// Copyright (c) 2019 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "cef/libcef/browser/net_service/cookie_helper.h"

#include "base/functional/bind.h"
#include "cef/libcef/browser/thread_util.h"
#include "cef/libcef/common/net_service/net_service_util.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/storage_partition.h"
#include "content/public/common/url_constants.h"
#include "net/base/load_flags.h"
#include "net/cookies/cookie_options.h"
#include "net/cookies/cookie_util.h"
#include "services/network/cookie_manager.h"
#include "services/network/public/cpp/resource_request.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#if BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
#include "base/command_line.h"
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#endif
#if BUILDFLAG(ARKWEB_COOKIE) || BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#include "libcef/browser/net_service/cookie_manager_impl_ext.h"
#endif

namespace net_service::cookie_helper {

// Do not keep a reference to the object returned by this method.
CefBrowserContext* GetBrowserContext(const CefBrowserContext::Getter& getter) {
  CEF_REQUIRE_UIT();
  DCHECK(!getter.is_null());

  // Will return nullptr if the BrowserContext has been shut down.
  return getter.Run();
}

// Do not keep a reference to the object returned by this method.
network::mojom::CookieManager* GetCookieManager(
    content::BrowserContext* browser_context) {
  CEF_REQUIRE_UIT();
  return browser_context->GetDefaultStoragePartition()
      ->GetCookieManagerForBrowserProcess();
}

net::CookieOptions GetCookieOptions(const network::ResourceRequest& request,
                                    bool for_loading_cookies
#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
                                    , const std::optional<GURL> new_url
#endif
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
                                    , bool is_off_the_record
#endif
) {
  // Match the logic from InterceptionJob::FetchCookies and
  // ChromeContentBrowserClient::ShouldIgnoreSameSiteCookieRestrictionsWhenTopLevel.
  bool should_treat_as_first_party =
      request.url.SchemeIsCryptographic() &&
      (request.site_for_cookies.scheme() == content::kChromeUIScheme
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
       || request.site_for_cookies.scheme() == content::kArkWebUIScheme
#endif
      );

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  CefRefPtr<CefCookieManagerImplExt> cookie_manager =
      CefCookieManagerImplExt::GetInstance(is_off_the_record);

  if (cookie_manager &&
      cookie_manager->ShouldForceIgnoreSiteForCookies(request)) {
    should_treat_as_first_party = true;
  }
#endif

  bool is_main_frame_navigation =
      request.trusted_params &&
      request.trusted_params->isolation_info.request_type() ==
          net::IsolationInfo::RequestType::kMainFrame;

  // Match the logic from URLRequest::SetURLChain.
  std::vector<GURL> url_chain{request.url};
  if (request.navigation_redirect_chain.size() >= 2) {
    // Keep |request.url| as the final entry in the chain.
    url_chain.insert(url_chain.begin(),
                     request.navigation_redirect_chain.begin(),
                     request.navigation_redirect_chain.begin() +
                         request.navigation_redirect_chain.size() - 1);
  }

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  if (new_url.has_value()) {
    url_chain.push_back(new_url.value());
  }
#endif

  net::CookieOptions options;
  options.set_include_httponly();
  if (for_loading_cookies) {
    // Match the logic from URLRequestHttpJob::AddCookieHeaderAndStart.
    options.set_same_site_cookie_context(
        net::cookie_util::ComputeSameSiteContextForRequest(
            request.method, url_chain, request.site_for_cookies,
            request.request_initiator, is_main_frame_navigation,
            should_treat_as_first_party));
  } else {
    // Match the logic from
    // URLRequestHttpJob::SaveCookiesAndNotifyHeadersComplete.
    options.set_same_site_cookie_context(
        net::cookie_util::ComputeSameSiteContextForResponse(
            url_chain, request.site_for_cookies, request.request_initiator,
            is_main_frame_navigation, should_treat_as_first_party));
  }

  return options;
}

//
// LOADING COOKIES.
//

void ContinueWithLoadedCookies(const AllowCookieCallback& allow_cookie_callback,
                               DoneCookieCallback done_callback,
                               const net::CookieAccessResultList& cookies) {
  CEF_REQUIRE_IOT();
  net::CookieList allowed_cookies;
  for (const auto& status : cookies) {
    bool allow = false;
    allow_cookie_callback.Run(status.cookie, &allow);
    if (allow) {
      allowed_cookies.push_back(status.cookie);
    }
  }
  std::move(done_callback).Run(cookies.size(), std::move(allowed_cookies));
}

void GetCookieListCallback(const AllowCookieCallback& allow_cookie_callback,
                           DoneCookieCallback done_callback,
                           const net::CookieAccessResultList& included_cookies,
                           const net::CookieAccessResultList&) {
#if !BUILDFLAG(ARKWEB_COOKIE)
  CEF_REQUIRE_UIT();
#endif
  CEF_POST_TASK(CEF_IOT,
                base::BindOnce(ContinueWithLoadedCookies, allow_cookie_callback,
                               std::move(done_callback), included_cookies));
}

void LoadCookiesOnUIThread(
    const CefBrowserContext::Getter& browser_context_getter,
    const GURL& url,
    const net::CookieOptions& options,
    net::CookiePartitionKeyCollection cookie_partition_key_collection,
    const AllowCookieCallback& allow_cookie_callback,
    DoneCookieCallback done_callback) {
  auto cef_browser_context = GetBrowserContext(browser_context_getter);
  auto browser_context =
      cef_browser_context ? cef_browser_context->AsBrowserContext() : nullptr;
  if (!browser_context) {
    GetCookieListCallback(allow_cookie_callback, std::move(done_callback),
                          net::CookieAccessResultList(),
                          net::CookieAccessResultList());
    return;
  }

  GetCookieManager(browser_context)
      ->GetCookieList(
          url, options, cookie_partition_key_collection,
          base::BindOnce(GetCookieListCallback, allow_cookie_callback,
                         std::move(done_callback)));
}

//
// SAVING COOKIES.
//

struct SaveCookiesProgress {
  DoneCookieCallback done_callback_;
  int total_count_;
  net::CookieList allowed_cookies_;
  int num_cookie_lines_left_;
};

void SetCanonicalCookieCallback(SaveCookiesProgress* progress,
                                const net::CanonicalCookie& cookie,
                                net::CookieAccessResult access_result) {
  CEF_REQUIRE_UIT();
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

void SaveCookiesOnUIThread(
    const CefBrowserContext::Getter& browser_context_getter,
    const GURL& url,
    const net::CookieOptions& options,
    int total_count,
    net::CookieList cookies,
    DoneCookieCallback done_callback) {
  DCHECK(!cookies.empty());

  auto cef_browser_context = GetBrowserContext(browser_context_getter);
  auto browser_context =
      cef_browser_context ? cef_browser_context->AsBrowserContext() : nullptr;
  if (!browser_context) {
#if BUILDFLAG(ARKWEB_COOKIE)
    CEF_POST_TASK(CEF_IOT, base::BindOnce(std::move(done_callback), 0,
                                          net::CookieList()));
#else
    std::move(done_callback).Run(0, net::CookieList());
#endif
    return;
  }

  network::mojom::CookieManager* cookie_manager =
      GetCookieManager(browser_context);

  // |done_callback| needs to be executed once and only once after the list has
  // been fully processed. |num_cookie_lines_left_| keeps track of how many
  // async callbacks are currently pending.
  auto progress = new SaveCookiesProgress;
  progress->done_callback_ = std::move(done_callback);
  progress->total_count_ = total_count;

  // Make sure to wait for the loop to complete.
  progress->num_cookie_lines_left_ = 1;

  for (const auto& cookie : cookies) {
    progress->num_cookie_lines_left_++;
    cookie_manager->SetCanonicalCookie(
        cookie, url, options,
        base::BindOnce(&SetCanonicalCookieCallback, base::Unretained(progress),
                       cookie));
  }

  net::CookieInclusionStatus status;
  status.AddExclusionReason(
      net::CookieInclusionStatus::ExclusionReason::EXCLUDE_UNKNOWN_ERROR);
  SetCanonicalCookieCallback(progress, net::CanonicalCookie(),
                             net::CookieAccessResult(std::move(status)));
}

}  // namespace

bool IsCookieableScheme(
    const GURL& url,
    const std::optional<std::vector<std::string>>& cookieable_schemes) {
  if (!url.has_scheme()) {
    return false;
  }

  if (cookieable_schemes) {
    // The client has explicitly registered the full set of schemes that should
    // be supported.
    const auto url_scheme = url.scheme_piece();
    for (auto scheme : *cookieable_schemes) {
      if (url_scheme == scheme) {
        return true;
      }
    }
    return false;
  }

  // Schemes that support cookies by default.
  // This should match CookieMonster::kDefaultCookieableSchemes.
  return url.SchemeIsHTTPOrHTTPS() || url.SchemeIsWSOrWSS();
}

void LoadCookies(const CefBrowserContext::Getter& browser_context_getter,
                 const network::ResourceRequest& request,
                 const AllowCookieCallback& allow_cookie_callback,
                 DoneCookieCallback done_callback) {
  CEF_REQUIRE_IOT();

  if ((request.load_flags & net::LOAD_DO_NOT_SEND_COOKIES) ||
      request.credentials_mode == network::mojom::CredentialsMode::kOmit ||
      request.url.IsAboutBlank()) {
    // Continue immediately without loading cookies.
    std::move(done_callback).Run(0, {});
    return;
  }

  net::CookiePartitionKeyCollection partition_key_collection;
  if (request.trusted_params.has_value() &&
      !request.trusted_params->isolation_info.IsEmpty()) {
    const auto& isolation_info = request.trusted_params->isolation_info;
    partition_key_collection = net::CookiePartitionKeyCollection(
        net::CookiePartitionKey::FromNetworkIsolationKey(
            isolation_info.network_isolation_key(), request.site_for_cookies,
            net::SchemefulSite(request.url),
            isolation_info.IsMainFrameRequest()));
  }

  CEF_POST_TASK(
      CEF_UIT,
      base::BindOnce(LoadCookiesOnUIThread, browser_context_getter, request.url,
                     GetCookieOptions(request, /*for_loading_cookies=*/true),
                     std::move(partition_key_collection), allow_cookie_callback,
                     std::move(done_callback)));
}

void SaveCookies(const CefBrowserContext::Getter& browser_context_getter,
                 const network::ResourceRequest& request,
                 net::HttpResponseHeaders* headers,
                 const AllowCookieCallback& allow_cookie_callback,
                 DoneCookieCallback done_callback) {
  CEF_REQUIRE_IOT();

  if (request.credentials_mode == network::mojom::CredentialsMode::kOmit ||
      request.url.IsAboutBlank() || !headers ||
      !headers->HasHeader(net_service::kHTTPSetCookieHeaderName)) {
    // Continue immediately without saving cookies.
    std::move(done_callback).Run(0, {});
    return;
  }

  // Match the logic in
  // URLRequestHttpJob::SaveCookiesAndNotifyHeadersComplete.
  const auto response_date = headers->GetDateValue();

  const std::string_view name(net_service::kHTTPSetCookieHeaderName);
  std::string cookie_string;
  size_t iter = 0;
  net::CookieList allowed_cookies;
  int total_count = 0;

#if BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
  CefRefPtr<CefCookieManagerImplExt> cookie_manager =
    CefCookieManagerImplExt::GetInstance(is_off_the_record);
#endif
  while (headers->EnumerateHeader(&iter, name, &cookie_string)) {
    total_count++;

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
    std::optional<net::CookiePartitionKey> optional_partition_key = std::nullopt;
    if (!isolation_info.IsEmpty()) {
      optional_partition_key = net::CookiePartitionKey::FromNetworkIsolationKey(
        isolation_info.network_isolation_key(), request.site_for_cookies,
        net::SchemefulSite(request.url),
        isolation_info.IsMainFrameRequest());
    }
#endif

    net::CookieInclusionStatus returned_status;
    std::unique_ptr<net::CanonicalCookie> cookie = net::CanonicalCookie::Create(
        request.url, cookie_string, base::Time::Now(), response_date,
#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
        /*cookie_partition_key=*/optional_partition_key, net::CookieSourceType::kHTTP,
#else
        /*cookie_partition_key=*/std::nullopt, net::CookieSourceType::kHTTP,
#endif
        &returned_status);
    if (!returned_status.IsInclude()) {
      continue;
    }

#if BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
    if (cookie && cookie_manager && !cookie_manager->CanSaveOrLoadCookies(request)) {
      returned_status.AddExclusionReason(
          net::CookieInclusionStatus::EXCLUDE_USER_PREFERENCES);
    }

    if (!returned_status.IsInclude()) {
      continue;
    }
#endif

    bool allow = false;
    allow_cookie_callback.Run(*cookie, &allow);
    if (allow) {
      allowed_cookies.push_back(*cookie);
    }
  }

  if (!allowed_cookies.empty()) {
#if BUILDFLAG(ARKWEB_COOKIE)
#if !BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
    CefRefPtr<CefCookieManagerImplExt> cookie_manager =
      CefCookieManagerImplExt::GetInstance(is_off_the_record);
#endif
    if (cookie_manager && cookie_manager->SupportAsyncThreadCookieLoad()) {
      cookie_manager->SaveCookiesOnAsyncThread(
        request.url,
#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
        GetCookieOptions(request, /*for_loading_cookies=*/false, {},
                         is_off_the_record),
#else
        GetCookieOptions(request, /*for_loading_cookies=*/false, {}),
#endif
#else
        GetCookieOptions(request, /*for_loading_cookies=*/false),
#endif
        total_count, std::move(allowed_cookies), std::move(done_callback));
      return;
    }
#endif
    CEF_POST_TASK(
        CEF_UIT,
        base::BindOnce(
            SaveCookiesOnUIThread, browser_context_getter, request.url,
#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
            GetCookieOptions(request, /*for_loading_cookies=*/false, {},
                             is_off_the_record),
#else
            GetCookieOptions(request, /*for_loading_cookies=*/false, {}),
#endif
#else
            GetCookieOptions(request, /*for_loading_cookies=*/false),
#endif
            total_count, std::move(allowed_cookies), std::move(done_callback)));

  } else {
    std::move(done_callback).Run(total_count, std::move(allowed_cookies));
  }
}

}  // namespace net_service::cookie_helper
