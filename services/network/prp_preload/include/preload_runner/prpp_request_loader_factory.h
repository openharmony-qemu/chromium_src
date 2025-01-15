// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_NETWORK_PRP_PRELOAD_INCLUDE_PRELOAD_RUNNER_PRPP_REQUEST_LOADER_FACTORY_H
#define SERVICES_NETWORK_PRP_PRELOAD_INCLUDE_PRELOAD_RUNNER_PRPP_REQUEST_LOADER_FACTORY_H

#include "services/network/public/mojom/network_context.mojom.h"
#include "net/base/page_res_request_info.h"

namespace net {
class URLRequestContext;
} // namespace net

namespace network {
class NetworkContext;
class URLLoaderContext;
} // namespace network

namespace ohos_prp_preload {
class PRPPRequestLoader;
// only run in network thread
class PRPPRequestLoaderFactory {
 public:
  // create PRPPRequestLoaderFactory
  static std::shared_ptr<PRPPRequestLoaderFactory> CreatePRPPRequestLoaderFactory(
	    const std::string& url,
	    base::WeakPtr<net::URLRequestContext> url_request_context);

  // destroy PRPPRequestLoaderFactory object
  virtual ~PRPPRequestLoaderFactory() = default;

  // create preload loader and start
  virtual void CreateReqLoaderAndStart(const std::shared_ptr<PRRequestInfo>& info,
	    bool only_send_reuse_request,
	    const std::set<std::string>& need_record_header_urls) = 0;

  // set isolation info created with cached origin
  virtual void SetPRPPIsolation(const net::IsolationInfo& isl) = 0;

  // get preload loader
  virtual std::shared_ptr<PRPPRequestLoader> GetPRPPReqLoader(
      const network::URLLoaderContext& context,
	    const network::ResourceRequest& resource_request,
	    std::shared_ptr<PRRequestInfo> req_info_binding) = 0;

  // get weak ptr of PRPPRequestLoaderFactory
  virtual base::WeakPtr<PRPPRequestLoaderFactory> GetWeak() = 0;

  // get main url of page
  virtual const std::string& GetMainUrl() const = 0;

  // get the count of prerequest can be send
  virtual uint32_t GetAndClearPreloadedCount() = 0;
};

} // namespace ohos_prp_preload

#endif // SERVICES_NETWORK_PRP_PRELOAD_INCLUDE_PRELOAD_RUNNER_PRPP_REQUEST_LOADER_FACTORY_H