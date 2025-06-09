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

#ifndef NET_PRP_PRELOAD_INCLUDE_PAGR_RES_PARALLEL_PRELOAD_MGR_H
#define NET_PRP_PRELOAD_INCLUDE_PAGR_RES_PARALLEL_PRELOAD_MGR_H

#include "base/task/thread_pool.h"
#include "net/base/network_anonymization_key.h"
#include "page_res_request_info.h"

namespace net {
class URLRequestContext;
}  // namespace net

namespace ohos_prp_preload {
class PRParallelPreloadMgr {
 public:
  static PRParallelPreloadMgr& GetInstance();
  static bool PRParallelPreloadEnabled();
  virtual ~PRParallelPreloadMgr() = default;
  virtual void Init(const scoped_refptr<base::SingleThreadTaskRunner>& net_task_runner,
                    const base::FilePath& cache_path) = 0;
  virtual void StartMainPage(const std::string& url, const net::NetworkAnonymizationKey& network_anonymization_key,
                             base::WeakPtr<net::URLRequestContext> url_request_context, uint64_t addr_web_handle) = 0;
  virtual void StopMainPage(const std::string& url) = 0;
  virtual void StopMainPage(uint64_t addr_web_handle) = 0;
  virtual void UpdateResRequestInfo(const std::string& key,
                                    const std::shared_ptr<PRRequestInfo>& info) = 0;
};

}  // namespace ohos_prp_preload

#endif  // NET_PRP_PRELOAD_INCLUDE_PAGR_RES_PARALLEL_PRELOAD_MGR_H
