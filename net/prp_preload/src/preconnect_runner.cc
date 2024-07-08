// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "preconnect_runner.h"

#include "net/base/http_user_agent_settings.h"
#include "net/base/network_anonymization_key.h"
#include "net/http/http_network_session.h"
#include "net/http/http_transaction_factory.h"
#include "net/http/transport_security_state.h"
#include "net/url_request/url_request_context.h"

namespace ohos_prp_preload {

void PreconnectRunner::PreconnectSocket(
    const GURL& original_url,
    bool allow_credentials,
    base::WeakPtr<net::URLRequestContext> url_request_context) {
  if (url_request_context.get() == nullptr) {
    return;
  }
  net::NetworkAnonymizationKey key =
      net::NetworkAnonymizationKey::CreateSameSite(
          net::SchemefulSite(original_url));
  GURL url = GetHSTSRedirect(original_url, url_request_context);

  std::string user_agent;
  if (url_request_context->http_user_agent_settings()) {
    user_agent =
        url_request_context->http_user_agent_settings()->GetUserAgent();
  }
  net::HttpRequestInfo request_info;
  request_info.url = url;
  request_info.method = net::HttpRequestHeaders::kGetMethod;
  request_info.extra_headers.SetHeader(net::HttpRequestHeaders::kUserAgent,
                                       user_agent);

  if (allow_credentials) {
    request_info.load_flags = net::LOAD_NORMAL;
    request_info.privacy_mode = net::PRIVACY_MODE_DISABLED;
  } else {
    request_info.load_flags = net::LOAD_DO_NOT_SAVE_COOKIES;
    request_info.privacy_mode = net::PRIVACY_MODE_ENABLED;
  }
  request_info.network_anonymization_key = key;

  net::HttpTransactionFactory* factory =
      url_request_context->http_transaction_factory();
  net::HttpNetworkSession* session = factory->GetSession();
  net::HttpStreamFactory* http_stream_factory = session->http_stream_factory();
  http_stream_factory->PreconnectStreams(1, request_info, true);
}

GURL PreconnectRunner::GetHSTSRedirect(
    const GURL& original_url,
    base::WeakPtr<net::URLRequestContext> url_request_context) {
  if (!url_request_context->transport_security_state() ||
      !original_url.SchemeIs("http") ||
      !url_request_context->transport_security_state()->ShouldUpgradeToSSL(
          original_url.host())) {
    return original_url;
  }

  GURL::Replacements replacements;
  replacements.SetSchemeStr("https");
  return original_url.ReplaceComponents(replacements);
}

}  // namespace ohos_prp_preload