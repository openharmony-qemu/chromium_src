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

#ifndef NET_URL_REQUEST_URL_REQUEST_CONTEXT_EXT_H_
#define NET_URL_REQUEST_URL_REQUEST_CONTEXT_EXT_H_

#include "net/url_request/url_request_context.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif
#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/threading/thread_checker.h"
#include "base/types/pass_key.h"
#include "build/build_config.h"
#include "build/chromeos_buildflags.h"
#include "net/base/net_export.h"
#include "net/base/network_handle.h"
#include "net/base/request_priority.h"
#include "net/log/net_log_source.h"
#include "net/net_buildflags.h"
#include "net/traffic_annotation/network_traffic_annotation.h"
#include "net/url_request/url_request.h"

namespace net {

class NET_EXPORT URLRequestContextExt : public URLRequestContext {
public:
  friend class URLRequestContext;

  explicit URLRequestContextExt(base::PassKey<URLRequestContextBuilder> pass_key);

  URLRequestContextExt* AsURLRequestContextExt() override {
    return this;
  }
#if BUILDFLAG(ARKWEB_EX_NETWORK_CONNECTION)
  handles::NetworkHandle bound_network_for_dns_;
#endif
#if BUILDFLAG(ARKWEB_EX_NETWORK_CONNECTION)
  handles::NetworkHandle bound_network_for_dns() const {
    return bound_network_for_dns_;
  }
  void SetConnectTimeout(int seconds);
  void BindDnsToNetwork(handles::NetworkHandle network);
#endif
#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
  void SetConnectJobWithSecureDnsOnlyTimeout(int second);
  bool CanUseSecureDnsFallback() const;
#endif
#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK_ON_DNS_HIJACKING)
  bool NeedRetryDnsOnDnsHijack(const GURL& url,
                               const std::string& errorcode) const;
#endif
#if BUILDFLAG(ARKWEB_NETWORK_SERVICE)
  void SetSocketIdleTimeout(int32_t timeout);
#endif

#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
  void SetShouldReportNewNavigationInfo(bool should_report_new_navigation_info);
  bool ShouldReportNewNavigationInfo() const;
#endif

 private:
#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
  bool should_report_new_navigation_info_{false};
#endif
};

}  // namespace net

#endif  // NET_URL_REQUEST_URL_REQUEST_CONTEXT_H_