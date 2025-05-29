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

#ifndef SERVICES_NETWORK_URL_LOADER_EXT_H_
#define SERVICES_NETWORK_URL_LOADER_EXT_H_

#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "net/url_request/url_request.h"

namespace network {

#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
// "4" means blink::mojom::ResourceType::kImage. here not depend blink.
constexpr int RESOURCE_TYPE_IMAGE = 4;
#endif

#if BUILDFLAG(ARKWEB_EXT_LOG_MESSAGE)
std::string BoolToString(bool value);

std::string GetProtocol(const GURL& url, const net::HttpResponseInfo& info);

void ReportUrlQuicInfo(net::URLRequest* url_request, int error_code);
#endif
}  // namespace network
#endif
