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

namespace net {

#if BUILDFLAG(ARKWEB_EXT_NETWORK_CONNECTION)
void ClientSocketPoolManagerImpl::SetConnectTimeout(int seconds) {
  timeout_override_ = seconds;
  for (const auto& it : socket_pools_) {
    it.second->utils->SetConnectTimeout(seconds);
  }
}
#endif

#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
void ClientSocketPoolManagerImpl::SetConnectJobWithSecureDnsOnlyTimeout(
    int seconds) {
  connect_job_with_secure_dns_timeout_ = seconds;
  for (const auto& it : socket_pools_) {
    it.second->utils->SetConnectJobWithSecureDnsOnlyTimeout(seconds);
  }
}
#endif

}  // namespace net
