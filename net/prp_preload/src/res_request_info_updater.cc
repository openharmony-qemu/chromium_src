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

#include "res_request_info_updater.h"

#include "base/logging.h"

namespace ohos_prp_preload {
ResRequestInfoUpdater::ResRequestInfoUpdater(
    const std::string& url,
    const net::NetworkAnonymizationKey& network_anonymization_key,
    const scoped_refptr<base::SingleThreadTaskRunner>& sth_task_runner,
    const scoped_refptr<DiskCacheBackendFactory>& disk_cache_backend_factory,
    const ResRequestInfoListCB& res_req_info_list_cb) :
    res_req_info_list_cb_(res_req_info_list_cb) {
  res_req_info_cache_mgr_ = base::WrapRefCounted(new (std::nothrow) ResReqInfoCacheMgr(url, network_anonymization_key,
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

void ResRequestInfoUpdater::OnResRequestInfoCacheLoaded(
  const std::list<std::shared_ptr<PRRequestInfo>>& load_info_list,
  const net::NetworkAnonymizationKey& network_anonymization_key) {
  if (!res_req_info_list_cb_.is_null()) {
    res_req_info_list_cb_.Run(load_info_list, network_anonymization_key);
  }
}

}  // namespace ohos_prp_preload
