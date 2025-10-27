// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef NET_SOCKET_ARKWEB_CLIENT_SOCKET_POOL_EXT_H_
#define NET_SOCKET_ARKWEB_CLIENT_SOCKET_POOL_EXT_H_

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "base/memory/raw_ptr.h"
#include "base/memory/ref_counted.h"
#include "base/time/time.h"
#include "base/values.h"
#include "net/base/completion_once_callback.h"
#include "net/base/load_states.h"
#include "net/base/net_export.h"
#include "net/base/network_anonymization_key.h"
#include "net/base/privacy_mode.h"
#include "net/base/request_priority.h"
#include "net/dns/host_resolver.h"
#include "net/dns/public/secure_dns_policy.h"
#include "net/http/http_request_info.h"
#include "net/log/net_log_capture_mode.h"
#include "net/socket/connect_job.h"
#include "net/socket/socket_tag.h"
#include "net/ssl/ssl_config.h"
#include "url/scheme_host_port.h"

namespace net {

class ConnectJobFactory;

class NET_EXPORT ArkWebClientSocketPoolExt {
 public:
  raw_ptr<ClientSocketPool> client_socket_pool;
  ArkWebClientSocketPoolExt(ClientSocketPool* socketPool);

  ArkWebClientSocketPoolExt(const ArkWebClientSocketPoolExt&) = delete;
  ArkWebClientSocketPoolExt& operator=(const ArkWebClientSocketPoolExt&) =
      delete;

  ~ArkWebClientSocketPoolExt();

#if BUILDFLAG(ARKWEB_EXT_NETWORK_CONNECTION)
  void SetConnectTimeout(int timeout_override);
  int GetConnectTimeout();
#endif
#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
  void SetConnectJobWithSecureDnsOnlyTimeout(int seconds);
  int GetConnectJobWithSecureDnsOnlyTimeout();
#endif

 private:
  friend class ClientSocketPool;
  friend class WebSocketTransportClientSocketPool;
#if BUILDFLAG(ARKWEB_EXT_NETWORK_CONNECTION)
  int timeout_override_{0};
#endif
#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
  int connect_job_with_secure_dns_only_timeout_{15};
#endif
};

}  // namespace net

#endif  // NET_SOCKET_ARKWEB_CLIENT_SOCKET_POOL_EXT_H_
