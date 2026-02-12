// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_NETWORK_PRP_PRELOAD_SRC_RES_PARALLEL_PRELOAD_CTRLER_H
#define SERVICES_NETWORK_PRP_PRELOAD_SRC_RES_PARALLEL_PRELOAD_CTRLER_H

#include "arkweb/chromium_ext/services/network/prp_preload/src/res_preload_scheduler.h"
#include "arkweb/chromium_ext/services/network/prp_preload/src/res_request_info_updater.h"

namespace ohos_prp_preload {
using PRPPCtrlerTimeoutCB = base::RepeatingCallback<void(const std::string& url)>;
class ResParallelPreloadCtrler : public base::RefCountedThreadSafe<ResParallelPreloadCtrler> {
 public:
  ResParallelPreloadCtrler(const std::string& url,
                           const net::NetworkAnonymizationKey& networkAnonymizationKey,
                           const scoped_refptr<base::SingleThreadTaskRunner>& sth_task_runner,
                           const PRPPCtrlerTimeoutCB& timeout_cb);
  ~ResParallelPreloadCtrler()
  {
    LOG(INFO) << "PRPPreload.ResParallelPreloadCtrler::~ResParallelPreloadCtrler";
  }

  static void InitDiskCacheBackendFactory();
  static void RemoveCache(base::Time start_time, base::Time end_time);
  void Init(const scoped_refptr<base::SingleThreadTaskRunner>& net_task_runner,
    base::WeakPtr<net::URLRequestContext> url_request_context,
    const PRPPOnPageOriginCB& on_page_origin_cb);
  void Start();
  void Stop();
  void UpdateResRequestInfo(const std::shared_ptr<PRRequestInfo>& info);
  void SetPageOrigin(const std::string& page_origin);
  void SetPRPPReqLoaderFac(base::WeakPtr<PRPPRequestLoaderFactory> loader_fac_weak, bool valid);
  void UpdateIdlePrerequestCount();
 private:
  void DoInit(const scoped_refptr<base::SingleThreadTaskRunner>& net_task_runner,
    base::WeakPtr<net::URLRequestContext> url_request_context,
    const PRPPOnPageOriginCB& on_page_origin_cb);
  void DoStart();
  void DoStop();
  void DoUpdateResRequestInfo(const std::shared_ptr<PRRequestInfo>& info);
  void DoSetPageOrigin(const std::string& page_origin);
  void DoSetPRPPReqLoaderFac(base::WeakPtr<PRPPRequestLoaderFactory> loader_fac_weak, bool valid);
  void DoUpdateIdlePrerequestCount();
  void OnResPreloadInfos(const PRPPPreconnectInfoList& preconnect_info_list,
    const std::shared_ptr<PRPPReqInfoTreeNode>& preload_info_tree,
    const std::set<std::string>& need_record_header_urls);
  void OnTimeout();
  void InitDiskCacheBackendFactoryForTest();
  void ResetDiskCacheBackendFactoryForTest();
  std::string url_;
  const net::NetworkAnonymizationKey networkAnonymizationKey_;
  scoped_refptr<base::SingleThreadTaskRunner> sth_task_runner_;
  scoped_refptr<ResRequestInfoUpdater> res_req_info_updater_;
  scoped_refptr<ResPreloadScheduler> res_preload_scheduler_;
  PRPPCtrlerTimeoutCB timeout_cb_;
  bool start_filter_info_ { false };
  uint32_t filter_count_ { 0 };

  base::WeakPtrFactory<ResParallelPreloadCtrler> weak_factory_{this};
};

}  // namespace ohos_prp_preload

#endif  // SERVICES_NETWORK_PRP_PRELOAD_SRC_RES_PARALLEL_PRELOAD_CTRLER_H