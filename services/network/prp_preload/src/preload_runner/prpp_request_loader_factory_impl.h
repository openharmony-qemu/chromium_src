// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_NETWORK_PRP_PRELOAD_SRC_PRELOAD_RUNNER_PRPP_REQUEST_LOADER_FACTORY_IMPL_H
#define SERVICES_NETWORK_PRP_PRELOAD_SRC_PRELOAD_RUNNER_PRPP_REQUEST_LOADER_FACTORY_IMPL_H

#include "services/network/url_loader_context.h"
#include "services/network/prp_preload/include/preload_runner/prpp_request_loader_factory.h"
#include "services/network/prp_preload/include/preload_runner/prpp_request_loader.h"

namespace net {
class URLRequestContext;
} // namespace net

namespace ohos_prp_preload {

// only run in network thread
class PRPPRequestLoaderFactoryImpl : public PRPPRequestLoaderFactory {
public:
  PRPPRequestLoaderFactoryImpl(const std::string& url,
	base::WeakPtr<net::URLRequestContext> url_request_context);
  ~PRPPRequestLoaderFactoryImpl() = default;
  void CreateReqLoaderAndStart(const std::shared_ptr<PRRequestInfo>& info,
	bool only_send_reuse_request,
	const std::set<std::string>& need_record_header_urls) override;
  void SetPRPPIsolation(const net::IsolationInfo& isl) override;
	std::shared_ptr<PRPPRequestLoader> GetPRPPReqLoader(const network::URLLoaderContext& context,
	const network::ResourceRequest& resource_request,
	std::shared_ptr<PRRequestInfo> req_info_binding) override;
  base::WeakPtr<PRPPRequestLoaderFactory> GetWeak() override { return weak_factory_.GetWeakPtr(); }
  const std::string& GetMainUrl() const override { return main_url_; }
private:
  bool IsInfoMatched(std::shared_ptr<PRRequestInfo>& req_info, const network::URLLoaderContext& context,
	const network::ResourceRequest& resource_request,
	std::shared_ptr<PRRequestInfo>& req_info_binding);
  bool MatchRequestHeaders(std::shared_ptr<PRRequestInfo>& req_info,
	const network::ResourceRequest& resource_request,
	std::shared_ptr<PRRequestInfo>& req_info_binding);
  void OnResPreloaded(PRPPRequestLoader* prpp_loader);
  void DoPendingCreateTask(const std::string& key);
  void CreatePendingReqLoaderAndStart(const std::shared_ptr<PRRequestInfo>& info);
  void ReplaceHeaders(const net::HttpRequestHeaders& headers,
    const std::shared_ptr<PRRequestInfo>& info);
  void RecordPendingPRPPLoader(const std::shared_ptr<PRRequestInfo>& info);
  void RecordNeedRecordHeaderUrls(const network::ResourceRequest& resource_request);

  using PRPPReqLoaderMap = std::unordered_map<std::string, std::shared_ptr<PRPPRequestLoader>>;
  std::string main_url_;
  net::IsolationInfo isolation_info_;
  base::WeakPtr<net::URLRequestContext> url_request_context_;
  PRPPReqLoaderMap prpp_req_loaders_;
  std::set<std::string> requests_already_start_set_;
  base::WeakPtrFactory<PRPPRequestLoaderFactoryImpl> weak_factory_ { this };
  std::unordered_map<std::string, net::HttpRequestHeaders> can_reuse_headers_map_;
  std::unordered_map<std::string, std::list<std::shared_ptr<PRRequestInfo>>> pending_prpp_loader_list_;
  std::set<std::string> need_record_header_urls_; // key of can_reuse_headers_map_
  bool has_set_only_send_reuse_request_ { false };
  bool only_send_reuse_request_ { false };
  std::set<std::string> dynamic_urls_;
};

} // namespace ohos_prp_preload

#endif // SERVICES_NETWORK_PRP_PRELOAD_SRC_PRELOAD_RUNNER_PRPP_REQUEST_LOADER_FACTORY_IMPL_H