// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "services/network/prp_preload/src/preload_runner/prpp_request_loader_factory_impl.h"

#include "base/logging.h"
#include "net/http/http_util.h"
#include "services/network/prp_preload/include/page_res_parallel_preload_mgr.h"
#include "services/network/public/cpp/resource_request.h"

namespace ohos_prp_preload {
std::shared_ptr<PRPPRequestLoaderFactory> PRPPRequestLoaderFactory::CreatePRPPRequestLoaderFactory(
	const std::string& url,
	base::WeakPtr<net::URLRequestContext> url_request_context)
{
  std::shared_ptr<PRPPRequestLoaderFactory> prpp_req_loader_fac = std::make_shared<PRPPRequestLoaderFactoryImpl>(
	url, url_request_context);
  return prpp_req_loader_fac;
}
PRPPRequestLoaderFactoryImpl::PRPPRequestLoaderFactoryImpl(const std::string& url,
	base::WeakPtr<net::URLRequestContext> url_request_context)
	: main_url_(url), url_request_context_(url_request_context) {}

void PRPPRequestLoaderFactoryImpl::CreateReqLoaderAndStart(const std::shared_ptr<PRRequestInfo>& info,
	bool only_send_reuse_request, const std::set<std::string>& need_record_header_urls)
{
  if (!url_request_context_ || !info) {
	LOG(WARNING) << "PRPPreload.PRPPRequestLoaderFactoryImpl::CreateReqLoaderAndStart failed, " <<
	  "invalid context or info";
	return;
  }
  
  if (!has_set_only_send_reuse_request_) {
    has_set_only_send_reuse_request_ = true;
    only_send_reuse_request_ = only_send_reuse_request;
  }

  std::string sub_url = info->url().spec();
  if (requests_already_start_set_.find(sub_url) != requests_already_start_set_.end()) {
	return;
  }

  if (!need_record_header_urls.empty() && need_record_header_urls_.empty()) {
	need_record_header_urls_ = need_record_header_urls;
  }

  if (((info->preload_flag() & PRPP_FLAGS_URL_DYNAMIC) == PRPP_FLAGS_URL_DYNAMIC) &&
	  only_send_reuse_request_) {
	(void)dynamic_urls_.emplace(sub_url);
	return;
  }

  if (((info->preload_flag() & PRPP_FLAGS_HDR_DYNAMIC) == PRPP_FLAGS_HDR_DYNAMIC)) {
	auto item = can_reuse_headers_map_.find(info->parent_for_dynamic_header().spec());
	if (item == can_reuse_headers_map_.end()) {
	  RecordPendingPRPPLoader(info);
	  return;
	}
	ReplaceHeaders(item->second, info);
  }
  std::shared_ptr<PRPPRequestLoader> prpp_req_loader = PRPPRequestLoader::CreatePRPPRequestLoader(
	main_url_, info, isolation_info_, url_request_context_,
	base::BindOnce(&PRPPRequestLoaderFactoryImpl::OnResPreloaded, weak_factory_.GetWeakPtr()));
  (void)prpp_req_loaders_.emplace(sub_url, prpp_req_loader);
}

void PRPPRequestLoaderFactoryImpl::SetPRPPIsolation(const net::IsolationInfo& isl)
{
  if (isl.IsEmpty()) {
	return;
  }

  isolation_info_ = isl;
}

std::shared_ptr<PRPPRequestLoader> PRPPRequestLoaderFactoryImpl::GetPRPPReqLoader(
	const network::URLLoaderContext& context, const network::ResourceRequest& resource_request,
	std::shared_ptr<PRRequestInfo> req_info_binding)
{
  req_info_binding->set_only_send_reuse_request(only_send_reuse_request_);
  RecordNeedRecordHeaderUrls(resource_request);
  if (!net::HttpUtil::IsMethodSafe(resource_request.method)) {
	LOG(DEBUG) << "PRPPreload.PRPPRequestLoaderFactoryImpl::GetPRPPReqLoader not support method:" <<
	  resource_request.method;
	return nullptr;
  }
  if (!isolation_info_.IsEmpty() && !context.GetFactoryParams().isolation_info.IsEmpty()) {
	if (isolation_info_.Serialize() != context.GetFactoryParams().isolation_info.Serialize()) {
	  LOG(WARNING) << "PRPPreload.PRPPRequestLoaderFactoryImpl::GetPRPPReqLoader isolationinfo not match";
	  return nullptr;
	}
  }
  std::string sub_url = resource_request.url.spec();
  PRPPReqLoaderMap::iterator it = prpp_req_loaders_.end();
  if (resource_request.is_preflight) {
	it = prpp_req_loaders_.find(std::string(PRPP_PREFLIGHT_PREFIX) + sub_url);
  } else {
	it = prpp_req_loaders_.find(sub_url);
  }
  if (it == prpp_req_loaders_.end()) {
	LOG(DEBUG) << "PRPPreload.PRPPRequestLoaderFactoryImpl::GetPRPPReqLoader not find";
	(void)requests_already_start_set_.emplace(sub_url);
	if (dynamic_urls_.find(sub_url) != dynamic_urls_.end() && only_send_reuse_request_) {
	  req_info_binding->set_preload_flag(PRPP_FLAGS_VISIBLE);
	}
	return nullptr;
  }
  std::shared_ptr<PRPPRequestLoader> loader = it->second;
  (void)prpp_req_loaders_.erase(it);
  if (!loader.get()) {
	LOG(WARNING) << "PRPPreload.PRPPRequestLoaderFactoryImpl::GetPRPPReqLoader loader is null";
	return nullptr;
  }
  if (loader->GetState() >= PRPPRequestLoader::STATE_UNSUPPORT) {
	LOG(WARNING) << "PRPPreload.PRPPRequestLoaderFactoryImpl::GetPRPPReqLoader loader invalid state";
	return nullptr;
  }
  std::shared_ptr<PRRequestInfo> info = loader->GetPRRequestInfo();
  if (!IsInfoMatched(info, context, resource_request, req_info_binding)) {
	return nullptr;
  }
  LOG(DEBUG) << "PRPPreload.PRPPRequestLoaderFactoryImpl::GetPRPPReqLoader find loader success";
  return loader;
}

bool PRPPRequestLoaderFactoryImpl::IsInfoMatched(std::shared_ptr<PRRequestInfo>& req_info,
	const network::URLLoaderContext& context, const network::ResourceRequest& resource_request,
	std::shared_ptr<PRRequestInfo>& req_info_binding)
{
  if (!req_info) {
	LOG(WARNING) << "PRPPreload.PRPPRequestLoaderFactoryImpl::IsInfoMatched no reqinfo";
	return false;
  }

  if (req_info->method() != resource_request.method) {
	LOG(DEBUG) << "PRPPreload.PRPPRequestLoaderFactoryImpl::IsInfoMatched check method failed";
	return false;
  }

  if (resource_request.request_body.get()) {
	LOG(DEBUG) << "PRPPreload.PRPPRequestLoaderFactoryImpl::IsInfoMatched has request body, not support";
	return false;
  }

  if (resource_request.trust_token_params) {
	req_info_binding->or_preload_flag(PRPP_FLAGS_HDR_NOT_MATCH);
	LOG(DEBUG) << "PRPPreload.PRPPRequestLoaderFactoryImpl::IsInfoMatched trust_token_params, not support";
	return false;
  }

  if ((req_info->cache_type() == PRRequestCacheType::FORCE_CACHE) &&
	  (base::Time::Now().ToInternalValue() < req_info->freshness_life_times()) &&
	  (((req_info->load_flags() & net::LOAD_BYPASS_CACHE) == 0) &&
	  ((req_info->load_flags() & net::LOAD_DISABLE_CACHE) == 0))) {
	return true;
  }

  return MatchRequestHeaders(req_info, resource_request, req_info_binding);
}

bool PRPPRequestLoaderFactoryImpl::MatchRequestHeaders(std::shared_ptr<PRRequestInfo>& req_info,
	const network::ResourceRequest& resource_request,
	std::shared_ptr<PRRequestInfo>& req_info_binding)
{
  bool match = true;
  std::set<std::string> dynamic_header_keys;
  for (auto item : resource_request.headers.GetHeaderVector()) {
	net::HttpRequestHeaders::HeaderVector::const_iterator iter =
	  req_info->extra_request_headers().GetHeaderVector().end();
	for (auto it = req_info->extra_request_headers().GetHeaderVector().begin();
	  it != req_info->extra_request_headers().GetHeaderVector().end(); ++it) {
	  if (base::EqualsCaseInsensitiveASCII(item.key, it->key)) {
		iter = it;
		break;
	  }
	}
	if (iter == req_info->extra_request_headers().GetHeaderVector().end()) {
	  req_info_binding->or_preload_flag(PRPP_FLAGS_HDR_NOT_MATCH);
	  LOG(DEBUG) << "PRPPreload.PRPPRequestLoaderFactoryImpl::MatchRequestHeaders header not match, " <<
        "can not reuse " << item.key;
	  return false;
	}

	if (item.value != iter->value) {
	  LOG(DEBUG) << "PRPPreload.PRPPRequestLoaderFactoryImpl::MatchRequestHeaders header value not match, " <<
        "can not reuse " << item.key;
	  match = false;
	  (void)dynamic_header_keys.emplace(item.key);
	}
  }

  if (!match) {
	req_info_binding->or_preload_flag(PRPP_FLAGS_HDR_DYNAMIC);
	req_info_binding->set_dynamic_header_keys(dynamic_header_keys);
  }
  return match;
}

void PRPPRequestLoaderFactoryImpl::OnResPreloaded(PRPPRequestLoader* prpp_loader)
{
  if (!prpp_loader) {
	return;
  }
  PRParallelPreloadMgr::GetInstance().UpdateIdlePrerequestCount(main_url_);
}

void PRPPRequestLoaderFactoryImpl::DoPendingCreateTask(const std::string& key)
{
  auto list_item = pending_prpp_loader_list_.find(key);
  if (list_item == pending_prpp_loader_list_.end()) {
	return;
  }
  for (auto info : list_item->second) {
	base::SingleThreadTaskRunner::GetCurrentDefault()->PostTask(
	  FROM_HERE, base::BindOnce(&PRPPRequestLoaderFactoryImpl::CreatePendingReqLoaderAndStart,
	  weak_factory_.GetWeakPtr(), info));
  }
}

void PRPPRequestLoaderFactoryImpl::CreatePendingReqLoaderAndStart(const std::shared_ptr<PRRequestInfo>& info)
{
  if (!url_request_context_) {
	LOG(WARNING) << "PRPPreload.PRPPRequestLoaderFactoryImpl::CreatePendingReqLoaderAndStart failed, " <<
	  "invalid context or info";
	return;
  }

  std::string sub_url = info->url().spec();
  if (requests_already_start_set_.find(sub_url) != requests_already_start_set_.end()) {
	return;
  }

  auto item = can_reuse_headers_map_.find(info->parent_for_dynamic_header().spec());
  if (item != can_reuse_headers_map_.end()) {
	ReplaceHeaders(item->second, info);
  }

  std::shared_ptr<PRPPRequestLoader> prpp_req_loader = PRPPRequestLoader::CreatePRPPRequestLoader(
	main_url_, info, isolation_info_, url_request_context_,
	base::BindOnce(&PRPPRequestLoaderFactoryImpl::OnResPreloaded, weak_factory_.GetWeakPtr()));
  (void)prpp_req_loaders_.emplace(sub_url, prpp_req_loader);
}

void PRPPRequestLoaderFactoryImpl::ReplaceHeaders(const net::HttpRequestHeaders& headers,
    const std::shared_ptr<PRRequestInfo>& info)
{
  for (auto key : info->dynamic_header_keys()) {
	std::string record_header;
	headers.GetHeader(key, &record_header);
	info->extra_request_headers().RemoveHeader(key);
	info->extra_request_headers().SetHeader(key, record_header);
  }
}

void PRPPRequestLoaderFactoryImpl::RecordPendingPRPPLoader(const std::shared_ptr<PRRequestInfo>& info)
{
  auto it = pending_prpp_loader_list_.find(info->parent_for_dynamic_header().spec());
  if (it != pending_prpp_loader_list_.end()) {
	it->second.push_back(info);
  } else {
	pending_prpp_loader_list_[info->parent_for_dynamic_header().spec()] =
	  std::list<std::shared_ptr<PRRequestInfo>>({info});
  }
}

void PRPPRequestLoaderFactoryImpl::RecordNeedRecordHeaderUrls(
    const network::ResourceRequest& resource_request)
{
  if (need_record_header_urls_.find(resource_request.url.spec()) != need_record_header_urls_.end()) {
    (void)can_reuse_headers_map_.emplace(resource_request.url.spec(), resource_request.headers);
    base::SingleThreadTaskRunner::GetCurrentDefault()->PostTask(
      FROM_HERE, base::BindOnce(&PRPPRequestLoaderFactoryImpl::DoPendingCreateTask,
      weak_factory_.GetWeakPtr(), resource_request.url.spec()));
    (void)need_record_header_urls_.erase(resource_request.url.spec());
  }
}

} // namespace ohos_prp_preload