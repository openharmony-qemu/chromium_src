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

#ifndef SERVICES_NETWORK_URL_LOADER_FACTORY_UTILS_H_
#define SERVICES_NETWORK_URL_LOADER_FACTORY_UTILS_H_

#include "base/memory/scoped_refptr.h"

#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
#include "arkweb/chromium_ext/services/network/prp_preload/include/preload_runner/prpp_request_loader_factory.h"
#endif

namespace network {
class URLLoaderFactory;

class URLLoaderFactoryUtils {
public:
    URLLoaderFactory* urlLoaderFactory;
    URLLoaderFactoryUtils(URLLoaderFactory* factory);

#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
    void PreloadRequestInit(const ResourceRequest& resource_request,
        std::shared_ptr<ohos_prp_preload::PRPPRequestLoader>& prpp_loader,
        std::shared_ptr<ohos_prp_preload::PRRequestInfo>& preload_info);

    base::WeakPtr<ohos_prp_preload::PRPPRequestLoaderFactory> weak_prpp_req_loader_fac_;
#endif
};

}  // namespace network

#endif  // SERVICES_NETWORK_URL_LOADER_FACTORY_UTILS_H_
