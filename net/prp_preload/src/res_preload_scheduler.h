// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
    const net::NetworkAnonymizationKey& networkAnonymizationKey);
  void StopPreload();
 private:
  using InfoIter = std::list<std::shared_ptr<PRRequestInfo>>::iterator;
  bool NeedToPreconnect(const GURL& url, bool allow_credentials);
  void PreconnectBeyondLimit(InfoIter info_iter, const int info_list_version,
    const net::NetworkAnonymizationKey& networkAnonymizationKey);

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