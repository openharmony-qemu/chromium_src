// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
  virtual void Init(const scoped_refptr<base::SingleThreadTaskRunner>& net_task_runner) = 0;
  virtual void StartMainPage(const std::string& url, const net::NetworkAnonymizationKey& networkAnonymizationKey,
                             base::WeakPtr<net::URLRequestContext> url_request_context,
                             uint64_t addr_web_handle) = 0;
  virtual void StopMainPage(const std::string& url) = 0;
  virtual void StopMainPage(uint64_t addr_web_handle) = 0;
  virtual void UpdateResRequestInfo(const std::string& key,
                                    const std::shared_ptr<PRRequestInfo>& info) = 0;
};

}  // namespace ohos_prp_preload

#endif  // NET_PRP_PRELOAD_INCLUDE_PAGR_RES_PARALLEL_PRELOAD_MGR_H
