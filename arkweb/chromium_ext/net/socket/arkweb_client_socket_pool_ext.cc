// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "net/socket/client_socket_pool.h"
#include "arkweb/chromium_ext/net/socket/arkweb_client_socket_pool_ext.h"

#include <memory>
#include <string_view>
#include <utility>
#include <vector>

#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "base/check_op.h"
#include "base/feature_list.h"
#include "base/functional/bind.h"
#include "base/strings/strcat.h"
#include "net/base/features.h"
#include "net/base/host_port_pair.h"
#include "net/base/proxy_chain.h"
#include "net/base/session_usage.h"
#include "net/dns/public/secure_dns_policy.h"
#include "net/http/http_proxy_connect_job.h"
#include "net/log/net_log_event_type.h"
#include "net/log/net_log_with_source.h"
#include "net/socket/connect_job.h"
#include "net/socket/connect_job_factory.h"
#include "net/socket/socks_connect_job.h"
#include "net/socket/ssl_connect_job.h"
#include "net/socket/stream_socket.h"
#include "net/spdy/spdy_session.h"
#include "net/spdy/spdy_session_pool.h"
#include "net/ssl/ssl_config.h"
#include "url/gurl.h"
#include "url/scheme_host_port.h"
#include "url/url_constants.h"

namespace net {

ArkWebClientSocketPoolExt::~ArkWebClientSocketPoolExt() = default;

ArkWebClientSocketPoolExt::ArkWebClientSocketPoolExt(
    ClientSocketPool* socketPool) {
  client_socket_pool = socketPool;
}

#if BUILDFLAG(ARKWEB_EXT_NETWORK_CONNECTION)
void ArkWebClientSocketPoolExt::SetConnectTimeout(int timeout_override) {
  timeout_override_ = timeout_override;
}

int ArkWebClientSocketPoolExt::GetConnectTimeout() {
  return timeout_override_;
}
#endif

#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
void ArkWebClientSocketPoolExt::SetConnectJobWithSecureDnsOnlyTimeout(
    int seconds) {
  connect_job_with_secure_dns_only_timeout_ = seconds;
}

int ArkWebClientSocketPoolExt::GetConnectJobWithSecureDnsOnlyTimeout() {
  return connect_job_with_secure_dns_only_timeout_;
}
#endif
}  // namespace net
