// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_LIBCEF_COMMON_NET_SERVICE_NET_SERVICE_UTIL_EXT_H_
#define CEF_LIBCEF_COMMON_NET_SERVICE_NET_SERVICE_UTIL_EXT_H_

#include <map>
#include <string>

#include "base/memory/scoped_refptr.h"
#include "cef/include/internal/cef_types_wrappers.h"
#include "net/cookies/cookie_constants.h"

namespace net {
class CanonicalCookie;
class HttpResponseHeaders;
struct RedirectInfo;
}  // namespace net

namespace network {
struct ResourceRequest;
}  // namespace network

class GURL;

namespace net_service {
// Populate |cookie|. Returns true on success.
bool MakeCefCookieEXT(const GURL& url,
                      const std::string& cookie_line,
#if BUILDFLAG(ARKWEB_COOKIE)
                   bool block_truncated,
#endif // BUILDFLAG(ARKWEB_COOKIE)
                   CefCookie& cookie);

}  // namespace net_service

#endif  // CEF_LIBCEF_COMMON_NET_SERVICE_NET_SERVICE_UTIL_EXT_H_
