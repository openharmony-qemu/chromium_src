// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_NETWORK_PRP_PRELOAD_SRC_RES_REQUEST_INFO_CACHE_MGR_H
#define SERVICES_NETWORK_PRP_PRELOAD_SRC_RES_REQUEST_INFO_CACHE_MGR_H

#include "base/task/thread_pool.h"
#include "services/network/prp_preload/src/disk_cache_file.h"
#include "services/network/prp_preload/src/disk_cache_info_parser.h"

namespace ohos_prp_preload {
using ResRequestInfoCacheLoadedCB =
  base::RepeatingCallback<void(const std::list<std::shared_ptr<PRRequestInfo>>& load_info_list)>;
class ResReqInfoCacheMgr : public base::RefCounted<ResReqInfoCacheMgr> {
 public:
  ResReqInfoCacheMgr(const std::string& url,
                     const scoped_refptr<base::SingleThreadTaskRunner>& sth_task_runner,
                     const scoped_refptr<DiskCacheBackendFactory>& disk_cache_backend_factory,
                     const ResRequestInfoCacheLoadedCB& info_list_cb);
  ~ResReqInfoCacheMgr() = default;

  void Start();
  void UpdateResRequestInfo(const std::shared_ptr<PRRequestInfo>& info);
  void Stop();
  void SetPageOrigin(const std::string& page_origin);
 private:
  void OnEntryLoadedCallback(const std::string& entry_content);
  void CheckFlush();

  scoped_refptr<base::SingleThreadTaskRunner> sth_task_runner_;
  scoped_refptr<DiskCacheFile> disk_cache_;
  std::list<std::shared_ptr<PRRequestInfo>> new_info_list_;
  std::list<std::shared_ptr<PRRequestInfo>> load_info_list_;
  size_t last_flush_len_{0};
  ResRequestInfoCacheLoadedCB info_cache_loaded_cb_;
  bool is_start_{false};
  std::string page_origin_;
  base::WeakPtrFactory<ResReqInfoCacheMgr> weak_factory_{this};
};

}  // namespace ohos_prp_preload

#endif  // SERVICES_NETWORK_PRP_PRELOAD_SRC_RES_REQUEST_INFO_CACHE_MGR_H
