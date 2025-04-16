// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_NETWORK_PRP_PRELOAD_INCLUDE_PAGE_RES_PARALLEL_PRELOAD_MGR_H
#define SERVICES_NETWORK_PRP_PRELOAD_INCLUDE_PAGE_RES_PARALLEL_PRELOAD_MGR_H

#include "base/task/thread_pool.h"
#include "net/base/network_anonymization_key.h"
#include "services/network/public/mojom/network_context.mojom.h"

namespace net {
class URLRequestContext;
}  // namespace net

namespace ohos_prp_preload {
class PRRequestInfo;
class PRPPRequestLoaderFactory;
enum class PRPPreloadMode {
  NONE, // close preload
  PRECONNECT, // only preconnect
  PRELOAD, // preconnect and prevalidate
};
using PageOriginCallback = base::OnceCallback<void(const std::string&)>;
class PRParallelPreloadMgr {
 public:
  // get single instance of PRParallelPreloadMgr
  static PRParallelPreloadMgr& GetInstance();

  // get preload mode: none, preconnect or preload
  static PRPPreloadMode GetPRParallelPreloadMode();

  // destroy the PRParallelPreloadMgr object
  virtual ~PRParallelPreloadMgr() = default;

  // init preload module, load preload info cache index and start preload thread
  virtual void Init(const scoped_refptr<base::SingleThreadTaskRunner>& net_task_runner) = 0;

  // start preload page sub_request
  virtual void StartPage(const std::string& url,
                         const net::NetworkAnonymizationKey& networkAnonymizationKey,
                         base::WeakPtr<net::URLRequestContext> url_request_context,
                         uint64_t addr_web_handle,
                         PageOriginCallback callback) = 0;

  // stop preload page
  virtual void StopPage(uint64_t addr_web_handle) = 0;

  // update preload request info
  virtual void UpdateResRequestInfo(const std::string& key,
                                    const std::shared_ptr<PRRequestInfo>& info) = 0;

  // update preload url for redirect page
  virtual void UpdateRedirectUrl(const std::string& org_url,
                                 const std::string& redirect_url,
                                 bool replace = false) = 0;

  // set URLLoaderFactoryParam, update isolation info
  virtual void SetURLLoaderFactoryParam(network::mojom::URLLoaderFactoryParamsPtr params) = 0;

  // set isolation info and origin
  virtual void SetPageOrigin(const std::string& url, const net::IsolationInfo& isl) = 0;

  // excute callback NavigationRequest::OnGetIsolation, create isolation info and set
  // callback by ResPreloadScheduler
  virtual void OnPageOrigin(const std::string& url, const std::string& page_origin) = 0;

  // get PRPPRequestLoaderFactory for page
  virtual base::WeakPtr<PRPPRequestLoaderFactory> GetRequestLoaderFactory(
    const std::string& main_url) = 0;

  // update the count of prerequest can be send to scheduler
  virtual void UpdateIdlePrerequestCount(const std::string& key) = 0;
};

}  // namespace ohos_prp_preload

#endif  // SERVICES_NETWORK_PRP_PRELOAD_INCLUDE_PAGE_RES_PARALLEL_PRELOAD_MGR_H
