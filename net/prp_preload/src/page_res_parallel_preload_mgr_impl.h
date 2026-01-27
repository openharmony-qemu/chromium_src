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
