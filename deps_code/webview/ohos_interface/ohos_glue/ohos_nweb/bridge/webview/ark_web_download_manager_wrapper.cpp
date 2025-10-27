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

#include "ohos_nweb/bridge/ark_web_download_manager_wrapper.h"

#include "ohos_nweb/bridge/ark_web_doh_config_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebDownloadManagerWrapper::ArkWebDownloadManagerWrapper(ArkWebRefPtr<ArkWebDownloadManager> ark_web_download_manager)
    : ark_web_download_manager_(ark_web_download_manager)
{}

void ArkWebDownloadManagerWrapper::SetHttpDns(std::shared_ptr<OHOS::NWeb::NWebDOHConfig> config)
{
    if (CHECK_SHARED_PTR_IS_NULL(config)) {
        ark_web_download_manager_->SetHttpDns(nullptr);
        return;
    }

    ark_web_download_manager_->SetHttpDns(new ArkWebDohConfigImpl(config));
}

void ArkWebDownloadManagerWrapper::SetConnectionTimeout(const int& timeout)
{
    ark_web_download_manager_->SetConnectionTimeout(timeout);
}

} // namespace OHOS::ArkWeb
