// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "res_request_info_updater.h"

#include "base/logging.h"

namespace ohos_prp_preload {
ResRequestInfoUpdater::ResRequestInfoUpdater(const std::string& url,
  const net::NetworkAnonymizationKey& networkAnonymizationKey,
  const scoped_refptr<base::SingleThreadTaskRunner>& sth_task_runner,
  const scoped_refptr<DiskCacheBackendFactory>& disk_cache_backend_factory,
  const ResRequestInfoListCB& res_req_info_list_cb) :
  res_req_info_list_cb_(res_req_info_list_cb) {
    res_req_info_cache_mgr_ = base::WrapRefCounted(new (std::nothrow) ResReqInfoCacheMgr(url, networkAnonymizationKey,
      sth_task_runner, disk_cache_backend_factory,
      base::BindRepeating(&ResRequestInfoUpdater::OnResRequestInfoCacheLoaded, weak_factory_.GetWeakPtr())));
    if (res_req_info_cache_mgr_ == nullptr) {
      LOG(ERROR) << "PRPPreload.ResRequestInfoUpdater::ResRequestInfoUpdater new ResReqInfoCacheMgr failed";
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

void ResRequestInfoUpdater::Stop() {
  if (res_req_info_cache_mgr_ == nullptr) {
    return;
  }
  res_req_info_cache_mgr_->Stop();
}

void ResRequestInfoUpdater::OnResRequestInfoCacheLoaded(const std::list<std::shared_ptr<PRRequestInfo>>& load_info_list,
  const net::NetworkAnonymizationKey& networkAnonymizationKey) {
  if (!res_req_info_list_cb_.is_null()) {
    res_req_info_list_cb_.Run(load_info_list, networkAnonymizationKey);
  }
}

}  // namespace ohos_prp_preload
