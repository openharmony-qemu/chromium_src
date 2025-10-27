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

#include "arkweb/chromium_ext/services/network/url_loader_factory_utils.h"
#include "services/network/url_loader_factory.h"
#include "services/network/public/cpp/resource_request.h"

#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
#include "arkweb/chromium_ext/services/network/prp_preload/include/page_res_parallel_preload_mgr.h"
#include "url_loader_factory_utils.h"
#endif

namespace network {
    URLLoaderFactoryUtils::URLLoaderFactoryUtils(URLLoaderFactory* factory)
    {
        this->urlLoaderFactory = factory;
    }

#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
    void URLLoaderFactoryUtils::PreloadRequestInit(const ResourceRequest& resource_request,
        std::shared_ptr<ohos_prp_preload::PRPPRequestLoader>& prpp_loader,
        std::shared_ptr<ohos_prp_preload::PRRequestInfo>& preload_info)
    {
        prpp_loader = nullptr;
        preload_info = std::make_shared<ohos_prp_preload::PRRequestInfo>();
        preload_info->set_preload_flag(ohos_prp_preload::PRPP_FLAGS_NONE);
        if (!resource_request.main_url.spec().empty() &&
            ohos_prp_preload::PRParallelPreloadMgr::GetInstance()
                .GetPRParallelPreloadMode() ==
                ohos_prp_preload::PRPPreloadMode::PRELOAD) {
            if (!weak_prpp_req_loader_fac_.get()) {
            weak_prpp_req_loader_fac_ =
                ohos_prp_preload::PRParallelPreloadMgr::GetInstance()
                .GetRequestLoaderFactory(resource_request.main_url.spec());
            }

            if (weak_prpp_req_loader_fac_.get()) {
            prpp_loader = weak_prpp_req_loader_fac_.get()->GetPRPPReqLoader(
                *(this->urlLoaderFactory), resource_request, preload_info);
            }
        }
    }
#endif
}