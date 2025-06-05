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

#ifndef NET_PRP_PRELOAD_SRC_RES_PRELOAD_SCHEDULER_H
#define NET_PRP_PRELOAD_SRC_RES_PRELOAD_SCHEDULER_H

#include <list>
#include "base/memory/weak_ptr.h"
#include "base/task/thread_pool.h"
#include "page_res_request_info.h"
#include "net/base/network_anonymization_key.h"


namespace net {
class URLRequestContext;
}  // namespace net

namespace ohos_prp_preload {
class ResPreloadScheduler : public base::RefCounted<ResPreloadScheduler> {
 public:
  ResPreloadScheduler(const scoped_refptr<base::SingleThreadTaskRunner>& sth_task_runner,
                      const scoped_refptr<base::SingleThreadTaskRunner>& net_task_runner,
                      base::WeakPtr<net::URLRequestContext> url_request_context);
  ResPreloadScheduler() = delete;
  ~ResPreloadScheduler() = default;
 
  void PreloadSchedule(const std::list<std::shared_ptr<PRRequestInfo>>& res_req_info_list,
      const net::NetworkAnonymizationKey& network_anonymization_key);
  void StopPreload();
 private:
  using InfoIter = std::list<std::shared_ptr<PRRequestInfo>>::iterator;
  bool NeedToPreconnect(const GURL& url, bool allow_credentials);
  void PreconnectBeyondLimit(InfoIter info_iter, const int info_list_version,
      const net::NetworkAnonymizationKey& network_anonymization_key);

  std::list<std::shared_ptr<PRRequestInfo>> info_list_;
  std::unordered_map<std::string, int> idle_connect_list_;
  scoped_refptr<base::SingleThreadTaskRunner> sth_task_runner_;
  scoped_refptr<base::SingleThreadTaskRunner> net_task_runner_;
  base::WeakPtr<net::URLRequestContext> url_request_context_;
  size_t socket_connected_ = 0;
  bool preload_triggered_ = false;
  int info_list_version_ = 0;
  base::WeakPtrFactory<ResPreloadScheduler> weak_factory_{this};
};

}  // namespace ohos_prp_preload

#endif  // NET_PRP_PRELOAD_SRC_RES_PRELOAD_SCHEDULER_H
