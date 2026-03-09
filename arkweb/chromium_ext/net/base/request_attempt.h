/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef ARKWEB_CHROMIUM_EXT_NET_BASE_REQUEST_ATTEMPT_H_
#define ARKWEB_CHROMIUM_EXT_NET_BASE_REQUEST_ATTEMPT_H_

#include <stdint.h>

#include <vector>

#include "arkweb/chromium_ext/net/dns/public/dns_info.h"
#include "arkweb/chromium_ext/net/socket/socket_info.h"
#include "arkweb/chromium_ext/net/ssl/arkweb_ssl_info.h"
#include "base/json/json_writer.h"
#include "net/base/net_errors.h"
#include "net/base/net_export.h"

namespace net {

enum AttemptType : int32_t {
  kNormal = 0,
  kHttpDnsOnly,
  kFallbackProxy,
  kFallbackProxyDirect,
  kWithAuth,
  kContinueWithCertificate,
  kContinueDespiteLastError,
  kCheckWirelessChange,
  kCheckSafeBrowsing,
  kCheckNotRetryHttpDns
};

// Request attempt info.
// This is really a struct.  All members are public.
struct NET_EXPORT RequestAttempt {
 public:
  RequestAttempt();
  RequestAttempt(const RequestAttempt& info);
  ~RequestAttempt();
  RequestAttempt& operator=(const RequestAttempt& info);

  std::string request_trace_id = "";
  int attempt_type = AttemptType::kNormal;
  int request_result = 0;
  bool was_fetched_via_proxy = false;
  DnsInfo dns_info;
  ArkWebSSLInfo ssl_info;
  SocketInfo socket_info;
};

}  // namespace net

#endif  // ARKWEB_CHROMIUM_EXT_NET_BASE_REQUEST_ATTEMPT_H_