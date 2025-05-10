// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "services/network/prp_preload/src/res_parallel_preload_ctrler.h"

#include "base/logging.h"

namespace {
static constexpr base::TimeDelta MAX_CHECK_FLUSH_TO_DISK_TIME = base::Seconds(5);
static constexpr uint32_t MAX_FILTER_INFO_COUNT = 30;
static constexpr base::TimeDelta MAX_FILTER_INFO_TIME = base::Seconds(2);
static scoped_refptr<ohos_prp_preload::DiskCacheBackendFactory> g_disk_cache_backend_factory;
}  // namespace

namespace ohos_prp_preload {
ResParallelPreloadCtrler::ResParallelPreloadCtrler(const std::string& url,
  const net::NetworkAnonymizationKey& networkAnonymizationKey,
  const scoped_refptr<base::SingleThreadTaskRunner>& sth_task_runner,
  const PRPPCtrlerTimeoutCB& timeout_cb) :
    url_(url), networkAnonymizationKey_(networkAnonymizationKey),
    sth_task_runner_(sth_task_runner), timeout_cb_(timeout_cb) { }

void ResParallelPreloadCtrler::InitDiskCacheBackendFactory()
{
  g_disk_cache_backend_factory = base::WrapRefCounted(new (std::nothrow) DiskCacheBackendFactory());
  if (g_disk_cache_backend_factory != nullptr) {
    g_disk_cache_backend_factory->CreateBackend();
  }
}

void ResParallelPreloadCtrler::Init(const scoped_refptr<base::SingleThreadTaskRunner>& net_task_runner,
    base::WeakPtr<net::URLRequestContext> url_request_context,
    const PRPPOnPageOriginCB& on_page_origin_cb) {
  if (sth_task_runner_ == nullptr) {
    return;
  }
  sth_task_runner_->PostTask(FROM_HERE, base::BindOnce(&ResParallelPreloadCtrler::DoInit, weak_factory_.GetWeakPtr(),
    net_task_runner, url_request_context, on_page_origin_cb));
}

void ResParallelPreloadCtrler::Start() {
  if (sth_task_runner_ == nullptr) {
    return;
  }
  sth_task_runner_->PostTask(FROM_HERE,
    base::BindOnce(&ResParallelPreloadCtrler::DoStart, weak_factory_.GetWeakPtr()));

  sth_task_runner_->PostDelayedTask(FROM_HERE,
    base::BindOnce(&ResParallelPreloadCtrler::OnTimeout, weak_factory_.GetWeakPtr()),
    MAX_CHECK_FLUSH_TO_DISK_TIME);
}

void ResParallelPreloadCtrler::Stop() {
  if (sth_task_runner_ == nullptr) {
    return;
  }
  sth_task_runner_->PostTask(FROM_HERE,
    base::BindOnce(&ResParallelPreloadCtrler::DoStop, weak_factory_.GetWeakPtr()));
}

void ResParallelPreloadCtrler::UpdateResRequestInfo(const std::shared_ptr<PRRequestInfo>& info) {
  if (sth_task_runner_ == nullptr) {
    return;
  }
  sth_task_runner_->PostTask(FROM_HERE, base::BindOnce(&ResParallelPreloadCtrler::DoUpdateResRequestInfo,
    weak_factory_.GetWeakPtr(), info));
}

void ResParallelPreloadCtrler::SetPageOrigin(const std::string& page_origin)
{
  if (sth_task_runner_ == nullptr) {
    return;
  }
  sth_task_runner_->PostTask(FROM_HERE, base::BindOnce(&ResParallelPreloadCtrler::DoSetPageOrigin,
    weak_factory_.GetWeakPtr(), std::move(page_origin)));
}

void ResParallelPreloadCtrler::SetPRPPReqLoaderFac(base::WeakPtr<PRPPRequestLoaderFactory> loader_fac_weak)
{
  if (sth_task_runner_ == nullptr) {
    return;
  }
  sth_task_runner_->PostTask(FROM_HERE, base::BindOnce(&ResParallelPreloadCtrler::DoSetPRPPReqLoaderFac,
    weak_factory_.GetWeakPtr(), loader_fac_weak));
}

void ResParallelPreloadCtrler::UpdateIdlePrerequestCount()
{
  if (sth_task_runner_ == nullptr) {
    return;
  }
  sth_task_runner_->PostTask(FROM_HERE, base::BindOnce(&ResParallelPreloadCtrler::DoUpdateIdlePrerequestCount,
    weak_factory_.GetWeakPtr()));
}

void ResParallelPreloadCtrler::DoUpdateIdlePrerequestCount()
{
  if (res_preload_scheduler_ == nullptr) {
    return;
  }
  res_preload_scheduler_->UpdateIdlePrerequestCount();
}

void ResParallelPreloadCtrler::DoSetPageOrigin(const std::string& page_origin)
{
  if (res_req_info_updater_ == nullptr) {
    return;
  }
  res_req_info_updater_->SetPageOrigin(page_origin);
}

void ResParallelPreloadCtrler::DoSetPRPPReqLoaderFac(base::WeakPtr<PRPPRequestLoaderFactory> loader_fac_weak)
{
  if (res_preload_scheduler_ == nullptr) {
    return;
  }
  res_preload_scheduler_->SetPRPPReqLoaderFac(loader_fac_weak);
}

void ResParallelPreloadCtrler::DoInit(const scoped_refptr<base::SingleThreadTaskRunner>& net_task_runner,
    base::WeakPtr<net::URLRequestContext> url_request_context,
    const PRPPOnPageOriginCB& on_page_origin_cb) {
  if (g_disk_cache_backend_factory == nullptr) {
    LOG(WARNING) << "PRPPreload.ResParallelPreloadCtrler::DoInit DiskCacheBackendFactory is null";
    return;
  }
  res_req_info_updater_ = base::WrapRefCounted(new (std::nothrow) ResRequestInfoUpdater(
    url_, networkAnonymizationKey_, sth_task_runner_, g_disk_cache_backend_factory,
    base::BindRepeating(&ResParallelPreloadCtrler::OnResPreloadInfos, weak_factory_.GetWeakPtr())));
  if (res_req_info_updater_ == nullptr) {
    LOG(WARNING) << "PRPPreload.ResParallelPreloadCtrler::Init new ResRequestInfoUpdater failed";
    return;
  }
  res_preload_scheduler_ = base::WrapRefCounted(new (std::nothrow) ResPreloadScheduler(
    url_, sth_task_runner_, net_task_runner, url_request_context, on_page_origin_cb));
  if (res_preload_scheduler_ == nullptr) {
    LOG(WARNING) << "PRPPreload.ResParallelPreloadCtrler::Init new ResPreloadScheduler failed";
    return;
  }
}

void ResParallelPreloadCtrler::DoStart() {
  if (res_req_info_updater_ == nullptr) {
    return;
  }
  res_req_info_updater_->Start();
}

void ResParallelPreloadCtrler::DoStop() {
  if ((res_req_info_updater_ == nullptr) ||
      (res_preload_scheduler_ == nullptr)) {
    return;
  }
  res_preload_scheduler_->StopPreload();
  res_req_info_updater_->Stop();
}

void ResParallelPreloadCtrler::DoUpdateResRequestInfo(const std::shared_ptr<PRRequestInfo>& info) {
  if ((info->preload_flag() == PRPP_FLAGS_NONE) && !start_filter_info_
      && sth_task_runner_ != nullptr) {
    start_filter_info_ = true;
    sth_task_runner_->PostDelayedTask(FROM_HERE,
      base::BindOnce(&ResParallelPreloadCtrler::OnTimeout, weak_factory_.GetWeakPtr()),
      MAX_FILTER_INFO_TIME);
  }
  if (start_filter_info_) {
    if (filter_count_ > MAX_FILTER_INFO_COUNT) {
      return;
    }
    ++filter_count_;
    if (filter_count_ == MAX_FILTER_INFO_COUNT && sth_task_runner_ != nullptr) {
      sth_task_runner_->PostTask(FROM_HERE,
        base::BindOnce(&ResParallelPreloadCtrler::OnTimeout, weak_factory_.GetWeakPtr()));
    }
  }

  if (res_req_info_updater_ != nullptr) {
    res_req_info_updater_->UpdateResRequestInfo(info);
  }
}

void ResParallelPreloadCtrler::OnResPreloadInfos(const PRPPPreconnectInfoList& preconnect_info_list,
  const std::shared_ptr<PRPPReqInfoTreeNode>& preload_info_tree, bool only_send_reuse_request,
  const std::set<std::string>& need_record_header_urls) {
  if (res_preload_scheduler_ == nullptr) {
    return;
  }
  res_preload_scheduler_->SchedulePreloads(preconnect_info_list, preload_info_tree,
    only_send_reuse_request, need_record_header_urls);
}

void ResParallelPreloadCtrler::OnTimeout() {
  DoStop();
  if (!timeout_cb_.is_null()) {
    timeout_cb_.Run(url_);
  }
}

}  // namespace ohos_prp_preload
