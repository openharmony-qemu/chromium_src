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

#ifndef SERVICES_NETWORK_SEC_HEADER_HELPERS__FOR_INCLUDE_H_
#define SERVICES_NETWORK_SEC_HEADER_HELPERS__FOR_INCLUDE_H_

#include "arkweb/build/features/features.h"
#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
#include <map>

#include "url/origin.h"
#endif

namespace network {

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
COMPONENT_EXPORT(NETWORK_SERVICE)
std::map<std::string, std::string> GetFetchMetadataHeaders(
    const GURL& target_url,
    network::mojom::RequestMode mode,
    bool has_user_activation,
    network::mojom::RequestDestination dest,
    const std::optional<url::Origin>& initiator);
#endif

}  // namespace network

#endif  // SERVICES_NETWORK_SEC_HEADER_HELPERS__FOR_INCLUDE_H_
