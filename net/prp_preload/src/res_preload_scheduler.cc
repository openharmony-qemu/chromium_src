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

#include "res_preload_scheduler.h"

#include "base/logging.h"
#include "preconnect_runner.h"
#include "url/origin.h"

namespace {
constexpr uint8_t SOCKET_LIMIT = 30;
constexpr uint8_t DELAYED_TIME = 5;
constexpr uint8_t MAX_PRECONNECT_NUM = 6;
const std::string PRIVACY_TAG = "ac/";
}  // namespace

namespace ohos_prp_preload {

ResPreloadScheduler::ResPreloadScheduler(
    const scoped_refptr<base::SingleThreadTaskRunner>& sth_task_runner,
    const scoped_refptr<base::SingleThreadTaskRunner>& net_task_runner,
    base::WeakPtr<net::URLRequestContext> url_request_context) :
    sth_task_runner_(sth_task_runner), net_task_runner_(net_task_runner), url_request_context_(url_request_context) {}

void ResPreloadScheduler::PreloadSchedule(
    const std::list<std::shared_ptr<PRRequestInfo>>& res_req_info_list,
    const net::NetworkAnonymizationKey& network_anonymization_key) {
  LOG(INFO) << "PRPPreload.ResPreloadScheduler::PreloadSchedule " << res_req_info_list.size();
  if (preload_triggered_) {
    LOG(INFO) << "PRPPreload.ResPreloadScheduler::PreloadSchedule called more than once";
    return;
  }
  preload_triggered_ = true;
  info_list_ = res_req_info_list;
  ++info_list_version_;
  socket_connected_ = 0;
  InfoIter iter = info_list_.begin();

  while (iter != info_list_.end()) {
    auto info = *iter;
    if (info->cache_type() != PRRequestCacheType::NEGOTIATION_CACHE ||
        base::Time::Now().ToInternalValue() > info->freshness_life_times()) {
      if (socket_connected_ <= SOCKET_LIMIT) {
        auto url = url::Origin::Create(info->url());
        if (NeedToPreconnect(url.GetURL(), info->allow_credentials()) && net_task_runner_ != nullptr) {
          ++socket_connected_;
          LOG(INFO) << "PRPPreload.ResPreloadScheduler::PreloadSchedule preconnect " << url;
          net_task_runner_->PostTask(FROM_HERE, base::BindOnce(&PreconnectRunner::PreconnectSocket,
                                     url.GetURL(), info->allow_credentials(), url_request_context_,
                                     network_anonymization_key));
        }
      } else {
        if (sth_task_runner_ != nullptr) {
          sth_task_runner_->PostDelayedTask(FROM_HERE,
              base::BindOnce(&ResPreloadScheduler::PreconnectBeyondLimit, weak_factory_.GetWeakPtr(),
              iter, info_list_version_, network_anonymization_key), base::Milliseconds(DELAYED_TIME));
        }
        return;
      }
    } else {
      LOG(INFO) << "PRPPreload.ResPreloadScheduler::PreloadSchedule use cache " << info->url();
    }
    ++iter;
  }
}

void ResPreloadScheduler::StopPreload() {
  preload_triggered_ = false;
  info_list_.clear();
  idle_connect_list_.clear();
}

bool ResPreloadScheduler::NeedToPreconnect(const GURL& url, bool allow_credentials) {
  std::string info = allow_credentials ? PRIVACY_TAG + url.spec() : url.spec();
  auto it = idle_connect_list_.find(info);
  if (it != idle_connect_list_.end()) {
    return --it->second > 0 ? true : false;
  } else {
    idle_connect_list_[info] = MAX_PRECONNECT_NUM;
    return true;
  }
}

void ResPreloadScheduler::PreconnectBeyondLimit(
  InfoIter info_iter,
  const int info_list_version,
  const net::NetworkAnonymizationKey& network_anonymization_key) {
  if (info_list_version != info_list_version_ || info_iter == info_list_.end() ||
      !preload_triggered_) {
    return;
  }

  auto info = *info_iter;
  if (info != nullptr && (info->cache_type() != PRRequestCacheType::NEGOTIATION_CACHE ||
      base::Time::Now().ToInternalValue() > info->freshness_life_times())) {
    auto url = url::Origin::Create(info->url());
    if (NeedToPreconnect(url.GetURL(), info->allow_credentials()) && net_task_runner_ != nullptr) {
      ++socket_connected_;
      LOG(INFO) << "PRPPreload.ResPreloadScheduler::PreconnectBeyondLimit preconnect " << url;
      net_task_runner_->PostTask(FROM_HERE, base::BindOnce(&PreconnectRunner::PreconnectSocket,
                                 url.GetURL(), info->allow_credentials(), url_request_context_,
                                 network_anonymization_key));
    }
  }
  ++info_iter;
  if (sth_task_runner_ != nullptr && info_iter != info_list_.end()) {
    sth_task_runner_->PostDelayedTask(FROM_HERE,
        base::BindOnce(&ResPreloadScheduler::PreconnectBeyondLimit, weak_factory_.GetWeakPtr(),
        info_iter, info_list_version, network_anonymization_key), base::Milliseconds(DELAYED_TIME));
  }
}
}  // namespace ohos_prp_preload
