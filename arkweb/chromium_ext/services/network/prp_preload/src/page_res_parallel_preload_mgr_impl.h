// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_NETWORK_PRP_PRELOAD_SRC_PAGR_RES_PARALLEL_PRELOAD_MGR_IMPL_H
#define SERVICES_NETWORK_PRP_PRELOAD_SRC_PAGR_RES_PARALLEL_PRELOAD_MGR_IMPL_H

#include <unordered_map>
#include "arkweb/chromium_ext/services/network/prp_preload/include/page_res_parallel_preload_mgr.h"
#include "arkweb/chromium_ext/services/network/prp_preload/src/res_parallel_preload_ctrler.h"

namespace ohos_prp_preload {
class PRParallelPreloadMgrImpl : public PRParallelPreloadMgr {
 public:
  PRParallelPreloadMgrImpl() = default;
  ~PRParallelPreloadMgrImpl() = default;
  void Init(const scoped_refptr<base::SingleThreadTaskRunner>& net_task_runner) override;
  void StartPage(const std::string& url,
                     const net::NetworkAnonymizationKey& networkAnonymizationKey,
                     base::WeakPtr<net::URLRequestContext> url_request_context,
                     uint64_t addr_web_handle,
                     PageOriginCallback callback,
                     bool no_use_cache) override;
  void StopPage(uint64_t addr_web_handle) override;
  void UpdateResRequestInfo(const std::string& key,
                            const std::shared_ptr<PRRequestInfo>& info) override;
  void UpdateRedirectUrl(const std::string& org_url, const std::string& redirect_url, bool replace = false) override;
  void SetPageOrigin(const std::string& url, const net::IsolationInfo& isl) override;
  void OnPageOrigin(const std::string& url, const std::string& page_origin) override;
  void SetURLLoaderFactoryParam(network::mojom::URLLoaderFactoryParamsPtr params) override;
  base::WeakPtr<PRPPRequestLoaderFactory> GetRequestLoaderFactory(const std::string& main_url) override;
  void UpdateIdlePrerequestCount(const std::string& key) override;
  void RemoveCache(base::Time start_time, base::Time end_time) override;
 private:
  struct PRParallelPreloadInfo {
    scoped_refptr<ResParallelPreloadCtrler> rp_preload_ctrler_;
    bool start_page_ { false };
    PageOriginCallback callback_;
    std::shared_ptr<PRPPRequestLoaderFactory> prpp_req_loader_fac_;
    bool page_origin_ready_ { false };
  };
  void StopPageInternal(const std::string& url);
  void OnRPPCtrlerTimeout(const std::string& url);
  void DoRPPCtrlerTimeout(const std::string& url);
  void DoRmPageUrl(const std::string& in_org_url);
  void FindCurPreloadInfoAndRun(const std::string& key,
      std::function<void(PRParallelPreloadInfo&)> func);
  bool IsPageAlreadyStarted(const std::string& key, PageOriginCallback& callback);
  bool is_inited_ { false };
  scoped_refptr<base::SingleThreadTaskRunner> sth_task_runner_;
  scoped_refptr<base::SingleThreadTaskRunner> net_task_runner_;
  std::unordered_map<std::string, PRParallelPreloadInfo> prp_preload_info_map_;
  std::unordered_map<const void*, std::string> web_handle_pages_map_;
  std::unordered_map<std::string, std::string> prp_page_url_;

  base::WeakPtrFactory<PRParallelPreloadMgrImpl> weak_factory_ { this };
};

}  // namespace ohos_prp_preload

#endif  // SERVICES_NETWORK_PRP_PRELOAD_SRC_PAGR_RES_PARALLEL_PRELOAD_MGR_IMPL_H