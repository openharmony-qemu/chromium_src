// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "cef/ohos_cef_ext/libcef/common/net_service/net_service_util_ext.h"

#include <set>

#include "base/logging.h"
#include "base/strings/string_number_conversions.h"
#include "cef/include/internal/cef_time_wrappers.h"
#include "cef/libcef/common/time_util.h"
#include "net/cookies/canonical_cookie.h"
#include "net/cookies/cookie_util.h"
#include "net/cookies/parsed_cookie.h"
#include "net/http/http_request_headers.h"
#include "net/http/http_response_headers.h"
#include "net/http/http_status_code.h"
#include "net/url_request/redirect_info.h"
#include "net/url_request/redirect_util.h"
#include "net/url_request/referrer_policy.h"
#include "net/url_request/url_request.h"
#include "services/network/public/cpp/resource_request.h"

namespace net_service {

namespace {

// Determine the cookie domain to use for setting the specified cookie.
// From net/cookies/canonical_cookie.cc.
bool GetCookieDomain(const GURL& url,
                     const net::ParsedCookie& pc,
                     std::string* result) {
  std::string domain_string;
  auto domain_opt = pc.Domain();
  if (domain_opt.has_value()) {
    domain_string = std::string(domain_opt.value());
  }
  net::CookieInclusionStatus status;
  auto domain_result = net::cookie_util::GetCookieDomainWithString(url, domain_string, status);
  if (domain_result.has_value()) {
    *result = domain_result.value();
    return true;
  }
  return false;
}

cef_cookie_same_site_t MakeCefCookieSameSite(net::CookieSameSite value) {
  switch (value) {
    case net::CookieSameSite::UNSPECIFIED:
      return CEF_COOKIE_SAME_SITE_UNSPECIFIED;
    case net::CookieSameSite::NO_RESTRICTION:
      return CEF_COOKIE_SAME_SITE_NO_RESTRICTION;
    case net::CookieSameSite::LAX_MODE:
      return CEF_COOKIE_SAME_SITE_LAX_MODE;
    case net::CookieSameSite::STRICT_MODE:
      return CEF_COOKIE_SAME_SITE_STRICT_MODE;
  }
}

cef_cookie_priority_t MakeCefCookiePriority(net::CookiePriority value) {
  switch (value) {
    case net::COOKIE_PRIORITY_LOW:
      return CEF_COOKIE_PRIORITY_LOW;
    case net::COOKIE_PRIORITY_MEDIUM:
      return CEF_COOKIE_PRIORITY_MEDIUM;
    case net::COOKIE_PRIORITY_HIGH:
      return CEF_COOKIE_PRIORITY_HIGH;
  }
}

}  // namespace

bool MakeCefCookieEXT(const GURL& url,
                   const std::string& cookie_line,
#if BUILDFLAG(ARKWEB_COOKIE)
                   bool block_truncated, 
#endif // BUILDFLAG(ARKWEB_COOKIE)
                   CefCookie& cookie) {
  net::CookieInclusionStatus status;
#if BUILDFLAG(ARKWEB_COOKIE)
  net::ParsedCookie pc(cookie_line, block_truncated, &status);
#else // BUILDFLAG(ARKWEB_COOKIE)
  net::ParsedCookie pc(cookie_line);
#endif // BUILDFLAG(ARKWEB_COOKIE)

  if (!pc.IsValid()) {
#if BUILDFLAG(ARKWEB_COOKIE)
    LOG(ERROR) << "Make CefCookie failed status reason: " << status.GetDebugString();
#endif // BUILDFLAG(ARKWEB_COOKIE)
    return false;
  }

  std::string cookie_domain;
  if (!GetCookieDomain(url, pc, &cookie_domain)) {
#if BUILDFLAG(ARKWEB_COOKIE)
    LOG(ERROR) << "Make CefCookie failed reason: Get cookie domain failed.";
#endif // BUILDFLAG(ARKWEB_COOKIE)
    return false;
  }

  std::string path_string;
  auto path_opt = pc.Path();
  if (path_opt.has_value()) {
    path_string = std::string(path_opt.value());
  }
  std::string cookie_path =
      net::cookie_util::CanonPathWithString(url, path_string);
  base::Time creation_time = base::Time::Now();
  base::Time cookie_expires =
      net::CanonicalCookie::ParseExpiration(pc, creation_time, creation_time);

  CefString(&cookie.name).FromString(pc.Name());
  CefString(&cookie.value).FromString(pc.Value());
  CefString(&cookie.domain).FromString(cookie_domain);
  CefString(&cookie.path).FromString(cookie_path);
  cookie.secure = pc.IsSecure();
  cookie.httponly = pc.IsHttpOnly();
  cookie.creation = CefBaseTime(creation_time);
  cookie.last_access = CefBaseTime(creation_time);
  cookie.has_expires = !cookie_expires.is_null();
  if (cookie.has_expires) {
    cookie.expires = CefBaseTime(cookie_expires);
  }
  auto [same_site_enum, same_site_string] = pc.SameSite();
  cookie.same_site = MakeCefCookieSameSite(same_site_enum);
  cookie.priority = MakeCefCookiePriority(pc.Priority());

  return true;
}

}  // namespace net_service
