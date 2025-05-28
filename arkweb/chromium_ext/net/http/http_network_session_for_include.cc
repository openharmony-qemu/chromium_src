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

namespace net {

#if BUILDFLAG(ARKWEB_EX_NETWORK_CONNECTION)
void HttpNetworkSession::SetConnectTimeout(int seconds) {
  normal_socket_pool_manager_->SetConnectTimeout(seconds);
  websocket_socket_pool_manager_->SetConnectTimeout(seconds);
}
#endif

#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
void HttpNetworkSession::SetConnectJobWithSecureDnsOnlyTimeout(int seconds) {
  normal_socket_pool_manager_->SetConnectJobWithSecureDnsOnlyTimeout(seconds);
}
#endif

}  // namespace net
