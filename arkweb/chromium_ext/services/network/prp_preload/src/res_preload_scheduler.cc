// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/chromium_ext/services/network/prp_preload/src/res_preload_scheduler.h"

#include "arkweb/chromium_ext/services/network/prp_preload/include/page_res_parallel_preload_mgr.h"
#include "arkweb/chromium_ext/services/network/prp_preload/src/preload_runner/preconnect_runner.h"
#include "base/logging.h"
#include "net/http/http_util.h"

namespace {
constexpr uint8_t SOCKET_LIMIT = 30;
constexpr uint8_t MAX_REQUEST_COUNT = 12;
constexpr uint8_t DELAYED_TIME = 5;
const std::string PRIVACY_TAG = "ac/";
const std::string CANCEL_ORIGIN = "origin.DEFAULT.Cancel";
}  // namespace

namespace ohos_prp_preload {

ResPreloadScheduler::ResPreloadScheduler(const std::string& url,
    const scoped_refptr<base::SingleThreadTaskRunner>& net_task_runner,
    base::WeakPtr<net::URLRequestContext> url_request_context,
    const PRPPOnPageOriginCB& on_page_origin_cb) :
  url_(raw_ref(url)), net_task_runner_(net_task_runner),
  url_request_context_(url_request_context), on_page_origin_cb_(on_page_origin_cb) {}

void ResPreloadScheduler::CreateReqLoaderAndStart(const std::shared_ptr<PRRequestInfo>& info)
{
  if (net_task_runner_ == nullptr) {
    return;
  }
  if (!loader_fac_weak_valid_) {
    prerequest_info_list_.push_back(info);
    return;
  }
  net_task_runner_->PostTask(FROM_HERE, base::BindOnce(&PRPPRequestLoaderFactory::CreateReqLoaderAndStart,
    loader_fac_weak_, info, need_record_header_urls_));
  if (!need_record_header_urls_.empty()) {
    need_record_header_urls_.clear();
  }
}

void ResPreloadScheduler::StopPreload()
{
  preload_triggered_ = false;
  idle_connect_list_.clear();
  prerequest_info_list_.clear();
  prpp_preconnect_info_list_.clear();
  preload_info_tree_ = nullptr;
  cur_parent_ = nullptr;
  need_record_header_urls_.clear();
}

void ResPreloadScheduler::SetPRPPReqLoaderFac(base::WeakPtr<PRPPRequestLoaderFactory> loader_fac_weak, bool valid)
{
  loader_fac_weak_ = loader_fac_weak;
  loader_fac_weak_valid_ = valid;
  if (!loader_fac_weak_valid_) {
    LOG(WARNING) << "PRPPreload.ResPreloadScheduler::SetPRPPReqLoaderFac invalid loader_fac_weak";
    return;
  }
  if (preload_triggered_) {
    SchedulePrerequests(MAX_REQUEST_COUNT, info_list_version_);
  }
}

void ResPreloadScheduler::SchedulePreloads(const PRPPPreconnectInfoList& preconnect_info_list,
    const std::shared_ptr<PRPPReqInfoTreeNode>& preload_info_tree,
    const std::set<std::string>& need_record_header_urls)
{
  if (preload_triggered_ || net_task_runner_ == nullptr) {
    return;
  }
  preload_triggered_ = true;
  prpp_preconnect_info_list_ = preconnect_info_list;
  preload_info_tree_ = preload_info_tree;
  ++info_list_version_;
  need_record_header_urls_ = need_record_header_urls;

  SchedulePreconnects();
  if (!preload_info_tree_ ||
      !preload_info_tree_->req_info_ ||
      (preload_info_tree_->req_info_->type() != PRRequestInfoType::TYPE_PAGE_ORIGIN) ||
      (preload_info_tree_->children_.size() == 0)) {
    if (!on_page_origin_cb_.is_null()) {
      net_task_runner_->PostTask(FROM_HERE, base::BindOnce(on_page_origin_cb_, *url_, CANCEL_ORIGIN));
    }
    LOG(DEBUG) << "PRPPreload.ResPreloadScheduler::SchedulePreloads invalid tree or no TYPE_PAGE_ORIGIN";
    return;
  }

  std::string origin = preload_info_tree_->req_info_->page_origin();
  if (!on_page_origin_cb_.is_null()) {
    net_task_runner_->PostTask(FROM_HERE, base::BindOnce(on_page_origin_cb_, *url_, std::move(origin)));
  }
  cur_parent_ = preload_info_tree_;
  cur_node_iter_ = preload_info_tree_->children_.begin();
  base::SingleThreadTaskRunner::GetCurrentDefault()->PostTask(FROM_HERE, base::BindOnce(
    &ResPreloadScheduler::SchedulePrerequests, weak_factory_.GetWeakPtr(), MAX_REQUEST_COUNT, info_list_version_));
}

void ResPreloadScheduler::SchedulePreconnects()
{
  if (net_task_runner_ == nullptr) {
    return;
  }
  PreconnectInfoListIter iter = prpp_preconnect_info_list_.begin();
  int32_t preconnect_num = 0;
  while (iter != prpp_preconnect_info_list_.end()) {
    PRPPPreconnectInfo& info = *iter;
    preconnect_num++;
    if (preconnect_num <= SOCKET_LIMIT) {
      net_task_runner_->PostTask(FROM_HERE, base::BindOnce(&PreconnectRunner::PreconnectSocket,
        info.origin_url_, info.allow_credential_, info.net_anonymization_key_, url_request_context_));
    } else {
      base::SingleThreadTaskRunner::GetCurrentDefault()->PostDelayedTask(FROM_HERE,
        base::BindOnce(&ResPreloadScheduler::ContinueSchedulePreconnects, weak_factory_.GetWeakPtr(),
        iter, info_list_version_), base::Milliseconds(DELAYED_TIME));
      break;
    }
    ++iter;
  }
}

void ResPreloadScheduler::ContinueSchedulePreconnects(PreconnectInfoListIter preconnect_infos_iter,
    int32_t info_list_version)
{
  if (info_list_version != info_list_version_ ||
      (preconnect_infos_iter == prpp_preconnect_info_list_.end()) ||
      !preload_triggered_ || net_task_runner_ == nullptr) {
    return;
  }

  PRPPPreconnectInfo& info = *preconnect_infos_iter;
  
  net_task_runner_->PostTask(FROM_HERE, base::BindOnce(&PreconnectRunner::PreconnectSocket,
    info.origin_url_, info.allow_credential_, info.net_anonymization_key_, url_request_context_));

  ++preconnect_infos_iter;
  if (preconnect_infos_iter != prpp_preconnect_info_list_.end()) {
    base::SingleThreadTaskRunner::GetCurrentDefault()->PostDelayedTask(FROM_HERE,
      base::BindOnce(&ResPreloadScheduler::ContinueSchedulePreconnects, weak_factory_.GetWeakPtr(),
      preconnect_infos_iter, info_list_version), base::Milliseconds(DELAYED_TIME));
  }
}

void ResPreloadScheduler::SchedulePrerequests(uint32_t limit, int32_t info_list_version)
{
  if ((info_list_version != info_list_version_) ||
      !preload_triggered_ || !cur_parent_ ||
      net_task_runner_ == nullptr || !loader_fac_weak_valid_) {
    return;
  }

  uint32_t prerequest_num = 0;
  if ((limit == 0) && (idle_prerequest_count_ == 0)) {
    base::SingleThreadTaskRunner::GetCurrentDefault()->PostDelayedTask(FROM_HERE,
      base::BindOnce(&ResPreloadScheduler::SchedulePrerequests,
      weak_factory_.GetWeakPtr(), 0, info_list_version),
      base::Milliseconds(DELAYED_TIME));
    return;
  }
  limit += static_cast<uint32_t>(idle_prerequest_count_);
  limit = limit > MAX_REQUEST_COUNT ? MAX_REQUEST_COUNT : limit;
  idle_prerequest_count_ = 0;
  bool need_continue = false;
  while (cur_parent_->children_.size() > 0) {
    for (auto child = cur_node_iter_; child != cur_parent_->children_.end(); child++) {
      cur_node_iter_ = child;
      if (!net::HttpUtil::IsMethodSafe((*child)->req_info_->method()) ||
          (((*child)->req_info_->preload_flag() & PRPP_FLAGS_HDR_NOT_MATCH) ==
            PRPP_FLAGS_HDR_NOT_MATCH) ||
          (((*child)->req_info_->preload_flag() & PRPP_FLAGS_UNSUPPORT) ==
            PRPP_FLAGS_UNSUPPORT)) {
        continue;
      }
      prerequest_num++;
      if (prerequest_num > limit) {
        need_continue = true;
        break;
      }
      CreateReqLoaderAndStart((*child)->req_info_);
    }
    if (need_continue) {
      base::SingleThreadTaskRunner::GetCurrentDefault()->PostDelayedTask(FROM_HERE,
        base::BindOnce(&ResPreloadScheduler::SchedulePrerequests, weak_factory_.GetWeakPtr(),
        0, info_list_version), base::Milliseconds(DELAYED_TIME));
      break;
    }
    cur_parent_ = *(cur_parent_->children_.begin());
    cur_node_iter_ = cur_parent_->children_.begin();
  }
}

void ResPreloadScheduler::UpdateIdlePrerequestCount()
{
  idle_prerequest_count_++;
}

}  // namespace ohos_prp_preload