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

#include "base/command_line.h"
#include "net/base/features.h"
#include "page_res_parallel_preload_mgr_impl.h"
#include "base/logging.h"

namespace {
static ohos_prp_preload::PRParallelPreloadMgrImpl g_prp_preload_mgr_impl;
const size_t MAX_PAGE_COUNT = 100;
}  // namespace

namespace ohos_prp_preload {
PRParallelPreloadMgr& PRParallelPreloadMgr::GetInstance() {
  return g_prp_preload_mgr_impl;
}
bool PRParallelPreloadMgr::PRParallelPreloadEnabled() {
  return base::FeatureList::IsEnabled(net::features::kEnableNetworkPreload);
}

void PRParallelPreloadMgrImpl::Init(
  const scoped_refptr<base::SingleThreadTaskRunner>& net_task_runner,
  const base::FilePath& cache_path) {
  if (!PRParallelPreloadMgr::GetInstance().PRParallelPreloadEnabled()) {
    LOG(ERROR) << "PRPPreload.PRParallelPreloadMgrImpl::Init NOT ENABLE";
    return;
  }
  bool inited = false;
  if (is_inited_.compare_exchange_strong(inited, true)) {
    sth_task_runner_ = base::ThreadPool::CreateSingleThreadTaskRunner(
        {base::TaskPriority::USER_VISIBLE},
        base::SingleThreadTaskRunnerThreadMode::DEDICATED);
    if (sth_task_runner_ == nullptr || net_task_runner == nullptr) {
      is_inited_.store(false);
      LOG(ERROR) << "PRPPreload.PRParallelPreloadMgrImpl::Init failed";
      return;
    }
    net_task_runner_ = net_task_runner;
    sth_task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(&ResParallelPreloadCtrler::InitDiskCacheBackendFactory,
                       std::move(cache_path)));
  }
}

void PRParallelPreloadMgrImpl::StartMainPage(
  const std::string& url,
  const net::NetworkAnonymizationKey& network_anonymization_key,
  base::WeakPtr<net::URLRequestContext> url_request_context,
  uint64_t addr_web_handle) {
  void* web_handle = reinterpret_cast<void*>(addr_web_handle);
  if (url.empty() || url == "about:blank" || web_handle == nullptr) {
    LOG(ERROR) << "PRPPreload.PRParallelPreloadMgrImpl::StartMainPage failed, invalid args";
    return;
  }
  if (!is_inited_) {
    return;
  }

  std::lock_guard<std::mutex> mgr_guard(mgr_mutex_);
  if (!RecycleRPPCtrler()) {
    LOG(ERROR) << "PRPPreload.PRParallelPreloadMgrImpl::StartMainPage RecycleRPPCtrler failed";
    return;
  }
  auto it = prp_preload_info_map_.find(url);
  if (it != prp_preload_info_map_.end()) {
    return;
  }

  scoped_refptr<ResParallelPreloadCtrler> rp_preload_ctrler =
      base::WrapRefCounted(new (std::nothrow) ResParallelPreloadCtrler(
          url, network_anonymization_key, url_request_context, sth_task_runner_,
          net_task_runner_,
          base::BindRepeating(&PRParallelPreloadMgrImpl::OnRPPCtrlerTimeout,
                              weak_factory_.GetWeakPtr())));
  if (rp_preload_ctrler == nullptr) {
    return;
  }
  rp_preload_ctrler->Init(net_task_runner_, url_request_context);
  
  auto it_web = web_handle_pages_map_.find(web_handle);
  if (it_web != web_handle_pages_map_.end()) {
    if (it_web->second != url) {
      StopMainPageInternal(it_web->second);
      it_web->second = url;
    }
  } else {
    web_handle_pages_map_[web_handle] = url;
  }
  rp_preload_ctrler->Start();
  prp_preload_info_map_[url].rp_preload_ctrler_ = rp_preload_ctrler;
  prp_preload_info_map_[url].start_page_ = true;
}

void PRParallelPreloadMgrImpl::StopMainPage(const std::string& url) {
  if (!is_inited_) {
    return;
  }

  std::lock_guard<std::mutex> mgr_guard(mgr_mutex_);
  StopMainPageInternal(url);
}

void PRParallelPreloadMgrImpl::StopMainPage(uint64_t addr_web_handle) {
  if (!is_inited_) {
    return;
  }

  std::lock_guard<std::mutex> mgr_guard(mgr_mutex_);
  void* web_handle = reinterpret_cast<void*>(addr_web_handle);
  if (web_handle == nullptr) {
    return;
  }

  auto it_web = web_handle_pages_map_.find(web_handle);
  if (it_web != web_handle_pages_map_.end()) {
    StopMainPageInternal(it_web->second);
    (void)web_handle_pages_map_.erase(it_web);
  }
}

void PRParallelPreloadMgrImpl::UpdateResRequestInfo(const std::string& key,
                                                    const std::shared_ptr<PRRequestInfo>& info) {
  if (!is_inited_) {
    return;
  }

  std::lock_guard<std::mutex> mgr_guard(mgr_mutex_);
  auto it = prp_preload_info_map_.find(key);
  if (it != prp_preload_info_map_.end()) {
    if (!it->second.start_page_) {
      return;
    } else {
      it->second.rp_preload_ctrler_->UpdateResRequestInfo(info);
    }
  }
}

void PRParallelPreloadMgrImpl::StopMainPageInternal(const std::string& url) {
  auto it = prp_preload_info_map_.find(url);
  if (it != prp_preload_info_map_.end()) {
    if (it->second.start_page_) {
      it->second.rp_preload_ctrler_->Stop();
      it->second.start_page_ = false;
      stopped_pages_.push_back(url);
    }
  }
}

void PRParallelPreloadMgrImpl::OnRPPCtrlerTimeout(const std::string& url) {
  std::lock_guard<std::mutex> mgr_guard(mgr_mutex_);
  stopped_pages_.emplace_back(url);
}

bool PRParallelPreloadMgrImpl::RecycleRPPCtrler() {
  if ((stopped_pages_.size() <= 0) || (prp_preload_info_map_.size() <= MAX_PAGE_COUNT)) {
    return true;
  }
  bool recycled = false;
  do {
    auto it = prp_preload_info_map_.find(stopped_pages_.front());
    stopped_pages_.pop_front();
    if (it == prp_preload_info_map_.end()) {
      continue;
    }
    auto ctrler = it->second.rp_preload_ctrler_;
    prp_preload_info_map_.erase(it);
    recycled = true;
    sth_task_runner_->PostTask(FROM_HERE,
        base::BindOnce([](const scoped_refptr<ResParallelPreloadCtrler>& ctrler) {},
        std::move(ctrler)));
    break;
  } while (stopped_pages_.size() > 0);
  if (!recycled) {
    LOG(ERROR) << "PRPPreload.PRParallelPreloadMgrImpl::RecycleRPPCtrler failed";
  }

  return recycled;
}

}  // namespace ohos_prp_preload
