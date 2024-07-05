// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_PRP_PRELOAD_SRC_RES_PARALLEL_PRELOAD_CTRLER_H
#define NET_PRP_PRELOAD_SRC_RES_PARALLEL_PRELOAD_CTRLER_H

#include "disk_cache_backend_factory.h"
#include "res_preload_scheduler.h"
#include "res_request_info_updater.h"

namespace ohos_prp_preload {
using RPPCtrlerTimeoutCB = base::RepeatingCallback<void(const std::string& url)>;
class ResParallelPreloadCtrler : public base::RefCounted<ResParallelPreloadCtrler> {
 public:
  ResParallelPreloadCtrler(const std::string& url,
                           base::WeakPtr<net::URLRequestContext> url_request_context,
                           const scoped_refptr<base::SingleThreadTaskRunner>& sth_task_runner,
                           const scoped_refptr<base::SingleThreadTaskRunner>& net_task_runner,
                           const scoped_refptr<DiskCacheBackendFactory>& disk_cache_backend_factory,
                           const RPPCtrlerTimeoutCB& timeout_cb);
  ~ResParallelPreloadCtrler() = default;
 
  void Start();
  void Stop();
  void UpdateResRequestInfo(const std::shared_ptr<PRRequestInfo>& info);
 private:
  void DoStart();
  void DoStop();
  void DoUpdateResRequestInfo(const std::shared_ptr<PRRequestInfo>& info);
  void OnResRequestInfoList(const std::list<std::shared_ptr<PRRequestInfo>>& res_req_info_list);
  void OnTimeout();

  std::string url_;
  scoped_refptr<base::SingleThreadTaskRunner> sth_task_runner_;
  scoped_refptr<ResRequestInfoUpdater> res_req_info_updater_;
  scoped_refptr<ResPreloadScheduler> res_preload_scheduler_;
  RPPCtrlerTimeoutCB timeout_cb_;
  base::WeakPtrFactory<ResParallelPreloadCtrler> weak_factory_{this};
};

}  // namespace ohos_prp_preload

#endif  // NET_PRP_PRELOAD_SRC_RES_PARALLEL_PRELOAD_CTRLER_H