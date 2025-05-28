// Copyright 2025 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/chromium_ext/net/url_request/url_request_context_ext.h"
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "base/compiler_specific.h"
#include "base/debug/alias.h"
#include "base/memory/ptr_util.h"
#include "base/metrics/histogram_functions.h"
#include "base/metrics/histogram_macros.h"
#include "base/strings/string_util.h"
#include "base/types/pass_key.h"
#include "build/build_config.h"
#include "build/chromeos_buildflags.h"
#include "net/base/http_user_agent_settings.h"
#include "net/base/network_delegate.h"
#include "net/base/proxy_delegate.h"
#include "net/cert/cert_verifier.h"
#include "net/cert/sct_auditing_delegate.h"
#include "net/cookies/cookie_store.h"
#include "net/dns/host_resolver.h"
#include "net/http/http_auth_handler_factory.h"
#include "net/http/http_cache.h"
#include "net/http/http_network_session.h"
#include "net/http/http_server_properties.h"
#include "net/http/http_transaction_factory.h"
#include "net/http/transport_security_persister.h"
#include "net/http/transport_security_state.h"
#include "net/log/net_log.h"
#include "net/log/net_log_source.h"
#include "net/nqe/network_quality_estimator.h"
#include "net/proxy_resolution/proxy_resolution_service.h"
#include "net/quic/quic_context.h"
#include "net/socket/client_socket_factory.h"
#include "net/socket/ssl_client_socket_impl.h"
#include "net/ssl/ssl_config_service.h"
#include "net/url_request/url_request.h"
#include "net/url_request/url_request_job_factory.h"
namespace net {

URLRequestContextExt::URLRequestContextExt(base::PassKey<URLRequestContextBuilder> pass_key)
  : URLRequestContext(pass_key) {
}

#if BUILDFLAG(ARKWEB_EX_NETWORK_CONNECTION)
void URLRequestContextExt::SetConnectTimeout(int seconds)
{
  HttpTransactionFactory* transaction_factory = http_transaction_factory();
  if (!transaction_factory) {
    return;
  }
  HttpNetworkSession* network_session = transaction_factory->GetSession();
  if (!network_session) {
    return;
  }
  network_session->SetConnectTimeout(seconds);
}
 
void URLRequestContextExt::BindDnsToNetwork(handles::NetworkHandle network)
{
  HttpTransactionFactory* transaction_factory = http_transaction_factory();
  if (!transaction_factory) {
    return;
  }
  HttpNetworkSession* network_session = transaction_factory->GetSession();
  if (!network_session) {
    return;
  }
  network_session->CloseAllConnections(ERR_NETWORK_CHANGED, "bind dns");
  bound_network_for_dns_ = network;
}
#endif
 
#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
void URLRequestContextExt::SetConnectJobWithSecureDnsOnlyTimeout(int second)
{
  HttpTransactionFactory* transaction_factory = http_transaction_factory();
  if (!transaction_factory) {
    return;
  }
  HttpNetworkSession* network_session = transaction_factory->GetSession();
  if (!network_session) {
    return;
  }
  network_session->SetConnectJobWithSecureDnsOnlyTimeout(second);
}
 
bool URLRequestContextExt::CanUseSecureDnsFallback() const
{
  if (!host_resolver()) {
    return false;
  }
  return host_resolver()->CanUseSecureDnsFallback();
}
#endif
}  // namespace net