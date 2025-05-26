// Copyright 2025 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This class represents contextual information (cookies, cache, etc.)
// that's necessary when processing resource requests.

#ifndef NET_URL_REQUEST_URL_REQUEST_CONTEXT_EXT_H_
#define NET_URL_REQUEST_URL_REQUEST_CONTEXT_EXT_H_

#include "net/url_request/url_request_context.h"
#include "arkweb/ohos_nweb_ex/build/features/features.h"
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
#if BUILDFLAG(ARKWEB_EXT_NETWORK_CONNECTION)
  handles::NetworkHandle bound_network_for_dns_;
#endif
#if BUILDFLAG(ARKWEB_EXT_NETWORK_CONNECTION)
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
};

}  // namespace net

#endif  // NET_URL_REQUEST_URL_REQUEST_CONTEXT_H_