/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "base/base_switches.h"
#include "base/command_line.h"
#endif

namespace net {

namespace {

#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
constexpr int64_t UNUSED_PRELOAD_SOCKET_TIMEOUT_MICRO_SEC = 5000000;
#endif

}  // namespace

class TransportClientSocketPoolUtils {
 public:
  static void ConnectJobSetConnectTimeoutExt(
      const ClientSocketPool::GroupId& group_id,
      std::unique_ptr<ConnectJob>& connect_job,
      raw_ptr<TransportClientSocketPool> obj) {
#if BUILDFLAG(ARKWEB_EX_NETWORK_CONNECTION)
#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
    if (group_id.secure_dns_only()) {
      connect_job->SetConnectTimeout(
          obj->utils->GetConnectJobWithSecureDnsOnlyTimeout());
    } else {
      connect_job->SetConnectTimeout(obj->utils->GetConnectTimeout());
    }
#else
    connect_job.get()->SetConnectTimeout(obj->utils->GetConnectTimeout());
#endif  // BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
#endif  // BUILDFLAG(ARKWEB_EX_NETWORK_CONNECTION)
  }

#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
  static base::TimeDelta UnusedIdleSocketTimeoutExt(
      std::list<TransportClientSocketPool::IdleSocket>::iterator idle_socket_it,
      raw_ptr<TransportClientSocketPool> obj) {
    base::TimeDelta timeout;
    if (!idle_socket_it->socket->WasEverUsed()) {
      if (idle_socket_it->socket->IsFromPreload()) {
        timeout = base::TimeDelta::FromInternalValue(
            UNUSED_PRELOAD_SOCKET_TIMEOUT_MICRO_SEC);
        LOG(DEBUG) << "PRPpreload set unused preload socket timeout to "
                   << timeout;
      } else {
        timeout = obj->unused_idle_socket_timeout_;
      }
    } else {
      timeout = obj->used_idle_socket_timeout_;
    }
    return timeout;
  }
#endif

  static void BackupJobSetConnectTimeoutExt(
      const ClientSocketPool::GroupId& group_id,
      ConnectJob* backup_job,
      raw_ptr<TransportClientSocketPool::Group> obj) {
#if BUILDFLAG(ARKWEB_EX_NETWORK_CONNECTION)
#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
    if (group_id.secure_dns_only()) {
      backup_job->SetConnectTimeout(
          obj->client_socket_pool_->utils
              ->GetConnectJobWithSecureDnsOnlyTimeout());
    } else {
      backup_job->SetConnectTimeout(
          obj->client_socket_pool_->utils->GetConnectTimeout());
    }
#else
    backup_job->SetConnectTimeout(
        obj->client_socket_pool_->utils->GetConnectTimeout());
#endif
#endif
  }
};

}  // namespace net
