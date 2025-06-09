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

#ifndef NET_PRP_PRELOAD_SRC_RES_REQUEST_INFO_CACHE_MGR_H
#define NET_PRP_PRELOAD_SRC_RES_REQUEST_INFO_CACHE_MGR_H

#include <cstdint>
#include <memory>
#include <list>
#include "base/task/thread_pool.h"
#include "base/types/optional_util.h"
#include "base/json/json_writer.h"
#include "base/json/json_reader.h"
#include "base/values.h"
#include "net/base/network_anonymization_key.h"
#include "disk_cache_file.h"
#include "page_res_request_info.h"

namespace ohos_prp_preload {
using ResRequestInfoCacheLoadedCB =
    base::RepeatingCallback<void(const std::list<std::shared_ptr<PRRequestInfo>>& load_info_list,
                                 const net::NetworkAnonymizationKey& network_anonymization_key)>;
class ResReqInfoCacheMgr : public base::RefCounted<ResReqInfoCacheMgr> {
 public:
  ResReqInfoCacheMgr(const std::string& url,
                     const net::NetworkAnonymizationKey& network_anonymization_key,
                     const scoped_refptr<base::SingleThreadTaskRunner>& sth_task_runner,
                     const scoped_refptr<DiskCacheBackendFactory>& disk_cache_backend_factory,
                     const ResRequestInfoCacheLoadedCB& info_list_cb);
  ~ResReqInfoCacheMgr() = default;

  void Start();
  void UpdateResRequestInfo(const std::shared_ptr<PRRequestInfo>& info);
  void Stop();
 private:
  void OnEntryLoadedCallback(const std::string& entry_content);
  void CheckFlush();

  const std::string& url_;
  const net::NetworkAnonymizationKey network_anonymization_key_;
  scoped_refptr<base::SingleThreadTaskRunner> sth_task_runner_;
  scoped_refptr<DiskCacheFile> disk_cache_;
  std::list<std::shared_ptr<PRRequestInfo>> new_info_list_;
  std::list<std::shared_ptr<PRRequestInfo>> load_info_list_;
  size_t last_flush_len_{0};
  ResRequestInfoCacheLoadedCB info_cache_loaded_cb_;
  bool is_start_{false};
  base::WeakPtrFactory<ResReqInfoCacheMgr> weak_factory_{this};
};

}  // namespace ohos_prp_preload

#endif  // NET_PRP_PRELOAD_SRC_RES_REQUEST_INFO_CACHE_MGR_H
