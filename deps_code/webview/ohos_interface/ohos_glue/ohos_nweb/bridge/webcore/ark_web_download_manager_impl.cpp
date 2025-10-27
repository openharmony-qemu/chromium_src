/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ohos_nweb/bridge/ark_web_download_manager_impl.h"

#include "ohos_nweb/bridge/ark_web_doh_config_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebDownloadManagerImpl::ArkWebDownloadManagerImpl(
    std::shared_ptr<OHOS::NWeb::NWebDownloadManager> nweb_download_manager)
    : nweb_download_manager_(nweb_download_manager)
{}

void ArkWebDownloadManagerImpl::SetHttpDns(ArkWebRefPtr<ArkWebDohConfig> config)
{
    if (CHECK_REF_PTR_IS_NULL(config)) {
        nweb_download_manager_->SetHttpDns(nullptr);
        return;
    }

    nweb_download_manager_->SetHttpDns(std::make_shared<ArkWebDohConfigWrapper>(config));
}

void ArkWebDownloadManagerImpl::SetConnectionTimeout(const int& timeout)
{
    nweb_download_manager_->SetConnectionTimeout(timeout);
}

} // namespace OHOS::ArkWeb
