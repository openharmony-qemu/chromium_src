// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_PRP_PRELOAD_SRC_PAGR_RES_PARALLEL_PRELOAD_MGR_IMPL_H
#define NET_PRP_PRELOAD_SRC_PAGR_RES_PARALLEL_PRELOAD_MGR_IMPL_H

#include <mutex>
#include <unordered_map>
#include "disk_cache_backend_factory.h"
#include "page_res_parallel_preload_mgr.h"
#include "res_parallel_preload_ctrler.h"

namespace ohos_prp_preload {
class PRParallelPreloadMgrImpl : public PRParallelPreloadMgr {
 public:
  PRParallelPreloadMgrImpl() = default;
  ~PRParallelPreloadMgrImpl() = default;
  void Init(const scoped_refptr<base::SingleThreadTaskRunner>& net_task_runner,
            const base::FilePath& chchePath) override;
  void StartMainPage(const std::string& url,
                     const net::NetworkAnonymizationKey& network_anonymization_key,
                     base::WeakPtr<net::URLRequestContext> url_request_context,
                     uint64_t addr_web_handle) override;
  void StopMainPage(const std::string& url) override;
  void StopMainPage(uint64_t addr_web_handle) override;
  void UpdateResRequestInfo(const std::string& key,
                            const std::shared_ptr<PRRequestInfo>& info) override;
 private:
  void StopMainPageInternal(const std::string& url);
  void OnRPPCtrlerTimeout(const std::string& url);
  bool RecycleRPPCtrler();
  struct PRParallelPreloadInfo {
    scoped_refptr<ResParallelPreloadCtrler> rp_preload_ctrler_;
    bool start_page_ { false };
  };

  std::unordered_map<std::string, PRParallelPreloadInfo> prp_preload_info_map_;
  std::unordered_map<const void*, std::string> web_handle_pages_map_;
  scoped_refptr<base::SingleThreadTaskRunner> sth_task_runner_;
  scoped_refptr<base::SingleThreadTaskRunner> net_task_runner_;
  std::atomic_bool is_inited_ { false };
  std::mutex mgr_mutex_;
  std::list<std::string> stopped_pages_;
  base::WeakPtrFactory<PRParallelPreloadMgrImpl> weak_factory_ { this };
};

}  // namespace ohos_prp_preload

#endif  // NET_PRP_PRELOAD_SRC_PAGR_RES_PARALLEL_PRELOAD_MGR_IMPL_H
