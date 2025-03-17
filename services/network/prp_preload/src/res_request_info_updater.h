// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_NETWORK_PRP_PRELOAD_SRC_RES_REQUEST_INFO_UPDATER_H
#define SERVICES_NETWORK_PRP_PRELOAD_SRC_RES_REQUEST_INFO_UPDATER_H

#include "services/network/prp_preload/src/res_request_info_cache_mgr.h"

namespace ohos_prp_preload {
using ResRequestInfoListCB =
  base::RepeatingCallback<void(const std::list<std::shared_ptr<PRRequestInfo>>& res_req_info_list,
    const net::NetworkAnonymizationKey& networkAnonymizationKey)>;
using ResPreloadInfosCB =
  base::RepeatingCallback<void(const PRPPPreconnectInfoList& preconnect_info_list,
    const std::shared_ptr<PRPPReqInfoTreeNode>& preload_info_tree,
    bool only_send_reuse_request,
    const std::set<std::string>& need_record_header_urls)>;
class ResRequestInfoUpdater : public base::RefCounted<ResRequestInfoUpdater> {
 public:
  ResRequestInfoUpdater(const std::string& url,
                        const net::NetworkAnonymizationKey& networkAnonymizationKey,
                        const scoped_refptr<base::SingleThreadTaskRunner>& sth_task_runner,
                        const scoped_refptr<DiskCacheBackendFactory>& disk_cache_backend_factory,
                        const ResPreloadInfosCB& preload_infos_cb);
  ~ResRequestInfoUpdater() = default;

  void Start();
  void UpdateResRequestInfo(const std::shared_ptr<PRRequestInfo>& info);
  void Stop();
  void SetPageOrigin(const std::string& page_origin);
 private:
  void UpdateResRequestInfoForDynamicHeaders(std::shared_ptr<PRPPReqInfoTreeNode> parent,
    std::shared_ptr<PRRequestInfo> child_info);
  struct PreconnectCount {
    int32_t need_count_ { 0 };
    int32_t reserved_count_ { 0 };
  };
  void OnResRequestInfoCacheLoaded(const std::list<std::shared_ptr<PRRequestInfo>>& load_info_list,
    const net::NetworkAnonymizationKey& networkAnonymizationKey);
 
  scoped_refptr<ResReqInfoCacheMgr> res_req_info_cache_mgr_;
  PRPPPreconnectInfoList prpp_preconnect_info_list_;
  std::shared_ptr<PRPPReqInfoTreeNode> preload_info_tree_;
  std::unordered_map<std::string, PreconnectCount> preconnect_org_url_map_;
  ResPreloadInfosCB preload_infos_cb_;
  std::set<std::string> need_record_header_urls_;
  base::WeakPtrFactory<ResRequestInfoUpdater> weak_factory_{this};
};

}  // namespace ohos_prp_preload

#endif  // SERVICES_NETWORK_PRP_PRELOAD_SRC_RES_REQUEST_INFO_UPDATER_H