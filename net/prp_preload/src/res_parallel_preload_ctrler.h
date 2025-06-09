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
                           const net::NetworkAnonymizationKey& network_anonymization_key,
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
  void OnResRequestInfoList(const std::list<std::shared_ptr<PRRequestInfo>>& res_req_info_list,
      const net::NetworkAnonymizationKey& network_anonymization_key);
  void OnTimeout();

  std::string url_;
  const net::NetworkAnonymizationKey network_anonymization_key_;
  scoped_refptr<base::SingleThreadTaskRunner> sth_task_runner_;
  scoped_refptr<ResRequestInfoUpdater> res_req_info_updater_;
  scoped_refptr<ResPreloadScheduler> res_preload_scheduler_;
  RPPCtrlerTimeoutCB timeout_cb_;
  base::WeakPtrFactory<ResParallelPreloadCtrler> weak_factory_{this};
};

}  // namespace ohos_prp_preload

#endif  // NET_PRP_PRELOAD_SRC_RES_PARALLEL_PRELOAD_CTRLER_H
