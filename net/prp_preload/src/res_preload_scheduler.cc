// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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

ResPreloadScheduler::ResPreloadScheduler(const scoped_refptr<base::SingleThreadTaskRunner>& sth_task_runner,
  const scoped_refptr<base::SingleThreadTaskRunner>& net_task_runner,
  base::WeakPtr<net::URLRequestContext> url_request_context) :
  sth_task_runner_(sth_task_runner), net_task_runner_(net_task_runner), url_request_context_(url_request_context) {}

void ResPreloadScheduler::PreloadSchedule(const std::list<std::shared_ptr<PRRequestInfo>>& res_req_info_list) {
  LOG(DEBUG) << "PRPPreload.ResPreloadScheduler::PreloadSchedule " << res_req_info_list.size();
  if (preload_triggered_) {
    LOG(DEBUG) << "PRPPreload.ResPreloadScheduler::PreloadSchedule called more than once";
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
          net_task_runner_->PostTask(FROM_HERE, base::BindOnce(&PreconnectRunner::PreconnectSocket,
                                     url.GetURL(), info->allow_credentials(), url_request_context_));
        }
      } else {
        if (sth_task_runner_ != nullptr) {
          sth_task_runner_->PostDelayedTask(FROM_HERE,
            base::BindOnce(&ResPreloadScheduler::PreconnectBeyondLimit, weak_factory_.GetWeakPtr(),
            iter, info_list_version_), base::Milliseconds(DELAYED_TIME));
        }
        return;
      }
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

void ResPreloadScheduler::PreconnectBeyondLimit(InfoIter info_iter,
                                                const int info_list_version) {
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
      net_task_runner_->PostTask(FROM_HERE, base::BindOnce(&PreconnectRunner::PreconnectSocket,
                                 url.GetURL(), info->allow_credentials(), url_request_context_));
    }
  }
  ++info_iter;
  if (sth_task_runner_ != nullptr && info_iter != info_list_.end()) {
    sth_task_runner_->PostDelayedTask(FROM_HERE,
      base::BindOnce(&ResPreloadScheduler::PreconnectBeyondLimit, weak_factory_.GetWeakPtr(),
      info_iter, info_list_version), base::Milliseconds(DELAYED_TIME));
  }
}

}  // namespace ohos_prp_preload