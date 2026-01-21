/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "net/dns/context_host_resolver.h"

namespace net {

#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
bool ContextHostResolver::CanUseSecureDnsFallback() const {
  if (!manager_) {
    return false;
  }
  return manager_->CanUseSecureDnsFallback(resolve_context_.get());
}

void ContextHostResolver::GetLocalAddress(IPEndPoint* address) {
  if (!manager_) {
    return;
  }

  manager_->GetLocalAddress(address);
}
#endif

#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK_ON_DNS_HIJACKING)
bool ContextHostResolver::NeedRetryDnsOnDnsHijack(
    const GURL& url,
    const std::string& errorcode) const {
  if (!manager_) {
    return false;
  }
  return manager_->NeedRetryDnsOnDnsHijack(url, errorcode);
}
#endif

}  // namespace net
