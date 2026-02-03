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

#ifndef ARKWEB_CHROMIUM_EXT_NET_SSL_ARKWEB_SSL_INFO_H_
#define ARKWEB_CHROMIUM_EXT_NET_SSL_ARKWEB_SSL_INFO_H_

#include <stdint.h>

#include <vector>

#include "base/values.h"
#include "net/base/net_errors.h"

namespace net {

// SSL connection info.
// This is really a struct.  All members are public.
struct NET_EXPORT ArkWebSSLInfo {
  ArkWebSSLInfo();
  ArkWebSSLInfo(const ArkWebSSLInfo& info);
  ~ArkWebSSLInfo();
  ArkWebSSLInfo& operator=(const ArkWebSSLInfo& info);

  std::string host;
  // The issuer of the certificate.
  std::string issuer;
  // Time period during which the certificate is valid.
  std::string expired_date;
  // True if there was a certificate error which should be treated as fatal,
  // and false otherwise.
  bool is_fatal_cert_error = false;
  int result = net::OK;
};

}  // namespace net

#endif  // ARKWEB_CHROMIUM_EXT_NET_SSL_ARKWEB_SSL_INFO_H_