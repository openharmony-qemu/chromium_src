/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "res_parallel_preload_ctrler.h"

#include "base/logging.h"

namespace {
static constexpr base::TimeDelta MAX_CHECK_FLUSH_TO_DISK_TIME = base::Seconds(5);
static scoped_refptr<ohos_prp_preload::DiskCacheBackendFactory> g_disk_cache_backend_factory;
}  // namespace

namespace ohos_prp_preload {
ResParallelPreloadCtrler::ResParallelPreloadCtrler(
    const std::string& url,
    const net::NetworkAnonymizationKey& network_anonymization_key,
    base::WeakPtr<net::URLRequestContext> url_request_context,
    const scoped_refptr<base::SingleThreadTaskRunner>& sth_task_runner,
    const scoped_refptr<base::SingleThreadTaskRunner>& net_task_runner,
    const RPPCtrlerTimeoutCB& timeout_cb)
    : url_(url),
      network_anonymization_key_(network_anonymization_key),
      sth_task_runner_(sth_task_runner),
      timeout_cb_(timeout_cb) {}

//static
void ResParallelPreloadCtrler::InitDiskCacheBackendFactory(const base::FilePath& cache_path)
{
  g_disk_cache_backend_factory = base::WrapRefCounted(new (std::nothrow) DiskCacheBackendFactory());
  if (g_disk_cache_backend_factory != nullptr) {
    g_disk_cache_backend_factory->CreateBackend(cache_path);
  }
}

void ResParallelPreloadCtrler::Init(
    const scoped_refptr<base::SingleThreadTaskRunner>& net_task_runner,
    base::WeakPtr<net::URLRequestContext> url_request_context) {
  if (sth_task_runner_ == nullptr) {
    return;
  }
    sth_task_runner_->PostTask(FROM_HERE, base::BindOnce(&ResParallelPreloadCtrler::DoInit, weak_factory_.GetWeakPtr(),
    net_task_runner, url_request_context));
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

void ResParallelPreloadCtrler::DoInit(
    const scoped_refptr<base::SingleThreadTaskRunner>& net_task_runner,
    base::WeakPtr<net::URLRequestContext> url_request_context) {
  if (g_disk_cache_backend_factory == nullptr) {
    LOG(WARNING) << "PRPPreload.ResParallelPreloadCtrler::DoInit DiskCacheBackendFactory is null";
    return;
  }
  res_req_info_updater_ = base::WrapRefCounted(new (std::nothrow) ResRequestInfoUpdater(
    url_, network_anonymization_key_, sth_task_runner_, g_disk_cache_backend_factory,
    base::BindRepeating(&ResParallelPreloadCtrler::OnResRequestInfoList, weak_factory_.GetWeakPtr())));
  if (res_req_info_updater_ == nullptr) {
    LOG(WARNING) << "PRPPreload.ResParallelPreloadCtrler::DoInit new ResRequestInfoUpdater failed";
    return;
  }
  res_preload_scheduler_ = base::WrapRefCounted(new (std::nothrow) ResPreloadScheduler(
    sth_task_runner_, net_task_runner, url_request_context));
  if (res_preload_scheduler_ == nullptr) {
    LOG(WARNING) << "PRPPreload.ResParallelPreloadCtrler::DoInit new ResPreloadScheduler failed";
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
  if ((res_req_info_updater_ == nullptr) || (res_preload_scheduler_ == nullptr)) {
    return;
  }
  res_preload_scheduler_->StopPreload();
  res_req_info_updater_->Stop();
}

void ResParallelPreloadCtrler::DoUpdateResRequestInfo(const std::shared_ptr<PRRequestInfo>& info) {
  if (res_req_info_updater_ == nullptr) {
    return;
  }
  res_req_info_updater_->UpdateResRequestInfo(info);
}

void ResParallelPreloadCtrler::OnResRequestInfoList(
    const std::list<std::shared_ptr<PRRequestInfo>>& res_req_info_list,
    const net::NetworkAnonymizationKey& network_anonymization_key) {
  if ((res_req_info_updater_ == nullptr) || (res_preload_scheduler_ == nullptr)) {
    return;
  }
  res_preload_scheduler_->PreloadSchedule(res_req_info_list, network_anonymization_key);
}

void ResParallelPreloadCtrler::OnTimeout() {
  DoStop();
  if (!timeout_cb_.is_null()) {
    timeout_cb_.Run(url_);
  }
}

}  // namespace ohos_prp_preload
