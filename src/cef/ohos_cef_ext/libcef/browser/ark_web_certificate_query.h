// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_ARK_WEB_CERTIFICATE_QUERY_H_
#define CEF_LIBCEF_BROWSER_ARK_WEB_CERTIFICATE_QUERY_H_

#include <string>

#include "arkweb/build/features/features.h"
#include "cef/libcef/browser/certificate_query.h"

namespace content {
class WebContents;
}

namespace net {
class SSLInfo;
}

class GURL;

namespace certificate_query {

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
// Called from ContentBrowserClient::AllowAllCertificateError.
// |callback| will be returned if the request is unhandled and
// |default_disallow| is false.
[[nodiscard]] CertificateErrorCallback AllowAllCertificateError(
    content::WebContents* web_contents,
    int cert_error,
    const net::SSLInfo& ssl_info,
    const GURL& request_url,
    bool is_main_frame_request,
    bool strict_enforcement,
    const GURL& origin_url,
    const std::string& referrer,
    CertificateErrorCallback callback,
    bool default_disallow);
#endif  // BUILDFLAG(ARKWEB_NETWORK_LOAD)

}  // namespace certificate_query

#endif  // CEF_LIBCEF_BROWSER_ARK_WEB_CERTIFICATE_QUERY_H_
