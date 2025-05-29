/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "arkweb/chromium_ext/services/network/network_service_network_delegate_ext.h"

#include <optional>
#include <string>

#include "base/debug/dump_without_crashing.h"
#include "base/functional/bind.h"
#include "base/ranges/algorithm.h"
#include "base/strings/utf_string_conversions.h"
#include "base/types/optional_util.h"
#include "build/build_config.h"
#include "components/domain_reliability/monitor.h"
#include "net/base/features.h"
#include "net/base/isolation_info.h"
#include "net/base/load_flags.h"
#include "net/base/net_errors.h"
#include "net/cookies/cookie_setting_override.h"
#include "net/cookies/cookie_util.h"
#include "net/url_request/clear_site_data.h"
#include "net/url_request/referrer_policy.h"
#include "net/url_request/url_request.h"
#include "services/network/cookie_manager.h"
#include "services/network/cookie_settings.h"
#include "services/network/network_context.h"
#include "services/network/network_service.h"
#include "services/network/network_service_network_delegate.h"
#include "services/network/network_service_proxy_delegate.h"
#include "services/network/pending_callback_chain.h"
#include "services/network/public/cpp/features.h"
#include "services/network/url_loader.h"
#include "url/gurl.h"
#if BUILDFLAG(ARKWEB_EXT_LOG_MESSAGE)
#include "net/base/ip_endpoint.h"
#include "net/nqe/network_quality_estimator.h"
#include "net/url_request/url_request_context.h"
#endif

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

namespace network {

#if BUILDFLAG(ARKWEB_EXT_LOG_MESSAGE)
void NetworkServiceNetworkDelegateExt::RecordErrorInfo(net::URLRequest* request,
                                                       int net_error) {
  int downlink_kbps = GetDownStreamThroughputKbps();
  int extended_error_code = 0;
  if (net_error == net::ERR_QUIC_PROTOCOL_ERROR) {
    net::NetErrorDetails details;
    request->PopulateNetErrorDetails(&details);
    extended_error_code = details.quic_connection_error;
  }
  std::string error_code_info =
      net::ExtendedErrorToString(net_error, extended_error_code);
  base::TimeDelta duration_time =
      base::TimeTicks::Now() - request->creation_time();

  URLLoader* url_loader = URLLoader::ForRequest(*request);
  uint32_t resource_type = -1;
  if (url_loader) {
    resource_type = url_loader->GetResourceType();
  }

  std::ostringstream ostr;
  ostr << ", error_code " << net_error << "(" << error_code_info
       << ", resource_type: " << resource_type
       << ", downstream throughput kbps: " << downlink_kbps
       << ", duration_time(ms) " << duration_time.InMilliseconds();
  LOG(INFO) << "final url: *** " << ostr.str();
}

int32_t NetworkServiceNetworkDelegateExt::GetDownStreamThroughputKbps() {
  if (network_context_->network_service() &&
      network_context_->network_service()->network_quality_estimator()) {
    return network_context_->network_service()
        ->network_quality_estimator()
        ->GetDownstreamThroughputKbps()
        .value_or(0);
  }

  return 0;
}

#endif

}  // namespace network