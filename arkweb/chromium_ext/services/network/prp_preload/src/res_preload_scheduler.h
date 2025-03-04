// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_NETWORK_PRP_PRELOAD_SRC_RES_PRELOAD_SCHEDULER_H
#define SERVICES_NETWORK_PRP_PRELOAD_SRC_RES_PRELOAD_SCHEDULER_H

#include "arkweb/chromium_ext/services/network/prp_preload/include/preload_runner/prpp_request_loader_factory.h"
#include "base/task/thread_pool.h"

namespace net {
class URLRequestContext;
}  // namespace net

namespace ohos_prp_preload {
using PRPPOnPageOriginCB = base::RepeatingCallback<void(const std::string& url,
  const std::string& page_origin)>;
class ResPreloadScheduler : public base::RefCounted<ResPreloadScheduler> {
 public:
  ResPreloadScheduler(const std::string& url,
    const scoped_refptr<base::SingleThreadTaskRunner>& sth_task_runner,
    const scoped_refptr<base::SingleThreadTaskRunner>& net_task_runner,
    base::WeakPtr<net::URLRequestContext> url_request_context,
    const PRPPOnPageOriginCB& on_page_origin_cb);
  ResPreloadScheduler() = delete;
  ~ResPreloadScheduler() = default;

  void StopPreload();
  void SetPRPPReqLoaderFac(base::WeakPtr<PRPPRequestLoaderFactory> loader_fac_weak);
  void SchedulePreloads(const PRPPPreconnectInfoList& preconnect_info_list,
    const std::shared_ptr<PRPPReqInfoTreeNode>& preload_info_tree,
    bool only_send_reuse_request,
    const std::set<std::string>& need_record_header_urls);
  void UpdateIdlePrerequestCount();
 private:
  using InfoIter = std::list<std::shared_ptr<PRRequestInfo>>::iterator;
  using PreconnectInfoListIter = PRPPPreconnectInfoList::iterator;
  void CreateReqLoaderAndStart(const std::shared_ptr<PRRequestInfo>& info);
  void SchedulePreconnects();
  void SchedulePrerequests(uint32_t limit, int32_t info_list_version);
  void ContinueSchedulePreconnects(PreconnectInfoListIter preconnect_infos_iter, int32_t info_list_version);

  const std::string& url_;
  std::list<std::shared_ptr<PRRequestInfo>> prerequest_info_list_;
  std::unordered_map<std::string, int> idle_connect_list_;
  scoped_refptr<base::SingleThreadTaskRunner> sth_task_runner_;
  scoped_refptr<base::SingleThreadTaskRunner> net_task_runner_;
  base::WeakPtr<net::URLRequestContext> url_request_context_;
  bool preload_triggered_ { false };
  int info_list_version_ { 0 };
  int idle_prerequest_count_ { 0 };
  base::WeakPtr<PRPPRequestLoaderFactory> loader_fac_weak_ { nullptr };
  PRPPPreconnectInfoList prpp_preconnect_info_list_;
  std::shared_ptr<PRPPReqInfoTreeNode> preload_info_tree_;
  std::shared_ptr<PRPPReqInfoTreeNode> cur_parent_;
  std::list<std::shared_ptr<PRPPReqInfoTreeNode>>::iterator cur_node_iter_;
  bool only_send_reuse_request_ { false };
  std::set<std::string> need_record_header_urls_;
  PRPPOnPageOriginCB on_page_origin_cb_;
  base::WeakPtrFactory<ResPreloadScheduler> weak_factory_{this};
};

}  // namespace ohos_prp_preload

#endif  // SERVICES_NETWORK_PRP_PRELOAD_SRC_RES_PRELOAD_SCHEDULER_H