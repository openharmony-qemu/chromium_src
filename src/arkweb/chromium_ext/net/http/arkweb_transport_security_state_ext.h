// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef NET_HTTP_ARKWEB_TRANSPORT_SECURITY_STATE_EXT_H_
#define NET_HTTP_ARKWEB_TRANSPORT_SECURITY_STATE_EXT_H_

#include <stdint.h>

#include <array>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <string_view>

#include "base/feature_list.h"
#include "base/functional/callback.h"
#include "base/gtest_prod_util.h"
#include "base/memory/raw_ptr.h"
#include "base/threading/thread_checker.h"
#include "base/time/time.h"
#include "base/values.h"
#include "crypto/sha2.h"
#include "net/base/expiring_cache.h"
#include "net/base/hash_value.h"
#include "net/base/net_export.h"
#include "net/base/network_anonymization_key.h"
#include "net/cert/signed_certificate_timestamp_and_status.h"
#include "net/http/transport_security_state_source.h"
#include "net/log/net_log_with_source.h"
#include "net/net_buildflags.h"
#include "url/gurl.h"

namespace net {

namespace {

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
const std::string_view kSha256Slash = "sha256/";
const std::string_view kSha256SlashOhos = "sha256//";
#endif  // BUILDFLAG(ARKWEB_NETWORK_BASE)

}  // namespace

class HostPortPair;

class NET_EXPORT ArkWebTransportSecurityStateExt
    : public TransportSecurityState {
 public:
  ArkWebTransportSecurityStateExt();

  explicit ArkWebTransportSecurityStateExt(
      std::vector<std::string> hsts_host_bypass_list);

  ArkWebTransportSecurityStateExt(const ArkWebTransportSecurityStateExt&) =
      delete;
  ArkWebTransportSecurityStateExt& operator=(
      const ArkWebTransportSecurityStateExt&) = delete;

  ~ArkWebTransportSecurityStateExt();

  ArkWebTransportSecurityStateExt* AsArkWebTransportSecurityStateExt()
      override {
    return this;
  }

 private:
  friend class TransportSecurityState;
#if BUILDFLAG(ARKWEB_NETWORK_BASE)
  TransportSecurityState::PKPStatus CheckPublicKeyPinsOhos(
      const HostPortPair& host_port_pair,
      const HashValueVector& public_key_hashes);
#endif  // BUILDFLAG(ARKWEB_NETWORK_BASE)
};

}  // namespace net

#endif  // NET_HTTP_ARKWEB_TRANSPORT_SECURITY_STATE_EXT_H_
