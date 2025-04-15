// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "services/network/prp_preload/src/res_request_info_updater.h"

#include "base/logging.h"
#include "base/ohos/sys_info_utils.h"
#include "net/http/http_util.h"
#include "services/network/prp_preload/include/page_res_parallel_preload_mgr.h"

namespace {
constexpr int32_t MAX_LEVEL_INTERVAL_US = 100000;
const std::string PRIVACY_TAG = "ac/";
constexpr int32_t MAX_RESERVED_COUNT = 1;
constexpr int32_t MAX_PRECONNECT_COUNT = 5;
} // namespace

namespace ohos_prp_preload {
ResRequestInfoUpdater::ResRequestInfoUpdater(const std::string& url,
  const net::NetworkAnonymizationKey& networkAnonymizationKey,
  const scoped_refptr<base::SingleThreadTaskRunner>& sth_task_runner,
  const scoped_refptr<DiskCacheBackendFactory>& disk_cache_backend_factory,
  const ResPreloadInfosCB& preload_infos_cb) : preload_infos_cb_(preload_infos_cb) {
    res_req_info_cache_mgr_ = base::WrapRefCounted(new (std::nothrow) ResReqInfoCacheMgr(url, networkAnonymizationKey,
      sth_task_runner, disk_cache_backend_factory,
      base::BindRepeating(&ResRequestInfoUpdater::OnResRequestInfoCacheLoaded, weak_factory_.GetWeakPtr())));
    if (res_req_info_cache_mgr_ == nullptr) {
      LOG(WARNING) << "PRPPreload.ResRequestInfoUpdater::ResRequestInfoUpdater new ResReqInfoCacheMgr failed";
    }
}

void ResRequestInfoUpdater::Start() {
  if (res_req_info_cache_mgr_ == nullptr) {
    return;
  }
  res_req_info_cache_mgr_->Start();
}

void ResRequestInfoUpdater::UpdateResRequestInfo(const std::shared_ptr<PRRequestInfo>& info) {
  if (res_req_info_cache_mgr_ == nullptr) {
    return;
  }
  res_req_info_cache_mgr_->UpdateResRequestInfo(info);
}

void ResRequestInfoUpdater::SetPageOrigin(const std::string& page_origin) {
  if (res_req_info_cache_mgr_ == nullptr) {
    return;
  }
  res_req_info_cache_mgr_->SetPageOrigin(page_origin);
}

void ResRequestInfoUpdater::Stop() {
  if (res_req_info_cache_mgr_ == nullptr) {
    return;
  }
  res_req_info_cache_mgr_->Stop();
}

void ResRequestInfoUpdater::OnResRequestInfoCacheLoaded(
    const std::list<std::shared_ptr<PRRequestInfo>>& load_info_list,
    const net::NetworkAnonymizationKey& networkAnonymizationKey) {
  if (preload_infos_cb_.is_null()) {
    return;
  }
  bool need_to_build_preload_tree =
    ohos_prp_preload::PRParallelPreloadMgr::GetInstance().GetPRParallelPreloadMode() ==
    PRPPreloadMode::PRELOAD;
  // build preconnect list and preload tree
  std::shared_ptr<PRPPReqInfoTreeNode> cur_parent = nullptr;
  std::shared_ptr<PRPPReqInfoTreeNode> cur_first = nullptr;
  std::shared_ptr<PRPPReqInfoTreeNode> current = nullptr;
  bool only_send_reuse_request = false;
  int64_t cur_level_end_time = 0;
  for (auto info : load_info_list) {
    if (info->type() == PRRequestInfoType::TYPE_PAGE_ORIGIN && need_to_build_preload_tree) {
      only_send_reuse_request = info->only_send_reuse_request();
      if (preload_info_tree_ == nullptr) {
        preload_info_tree_ = std::make_shared<PRPPReqInfoTreeNode>();
        preload_info_tree_->req_info_ = info;
        cur_parent = preload_info_tree_;
      }
      continue;
    }

    BuildPreconnectList(info, networkAnonymizationKey);
    if (!need_to_build_preload_tree || cur_parent == nullptr) {
      continue;
    }
    BuildPreloadTree(info, current, cur_first, cur_parent, cur_level_end_time);
  }
  preload_infos_cb_.Run(prpp_preconnect_info_list_, preload_info_tree_,
    only_send_reuse_request, need_record_header_urls_);
}

void ResRequestInfoUpdater::BuildPreconnectList(const std::shared_ptr<PRRequestInfo>& info,
    const net::NetworkAnonymizationKey& networkAnonymizationKey)
{
  GURL origin_url = url::Origin::Create(info->url()).GetURL();
  bool need_connect = false;
  bool need_add_connect = false;
  if (!net::HttpUtil::IsMethodSafe(info->method()) ||
      (info->cache_type() != PRRequestCacheType::FORCE_CACHE) ||
      (base::Time::Now().ToInternalValue() > info->freshness_life_times())) {
    need_connect = true;
  }

  std::string key = info->allow_credentials() ?
    PRIVACY_TAG + origin_url.spec() : origin_url.spec();
  if (preconnect_org_url_map_.count(key) != 0) {
    PreconnectCount& cur_count = preconnect_org_url_map_[key];
    if (cur_count.need_count_ + cur_count.reserved_count_ < MAX_PRECONNECT_COUNT) {
      if (need_connect) {
        cur_count.need_count_++;
        need_add_connect = true;
      } else if (cur_count.reserved_count_ < MAX_RESERVED_COUNT) {
        cur_count.reserved_count_++;
        need_add_connect = true;
      }
    }
  } else {
    need_add_connect = true;
    if (need_connect) {
      preconnect_org_url_map_[key] = PreconnectCount{1, 0};
    } else {
      preconnect_org_url_map_[key] = PreconnectCount{0, 1};
    }
  }

  if (need_add_connect) {
    if (base::ohos::IsMobileDevice() == true) {
      prpp_preconnect_info_list_.emplace_back(PRPPPreconnectInfo{origin_url,
        info->allow_credentials(),
        net::NetworkAnonymizationKey::CreateSameSite(net::SchemefulSite(origin_url))});
    } else {
      prpp_preconnect_info_list_.emplace_back(PRPPPreconnectInfo{origin_url,
        info->allow_credentials(),
        networkAnonymizationKey});
    }
  }
}

void ResRequestInfoUpdater::BuildPreloadTree(const std::shared_ptr<PRRequestInfo>& info,
    std::shared_ptr<PRPPReqInfoTreeNode> current,
    std::shared_ptr<PRPPReqInfoTreeNode> cur_first,
    std::shared_ptr<PRPPReqInfoTreeNode> cur_parent,
    int64_t cur_level_end_time)
{
  current = std::make_shared<PRPPReqInfoTreeNode>();
  current->req_info_ = info;
  int64_t cur_request_start_time = current->req_info_->request_start_time();
  int64_t cur_request_end_time = current->req_info_->request_end_time();
  if (cur_first == nullptr) {
    cur_first = current;
    cur_first->parent_ = cur_parent.get();
    cur_parent->children_.emplace_back(cur_first);
    cur_level_end_time = cur_request_end_time;
    return;
  }

  if ((cur_request_start_time > cur_first->req_info_->request_start_time() + MAX_LEVEL_INTERVAL_US) ||
      (cur_level_end_time > 0 && cur_request_start_time > cur_level_end_time)) {
    // new level
    cur_parent = cur_first;
    cur_first = current;
    cur_first->parent_ = cur_parent.get();
    cur_parent->children_.emplace_back(cur_first);
    cur_level_end_time = cur_request_end_time;
  } else {
    cur_parent->children_.emplace_back(current);
    if (cur_request_end_time > 0) {
      cur_level_end_time = cur_level_end_time > 0 ?
        std::min(cur_request_end_time, cur_level_end_time) : cur_request_end_time;
    }
  }

  if (((current->req_info_->preload_flag() & PRPP_FLAGS_HDR_DYNAMIC) == PRPP_FLAGS_HDR_DYNAMIC)) {
    UpdateResRequestInfoForDynamicHeaders(cur_parent, current->req_info_);
  }
}

void ResRequestInfoUpdater::UpdateResRequestInfoForDynamicHeaders(
    std::shared_ptr<PRPPReqInfoTreeNode> parent, std::shared_ptr<PRRequestInfo> child_info)
{
  if (parent == nullptr || parent->parent_ == nullptr || parent->parent_->children_.empty()) {
    return;
  }
  for (auto it : parent->parent_->children_) {
    if (it->req_info_->cache_type() == PRRequestCacheType::FORCE_CACHE) {
      continue;
    }
    if (IsDynamicHeadersMatch(child_info, it->req_info_)) {
      child_info->set_parent_for_dynamic_header(it->req_info_->url());
      (void)need_record_header_urls_.emplace(it->req_info_->url().spec());
      return;
    }
  }
}

bool ResRequestInfoUpdater::IsDynamicHeadersMatch(const std::shared_ptr<PRRequestInfo>& child_info,
    const std::shared_ptr<PRRequestInfo>& parent_info)
{
  for (auto header : child_info->dynamic_header_keys()) {
    if (!parent_info->extra_request_headers().HasHeader(header)) {
      return false;
    }
  }
  return true;
}
}  // namespace ohos_prp_preload
