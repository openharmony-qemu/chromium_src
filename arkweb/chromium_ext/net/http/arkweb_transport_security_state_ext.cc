// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifdef UNSAFE_BUFFERS_BUILD
// TODO(crbug.com/40284755): Remove this and spanify to fix the errors.
#pragma allow_unsafe_buffers
#endif

#include "net/http/transport_security_state.h"
#include "arkweb/chromium_ext/net/http/arkweb_transport_security_state_ext.h"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <optional>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include "base/base64.h"
#include "base/build_time.h"
#include "base/containers/contains.h"
#include "base/containers/span.h"
#include "base/feature_list.h"
#include "base/functional/bind.h"
#include "base/json/json_writer.h"
#include "base/logging.h"
#include "base/metrics/field_trial.h"
#include "base/metrics/field_trial_params.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "base/time/time.h"
#include "base/values.h"
#include "build/branding_buildflags.h"
#include "build/build_config.h"
#include "crypto/sha2.h"
#include "net/base/features.h"
#include "net/base/hash_value.h"
#include "net/base/host_port_pair.h"
#include "net/base/url_util.h"
#include "net/cert/ct_policy_status.h"
#include "net/cert/x509_certificate.h"
#include "net/dns/dns_names_util.h"
#include "net/extras/preload_data/decoder.h"
#include "net/http/http_security_headers.h"
#include "net/net_buildflags.h"
#include "net/ssl/ssl_info.h"

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#endif  // BUILDFLAG(ARKWEB_NETWORK_BASE)

namespace net {

ArkWebTransportSecurityStateExt::ArkWebTransportSecurityStateExt()
    : ArkWebTransportSecurityStateExt(std::vector<std::string>()) {}

ArkWebTransportSecurityStateExt::ArkWebTransportSecurityStateExt(
    std::vector<std::string> hsts_host_bypass_list)
    : TransportSecurityState(hsts_host_bypass_list) {}

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
TransportSecurityState::PKPStatus
ArkWebTransportSecurityStateExt::CheckPublicKeyPinsOhos(
    const HostPortPair& host_port_pair,
    const HashValueVector& public_key_hashes) {
  auto RootCertDataAdapter =
      OHOS::NWeb::OhosAdapterHelper::GetInstance().GetRootCertDataAdapter();
  std::vector<std::string> pins;
  if (!RootCertDataAdapter->GetPinSetForHostName(host_port_pair.host(), pins) ||
      pins.empty()) {
    return PKPStatus::OK;
  }
  for (auto& public_key_hash : public_key_hashes) {
    std::string public_key_hash_string = public_key_hash.ToString();
    if (!public_key_hash_string.starts_with(kSha256Slash)) {
      continue;
    }
    std::string public_key_hash_string_code =
        public_key_hash_string.substr(kSha256Slash.size());
    auto it = find_if(pins.begin(), pins.end(),
                      [&public_key_hash_string_code](const std::string& pin) {
                        if (!pin.starts_with(kSha256SlashOhos)) {
                          return false;
                        }
                        return public_key_hash_string_code ==
                               pin.substr(kSha256SlashOhos.size());
                      });
    if (it != pins.end()) {
      return PKPStatus::OK;
    }
  }
  LOG(INFO) << "CheckPublicKeyPinsOhos ssl pinning PKPStatus::VIOLATED";
  return PKPStatus::VIOLATED;
}
#endif  // BUILDFLAG(ARKWEB_NETWORK_BASE)

}  // namespace net
