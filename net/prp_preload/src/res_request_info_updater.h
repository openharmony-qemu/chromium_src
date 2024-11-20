// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_PRP_PRELOAD_SRC_RES_REQUEST_INFO_UPDATER_H
#define NET_PRP_PRELOAD_SRC_RES_REQUEST_INFO_UPDATER_H

#include "res_request_info_cache_mgr.h"

namespace ohos_prp_preload {
using ResRequestInfoListCB =
  base::RepeatingCallback<void(const std::list<std::shared_ptr<PRRequestInfo>>& res_req_info_list,
    const net::NetworkAnonymizationKey& networkAnonymizationKey)>;
class ResRequestInfoUpdater : public base::RefCounted<ResRequestInfoUpdater> {
 public:
  ResRequestInfoUpdater(const std::string& url,
                        const net::NetworkAnonymizationKey& networkAnonymizationKey,
                        const scoped_refptr<base::SingleThreadTaskRunner>& sth_task_runner,
                        const scoped_refptr<DiskCacheBackendFactory>& disk_cache_backend_factory,
                        const ResRequestInfoListCB& res_req_info_list_cb);
  ~ResRequestInfoUpdater() = default;

  void Start();
  void UpdateResRequestInfo(const std::shared_ptr<PRRequestInfo>& info);
  void Stop();
 private:
  void OnResRequestInfoCacheLoaded(const std::list<std::shared_ptr<PRRequestInfo>>& load_info_list,
    const net::NetworkAnonymizationKey& networkAnonymizationKey);
 
  scoped_refptr<ResReqInfoCacheMgr> res_req_info_cache_mgr_;
  ResRequestInfoListCB res_req_info_list_cb_;
  base::WeakPtrFactory<ResRequestInfoUpdater> weak_factory_{this};
};

}  // namespace ohos_prp_preload

#endif  // NET_PRP_PRELOAD_SRC_RES_REQUEST_INFO_UPDATER_H